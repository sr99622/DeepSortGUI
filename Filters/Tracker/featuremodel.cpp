#include "featuremodel.h"
#include "mainwindow.h"

#define FM ((FeatureModel*)featureModel)

FeatureModel::FeatureModel(QMainWindow *parent) : Panel(parent)
{
    savedModelSetter = new DirectorySetter(mainWindow, "Saved Model");
    if (MW->settings->contains(savedModelKey)) {
        savedModelDir = MW->settings->value(savedModelKey).toString();
        savedModelSetter->setPath(savedModelDir);
    }
    connect(savedModelSetter, SIGNAL(directorySet(const QString&)), this, SLOT(setSavedModelDir(const QString&)));

    QLabel *lbl00 = new QLabel("% GPU Mem");
    spinPctGpuMem = new QSpinBox();
    if (MW->settings->contains(pctGpuMemKey)) {
        pct_gpu_mem = MW->settings->value(pctGpuMemKey).toDouble();
        spinPctGpuMem->setValue((int)(pct_gpu_mem * 100));
    }
    connect(spinPctGpuMem, SIGNAL(valueChanged(int)), this, SLOT(pctGpuMemChanged(int)));

    QPushButton *loadModel = new QPushButton("Load");
    loadModel->setMaximumWidth(loadModel->fontMetrics().boundingRect(loadModel->text()).width() * 1.5);
    connect(loadModel, SIGNAL(clicked()), this, SLOT(load()));

    QPushButton *showCrops = new QPushButton("Crops");
    connect(showCrops, SIGNAL(clicked()), this, SLOT(showCrops()));

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(savedModelSetter,  0, 0, 1, 4);
    layout->addWidget(lbl00,             1, 0, 1, 1);
    layout->addWidget(spinPctGpuMem,     1, 1, 1, 1);
    layout->addWidget(showCrops,         1, 2, 1, 1);
    layout->addWidget(loadModel,         1, 3, 1, 1);
    setLayout(layout);

    waitBox = new WaitBox(mainWindow);
    cropDialog = new CropDialog(mainWindow);
}

void FeatureModel::showCrops()
{
    cropDialog->show();
}

bool FeatureModel::infer(ImageFrame* frame)
{
    int ndims = 4;
    int batch_size = frame->crops.size();
    int64_t dims[] { batch_size, crop_height, crop_width, num_channels };
    int crop_size = crop_height * crop_width * num_channels;
    size_t ndata = crop_size * batch_size;

    uint8_t *buffer = (uint8_t*)malloc(ndata);
    for (int i = 0; i < batch_size; i++)
        memcpy(buffer + i * crop_size, frame->crops[i].data, crop_size);

    InputValues[0] = TF_NewTensor(TF_UINT8, dims, ndims, buffer, ndata, &NoOpDeallocator, NULL);
    if (InputValues[0] == nullptr) {
        std::cout << "ERROR failed TF_NewTensor" << std::endl;
        return false;
    }

    TF_SessionRun(Session, nullptr, Input, InputValues, NumInputs, Output, OutputValues, NumOutputs, nullptr, 0, nullptr, Status);
    if (TF_GetCode(Status) != TF_OK) {
        std::cout << "ERROR: " << TF_Message(Status);
        return false;
    }

    float *sig = (float*)TF_TensorData(OutputValues[0]);
    for (int i = 0; i < batch_size; i++) {
        std::vector<float> feature;
        for (int j = 0; j < feature_size; j++) {
            //cout << "batch_no: " << i << " index: " << j << " value: " << sig[i * feature_size + j] << endl;
            feature.push_back(sig[i * feature_size + j]);
        }
        //frames[current_index].features.push_back(feature);
        frame->features.push_back(feature);
    }

    free(buffer);
    TF_DeleteTensor(InputValues[0]);
    return true;
}

void FeatureModel::loaderCallback(int arg)
{
    loading = false;
}

void FeatureModel::load()
{
    std::cout << "FeatuerModel::load" << std::endl;
    loader = new FeatureModelLoader(this);
    connect(loader, SIGNAL(done(int)), waitBox, SLOT(done(int)));
    connect(loader, SIGNAL(done(int)), this, SLOT(loaderCallback(int)));
    QThreadPool::globalInstance()->start(loader);
    waitBox->exec();

}

