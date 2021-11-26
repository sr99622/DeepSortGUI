#ifndef MATCHER_H
#define MATCHER_H

#include <QMainWindow>
#include <QRunnable>
#include "Filters/filter.h"
#include "Models/yolo.h"
#include "imageframe.h"
#include "featuremodel.h"
#include "cropdialog.h"
#include "focusdialog.h"
#include "trackerstatspanel.h"
#include "DeepSort/matching/tracker.h"
#include "Utilities/numbertextbox.h"

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
    bool threadsFinished();
    void initializeModels();
    void checkDisplayClicked();
    void fillCropDialog();
    void fillFocusDialog();

    QMainWindow *mainWindow;

    Yolo *yolo;
    FeatureModel *featureModel;
    tracker *mytracker;
    CropDialog *cropDialog;
    FocusDialog *focusDialog;
    TrackerStatsDialog *trackerStatsDialog;

    QSize resolution;

    ImageFrame imageFrames[3];
    Runner_0 *runner_0;
    Runner_1 *runner_1;
    Runner_2 *runner_2;

    QLabel *elapsed_0;
    QLabel *elapsed_1;
    QLabel *elapsed_2;
    QLabel *detection_count;

    NumberTextBox *txtNNBudget;
    NumberTextBox *txtMaxCosineDistance;
    NumberTextBox *txtMinConfidence;
    NumberTextBox *txtNmsMaxOverlap;

    QPoint displayClickedPos;

signals:
    void subPictureApply();
    void subPictureReset();

public slots:
    void showTrackerStats();
    void handleDisplayClicked(const QPoint&);

};

#endif // MATCHER_H
