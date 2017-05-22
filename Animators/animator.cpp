#include "animator.h"
#include "Animators/complexanimator.h"
#include "mainwindow.h"
#include "key.h"
#include "BoxesList/boxsinglewidget.h"

Animator::Animator() :
    Property() {
}

Animator::~Animator() {
    emit beingDeleted();
}

int Animator::anim_getNextKeyRelFrame(Key *key) {
    if(key == NULL) return INT_MAX;
    Key *nextKey = key->getNextKey();
    if(nextKey == NULL) {
        return INT_MAX;
    }
    return nextKey->getRelFrame();
}

int Animator::anim_getPrevKeyRelFrame(Key *key) {
    if(key == NULL) return INT_MIN;
    Key *prevKey = key->getPrevKey();
    if(prevKey == NULL) {
        return INT_MIN;
    }
    return prevKey->getRelFrame();
}

void Animator::anim_updateAfterChangedKey(Key *key) {
    if(anim_mIsComplexAnimator) return;
    prp_updateAfterChangedRelFrameRange(anim_getPrevKeyRelFrame(key),
                                        anim_getNextKeyRelFrame(key));
}

void Animator::prp_setAbsFrame(const int &frame) {
    anim_mCurrentAbsFrame = frame;
    anim_updateRelFrame();
    anim_updateKeyOnCurrrentFrame();
    //anim_callFrameChangeUpdater();
}

void Animator::anim_updateRelFrame() {
    anim_mCurrentRelFrame = anim_mCurrentAbsFrame -
                            prp_getFrameShift();
}

void Animator::prp_switchRecording() {
    prp_setRecording(!anim_mIsRecording);
}

void Animator::anim_mergeKeysIfNeeded() {
    Key *lastKey = NULL;
    QList<KeyPair> keyPairsToMerge;
    foreach(const std::shared_ptr<Key> &key, anim_mKeys) {
        Key *keyPtr = key.get();
        if(lastKey != NULL) {
            if(keyPtr->getAbsFrame() == lastKey->getAbsFrame() ) {
                if(keyPtr->isDescendantSelected()) {
                    keyPairsToMerge << KeyPair(keyPtr, lastKey);
                } else {
                    keyPairsToMerge << KeyPair(lastKey, keyPtr);
                    keyPtr = NULL;
                }
            }
        }
        lastKey = keyPtr;
    }
    foreach(KeyPair keyPair, keyPairsToMerge) {
        keyPair.merge();
    }
}

Key *Animator::anim_getKeyAtAbsFrame(const int &frame) {
    return anim_getKeyAtRelFrame(prp_absFrameToRelFrame(frame));
}

int Animator::anim_getKeyIndex(Key *key) {
    int index = -1;
    for(int i = 0; i < anim_mKeys.count(); i++) {
        if(anim_mKeys.at(i).get() == key) {
            index = i;
        }
    }
    return index;
}

Key *Animator::anim_getNextKey(Key *key) {
    int keyId = anim_getKeyIndex(key);
    if(keyId == anim_mKeys.count() - 1) return NULL;
    return anim_mKeys.at(keyId + 1).get();
}

Key *Animator::anim_getPrevKey(Key *key) {
    int keyId = anim_getKeyIndex(key);
    if(keyId <= 0) return NULL;
    return anim_mKeys.at(keyId - 1).get();
}

void Animator::anim_deleteCurrentKey() {
    if(anim_mKeyOnCurrentFrame == NULL) return;
    anim_mKeyOnCurrentFrame->deleteKey();
}

int Animator::anim_getClosestsKeyOccupiedRelFrame(const int &frame) {
    if(anim_mKeys.isEmpty()) return 0;
    int firstFrame = anim_mKeys.first()->getRelFrame();
    int lastFrame = anim_mKeys.last()->getRelFrame();
    if(frame < firstFrame) return firstFrame;
    if(frame > lastFrame) return lastFrame;
    return frame;
}

Key *Animator::anim_getKeyAtRelFrame(const int &frame) {
    if(anim_mKeys.isEmpty()) return NULL;
    if(frame > anim_mKeys.last()->getRelFrame()) return NULL;
    if(frame < anim_mKeys.first()->getRelFrame()) return NULL;
    int minId = 0;
    int maxId = anim_mKeys.count() - 1;
    while(maxId - minId > 1) {
        int guess = (maxId + minId)/2;
        if(anim_mKeys.at(guess)->getRelFrame() > frame) {
            maxId = guess;
        } else {
            minId = guess;
        }
    }
    Key *minIdKey = anim_mKeys.at(minId).get();
    if(minIdKey->getRelFrame() == frame) return minIdKey;
    Key *maxIdKey = anim_mKeys.at(maxId).get();
    if(maxIdKey->getRelFrame() == frame) return maxIdKey;
    return NULL;
}

bool Animator::anim_hasPrevKey(Key *key) {
    int keyId = anim_getKeyIndex(key);
    if(keyId > 0) return true;
    return false;
}