bool FeatureModel::load(const QString& saved_model_dir, double pct_gpu_mem)
{
    bool result = false;
    Graph = TF_NewGraph();
    Status = TF_NewStatus();
    SessionOpts = CreateSessionOptions(pct_gpu_mem);
    RunOpts = nullptr;
    ntags = 1;

    NumInputs = 1;
    NumOutputs = 1;

    Session = TF_LoadSessionFromSavedModel(SessionOpts, RunOpts, saved_model_dir.toLatin1().data(), &tags, ntags, Graph, nullptr, Status);

    if (TF_GetCode(Status) == TF_OK) {
        std::cout << "Model loaded from " << saved_model_dir.toStdString() << " successfully" << std::endl;
        result = true;
    }
    else {
        std::cout << "Error loading model: " << TF_Message(Status) << std::endl;
        //freeModel();
        return false;
    }

    Input = (TF_Output *)malloc(sizeof(TF_Output) * NumInputs);
    Output = (TF_Output *)malloc(sizeof(TF_Output) * NumOutputs);

    InputValues = (TF_Tensor **)malloc(sizeof(TF_Tensor *) * NumInputs);
    OutputValues = (TF_Tensor **)malloc(sizeof(TF_Tensor *) * NumOutputs);

    Input[0] = {TF_GraphOperationByName(Graph, "images"), 0};
    Output[0] = {TF_GraphOperationByName(Graph, "features"), 0};

    cv::Mat dummy(crop_height, crop_width, CV_8UC3, cv::Scalar(0,0,0));
    ImageFrame frame;
    frame.crops.push_back(dummy);
    if (!infer(&frame))
        return false;

    std::cout << "model initialized successfully" << std::endl;
    initialized = true;
    return result;
}

TF_SessionOptions* FeatureModel::CreateSessionOptions(double percentage)
{
    //Xonxt commented on Jun 24, 2019 • https://github.com/Neargye/hello_tf_c_api/issues/21

    TF_Status* status = TF_NewStatus();
    TF_SessionOptions* options = TF_NewSessionOptions();

    uint8_t config[15] = { 0x32, 0xb, 0x9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x1, 0x38, 0x1 };

    uint8_t* bytes = reinterpret_cast<uint8_t*>(&percentage);

    for ( size_t i = 0; i < sizeof( percentage ); i++ ) {
        config[i + 3] = bytes[i];
    }

    TF_SetConfig( options, (void *) config, 15, status );

    if ( TF_GetCode( status ) != TF_OK ) {
        std::cerr << "Can't set options: " << TF_Message( status ) << std::endl;

        TF_DeleteStatus( status );
        return nullptr;
    }

    TF_DeleteStatus( status );
    return options;
}

void FeatureModel::setSavedModelDir(const QString& arg)
{
    savedModelDir = arg;
    MW->settings->setValue(savedModelKey, savedModelDir);
}

void FeatureModel::pctGpuMemChanged(int arg)
{
    pct_gpu_mem = arg / (double)100;
    MW->settings->setValue(pctGpuMemKey, pct_gpu_mem);
}

void FeatureModel::autoSave()
{

}

void FeatureModel::clear()
{
    if (Graph)        TF_DeleteGraph(Graph);
    if (Session)      TF_DeleteSession(Session, Status);
    if (SessionOpts)  TF_DeleteSessionOptions(SessionOpts);
    if (Status)       TF_DeleteStatus(Status);
    if (Input)        free(Input);
    if (Output)       free(Output);
    if (InputValues)  free(InputValues);
    if (OutputValues) free(OutputValues);
}

FeatureModel::~FeatureModel()
{
    clear();
}

FeatureModelLoader::FeatureModelLoader(QObject *featureModel)
{
    this->featureModel = featureModel;
}

void FeatureModelLoader::run()
{
    FM->load(FM->savedModelDir, FM->pct_gpu_mem);
    emit done(0);
}

CropDialog::CropDialog(QMainWindow *parent) : PanelDialog(parent)
{
    setWindowTitle("Crop Dialog");
    lblImage = new QLabel();
    lblImage->setMinimumWidth(640);
    lblImage->setMinimumHeight(1024);
    QGridLayout *panelLayout = new QGridLayout();
    panelLayout->addWidget(lblImage,   0, 0, 1, 1);
    panel = new Panel(mainWindow);
    panel->setLayout(panelLayout);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(panel);
    setLayout(layout);
}
