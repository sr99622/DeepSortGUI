#ifndef FILESETTER_H
#define FILESETTER_H

#include <QMainWindow>
#include <QSettings>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class FileSetter : public QWidget
{
    Q_OBJECT

public:
    FileSetter(QMainWindow *parent, const QString& labelText, const QString& filter);
    FileSetter(QMainWindow *parent, const QString& labelText, const QString& filter,
               QSettings *settings, const QString& settingsKey);

    void setup(QMainWindow *parent, const QString& labelText, const QString& filter);
    void setPath(const QString& path);
    QString path() const;

    QLabel *label;
    QLineEdit *text;
    QPushButton *button;
    QString filename;
    QString filter;
    QString defaultPath;

    QMainWindow *mainWindow;
    QSettings *settings = nullptr;
    QString settingsKey;

signals:
    void fileSet(const QString&);

public slots:
    void selectFile();

};

#endif // FILESETTER_H
