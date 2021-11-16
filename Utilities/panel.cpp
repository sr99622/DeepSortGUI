#include "panel.h"

Panel::Panel(QMainWindow *parent)
{
    mainWindow = parent;
}

Panel::Panel(QMainWindow *parent, QDialog *dlg)
{
    mainWindow = parent;
    this->dlg = dlg;
}

void Panel::autoSave()
{

}
