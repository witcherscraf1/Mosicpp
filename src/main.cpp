#include "mosaic.h"
#include <iostream>
#include <map>

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " --target=<target.jpg> --tile-dir=<tiles_folder> --output=<output.jpg> [--tile-size=30] [--scale=3.0] [--jpeg-quality=95]\n";
}

std::map<std::string, std::string> parseArgs(int argc, char** argv) {
    std::map<std::string, std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        size_t eq = arg.find('=');
        if (arg.rfind("--", 0) == 0 && eq != std::string::npos) {
            std::string key = arg.substr(2, eq - 2);
            std::string val = arg.substr(eq + 1);
            args[key] = val;
        }
    }
    return args;
}

int main(int argc, char** argv) {
    auto args = parseArgs(argc, argv);

    if (args.count("target") == 0 || args.count("tile-dir") == 0 || args.count("output") == 0) {
        printUsage(argv[0]);
        return 1;
    }

    std::string targetImagePath = args["target"];
    std::string tileFolder = args["tile-dir"];
    std::string outputImagePath = args["output"];

    int gridSize = args.count("tile-size") ? std::stoi(args["tile-size"]) : 30;
    float scale = args.count("scale") ? std::stof(args["scale"]) : 3.0f;
    int jpegQuality = args.count("jpeg-quality") ? std::stoi(args["jpeg-quality"]) : 95;

    try {
        MosaicMaker maker(targetImagePath, tileFolder, gridSize, scale);
        maker.buildMosaic(outputImagePath, jpegQuality);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}


// #include "mosaic.h"
// #include <crow.h>
// #include <filesystem>
// #include <fstream>

// namespace fs = std::filesystem;

// // 辅助函数
// bool endsWith(const std::string& str, const std::string& suffix) {
//     if (str.length() < suffix.length()) {
//         return false;
//     }
//     return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
// }

// int main() {
//     crow::SimpleApp app;
    
//     // 主页路由
//     CROW_ROUTE(app, "/")([]() {
//         std::ifstream file("templates/index.html");
//         if (!file) {
//             return crow::response(404);
//         }
        
//         std::string content((std::istreambuf_iterator<char>(file)),
//                            std::istreambuf_iterator<char>());
//         crow::response res(content);
//         res.set_header("Content-Type", "text/html");
//         return res;
//     });

//     // 静态文件路由
//     CROW_ROUTE(app, "/static/<string>")(
//         [](const std::string& path) {
//             std::string filePath = "static/" + path;
//             if (!fs::exists(filePath)) {
//                 return crow::response(404);
//             }
            
//             std::ifstream file(filePath, std::ios::binary);
//             if (!file) {
//                 return crow::response(404);
//             }
            
//             std::string content((std::istreambuf_iterator<char>(file)),
//                                std::istreambuf_iterator<char>());
                               
//             crow::response res(content);
//             if (endsWith(path, ".css")) {
//                 res.set_header("Content-Type", "text/css");
//             } else if (endsWith(path, ".js")) {
//                 res.set_header("Content-Type", "application/javascript");
//             } else if (endsWith(path, ".jpg") || endsWith(path, ".jpeg")) {
//                 res.set_header("Content-Type", "image/jpeg");
//             } else if (endsWith(path, ".png")) {
//                 res.set_header("Content-Type", "image/png");
//             }
//             return res;
//         }
//     );

//     // 设置端口并启动服务器
//     uint16_t port = 18080;
//     if (const char* env_port = std::getenv("PORT")) {
//         port = static_cast<uint16_t>(std::atoi(env_port));
//     }
    
//     std::cout << "Server starting on port " << port << std::endl;
//     app.port(port).run();  // 移除 multithreaded()，看看是否解决问题
    
//     return 0;
// }