#ifndef FEATUREMODEL_H
#define FEATUREMODEL_H

#include <iostream>
#include <tensorflow/c/c_api.h>
#include <SDL.h>

#include <QMainWindow>
#include <QObject>
#include <QRunnable>
#include <QThreadPool>

#include "imageframe.h"
#include "cropdialog.h"
#include "Utilities/panel.h"
#include "Utilities/directorysetter.h"
#include "Utilities/waitbox.h"
#include "Utilities/spinbox.h"

class FeatureModelLoader : public QObject, public QRunnable
{
    Q_OBJECT

public:
    FeatureModelLoader(QObject *featureModel);
    void run() override;

    QObject *featureModel;

signals:
    void done(int);

};

class FeatureModel : public Panel
{
    Q_OBJECT

public:
    FeatureModel(QMainWindow *parent);
    ~FeatureModel();

    bool load(const QString& saved_model_dir, double pct_gpu_mem = 0.2);
    bool infer(ImageFrame* frame);
    void clear();

    TF_SessionOptions* CreateSessionOptions(double perecentage);
    static void NoOpDeallocator(void *data, size_t a, void *b) { }

    DirectorySetter *savedModelSetter;
    SpinBox *spinPctGpuMem;

    bool initialized = false;
    const char *tags = "serve";
    TF_Graph *Graph = nullptr;
    TF_Status *Status = nullptr;
    TF_SessionOptions *SessionOpts = nullptr;
    TF_Buffer *RunOpts = nullptr;
    TF_Session *Session = nullptr;
    int ntags = 1;

    TF_Output *Input = nullptr;
    TF_Output *Output = nullptr;
    TF_Tensor **InputValues = nullptr;
    TF_Tensor **OutputValues = nullptr;
    int NumInputs = 1;
    int NumOutputs = 1;

    FeatureModelLoader *loader;
    WaitBox *waitBox;
    bool loading;

public slots:
    void loaderCallback(int);
    void load();

};

#endif // FEATUREMODEL_H
