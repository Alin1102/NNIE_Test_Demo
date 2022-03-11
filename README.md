1 需要使用交叉编译工具编译该项目

2 需要修改 main.cpp 文件中的以下几个变量：  
    string testDir = "./testImgs/";

    string resultDir = "./resultLabels/";

    int forwardImgWidth = 512;

    int forwardImgHeight = 384;

    该程序会依次读取目录 testDir 下的每一张图片，并将图片 resize 成(forwardImgWidth, forwardImgHeight)后传入NNIE进行前向推理。

    程序最终会将每一张图的检测框保存在路径 resultDir 下。

3 需要根据实际使用的网络结构，修改 

    (1) Yolov3.cpp 文件中 YOLOV3类构造函数中：classNum变量的值
    
    (2) Yolov3_layer.cpp 文件中 Yolov3_layer构造函数中的 Anchors为训练时使用的