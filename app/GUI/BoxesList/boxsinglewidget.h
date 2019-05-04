#ifndef BOXSINGLEWIDGET_H
#define BOXSINGLEWIDGET_H

#include "OptimalScrollArea/singlewidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMimeData>
#include <QComboBox>
#include "skia/skiaincludes.h"
#include "smartPointers/sharedpointerdefs.h"
#include "framerange.h"
class QrealAnimatorValueSlider;
class DurationRectangleMovable;
class Key;
class BoxTargetWidget;
class BoxesListActionButton;
class PixmapActionButton;
class BoolPropertyWidget;
class ComboBoxProperty;
class ColorAnimator;
class ColorAnimatorButton;

class BoxSingleWidget : public SingleWidget {
public:
    explicit BoxSingleWidget(ScrollWidgetVisiblePart *parent = nullptr);

    void setTargetAbstraction(SingleWidgetAbstraction *abs);

    static QPixmap* VISIBLE_PIXMAP;
    static QPixmap* INVISIBLE_PIXMAP;
    static QPixmap* HIDE_CHILDREN;
    static QPixmap* SHOW_CHILDREN;
    static QPixmap* LOCKED_PIXMAP;
    static QPixmap* UNLOCKED_PIXMAP;
    static QPixmap* ANIMATOR_CHILDREN_VISIBLE;
    static QPixmap* ANIMATOR_CHILDREN_HIDDEN;
    static QPixmap* ANIMATOR_RECORDING;
    static QPixmap* ANIMATOR_NOT_RECORDING;
    static QPixmap* ANIMATOR_DESCENDANT_RECORDING;
    static bool sStaticPixmapsLoaded;
    static void loadStaticPixmaps();
    static void clearStaticPixmaps();

    void rename();
    void drawKeys(QPainter * const p,
                  const qreal &pixelsPerFrame,
                  const FrameRange &viewedFrames);
    Key *getKeyAtPos(const int &pressX,
                     const qreal &pixelsPerFrame,
                     const int &minViewedFrame);
    void getKeysInRect(const QRectF &selectionRect,
                       const qreal &pixelsPerFrame,
                       QList<Key *> &listKeys);
    DurationRectangleMovable *getRectangleMovableAtPos(
                        const int &pressX,
                        const qreal &pixelsPerFrame,
                        const int &minViewedFrame);
    void setBlendMode(const SkBlendMode &mode);
    void setSelected(const bool &bT) {
        mSelected = bT;
        update();
    }
    bool isTargetDisabled();
    int getOptimalNameRightX();
protected:
    bool mSelected = false;
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *);

    bool mCompositionModeVisible = false;
    void updateCompositionBoxVisible();
    void clearAndHideValueAnimators();
    void updateValueSlidersForQPointFAnimator();
private:
    void clearColorButton();

    void clearSelected() {
        setSelected(false);
    }
    void switchContentVisibleAction();
    void switchRecordingAction();
    void switchBoxLockedAction();

    void switchBoxVisibleAction();
    void setCompositionMode(const int &id);
    ColorAnimator* getColorTarget() const;

    QPoint mDragStartPos;

    PixmapActionButton *mRecordButton;
    PixmapActionButton *mContentButton;
    PixmapActionButton *mVisibleButton;
    PixmapActionButton *mLockedButton;
    ColorAnimatorButton *mColorButton;
    BoxTargetWidget *mBoxTargetWidget;

    qptr<ComboBoxProperty> mLastComboBoxProperty;
    QWidget *mFillWidget;
    BoolPropertyWidget *mCheckBox;
    QHBoxLayout *mMainLayout;
    QrealAnimatorValueSlider *mValueSlider;
    QrealAnimatorValueSlider *mSecondValueSlider;

    QComboBox *mPropertyComboBox;
    QComboBox *mCompositionModeCombo;
};

#endif // BOXSINGLEWIDGET_H
