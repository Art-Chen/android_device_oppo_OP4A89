#include "aidl/vendor/chen/aidl/syshelper/IUdfpsHelperCallback.h"

#include <android/binder_parcel_utils.h>
#include <aidl/vendor/chen/aidl/syshelper/BnUdfpsHelperCallback.h>
#include <aidl/vendor/chen/aidl/syshelper/BpUdfpsHelperCallback.h>

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
static binder_status_t _aidl_vendor_chen_aidl_syshelper_IUdfpsHelperCallback_onTransact(AIBinder* _aidl_binder, transaction_code_t _aidl_code, const AParcel* _aidl_in, AParcel* _aidl_out) {
  (void)_aidl_in;
  (void)_aidl_out;
  binder_status_t _aidl_ret_status = STATUS_UNKNOWN_TRANSACTION;
  std::shared_ptr<BnUdfpsHelperCallback> _aidl_impl = std::static_pointer_cast<BnUdfpsHelperCallback>(::ndk::ICInterface::asInterface(_aidl_binder));
  switch (_aidl_code) {
    case (FIRST_CALL_TRANSACTION + 0 /*onUdfpsTouchStatusChanged*/): {
      bool in_isDown;

      _aidl_ret_status = ::ndk::AParcel_readData(_aidl_in, &in_isDown);
      if (_aidl_ret_status != STATUS_OK) break;

      ::ndk::ScopedAStatus _aidl_status = _aidl_impl->onUdfpsTouchStatusChanged(in_isDown);
      _aidl_ret_status = STATUS_OK;
      break;
    }
  }
  return _aidl_ret_status;
}

static AIBinder_Class* _g_aidl_vendor_chen_aidl_syshelper_IUdfpsHelperCallback_clazz = ::ndk::ICInterface::defineClass(IUdfpsHelperCallback::descriptor, _aidl_vendor_chen_aidl_syshelper_IUdfpsHelperCallback_onTransact);

BpUdfpsHelperCallback::BpUdfpsHelperCallback(const ::ndk::SpAIBinder& binder) : BpCInterface(binder) {}
BpUdfpsHelperCallback::~BpUdfpsHelperCallback() {}

::ndk::ScopedAStatus BpUdfpsHelperCallback::onUdfpsTouchStatusChanged(bool in_isDown) {
  binder_status_t _aidl_ret_status = STATUS_OK;
  ::ndk::ScopedAStatus _aidl_status;
  ::ndk::ScopedAParcel _aidl_in;
  ::ndk::ScopedAParcel _aidl_out;

  _aidl_ret_status = AIBinder_prepareTransaction(asBinder().get(), _aidl_in.getR());
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = ::ndk::AParcel_writeData(_aidl_in.get(), in_isDown);
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AIBinder_transact(
    asBinder().get(),
    (FIRST_CALL_TRANSACTION + 0 /*onUdfpsTouchStatusChanged*/),
    _aidl_in.getR(),
    _aidl_out.getR(),
    FLAG_ONEWAY
    #ifdef BINDER_STABILITY_SUPPORT
    | FLAG_PRIVATE_LOCAL
    #endif  // BINDER_STABILITY_SUPPORT
    );
  if (_aidl_ret_status == STATUS_UNKNOWN_TRANSACTION && IUdfpsHelperCallback::getDefaultImpl()) {
    _aidl_status = IUdfpsHelperCallback::getDefaultImpl()->onUdfpsTouchStatusChanged(in_isDown);
    goto _aidl_status_return;
  }
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_error:
  _aidl_status.set(AStatus_fromStatus(_aidl_ret_status));
  _aidl_status_return:
  return _aidl_status;
}
// Source for BnUdfpsHelperCallback
BnUdfpsHelperCallback::BnUdfpsHelperCallback() {}
BnUdfpsHelperCallback::~BnUdfpsHelperCallback() {}
::ndk::SpAIBinder BnUdfpsHelperCallback::createBinder() {
  AIBinder* binder = AIBinder_new(_g_aidl_vendor_chen_aidl_syshelper_IUdfpsHelperCallback_clazz, static_cast<void*>(this));
  #ifdef BINDER_STABILITY_SUPPORT
  AIBinder_markVintfStability(binder);
  #endif  // BINDER_STABILITY_SUPPORT
  return ::ndk::SpAIBinder(binder);
}
// Source for IUdfpsHelperCallback
const char* IUdfpsHelperCallback::descriptor = "vendor.chen.aidl.syshelper.IUdfpsHelperCallback";
IUdfpsHelperCallback::IUdfpsHelperCallback() {}
IUdfpsHelperCallback::~IUdfpsHelperCallback() {}


std::shared_ptr<IUdfpsHelperCallback> IUdfpsHelperCallback::fromBinder(const ::ndk::SpAIBinder& binder) {
  if (!AIBinder_associateClass(binder.get(), _g_aidl_vendor_chen_aidl_syshelper_IUdfpsHelperCallback_clazz)) { return nullptr; }
  std::shared_ptr<::ndk::ICInterface> interface = ::ndk::ICInterface::asInterface(binder.get());
  if (interface) {
    return std::static_pointer_cast<IUdfpsHelperCallback>(interface);
  }
  return ::ndk::SharedRefBase::make<BpUdfpsHelperCallback>(binder);
}

binder_status_t IUdfpsHelperCallback::writeToParcel(AParcel* parcel, const std::shared_ptr<IUdfpsHelperCallback>& instance) {
  return AParcel_writeStrongBinder(parcel, instance ? instance->asBinder().get() : nullptr);
}
binder_status_t IUdfpsHelperCallback::readFromParcel(const AParcel* parcel, std::shared_ptr<IUdfpsHelperCallback>* instance) {
  ::ndk::SpAIBinder binder;
  binder_status_t status = AParcel_readStrongBinder(parcel, binder.getR());
  if (status != STATUS_OK) return status;
  *instance = IUdfpsHelperCallback::fromBinder(binder);
  return STATUS_OK;
}
bool IUdfpsHelperCallback::setDefaultImpl(const std::shared_ptr<IUdfpsHelperCallback>& impl) {
  // Only one user of this interface can use this function
  // at a time. This is a heuristic to detect if two different
  // users in the same process use this function.
  assert(!IUdfpsHelperCallback::default_impl);
  if (impl) {
    IUdfpsHelperCallback::default_impl = impl;
    return true;
  }
  return false;
}
const std::shared_ptr<IUdfpsHelperCallback>& IUdfpsHelperCallback::getDefaultImpl() {
  return IUdfpsHelperCallback::default_impl;
}
std::shared_ptr<IUdfpsHelperCallback> IUdfpsHelperCallback::default_impl = nullptr;
::ndk::ScopedAStatus IUdfpsHelperCallbackDefault::onUdfpsTouchStatusChanged(bool /*in_isDown*/) {
  ::ndk::ScopedAStatus _aidl_status;
  _aidl_status.set(AStatus_fromStatus(STATUS_UNKNOWN_TRANSACTION));
  return _aidl_status;
}
::ndk::SpAIBinder IUdfpsHelperCallbackDefault::asBinder() {
  return ::ndk::SpAIBinder();
}
bool IUdfpsHelperCallbackDefault::isRemote() {
  return false;
}
}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
