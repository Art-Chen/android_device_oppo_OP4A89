package vendor.chen.aidl.syshelper;

import vendor.chen.aidl.syshelper.IALSHelperCallback;
import vendor.chen.aidl.syshelper.ScreenShotInfo;

@VintfStability
interface IALSHelper {
    ScreenShotInfo takeScreenShot();
    void registerCallback(in IALSHelperCallback callback);
    void unregisterCallback(in IALSHelperCallback callback);
}