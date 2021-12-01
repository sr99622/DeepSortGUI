#include "matcher.h"
#include "mainwindow.h"

Matcher::Matcher(QMainWindow *parent)
{
    mainWindow = parent;
    name = "Tracker";
    panel = new Panel(mainWindow);
    yolo = new Yolo(mainWindow, "Matcher");
    featureModel = new FeatureModel(mainWindow);
    cropDialog = new CropDialog(mainWindow);
    focusDialog = new FocusDialog(mainWindow);
    trackerStatsDialog = new TrackerStatsDialog(mainWindow);

    txtNNBudget = new NumberTextBox(MW->settings, "Matcher/txtNNBudget", INTEGER_NUMBER, "nn budget");
    if (txtNNBudget->text().length() == 0) txtNNBudget->setIntValue(100);
    txtMaxCosineDistance = new NumberTextBox(MW->settings, "Matcher/txtMaxCosineDistance", FLOAT_NUMBER, "max cosine dist");
    if (txtMaxCosineDistance->text().length() == 0) txtMaxCosineDistance->setFloatValue(0.2f);
    txtMinConfidence = new NumberTextBox(MW->settings, "Matcher/txtMinConfidence", FLOAT_NUMBER, "min confidence");
    if (txtMinConfidence->text().length() == 0) txtMinConfidence->setFloatValue(0.3f);
    txtNmsMaxOverlap = new NumberTextBox(MW->settings, "Matcher/txtNmsMaxOverlap", FLOAT_NUMBER, "nms max overlap");
    if (txtNmsMaxOverlap->text().length() == 0) txtNmsMaxOverlap->setFloatValue(1.0f);

    elapsed_0 = new QLabel();
    QLabel *lbl04 = new QLabel("time elapsed:");
    elapsed_1 = new QLabel();
    QLabel *lbl05 = new QLabel("time elapsed:");
    elapsed_2 = new QLabel();
    QLabel *lbl06 = new QLabel("time elapsed:");
    detection_count = new QLabel();
    QLabel *lbl07 = new QLabel("count:");

    QPushButton *btnShowTrackerStats = new QPushButton("Stats");
    btnShowTrackerStats->setMaximumWidth(btnShowTrackerStats->fontMetrics().boundingRect(btnShowTrackerStats->text()).width() * 1.5);
    connect(btnShowTrackerStats, SIGNAL(clicked()), this, SLOT(showTrackerStats()));

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(yolo,                       0, 0, 1, 4);
    layout->addWidget(lbl07,                      1, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(detection_count,            1, 1, 1, 1);
    layout->addWidget(lbl04,                      1, 2, 1, 1, Qt::AlignRight);
    layout->addWidget(elapsed_0,                  1, 3, 1, 1);
    layout->addWidget(featureModel,               2, 0, 1, 4);
    layout->addWidget(txtNNBudget->lbl,           3, 0, 1, 1);
    layout->addWidget(txtNNBudget,                3, 1, 1, 1);
    layout->addWidget(txtMaxCosineDistance->lbl,  3, 2, 1, 1);
    layout->addWidget(txtMaxCosineDistance,       3, 3, 1, 1);
    layout->addWidget(txtMinConfidence->lbl,      4, 0, 1, 1);
    layout->addWidget(txtMinConfidence,           4, 1, 1, 1);
    layout->addWidget(txtNmsMaxOverlap->lbl,      4, 2, 1, 1);
    layout->addWidget(txtNmsMaxOverlap,           4, 3, 1, 1);
    layout->addWidget(btnShowTrackerStats,        5, 0, 1, 1);
    layout->addWidget(lbl05,                      5, 2, 1, 1, Qt::AlignRight);
    layout->addWidget(elapsed_1,                  5, 3, 1, 1);
    layout->addWidget(cropDialog->param,          6, 0, 1, 4);
    layout->addWidget(focusDialog->param,         7, 0, 1, 4);
    layout->addWidget(lbl06,                      8, 2, 1, 1, Qt::AlignRight);
    layout->addWidget(elapsed_2,                  8, 3, 1, 1);
    panel->setLayout(layout);

    runner_0 = new Runner_0(this);
    runner_1 = new Runner_1(this);
    runner_2 = new Runner_2(this);

    mytracker = new tracker(txtMaxCosineDistance->floatValue(), txtNNBudget->intValue());
    connect(MW->display(), SIGNAL(clicked(const QPoint&)), this, SLOT(handleDisplayClicked(const QPoint&)));

}

void Matcher::showTrackerStats()
{
    trackerStatsDialog->show();
}

void Matcher::initializeModels()
{
    yolo->loadModel();
    featureModel->load(featureModel->savedModelSetter->path(), featureModel->spinPctGpuMem->value()/(float)100);
}

void Matcher::filter(Frame *vp)
{
    if (!yolo->started) {
        yolo->started = true;
        yolo->loading = true;
        initializeModels();
    }

    if (!yolo->loading) {
        imageFrames[2] = imageFrames[1];
        imageFrames[1] = imageFrames[0];
        imageFrames[0].clear();
        imageFrames[0].image = vp->toMat();

        runner_0->finished = false;
        QThreadPool::globalInstance()->tryStart(runner_0);
        runner_1->finished = false;
        QThreadPool::globalInstance()->tryStart(runner_1);
        runner_2->finished = false;
        QThreadPool::globalInstance()->tryStart(runner_2);

        while (!threadsFinished())
            QThread::msleep(1);

        if (imageFrames[2].image.rows > 0)
            vp->readMat(imageFrames[2].image);
    }
}

bool Matcher::threadsFinished()
{
    return runner_0->finished && runner_1->finished && runner_2->finished;
}

void Matcher::handleDisplayClicked(const QPoint& pos)
{
    displayClickedPos = pos;
}

void Matcher::checkDisplayClicked()
{
    // if the main display has been clicked, translate display coordinates to find track id of
    // object clicked upon


    if (!displayClickedPos.isNull()) {

        float h1 = imageFrames[2].image.rows;
        float w1 = imageFrames[2].image.cols;

        float h = MW->display()->height();
        float w = MW->display()->width();
        int x = displayClickedPos.x();
        int y = displayClickedPos.y();

        float image_aspect_ratio = w1 / h1;
        float display_aspect_ratio = w / h;

        QPoint scaledPos;

        if (image_aspect_ratio > display_aspect_ratio) {
            float slope = w1 / w;
            float intercept = (h1 - h * slope) / 2;
            int x1 = slope * x;
            int y1 = slope * y + intercept;
            scaledPos = QPoint(x1, y1);
        }
        else {
            float slope = h1 / h;
            float intercept = (w1 - w * slope) / 2;
            int y1 = slope * y;
            int x1 = slope * x + intercept;
            scaledPos = QPoint(x1, y1);
        }

        std::vector<pair<int, QRect>> rects;
        for (RESULT_DATA result : imageFrames[2].result) {
            DETECTBOX box = result.second;
            QRect rect(box[IDX_X], box[IDX_Y], box[IDX_W], box[IDX_H]);
            if (rect.contains(scaledPos))
                rects.push_back(make_pair(result.first, rect));
        }

        if (rects.size() > 0) {
            QRect minRect = rects[0].second;
            int min_track_id = rects[0].first;
            for (size_t i = 1; i < rects.size(); i++) {
                if (minRect.width() * minRect.height() > rects[i].second.width() * rects[i].second.height()) {
                    minRect = rects[i].second;
                    min_track_id = rects[i].first;
                }
            }
            cropDialog->focus_track_id = min_track_id;
        }

        displayClickedPos = QPoint(0, 0);
    }
}

void Matcher::fillFocusDialog()
{
    bool found = false;
    for (size_t i = 0; i < imageFrames[2].result.size(); i++) {
        int track_id = imageFrames[2].result[i].first;
        fbox box(imageFrames[2].result[i].second);
        //cv::Rect rect = cv::Rect(box.x, box.y, box.w, box.h);

        if (track_id == cropDialog->focus_track_id) {
            found = true;
            int width = focusDialog->param->txtCropWidth->intValue();
            int height = focusDialog->param->txtCropHeight->intValue();
            QRect rect(0, 0, width, height);
            rect.moveCenter(QPoint(box.x + box.w/2, box.y + box.h/2));
            fbox focus_box(rect);
            cv::Mat image = imageFrames[2].getFocusCrop(&focus_box);

            QSize sizeRect = rect.size();
            sizeRect.scale(focusDialog->width(), focusDialog->height(), Qt::KeepAspectRatio);
            if (sizeRect.isValid()) {
                try {
                    cv::resize(image, image, cv::Size(sizeRect.width(), sizeRect.height()));
                }
                catch (const exception& e) {
                    std::cout << "Matcher::fillFocusDialog error: " << e.what() << std::endl;
                }
            }

            focusDialog->lblImage->setPixmap(QPixmap::fromImage(QImage(image.data, image.cols, image.rows, image.step, QImage::Format_BGR888)));
        }
    }

}

void Matcher::fillCropDialog()
{
    int rows = cropDialog->param->spinRows->value();
    int cols = cropDialog->param->spinCols->value();
    cv::Mat image(rows * crop_height, cols * crop_width, CV_8UC3, cv::Scalar(64, 64, 64));
    size_t loop_end = min((size_t)rows * cols, imageFrames[2].result.size());

    for (size_t i = 0; i < loop_end; i++) {
        int track_id = imageFrames[2].result[i].first;
        int assign_id = cropDialog->param->findAssignment(track_id);
        if (assign_id < 0) {
            assign_id = cropDialog->param->findAssignment(-1);
            if (assign_id > -1) {
                cropDialog->param->trackAssignments[assign_id] = track_id;
            }
        }

        size_t y = assign_id / cols;
        size_t x = assign_id - y * cols;

        fbox box(imageFrames[2].result[i].second);
        cv::Size target_size(crop_width, crop_height);
        imageFrames[2].validateBox(&box, target_size);

        if (box.w > 1 && box.h > 1) {
            try {
                cv::Mat crop = imageFrames[2].image(cv::Range(box.y, box.y + box.h), cv::Range(box.x, box.x + box.w));
                cv::resize(crop, crop, target_size);
                crop.copyTo(image(cv::Rect(x * crop_width, y * crop_height, crop_width, crop_height)));
            }
            catch (const std::exception& e) {
                std::cout << "x: " << box.x << " y: " << box.y << " w: " << box.w << " h: " << box.h << std::endl;
                std::cout << "Runner 2 error: " << e.what() << std::endl;
            }
        }

        for (int track_id : cropDialog->param->expiredTrackIds) {
            int assign_id = cropDialog->param->findAssignment(track_id);
            if (assign_id > -1) {
                cropDialog->param->trackAssignments[assign_id] = -1;
            }
        }
    }

    cropDialog->lblImage->setPixmap(QPixmap::fromImage(QImage(image.data, image.cols, image.rows, image.step, QImage::Format_BGR888)));
}

void Runner_0::run()
{
    auto start = std::chrono::high_resolution_clock::now();

    Matcher *M = (Matcher*)matcher;

    std::vector<bbox_t> detections = M->yolo->detector->detect(M->imageFrames[0].image);
    for (size_t i = 0; i < detections.size(); i++) {
        bbox_t d = detections[i];
        if (d.obj_id == 0) {
            //cv::rectangle(M->imageFrames[0].image, cv::Rect(d.x, d.y, d.w, d.h), cv::Scalar(255, 255, 255), 2);
            fbox f(d);
            M->imageFrames[0].detections.push_back(f);
        }
    }

    finished = true;

    auto stop = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    M->detection_count->setText(QString::number(M->imageFrames[0].detections.size()));
    M->elapsed_0->setText(QString::number(elapsed));
}

void Runner_1::run()
{
    auto start = std::chrono::high_resolution_clock::now();

    Matcher *M = (Matcher*)matcher;
    if (M->imageFrames[1].image.rows > 0) {
        M->imageFrames[1].getCrops();
        M->featureModel->infer(&M->imageFrames[1]);

        DETECTIONS detections;
        M->imageFrames[1].writeToDetections(&detections);

        ModelDetection::getInstance()->dataMoreConf(M->txtMinConfidence->floatValue(), detections);
        ModelDetection::getInstance()->dataPreprocessing(M->txtNmsMaxOverlap->floatValue(), detections);

        M->mytracker->predict();
        M->mytracker->update(detections);

        M->cropDialog->param->expiredTrackIds.clear();

        for (Track& track : M->mytracker->tracks) {
            if (!track.is_confirmed() /* || track.time_since_update > 1 */ )
                continue;

            M->imageFrames[1].result.push_back(std::make_pair(track.track_id, track.to_tlwh()));

            if (track.time_since_update == track._max_age)
                M->cropDialog->param->expiredTrackIds.push_back(track.track_id);
        }

        ((TrackerStatsPanel*)M->trackerStatsDialog->panel)->populate(M->mytracker);
    }

    finished = true;

    auto stop = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    M->elapsed_1->setText(QString::number(elapsed));
}

void Runner_2::run()
{
    auto start = std::chrono::high_resolution_clock::now();

    Matcher *M = (Matcher*)matcher;
    if (M->imageFrames[2].result.size() > 0) {

        M->fillCropDialog();
        M->checkDisplayClicked();
        M->fillFocusDialog();

        cv::Scalar blue(255, 0, 0);
        cv::Scalar red(0, 0, 255);
        for (size_t i = 0; i < M->imageFrames[2].result.size(); i++) {
            int track_id = M->imageFrames[2].result[i].first;
            fbox box(M->imageFrames[2].result[i].second);
            cv::Rect rect = cv::Rect(box.x, box.y, box.w, box.h);


            if (track_id == M->cropDialog->focus_track_id) {
                cv::rectangle(M->imageFrames[2].image, rect, red, 2);
            }
            else {
                cv::rectangle(M->imageFrames[2].image, rect, blue, 2);
            }

            cv::putText(M->imageFrames[2].image, QString::number(track_id).toStdString(), cv::Point(rect.x, rect.y),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        }
    }

    finished = true;

    auto stop = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    M->elapsed_2->setText(QString::number(elapsed));
}
