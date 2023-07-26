package vendor.chen.aidl.syshelper;

@VintfStability
interface IUdfpsHelperCallback {
    void onUdfpsTouchStatusChanged(boolean isDown);
}