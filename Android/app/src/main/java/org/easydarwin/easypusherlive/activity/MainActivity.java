package org.easydarwin.easypusherlive.activity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.support.v4.widget.SwipeRefreshLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.Toast;

import org.easydarwin.easypusherlive.R;
import org.easydarwin.easypusherlive.adapter.LiveVOAdapter;
import org.easydarwin.easypusherlive.callback.LiveVOCallback;
import org.easydarwin.easypusherlive.config.DarwinConfig;
import org.easydarwin.easypusherlive.domain.LiveSession;
import org.easydarwin.easypusherlive.domain.LiveVO;
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
    }

    @Override
    protected void onResume() {
        super.onResume();
        boolean severUpdated = mSettingSharedPreference.getBoolean(DarwinConfig.SERVER_SETTING_UPDATED, true);
        if (!severUpdated) {
            return;
        }
        updateServerSettingState(false);
        serverIp = mSettingSharedPreference.getString(DarwinConfig.SERVER_IP, "");
        serverPort = mSettingSharedPreference.getString(DarwinConfig.SERVER_PORT, "80");
        if (TextUtils.isEmpty(serverIp)) {
            Toast.makeText(this, "请配置您的服务器信息", Toast.LENGTH_LONG).show();
            return;
        }
        getSessions(serverIp, serverPort);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        updateServerSettingState(true);
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
                        if(sessions.size()==0){
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
        Intent intent = new Intent(this, LivePlayActivity.class);
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
        getSessions(serverIp, serverPort);
    }
}
