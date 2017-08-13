## SeetaFaceLib - 人脸检索系统

[![License](https://img.shields.io/badge/license-BSD-blue.svg)](../LICENSE)

### 简介

SeetaFaceLib是一个以[SeetaFaceEngine](https://github.com/seetaface/SeetaFaceEngine)为基础建立起来的人脸检索系统，目前QT工程中GUI的界面设计界面如下：

<p align="center"><img src="http://ose5hybez.bkt.clouddn.com/github/faceRetrieval.jpg" alt="CocsoFace"/></p>

### 目录结构

`CocsoFace`: 该目录下是QT工程
`FaceLib`: SeetaFaceEnine的三个部件文件
`XcodePro`: Xcode工程，便于开发(个人开发的思路是这样的，先在Xcode里面完成功能，然后将开发的功能添加到QT工程里面)

### 安装

SeetaFaceLib依赖[SeetaFaceEngine](https://github.com/seetaface/SeetaFaceEngine)、[Falconn](https://github.com/FALCONN-LIB/FALCONN)、OpenCV以及Boost，其中SeetaFaceEngine以及Falconn已经包含进来的，所以只需要安装OpenCV和Boost了。注意对于非Mac用户，`FaceLib/FaceIdentification/lib/libviplnet.4.5.dylib`中的`libviplnet.4.5.dylib`需要单独进入`FaceLib/FaceIdentification/`中进行编译，编译命令如下：

```sh
mkdir build
cd build
cmake .. && make
```

### License

SeetaFace Detection is released under the [BSD 2-Clause license](../LICENSE

### Troubleshooting

1. 编译后在执行时，如果出现`Using on OS X: 'Symbol not found: __cg_jpeg_resync_to_restart'`，执行：

```sh
cd /usr/local/lib
mv libjpeg.dylib libjpeg.dylib.backup
ln -s /System/Library/Frameworks/ImageIO.framework/Resources/libJPEG.dylib libJPEG.dylib
mv libtiff.dylib libtiff.dylib.backup
ln -s /System/Library/Frameworks/ImageIO.framework/Resources/libTIFF.dylib libTIFF.dylib
mv libpng.dylib libpng.dylib.backup
ln -s /System/Library/Frameworks/ImageIO.framework/Resources/libPng.dylib libPNG.dylib
```