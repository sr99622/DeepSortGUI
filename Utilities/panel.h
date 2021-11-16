#ifndef PANEL_H
#define PANEL_H

#include <QMainWindow>

class Panel : public QWidget
{
    Q_OBJECT

public:
    Panel(QMainWindow *parent);
    Panel(QMainWindow *parent, QDialog *dlg);

    virtual void autoSave();
    bool changed = false;

    QMainWindow *mainWindow;
    QDialog *dlg;

};

#endif // PANEL_H
