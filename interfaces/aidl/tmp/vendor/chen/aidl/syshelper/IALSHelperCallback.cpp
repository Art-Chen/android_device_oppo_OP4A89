#include "aidl/vendor/chen/aidl/syshelper/IALSHelperCallback.h"

#include <android/binder_parcel_utils.h>
#include <aidl/vendor/chen/aidl/syshelper/BnALSHelperCallback.h>
#include <aidl/vendor/chen/aidl/syshelper/BpALSHelperCallback.h>

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
static binder_status_t _aidl_vendor_chen_aidl_syshelper_IALSHelperCallback_onTransact(AIBinder* _aidl_binder, transaction_code_t _aidl_code, const AParcel* _aidl_in, AParcel* _aidl_out) {
  (void)_aidl_in;
  (void)_aidl_out;
  binder_status_t _aidl_ret_status = STATUS_UNKNOWN_TRANSACTION;
  std::shared_ptr<BnALSHelperCallback> _aidl_impl = std::static_pointer_cast<BnALSHelperCallback>(::ndk::ICInterface::asInterface(_aidl_binder));
  switch (_aidl_code) {
    case (FIRST_CALL_TRANSACTION + 0 /*takeScreenShot*/): {
      ::aidl::vendor::chen::aidl::syshelper::ScreenShotInfo _aidl_return;

      ::ndk::ScopedAStatus _aidl_status = _aidl_impl->takeScreenShot(&_aidl_return);
      _aidl_ret_status = AParcel_writeStatusHeader(_aidl_out, _aidl_status.get());
      if (_aidl_ret_status != STATUS_OK) break;

      if (!AStatus_isOk(_aidl_status.get())) break;

      _aidl_ret_status = ::ndk::AParcel_writeData(_aidl_out, _aidl_return);
      if (_aidl_ret_status != STATUS_OK) break;

      break;
    }
  }
  return _aidl_ret_status;
}

static AIBinder_Class* _g_aidl_vendor_chen_aidl_syshelper_IALSHelperCallback_clazz = ::ndk::ICInterface::defineClass(IALSHelperCallback::descriptor, _aidl_vendor_chen_aidl_syshelper_IALSHelperCallback_onTransact);

BpALSHelperCallback::BpALSHelperCallback(const ::ndk::SpAIBinder& binder) : BpCInterface(binder) {}
BpALSHelperCallback::~BpALSHelperCallback() {}

::ndk::ScopedAStatus BpALSHelperCallback::takeScreenShot(::aidl::vendor::chen::aidl::syshelper::ScreenShotInfo* _aidl_return) {
  binder_status_t _aidl_ret_status = STATUS_OK;
  ::ndk::ScopedAStatus _aidl_status;
  ::ndk::ScopedAParcel _aidl_in;
  ::ndk::ScopedAParcel _aidl_out;

  _aidl_ret_status = AIBinder_prepareTransaction(asBinder().get(), _aidl_in.getR());
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AIBinder_transact(
    asBinder().get(),
    (FIRST_CALL_TRANSACTION + 0 /*takeScreenShot*/),
    _aidl_in.getR(),
    _aidl_out.getR(),
    0
    #ifdef BINDER_STABILITY_SUPPORT
    | FLAG_PRIVATE_LOCAL
    #endif  // BINDER_STABILITY_SUPPORT
    );
  if (_aidl_ret_status == STATUS_UNKNOWN_TRANSACTION && IALSHelperCallback::getDefaultImpl()) {
    _aidl_status = IALSHelperCallback::getDefaultImpl()->takeScreenShot(_aidl_return);
    goto _aidl_status_return;
  }
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AParcel_readStatusHeader(_aidl_out.get(), _aidl_status.getR());
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  if (!AStatus_isOk(_aidl_status.get())) goto _aidl_status_return;
  _aidl_ret_status = ::ndk::AParcel_readData(_aidl_out.get(), _aidl_return);
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_error:
  _aidl_status.set(AStatus_fromStatus(_aidl_ret_status));
  _aidl_status_return:
  return _aidl_status;
}
// Source for BnALSHelperCallback
BnALSHelperCallback::BnALSHelperCallback() {}
BnALSHelperCallback::~BnALSHelperCallback() {}
::ndk::SpAIBinder BnALSHelperCallback::createBinder() {
  AIBinder* binder = AIBinder_new(_g_aidl_vendor_chen_aidl_syshelper_IALSHelperCallback_clazz, static_cast<void*>(this));
  #ifdef BINDER_STABILITY_SUPPORT
  AIBinder_markVintfStability(binder);
  #endif  // BINDER_STABILITY_SUPPORT
  return ::ndk::SpAIBinder(binder);
}
// Source for IALSHelperCallback
const char* IALSHelperCallback::descriptor = "vendor.chen.aidl.syshelper.IALSHelperCallback";
IALSHelperCallback::IALSHelperCallback() {}
IALSHelperCallback::~IALSHelperCallback() {}


std::shared_ptr<IALSHelperCallback> IALSHelperCallback::fromBinder(const ::ndk::SpAIBinder& binder) {
  if (!AIBinder_associateClass(binder.get(), _g_aidl_vendor_chen_aidl_syshelper_IALSHelperCallback_clazz)) { return nullptr; }
  std::shared_ptr<::ndk::ICInterface> interface = ::ndk::ICInterface::asInterface(binder.get());
  if (interface) {
    return std::static_pointer_cast<IALSHelperCallback>(interface);
  }
  return ::ndk::SharedRefBase::make<BpALSHelperCallback>(binder);
}

binder_status_t IALSHelperCallback::writeToParcel(AParcel* parcel, const std::shared_ptr<IALSHelperCallback>& instance) {
  return AParcel_writeStrongBinder(parcel, instance ? instance->asBinder().get() : nullptr);
}
binder_status_t IALSHelperCallback::readFromParcel(const AParcel* parcel, std::shared_ptr<IALSHelperCallback>* instance) {
  ::ndk::SpAIBinder binder;
  binder_status_t status = AParcel_readStrongBinder(parcel, binder.getR());
  if (status != STATUS_OK) return status;
  *instance = IALSHelperCallback::fromBinder(binder);
  return STATUS_OK;
}
bool IALSHelperCallback::setDefaultImpl(const std::shared_ptr<IALSHelperCallback>& impl) {
  // Only one user of this interface can use this function
  // at a time. This is a heuristic to detect if two different
  // users in the same process use this function.
  assert(!IALSHelperCallback::default_impl);
  if (impl) {
    IALSHelperCallback::default_impl = impl;
    return true;
  }
  return false;
}
const std::shared_ptr<IALSHelperCallback>& IALSHelperCallback::getDefaultImpl() {
  return IALSHelperCallback::default_impl;
}
std::shared_ptr<IALSHelperCallback> IALSHelperCallback::default_impl = nullptr;
::ndk::ScopedAStatus IALSHelperCallbackDefault::takeScreenShot(::aidl::vendor::chen::aidl::syshelper::ScreenShotInfo* /*_aidl_return*/) {
  ::ndk::ScopedAStatus _aidl_status;
  _aidl_status.set(AStatus_fromStatus(STATUS_UNKNOWN_TRANSACTION));
  return _aidl_status;
}
::ndk::SpAIBinder IALSHelperCallbackDefault::asBinder() {
  return ::ndk::SpAIBinder();
}
bool IALSHelperCallbackDefault::isRemote() {
  return false;
}
}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
