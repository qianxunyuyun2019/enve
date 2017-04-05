#ifndef RENDERCACHEHANDLER_H
#define RENDERCACHEHANDLER_H
class Key;
#include <QtCore>
class QPainter;
class BoundingBoxRenderContainer;
class DurationRectangle;

enum RenderCacheRangeChangeType {
//    MOVE_KEY,
    REMOVE_KEY,
    ADD_KEY,
    CHANGE_KEY
};

struct RenderCacheRangeChange {
    RenderCacheRangeChange(const RenderCacheRangeChangeType &typeT,
                           const int &relFrameT);

    RenderCacheRangeChange(const RenderCacheRangeChangeType &typeT,
                           Key *keyToAdd);

    RenderCacheRangeChangeType type;
    int relFrame;
    Key *key;
};

class RenderCacheRange {
public:
    RenderCacheRange(Key *minKey,
                     Key *maxKey,
                     const int &minFrame,
                     const int &maxFrame);

    virtual ~RenderCacheRange();

    void setInternalDifferencesPresent(const bool &bT);

    bool areInternalDifferencesPresent() const;

    bool relFrameInRange(const int &relFrame);

    bool getRenderContainterIdAtRelFrame(const int &relFrame, int *id);

    int getRenderContainterInsertIdAtRelFrame(const int &relFrame);

    BoundingBoxRenderContainer *getRenderContainerAtRelFrame(
                                const int &frame);

    BoundingBoxRenderContainer *createNewRenderContainerAtRelFrame(
                                const int &frame);

    void clearCache();

    const int &getMaxRelFrame();

    const int &getMinRelFrame();

    virtual void updateAfterKeysChanged();

    void setMinKeyTMP(Key *minKey);

    void setMaxKeyTMP(Key *maxKey);

    void setMaxRelFrame(const int &maxRelFrame);

    void setMinRelFrame(const int &minRelFrame);

    Key *getMinKey();

    Key *getMaxKey();

    void drawCacheOnTimeline(QPainter *p,
                             const qreal &pixelsPerFrame,
                             const qreal &drawY,
                             const int &startFrame,
                             const int &endFrame,
                             int *lastDrawnRelToStartFrame,
                             int *lastDrawFrameRightPosP);
    void insertRenderContainer(BoundingBoxRenderContainer *cont);

    virtual bool isBlocked() { return false; }
protected:
    QList<BoundingBoxRenderContainer*> mRenderContainers;
    bool mInternalDifferences;

    int mMinRelFrame;
    int mMaxRelFrame;

    Key *mMinKey = NULL;
    Key *mMaxKey = NULL;
};

class RenderCacheHandler;

class AnimationRenderCacheRange : public RenderCacheRange {
public:
    AnimationRenderCacheRange(DurationRectangle *durationRect);

    bool isBlocked() { return true; }

    void updateAfterKeysChanged() {}

    void addInternalKey(Key *key);
    void removeInternalKey(Key *key);

    void updateMaxKey() {
        mMaxKey = getLastInternalKey();
    }

    void updateMinKey() {
        mMinKey = getFirstInternalKey();
    }

    void addAllInternalKeysToHandler(RenderCacheHandler *handler);
private:
    Key *getLastInternalKey() {
        if(mInternalKeys.isEmpty()) return NULL;
        return mInternalKeys.last();
    }

    Key *getFirstInternalKey() {
        if(mInternalKeys.isEmpty()) return NULL;
        return mInternalKeys.first();
    }
    int getKeyInsertIdForFrame(const int &frame);

    QList<Key*> mInternalKeys;
    DurationRectangle *mDurationRect = NULL;
};

class RenderCacheHandler {
public:
    RenderCacheHandler();

    void applyChanges();

    void addRenderCacheRangeChange(const RenderCacheRangeChange &change);

    BoundingBoxRenderContainer *getRenderContainerAtRelFrame(
                                    const int &frame);

    BoundingBoxRenderContainer *createNewRenderContainerAtRelFrame(
                                    const int &frame);

    void clearAllCache();

    void addRangeNeedingUpdate(const int &min, const int &max);

    void addAddedKey(Key *key);

    void addRemovedKey(Key *key);

    void addChangedKey(Key *key);

    void updateCurrentRenderContainerFromFrame(const int &relFrame);

    bool updateCurrentRenderContainerFromFrameIfNotNull(
                                                const int &relFrame);

    void duplicateCurrentRenderContainerFrom(
            BoundingBoxRenderContainer *cont);

    void updateCurrentRenderContainerTransform(const QMatrix &trans);

    void drawCurrentRenderContainer(QPainter *p);

    void drawCacheOnTimeline(QPainter *p,
                             const qreal &pixelsPerFrame,
                             const qreal &drawY,
                             const int &startFrame, const int &endFrame);
    void setNoCache(const bool &noCache);

    bool isRelFrameInDurationRectAnimationRange(const int &frame);
    void setupRenderRangeforAnimationRange();

    void divideRenderCacheRangeAtRelFrame(const int &divideRelFrame,
                                          Key *newKey);
    bool isThereBarrierAtRelFrame(const int &frame);
    void setDurationRectangle(DurationRectangle *durRect);
private:
    void clearRenderRangeforAnimationRange();

    bool mAnimationRangeSetup = false;
    bool mNoCache = false;
    DurationRectangle *mDurationRect = NULL;

    void setCurrentRenderContainerVar(BoundingBoxRenderContainer *cont);

    BoundingBoxRenderContainer *mCurrentRenderContainer = NULL;

    void divideRenderCacheRange(RenderCacheRange *oldRange,
                                const int &relFrame,
                                Key *newKey);

    void mergeRenderCacheRanges(RenderCacheRange *prevRange,
                              RenderCacheRange *nextRange);

    void getRenderCacheRangesWithBarrierAtRelFrame(
                            RenderCacheRange **prevRange,
                            RenderCacheRange **nextRange,
                            const int &relFrame);

    void removeRangesBarrierAtRelFrame(const int &relFrame, Key *key);

    void updateAfterBarrierValueChanged(const int &barrierRelFrame);

    RenderCacheRange *getRenderCacheRangeContainingRelFrame(
                                        const int &relFrame);

    void removeRenderCacheRangeFromList(RenderCacheRange *range);

    void applyChange(const RenderCacheRangeChange &change);

    void removeRangeNeedingUpdate(RenderCacheRange *range);

    void addRangeNeedingUpdate(RenderCacheRange *range);

    QList<RenderCacheRangeChange> mRenderCacheRangeRemovals;
    QList<RenderCacheRangeChange> mRenderCacheRangeAdds;
    QList<RenderCacheRangeChange> mRenderCacheRangeValueChanges;

    QList<RenderCacheRange*> mRangesNeedingUpdate;

    QList<RenderCacheRange*> mRenderCacheRange;
};

#endif // RENDERCACHEHANDLER_H