#include "focusdialog.h"
#include "mainwindow.h"
#include "Filters/subpicture.h"

FocusDialog::FocusDialog(QMainWindow *parent) : PanelDialog(parent)
{
    setWindowTitle("Focus Dialog");
    lblImage = new QLabel();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(lblImage);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    setContentsMargins(0, 0, 0, 0);

    param = new FocusParam(this);

    defaultWidth = 320;
    defaultHeight = 320;
    settingsKey = "FocusDialog/geometry";
}

FocusParam::FocusParam(PanelDialog *focusDialog)
{
    this->focusDialog = focusDialog;
    mainWindow = ((FocusDialog*)focusDialog)->mainWindow;

    txtCropWidth = new NumberTextBox(MW->settings, "FocusParam/txtCropWidth", INTEGER_NUMBER, "crop width");
    if (txtCropWidth->text().length() == 0) txtCropWidth->setIntValue(320);
    txtCropHeight = new NumberTextBox(MW->settings, "FocusParam/txtCropHeight", INTEGER_NUMBER, "crop height");
    if (txtCropHeight->text().length() == 0) txtCropHeight->setIntValue(320);

    QPushButton *btnWidthUp = new QPushButton("^");
    btnWidthUp->setFixedWidth(25);
    connect(btnWidthUp, SIGNAL(clicked()), this, SLOT(widthUp()));
    QPushButton *btnWidthDown = new QPushButton("v");
    btnWidthDown->setFixedWidth(25);
    QPushButton *btnHeightUp = new QPushButton("^");
    btnHeightUp->setFixedWidth(25);
    QPushButton *btnHeightDown = new QPushButton("v");
    btnHeightDown->setFixedWidth(25);
    QCheckBox *chkLockAspectRatio = new QCheckBox("LAR");

    btnShow = new QPushButton("Show");
    btnShow->setMaximumWidth(btnShow->fontMetrics().boundingRect(btnShow->text()).width() * 1.5);
    connect(btnShow, SIGNAL(clicked()), this, SLOT(showDialog()));
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(txtCropWidth->lbl,    0, 0, 1, 2);
    layout->addWidget(txtCropWidth,         0, 2, 1, 2);
    layout->addWidget(txtCropHeight->lbl,   0, 4, 1, 2);
    layout->addWidget(txtCropHeight,        0, 6, 1, 2);
    layout->addWidget(btnShow,              0, 8, 1, 2);
    layout->addWidget(btnWidthDown,         1, 0, 1, 1, Qt::AlignRight);
    layout->addWidget(btnWidthUp,           1, 1, 1, 1, Qt::AlignLeft);
    layout->addWidget(btnHeightDown,        1, 4, 1, 1, Qt::AlignRight);
    layout->addWidget(btnHeightUp,          1, 5, 1, 1, Qt::AlignLeft);
    layout->addWidget(chkLockAspectRatio,   1, 9, 1, 1);


    QGroupBox *groupBox = new QGroupBox("Focus Dialog Parameters");
    groupBox->setLayout(layout);
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(groupBox);
    setLayout(mainLayout);
    setContentsMargins(0, 0, 0, 0);

}

void FocusParam::widthUp()
{
    int width = -1;
    int height = -1;

    SubPicture *subPicture = (SubPicture*)MW->filter()->getFilterByName("Sub Picture");
    if (MW->filter()->isFilterActive(subPicture) && MW->control()->engageFilter->isChecked()) {
        width = subPicture->w;
        height = subPicture->h;
    }
    else if (MW->is) {
        width = MW->is->viddec.avctx->width;
        height = MW->is->viddec.avctx->height;
    }

    std::cout << "FocusParam::widthUp width: " << width << " height: " << height << std::endl;
}

void FocusParam::showDialog()
{
    ((FocusDialog*)focusDialog)->show();
}

void FocusDialog::wheelEvent(QWheelEvent *event)
{
    QPointF point = event->position();
    std::cout << "wheel position x: " << point.x() << " y: " << point.y() << std::endl;
}
