#ifndef FEATUREMODEL_H
#define FEATUREMODEL_H

#include <iostream>
#include <tensorflow/c/c_api.h>

#include <QMainWindow>
#include <QObject>
#include <QSpinBox>
#include <QRunnable>
#include <QThreadPool>

#include "imageframe.h"
#include "Utilities/panel.h"
#include "Utilities/directorysetter.h"
#include "Utilities/waitbox.h"

class FeatureModelLoader : public QObject, public QRunnable
{
    Q_OBJECT

public:
    FeatureModelLoader(QObject *featureModel);
    void run() override;

    QObject *featureModel;
    QString saved_model_dir;
    double pct_gpu_mem;

signals:
    void done(int);

};

class FeatureModel : public Panel
{
    Q_OBJECT

public:
    FeatureModel(QMainWindow *parent);
    ~FeatureModel();

    void autoSave() override;
    bool load(const QString& saved_model_dir, double pct_gpu_mem = 0.2);
    bool infer(ImageFrame* frame);
    void clear();

    TF_SessionOptions* CreateSessionOptions(double perecentage);
    static void NoOpDeallocator(void *data, size_t a, void *b) { }

    DirectorySetter *savedModelSetter;
    QSpinBox *spinPctGpuMem;

    QString savedModelDir;
    double pct_gpu_mem;
    const QString savedModelKey = "FeatureModel/savedModelDir";
    const QString pctGpuMemKey = "FeatureModel/pctGpuMemKey";

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
    void setSavedModelDir(const QString&);
    void pctGpuMemChanged(int);
    void loaderCallback(int);
    void load();

};

#endif // FEATUREMODEL_H
