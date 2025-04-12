/*
README.md 内容：

# MosaicCpp

🎨 一个用 C++ 和 OpenCV 实现的马赛克图像生成器，支持高质量、缩放、自定义色块等特性。

## 功能特色

- 使用给定图块（tiles）生成马赛克图像
- 自动匹配最接近颜色的 tile
- 支持自定义 tile 大小
- 支持输出图像放大（scale 参数）
- 支持调整 JPEG 输出质量
- 命令行参数友好支持：`--tile-dir=...` 等

## 使用方法

编译（需要安装 OpenCV）：

```bash
mkdir build && cd build
cmake ..
make
```

运行示例：

```bash
./MosaicCpp --target=images/input.jpg --tile-dir=tiles/ --output=output.jpg --tile-size=40 --scale=4.0 --jpeg-quality=95
```

### 参数说明
| 参数名         | 示例值                   | 说明                                   |
|----------------|--------------------------|----------------------------------------|
| `--target`     | `images/input.jpg`       | 输入图像路径（要生成马赛克的图）      |
| `--tile-dir`   | `tiles/`                 | 图块图像文件夹路径                     |
| `--output`     | `output.jpg`             | 输出图像保存路径                       |
| `--tile-size`  | `40`                     | 每个 tile 的边长（像素）              |
| `--scale`      | `3.0`                    | 输出图像尺寸相对于输入图的放大倍数    |
| `--jpeg-quality` | `95`                   | 输出 JPEG 图像质量（1-100）           |

## 依赖

- C++17 或更高
- OpenCV >= 4.5
- CMake >= 3.10

## 示例输出

📷 原始图像（左） vs 马赛克图像（右）

> 可以附图或链接

## 许可

MIT License.
*/
