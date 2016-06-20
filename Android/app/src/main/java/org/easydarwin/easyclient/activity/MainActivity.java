/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.activity;

import android.app.DownloadManager;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.view.ViewPager;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AlertDialog;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import org.easydarwin.easyclient.R;
import org.easydarwin.easyclient.adapter.FragmentAdapter;
import org.easydarwin.easyclient.callback.VersionCallback;
import org.easydarwin.easyclient.domain.RemoteVersionInfo;
import org.easydarwin.easyclient.view.PagerSlidingTabStrip;
import org.easydarwin.okhttplibrary.OkHttpUtils;

import okhttp3.Call;
public class MainActivity extends BaseActivity implements View.OnClickListener{

    private static final String TAG="Easy_MainActivity";
    public static MainActivity instance = null;

    PagerSlidingTabStrip mainTab;
    ViewPager mainPager;
    FragmentAdapter mFragmentAdapter;
    Button mBtnSetting;
    public boolean mFirstStart = false;
    public boolean mStarted = false;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.hide();
        }

        instance = this;
        mainTab = (PagerSlidingTabStrip)this.findViewById(R.id.main_pager_tab);
        mainPager = (ViewPager)this.findViewById(R.id.main_pager);
        mainPager.setOffscreenPageLimit(3);
        mBtnSetting = (Button)this.findViewById(R.id.btSetting);
        mBtnSetting.setOnClickListener(this);
        mFragmentAdapter = new FragmentAdapter(this.getSupportFragmentManager());
        mainPager.setAdapter(mFragmentAdapter);
        mainTab.setViewPager(mainPager);

        checkUpdate();
    }

    @Override
    protected void onStart() {
        super.onStart();
        mFirstStart = true;
        mStarted = true;
    }

    public static SharedPreferences getSettingPref(){
        return mSettingSharedPreference;
    }

    /**
     * 检测当前APP是否需要升级
     */
    private void checkUpdate(){
        String url="http://www.easydarwin.org/versions/easyclient/version.txt";
        OkHttpUtils.get().url(url).build().execute(new VersionCallback(){
            @Override
            public void onError(Call call, Exception e) {
                Log.e(TAG,"Check update faill ...");
            }
            @Override
            public void onResponse(RemoteVersionInfo response) {
                if(response == null && TextUtils.isEmpty(response.getUrl())){
                    return;
                }
                PackageManager packageManager=getPackageManager();
                try {
                    PackageInfo packageInfo=packageManager.getPackageInfo(getPackageName(),0);
                    int localVersionCode=packageInfo.versionCode;
                    int remoteVersionCode= Integer.valueOf(response.getVersionCode());
                    if(localVersionCode<remoteVersionCode){
                        showUpdateDialog(response.getUrl());
                    }
                } catch (PackageManager.NameNotFoundException e) {
                    Log.e(TAG,"Get PackageInfo error !");
                }
            }
        });
    }

    /**
     * 提示升级对话框
     */
    private void showUpdateDialog(String linkUrl){
        final String apkUrl=linkUrl;
        new AlertDialog.Builder(this)
                .setMessage("EasyClient可以升级到更高的版本，是否升级")
                .setTitle("升级提示")
                .setIcon(R.drawable.client_logo)
                .setPositiveButton("升级", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        DownloadManager downloadManager = (DownloadManager) getSystemService(DOWNLOAD_SERVICE);
                        Uri uri = Uri.parse(apkUrl);
                        DownloadManager.Request request = new DownloadManager.Request(uri);
                        // 设置允许使用的网络类型，这里是移动网络和wifi都可以
                        request.setAllowedNetworkTypes(DownloadManager.Request.NETWORK_MOBILE | DownloadManager.Request.NETWORK_WIFI);
                        // 禁止发出通知，既后台下载，如果要使用这一句必须声明一个权限：android.permission.DOWNLOAD_WITHOUT_NOTIFICATION
                        request.setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE);
                        downloadManager.enqueue(request);
                        dialog.dismiss();
                    }
                }).setNegativeButton("取消", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        }).create().show();
    }

    /**
     * 跳转到设置界面
     */
    private void toSetting() {
        startActivity(new Intent(MainActivity.this, SettingActivity.class));
    }

    @Override
    public void onClick(View v) {
        if(v.getId() == R.id.btSetting){
            toSetting();
        }
    }
}
