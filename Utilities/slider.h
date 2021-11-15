#ifndef SLIDER_H
#define SLIDER_H

#include <iostream>
#include <QSlider>
#include <QLabel>
#include <QSettings>
#include <QGridLayout>

class Slider : public QWidget
{
    Q_OBJECT

public:
    Slider(Qt::Orientation orientation, QSettings *settings, const QString& settingsKey, const QString& displayName, QWidget *parent);

    QSlider *slider;
    QLabel *lblDisplay;
    QLabel *lblValue;
    QSettings *settings;
    QString settingsKey;

public slots:
    void updateValue(int);
    void setValue(int);

};

#endif // SLIDER_H
