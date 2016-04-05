/*
 * 	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
 * 	Github: https://github.com/EasyDarwin
 * 	WEChat: EasyDarwin
 *	Website: http://www.easydarwin.org
 */
package org.easydarwin.easyclient;

import android.app.Application;
import android.content.Context;
import android.content.res.Resources;


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
    }



    /**
     * @return the main context of the Application
     */
    public static Context getAppContext()
    {
        return instance;
    }

    public static Resources getAppResources()
    {
        return instance.getResources();
    }
}
