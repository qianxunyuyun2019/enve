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

#ifndef VIDEOFRAMELOADER_H
#define VIDEOFRAMELOADER_H
#include "Tasks/updatable.h"
#include "skia/skiaincludes.h"
#include "videocachehandler.h"
extern "C" {
    #include <libavutil/opt.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}

struct VideoStreamsData;
class VideoFrameLoader : public eHddTask {
    e_OBJECT
protected:
    VideoFrameLoader(VideoFrameHandler * const cacheHandler,
                     const stdsptr<VideoStreamsData>& openedVideo,
                     const int frameId) :
        mCacheHandler(cacheHandler), mOpenedVideo(openedVideo),
        mFrameId(frameId) {}
    VideoFrameLoader(VideoFrameHandler * const cacheHandler,
                     const stdsptr<VideoStreamsData>& openedVideo,
                     const int frameId, AVFrame* const frame);

    void afterProcessing();
    void afterCanceled();

    void scheduleTaskNow();

    void setFrameToConvert(AVFrame * const frame,
                           AVCodecContext * const codecContext) {
        cleanUp();
        mFrameToConvert = frame;
        mSwsContext = sws_getContext(codecContext->width,
                                     codecContext->height,
                                     codecContext->pix_fmt,
                                     codecContext->width,
                                     codecContext->height,
                                     AV_PIX_FMT_RGBA, SWS_BICUBIC,
                                     nullptr, nullptr, nullptr);
    }
public:
    ~VideoFrameLoader() {
        for(auto& excess : mExcessFrames) {
            av_frame_unref(excess.second);
            av_frame_free(&excess.second);
        }
        cleanUp();
    }
    void process();
private:
    void cleanUp() {
        if(mFrameToConvert) {
            av_frame_unref(mFrameToConvert);
            av_frame_free(&mFrameToConvert);
            mFrameToConvert = nullptr;
        }
        if(mSwsContext) {
            sws_freeContext(mSwsContext);
            mSwsContext = nullptr;
        }
    }
    void setupSwsContext(AVCodecContext * const codecContext);
    void readFrame();
    void convertFrame();

    const qptr<VideoFrameHandler> mCacheHandler;
    const stdsptr<VideoStreamsData> mOpenedVideo;
    const int mFrameId;
    sk_sp<SkImage> mLoadedFrame;

    QList<std::pair<int, AVFrame*>> mExcessFrames;

    AVFrame * mFrameToConvert = nullptr;
    struct SwsContext * mSwsContext = nullptr;
};

#endif // VIDEOFRAMELOADER_H