bool Animator::anim_hasNextKey(Key *key) {
    int keyId = anim_getKeyIndex(key);
    if(keyId < anim_mKeys.count() - 1) return true;
    return false;
}

void Animator::anim_callFrameChangeUpdater() {
    if(prp_mUpdater == NULL) {
        return;
    } else {
        prp_mUpdater->frameChangeUpdate();
    }
}

void Animator::anim_updateAfterShifted() {
    foreach(const std::shared_ptr<Key> &key, anim_mKeys) {
        emit prp_removingKey(key.get());
        emit prp_addingKey(key.get());
    }
}

bool keysFrameSort(const std::shared_ptr<Key> &key1,
                   const std::shared_ptr<Key> &key2) {
    return key1->getAbsFrame() < key2->getAbsFrame();
}

void Animator::anim_sortKeys() {
    qSort(anim_mKeys.begin(), anim_mKeys.end(), keysFrameSort);
}

void Animator::anim_appendKey(Key *newKey,
                              const bool &saveUndoRedo) {
    if(saveUndoRedo && !anim_isComplexAnimator()) {
        addUndoRedo(new AddKeyToAnimatorUndoRedo(newKey, this));
    }
    anim_mKeys.append(newKey->ref<Key>());
    anim_sortKeys();
    //mergeKeysIfNeeded();
    emit prp_addingKey(newKey);

    if(anim_mIsCurrentAnimator) {
        graphScheduleUpdateAfterKeysChanged();
    }

    anim_updateKeyOnCurrrentFrame();

    anim_updateAfterChangedKey(newKey);
}

void Animator::anim_removeKey(Key *keyToRemove,
                              const bool &saveUndoRedo) {
    anim_updateAfterChangedKey(keyToRemove);

    if(saveUndoRedo && !anim_isComplexAnimator()) {
        addUndoRedo(new RemoveKeyFromAnimatorUndoRedo(
                                keyToRemove, this));
    }
    anim_mKeys.removeAt(anim_getKeyIndex(keyToRemove));

    emit prp_removingKey(keyToRemove);
    anim_sortKeys();

    if(anim_mIsCurrentAnimator) {
        graphScheduleUpdateAfterKeysChanged();
    }

    anim_updateKeyOnCurrrentFrame();
}

void Animator::anim_moveKeyToRelFrame(Key *key,
                                      const int &newFrame,
                                      const bool &finish) {
    anim_updateAfterChangedKey(key);
    emit prp_removingKey(key);
    key->setRelFrame(newFrame);
    emit prp_addingKey(key);
    anim_sortKeys();
    anim_updateKeyOnCurrrentFrame();
    anim_updateAfterChangedKey(key);
}

void Animator::anim_keyValueChanged(Key *key) {
    anim_updateAfterChangedKey(key);
}

void Animator::anim_updateKeyOnCurrrentFrame() {
    if(anim_mTraceKeyOnCurrentFrame) {
        anim_mKeyOnCurrentFrame = anim_getKeyAtAbsFrame(anim_mCurrentAbsFrame);
    }
}

Key *Animator::prp_getKeyAtPos(const qreal &relX,
                               const int &minViewedFrame,
                               const qreal &pixelsPerFrame) {
    qreal relFrame = relX/pixelsPerFrame - prp_getFrameShift();
    qreal pressFrame = relFrame + minViewedFrame;
    if(pixelsPerFrame > KEY_RECT_SIZE) {
        int relFrameInt = relFrame;
        if( qAbs((relFrameInt + 0.5)*pixelsPerFrame - relX +
                 prp_getFrameShift()*pixelsPerFrame) > KEY_RECT_SIZE*0.5) {
            return NULL;
        }
    }
    if(pressFrame < 0) pressFrame -= 1.;
    qreal keyRectFramesSpan = 0.5*KEY_RECT_SIZE/pixelsPerFrame;
    int minPossibleKey = pressFrame - keyRectFramesSpan;
    int maxPossibleKey = pressFrame + keyRectFramesSpan;
    Key *keyAtPos = NULL;
    for(int i = maxPossibleKey; i >= minPossibleKey; i--) {
        keyAtPos = anim_getKeyAtRelFrame(i);
        if(keyAtPos != NULL) {
            return keyAtPos;
        }
    }
    return NULL;
}

void Animator::prp_addAllKeysToComplexAnimator(ComplexAnimator *target) {
    foreach(const std::shared_ptr<Key> &key, anim_mKeys) {
        target->ca_addDescendantsKey(key.get());
    }
}

void Animator::prp_removeAllKeysFromComplexAnimator(ComplexAnimator *target) {
    foreach(const std::shared_ptr<Key> &key, anim_mKeys) {
        target->ca_removeDescendantsKey(key.get());
    }
}

bool Animator::prp_hasKeys() {
    return !anim_mKeys.isEmpty();
}

void Animator::anim_setRecordingWithoutChangingKeys(const bool &rec,
                                                    const bool &saveUndoRedo) {
    if(saveUndoRedo) {
        addUndoRedo(new AnimatorRecordingSetUndoRedo(anim_mIsRecording,
                                                     rec,
                                                     this));
    }

    anim_setRecordingValue(rec);
    anim_setTraceKeyOnCurrentFrame(rec); // !!!
}

