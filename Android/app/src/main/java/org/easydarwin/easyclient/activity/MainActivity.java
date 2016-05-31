/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.activity;

import android.content.Intent;
import android.support.v4.widget.SwipeRefreshLayout;
import android.support.v7.app.ActionBar;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.Toast;

import org.easydarwin.easyclient.MyApplication;
import org.easydarwin.easyclient.R;
import org.easydarwin.easyclient.adapter.OnlineCameraAdapter;
import org.easydarwin.easyclient.callback.DeviceInfoCallback;
import org.easydarwin.easyclient.callback.LiveVOCallback;
import org.easydarwin.easyclient.config.DarwinConfig;
import org.easydarwin.easyclient.domain.Device;
import org.easydarwin.easyclient.domain.DeviceHeader;
import org.easydarwin.easyclient.domain.DeviceInfoWrapper;
import org.easydarwin.easyclient.domain.LiveVO;
import org.easydarwin.okhttplibrary.OkHttpUtils;

import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;
import okhttp3.Request;

public class MainActivity extends BaseActivity implements
        AdapterView.OnItemClickListener, SwipeRefreshLayout.OnRefreshListener {

    ListView listView;
    OnlineCameraAdapter liveVOAdapter;
    SwipeRefreshLayout swipeRefreshLayout;
    String serverIp, serverPort;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setTitle("EasyClient");
            actionBar.setDisplayHomeAsUpEnabled(false);
            actionBar.setLogo(R.mipmap.ic_launcher);
            actionBar.setDisplayUseLogoEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
            actionBar.show();
        }
        listView = (ListView) findViewById(R.id.list_live);
        liveVOAdapter = new OnlineCameraAdapter(new ArrayList<Device>());
        listView.setAdapter(liveVOAdapter);
        listView.setOnItemClickListener(this);
        swipeRefreshLayout = (SwipeRefreshLayout) findViewById(R.id.swip_refersh);
        swipeRefreshLayout.setOnRefreshListener(this);

        getDevices();
    }

    private void getDevices() {
        serverIp = mSettingSharedPreference.getString(DarwinConfig.SERVER_IP, DarwinConfig.DEFAULT_SERVER_IP);
        serverPort = mSettingSharedPreference.getString(DarwinConfig.SERVER_PORT, DarwinConfig.DEFAULT_SERVER_PORT);
        getDevices(serverIp, serverPort);
    }

    @Override
    protected void onResume() {
        super.onResume();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    /**
     * 获取直播地址列表
     *
     * @param ip   服务器地址
     * @param port 服务器端口号
     */
    private void getDevices(String ip, String port) {

        if (TextUtils.isEmpty(ip) || TextUtils.isEmpty(port)) {
            return;
        }

        String url = String.format("http://%s:%s/api/getdevicelist", ip, port);
        OkHttpUtils.post().url(url).build().execute(new LiveVOCallback() {

            @Override
            public void onBefore(Request request) {
                swipeRefreshLayout.setRefreshing(false);
                showWaitProgress("");
            }

            @Override
            public void onAfter() {
                hideWaitProgress();
            }

            @Override
            public void onError(Call call, Exception e) {
                Toast.makeText(MainActivity.this, "onError:" + e.toString(), Toast.LENGTH_LONG).show();
            }

            @Override
            public void onResponse(LiveVO liveVO) {
                List<Device> devices = liveVO.getEasyDarwin().getBody().getDevices();
                if (devices.size() == 0) {
                    showToadMessage("暂无直播信息");
                    liveVOAdapter = new OnlineCameraAdapter(new ArrayList<Device>());
                } else {
                    liveVOAdapter = new OnlineCameraAdapter(devices);
                }
                listView.setAdapter(liveVOAdapter);
            }
        });
    }


    private void getDeviceRtspUrl(String serial){
        String url=String.format("http://%s:%s/api/getdevicestream?device=%s&protocol=RTSP",
                MyApplication.getInstance().getIp(),
                MyApplication.getInstance().getPort(),serial);
        OkHttpUtils.post().url(url).build().execute(new DeviceInfoCallback(){
            @Override
            public void onBefore(Request request) {
                swipeRefreshLayout.setRefreshing(false);
                showWaitProgress("");
            }

            @Override
            public void onAfter() {
                hideWaitProgress();
            }
            @Override
            public void onError(Call call, Exception e) {
                Toast.makeText(MainActivity.this, "onError:" + e.toString(), Toast.LENGTH_LONG).show();
            }

            @Override
            public void onResponse(DeviceInfoWrapper deviceInfoWrapper) {
                if(deviceInfoWrapper.getEasyDarwin().getBody()==null){
                    DeviceHeader header=deviceInfoWrapper.getEasyDarwin().getHeader();
                    Toast.makeText(MainActivity.this, header.getErrorString()+"(" +header.getErrorNum()+")",
                            Toast.LENGTH_LONG).show();
                    return;
                }
                Intent intent = new Intent(MainActivity.this, EasyPlayerActivity.class);
                intent.putExtra(DarwinConfig.CAM_Serial, deviceInfoWrapper.getEasyDarwin().getBody().getURL());
                startActivity(intent);
            }
        });
    }

    /**
     * 跳转到设置界面
     */
    private void toSetting() {
        startActivity(new Intent(MainActivity.this, SettingActivity.class));
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        Device device = (Device) parent.getAdapter().getItem(position);
        getDeviceRtspUrl(device.getSerial());
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater menuInflater = getMenuInflater();
        menuInflater.inflate(R.menu.main_menu, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.main_menu_setting:
                toSetting();
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onRefresh() {
        getDevices();
    }
}
