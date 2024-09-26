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

#include <aidl/vendor/chen/aidl/syshelper/IUdfpsCallback.h>

#include <android-base/macros.h>
#include <android-base/result.h>
#include <android/binder_auto_utils.h>
#include "UdfpsHelper.h"

#include <memory>

namespace aidl::vendor::chen::aidl::syshelper {

// Type of the cookie pointer in linkToDeath.
// A (UdfpsHelper, IUdfpsCallback) tuple.
class LinkedCallback {
  public:
    // Automatically linkToDeath upon construction with the returned object as the cookie.
    // The deathRecipient owns the LinkedCallback object and will delete it with
    // cookie when it's unlinked.
    static ::android::base::Result<LinkedCallback*> Make(
            std::shared_ptr<UdfpsHelper> service, std::shared_ptr<IUdfpsCallback> callback);
   // On callback died, unreigster it from the service.
   void OnCallbackDied();

  private:
    LinkedCallback();
    DISALLOW_COPY_AND_ASSIGN(LinkedCallback);

    std::shared_ptr<UdfpsHelper> service();

    std::weak_ptr<UdfpsHelper> service_;
    std::weak_ptr<IUdfpsCallback> callback_;
};

}  // namespace aidl::vendor::chen::aidl::syshelper
