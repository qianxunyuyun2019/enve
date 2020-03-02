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

#ifndef EBLUR_H
#define EBLUR_H

#include "eblur_global.h"
#include "enveCore/gpurendertools.h"

class eBlurCaller : public RasterEffectCaller {
public:
    eBlurCaller(const HardwareSupport hwSupport,
                const qreal radius);

    void processGpu(QGL33 * const gl,
                    GpuRenderTools& renderTools);
    void processCpu(CpuRenderTools& renderTools,
                    const CpuRenderData &data);
private:
    const float mRadius;
};

class eBlur : public CustomRasterEffect {
public:
    eBlur();

    stdsptr<RasterEffectCaller> getEffectCaller(const qreal relFrame,
                                                const qreal resolution,
                                                const qreal influence) const;
    QMargins getMargin() const;
    bool forceMargin() const { return true; }

    CustomIdentifier getIdentifier() const;
private:
    qsptr<QrealAnimator> mRadius;
};

#endif // EBLUR_H
