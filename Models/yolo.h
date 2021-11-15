#ifndef YOLO_H
#define YOLO_H

#include <QMainWindow>
#include <QGroupBox>
#include <QRunnable>

#include "Utilities/filesetter.h"
#include "Utilities/numbertextbox.h"
#include "Utilities/slider.h"
#include "Utilities/waitbox.h"
#include "yolo_v2_class.hpp"

class YoloLoader : public QObject, public QRunnable
{
    Q_OBJECT

public:
    YoloLoader(QObject *parent);
    void run() override;

    QObject *yolo;

signals:
    void done(int);

};

class YoloCfg : public QGroupBox
{
    Q_OBJECT

public:
    YoloCfg(QObject *parent);
    QSize getModelDimensions() const;

    QObject *yolo;
    QSize modelDimensions;
    NumberTextBox *modelWidth;
    NumberTextBox *modelHeight;
    QPushButton *setDims;

public slots:
    void setModelDimensions();
    void cfgEdited(const QString&);
    void setCfg(const QString&);

};

class Yolo : public QWidget
{
    Q_OBJECT

public:
    Yolo(QMainWindow *parent, const QString& baseKey);

    QMainWindow *mainWindow;
    QString baseKey;

    Detector *detector = nullptr;
    YoloLoader *loader;
    WaitBox *waitBox;
    bool loading;

    FileSetter *names;
    FileSetter *cfg;
    FileSetter *weights;
    YoloCfg *yoloCfg;
    Slider *slider;

    int gpu_id = 0;
    std::vector<std::string> obj_names;

signals:
    void namesSet();

public slots:
    void setNames(const QString&);
    void loadModel();
    void loaderCallback(int);

};

#endif // YOLO_H
