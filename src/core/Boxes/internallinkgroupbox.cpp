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

#include "internallinkgroupbox.h"
#include "Animators/transformanimator.h"
#include "Timeline/durationrectangle.h"
#include "layerboxrenderdata.h"

InternalLinkGroupBox::InternalLinkGroupBox(ContainerBox * const linkTarget) :
    ContainerBox(TYPE_INTERNAL_LINK_GROUP) {
    setLinkTarget(linkTarget);

    ca_prependChildAnimator(mTransformAnimator.data(), mBoxTarget);
    connect(mBoxTarget.data(), &BoxTargetProperty::targetSet,
            this, &InternalLinkGroupBox::setTargetSlot);
}

InternalLinkGroupBox::~InternalLinkGroupBox() {
    setLinkTarget(nullptr);
}

void InternalLinkGroupBox::writeBoundingBox(eWriteStream& dst) {
    BoundingBox::writeBoundingBox(dst);
}

void InternalLinkGroupBox::readBoundingBox(eReadStream& src) {
    BoundingBox::readBoundingBox(src);
}

//bool InternalLinkGroupBox::relPointInsidePath(const QPointF &relPos) {
//    return mLinkTarget->relPointInsidePath(point);
//}

FrameRange InternalLinkGroupBox::prp_getIdenticalRelRange(
        const int relFrame) const {
    FrameRange range{FrameRange::EMIN, FrameRange::EMAX};
    if(!getLinkTarget()) return range;
    if(mVisible) {
        if(isFrameInDurationRect(relFrame)) {
            range *= BoundingBox::prp_getIdenticalRelRange(relFrame);
        } else {
            if(relFrame > mDurationRectangle->getMaxRelFrame()) {
                range = mDurationRectangle->getRelFrameRangeToTheRight();
            } else if(relFrame < mDurationRectangle->getMinRelFrame()) {
                range = mDurationRectangle->getRelFrameRangeToTheLeft();
            }
        }
    }
    auto targetRange = getLinkTarget()->prp_getIdenticalRelRange(relFrame);

    return range*targetRange;
}

QMatrix InternalLinkGroupBox::getRelativeTransformAtFrame(
        const qreal relFrame) {
    if(isParentLink() && getLinkTarget()) {
        return getLinkTarget()->getRelativeTransformAtFrame(relFrame);
    } else {
        return BoundingBox::getRelativeTransformAtFrame(relFrame);
    }
}

void InternalLinkGroupBox::setupRasterEffectsF(const qreal relFrame,
                                         BoxRenderData * const data) {
    if(isParentLink() && getLinkTarget()) {
        getLinkTarget()->setupRasterEffectsF(relFrame, data);
    } else {
        BoundingBox::setupRasterEffectsF(relFrame, data);
    }
}

SkBlendMode InternalLinkGroupBox::getBlendMode() {
    if(isParentLink() && getLinkTarget()) {
        return getLinkTarget()->getBlendMode();
    }
    return BoundingBox::getBlendMode();
}

void InternalLinkGroupBox::setupRenderData(const qreal relFrame,
                                           BoxRenderData * const data) {
    const auto linkTarget = getLinkTarget();
    if(linkTarget) linkTarget->BoundingBox::setupRenderData(relFrame, data);

    ContainerBox::setupRenderData(relFrame, data);
}

ContainerBox *InternalLinkGroupBox::getFinalTarget() const {
    if(!getLinkTarget()) return nullptr;
    if(getLinkTarget()->SWT_isLinkBox()) {
        return static_cast<InternalLinkGroupBox*>(getLinkTarget())->getFinalTarget();
    }
    return getLinkTarget();
}

int InternalLinkGroupBox::prp_getRelFrameShift() const {
    if(!getLinkTarget()) return ContainerBox::prp_getRelFrameShift();
    if(getLinkTarget()->SWT_isLinkBox() || isParentLink()) {
        return ContainerBox::prp_getRelFrameShift() +
                getLinkTarget()->prp_getRelFrameShift();
    }
    return ContainerBox::prp_getRelFrameShift();
}

bool InternalLinkGroupBox::relPointInsidePath(const QPointF &relPos) const {
    if(!getFinalTarget()) return false;
    return getFinalTarget()->relPointInsidePath(relPos);
}

void InternalLinkGroupBox::setTargetSlot(BoundingBox * const target) {
    if(target->SWT_isLayerBox())
        setLinkTarget(static_cast<ContainerBox*>(target));
}

void InternalLinkGroupBox::setLinkTarget(ContainerBox * const linkTarget) {
    disconnect(mBoxTarget.data(), nullptr, this, nullptr);
    if(getLinkTarget()) {
        disconnect(getLinkTarget(), nullptr, this, nullptr);
        getLinkTarget()->removeLinkingBox(this);
        removeAllContained();
    }
    if(linkTarget) {
        prp_setName(linkTarget->prp_getName() + " link");
        connect(linkTarget, &Property::prp_nameChanged,
                this, &Property::prp_setName);
        mBoxTarget->setTarget(linkTarget);
        linkTarget->addLinkingBox(this);
        connect(linkTarget, &BoundingBox::prp_absFrameRangeChanged,
                this, [this, linkTarget](const FrameRange& targetAbs) {
            const auto relRange = linkTarget->prp_absRangeToRelRange(targetAbs);
            prp_afterChangedRelRange(relRange);
        });

        const auto &boxesList = linkTarget->getContainedBoxes();
        for(const auto& child : boxesList) {
            const auto newLink = child->createLinkForLinkGroup();
            addContained(newLink);
        }
    } else {
        prp_setName("empty link");
        mBoxTarget->setTarget(nullptr);
    }
    planScheduleUpdate(UpdateReason::userChange);
    connect(mBoxTarget.data(), &BoxTargetProperty::targetSet,
            this, &InternalLinkGroupBox::setTargetSlot);
}

QPointF InternalLinkGroupBox::getRelCenterPosition() {
    if(!getLinkTarget()) return QPointF();
    return getLinkTarget()->getRelCenterPosition();
}

ContainerBox *InternalLinkGroupBox::getLinkTarget() const {
    return static_cast<ContainerBox*>(mBoxTarget->getTarget());
}

qsptr<BoundingBox> InternalLinkGroupBox::createLink() {
    if(!getLinkTarget()) return createLink();
    return getLinkTarget()->createLink();
}

qsptr<BoundingBox> InternalLinkGroupBox::createLinkForLinkGroup() {
    if(!getLinkTarget()) return createLink();
    if(isParentLink()) {
        return getLinkTarget()->createLinkForLinkGroup();
    } else {
        return enve::make_shared<InternalLinkGroupBox>(this);
    }
}

bool InternalLinkGroupBox::isFrameInDurationRect(const int relFrame) const {
    if(!getLinkTarget()) return false;
    return ContainerBox::isFrameInDurationRect(relFrame) &&
            getLinkTarget()->isFrameInDurationRect(relFrame);
}

stdsptr<BoxRenderData> InternalLinkGroupBox::createRenderData() {
    if(!getLinkTarget()) return nullptr;
    auto renderData = getLinkTarget()->createRenderData();
    renderData->fParentBox = this;
    return renderData;
}
