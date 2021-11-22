#ifndef CHECKBOX_H
#define CHECKBOX_H

#include <QCheckBox>
#include <QSettings>

class CheckBox : public QCheckBox
{
    Q_OBJECT

public:
    CheckBox(const QString& text, QSettings *settings, const QString& settingsKey);

    QSettings *settings;
    QString settingsKey;

public slots:
    void checkSet(bool);

};

#endif // CHECKBOX_H
