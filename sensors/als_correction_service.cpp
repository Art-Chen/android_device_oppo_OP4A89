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

#include <aidl/vendor/chen/aidl/syshelper/BnALSHelper.h>
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
        static sp<GraphicBuffer> outBuffer = new GraphicBuffer(
            screenshot_rect.getWidth(), screenshot_rect.getHeight(),
            android::PIXEL_FORMAT_RGB_888,
            GraphicBuffer::USAGE_SW_READ_OFTEN | GraphicBuffer::USAGE_SW_WRITE_OFTEN);

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
             if (captureResults.fenceResult.ok()) outBuffer = captureResults.buffer;
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
};

class ALSHelper : public ::aidl::vendor::chen::aidl::syshelper::BnALSHelper {
   public:
    ALSHelper(AlsCorrection* listener) : listener_(listener) {}
    ::ndk::ScopedAStatus takeScreenShot(::aidl::vendor::chen::aidl::syshelper::ScreenShotInfo* _aidl_return) override {
        ScreenShotInfo ret = listener_->takeScreenshot();
        _aidl_return->r = ret.r;
        _aidl_return->g = ret.g;
        _aidl_return->b = ret.b;
        _aidl_return->timestamp = ret.timestamp;
        return ndk::ScopedAStatus::ok();
    };

   protected:
    AlsCorrection* listener_;
};

int main() {
    ProcessState::self()->setThreadPoolMaxThreadCount(3);
    ProcessState::self()->startThreadPool();

    auto listener = new AlsCorrection();

    std::shared_ptr<ALSHelper> alsHelper = ndk::SharedRefBase::make<ALSHelper>(listener);

    std::string instance = std::string() + ALSHelper::descriptor + "/default";
    binder_status_t status =
        AServiceManager_addService(alsHelper->asBinder().get(), instance.c_str());
    CHECK_EQ(status, STATUS_OK);

    LOG(INFO) << "Als Screen Shoter init done!";
    ABinderProcess_joinThreadPool();
    return 0;
}
