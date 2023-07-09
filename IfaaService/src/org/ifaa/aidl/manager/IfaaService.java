package org.ifaa.aidl.manager;

import android.app.KeyguardManager;
import android.app.Service;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.hardware.fingerprint.Fingerprint;
import android.hardware.fingerprint.FingerprintManager;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import java.util.ArrayList;
import java.util.List;
import org.ifaa.aidl.manager.IfaaManagerService;
import vendor.oplus.hardware.biometrics.fingerprintpay.V1_0.IFingerprintPay;

public class IfaaService extends Service {
    public static final int AUTH_TYPE_FINGERPRINT = 1;
    public static final int AUTH_TYPE_IRIS = 2;
    public static final int AUTH_TYPE_NOT_SUPPORT = 0;
    public static final int AUTH_TYPE_OPTICAL_FINGERPRINT = 17;
    public static final int COMMAND_FAIL = -1;
    public static final int COMMAND_OK = 0;

    private IfaaManagerService.Stub mBinder = new IfaaManagerService.Stub() {
        @Override // org.ifaa.aidl.manager.IfaaManagerService
        public int getSupportBIOTypes() {
            return IfaaService.this.getSupportBIOTypesImpl();
        }

        @Override // org.ifaa.aidl.manager.IfaaManagerService
        public int startBIOManager(int i) {
            return IfaaService.this.startBIOManagerImpl(i);
        }

        @Override // org.ifaa.aidl.manager.IfaaManagerService
        public String getDeviceModel() {
            return IfaaService.this.getDeviceModelImpl();
        }

        @Override // org.ifaa.aidl.manager.IfaaManagerService
        public byte[] processCmd(byte[] bArr) {
            return IfaaService.this.processCmdImpl(bArr);
        }

        @Override // org.ifaa.aidl.manager.IfaaManagerService
        public int getVersion() {
            return IfaaService.this.getVersionImpl();
        }

        @Override // org.ifaa.aidl.manager.IfaaManagerService
        public String getExtInfo(int i, String str) {
            return IfaaService.this.getExtInfoImpl(i, str);
        }

        @Override // org.ifaa.aidl.manager.IfaaManagerService
        public void setExtInfo(int i, String str, String str2) {
            IfaaService.this.setExtInfoImpl(i, str, str2);
        }

        @Override // org.ifaa.aidl.manager.IfaaManagerService
        public int getEnabled(int i) {
            int enabledImpl = IfaaService.this.getEnabledImpl(i);
            Log.i("IfaaService", "getEnabled type=" + i + ", ret=" + enabledImpl);
            return enabledImpl;
        }

        @Override // org.ifaa.aidl.manager.IfaaManagerService
        public int[] getIDList(int i) {
            return IfaaService.this.getIDListImpl(i);
        }
    };
    private IFingerprintPay mFingerprintPay = null;
    private FingerprintManager mFPManager;

    public void setExtInfoImpl(int i, String str, String str2) {
    }

    static {
    }

    @Override // android.app.Service
    public IBinder onBind(Intent intent) {
        return this.mBinder;
    }

    @Override // android.app.Service
    public void onStart(Intent intent, int i) {
        super.onStart(intent, i);
    }

    @Override // android.app.Service
    public void onDestroy() {
        super.onDestroy();
    }

    private ArrayList<Byte> toByteArray(byte[] bArr, int i, int i2) {
        ArrayList<Byte> arrayList = new ArrayList<>(i2);
        for (int i3 = 0; i3 < i2; i3++) {
            arrayList.add(Byte.valueOf(bArr[i + i3]));
        }
        return arrayList;
    }

    private ArrayList<Byte> toByteArray(byte[] bArr) {
        if (bArr == null) {
            return new ArrayList<>();
        }
        return toByteArray(bArr, 0, bArr.length);
    }

