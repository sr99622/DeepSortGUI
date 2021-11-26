#ifndef FOCUSDIALOG_H
#define FOCUSDIALOG_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

#include "Utilities/paneldialog.h"
#include "Utilities/numbertextbox.h"

class FocusParam : public QWidget
{
    Q_OBJECT

public:
    FocusParam(QObject *focusDialog);

    QObject *focusDialog;
    QMainWindow *mainWindow;
    NumberTextBox *txtCropWidth;
    NumberTextBox *txtCropHeight;
    QPushButton *btnShow;

public slots:
    void showDialog();

};

class FocusDialog : public PanelDialog
{
    Q_OBJECT

public:
    FocusDialog(QMainWindow *parent);

    QLabel *lblImage;
    FocusParam *param;

};

#endif // FOCUSDIALOG_H
