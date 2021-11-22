#include "trackerstatspanel.h"
#include "mainwindow.h"
#include <QTextStream>

TrackerStatsPanel::TrackerStatsPanel(QMainWindow *parent) : Panel(parent)
{
    labels.insert(std::make_pair("Confirmed Tracks", new QLabel()));
    labels.insert(std::make_pair("Tentative Tracks", new QLabel()));
    labels.insert(std::make_pair("Total Tracks", new QLabel()));
    labels.insert(std::make_pair("Tracks Initiated", new QLabel()));
    labels.insert(std::make_pair("Tracks Deleted", new QLabel()));
    labels.insert(std::make_pair("n init", new QLabel()));
    labels.insert(std::make_pair("max age", new QLabel()));

    std::map<QString, QLabel>:: iterator label;
    size_t i = 0;

    QGridLayout *layout = new QGridLayout();
    int num_label_cols = 3;
    for (label =  labels.begin(); label !=  labels.end(); ++label) {
        int y = i / num_label_cols;
        int x = i % num_label_cols;

        std::cout << "x: " << x << " y: " << y << std::endl;

        layout->addWidget(new QLabel(label->first),     i, 0, 1, 1);
        layout->addWidget(&label->second,               i, 1, 1, 1);
        i++;
    }

    histTrackerAgeAtDelete = new TextHistogram("Delete Age", MW->settings, "histTrackerAgeAtDelete");

    layout->addWidget(histTrackerAgeAtDelete,  0, 2, i, 2);

    histTrackerHits = new TextHistogram("Tracker Hits", MW->settings, "histTrackerHits");
    histTrackerAge = new TextHistogram("Tracker Age", MW->settings, "histTrackerAge");
    histTrackerTsu = new TextHistogram("Time Since Update", MW->settings, "histTrackerTsu");

    layout->addWidget(histTrackerHits,    i, 0, 1, 2);
    layout->addWidget(histTrackerAge,     i, 2, 1, 2);
    layout->addWidget(histTrackerTsu,     i, 4, 1, 2);

    setLayout(layout);
}

void TrackerStatsPanel::populate(tracker* mytracker)
{
    labels["Total Tracks"].setText(QString::number(mytracker->tracks.size()));

    size_t number_of_confirmed_tracks = 0;
    size_t number_of_tentative_tracks = 0;
    for (Track& track : mytracker->tracks) {
        if (track.is_confirmed())
            number_of_confirmed_tracks++;
        if (track.is_tentative())
            number_of_tentative_tracks++;
    }

    labels["Confirmed Tracks"].setText(QString::number(number_of_confirmed_tracks));
    labels["Tentative Tracks"].setText(QString::number(number_of_tentative_tracks));

    std::vector<int> tracker_hits;
    std::vector<int> tracker_age;
    std::vector<int> tracker_tsu;

    size_t new_tracker_count = 0;
    size_t expired_tracker_count = 0;

    for (Track& track : mytracker->tracks) {
        tracker_hits.push_back(track.hits);
        tracker_age.push_back(track.age);
        tracker_tsu.push_back(track.time_since_update);

        if (track.age == 1) {
            new_tracker_count++;
        }

        if (track.time_since_update == track._max_age) {
            expired_tracker_count++;
            tracker_age_at_delete.push_back(track.age);
        }
    }

    histTrackerHits->histogram(tracker_hits);
    histTrackerAge->histogram(tracker_age);
    histTrackerTsu->histogram(tracker_tsu);
    histTrackerAgeAtDelete->histogram(tracker_age_at_delete);

    labels["Tracks Initiated"].setText(QString::number(new_tracker_count));
    labels["Tracks Deleted"].setText(QString::number(expired_tracker_count));
    labels["n init"].setText(QString::number(mytracker->n_init));
    labels["max age"].setText(QString::number(mytracker->max_age));
}

TrackerStatsDialog::TrackerStatsDialog(QMainWindow *parent) : PanelDialog(parent)
{
    setWindowTitle("Tracker Stats");
    panel = new TrackerStatsPanel(mainWindow);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(panel);
    setLayout(layout);

    defaultWidth = 640;
    defaultHeight = 480;
    settingsKey = "TrackerStatsDialog/geometry";
}
