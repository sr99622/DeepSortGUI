#ifndef SPINBOX_H
#define SPINBOX_H

#include <iostream>
#include <QSpinBox>
#include <QSettings>
#include <QLabel>

class SpinBox : public QSpinBox
{
    Q_OBJECT

public:
    SpinBox(QSettings *settings, const QString& settingsKey, const QString& displayName);

    QSettings *settings;
    QString settingsKey;
    QLabel *lbl;

public slots:
    void valueChanged(int);
};

#endif // SPINBOX_H
