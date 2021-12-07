/*******************************************************************************
* filterchain.cpp
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

#include "filterchain.h"
#include "mainwindow.h"

FilterChain::FilterChain(QMainWindow *parent)
{
    mainWindow = parent;
    fp = new Frame();
}

FilterChain::~FilterChain()
{
}

void FilterChain::process(Frame *vp)
{
    auto start = std::chrono::high_resolution_clock::now();

    if (!counting) {
        t1 = start;
        count = 0;
        counting = true;
    }
    else {
        count++;
    }

    this->vp = vp;

    if (!MW->is->paused) {
        fp->copy(vp);
    }
    else {
        bool found = false;
        Frame *tmp = MW->is->pictq.peek_next();
        if (tmp) {
            if (tmp->frame->width) {
                fp->copy(tmp);
                found = true;
            }
        }
        if (!found) {
            Frame *tmp = MW->is->pictq.peek_last();
            if (tmp) {
                if (tmp->frame->width) {
                    fp->copy(tmp);
                }
            }
        }
    }

    FilterPanel *panel = MW->filter();

    if (panel->engageFilter->isChecked() && !disengaged) {
        for (int i = 0; i < panel->leftModel->filters.size(); i++)
            panel->leftModel->filters[i]->filter(vp);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    long msec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();

    long interval = std::chrono::duration_cast<std::chrono::milliseconds>(stop - t1).count();
    if (interval > 1000) {
        counting = false;
        float fps = 1000 * count / (float)interval;
        char buf[64];
        sprintf(buf, "%0.2f", fps);
        panel->fps->setText(buf);
    }

    char buf[64];
    sprintf(buf, "%d", msec);
    panel->filterTime->setText(buf);
}

