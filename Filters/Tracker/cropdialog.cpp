#include "cropdialog.h"
#include "mainwindow.h"
#include "imageframe.h"

CropDialog::CropDialog(QMainWindow *parent) : PanelDialog(parent)
{
    setWindowTitle("Crop Dialog");
    lblImage = new QLabel();
    lblImage->setMinimumWidth(640);
    lblImage->setMinimumHeight(480);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(lblImage);
    layout->setContentsMargins(0, 0, 0, 0);
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
    connect(spinCols, SIGNAL(valueChanged(int)), this, SLOT(adjustDialog(int)));
    spinRows = new SpinBox(MW->settings, "CropParam/spinRows", "Rows");
    connect(spinRows, SIGNAL(valueChanged(int)), this, SLOT(adjustDialog(int)));

    initializeTrackAssignment();

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

void CropParam::initializeTrackAssignment()
{
    size_t num_crops = spinRows->value() * spinCols->value();
    if (num_crops > trackAssignments.size()) {
        for (size_t i = trackAssignments.size(); i < num_crops; i++)
            trackAssignments.push_back(-1);
    }
    else {
        while (trackAssignments.size() > num_crops)
            trackAssignments.erase(trackAssignments.end() - 1);
    }
}

int CropParam::findAssignment(int track_id)
{
    int result = -1;
    auto position = std::find(trackAssignments.begin(), trackAssignments.end(), track_id);
    if (position != trackAssignments.end())
        result = position - trackAssignments.begin();
    return result;
}

void CropParam::adjustDialog(int arg)
{
    Q_UNUSED(arg);
    CropDialog *cp = (CropDialog*)cropDialog;
    int width = spinCols->value() * crop_width;
    int height = spinRows->value() * crop_height;
    cp->lblImage->setFixedWidth(width);
    cp->lblImage->setFixedHeight(height);
    QMargins margins = cp->contentsMargins();
    cp->setFixedWidth(width + margins.right() + margins.left());
    cp->setFixedHeight(height + margins.top() + margins.bottom());
    initializeTrackAssignment();
}

void CropParam::showDialog()
{
    adjustDialog(0);
    ((CropDialog*)cropDialog)->show();
}
