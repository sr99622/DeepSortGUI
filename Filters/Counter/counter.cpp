#include "counter.h"
#include "mainwindow.h"

Counter::Counter(QMainWindow *parent)
{
    mainWindow = parent;
    name = "Counter";
    panel = new Panel(mainWindow);
    yolo = new Yolo(mainWindow, "Counter");
    countPanel = new CountPanel(mainWindow, yolo);

    runner_0 = new CounterRunner_0(this);
    runner_1 = new CounterRunner_1(this);

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(yolo,                    0, 0, 1, 4);
    layout->addWidget(new QLabel("elapsed:"),  1, 2, 1, 1);
    layout->addWidget(runner_0->lblElapsed,    1, 3, 1, 1);
    layout->addWidget(countPanel,              2, 0, 1, 4);
    layout->addWidget(new QLabel("elapsed:"),  3, 2, 1, 1);
    layout->addWidget(runner_1->lblElapsed,    3, 3, 1, 1);
    panel->setLayout(layout);

}

void Counter::filter(Frame *vp)
{
    if (!yolo->started) {
        yolo->started = true;
        yolo->loading = true;
        yolo->loadModel();
    }

    if (!yolo->loading) {
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

    Counter *C = (Counter*)counter;
    C->counterFrames[0].detections = C->yolo->detector->detect(C->counterFrames[0].image);

    finished = true;

    auto stop = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    lblElapsed->setText(QString::number(elapsed));
}

void CounterRunner_1::run()
{
    auto start = std::chrono::high_resolution_clock::now();

    Counter *C = (Counter*)counter;
    C->countPanel->feed(C->counterFrames[1]);

    finished = true;

    auto stop = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    lblElapsed->setText(QString::number(elapsed));
}

