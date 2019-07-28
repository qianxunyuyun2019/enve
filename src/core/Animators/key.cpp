#include "key.h"
#include "qrealpoint.h"
#include "animator.h"
#include "pointhelpers.h"

Key::Key(Animator * const parentAnimator) :
    Key(0, parentAnimator) {}

Key::Key(const int frame, Animator * const parentAnimator) :
    mParentAnimator(parentAnimator) {
    mRelFrame = frame;
}

bool Key::isSelected() const { return mIsSelected; }

void Key::writeKey(QIODevice * const dst) {
    dst->write(rcConstChar(&mRelFrame), sizeof(int));
}

void Key::readKey(QIODevice * const src) {
    src->read(rcChar(&mRelFrame), sizeof(int));
}

void Key::removeFromAnimator() {
    if(!mParentAnimator) return;
    mParentAnimator->anim_removeKey(ref<Key>());
}

bool Key::hasPrevKey() const {
    if(!mParentAnimator) return false;
    return mParentAnimator->anim_hasPrevKey(this);
}

bool Key::hasNextKey() const {
    if(!mParentAnimator) return false;
    return mParentAnimator->anim_hasNextKey(this);
}

void Key::incFrameAndUpdateParentAnimator(const int inc) {
    setRelFrameAndUpdateParentAnimator(mRelFrame + inc);
}

void Key::setRelFrameAndUpdateParentAnimator(const int relFrame) {
    if(!mParentAnimator) return;
    mParentAnimator->anim_moveKeyToRelFrame(this, relFrame);
}

void Key::addToSelection(QList<Animator*> &toSelect) {
    if(isSelected()) return;
    if(!mParentAnimator->hasSelectedKeys()) {
        toSelect << mParentAnimator;
    }
    mParentAnimator->addKeyToSelected(this);
}

void Key::removeFromSelection(QList<Animator*> &toRemove) {
    if(isSelected()) {
        mParentAnimator->removeKeyFromSelected(this);
        if(!mParentAnimator->hasSelectedKeys()) {
            toRemove << mParentAnimator;
        }
    }
}

void Key::startFrameTransform() {
    mSavedRelFrame = mRelFrame;
}

void Key::cancelFrameTransform() {
    mParentAnimator->anim_moveKeyToRelFrame(this, mSavedRelFrame);
}

void Key::scaleFrameAndUpdateParentAnimator(
        const int relativeToFrame,
        const qreal scaleFactor,
        const bool useSavedFrame) {
    int thisRelFrame = useSavedFrame ? mSavedRelFrame : mRelFrame;
    int relativeToRelFrame =
            mParentAnimator->prp_absFrameToRelFrame(relativeToFrame);
    int newFrame = qRound(relativeToRelFrame +
                          (thisRelFrame - relativeToRelFrame)*scaleFactor);
    if(newFrame == mRelFrame) return;
    setRelFrameAndUpdateParentAnimator(newFrame);
}

void Key::setSelected(const bool bT) {
    mIsSelected = bT;
}

void Key::finishFrameTransform() {
    if(!mParentAnimator) return;
//    mParentAnimator->addUndoRedo(
//                new ChangeKeyFrameUndoRedo(mSavedRelFrame,
//                                           mRelFrame, this));
}

int Key::relFrameToAbsFrame(const int relFrame) const {
    if(!mParentAnimator) return relFrame;
    return mParentAnimator->prp_relFrameToAbsFrame(relFrame);
}

int Key::absFrameToRelFrame(const int absFrame) const {
    if(!mParentAnimator) return absFrame;
    return mParentAnimator->prp_absFrameToRelFrame(absFrame);
}

qreal Key::relFrameToAbsFrameF(const qreal relFrame) const {
    if(!mParentAnimator) return relFrame;
    return mParentAnimator->prp_relFrameToAbsFrameF(relFrame);
}

qreal Key::absFrameToRelFrameF(const qreal absFrame) const {
    if(!mParentAnimator) return absFrame;
    return mParentAnimator->prp_absFrameToRelFrameF(absFrame);
}

int Key::getAbsFrame() const {
    return relFrameToAbsFrame(mRelFrame);
}

int Key::getRelFrame() const {
    return mRelFrame;
}

void Key::setRelFrame(const int frame) {
    mRelFrame = frame;
}

void Key::setAbsFrame(const int frame) {
    setRelFrame(absFrameToRelFrame(frame));
}

Key *Key::getNextKey() const {
    return mParentAnimator->anim_getNextKey(this);
}

Key *Key::getPrevKey() const {
    return mParentAnimator->anim_getPrevKey(this);
}

void Key::afterKeyChanged() {
    mParentAnimator->anim_updateAfterChangedKey(this);
}
