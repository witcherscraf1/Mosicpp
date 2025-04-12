#pragma once
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <filesystem>
#include <future>
#include <opencv2/flann/flann.hpp>

class MosaicMaker {
public:
    MosaicMaker(const std::string& targetPath, const std::string& tileDir, int gridSize, float scale);
    void buildMosaic(const std::string& outputPath, int jpegQuality = 95);

private:
    cv::Mat targetImage;
    std::vector<cv::Mat> tileImages;
    std::vector<cv::Mat> tileLABs;
    cv::Mat tileFeatures; // 用于 KDTree
    cv::flann::Index kdtree;
    int tileSize;
    std::mutex mutex;

    void loadTileImages(const std::string& folder);
    cv::Mat computeLABAverage(const cv::Mat& lab);
    cv::Mat getBestMatchTile(const cv::Mat& patch);
    double weightedLABDistance(const cv::Mat& a, const cv::Mat& b);
};