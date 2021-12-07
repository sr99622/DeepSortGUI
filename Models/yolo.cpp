#include "yolo.h"
#include "mainwindow.h"

Yolo::Yolo(QMainWindow *parent, const QString& baseKey) : QWidget(parent)
{
    mainWindow = parent;
    this->baseKey = baseKey;

    names = new FileSetter(mainWindow, "Names", "Names(*.names)", MW->settings, baseKey + "/Yolo/names");
    if (names->path().length() > 0) setNames(names->path());
    connect(names, SIGNAL(fileSet(const QString&)), this, SLOT(setNames(const QString&)));

    weights = new FileSetter(mainWindow, "Weight", "Weights(*.weights)", MW->settings, baseKey + "/Yolo/weights");

    cfg = new FileSetter(mainWindow, "Config", "Config(*.cfg)", MW->settings, baseKey + "/Yolo/cfg");
    yoloCfg = new YoloCfg(this);
    connect(cfg, SIGNAL(fileSet(const QString&)), yoloCfg, SLOT(setCfg(const QString&)));

    slider = new Slider(Qt::Horizontal, MW->settings, baseKey + "/Yolo/threshold", "Threshold", this);

    QPushButton *btnLoad = new QPushButton("Load");
    btnLoad->setMaximumWidth(btnLoad->fontMetrics().boundingRect(btnLoad->text()).width() * 1.5);
    connect(btnLoad, SIGNAL(clicked()), this, SLOT(loadModel()));

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(names,      0, 0, 1, 6);
    layout->addWidget(weights,    1, 0, 1, 6);
    layout->addWidget(cfg,        2, 0, 1, 6);
    layout->addWidget(yoloCfg,    3, 0, 1, 6);
    layout->addWidget(slider,     4, 0, 1, 5);
    layout->addWidget(btnLoad,    4, 5, 1, 1);
    setLayout(layout);

    waitBox = new WaitBox(mainWindow);
}

void Yolo::loadModel()
{
    started = true;
    if (detector != nullptr)
        detector->~Detector();

    if (!QFile::exists(cfg->filename)) {
        QString str;
        QTextStream(&str) << "Unable to load config file: " << cfg->filename;
        QMessageBox::critical(mainWindow, "Model Config Load Error", str);
        return;
    }

    if (!QFile::exists(weights->filename)) {
        QString str;
        QTextStream(&str) << "Unable to load weights file: " << weights->filename;
        QMessageBox::critical(mainWindow, "Model Weights Load Error", str);
        return;
    }

    loader = new YoloLoader(this);
    connect(loader, SIGNAL(done(int)), waitBox, SLOT(done(int)));
    connect(loader, SIGNAL(done(int)), this, SLOT(loaderCallback(int)));
    QThreadPool::globalInstance()->start(loader);
    waitBox->exec();
}

void Yolo::loaderCallback(int arg)
{
    Q_UNUSED(arg);
    loading = false;
}

void Yolo::setNames(const QString& path)
{
    obj_names.clear();
    std::ifstream file(path.toStdString());
    if (!file.is_open()) {
        QString str;
        QTextStream(&str) << "Unable to load model names file: " << path;
        MW->msg(str);
        return;
    }

    obj_names.clear();
    for (std::string line; getline(file, line);) {
        obj_names.push_back(line);
        std::cout << "names: " << line << std::endl;
    }

    emit namesSet();
}

