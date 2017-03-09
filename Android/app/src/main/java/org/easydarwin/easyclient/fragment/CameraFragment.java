/*
	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.fragment;

import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.AsyncTask;
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
import android.widget.AdapterView;
import android.widget.GridView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.gson.JsonObject;

import org.easydarwin.easyclient.MyApplication;
import org.easydarwin.easyclient.R;
import org.easydarwin.easyclient.activity.EasyPlayerActivity;
import org.easydarwin.easyclient.activity.MainActivity;
import org.easydarwin.easyclient.adapter.OnlineCameraAdapter;
import org.easydarwin.easyclient.callback.CallbackWrapper;
import org.easydarwin.easyclient.config.DarwinConfig;
import org.easydarwin.easyclient.domain.DeviceListBody;
import org.easydarwin.easyclient.domain.DeviceListBody.Device;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.FutureTask;

import okhttp3.Call;
import okhttp3.Request;

public class CameraFragment extends BaseFragment implements SwipeRefreshLayout.OnRefreshListener, AdapterView.OnItemClickListener {
    private static final String TAG = "CameraFragment";
    private Context mContext;
    private String mServerIp;
    private String mServerPort;
    private OnlineCameraAdapter liveVOAdapter;

    SwipeRefreshLayout mSwipeRefreshLayout;
    GridView mGrid_live;
    protected String mType = "ARM_Linux";

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.fragment_camera, container, false);
        mSwipeRefreshLayout = (SwipeRefreshLayout) view.findViewById(R.id.swip_refersh);
        mSwipeRefreshLayout.setOnRefreshListener(this);
        mGrid_live = (GridView) view.findViewById(R.id.adapter_view);
        mGrid_live.setOnItemClickListener(this);
        return view;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        mContext = getActivity();
        initviews();
    }

    @Override
    protected void lazyLoad() {
        super.lazyLoad();
        getDevices();
    }

    @Override
    public void onResume() {
        super.onResume();
        mGrid_live.setEnabled(true);
    }

    private void initviews() {
        mSwipeRefreshLayout.setColorSchemeResources(android.R.color.holo_blue_bright,
                android.R.color.holo_green_light,
                android.R.color.holo_orange_light,
                android.R.color.holo_red_light);
        mGrid_live.setEnabled(true);
    }

    @Override
    public void onRefresh() {
        getDevices();
    }

    private void getDevices() {
        mServerIp = MyApplication.getInstance().getIp();
        mServerPort = MyApplication.getInstance().getPort();
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
        String url = String.format("http://%s:%s/api/v1/getdevicelist?AppType=EasyCamera&TerminalType=" + mType, ip, port);
        Log.d(TAG, "kim camera url=" + url);
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
                List<DeviceListBody.Device> devices = body.getDevices();
                if (devices.size() == 0) {
                    activity.showToadMessage("暂无直播信息");
                    liveVOAdapter = new OnlineCameraAdapter(new ArrayList<DeviceListBody.Device>());
                } else {
                    liveVOAdapter = new OnlineCameraAdapter(devices);
                    int screenW = mContext.getResources().getDisplayMetrics().widthPixels;
                    int columnum = mGrid_live.getNumColumns();
                    int space = (int) (mContext.getResources().getDimension(R.dimen.gridview_horizontalspacing));
                    int itemWidth = (int) ((screenW - (columnum - 1) * space) / columnum);
                    int itemHeight = getItemHeight(itemWidth);
                    liveVOAdapter.setmSnapshotWidth(itemWidth);
                    liveVOAdapter.setmSnapshotHeight(itemHeight);


                    //                    int screenW = mContext.getResources().getDisplayMetrics().widthPixels;
//                    int columnum = mGrid_live.getNumColumns();
//                    int space = (int)(mContext.getResources().getDimension(R.dimen.gridview_horizontalspacing));
//                    int itemWidth = (int)((screenW-(columnum-1)*space)/columnum);
//                    int itemHeight = (int) (itemWidth * 4 / 3.0 + 0.5f);
//                    liveVOAdapter.setmSnapshotWidth(itemWidth);
//                    liveVOAdapter.setmSnapshotHeight(itemHeight);

                }
                mGrid_live.setAdapter(liveVOAdapter);
            }
        });
    }

    protected int getItemHeight(int itemWidth) {
        return (int) (itemWidth * 9 / 16.0 + 0.5f);
    }

    private void getDeviceRtspUrl(final String serial) {
        final MainActivity activity = (MainActivity) getActivity();
        if (activity == null) return;
        activity.showWaitProgress("正在请求EasyCMS启动视频...");
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                String url = String.format("http://%s:%s/api/v1/startdevicestream?device=%s&channel=1&reserve=1",
                        MyApplication.getInstance().getIp(),
                        MyApplication.getInstance().getPort(), serial);
                FutureTask<JsonObject> futureTask = MyApplication.syncGet(url);
                futureTask.run();
                try {
                    JsonObject Body = futureTask.get();
                    String Service = Body.getAsJsonPrimitive("Service").getAsString();
                    String[] svc = Service.split(";");
                    String IP = svc[0];
                    String PORT = svc[1];
                    String Type = svc[2];

                    IP = IP.split("=")[1];
                    PORT = PORT.split("=")[1];
                    activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            activity.showWaitProgress("正在获取RTSP地址...");
                        }
                    });
                    //http://[ip]:[port]/api/v1/getdevicestream?device=001002000001&channel=1&protocol=rt sp&reserve=1
                    url = String.format("http://%s:%s/api/v1/getdevicestream?device=%s&channel=1&protocol=RTSP&reserve=1",
                            IP,
                            PORT,
                            serial);

                    futureTask = MyApplication.syncGet(url);
                    futureTask.run();
                    Body = futureTask.get();
                    Log.d(TAG, Body.toString());
//                    Body: {
//                        Protocol: "RTSP",
//                                URL: "rtsp://139.199.154.127:11554/001001000099/1.sdp"
//                    },

                    String Protocal = Body.getAsJsonPrimitive("Protocol").getAsString();
                    String streamUrl = Body.getAsJsonPrimitive("URL").getAsString();
                    if (!TextUtils.isEmpty(streamUrl)) {

                        Intent intent = new Intent(getContext(), EasyPlayerActivity.class);
                        intent.putExtra(DarwinConfig.CAM_URL, streamUrl);
                        intent.putExtra(DarwinConfig.DEV_SERIAL, serial);
                        intent.putExtra(DarwinConfig.DEV_TYPE, "android");
                        startActivity(intent);
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                } catch (ExecutionException e) {
                    e.printStackTrace();
                    e.printStackTrace();
                    Throwable thr = e.getCause();
                    if (thr instanceof CallbackWrapper.HttpCodeErrorExcepetion) {
                        final CallbackWrapper.HttpCodeErrorExcepetion exception = (CallbackWrapper.HttpCodeErrorExcepetion) thr;
                        activity.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                Toast.makeText(activity, exception.getMessage(), Toast.LENGTH_SHORT).show();
                            }
                        });
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                } finally {
                    activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            mGrid_live.setEnabled(true);
                            activity.hideWaitProgress();
                        }
                    });
                }
            }
        });
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        mGrid_live.setEnabled(false);
        Device device = (Device) parent.getAdapter().getItem(position);
        getDeviceRtspUrl(device.getSerial());
    }


    protected void onSetTipContent(TextView tipContent) {
        /*
EasyCamera ARM摄像机服务是一套独立于各个厂家芯片的对接服务，通过将EasyCamera ARM服务内置到各个硬件摄像机厂家（海康、大华、雄迈等）的摄像机嵌入式系统中，与平台进行交互，控制摄像机的音视频、对讲、PTZ、更新配置、重启服务等动作；
EasyCamera ARM摄像机样机：http://www.easydarwin.org/camera
        * */
        tipContent.append("EasyCamera ARM摄像机服务是一套独立于各个厂家芯片的对接服务，通过将EasyCamera ARM服务内置到各个硬件摄像机厂家（海康、大华、雄迈等）的摄像机嵌入式系统中，与平台进行交互，控制摄像机的音视频、对讲、PTZ、更新配置、重启服务等动作；\n" +
                "EasyCamera ARM摄像机样机：");
        tipContent.setMovementMethod(LinkMovementMethod.getInstance());
        SpannableString spannableString = new SpannableString("http://www.easydarwin.org/camera");
        //设置下划线文字
        spannableString.setSpan(new URLSpan("http://www.easydarwin.org/camera"), 0, spannableString.length(), Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
        //设置文字的前景色
        spannableString.setSpan(new ForegroundColorSpan(Color.RED), 0, spannableString.length(), Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
        tipContent.append(spannableString);
    }
}