void Animator::anim_setRecordingValue(const bool &rec) {
    if(rec == anim_mIsRecording) return;
    anim_mIsRecording = rec;
    emit prp_isRecordingChanged();
}

bool Animator::prp_isRecording() {
    return anim_mIsRecording;
}

void Animator::anim_removeAllKeys() {
    if(anim_mKeys.isEmpty()) return;
    QList<std::shared_ptr<Key>> keys = anim_mKeys;
    foreach(const std::shared_ptr<Key> &key, keys) {
        anim_removeKey(key.get());
    }
}

bool Animator::prp_isKeyOnCurrentFrame() {
    return anim_mKeyOnCurrentFrame != NULL;
}

void Animator::prp_getKeysInRect(const QRectF &selectionRect,
                                 const qreal &pixelsPerFrame,
                                 QList<Key *> *keysList) {
    //selectionRect.translate(-getFrameShift(), 0.);
    int selLeftFrame = selectionRect.left();
    if(0.5*pixelsPerFrame + KEY_RECT_SIZE*0.5 <
       selectionRect.left() - selLeftFrame*pixelsPerFrame) {
        selLeftFrame++;
    }
    int selRightFrame = selectionRect.right();
    if(0.5*pixelsPerFrame - KEY_RECT_SIZE*0.5 >
       selectionRect.right() - selRightFrame*pixelsPerFrame) {
        selRightFrame--;
    }
    for(int i = selRightFrame; i >= selLeftFrame; i--) {
        Key *keyAtPos = anim_getKeyAtAbsFrame(i);
        if(keyAtPos != NULL) {
            keysList->append(keyAtPos);
        }
    }
}

bool Animator::anim_getNextAndPreviousKeyIdForRelFrame(
                        int *prevIdP, int *nextIdP,
                        const int &frame) const {
    if(anim_mKeys.isEmpty()) return false;
    int minId = 0;
    int maxId = anim_mKeys.count() - 1;
    if(frame >= anim_mKeys.last()->getRelFrame()) {
        *prevIdP = maxId;
        *nextIdP = maxId;
        return true;
    }
    if(frame <= anim_mKeys.first()->getRelFrame()) {
        *prevIdP = minId;
        *nextIdP = minId;
        return true;
    }
    while(maxId - minId > 1) {
        int guess = (maxId + minId)/2;
        int keyFrame = anim_mKeys.at(guess)->getRelFrame();
        if(keyFrame > frame) {
            maxId = guess;
        } else if(keyFrame < frame) {
            minId = guess;
        } else {
            *nextIdP = guess;
            *prevIdP = guess;
            return true;
        }
    }

    if(minId == maxId) {
        Key *key = anim_mKeys.at(minId).get();
        if(key->getRelFrame() > frame) {
            if(minId != 0) {
                minId = minId - 1;
            }
        } else if(key->getRelFrame() < frame) {
            if(minId < anim_mKeys.count() - 1) {
                maxId = minId + 1;
            }
        }
    }
    *prevIdP = minId;
    *nextIdP = maxId;
    return true;
}

void Animator::anim_drawKey(QPainter *p,
                            Key *key,
                            const qreal &pixelsPerFrame,
                            const qreal &drawY,
                            const int &startFrame) {
    if(key->isSelected()) {
        p->setBrush(Qt::yellow);
    } else {
        p->setBrush(Qt::red);
    }
    if(key->isHovered()) {
        p->save();
        p->setPen(QPen(Qt::black, 1.5));
        p->drawEllipse(
            QRectF(
                QPointF((key->getAbsFrame() - startFrame + 0.5)*
                        pixelsPerFrame - KEY_RECT_SIZE*0.5,
                        drawY + (MIN_WIDGET_HEIGHT -
                                  KEY_RECT_SIZE)*0.5 ),
                QSize(KEY_RECT_SIZE, KEY_RECT_SIZE) ) );
        p->restore();
    } else {
        p->drawEllipse(
            QRectF(
                QPointF((key->getAbsFrame() - startFrame + 0.5)*
                        pixelsPerFrame - KEY_RECT_SIZE*0.5,
                        drawY + (MIN_WIDGET_HEIGHT -
                                  KEY_RECT_SIZE)*0.5 ),
                QSize(KEY_RECT_SIZE, KEY_RECT_SIZE) ) );
    }
}

void Animator::prp_drawKeys(QPainter *p,
                            const qreal &pixelsPerFrame,
                            const qreal &drawY,
                            const int &startFrame,
                            const int &endFrame) {
    p->save();
    //p->translate(getFrameShift()*pixelsPerFrame, 0.);
    foreach(const std::shared_ptr<Key> &key, anim_mKeys) {
        if(key->getAbsFrame() >= startFrame &&
           key->getAbsFrame() <= endFrame) {
            anim_drawKey(p, key.get(), pixelsPerFrame, drawY, startFrame);
        }
    }
    p->restore();
}
