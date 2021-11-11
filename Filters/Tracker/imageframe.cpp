#include "imageframe.h"

ImageFrame::ImageFrame()
{

}

void ImageFrame::writeToDetections(DETECTIONS* arg)
{
    for (size_t i = 0; i < detections.size(); i++) {
        DETECTION_ROW tmpRow;
        float x = detections[i].x;
        float y = detections[i].y;
        float w = detections[i].w;
        float h = detections[i].h;
        tmpRow.tlwh = DETECTBOX(x, y, w, h);
        tmpRow.confidence = detections[i].confidence;
        for (int j = 0; j < feature_size; j++)
            tmpRow.feature[j] = features[i][j];
        arg->push_back(tmpRow);
    }
}

void ImageFrame::getCrops()
{
    for (size_t i = 0; i < detections.size(); i++)
        crops.push_back(getCrop(image, &detections[i], cv::Size(crop_width, crop_height)));
}

cv::Mat ImageFrame::getCrop(const cv::Mat& image, fbox* box, const cv::Size& size) const
{
    cv::Mat crop;
    float target_aspect = size.width / (float)size.height;
    float new_width = target_aspect * box->h;
    box->x -= (new_width - box->w) / 2;
    box->w = new_width;

    box->x = std::max(box->x, 0.0f);
    box->y = std::max(box->y, 0.0f);
    box->w = std::min(box->w, image.cols - 1 - box->x);
    box->h = std::min(box->h, image.rows - 1 - box->y);

    //std::cout << "x1: " << box->x << " y1: " << box->y << " x2: " << box->x + box->w << " y2: " << box->y + box->h << std::endl;

    crop = image(cv::Range(box->y, box->y + box->h), cv::Range(box->x, box->x + box->w));
    cv::resize(crop, crop, size);

    return crop;
}

void ImageFrame::clear()
{
    for (size_t i = 0; i < features.size(); i++)
        features[i].clear();
    features.clear();
    crops.clear();
    detections.clear();
}

ImageFrame::~ImageFrame()
{
    clear();
}

fbox::fbox(const bbox_t& bbox)
{
    x = (float)bbox.x;
    y = (float)bbox.y;
    w = (float)bbox.w;
    h = (float)bbox.h;
    confidence = bbox.prob;
}

float fbox::x2()
{
    return x + w;
}

float fbox::y2()
{
    return y + h;
}

bbox_t fbox::to_bbox() const
{
    bbox_t result;
    result.x = (int)x;
    result.y = (int)y;
    result.w = (int)w;
    result.h = (int)h;
    result.prob = confidence;
    return result;
}
