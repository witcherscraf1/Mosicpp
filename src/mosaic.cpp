#include "mosaic.h"
#include <cmath>
#include <thread>
#include <numeric>

namespace fs = std::filesystem;

MosaicMaker::MosaicMaker(const std::string& targetPath, const std::string& tileDir, int gridSize, float scale)
    : tileSize(gridSize)
{
    targetImage = cv::imread(targetPath);
    if (targetImage.empty()) {
        throw std::runtime_error("Failed to load target image.");
    }
    if (scale != 1.0f) {
        cv::resize(targetImage, targetImage, {}, scale, scale);
    }
    loadTileImages(tileDir);
}
void MosaicMaker::loadTileImages(const std::string& folder) {
    std::vector<std::future<void>> futures;
    std::vector<cv::Mat> features;
    
    if (!fs::exists(folder)) {
        throw std::runtime_error("Tile directory does not exist: " + folder);
    }

    for (const auto& entry : fs::directory_iterator(folder)) {
        futures.push_back(std::async(std::launch::async, [this, entry, &features]() {
            cv::Mat img = cv::imread(entry.path().string());
            if (!img.empty()) {
                cv::resize(img, img, {tileSize, tileSize});
                cv::Mat lab;
                cv::cvtColor(img, lab, cv::COLOR_BGR2Lab);
                cv::Mat avg = computeLABAverage(lab);
                
                if (avg.type() != CV_32F) {
                    avg.convertTo(avg, CV_32F);
                }

                std::lock_guard<std::mutex> lock(mutex);
                tileImages.push_back(img);
                tileLABs.push_back(avg);
                features.push_back(avg);
            }
        }));
    }

    for (auto& f : futures) f.get();

    if (features.empty()) {
        throw std::runtime_error("No valid images loaded from tile directory");
    }

    tileFeatures = cv::Mat(static_cast<int>(features.size()), 3, CV_32F);
    for (int i = 0; i < features.size(); ++i) {
        features[i].copyTo(tileFeatures.row(i));
    }

    if (tileFeatures.empty() || tileFeatures.type() != CV_32F) {
        throw std::runtime_error("Invalid feature matrix");
    }

    try {
        // 初始化和构建KD树
        kdtree = cv::flann::Index();
        kdtree.build(tileFeatures, cv::flann::KDTreeIndexParams(4), cvflann::FLANN_DIST_L2);
    } catch (const cv::Exception& e) {
        throw std::runtime_error("Failed to build KD-tree: " + std::string(e.what()));
    }
}

cv::Mat MosaicMaker::computeLABAverage(const cv::Mat& labImg) {
    cv::Scalar mean = cv::mean(labImg);
    return (cv::Mat_<float>(1, 3) << mean[0], mean[1], mean[2]);
}

double MosaicMaker::weightedLABDistance(const cv::Mat& a, const cv::Mat& b) {
    double l = a.at<float>(0, 0) - b.at<float>(0, 0);
    double a_ = a.at<float>(0, 1) - b.at<float>(0, 1);
    double b_ = a.at<float>(0, 2) - b.at<float>(0, 2);
    return std::sqrt(l * l * 1.0 + a_ * a_ * 1.5 + b_ * b_ * 2.0);
}

cv::Mat MosaicMaker::getBestMatchTile(const cv::Mat& patch) {
    if (patch.empty()) {
        throw std::runtime_error("Empty patch provided");
    }

    cv::Mat labPatch;
    cv::cvtColor(patch, labPatch, cv::COLOR_BGR2Lab);
    cv::Mat avg = computeLABAverage(labPatch);
    
    if (avg.type() != CV_32F) {
        avg.convertTo(avg, CV_32F);
    }

    std::vector<int> indices(1);
    std::vector<float> dists(1);
    
    try {
        std::lock_guard<std::mutex> lock(mutex);
        kdtree.knnSearch(avg, indices, dists, 1, cv::flann::SearchParams(32));
        
        if (indices.empty() || indices[0] >= tileImages.size()) {
            throw std::runtime_error("Invalid KNN search result");
        }
        
        return tileImages[indices[0]].clone();  // 返回副本避免并发访问问题
    } catch (const cv::Exception& e) {
        throw std::runtime_error("KNN search failed: " + std::string(e.what()));
    }
}

void MosaicMaker::buildMosaic(const std::string& outputPath, int jpegQuality) {
    int h = targetImage.rows;
    int w = targetImage.cols;

    cv::Mat output(h, w, targetImage.type());
    std::vector<std::future<void>> futures;
    
    const int maxThreads = std::min(8, (int)std::thread::hardware_concurrency());
    std::atomic<int> activeThreads(0);
    
    for (int y = 0; y < h; y += tileSize) {
        for (int x = 0; x < w; x += tileSize) {
            while (activeThreads >= maxThreads) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            
            int roiWidth = std::min(tileSize, w - x);
            int roiHeight = std::min(tileSize, h - y);
            cv::Rect roi(x, y, roiWidth, roiHeight);
            
            futures.push_back(std::async(std::launch::async, [this, &output, roi, &activeThreads]() {
                ++activeThreads;
                try {
                    cv::Mat patch = targetImage(roi);
                    cv::Mat tile = getBestMatchTile(patch);
                    std::lock_guard<std::mutex> lock(mutex);
                    cv::resize(tile, tile, roi.size());
                    tile.copyTo(output(roi));
                } catch (const std::exception& e) {
                    std::cerr << "Error processing tile: " << e.what() << std::endl;
                }
                --activeThreads;
            }));
        }
    }
    
    for (auto& f : futures) f.get();

    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, jpegQuality};
    if (!cv::imwrite(outputPath, output, params)) {
        throw std::runtime_error("Failed to write output image");
    }
}