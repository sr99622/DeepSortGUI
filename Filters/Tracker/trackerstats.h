#ifndef TRACKERSTATS_H
#define TRACKERSTATS_H

#include <QMainWindow>
#include <QGridLayout>
#include "Utilities/paneldialog.h"


class TrackerStats : public Panel
{
    Q_OBJECT

public:
    TrackerStats(QMainWindow *parent, QDialog *dlg);


};

class TrackerStatsDialog : public PanelDialog
{
    Q_OBJECT

public:
    TrackerStatsDialog(QMainWindow *parent, const QString& settingsKey);

};

#endif // TRACKERSTATS_H
