#include "counter.h"
#include "mainwindow.h"

#define C ((Counter*)counter)

Counter::Counter(QMainWindow *parent)
{
    mainWindow = parent;
    name = "Counter";
    panel = new Panel(mainWindow);
    darknet = new Darknet(mainWindow, "Counter");
    countPanel = new CountPanel(mainWindow, darknet);

    runner_0 = new CounterRunner_0(this);
    runner_1 = new CounterRunner_1(this);

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(darknet->panel,          0, 0, 1, 4);
    layout->addWidget(new QLabel("elapsed:"),  1, 2, 1, 1);
    layout->addWidget(runner_0->lblElapsed,    1, 3, 1, 1);
    layout->addWidget(countPanel,              2, 0, 1, 4);
    layout->addWidget(new QLabel("elapsed:"),  3, 2, 1, 1);
    layout->addWidget(runner_1->lblElapsed,    3, 3, 1, 1);
    panel->setLayout(layout);

}

void Counter::filter(Frame *vp)
{
    if (darknet->model == nullptr) {
        darknet->loading = true;
        darknet->model = new DarknetModel(mainWindow, darknet);
        darknet->model->initialize(darknet->cfg->filename, darknet->weights->filename, darknet->names->filename);
    }

    if (!darknet->loading) {
        counterFrames[1] = counterFrames[0];
        counterFrames[0].clear();
        counterFrames[0].image = vp->toMat();

        runner_0->finished = false;
        QThreadPool::globalInstance()->tryStart(runner_0);
        runner_1->finished = false;
        QThreadPool::globalInstance()->tryStart(runner_1);

        while (!threadsFinished())
            QThread::msleep(1);

        if (counterFrames[1].image.rows > 0)
            vp->readMat(counterFrames[1].image);
    }
}

void Counter::autoSave()
{

}

bool Counter::threadsFinished()
{
    return runner_0->finished && runner_1->finished;
}

void CounterRunner_0::run()
{
    auto start = std::chrono::high_resolution_clock::now();

    //QThread::msleep(10);
    C->counterFrames[0].detections = C->darknet->model->detector->detect(C->counterFrames[0].image);

    finished = true;

    auto stop = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    lblElapsed->setText(QString::number(elapsed));
}

void CounterRunner_1::run()
{
    auto start = std::chrono::high_resolution_clock::now();

    //QThread::msleep(10);
    //C->countPanel->feed(C->counterFrames[1].detections);
    C->countPanel->feed(C->counterFrames[1]);

    finished = true;

    auto stop = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    lblElapsed->setText(QString::number(elapsed));
}

