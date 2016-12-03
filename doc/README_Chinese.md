## SeetaFaceLib - 人脸检索系统

[![License](https://img.shields.io/badge/license-BSD-blue.svg)](../LICENSE)

### 简介

SeetaFaceLib是一个以[SeetaFaceEngine](https://github.com/seetaface/SeetaFaceEngine)为基础建立起来的人脸检索系统，目前QT工程中GUI的界面设计界面如下：

<p align="center"><img src="http://i300.photobucket.com/albums/nn17/willard-yuan/demo_zpsvuygqatm.png" alt="CocsoFace"/></p>


### 安装

SeetaFaceLib依赖[SeetaFaceEngine](https://github.com/seetaface/SeetaFaceEngine)、[Falconn](https://github.com/FALCONN-LIB/FALCONN)、OpenCV以及Boost，其中SeetaFaceEngine以及Falconn已经包含进来的，所以只需要安装OpenCV和Boost了。注意对于非Mac用户，`FaceLib/FaceIdentification/lib/libviplnet.4.5.dylib`中的`libviplnet.4.5.dylib`需要单独进入`FaceLib/FaceIdentification/`中进行编译，编译命令如下：

```sh
mkdir build
cd build
cmake .. && make
```


### License

SeetaFace Detection is released under the [BSD 2-Clause license](../LICENSE).