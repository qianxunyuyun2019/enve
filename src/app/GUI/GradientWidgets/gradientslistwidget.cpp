// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "gradientslistwidget.h"
#include <QScrollBar>
#include <QResizeEvent>
#include "displayedgradientswidget.h"

GradientsListWidget::GradientsListWidget(QWidget *parent) :
    ScrollArea(parent) {
    setFixedHeight(6*MIN_WIDGET_DIM);
    verticalScrollBar()->setSingleStep(MIN_WIDGET_DIM);
    mDisplayedGradients = new DisplayedGradientsWidget(this);
    setWidget(mDisplayedGradients);
}

DisplayedGradientsWidget *GradientsListWidget::getList() {
    return mDisplayedGradients;
}

void GradientsListWidget::scrollContentsBy(int dx, int dy) {
    mDisplayedGradients->incTop(dy);
    QScrollArea::scrollContentsBy(dx, dy);
}

void GradientsListWidget::resizeEvent(QResizeEvent *e) {
    const QSize size = e->size();
    mDisplayedGradients->setMinimumHeight(size.height());
    mDisplayedGradients->updateHeight();
    const int scrollBarWidth = verticalScrollBar()->width();
    const int availableWidth = size.width() - scrollBarWidth;
    mDisplayedGradients->setFixedWidth(availableWidth);
}

void GradientsListWidget::showEvent(QShowEvent *e) {
    Q_UNUSED(e)
    mDisplayedGradients->update();
    return ScrollArea::showEvent(e);
}
