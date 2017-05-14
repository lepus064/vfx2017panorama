# vfx2017panorama
website:https://r05521501.wixsite.com/vfx2017spring

## Project2: Image stitching


### 使用相機:

Panasonic G7 + Panasonic LUMIX 14-42mm F3.5-5.6 ASPH



### 使用工具:

OpenCV 3.2.0 :http://opencv.org

OpenMP(Optional):http://www.openmp.org 



### 系統:

MacOS 10.12.4

Ubuntu 16.04



### 使用方法:
```
git clone https://github.com/lepus064/vfx2017panorama.git

cd src && mkdir build && cd build

cmake ..

./image_stitching ../../test_data ../../test_data/pano.txt

```



#### OpenMP: （只在Ubuntu 16.04上測試過）

把openmp資料夾裡的檔案替換src裡的檔案

在執行上述步驟

使用多執行緒會比原本快上非常多
