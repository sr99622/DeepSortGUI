#include "checkbox.h"

CheckBox::CheckBox(const QString& text, QSettings *settings, const QString& settingsKey) : QCheckBox(text)
{
    this->settings = settings;
    this->settingsKey = settingsKey;

    if (settings->contains(settingsKey))
        setChecked(settings->value(settingsKey).toBool());

    connect(this, SIGNAL(clicked(bool)), this, SLOT(checkSet(bool)));

}

void CheckBox::checkSet(bool arg)
{
    settings->setValue(settingsKey, arg);
}
