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

#include "eblur.h"

qsptr<CustomRasterEffect> eCreateNewestVersion() {
    // Use default, most up to date, version
    return enve::make_shared<eBlur>();
}

qsptr<CustomRasterEffect> eCreate(const CustomIdentifier &identifier) {
    Q_UNUSED(identifier)
    // Choose version based on identifier
    // if(identifier.fVersion == CustomIdentifier::Version{0, 0, 0})
    return enve::make_shared<eBlur>();
}

// Returned value must be unique, lets enve distinguish effects
QString effectId() {
    return "iz4784jsvg6fw7bc3clb";
}

// Name of your effect used in UI
QString eName() {
    return "eBlur";
}

// here specify your effect's most up to date version
CustomIdentifier::Version effectVersion() {
    return { 0, 0, 0 };
}

CustomIdentifier eIdentifier() {
    return { effectId(), eName(), effectVersion() };
}

bool eSupports(const CustomIdentifier &identifier) {
    if(identifier.fEffectId != effectId()) return false;
    if(identifier.fEffectName != eName()) return false;
    return identifier.fVersion == effectVersion();
}

#include "enveCore/Animators/qrealanimator.h"
eBlur::eBlur() : CustomRasterEffect(eName().toLower(),
                                    HardwareSupport::gpuPreffered, false) {
    mRadius = enve::make_shared<QrealAnimator>(10, 0, 999.999, 1, "radius");
    ca_addChild(mRadius);
    connect(mRadius.get(), &QrealAnimator::effectiveValueChanged,
            this, &CustomRasterEffect::forcedMarginChanged);
}

stdsptr<RasterEffectCaller> eBlur::getEffectCaller(
        const qreal relFrame, const qreal resolution,
        const qreal influence, BoxRenderData * const data) const {
    Q_UNUSED(data)
    const qreal radius = mRadius->getEffectiveValue(relFrame)*resolution*influence;
    if(isZero4Dec(radius)) return nullptr;
    return enve::make_shared<eBlurCaller>(instanceHwSupport(), radius);
}

QMargins radiusToMargin(const qreal radius) {
    return QMargins() + qCeil(radius);
}

QMargins eBlur::getMargin() const {
    return radiusToMargin(mRadius->getEffectiveValue());
}

CustomIdentifier eBlur::getIdentifier() const {
    return { effectId(), eName(), { 0, 0, 0 } };
}

eBlurCaller::eBlurCaller(const HardwareSupport hwSupport, const qreal radius) :
    RasterEffectCaller(hwSupport, true, radiusToMargin(radius)),
    mRadius(static_cast<float>(radius)) {}

void eBlurCaller::processGpu(QGL33 * const gl,
                             GpuRenderTools &renderTools) {
    Q_UNUSED(gl)

    const float sigma = mRadius*0.3333333f;
    const auto filter = SkBlurImageFilter::Make(sigma, sigma, nullptr);

    SkPaint paint;
    paint.setImageFilter(filter);

    renderTools.switchToSkia();
    const auto canvas = renderTools.requestTargetCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    const auto srcTex = renderTools.requestSrcTextureImageWrapper();
    canvas->drawImage(srcTex, 0, 0, &paint);
    canvas->flush();

    renderTools.swapTextures();
}

void eBlurCaller::processCpu(CpuRenderTools &renderTools,
                             const CpuRenderData &data) {
    Q_UNUSED(data)

    const float sigma = mRadius*0.3333333f;
    const auto filter = SkBlurImageFilter::Make(sigma, sigma, nullptr);

    SkPaint paint;
    paint.setImageFilter(filter);

    SkBitmap tile;
    renderTools.fDstBtmp.extractSubset(&tile, data.fTexTile);
    SkCanvas canvas(tile);
    canvas.clear(SK_ColorTRANSPARENT);

    const int radCeil = static_cast<int>(ceil(mRadius));
    const auto& srcBtmp = renderTools.fSrcBtmp;
    const auto& texTile = data.fTexTile;
    auto srcRect = texTile.makeOutset(radCeil, radCeil);
    if(srcRect.intersect(srcRect, srcBtmp.bounds())) {
        SkBitmap tileSrc;
        srcBtmp.extractSubset(&tileSrc, srcRect);
        canvas.drawBitmap(tileSrc,
                          srcRect.left() - texTile.left(),
                          srcRect.top() - texTile.top(), &paint);
    }
}
