﻿#ifndef BOXESLISTANIMATIONDOCKWIDGET_H
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
#include "smartPointers/sharedpointerdefs.h"
class FrameScrollBar;
class BoxesListKeysViewWidget;
class VWidgetStack;
class RenderWidget;
class MainWindow;
class AnimationDockWidget;
class RenderWidget;
class ActionButton;
class Canvas;
class Document;

class ChangeWidthWidget : public QWidget {
    Q_OBJECT
public:
    ChangeWidthWidget(QWidget *parent = nullptr);

    void updatePos();

    void paintEvent(QPaintEvent *);

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

    void enterEvent(QEvent *);

    void leaveEvent(QEvent *);
    int getCurrentWidth() {
        return mCurrentWidth;
    }

signals:
    void widthSet(int);
private:
    bool mHover = false;
    bool mPressed = false;
    int mCurrentWidth = 400;
    int mPressX;
};

class BoxesListAnimationDockWidget : public QWidget {
    Q_OBJECT
public:
    explicit BoxesListAnimationDockWidget(Document &document, MainWindow *parent);
    bool processKeyEvent(QKeyEvent *event);
    void previewFinished();
    void previewBeingPlayed();
    void previewBeingRendered();
    void previewPaused();

    void updateSettingsForCurrentCanvas(Canvas * const canvas);
    void addNewBoxesListKeysViewWidgetBelow(
            BoxesListKeysViewWidget *widget);
    void clearAll();

    RenderWidget *getRenderWidget();

    void setCurrentFrame(const int frame);
    void setViewedFrameRange(const int minFrame,
                             const int maxFrame);
    void setCanvasFrameRange(const int minFrame,
                                const int maxFrame);
    void addNewBoxesListKeysViewWidget(int id = 0);
    void removeBoxesListKeysViewWidget(
            BoxesListKeysViewWidget *widget);
signals:
    void viewedVerticalRangeChanged(int, int);
private slots:
    void setResolutionFractionText(QString text);

    void interruptPreview();

    void setLocalPivot(const bool bT);

    void setTimelineMode();
    void setRenderMode();
    void playPreview();
    void renderPreview();
    void pausePreview();
    void resumePreview();
private:
    Document& mDocument;

    QToolBar *mToolBar;

    QWidget *mTimelineWidget;
    QVBoxLayout *mTimelineLayout;
    QVBoxLayout *mMainLayout;

    QLabel *mControlButtonsWidget;

    QComboBox *mResolutionComboBox;

    ActionButton *mPlayButton;
    ActionButton *mStopButton;
    ActionButton *mLocalPivot;

    QAction *mTimelineAction;
    QAction *mRenderAction;

    VWidgetStack *mBoxesListKeysViewStack;
    QList<BoxesListKeysViewWidget*> mBoxesListKeysViewWidgets;
    RenderWidget *mRenderWidget;
    AnimationDockWidget *mAnimationDockWidget;

    ChangeWidthWidget *mChww;

    MainWindow *mMainWindow;

    FrameScrollBar *mFrameRangeScrollbar;
    FrameScrollBar *mAnimationWidgetScrollbar;
};

#endif // BOXESLISTANIMATIONDOCKWIDGET_H
