#ifndef TESTER_H
#define TESTER_H

#include <QMainWindow>
#include "Filters/filter.h"
#include "Models/yolo.h"

class Tester : public Filter
{
    Q_OBJECT

public:
    Tester(QMainWindow *parent);
    void filter(Frame *vp) override;

    QMainWindow *mainWindow;
    Yolo *yolo;

};

#endif // TESTER_H
