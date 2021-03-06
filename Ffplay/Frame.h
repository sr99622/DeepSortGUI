#pragma once

#include <iostream>

extern "C" {
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/pixdesc.h"
}
#include "opencv2/opencv.hpp"
#include <npp.h>

#include <QObject>
#include <QRect>
#include <QMargins>

#include "Utilities/yuvcolor.h"
#include "Utilities/cudaexception.h"

class Frame : public QObject
{
    Q_OBJECT

public:
    Frame();
    Frame(int width, int height, const AVPixelFormat& pix_fmt);
    ~Frame();
    void paintItBlack();
    void grayscale();
    bool writable();
    void fillPixel(int x, int y, const YUVColor &color);
    void drawBox(const QRect &rect, int line_width, const YUVColor &color);
    void slice(int x, int y, Frame *sub_vp);
    void pip(int ulc_x, int ulc_y, Frame *sub_vp);
    void allocateFrame(int width, int height, const AVPixelFormat& pix_fmt);
    void copy(Frame *vp);

    cv::Mat toMat();
    void readMat(const cv::Mat& mat);
    cv::Mat hwToMat();
    void hwReadMat(const cv::Mat& mat);

    Npp8u *pYUV[3] = {nullptr, nullptr, nullptr};
    Npp8u *pBGR = nullptr;

    AVFrame* frame = nullptr;
    cv::Mat mat;
	AVSubtitle sub;
	int serial;
	double pts;
	double duration;
	int64_t pos;
    int width = 0;
    int height = 0;
	int format;
	AVRational sar;
	int uploaded;
	int flip_v;

    CudaExceptionHandler eh;
};

