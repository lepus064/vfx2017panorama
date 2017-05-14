# vfx2017panorama
website:https://r05521501.wixsite.com/vfx2017spring
## Project1: High Dynamic Range Imaging

### 使用相機:

Panasonic G7 + Panasonic LUMIX 14-42mm F3.5-5.6 ASPH


### 使用工具:

OpenCV 3.2.0 :http://opencv.org

OpenMP(Optional):http://www.openmp.org 使用多執行緒計算特徵


### 使用方法:
```
git clone https://github.com/lepus064/vfx2017spring.git

cd src && mkdir build && cd build

cmake ..

./image_stitching ../../test_data ../../test_data/pano.txt

```

如果無法compile可將CMakeLists.txt中的hdr_imaging_multi_thread.cpp

改成hdr_imaging.cpp


#### OpenMP:

把openmp資料夾裡的檔案替換src裡的檔案

在執行上述步驟
