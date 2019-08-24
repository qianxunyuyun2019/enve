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

#ifndef RANDOMGRID_H
#define RANDOMGRID_H
#include "Animators/staticcomplexanimator.h"
#include "Animators/intanimator.h"

class RandomGrid : public StaticComplexAnimator {
    e_OBJECT
protected:
    RandomGrid();
public:
    qreal getBaseSeed(const qreal relFrame) const;
    qreal getGridSize(const qreal relFrame) const;

    qreal getRandomValue(const qreal relFrame, const QPointF& pos) const;

    static qreal sGetRandomValue(const qreal baseSeed, const qreal gridSize,
                                 const QPointF& pos);
    static qreal sGetRandomValue(const qreal min, const qreal max,
                                 const qreal baseSeed, const qreal gridSize,
                                 const QPointF& pos);
private:
    static qreal sGetRandomValue(const qreal baseSeed, const QPoint& gridId);

    qsptr<QrealAnimator> mGridSize;
    qsptr<QrealAnimator> mSeed;
};

#endif // RANDOMGRID_H
