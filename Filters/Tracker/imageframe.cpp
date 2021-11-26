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

cv::Mat ImageFrame::getFocusCrop(fbox* box) const
{
    box->x = std::min((float)(image.cols - 1), std::max(box->x, 0.0f));
    box->y = std::min((float)(image.rows - 1), std::max(box->y, 0.0f));
    box->w = std::max(0.0f, std::min(box->w, image.cols - 1 - box->x));
    box->h = std::max(0.0f, std::min(box->h, image.rows - 1 - box->y));

    cv::Mat crop;
    try {
        crop = image(cv::Range(box->y, box->y + box->h), cv::Range(box->x, box->x + box->w));
    }
    catch (const std::exception& e) {
        std::cout << "ImageFrame::getCrop exception: " << e.what() << std::endl;
        std::cout << "x1: " << box->x << " y1: " << box->y << " x2: " << box->x + box->w << " y2: " << box->y + box->h << std::endl;
        crop = cv::Mat(box->h, box->w, CV_8UC3, cv::Scalar(64, 64, 64));
    }
    return crop;
}

void ImageFrame::getCrops()
{
    for (size_t i = 0; i < detections.size(); i++)
        crops.push_back(getCrop(&detections[i], cv::Size(crop_width, crop_height)));
}

void ImageFrame::validateBox(fbox* box, const cv::Size& size)
{
    float target_aspect = size.width / (float)size.height;
    float new_width = target_aspect * box->h;
    box->x -= (new_width - box->w) / 2;
    box->w = new_width;

    box->x = std::min((float)(image.cols - 1), std::max(box->x, 0.0f));
    box->y = std::min((float)(image.rows - 1), std::max(box->y, 0.0f));
    box->w = std::max(0.0f, std::min(box->w, image.cols - 1 - box->x));
    box->h = std::max(0.0f, std::min(box->h, image.rows - 1 - box->y));
}

cv::Mat ImageFrame::getCrop(fbox* box, const cv::Size& size) const
{
    fbox original_box(box->x, box->y, box->w, box->h);
    float target_aspect = size.width / (float)size.height;
    float new_width = target_aspect * box->h;
    box->x -= (new_width - box->w) / 2;
    box->w = new_width;

    box->x = std::min((float)(image.cols - 1), std::max(box->x, 0.0f));
    box->y = std::min((float)(image.rows - 1), std::max(box->y, 0.0f));
    box->w = std::max(0.0f, std::min(box->w, image.cols - 1 - box->x));
    box->h = std::max(0.0f, std::min(box->h, image.rows - 1 - box->y));

    cv::Mat crop;
    try {
        crop = image(cv::Range(box->y, box->y + box->h), cv::Range(box->x, box->x + box->w));
        cv::resize(crop, crop, size);
    }
    catch (const std::exception& e) {
        std::cout << "ImageFrame::getCrop exception: " << e.what() << std::endl;
        std::cout << "x1: " << box->x << " y1: " << box->y << " x2: " << box->x + box->w << " y2: " << box->y + box->h << std::endl;
        std::cout << "x: " << original_box.x << " y: " << original_box.y << " w: " << original_box.w << " h: " << original_box.h << std::endl;
        crop = cv::Mat(size.height, size.width, CV_8UC3, cv::Scalar(64, 64, 64));
    }

    return crop;
}

void ImageFrame::clear()
{
    for (size_t i = 0; i < features.size(); i++)
        features[i].clear();
    features.clear();
    crops.clear();
    detections.clear();
    result.clear();
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

fbox::fbox(const DETECTBOX& box)
{
    x = (float)box[0];
    y = (float)box[1];
    w = (float)box[2];
    h = (float)box[3];
}

fbox::fbox(const QRect& rect)
{
    x = (float)rect.x();
    y = (float)rect.y();
    w = (float)rect.width();
    h = (float)rect.height();
}

fbox::fbox(float x, float y, float w, float h)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
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
