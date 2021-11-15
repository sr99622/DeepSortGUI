#ifndef COUNTERFRAME_H
#define COUNTERFRAME_H

#include <opencv2/opencv.hpp>
#include "yolo_v2_class.hpp"


class CounterFrame
{

public:
    CounterFrame();
    ~CounterFrame();
    void clear();

    cv::Mat image;
    std::vector<bbox_t> detections;

};


#endif // COUNTERFRAME_H
