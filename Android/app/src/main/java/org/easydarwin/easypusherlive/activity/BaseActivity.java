/*
 * Copyright 2013-2015 duolabao.com All right reserved. This software is the
 * confidential and proprietary information of duolabao.com ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement you entered
 * into with duolabao.com.
 */

package org.easydarwin.easypusherlive.activity;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.Toast;

import org.easydarwin.easypusherlive.config.DarwinConfig;
import org.easydarwin.easypusherlive.indicator.WaitProgressDialog;

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

    protected void showToadMessage(String message){
        Toast.makeText(this,message,Toast.LENGTH_LONG).show();
    }

    protected void updateServerSettingState(Boolean state){
        SharedPreferences.Editor editor=mSettingSharedPreference.edit();
        editor.putBoolean(DarwinConfig.SERVER_SETTING_UPDATED,state);
        editor.apply();
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
