package vendor.chen.aidl.syshelper;

import vendor.chen.aidl.syshelper.ScreenShotInfo;

@VintfStability
interface IALSHelperCallback {
    ScreenShotInfo takeScreenShot();
}