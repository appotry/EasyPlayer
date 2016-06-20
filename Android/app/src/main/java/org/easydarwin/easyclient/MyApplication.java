/*
 * 	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
 * 	Github: https://github.com/EasyDarwin
 * 	WEChat: EasyDarwin
 *	Website: http://www.easydarwin.org
 */
package org.easydarwin.easyclient;

import android.app.Application;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.text.TextUtils;


import org.easydarwin.easyclient.config.DarwinConfig;
import org.easydarwin.okhttplibrary.OkHttpUtils;

import java.util.concurrent.TimeUnit;

/**
 * Created by zhy on 15/8/25.
 */
public class MyApplication extends Application
{

    private static MyApplication instance;
    @Override
    public void onCreate()
    {
        super.onCreate();
        OkHttpUtils.getInstance().setConnectTimeout(100000, TimeUnit.MILLISECONDS);
        instance=this;
        initServerInfo();
    }

    private void initServerInfo(){
        SharedPreferences sharedPreferences = getSharedPreferences(DarwinConfig.SETTING_PREF_NAME, MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        String serverIp = sharedPreferences.getString(DarwinConfig.SERVER_IP, "");
        if (TextUtils.isEmpty(serverIp)) {
            editor.putString(DarwinConfig.SERVER_IP, DarwinConfig.DEFAULT_SERVER_IP);
            editor.apply();
        }

        String port = sharedPreferences.getString(DarwinConfig.SERVER_PORT, "");
        if (TextUtils.isEmpty(port)) {
            editor.putString(DarwinConfig.SERVER_PORT, DarwinConfig.DEFAULT_SERVER_PORT);
            editor.apply();
        }

    }

    public String getIp(){
        SharedPreferences sharedPreferences=getSharedPreferences(DarwinConfig.SETTING_PREF_NAME,MODE_PRIVATE);
       return sharedPreferences.getString(DarwinConfig.SERVER_IP, DarwinConfig.DEFAULT_SERVER_IP);
    }

    public String getPort(){
        SharedPreferences sharedPreferences=getSharedPreferences(DarwinConfig.SETTING_PREF_NAME,MODE_PRIVATE);
        return sharedPreferences.getString(DarwinConfig.SERVER_PORT, DarwinConfig.DEFAULT_SERVER_PORT);
    }


    /**
     * @return the main context of the Application
     */
    public static MyApplication getInstance()
    {
        return instance;
    }

    public static Resources getAppResources()
    {
        return instance.getResources();
    }
}
