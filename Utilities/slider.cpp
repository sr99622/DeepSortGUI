#include "slider.h"

Slider::Slider(Qt::Orientation orientation, QSettings *settings, const QString& settingsKey, const QString& displayName, QWidget *parent) : QWidget(parent)
{
    this->settings = settings;
    this->settingsKey = settingsKey;
    lblDisplay = new QLabel(displayName);
    lblValue = new QLabel();
    slider = new QSlider(orientation, parent);
    slider->setTracking(false);
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(updateValue(int)));
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    slider->setValue(settings->value(settingsKey, 20).toInt());

    QGridLayout *layout = new QGridLayout();
    if (orientation == Qt::Horizontal) {
        layout->addWidget(lblDisplay,   0, 0, 1, 1);
        layout->addWidget(slider,       0, 1, 1, 1);
        layout->addWidget(lblValue,     0, 2, 1, 1);
        layout->setColumnStretch(1, 10);
    }
    else {
        layout->addWidget(lblValue,     0, 0, 1, 1);
        layout->addWidget(slider,       1, 0, 1, 1);
        layout->addWidget(lblDisplay,   2, 0, 1, 1);
        layout->setRowStretch(1, 10);
    }
    setLayout(layout);
}

void Slider::updateValue(int arg)
{
    lblValue->setText(QString::number(arg));
}

void Slider::setValue(int arg)
{
    lblValue->setText(QString::number(arg));
    settings->setValue(settingsKey, arg);
}
