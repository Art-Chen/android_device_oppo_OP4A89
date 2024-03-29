/*
 * Copyright (C) 2021 The Android Open Source Project
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

#pragma once

#include <aidl/vendor/chen/aidl/syshelper/IUdfpsHelperCallback.h>

#include <android-base/macros.h>
#include <android/binder_auto_utils.h>
#include "UdfpsHelper.h"

#include <memory>

namespace aidl::vendor::chen::aidl::syshelper {

// Type of the cookie pointer in linkToDeath.
// A (UdfpsHelper, IUdfpsHelperCallback) tuple.
class LinkedCallback {
  public:
    // Automatically linkToDeath upon construction with the returned object as the cookie.
    // service->death_reciepient() should be from CreateDeathRecipient().
    // Not using a strong reference to |service| to avoid circular reference. The lifetime
    // of |service| must be longer than this LinkedCallback object.
   static std::unique_ptr<LinkedCallback> Make(std::shared_ptr<UdfpsHelper> service,
                                               std::shared_ptr<IUdfpsHelperCallback> callback);

   // Automatically unlinkToDeath upon destruction. So, it is always safe to reinterpret_cast
   // the cookie back to the LinkedCallback object.
   ~LinkedCallback();

   // The wrapped IUdfpsHelperCallback object.
   const std::shared_ptr<IUdfpsHelperCallback>& callback() const { return callback_; }

   // On callback died, unreigster it from the service.
   void OnCallbackDied();

  private:
    LinkedCallback();
    DISALLOW_COPY_AND_ASSIGN(LinkedCallback);

    std::shared_ptr<UdfpsHelper> service();

    std::weak_ptr<UdfpsHelper> service_;
    std::shared_ptr<IUdfpsHelperCallback> callback_;
};

}  // namespace aidl::vendor::chen::aidl::syshelper
