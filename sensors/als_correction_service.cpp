/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <aidl/vendor/chen/aidl/syshelper/IALSHelper.h>
#include <aidl/vendor/chen/aidl/syshelper/BnALSHelperCallback.h>
#include <aidl/vendor/chen/aidl/syshelper/IALSHelperCallback.h>
#include <aidl/vendor/chen/aidl/syshelper/ScreenShotInfo.h>

#include <android-base/properties.h>
#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/SyncScreenCaptureListener.h>
#include <signal.h>
#include <sysutils/FrameworkCommand.h>
#include <sysutils/FrameworkListener.h>
#include <ui/DisplayState.h>
#include <unistd.h>
#include <utils/Timers.h>

#include <cstdio>

using android::base::SetProperty;
using android::ui::Rotation;
using android::GraphicBuffer;
using android::IBinder;
using android::Rect;
using android::ScreenshotClient;
using android::sp;
using android::SurfaceComposerClient;
using namespace android;

using aidl::vendor::chen::aidl::syshelper::IALSHelper;
using aidl::vendor::chen::aidl::syshelper::BnALSHelperCallback;
using aidl::vendor::chen::aidl::syshelper::ScreenShotInfo;

constexpr int ALS_POS_X = 692;
constexpr int ALS_POS_Y = 34;
constexpr int ALS_RADIUS = 15;

static Rect screenshot_rect_0(ALS_POS_X - ALS_RADIUS, ALS_POS_Y - ALS_RADIUS, ALS_POS_X + ALS_RADIUS, ALS_POS_Y + ALS_RADIUS);
static Rect screenshot_rect_land_90(ALS_POS_Y - ALS_RADIUS, 1080 - ALS_POS_X - ALS_RADIUS, ALS_POS_Y + ALS_RADIUS, 1080 - ALS_POS_X + ALS_RADIUS);
static Rect screenshot_rect_180(1080 - ALS_POS_X - ALS_RADIUS, 2400 - ALS_POS_Y - ALS_RADIUS, 1080 - ALS_POS_X + ALS_RADIUS, 2400 - ALS_POS_Y + ALS_RADIUS);
static Rect screenshot_rect_land_270(2400 - (ALS_POS_Y + ALS_RADIUS),ALS_POS_X - ALS_RADIUS, 2400 - (ALS_POS_Y - ALS_RADIUS), ALS_POS_X + ALS_RADIUS);

static sp<IBinder> getInternalDisplayToken() {
    const auto displayIds = SurfaceComposerClient::getPhysicalDisplayIds();
    sp<IBinder> token = SurfaceComposerClient::getPhysicalDisplayToken(displayIds[0]);
    return token;
}

class AlsCorrection {
   public:
    ScreenShotInfo takeScreenshot() {
        static sp<GraphicBuffer> outBuffer = new GraphicBuffer(
                screenshot_rect.getWidth(), screenshot_rect.getHeight(),
                android::PIXEL_FORMAT_RGB_888,
                GraphicBuffer::USAGE_SW_READ_OFTEN | GraphicBuffer::USAGE_SW_WRITE_OFTEN);

        sp<SyncScreenCaptureListener> captureListener = new SyncScreenCaptureListener();
        gui::ScreenCaptureResults captureResults;
        sp<IBinder> display = getInternalDisplayToken();
        DisplayCaptureArgs captureArgs = {};
        android::ui::DisplayState state = {};
        Rect screenshot_rect;
        switch (state.orientation) {
             case Rotation::Rotation90:  screenshot_rect = screenshot_rect_land_90;
                                         break;
             case Rotation::Rotation180: screenshot_rect = screenshot_rect_180;
                                         break;
             case Rotation::Rotation270: screenshot_rect = screenshot_rect_land_270;
                                         break;
             default:                    screenshot_rect = screenshot_rect_0;
                                         break;
        }

        SurfaceComposerClient::getDisplayState(display, &state);
        
        captureArgs.displayToken = display;
        captureArgs.pixelFormat = android::ui::PixelFormat::RGBA_8888;
        captureArgs.sourceCrop = screenshot_rect;
        captureArgs.width = screenshot_rect.getWidth();
        captureArgs.height = screenshot_rect.getHeight();
        captureArgs.useIdentityTransform = true;
        captureArgs.captureSecureLayers = true;
        if (ScreenshotClient::captureDisplay(captureArgs, captureListener) == NO_ERROR) {
             captureResults = captureListener->waitForResults();
             if (captureResults.result == NO_ERROR) outBuffer = captureResults.buffer;
        }

        uint8_t *out;
        auto resultWidth = outBuffer->getWidth();
        auto resultHeight = outBuffer->getHeight();
        auto stride = outBuffer->getStride();

        outBuffer->lock(GraphicBuffer::USAGE_SW_READ_OFTEN, reinterpret_cast<void **>(&out));
        // we can sum this directly on linear light
        uint32_t rsum = 0, gsum = 0, bsum = 0;
        for (int y = 0; y < resultHeight; y++) {
            for (int x = 0; x < resultWidth; x++) {
                rsum += out[y * (stride * 4) + x * 4];
                gsum += out[y * (stride * 4) + x * 4 + 1];
                bsum += out[y * (stride * 4) + x * 4 + 2];
            }
        }
        uint32_t max = resultWidth * resultHeight;
        outBuffer->unlock();

        ScreenShotInfo info = ScreenShotInfo();
        info.r = rsum / max;
        info.g = gsum / max;
        info.b = bsum / max;
        info.timestamp = systemTime(SYSTEM_TIME_BOOTTIME);

        return info;
    }

    void init();

   private:
    std::shared_ptr<IALSHelper> mChenALSHelper;
};

class ChenALSHelperCallback : public BnALSHelperCallback {
   public:
    ChenALSHelperCallback(AlsCorrection* thiz) : mThiz(thiz){};

    ::ndk::ScopedAStatus takeScreenShot(ScreenShotInfo* _aidl_return) override {
        ScreenShotInfo info = mThiz->takeScreenshot();
        *_aidl_return = info;
        return ::ndk::ScopedAStatus::ok();
    }

   private:
    AlsCorrection* mThiz;
};

void AlsCorrection::init() {
    std::string instanceName = std::string() + IALSHelper::descriptor + "/default";
    bool isSupportChenSysHelper = AServiceManager_isDeclared(instanceName.c_str());
    if (!isSupportChenSysHelper) {
        LOG(FATAL) << "Chen System Helper is NOT Declared!! Panic Here!";
    }

    mChenALSHelper = IALSHelper::fromBinder(ndk::SpAIBinder(AServiceManager_waitForService(instanceName.c_str())));

    const std::shared_ptr<ChenALSHelperCallback> mChenALSCallback = ndk::SharedRefBase::make<ChenALSHelperCallback>(this);
    mChenALSHelper->registerCallback(mChenALSCallback);

    LOG(INFO) << "Als Screen Shoter init done!";
}

int main() {
    ProcessState::self()->setThreadPoolMaxThreadCount(2);
    ProcessState::self()->startThreadPool();

    auto listener = new AlsCorrection();
    listener->init();

    // Don't know why we should call it first
    // If it didn't called, the takeScreenshot will hang at waitResult
    ScreenShotInfo info = listener->takeScreenshot();
    LOG(INFO) << info.toString();

    while (true) {
        pause();
    }

    return 0;
}
