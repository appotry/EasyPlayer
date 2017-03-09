/*
	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.fragment;


import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.widget.SwipeRefreshLayout;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.TextUtils;
import android.text.method.LinkMovementMethod;
import android.text.style.ForegroundColorSpan;
import android.text.style.URLSpan;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ExpandableListView;
import android.widget.TextView;
import android.widget.Toast;

import org.easydarwin.easyclient.MyApplication;
import org.easydarwin.easyclient.R;
import org.easydarwin.easyclient.activity.MainActivity;
import org.easydarwin.easyclient.adapter.ExpandListAdapter;
import org.easydarwin.easyclient.callback.CallbackWrapper;
import org.easydarwin.easyclient.config.DarwinConfig;
import org.easydarwin.easyclient.domain.DeviceListBody;
import org.easydarwin.easyclient.domain.DeviceListBody.Device;


import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;
import okhttp3.Request;

/**
 * Created by Kim on 2016/6/16
 */
public class NVRFragment extends BaseFragment implements SwipeRefreshLayout.OnRefreshListener {
    private static final String TAG = "NVRFragment";
    private Context mContext;
    private String mServerIp;
    private String mServerPort;
    private ExpandListAdapter liveVOAdapter;

    SwipeRefreshLayout mSwipeRefreshLayout;
    ExpandableListView mListNvr;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_nvr, container, false);
        mSwipeRefreshLayout = (SwipeRefreshLayout) view.findViewById(R.id.swip_refersh);
        mSwipeRefreshLayout.setOnRefreshListener(this);
        mListNvr = (ExpandableListView) view.findViewById(R.id.adapter_view);
        mListNvr.setOnGroupExpandListener(new ExpandableListView.OnGroupExpandListener() {
            @Override
            public void onGroupExpand(int groupPosition) {
                ExpandListAdapter adapter = (ExpandListAdapter) mListNvr.getExpandableListAdapter();
                for (int i = 0, count = adapter.getGroupCount(); i < count; i++) {
                    if (groupPosition != i) {// 关闭其他分组
                        mListNvr.collapseGroup(i);
                    }
                }
                Device dvc = (Device) adapter.getGroup(groupPosition);
                if (dvc.channels.isEmpty()) {
                    adapter.fetchChild(groupPosition);
                }
            }
        });

        return view;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mContext = getActivity();
    }

    @Override
    protected void lazyLoad() {
        super.lazyLoad();
        getDevices();
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

        final MainActivity activity = (MainActivity) getActivity();
        if (activity == null) return;
        String url = String.format("http://%s:%s/api/v1/getdevicelist?AppType=EasyNVR", ip, port);
        Log.d(TAG, "nvr url=" + url);
        MyApplication.asyncGet(url, new CallbackWrapper<DeviceListBody>(DeviceListBody.class) {

            @Override
            public void onBefore(Request request) {
                mSwipeRefreshLayout.setRefreshing(false);
                activity.showWaitProgress("");
            }

            @Override
            public void onAfter() {
                activity.hideWaitProgress();
            }

            @Override
            public void onError(Call call, Exception e) {
                Toast.makeText(mContext, "onError:" + e.toString(), Toast.LENGTH_LONG).show();
            }

            @Override
            public void onResponse(DeviceListBody body) {
                List<Device> devices = body.getDevices();
                if (devices.size() == 0) {
                    activity.showToadMessage("暂无直播信息");
                    liveVOAdapter = new ExpandListAdapter((MainActivity) getActivity(), new ArrayList<Device>());
                } else {
                    liveVOAdapter = new ExpandListAdapter((MainActivity) getActivity(), devices);
                }
                mListNvr.setAdapter(liveVOAdapter);
            }
        });
    }


    protected void onSetTipContent(TextView tipContent) {
        /*
EasyCamera ARM摄像机服务是一套独立于各个厂家芯片的对接服务，通过将EasyCamera ARM服务内置到各个硬件摄像机厂家（海康、大华、雄迈等）的摄像机嵌入式系统中，与平台进行交互，控制摄像机的音视频、对讲、PTZ、更新配置、重启服务等动作；
EasyCamera ARM摄像机样机：http://www.easydarwin.org/camera
        * */
        tipContent.append("EasyNVR是一款能够将各个厂家的通用RTSP/ONVIF摄像机接入，并整合成为全平台直播的综合服务，EasyNVR能够接入到EasyDarwin云平台，接收EasyDarwin云平台对各个通道摄像机的音视频、转动等操作指令，将通用的安防摄像机接入到统一平台中，常用在幼儿园视频、智能家居、智慧社区、智慧农业等项目应用中！\n\n版本下载: ");
        tipContent.setMovementMethod(LinkMovementMethod.getInstance());
        SpannableString spannableString = new SpannableString("http://github.com/EasyDarwin/EasyNVR");
        //设置下划线文字
        spannableString.setSpan(new URLSpan("http://github.com/EasyDarwin/EasyNVR"), 0, spannableString.length(), Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
        //设置文字的前景色
        spannableString.setSpan(new ForegroundColorSpan(Color.RED), 0, spannableString.length(), Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
        tipContent.append(spannableString);
    }
}