    private byte[] toBytes(ArrayList<Byte> arrayList) {
        if (arrayList == null) {
            return null;
        }
        int size = arrayList.size();
        byte[] bArr = new byte[size];
        for (int i = 0; i < size; i++) {
            bArr[i] = arrayList.get(i).byteValue();
        }
        return bArr;
    }

    private byte[] processCmdImpl(byte[] bArr) {
        if (mFingerprintPay == null) {
            try {
                mFingerprintPay = IFingerprintPay.getService();
           } catch (RemoteException e) {
                Log.e("IfaaService", "Failed to open fingerprintAlipayService HAL", e);
           }
        }

        byte[] receiveBuffer = null;
        try {
            ArrayList<Byte> paramByteArray = new ArrayList();
            int i = 0;
            for (byte b : bArr) {
                paramByteArray.add(new Byte(b));
            }
            ArrayList<Byte> receiveBufferByteArray = mFingerprintPay.alipay_invoke_command(paramByteArray);
            receiveBuffer = new byte[receiveBufferByteArray.size()];
            while (i < receiveBufferByteArray.size()) {
                receiveBuffer[i] = ((Byte) receiveBufferByteArray.get(i)).byteValue();
                i++;
            }
        } catch (RemoteException e) {
            Log.e("IfaaService", "processCmdImpl failed", e);
        }
        return receiveBuffer;
    }

    public int getSupportBIOTypesImpl() {
        return AUTH_TYPE_OPTICAL_FINGERPRINT;
    }

    public int startBIOManagerImpl(int i) {
        try {
            Intent intent = new Intent("android.settings.SECURITY_SETTINGS");
            intent.addFlags(268435456);
            getApplicationContext().startActivity(intent);
            return 0;
        } catch (ActivityNotFoundException e) {
            e.printStackTrace();
        } catch (Throwable unused) {
        }
        return 0;
    }

    public String getDeviceModelImpl() {
        return "OPPO-R9081";
    }

    public int getVersionImpl() {
        return 4;
    }

    public String getExtInfoImpl(int i, String str) {
        if ("org.ifaa.ext.key.GET_SENSOR_LOCATION".equals(str)) {
            return "{'type': 0, 'fullView': {'startX': 445, 'startY': 2061,'width': 190, 'height': 190, 'navConflict': true}}";
        }
        Log.e("IfaaService", "getExtInfo: Didn't request supported ext info");
        return "";
    }

    public int getEnabledImpl(int i) {
        if (!((KeyguardManager) getApplicationContext().getSystemService("keyguard")).isDeviceSecure()) {
            return 1003;
        }
        if (i == 4) {
            return 1000;
        }
        if (i != 1) {
            return 1001;
        }
        ensureFingerprintServiceAvailable();
        FingerprintManager fingerprintManager = this.mFPManager;
        if (fingerprintManager == null) {
            return 1001;
        }
        if (!fingerprintManager.hasEnrolledFingerprints()) {
            return 1002;
        }
        return 1000;
    }

    public int[] getIDListImpl(int i) {
        if (i == 4) {
            return new int[0];
        }
        if (i != 1) {
            return new int[0];
        }
        ensureFingerprintServiceAvailable();
        FingerprintManager fingerprintManager = this.mFPManager;
        if (fingerprintManager == null) {
            return new int[0];
        }
        List enrolledFingerprints = fingerprintManager.getEnrolledFingerprints();
        int[] iArr = new int[enrolledFingerprints.size()];
        for (int i2 = 0; i2 < enrolledFingerprints.size(); i2++) {
            iArr[i2] = ((Fingerprint) enrolledFingerprints.get(i2)).getBiometricId();
            Log.i("IfaaService", "getIDList i: " + iArr[i2]);
        }
        return iArr;
    }

    private void ensureFingerprintServiceAvailable() {
        Context applicationContext = getApplicationContext();
        if (applicationContext != null && applicationContext.getPackageManager().hasSystemFeature("android.hardware.fingerprint")) {
            this.mFPManager = (FingerprintManager) applicationContext.getSystemService("fingerprint");
        }
    }
}
