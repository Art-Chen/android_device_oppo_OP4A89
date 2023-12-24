package vendor.chen.aidl.syshelper;

@VintfStability
oneway interface IUdfpsHelperCallback {
    void onUdfpsTouchStatusChanged(boolean isDown);
}