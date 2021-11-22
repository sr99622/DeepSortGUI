#ifndef TEXTHISTOGRAM_H
#define TEXTHISTOGRAM_H

#include <iostream>
#include <QTextStream>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSettings>

#include "Utilities/numbertextbox.h"
#include "Utilities/checkbox.h"

class TextHistogram : public QWidget
{
    Q_OBJECT

public:
    TextHistogram(const QString& title, QSettings *settings, const QString& settingsKey);
    void histogram(const std::vector<int>& data);

    int number_of_buckets;
    int max_data_value;

    QLabel *lblBuckets;
    QLabel *lblHistogram;
    QLabel *lblHeights;
    QLabel *lblTitle;

    NumberTextBox *txtMaxValue;
    CheckBox *chkAutoMax;
    CheckBox *chkEnabled;
    QSettings *settings;
    QString settingsKey;

public slots:
    void autoMaxChecked(bool);

};


#endif // TEXTHISTOGRAM_H
