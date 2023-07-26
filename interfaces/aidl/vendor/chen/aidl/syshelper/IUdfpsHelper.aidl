package vendor.chen.aidl.syshelper;

import vendor.chen.aidl.syshelper.IUdfpsHelperCallback;

@VintfStability
interface IUdfpsHelper {
    void touchDown();
    void touchUp();
    boolean getTouchStatus();
    void registerCallback(in IUdfpsHelperCallback callback);
    void unregisterCallback(in IUdfpsHelperCallback callback);
}