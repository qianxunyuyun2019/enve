#ifndef BRIGHTNESSEFFECT_H
#define BRIGHTNESSEFFECT_H
#include "pixmapeffect.h"

struct BrightnessEffectRenderData : public PixmapEffectRenderData {
    friend class StdSelfRef;
    void applyEffectsSk(const SkBitmap &bitmap,
                        const qreal scale);

    bool hasKeys = false;
    qreal brightness;
private:
    BrightnessEffectRenderData() {}
};

class BrightnessEffect : public PixmapEffect {
    friend class SelfRef;
protected:
    BrightnessEffect(const qreal brightness = 0);
public:
    stdsptr<PixmapEffectRenderData> getPixmapEffectRenderDataForRelFrameF(
            const qreal relFrame, BoxRenderData*);
private:
    qsptr<QrealAnimator> mBrightnessAnimator;
};
#endif // BRIGHTNESSEFFECT_H