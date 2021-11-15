#ifndef COUNTER_H
#define COUNTER_H

#include <QMainWindow>
#include <QRunnable>
#include "Filters/filter.h"
#include "Filters/darknet.h"
#include "Filters/Counter/countpanel.h"
#include "Filters/Counter/counterframe.h"

class CounterRunner_0 : public QObject, public QRunnable
{
    Q_OBJECT

public:
    CounterRunner_0(QObject *parent) {
        counter = parent;
        setAutoDelete(false);
    };
    void run() override;
    bool finished;
    QLabel *lblElapsed = new QLabel();

    QObject *counter;

};

class CounterRunner_1 : public QObject, public QRunnable
{
    Q_OBJECT

public:
    CounterRunner_1(QObject *parent) {
        counter = parent;
        setAutoDelete(false);
    };
    void run() override;
    bool finished;
    QLabel *lblElapsed = new QLabel();

    QObject *counter;

};

class Counter : public Filter
{
    Q_OBJECT

public:
    Counter(QMainWindow *parent);
    void filter(Frame *vp) override;
    void autoSave() override;
    bool threadsFinished();

    QMainWindow *mainWindow;
    Darknet *darknet;
    CountPanel *countPanel;

    CounterFrame counterFrames[2];
    CounterRunner_0 *runner_0;
    CounterRunner_1 *runner_1;

};

#endif // COUNTER_H
