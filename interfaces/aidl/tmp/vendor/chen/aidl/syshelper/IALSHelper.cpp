#include "aidl/vendor/chen/aidl/syshelper/IALSHelper.h"

#include <android/binder_parcel_utils.h>
#include <aidl/vendor/chen/aidl/syshelper/BnALSHelper.h>
#include <aidl/vendor/chen/aidl/syshelper/BnALSHelperCallback.h>
#include <aidl/vendor/chen/aidl/syshelper/BpALSHelper.h>
#include <aidl/vendor/chen/aidl/syshelper/BpALSHelperCallback.h>
#include <aidl/vendor/chen/aidl/syshelper/IALSHelperCallback.h>

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
static binder_status_t _aidl_vendor_chen_aidl_syshelper_IALSHelper_onTransact(AIBinder* _aidl_binder, transaction_code_t _aidl_code, const AParcel* _aidl_in, AParcel* _aidl_out) {
  (void)_aidl_in;
  (void)_aidl_out;
  binder_status_t _aidl_ret_status = STATUS_UNKNOWN_TRANSACTION;
  std::shared_ptr<BnALSHelper> _aidl_impl = std::static_pointer_cast<BnALSHelper>(::ndk::ICInterface::asInterface(_aidl_binder));
  switch (_aidl_code) {
    case (FIRST_CALL_TRANSACTION + 0 /*registerCallback*/): {
      std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback> in_callback;

      _aidl_ret_status = ::ndk::AParcel_readData(_aidl_in, &in_callback);
      if (_aidl_ret_status != STATUS_OK) break;

      ::ndk::ScopedAStatus _aidl_status = _aidl_impl->registerCallback(in_callback);
      _aidl_ret_status = AParcel_writeStatusHeader(_aidl_out, _aidl_status.get());
      if (_aidl_ret_status != STATUS_OK) break;

      if (!AStatus_isOk(_aidl_status.get())) break;

      break;
    }
    case (FIRST_CALL_TRANSACTION + 1 /*unregisterCallback*/): {
      std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback> in_callback;

      _aidl_ret_status = ::ndk::AParcel_readData(_aidl_in, &in_callback);
      if (_aidl_ret_status != STATUS_OK) break;

      ::ndk::ScopedAStatus _aidl_status = _aidl_impl->unregisterCallback(in_callback);
      _aidl_ret_status = AParcel_writeStatusHeader(_aidl_out, _aidl_status.get());
      if (_aidl_ret_status != STATUS_OK) break;

      if (!AStatus_isOk(_aidl_status.get())) break;

      break;
    }
  }
  return _aidl_ret_status;
}

static AIBinder_Class* _g_aidl_vendor_chen_aidl_syshelper_IALSHelper_clazz = ::ndk::ICInterface::defineClass(IALSHelper::descriptor, _aidl_vendor_chen_aidl_syshelper_IALSHelper_onTransact);

BpALSHelper::BpALSHelper(const ::ndk::SpAIBinder& binder) : BpCInterface(binder) {}
BpALSHelper::~BpALSHelper() {}

