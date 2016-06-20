/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.fragment;


import android.content.Context;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.widget.SwipeRefreshLayout;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ExpandableListView;
import android.widget.Toast;

import org.easydarwin.easyclient.R;
import org.easydarwin.easyclient.activity.MainActivity;
import org.easydarwin.easyclient.adapter.ExpandListAdapter;
import org.easydarwin.easyclient.callback.LiveVOCallback;
import org.easydarwin.easyclient.config.DarwinConfig;
import org.easydarwin.easyclient.domain.Device;
import org.easydarwin.easyclient.domain.LiveVO;
import org.easydarwin.okhttplibrary.OkHttpUtils;

import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;
import okhttp3.Request;

/**
 * Created by Kim on 2016/6/16
 */
public class NVRFragment extends BaseFragment implements SwipeRefreshLayout.OnRefreshListener{
    private static final String TAG = "NVRFragment";
    private Context mContext;
    private String mServerIp;
    private String mServerPort;
    private ExpandListAdapter liveVOAdapter;
    private boolean mIsPrepared;/* 标志位，标志已经初始化完成 */

    SwipeRefreshLayout mSwipeRefreshLayout;
    ExpandableListView mListNvr;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_nvr, container, false);
        mSwipeRefreshLayout = (SwipeRefreshLayout) view.findViewById(R.id.swip_refersh);
        mSwipeRefreshLayout.setOnRefreshListener(this);
        mListNvr = (ExpandableListView) view.findViewById(R.id.list_nvr);
        mListNvr.setOnGroupExpandListener(new ExpandableListView.OnGroupExpandListener() {
            @Override
            public void onGroupExpand(int groupPosition) {
                for (int i = 0, count = mListNvr.getExpandableListAdapter().getGroupCount(); i < count; i++) {
                    if (groupPosition != i) {// 关闭其他分组
                        mListNvr.collapseGroup(i);
                    }
                }
            }
        });

        return view;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mContext = getActivity();
        mIsPrepared = true;
    }

    @Override
    public void onRefresh() {
        getDevices();
    }

    private void getDevices() {
        mServerIp = MainActivity.getSettingPref().getString(DarwinConfig.SERVER_IP, DarwinConfig.DEFAULT_SERVER_IP);
        mServerPort = MainActivity.getSettingPref().getString(DarwinConfig.SERVER_PORT, DarwinConfig.DEFAULT_SERVER_PORT);
        getDevices(mServerIp, mServerPort);
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

        String url = String.format("http://%s:%s/api/getdevicelist?AppType=EasyNVR", ip, port);
        Log.d(TAG, "nvr url=" + url);
        OkHttpUtils.post().url(url).build().execute(new LiveVOCallback() {

            @Override
            public void onBefore(Request request) {
                mSwipeRefreshLayout.setRefreshing(false);
                MainActivity.instance.showWaitProgress("");
            }

            @Override
            public void onAfter() {
                MainActivity.instance.hideWaitProgress();
            }

            @Override
            public void onError(Call call, Exception e) {
                Toast.makeText(mContext, "onError:" + e.toString(), Toast.LENGTH_LONG).show();
            }

            @Override
            public void onResponse(LiveVO liveVO) {
                List<Device> devices = liveVO.getEasyDarwin().getBody().getDevices();
                if (devices.size() == 0) {
                    MainActivity.instance.showToadMessage("暂无直播信息");
                    liveVOAdapter = new ExpandListAdapter(new ArrayList<Device>());
                } else {
                    liveVOAdapter = new ExpandListAdapter(devices);
                }
                mListNvr.setAdapter(liveVOAdapter);
            }
        });
    }

    @Override
    protected void lazyLoad() {
        Log.d(TAG,"lazyLoad mIsPrepared="+mIsPrepared);
        if(!mIsPrepared && MainActivity.instance.mStarted)
            return;
        getDevices();
    }
}