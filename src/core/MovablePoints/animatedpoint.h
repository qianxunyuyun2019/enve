// enve - 2D animations software
// Copyright (C) 2016-2019 Maurycy Liebner

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

#ifndef ANIMATEDPOINT_H
#define ANIMATEDPOINT_H

#include "movablepoint.h"
#include <QPointF>
#include <QRectF>
#include <QPainter>
#include "Animators/qpointfanimator.h"
#include "skia/skiaincludes.h"

class BoundingBox;
class NodePoint;

class PropertyUpdater;

class QPointFAnimator;

class AnimatedPoint : public MovablePoint {
    e_OBJECT
protected:
    AnimatedPoint(QPointFAnimator * const associatedAnimator,
                  const MovablePointType &type);
    AnimatedPoint(QPointFAnimator * const associatedAnimator,
                  BasicTransformAnimator * const trans,
                  const MovablePointType &type);
public:    
    QPointF getRelativePos() const;
    void setRelativePos(const QPointF &relPos);

    void startTransform();
    void finishTransform();
    void cancelTransform();

    QPointFAnimator * getAnimator() const {
        return mAssociatedAnimator_k;
    }

    QrealAnimator * getXAnimator() const {
        return mAssociatedAnimator_k->getXAnimator();
    }

    QrealAnimator * getYAnimator() const {
        return mAssociatedAnimator_k->getYAnimator();
    }
protected:
    void setValue(const QPointF& value) {
        mAssociatedAnimator_k->setBaseValue(value);
    }

    QPointF getValue() const {
        return mAssociatedAnimator_k->getBaseValue();
    }
private:
    QPointFAnimator* const mAssociatedAnimator_k;
};

#endif // ANIMATEDPOINT_H
