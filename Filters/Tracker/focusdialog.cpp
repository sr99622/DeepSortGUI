#include "focusdialog.h"
#include "mainwindow.h"

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

FocusParam::FocusParam(QObject *focusDialog)
{
    this->focusDialog = focusDialog;
    mainWindow = ((FocusDialog*)focusDialog)->mainWindow;

    txtCropWidth = new NumberTextBox(MW->settings, "FocusParam/txtCropWidth", INTEGER_NUMBER, "crop width");
    if (txtCropWidth->text().length() == 0) txtCropWidth->setIntValue(320);
    txtCropHeight = new NumberTextBox(MW->settings, "FocusParam/txtCropHeight", INTEGER_NUMBER, "crop height");
    if (txtCropHeight->text().length() == 0) txtCropHeight->setIntValue(320);

    btnShow = new QPushButton("Show");
    btnShow->setMaximumWidth(btnShow->fontMetrics().boundingRect(btnShow->text()).width() * 1.5);
    connect(btnShow, SIGNAL(clicked()), this, SLOT(showDialog()));
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(txtCropWidth->lbl,    0, 0, 1, 1);
    layout->addWidget(txtCropWidth,         0, 1, 1, 1);
    layout->addWidget(txtCropHeight->lbl,   0, 2, 1, 1);
    layout->addWidget(txtCropHeight,        0, 3, 1, 1);
    layout->addWidget(btnShow,              0, 4, 1, 1);

    QGroupBox *groupBox = new QGroupBox("Focus Dialog Parameters");
    groupBox->setLayout(layout);
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(groupBox);
    setLayout(mainLayout);
    setContentsMargins(0, 0, 0, 0);

}

void FocusParam::showDialog()
{
    ((FocusDialog*)focusDialog)->show();
}

