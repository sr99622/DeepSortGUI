#include "filesetter.h"

#include <QFileDialog>
#include <QGridLayout>
#include <iostream>

using namespace std;

FileSetter::FileSetter(QMainWindow *parent, const QString& labelText, const QString& filter)
{
    setup(parent, labelText, filter);
}

FileSetter::FileSetter(QMainWindow *parent, const QString& labelText, const QString& filter,
                       QSettings *settings, const QString& settingsKey)
{
    setup(parent, labelText, filter);
    this->settings = settings;
    this->settingsKey = settingsKey;
    setPath(settings->value(settingsKey).toString());
    emit fileSet(filename);
}

void FileSetter::setup(QMainWindow *parent, const QString& labelText, const QString& filter)
{
    mainWindow = parent;
    this->filter = filter;
    label = new QLabel(labelText);
    text = new QLineEdit();
    button = new QPushButton("...");
    button->setMaximumWidth(30);
    connect(button, SIGNAL(clicked()), this, SLOT(selectFile()));

    QGridLayout *layout = new QGridLayout;
    if (label->text() != "")
        layout->addWidget(label,  0, 0, 1, 1);
    layout->addWidget(text,       0, 1, 1, 1);
    layout->addWidget(button,     0, 2, 1, 1);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    setContentsMargins(0, 0, 0, 0);
}

QString FileSetter::path() const
{
    return filename;
}

void FileSetter::setPath(const QString& path)
{
    filename = path;
    text->setText(path);
}

void FileSetter::selectFile()
{
    if (text->text().length() > 0) {
        defaultPath = text->text();
    }
    else {
        if (defaultPath.length() == 0) {
            defaultPath = QDir::homePath();
        }
    }

    QString path = QFileDialog::getOpenFileName(mainWindow, label->text(), defaultPath, filter);
    if (path.length() > 0) {
        setPath(path);
        if (settings)
            settings->setValue(settingsKey, filename);
        emit fileSet(filename);
    }
}

