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

}
