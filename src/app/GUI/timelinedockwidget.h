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

#ifndef BOXESLISTANIMATIONDOCKWIDGET_H
#define BOXESLISTANIMATIONDOCKWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <qscrollarea.h>
#include <QScrollArea>
#include <QApplication>
#include <QScrollBar>
#include <QComboBox>
#include <QMenuBar>
#include <QLineEdit>
#include <QWidgetAction>
#include <QToolBar>
#include <QStackedWidget>
#include "smartPointers/ememory.h"
#include "framerange.h"
#include "timelinebasewrappernode.h"
#include "triggerlabel.h"
#include "BrushWidgets/bookmarkedbrushes.h"
#include "qdoubleslider.h"

class FrameScrollBar;
class TimelineWidget;
class RenderWidget;
class MainWindow;
class AnimationDockWidget;
class RenderWidget;
class ActionButton;
class Canvas;
class Document;
class LayoutHandler;
class SwitchButton;
class BrushLabel;
class BrushContexedWrapper;

enum class CanvasMode : short;

class TimelineDockWidget : public QWidget {
public:
    explicit TimelineDockWidget(Document &document,
                                LayoutHandler* const layoutH,
                                MainWindow * const parent);
    bool processKeyPress(QKeyEvent *event);
    void previewFinished();
    void previewBeingPlayed();
    void previewBeingRendered();
    void previewPaused();

    void updateSettingsForCurrentCanvas(Canvas * const canvas);
    void clearAll();

    RenderWidget *getRenderWidget();
private:
    void setResolutionFractionText(QString text);

    void interruptPreview();

    void setLocalPivot(const bool local);

    void setTimelineMode();
    void setRenderMode();
    void playPreview();
    void renderPreview();
    void pausePreview();
    void resumePreview();

    void updateButtonsVisibility(const CanvasMode mode);
private:
    QAction *addSpaceToToolbar();

    void setBrushColor(const QColor& color);
    void setBrush(BrushContexedWrapper* const brush);

    void setSculptBrushColor(const QColor& color);
    void setupSculptBrushSizeButtons(const QString &iconsDir);
    void setupSculptBrushColorLabel();
    void setupSculptModeButtons(const QString& iconsDir);
    void setupSculptTargetButtons(const QString& iconsDir);
    void setupSculptValueSpins();
    void sculptModeChanged();
    void sculptTargetChanged();

    void updateSculptPositionEnabled();

    Document& mDocument;
    MainWindow* const mMainWindow;
    QStackedWidget* const mTimelineLayout;

    QToolBar *mToolBar;

    QVBoxLayout *mMainLayout;
    QComboBox *mResolutionComboBox;

    ActionButton *mPlayFromBeginningButton;
    SwitchButton *mPlayButton;
    ActionButton *mStopButton;

    QAction* mLocalPivotAct;
    SwitchButton *mLocalPivot;
    QAction* mNodeVisibilityAct;
    SwitchButton *mNodeVisibility;

    QAction* mBrushColorWidgetAct;
    TriggerLabel* mColorLabel;
    BrushLabel* mBrushLabel;
    QAction* mDecBrushSizeAct;
    ActionButton *mDecBrushSize;
    QAction* mBrushSizeLabelAct;
    QLabel* mBrushSizeLabel;
    QAction* mIncBrushSizeAct;
    ActionButton *mIncBrushSize;

    QAction* mOnionAct;
    SwitchButton *mOnion;

    QAction* mPaintModeSpace;

    QAction* mPaintNormalModeAct;
    SwitchButton *mPaintNormalMode;
    QAction* mPaintEraseModeAct;
    SwitchButton *mPaintEraseMode;
    QAction* mPaintLockAlphaModeAct;
    SwitchButton *mPaintLockAlphaMode;
    QAction* mPaintColorizeModeAct;
    SwitchButton *mPaintColorizeMode;

    QAction* mPaintModesSeparator;

    QAction* mPaintMoveModeAct;
    SwitchButton *mPaintMoveMode;
    QAction* mPaintCropModeAct;
    SwitchButton *mPaintCropMode;

    //
    QAction* mDecSculptBrushSizeAct;
    ActionButton* mDecSculptBrushSize;
    QAction* mSculptBrushSizeLabelAct;
    QLabel* mSculptBrushSizeLabel;
    QAction* mIncSculptBrushSizeAct;
    ActionButton* mIncSculptBrushSize;

    TriggerLabel* mSculptColorLabel;
    QAction* mSculptColorLabelAct;

    QAction* mSculptNodeVisibilityAct;
    SwitchButton *mSculptNodeVisibility;
    QAction* mSculptModeNodeVisibilitySpace;

    QAction* mDragModeAct;
    SwitchButton* mDragMode;
    QAction* mAddModeAct;
    SwitchButton* mAddMode;
    QAction* mReplaceModeAct;
    SwitchButton* mReplaceMode;
    QAction* mSubtractModeAct;
    SwitchButton* mSubtractMode;

    QAction* mSculptModeTargetSeperator;

    QAction* mPositionTargetAct;
    SwitchButton* mPositionTarget;
    QAction* mWidthTargetAct;
    SwitchButton* mWidthTarget;
    QAction* mPressureTargetAct;
    SwitchButton* mPressureTarget;
    QAction* mSpacingTargetAct;
    SwitchButton* mSpacingTarget;
    QAction* mTimeTargetAct;
    SwitchButton* mTimeTarget;
    QAction* mColorTargetAct;
    SwitchButton* mColorTarget;

    QAction* mSculptSpace0;

    QDoubleSlider* mValue;
    QAction* mValueAct;

    QAction* mSculptSpace1;

    QDoubleSlider* mHardness;
    QAction* mHardnessAct;

    QAction* mSculptSpace2;

    QDoubleSlider* mOpacity;
    QAction* mOpacityAct;
    //
    QAction *mTimelineAction;
    QAction *mRenderAction;

    QList<TimelineWidget*> mTimelineWidgets;
    RenderWidget *mRenderWidget;
    AnimationDockWidget *mAnimationDockWidget;
};

#endif // BOXESLISTANIMATIONDOCKWIDGET_H
