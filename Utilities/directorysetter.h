/*******************************************************************************
* directorysetter.h
*
* Copyright (c) 2020 Stephen Rhodes
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
*******************************************************************************/

#ifndef DIRECTORYSETTER_H
#define DIRECTORYSETTER_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMainWindow>
#include <QSettings>

class DirectorySetter : public QWidget
{
    Q_OBJECT

public:
    DirectorySetter(QMainWindow *parent, const QString& labelText);
    DirectorySetter(QMainWindow *parent, const QString& labelText, QSettings *settings, const QString& settingsKey);
    void setPath(const QString& path);
    QString path() const;

    QLabel *label;
    QLineEdit *text;
    QPushButton *button;
    QString directory;

    QMainWindow *mainWindow;
    QSettings *settings = nullptr;
    QString settingsKey;

signals:
    void directorySet(const QString&);

public slots:
    void selectDirectory();

};

#endif // DIRECTORYSETTER_H
