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

import org.easydarwin.easyclient.R;
import org.easydarwin.easyclient.adapter.LiveVOAdapter;
import org.easydarwin.easyclient.callback.LiveVOCallback;
import org.easydarwin.easyclient.config.DarwinConfig;
import org.easydarwin.easyclient.domain.LiveSession;
import org.easydarwin.easyclient.domain.LiveVO;
import org.easydarwin.okhttplibrary.OkHttpUtils;

import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;
import okhttp3.Request;

public class MainActivity extends BaseActivity implements
        AdapterView.OnItemClickListener, SwipeRefreshLayout.OnRefreshListener {

    ListView listView;
    LiveVOAdapter liveVOAdapter;
    SwipeRefreshLayout swipeRefreshLayout;
    String serverIp, serverPort;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayShowHomeEnabled(false);
            actionBar.setDisplayShowTitleEnabled(false);
            actionBar.setDisplayHomeAsUpEnabled(false);
            actionBar.show();
        }
        listView = (ListView) findViewById(R.id.list_live);
        liveVOAdapter = new LiveVOAdapter(new ArrayList<LiveSession>());
        listView.setAdapter(liveVOAdapter);
        listView.setOnItemClickListener(this);
        swipeRefreshLayout = (SwipeRefreshLayout) findViewById(R.id.swip_refersh);
        swipeRefreshLayout.setOnRefreshListener(this);

        getSeeeion();
    }

    private void getSeeeion() {
        serverIp = mSettingSharedPreference.getString(DarwinConfig.SERVER_IP, DarwinConfig.DEFAULT_SERVER_IP);
        serverPort = mSettingSharedPreference.getString(DarwinConfig.SERVER_PORT, DarwinConfig.DEFAULT_SERVER_PORT);
        getSessions(serverIp, serverPort);
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
    private void getSessions(String ip, String port) {

        if (TextUtils.isEmpty(ip) || TextUtils.isEmpty(port)) {
            return;
        }

        String url = String.format("http://%s:%s/api/getrtsppushsessions", ip, port);
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
                Toast.makeText(MainActivity.this, "onError:" + e.getMessage(), Toast.LENGTH_LONG).show();
            }

            @Override
            public void onResponse(LiveVO liveVO) {
                List<LiveSession> sessions = liveVO.getEasyDarwin().getBody().getSessions();
                if (sessions.size() == 0) {
                    showToadMessage("暂无直播信息");
                    return;
                }
                liveVOAdapter = new LiveVOAdapter(sessions);
                listView.setAdapter(liveVOAdapter);
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
        LiveSession session = (LiveSession) parent.getAdapter().getItem(position);
        Intent intent = new Intent(this, EasyPlayerActivity.class);
        intent.putExtra(DarwinConfig.RTSP_ADDRESS, session.getUrl());
        startActivity(intent);
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
        getSeeeion();
    }
}
