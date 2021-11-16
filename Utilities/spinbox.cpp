#include "spinbox.h"

SpinBox::SpinBox(QSettings *settings, const QString& settingsKey, const QString& displayName)
{
    this->settings = settings;
    this->settingsKey = settingsKey;
    lbl = new QLabel(displayName);
    setValue(settings->value(settingsKey, 1).toInt());
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
}

void SpinBox::valueChanged(int arg)
{
    settings->setValue(settingsKey, arg);
}
