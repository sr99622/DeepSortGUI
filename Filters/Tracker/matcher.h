#ifndef MATCHER_H
#define MATCHER_H

#include <QMainWindow>
#include <QRunnable>
#include "Filters/filter.h"
#include "Filters/darknet.h"
#include "imageframe.h"
#include "featuremodel.h"
#include "DeepSort/matching/tracker.h"
#include "Utilities/numbertextbox.h"

#define args_nn_budget 100
#define args_max_cosine_distance 0.2
#define args_min_confidence 0.3
#define args_nms_max_overlap 1.0

class Runner_0 : public QObject, public QRunnable
{
    Q_OBJECT

public:
    Runner_0(QObject *parent) {
        matcher = parent;
        setAutoDelete(false);
    };
    void run() override;
    bool finished;

    QObject *matcher;

};

class Runner_1 : public QObject, public QRunnable
{
    Q_OBJECT

public:
    Runner_1(QObject *parent) {
        matcher = parent;
        setAutoDelete(false);
    };
    void run() override;
    bool finished;

    QObject *matcher;

};

class Runner_2 : public QObject, public QRunnable
{
    Q_OBJECT

public:
    Runner_2(QObject *parent) {
        matcher = parent;
        setAutoDelete(false);
    };
    void run() override;
    bool finished;

    QObject *matcher;

};

class Matcher : public Filter
{
    Q_OBJECT

public:
    Matcher(QMainWindow *parent);
    void filter(Frame *vp) override;
    void autoSave() override;
    bool threadsFinished();
    void initializeModels();

    QMainWindow *mainWindow;

    Darknet *darknet;
    FeatureModel *featureModel;
    tracker *mytracker;

    ImageFrame imageFrames[3];
    Runner_0 *runner_0;
    Runner_1 *runner_1;
    Runner_2 *runner_2;

    NumberTextBox *txtNNBudget;
    NumberTextBox *txtMaxCosineDistance;
    NumberTextBox *txtMinConfidence;
    NumberTextBox *txtNmsMaxOverlap;

    const QString nnBudgetKey = "Matcher/NNBudget";
    const QString maxCosineDistanceKey = "Matcher/MaxCosineDistance";
    const QString minConfidenceKey = "Matcher/MinConfidence";
    const QString nmsMaxOverlapKey = "Matcher/NmsMaxOverlap";

    std::string cfg_filename = "C:\\Users\\sr996\\models\\reduced\\ami1\\yolov4.cfg";
    std::string weight_filename = "C:\\Users\\sr996\\models\\reduced\\ami1\\yolov4.weights";
    QString saved_model_dir = "C:\\Users\\sr996\\source\\repos\\deep_sort_v2\\saved_model";

public slots:
    void nnBudgetSet();
    void maxCosineDistanceSet();
    void minConfidenceSet();
    void nmsMaxOverlapSet();

};

#endif // MATCHER_H
