#include <ALSHelper.h>
#include <android-base/logging.h>

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {

::ndk::ScopedAStatus ALSHelper::takeScreenShot(::aidl::vendor::chen::aidl::syshelper::ScreenShotInfo* _aidl_return) {
    LOG(VERBOSE) << "in takeScreenShot";
    if (callback_ == nullptr) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    }
    ScreenShotInfo info;
    callback_->takeScreenShot(&info);
    LOG(VERBOSE) << info.toString();
    *_aidl_return = info;
    return ndk::ScopedAStatus::ok();
};

}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
