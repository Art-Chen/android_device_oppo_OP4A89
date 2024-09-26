package vendor.chen.aidl.syshelper;

@VintfStability
oneway interface IUdfpsCallback {
    void onUdfpsTouchStatusChanged(boolean down);
}