#include "cropdialog.h"
#include "mainwindow.h"
#include "imageframe.h"

CropDialog::CropDialog(QMainWindow *parent) : PanelDialog(parent)
{
    setWindowTitle("Crop Dialog");
    lblImage = new QLabel();
    lblImage->setMinimumWidth(640);
    lblImage->setMinimumHeight(480);
    QGridLayout *panelLayout = new QGridLayout();
    panelLayout->addWidget(lblImage,   0, 0, 1, 1);
    panel = new Panel(mainWindow);
    panel->setLayout(panelLayout);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(panel);
    setLayout(layout);

    settingsKey = "CropDialog/geometry";
}

CropParam::CropParam(QMainWindow *parent, QObject *cropDialog)
{
    mainWindow = parent;
    this->cropDialog = cropDialog;
    btnShow = new QPushButton("Show");
    btnShow->setMaximumWidth(btnShow->fontMetrics().boundingRect(btnShow->text()).width() * 1.5);
    connect(btnShow, SIGNAL(clicked()), this, SLOT(showDialog()));

    spinCols = new SpinBox(MW->settings, "CropParam/spinCols", "Cols");
    connect(spinCols, SIGNAL(valueChanged(int)), this, SLOT(adjustDailog(int)));
    spinRows = new SpinBox(MW->settings, "CropParam/spinRows", "Rows");
    connect(spinRows, SIGNAL(valueChanged(int)), this, SLOT(adjustDailog(int)));

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(spinCols->lbl,    0, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(spinCols,         0, 1, 1, 1);
    layout->addWidget(spinRows->lbl,    0, 2, 1, 1, Qt::AlignRight);
    layout->addWidget(spinRows,         0, 3, 1, 1);
    layout->addWidget(btnShow,          0, 4, 1, 1);

    QGroupBox *groupBox = new QGroupBox("Crop Dialog Parameters");
    groupBox->setLayout(layout);
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(groupBox);
    setLayout(mainLayout);
}

void CropParam::adjustDialog(int arg)
{
    Q_UNUSED(arg);
    CropDialog *cp = (CropDialog*)cropDialog;
    cp->lblImage->setMinimumWidth(spinCols->value() * crop_width);
    cp->lblImage->setMaximumWidth(spinCols->value() * crop_width);
    cp->lblImage->setMinimumHeight(spinRows->value() * crop_height);
    cp->lblImage->setMaximumHeight(spinRows->value() * crop_height);
    /*
    if (cp->isVisible()) {
        cp->hide();
        cp->show();
    }
    */
}

void CropParam::showDialog()
{
    adjustDialog(0);
    ((CropDialog*)cropDialog)->show();
}
