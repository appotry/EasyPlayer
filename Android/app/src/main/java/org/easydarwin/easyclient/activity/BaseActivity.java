/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/

package org.easydarwin.easyclient.activity;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.Toast;

import org.easydarwin.easyclient.config.DarwinConfig;
import org.easydarwin.easyclient.indicator.WaitProgressDialog;

/**
 * 类BaseActivity的实现描述：//TODO 类实现描述
 *
 * @author HELONG 2016/3/16 18:06
 */
public class BaseActivity extends AppCompatActivity {

    static SharedPreferences mSettingSharedPreference;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if(mSettingSharedPreference==null){
            mSettingSharedPreference=getSharedPreferences(DarwinConfig.SETTING_PREF_NAME,MODE_PRIVATE);
        }
    }

    private Toast mToast;
    public void showToadMessage(String text) {
        if(mToast == null) {
            mToast = Toast.makeText(this, text, Toast.LENGTH_SHORT);
        } else {
            mToast.setText(text);
            mToast.setDuration(Toast.LENGTH_SHORT);
        }
        mToast.show();
    }

    public void cancelToast() {
        if (mToast != null) {
            mToast.cancel();
        }
    }

    private WaitProgressDialog waitProgressDialog;

    // 显示等待框
    public void showWaitProgress(String message) {
        if (waitProgressDialog == null) {
            waitProgressDialog = new WaitProgressDialog(this);
        }
        waitProgressDialog.showProgress(message);
    }

    // 隐藏等待框
    public void hideWaitProgress() {
        if (waitProgressDialog != null) {
            waitProgressDialog.hideProgress();
        }
    }

}
