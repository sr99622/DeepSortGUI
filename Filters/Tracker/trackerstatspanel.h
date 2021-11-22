#ifndef TRACKERSTATSPANEL_H
#define TRACKERSTATSPANEL_H

#include <iostream>
#include <QMainWindow>
#include <QGridLayout>
#include <QLabel>
#include "Utilities/paneldialog.h"
#include "Utilities/texthistogram.h"
#include "DeepSort/matching/tracker.h"

class TrackerStatsPanel : public Panel
{
    Q_OBJECT

public:
    TrackerStatsPanel(QMainWindow *parent);
    void populate(tracker* mytracker);
    std::map<QString, QLabel> labels;
    TextHistogram *histTrackerHits;
    TextHistogram *histTrackerAge;
    TextHistogram *histTrackerTsu;
    TextHistogram *histTrackerAgeAtDelete;

    QWidget *panelHistogram;
    QLabel *lblHistogram;
    QLabel *lblBuckets;

    std::vector<int> tracker_age_at_delete;

};

class TrackerStatsDialog : public PanelDialog
{
    Q_OBJECT

public:
    TrackerStatsDialog(QMainWindow *parent);

};

#endif // TRACKERSTATSPANEL_H
