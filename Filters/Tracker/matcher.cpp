#include "matcher.h"
#include "mainwindow.h"

#define M ((Matcher*)matcher)

Matcher::Matcher(QMainWindow *parent)
{
    mainWindow = parent;
    name = "Tracker";
    panel = new Panel(mainWindow);
    darknet = new Darknet(mainWindow, "Matcher");
    featureModel = new FeatureModel(mainWindow);

    QLabel *lbl00 = new QLabel("nn budget");
    txtNNBudget = new NumberTextBox();
    txtNNBudget->setIntValue(MW->settings->value(nnBudgetKey, 100).toInt());
    connect(txtNNBudget, SIGNAL(editingFinished()), this, SLOT(nnBudgetSet()));
    QLabel *lbl01 = new QLabel("max cosine dist");
    txtMaxCosineDistance = new NumberTextBox();
    txtMaxCosineDistance->setFloatValue(MW->settings->value(maxCosineDistanceKey, 0.2f).toFloat());
    connect(txtMaxCosineDistance, SIGNAL(editingFinished()), this, SLOT(maxCosineDistanceSet()));
    QLabel *lbl02 = new QLabel("min confidence");
    txtMinConfidence = new NumberTextBox();
    txtMinConfidence->setFloatValue(MW->settings->value(minConfidenceKey, 0.3f).toFloat());
    connect(txtMinConfidence, SIGNAL(editingFinished()), this, SLOT(minConfidenceSet()));
    QLabel *lbl03 = new QLabel("nms max overlap");
    txtNmsMaxOverlap = new NumberTextBox();
    txtNmsMaxOverlap->setFloatValue(MW->settings->value(nmsMaxOverlapKey, 1.0f).toFloat());
    connect(txtNmsMaxOverlap, SIGNAL(editingFinished()), this, SLOT(nmsMaxOverlapSet()));

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(darknet->panel,       0, 0, 1, 4);
    layout->addWidget(featureModel,         1, 0, 1, 4);
    layout->addWidget(lbl00,                2, 0, 1, 1);
    layout->addWidget(txtNNBudget,          2, 1, 1, 1);
    layout->addWidget(lbl01,                2, 2, 1, 1);
    layout->addWidget(txtMaxCosineDistance, 2, 3, 1, 1);
    layout->addWidget(lbl02,                3, 0, 1, 1);
    layout->addWidget(txtMinConfidence,     3, 1, 1, 1);
    layout->addWidget(lbl03,                3, 2, 1, 1);
    layout->addWidget(txtNmsMaxOverlap,     3, 3, 1, 1);
    panel->setLayout(layout);

    runner_0 = new Runner_0(this);
    runner_1 = new Runner_1(this);
    runner_2 = new Runner_2(this);

    std::cout << "cfg->filename: " << darknet->cfg->filename.toStdString() << std::endl;
    std::cout << "weights->filename: " << darknet->weights->filename.toStdString() << std::endl;
    std::cout << "names->filename: " << darknet->names->filename.toStdString() << std::endl;

    mytracker = new tracker(txtMaxCosineDistance->floatValue(), txtNNBudget->intValue());
    //darknet->model->detector = new Detector(cfg_filename, weight_filename);

    //initializeModels();

}

void Matcher::initializeModels()
{
    darknet->model = new DarknetModel(mainWindow, darknet);
    darknet->model->initialize(darknet->cfg->filename, darknet->weights->filename, darknet->names->filename);

    //featureModel->load();
    featureModel->load(featureModel->savedModelDir, featureModel->pct_gpu_mem);
}

void Matcher::filter(Frame *vp)
{
    if (darknet->model == nullptr) {
        darknet->loading = true;
        initializeModels();
    }

    if (!darknet->loading) {
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

void Matcher::nnBudgetSet()
{
    MW->settings->setValue(nnBudgetKey, txtNNBudget->intValue());
}

void Matcher::maxCosineDistanceSet()
{
    MW->settings->setValue(maxCosineDistanceKey, txtMaxCosineDistance->floatValue());
}

void Matcher::minConfidenceSet()
{
    MW->settings->setValue(minConfidenceKey, txtMinConfidence->floatValue());
}

void Matcher::nmsMaxOverlapSet()
{
    MW->settings->setValue(nmsMaxOverlapKey, txtNmsMaxOverlap->floatValue());
}

void Matcher::autoSave()
{

}

bool Matcher::threadsFinished()
{
    return runner_0->finished && runner_1->finished && runner_2->finished;
}

void Runner_0::run()
{
    std::cout << "runner 0 start: " << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<bbox_t> detections = M->darknet->model->detector->detect(M->imageFrames[0].image);
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
    std::cout << "runner 0 elapsed time: " << elapsed << std::endl;
}

void Runner_1::run()
{
    std::cout << "runner 1 start: " << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    if (M->imageFrames[1].image.rows > 0) {

        std::cout << "test 1" << std::endl;

        M->imageFrames[1].getCrops();
        M->featureModel->infer(&M->imageFrames[1]);

        DETECTIONS detections;
        M->imageFrames[1].writeToDetections(&detections);

        ModelDetection::getInstance()->dataMoreConf(M->txtMinConfidence->floatValue(), detections);
        ModelDetection::getInstance()->dataPreprocessing(M->txtNmsMaxOverlap->floatValue(), detections);

        std::cout << "test 2" << std::endl;

        M->mytracker->predict();
        std::cout << "test 2A: " << std::endl;
        M->mytracker->update(detections);

        std::cout << "test 3" << std::endl;

        std::vector<RESULT_DATA> result;
        for (Track& track : M->mytracker->tracks) {
            if (!track.is_confirmed() || track.time_since_update > 1)
                continue;
            result.push_back(std::make_pair(track.track_id, track.to_tlwh()));
        }

        std::cout << "test 4" << std::endl;

        for (size_t i = 0; i < result.size(); i++) {
            DETECTBOX box = result[i].second;
            cv::Rect rect = cv::Rect(box[0], box[1], box[2], box[3]);
            cv::rectangle(M->imageFrames[1].image, rect, cv::Scalar(255, 0, 0), 2);
            cv::putText(M->imageFrames[1].image, QString::number(result[i].first).toStdString(), cv::Point(rect.x, rect.y),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

        }

    }

    finished = true;

    auto stop = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    cout << "runner 1 elapsed time: " << elapsed << endl;
}

void Runner_2::run()
{
    std::cout << "runner 2 start: " << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    //rectangle(M->imageFrames[2].image, Rect(150, 150, 100, 100), Scalar(0, 0, 255), 2);
    finished = true;

    auto stop = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    cout << "runner 2 elapsed time: " << elapsed << endl;
}
