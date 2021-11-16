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
    cropParam = new CropParam(mainWindow, cropDialog);

    txtNNBudget = new NumberTextBox(MW->settings, "Matcher/txtNNBudget", INTEGER_NUMBER, "nn budget");
    if (txtNNBudget->text().length() == 0) txtNNBudget->setIntValue(100);
    txtMaxCosineDistance = new NumberTextBox(MW->settings, "Matcher/txtMaxCosineDistance", FLOAT_NUMBER, "max cosine dist");
    if (txtMaxCosineDistance->text().length() == 0) txtMaxCosineDistance->setFloatValue(0.2f);
    txtMinConfidence = new NumberTextBox(MW->settings, "Matcher/txtMinConfidence", FLOAT_NUMBER, "min confidence");
    if (txtMinConfidence->text().length() == 0) txtMinConfidence->setFloatValue(0.3f);
    txtNmsMaxOverlap = new NumberTextBox(MW->settings, "Matcher/txtNmsMaxOverlap", FLOAT_NUMBER, "nms max overlap");
    if (txtNmsMaxOverlap->text().length() == 0) txtNmsMaxOverlap->setFloatValue(1.0f);

    elapsed_0 = new QLabel();
    QLabel *lbl04 = new QLabel("time elpased:");
    elapsed_1 = new QLabel();
    QLabel *lbl05 = new QLabel("time elpased:");
    elapsed_2 = new QLabel();
    QLabel *lbl06 = new QLabel("time elpased:");
    detection_count = new QLabel();
    QLabel *lbl07 = new QLabel("count:");

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
    layout->addWidget(lbl05,                      5, 2, 1, 1, Qt::AlignRight);
    layout->addWidget(elapsed_1,                  5, 3, 1, 1);
    layout->addWidget(cropParam,                  6, 0, 1, 4);
    panel->setLayout(layout);

    runner_0 = new Runner_0(this);
    runner_1 = new Runner_1(this);
    runner_2 = new Runner_2(this);

    mytracker = new tracker(txtMaxCosineDistance->floatValue(), txtNNBudget->intValue());
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

        for (Track& track : M->mytracker->tracks) {
            if (!track.is_confirmed() || track.time_since_update > 1)
                continue;
            M->imageFrames[1].result.push_back(std::make_pair(track.track_id, track.to_tlwh()));
        }
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
        int rows = M->cropParam->spinRows->value();
        int cols = M->cropParam->spinCols->value();
        cv::Mat image(rows * crop_height, cols * crop_width, CV_8UC3, cv::Scalar(128, 128, 128));
        size_t max_block = rows * cols;
        size_t loop_end = min(max_block, M->imageFrames[2].result.size());
        for (size_t i = 0; i < loop_end; i++) {
            int id = M->imageFrames[2].result[i].first % (rows * cols);
            size_t y = id / cols;
            size_t x = id - y * cols;

            fbox box(M->imageFrames[2].result[i].second);
            cv::Mat crop = M->imageFrames[2].getCrop(M->imageFrames[2].image, &box, cv::Size(crop_width, crop_height));
            crop.copyTo(image(cv::Rect(x*crop_width, y*crop_height, crop_width, crop_height)));
        }

        M->cropDialog->lblImage->setPixmap(QPixmap::fromImage(QImage(image.data, image.cols, image.rows, image.step, QImage::Format_BGR888)));

        for (size_t i = 0; i < M->imageFrames[2].result.size(); i++) {
            DETECTBOX box = M->imageFrames[2].result[i].second;
            cv::Rect rect = cv::Rect(box[0], box[1], box[2], box[3]);
            cv::rectangle(M->imageFrames[2].image, rect, cv::Scalar(255, 0, 0), 2);
            cv::putText(M->imageFrames[2].image, QString::number(M->imageFrames[2].result[i].first).toStdString(), cv::Point(rect.x, rect.y),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        }
    }

    finished = true;

    auto stop = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    //std::cout << "elasped 2 : " << elapsed << std::endl;
}