YoloCfg::YoloCfg(QObject *parent) : QGroupBox("Model Resolution")
{
    yolo = parent;
    modelWidth = new NumberTextBox();
    modelWidth->setMaximumWidth(modelWidth->fontMetrics().boundingRect("00000").width() * 1.5);
    connect(modelWidth, SIGNAL(textEdited(const QString&)), this, SLOT(cfgEdited(const QString&)));
    QLabel *lbl00 = new QLabel("Width");

    modelHeight = new NumberTextBox();
    modelHeight->setMaximumWidth(modelHeight->fontMetrics().boundingRect("00000").width() * 1.5);
    connect(modelHeight, SIGNAL(textEdited(const QString&)), this, SLOT(cfgEdited(const QString&)));
    QLabel *lbl01 = new QLabel("Height");

    QSize dims = getModelDimensions();
    if (dims != QSize(0,0)) {
        modelWidth->setIntValue(dims.width());
        modelHeight->setIntValue(dims.height());
    }

    setDims = new QPushButton("Set");
    setDims->setMaximumWidth(50);
    connect(setDims, SIGNAL(clicked()), this, SLOT(setModelDimensions()));
    setDims->setEnabled(false);

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(lbl00,                    0, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(modelWidth,               0, 1, 1, 1, Qt::AlignLeft);
    layout->addWidget(lbl01,                    0, 2, 1, 1, Qt::AlignRight);
    layout->addWidget(modelHeight,              0, 3, 1, 1, Qt::AlignLeft);
    layout->addWidget(setDims,                  0, 5, 1, 1, Qt::AlignRight);
    setLayout(layout);
}

void YoloCfg::setModelDimensions()
{
    int width = modelWidth->text().toInt();
    int height = modelHeight->text().toInt();

    if (width%32 != 0 || height%32 != 0) {
        MainWindow *mw = (MainWindow*)((Yolo*)yolo)->mainWindow;
        QMessageBox::critical(mw->filterDialog, "Model Dimension Error", "Model Dimensions must be divisible by 32");
        return;
    }

    modelDimensions = QSize(width, height);

    QFile file(((Yolo*)yolo)->cfg->filename);
    if (!file.exists())
        return;

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    QString arg_width = "width=";
    arg_width.append(QString::number(width));
    QString arg_height = "height=";
    arg_height.append(QString::number(height));

    QString contents = file.readAll();

    int width_index = contents.indexOf("width");
    int width_length = contents.indexOf("\n", width_index) - width_index;

    contents.remove(width_index, width_length);
    contents.insert(width_index, arg_width);

    int height_index = contents.indexOf("height");
    int height_length = contents.indexOf("\n", height_index) - height_index;
    contents.remove(height_index, height_length);
    contents.insert(height_index, arg_height);

    file.seek(0);
    file.write(contents.toUtf8());
    file.close();
    setDims->setEnabled(false);
}

QSize YoloCfg::getModelDimensions() const
{
    QFile file(((Yolo*)yolo)->cfg->filename);

    if (!file.exists())
        return QSize(0, 0);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QSize(0, 0);

    QString contents = file.readAll();
    QString width_key = "width=";
    int width_index = contents.indexOf(width_key);
    int width_length = contents.indexOf("\n", width_index) - (width_index + width_key.length());
    QString arg_width = contents.mid(width_index + width_key.length(), width_length);

    QString height_key = "height=";
    int height_index = contents.indexOf(height_key);
    int height_length = contents.indexOf("\n", height_index) - (height_index + height_key.length());
    QString arg_height = contents.mid(height_index + height_key.length(), height_length);

    return QSize(arg_width.toInt(), arg_height.toInt());
}

void YoloCfg::cfgEdited(const QString& arg)
{
    Q_UNUSED(arg);
    setDims->setEnabled(true);
}

void YoloCfg::setCfg(const QString& arg)
{
    Q_UNUSED(arg)
    QSize dims = getModelDimensions();
    if (dims == QSize(0, 0)) {
        modelWidth->setText("");
        modelHeight->setText("");
    }
    else {
        modelWidth->setIntValue(dims.width());
        modelHeight->setIntValue(dims.height());
    }
    setDims->setEnabled(false);
}

YoloLoader::YoloLoader(QObject *parent)
{
    yolo = parent;
}

void YoloLoader::run()
{
    Yolo *Y = (Yolo*)yolo;
    Y->detector = new Detector(Y->cfg->filename.toStdString(), Y->weights->filename.toStdString(), Y->gpu_id);
    emit done(0);
}













