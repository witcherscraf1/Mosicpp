#include "mosaic.h"

int main(int argc, char** argv) {
    std::string targetImagePath = "target.jpg";
    std::string tileFolder = "tiles/";
    std::string outputImagePath = "output.jpg";
    int gridSize = 30; // tile 尺寸
    int jpegQuality = 95; // 输出 JPEG 质量
    float scale = 10.0f; // 放大输出图像尺寸

    try {
        MosaicMaker maker(targetImagePath, tileFolder, gridSize, scale);
        maker.buildMosaic(outputImagePath, jpegQuality);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
