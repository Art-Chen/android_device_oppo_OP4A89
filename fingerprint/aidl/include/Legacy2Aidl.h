/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <aidl/android/hardware/keymaster/HardwareAuthToken.h>
#include <endian.h>
#include <hardware/hw_auth_token.h>
#include <hidl/HidlSupport.h>

using ::android::hardware::hidl_array;
using aidl::android::hardware::keymaster::HardwareAuthToken;

namespace aidl {
namespace android {
namespace hardware {
namespace biometrics {
namespace fingerprint {

inline void translate(const HardwareAuthToken& authToken, hw_auth_token_t& hat) {
    hat.challenge = authToken.challenge;
    hat.user_id = authToken.userId;
    hat.authenticator_id = authToken.authenticatorId;
    // these are in host order: translate to network order
    hat.authenticator_type = htobe32(static_cast<uint32_t>(authToken.authenticatorType));
    hat.timestamp = htobe64(authToken.timestamp.milliSeconds);
    std::copy(authToken.mac.begin(), authToken.mac.end(), hat.hmac);
}

inline void translate(const hw_auth_token_t& hat, HardwareAuthToken& authToken) {
    authToken.challenge = hat.challenge;
    authToken.userId = hat.user_id;
    authToken.authenticatorId = hat.authenticator_id;
    // these are in network order: translate to host
    authToken.authenticatorType =
        static_cast<keymaster::HardwareAuthenticatorType>(
            be32toh(hat.authenticator_type));
    authToken.timestamp.milliSeconds = be64toh(hat.timestamp);
    authToken.mac.insert(authToken.mac.begin(), std::begin(hat.hmac),
                         std::end(hat.hmac));
}

inline void translate(const hw_auth_token_t& hat, hidl_array<uint8_t, 69>& hat_hidl) {
    hat_hidl = hidl_array<uint8_t, 69>((uint8_t *) &hat);
}

inline int32_t RequestStatusToInt(RequestStatus error) {
    switch (error) {
        case RequestStatus::SYS_OK:
            return 0;
        case RequestStatus::SYS_ENOENT:
            return -2;
        case RequestStatus::SYS_EINTR:
            return -4;
        case RequestStatus::SYS_EIO:
            return -5;
        case RequestStatus::SYS_EAGAIN:
            return -11;
        case RequestStatus::SYS_ENOMEM:
            return -12;
        case RequestStatus::SYS_EACCES:
            return -13;
        case RequestStatus::SYS_EFAULT:
            return -14;
        case RequestStatus::SYS_EBUSY:
            return -16;
        case RequestStatus::SYS_EINVAL:
            return -22;
        case RequestStatus::SYS_ENOSPC:
            return -28;
        case RequestStatus::SYS_ETIMEDOUT:
            return -110;
        default:
            return -1;
    }
    return -1;
}

}  // namespace fingerprint
}  // namespace biometrics
}  // namespace hardware
}  // namespace android
}  // namespace aidl