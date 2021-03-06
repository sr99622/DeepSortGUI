#ifndef IMAGEFRAME_H
#define IMAGEFRAME_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <QRect>
#include "yolo_v2_class.hpp"
#include "DeepSort/feature/model.h"

#define num_channels 3
#define feature_size 128
#define crop_width 64
#define crop_height 128

class fbox {

public:
    fbox(const bbox_t& bbox);
    fbox(const DETECTBOX& box);
    fbox(const QRect& rect);
    fbox(float x, float y, float w, float h);
    bbox_t to_bbox() const;

    float x2();
    float y2();
    float x, y, w, h, confidence;
};

class ImageFrame
{

public:
    ImageFrame();
    ~ImageFrame();
    void clear();
    void getCrops();
    cv::Mat getCrop(fbox* box, const cv::Size& size) const;
    cv::Mat getFocusCrop(fbox* box) const;
    void writeToDetections(DETECTIONS *detections);
    void validateBox(fbox* box, const cv::Size& size);

    cv::Mat image;

    std::vector<fbox> detections;
    std::vector<cv::Mat> crops;
    std::vector<std::vector<float>> features;
    std::vector<RESULT_DATA> result;

};

#endif // IMAGEFRAME_H
