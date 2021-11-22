#ifndef CROPDIALOG_H
#define CROPDIALOG_H

#include <QMainWindow>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include "Utilities/paneldialog.h"
#include "Utilities/spinbox.h"

class CropDialog : public PanelDialog
{
    Q_OBJECT

public:
    CropDialog(QMainWindow *parent);

    QLabel *lblImage;

};

class CropParam : public QWidget
{
    Q_OBJECT

public:
    CropParam(QMainWindow *parent, QObject *cropDialog);
    int findAssignment(int track_id);
    void initializeTrackAssignment();

    QMainWindow *mainWindow;
    QObject *cropDialog;
    QPushButton *btnShow;
    SpinBox *spinCols;
    SpinBox *spinRows;

    std::vector<int> trackAssignments;
    std::vector<int> expiredTrackIds;

public slots:
    void showDialog();
    void adjustDialog(int);

};


#endif // CROPDIALOG_H
