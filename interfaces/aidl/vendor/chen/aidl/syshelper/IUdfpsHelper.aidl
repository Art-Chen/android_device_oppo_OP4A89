package vendor.chen.aidl.syshelper;

import vendor.chen.aidl.syshelper.IUdfpsCallback;

@VintfStability
interface IUdfpsHelper {
    boolean getTouchStatus();
    void registerCallback(in IUdfpsCallback cb);
    void unregisterCallback(in IUdfpsCallback cb);
}