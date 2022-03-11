/******************************************************************************
  File Name     : QRDetector.cpp
  Version       : Initial
  Author        : TD1500
  Created       : 2020
  Description   : QRDetector module source code
******************************************************************************/
#include "nnie_forward.h"
#include "AnchorGenerator.h"
#include "opencv2/opencv.hpp"
#include "Yolov3_layer.h"
#include "Yolov3.h"
#include "Value_Pos.h"

#include <dirent.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <functional>
#include <sys/time.h>

using namespace std;
using namespace cv;

#define MAX_DETECT_NUM 30
#define UTILS_MIN(a,b)  (((a) < (b)) ? (a) : (b))

int main(int argc, char** argv) {
    bool ret;
    char* modelPath = argv[1];

    printf("Load NNIE Model ...\n");
    YOLOV3 *yolov3 = new YOLOV3(modelPath);
    if (yolov3 == NULL) {
        printf("new a QR Detect failed !\n");
        return -1;
    }

    ret = yolov3->Init();
    if (ret == false) {
        printf("QR Init failed !\n");
        return -1;
    }



    string testDir = "/home/linaro/testModel/pigTestImg/";
    string resultDir = "/home/linaro/testModel/pigPreLabels/";
    DIR* dir = opendir(testDir.c_str());
    struct dirent* filename;
    
    int count = 0;
    while((filename = readdir(dir))) {
        if(strcmp( filename->d_name , "." ) == 0 || strcmp( filename->d_name , "..") == 0)
            continue;

        string fileName = filename->d_name;
        string filePath = testDir + fileName;
        printf("Begin process File: %s\n", filePath.c_str());
        count += 1;

        Mat img = imread(filePath.c_str());
        int imgWidth = img.cols;
        int imgHeight = img.rows;
        printf("Img width: %d, Height: %d\n", imgWidth, imgHeight);

        Mat resizeImg;
        cv::resize(img, resizeImg, cv::Size(512, 384));
        printf("Resize Img to %d*%d\n", resizeImg.cols, resizeImg.rows);

        // 用于根据检测框置信度排序
        vector<Anchor> detectResultVecTmp;
        vector<Value_Pos<float>> scoreVec;

        
        yolov3->Forward(resizeImg);
       
        
        detectResultVecTmp.clear();
        scoreVec.clear();

        for(int i = 0, pos = 0; i < yolov3->result.size(); i++) {
            detectResultVecTmp.push_back(yolov3->result[i]);
            scoreVec.push_back(Value_Pos<float>(yolov3->result[i].score, pos++));
        }

        if(count % 10 == 0) {
            printf("Process %dth image!\n", count);
        }
        


        // 如果二维码框大于30张，则按检测的置信度进行排序，取置信度最高的30个
        if (detectResultVecTmp.size() >= MAX_DETECT_NUM) {
            sort(scoreVec.begin(), scoreVec.end(), cmpLarge2Small<float>);
        }
        int detect_num = UTILS_MIN(MAX_DETECT_NUM, scoreVec.size());
        int boxWidth, boxHeight;

        int fileNameLength = fileName.length();
        string outFile = resultDir + fileName.substr(0, fileNameLength-4) + ".txt";

        FILE* fpt = fopen(outFile.c_str(), "w");
        for(int i = 0; i < detect_num; i++) {
            int p = scoreVec[i].pos;

            int classId = detectResultVecTmp[p].classIndex;
            float boxX = (detectResultVecTmp[p].box.x + detectResultVecTmp[p].box.width * 0.5) * 1.0 / imgWidth;
            float boxY = (detectResultVecTmp[p].box.y + detectResultVecTmp[p].box.height * 0.5) * 1.0 / imgHeight;
            float boxWidth = detectResultVecTmp[p].box.width * 1.0  / imgWidth;
            float boxHeight = detectResultVecTmp[p].box.height * 1.0 / imgHeight;

            fprintf(fpt, "%d %.6f %.6f %.6f %.6f\n", classId, boxX, boxY, boxWidth, boxHeight);
        }
        fclose(fpt);
        detectResultVecTmp.clear();
        scoreVec.clear();

    }

    return 0;
}