::ndk::ScopedAStatus BpALSHelper::registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& in_callback) {
  binder_status_t _aidl_ret_status = STATUS_OK;
  ::ndk::ScopedAStatus _aidl_status;
  ::ndk::ScopedAParcel _aidl_in;
  ::ndk::ScopedAParcel _aidl_out;

  _aidl_ret_status = AIBinder_prepareTransaction(asBinder().get(), _aidl_in.getR());
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = ::ndk::AParcel_writeData(_aidl_in.get(), in_callback);
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AIBinder_transact(
    asBinder().get(),
    (FIRST_CALL_TRANSACTION + 0 /*registerCallback*/),
    _aidl_in.getR(),
    _aidl_out.getR(),
    0
    #ifdef BINDER_STABILITY_SUPPORT
    | FLAG_PRIVATE_LOCAL
    #endif  // BINDER_STABILITY_SUPPORT
    );
  if (_aidl_ret_status == STATUS_UNKNOWN_TRANSACTION && IALSHelper::getDefaultImpl()) {
    _aidl_status = IALSHelper::getDefaultImpl()->registerCallback(in_callback);
    goto _aidl_status_return;
  }
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AParcel_readStatusHeader(_aidl_out.get(), _aidl_status.getR());
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  if (!AStatus_isOk(_aidl_status.get())) goto _aidl_status_return;
  _aidl_error:
  _aidl_status.set(AStatus_fromStatus(_aidl_ret_status));
  _aidl_status_return:
  return _aidl_status;
}
::ndk::ScopedAStatus BpALSHelper::unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& in_callback) {
  binder_status_t _aidl_ret_status = STATUS_OK;
  ::ndk::ScopedAStatus _aidl_status;
  ::ndk::ScopedAParcel _aidl_in;
  ::ndk::ScopedAParcel _aidl_out;

  _aidl_ret_status = AIBinder_prepareTransaction(asBinder().get(), _aidl_in.getR());
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = ::ndk::AParcel_writeData(_aidl_in.get(), in_callback);
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AIBinder_transact(
    asBinder().get(),
    (FIRST_CALL_TRANSACTION + 1 /*unregisterCallback*/),
    _aidl_in.getR(),
    _aidl_out.getR(),
    0
    #ifdef BINDER_STABILITY_SUPPORT
    | FLAG_PRIVATE_LOCAL
    #endif  // BINDER_STABILITY_SUPPORT
    );
  if (_aidl_ret_status == STATUS_UNKNOWN_TRANSACTION && IALSHelper::getDefaultImpl()) {
    _aidl_status = IALSHelper::getDefaultImpl()->unregisterCallback(in_callback);
    goto _aidl_status_return;
  }
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  _aidl_ret_status = AParcel_readStatusHeader(_aidl_out.get(), _aidl_status.getR());
  if (_aidl_ret_status != STATUS_OK) goto _aidl_error;

  if (!AStatus_isOk(_aidl_status.get())) goto _aidl_status_return;
  _aidl_error:
  _aidl_status.set(AStatus_fromStatus(_aidl_ret_status));
  _aidl_status_return:
  return _aidl_status;
}
// Source for BnALSHelper
BnALSHelper::BnALSHelper() {}
BnALSHelper::~BnALSHelper() {}
::ndk::SpAIBinder BnALSHelper::createBinder() {
  AIBinder* binder = AIBinder_new(_g_aidl_vendor_chen_aidl_syshelper_IALSHelper_clazz, static_cast<void*>(this));
  #ifdef BINDER_STABILITY_SUPPORT
  AIBinder_markVintfStability(binder);
  #endif  // BINDER_STABILITY_SUPPORT
  return ::ndk::SpAIBinder(binder);
}
// Source for IALSHelper
const char* IALSHelper::descriptor = "vendor.chen.aidl.syshelper.IALSHelper";
IALSHelper::IALSHelper() {}
IALSHelper::~IALSHelper() {}


std::shared_ptr<IALSHelper> IALSHelper::fromBinder(const ::ndk::SpAIBinder& binder) {
  if (!AIBinder_associateClass(binder.get(), _g_aidl_vendor_chen_aidl_syshelper_IALSHelper_clazz)) { return nullptr; }
  std::shared_ptr<::ndk::ICInterface> interface = ::ndk::ICInterface::asInterface(binder.get());
  if (interface) {
    return std::static_pointer_cast<IALSHelper>(interface);
  }
  return ::ndk::SharedRefBase::make<BpALSHelper>(binder);
}

binder_status_t IALSHelper::writeToParcel(AParcel* parcel, const std::shared_ptr<IALSHelper>& instance) {
  return AParcel_writeStrongBinder(parcel, instance ? instance->asBinder().get() : nullptr);
}
binder_status_t IALSHelper::readFromParcel(const AParcel* parcel, std::shared_ptr<IALSHelper>* instance) {
  ::ndk::SpAIBinder binder;
  binder_status_t status = AParcel_readStrongBinder(parcel, binder.getR());
  if (status != STATUS_OK) return status;
  *instance = IALSHelper::fromBinder(binder);
  return STATUS_OK;
}
bool IALSHelper::setDefaultImpl(const std::shared_ptr<IALSHelper>& impl) {
  // Only one user of this interface can use this function
  // at a time. This is a heuristic to detect if two different
  // users in the same process use this function.
  assert(!IALSHelper::default_impl);
  if (impl) {
    IALSHelper::default_impl = impl;
    return true;
  }
  return false;
}
const std::shared_ptr<IALSHelper>& IALSHelper::getDefaultImpl() {
  return IALSHelper::default_impl;
}
std::shared_ptr<IALSHelper> IALSHelper::default_impl = nullptr;
::ndk::ScopedAStatus IALSHelperDefault::registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& /*in_callback*/) {
  ::ndk::ScopedAStatus _aidl_status;
  _aidl_status.set(AStatus_fromStatus(STATUS_UNKNOWN_TRANSACTION));
  return _aidl_status;
}
::ndk::ScopedAStatus IALSHelperDefault::unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& /*in_callback*/) {
  ::ndk::ScopedAStatus _aidl_status;
  _aidl_status.set(AStatus_fromStatus(STATUS_UNKNOWN_TRANSACTION));
  return _aidl_status;
}
::ndk::SpAIBinder IALSHelperDefault::asBinder() {
  return ::ndk::SpAIBinder();
}
bool IALSHelperDefault::isRemote() {
  return false;
}
}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
