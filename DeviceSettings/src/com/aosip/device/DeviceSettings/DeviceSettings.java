/*
* Copyright (C) 2016 The OmniROM Project
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/
package com.aosip.device.DeviceSettings;

import android.content.Intent;
import android.os.Bundle;
import android.provider.Settings;
import android.view.MenuItem;

import androidx.preference.ListPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceCategory;
import androidx.preference.PreferenceFragment;
import androidx.preference.PreferenceGroup;
import androidx.preference.PreferenceManager;
import androidx.preference.PreferenceScreen;
import androidx.preference.SwitchPreference;
import androidx.preference.TwoStatePreference;

public class DeviceSettings extends PreferenceFragment
        implements Preference.OnPreferenceChangeListener {

    public static final String KEY_SRGB_SWITCH = "srgb";
    public static final String KEY_DC_SWITCH = "dc";

    private static final String KEY_CATEGORY_REFRESH = "refresh";
    public static final String KEY_REFRESH_RATE = "refresh_rate";
    public static final String KEY_AUTO_REFRESH_RATE = "auto_refresh_rate";

    public static final String KEY_SETTINGS_PREFIX = "device_setting_";

    private static TwoStatePreference mRefreshRate;
    private static SwitchPreference mAutoRefreshRate;

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.main);

        TwoStatePreference mDCModeSwitch = (TwoStatePreference) findPreference(KEY_DC_SWITCH);
        mDCModeSwitch.setEnabled(DCModeSwitch.isSupported());
        mDCModeSwitch.setChecked(DCModeSwitch.isCurrentlyEnabled(this.getContext()));
        mDCModeSwitch.setOnPreferenceChangeListener(new DCModeSwitch());

        if (getResources().getBoolean(R.bool.config_deviceHasHighRefreshRate)) {
            mAutoRefreshRate = (SwitchPreference) findPreference(KEY_AUTO_REFRESH_RATE);
            mAutoRefreshRate.setChecked(AutoRefreshRateSwitch.isCurrentlyEnabled(this.getContext()));
            mAutoRefreshRate.setOnPreferenceChangeListener(new AutoRefreshRateSwitch(getContext()));

            mRefreshRate = (TwoStatePreference) findPreference(KEY_REFRESH_RATE);
            mRefreshRate.setEnabled(!AutoRefreshRateSwitch.isCurrentlyEnabled(this.getContext()));
            mRefreshRate.setChecked(RefreshRateSwitch.isCurrentlyEnabled(this.getContext()));
            mRefreshRate.setOnPreferenceChangeListener(new RefreshRateSwitch(getContext()));
        } else {
            getPreferenceScreen().removePreference((Preference) findPreference(KEY_CATEGORY_REFRESH));
        }
    }

    @Override
    public void onResume() {
        super.onResume();
    }
    
    @Override
    public boolean onPreferenceTreeClick(Preference preference) {
        if (preference == mAutoRefreshRate) {
              mRefreshRate.setEnabled(!AutoRefreshRateSwitch.isCurrentlyEnabled(this.getContext()));
        }
        return super.onPreferenceTreeClick(preference);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        return true;
    }
}
