#include "tester.h"
#include "mainwindow.h"

Tester::Tester(QMainWindow *parent)
{
    mainWindow = parent;
    name = "Tester";
    panel = new Panel(mainWindow);
    yolo = new Yolo(mainWindow, "Tester");

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(yolo, 0, 0, 1, 1);
    panel->setLayout(layout);
}

void Tester::filter(Frame *vp)
{
    cv::Mat image = vp->toMat();
    if (!yolo->started) {
        yolo->started = true;
        yolo->loading = true;
        yolo->loadModel();
    }

    if (!yolo->loading) {
        std::vector<bbox_t> results = yolo->detector->detect(image);
        std::cout << "results size: " << results.size() << std::endl;
    }

    vp->readMat(image);
}
