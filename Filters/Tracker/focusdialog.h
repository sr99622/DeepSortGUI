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
    FocusParam(PanelDialog *focusDialog);

    PanelDialog *focusDialog;
    QMainWindow *mainWindow;
    NumberTextBox *txtCropWidth;
    NumberTextBox *txtCropHeight;
    QPushButton *btnShow;

public slots:
    void showDialog();
    void widthUp();

};

class FocusDialog : public PanelDialog
{
    Q_OBJECT

public:
    FocusDialog(QMainWindow *parent);

    QLabel *lblImage;
    FocusParam *param;

protected:
    void wheelEvent(QWheelEvent *event) override;

};

#endif // FOCUSDIALOG_H
