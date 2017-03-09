/*
	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.adapter;

import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.support.v7.widget.GridLayout;
import android.text.TextUtils;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.BaseExpandableListAdapter;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.bumptech.glide.Glide;
import com.google.gson.JsonObject;

import org.easydarwin.easyclient.MyApplication;
import org.easydarwin.easyclient.R;
import org.easydarwin.easyclient.activity.EasyPlayerActivity;
import org.easydarwin.easyclient.activity.MainActivity;
import org.easydarwin.easyclient.callback.CallbackWrapper;
import org.easydarwin.easyclient.config.DarwinConfig;
import org.easydarwin.easyclient.domain.Channel;
import org.easydarwin.easyclient.domain.DeviceListBody.Device;
import org.easydarwin.easyclient.domain.NVRInfoBody;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.FutureTask;

import okhttp3.Call;
import okhttp3.Request;

public class ExpandListAdapter extends BaseExpandableListAdapter implements View.OnClickListener {
    private static final String TAG = "ExpandListAdapter";
    public static final int ItemHeight = 100;// 每项的高度
    public static final int PaddingLeft = 36;// 每项的高度
    private int myPaddingLeft = 50;


    private MainActivity mContext;
    private LayoutInflater layoutInflater;
    private List<Device> mDevice;
    private String mServerIp;
    private String mServerPort;

    public void fetchChild(final int groupPosition) {
        mServerIp = MyApplication.getInstance().getIp();
        mServerPort = MyApplication.getInstance().getPort();
        final Device group = (Device) getGroup(groupPosition);
        if (TextUtils.isEmpty(mServerIp) || TextUtils.isEmpty(mServerPort)) {
            return;
        }
        String url = String.format("http://%s:%s/api/v1/getdeviceinfo?device=%s", mServerIp, mServerPort, group.getSerial());
        Log.d(TAG, "camera url=" + url);
        MyApplication.asyncGet(url, new CallbackWrapper<NVRInfoBody>(NVRInfoBody.class) {

            @Override
            public void onBefore(Request request) {
                mContext.showWaitProgress("");
            }

            @Override
            public void onAfter() {
                mContext.hideWaitProgress();
            }

            @Override
            public void onError(Call call, Exception e) {
                Toast.makeText(mContext, "onError:" + e.toString(), Toast.LENGTH_LONG).show();
            }

            @Override
            public void onResponse(NVRInfoBody body) {
                List<Channel> channels = body.Channels;
                for (Channel c : channels) {
                    c.setNVRSerial(body.Serial);
                }
                group.channels.clear();
                group.channels.addAll(channels);
                notifyDataSetChanged();
                if (channels.size() == 0) {
                    mContext.showToadMessage("暂无直播信息");
                }
            }
        });
    }

    public ExpandListAdapter(MainActivity activity, List<Device> devices) {
        this.mDevice = devices;
        mContext = activity;
    }

    public Object getChild(int groupPosition, int childPosition) {
        Device device = (Device) getGroup(groupPosition);
        return device.channels.get(childPosition);
    }

    public int getChildrenCount(int groupPosition) {
        return 1;
    }

    static public TextView getTextView(Context context) {
        AbsListView.LayoutParams lp = new AbsListView.LayoutParams(ViewGroup.LayoutParams.FILL_PARENT, ItemHeight);

        TextView textView = new TextView(context);
        textView.setLayoutParams(lp);
        textView.setGravity(Gravity.CENTER_VERTICAL | Gravity.LEFT);
        return textView;
    }

    /**
     * 可自定义ExpandableListView
     */
    public View getChildView(int groupPosition, int childPosition,
                             boolean isLastChild, View convertView, ViewGroup parent) {
        if (convertView == null) {
            convertView = LayoutInflater.from(parent.getContext()).inflate(R.layout.nvr_channels, parent, false);
        }
        GridLayout grid = (GridLayout) convertView;
        grid.removeAllViews();
        Device group = (Device) getGroup(groupPosition);

        List<Channel> channels = new ArrayList<>();
        channels.addAll(group.channels);
        int size = channels.size();
        if (size % 2 == 1) {
            channels.add(null);
        }

        for (int i = 0; i < channels.size(); i++) {
            Channel channel = channels.get(i);
            View item = LayoutInflater.from(parent.getContext()).inflate(R.layout.list_live_item_withphoto, grid, false);
            item.getLayoutParams().width = 0;
            grid.addView(item);
            item.setTag(channel);
            item.setOnClickListener(this);
            if (channel == null) {
                item.setVisibility(View.INVISIBLE);
            } else {
                TextView txtTitle = (TextView) item.findViewById(R.id.txt_title);
                TextView txtTermialType = (TextView) item.findViewById(R.id.txt_terminal_type);
                ImageView imvSnapshot = (ImageView) item.findViewById(R.id.imv_photo);

                txtTitle.setText(String.format("%s（%s）", channel.getName(), channel.getStatus()));
                txtTermialType.setVisibility(View.GONE);
                String url = channel.getSnapURL();
                Glide.with(mContext).load(url).placeholder(R.drawable.snap).into(imvSnapshot);
            }
        }
        return convertView;
    }

    /**
     * 可自定义list
     */
    public View getGroupView(int groupPosition, boolean isExpanded,
                             View convertView, ViewGroup parent) {
        Log.d(TAG, "kim getGroupView groupPosition=" + groupPosition);
        TextView textView = getTextView(mContext);//this.parentContext
        Device nvr = (Device) getGroup(groupPosition);
        String text = String.format("%s:%s:%s", nvr.getAppType(), nvr.getName(), nvr.getSerial());
        textView.setText(text);
        textView.setPadding(myPaddingLeft + PaddingLeft, 0, 0, 0);
        return textView;
    }

    public long getChildId(int groupPosition, int childPosition) {
        return childPosition;
    }

    public Object getGroup(int groupPosition) {
        return mDevice.get(groupPosition);
    }

    public int getGroupCount() {
        return mDevice.size();
    }

    public long getGroupId(int groupPosition) {
        return groupPosition;
    }

    public boolean isChildSelectable(int groupPosition, int childPosition) {
        return true;
    }

    public boolean hasStableIds() {
        return true;
    }


    private void getDeviceRtspUrl(final String serial, final String channel) {
        final MainActivity activity = mContext;
        if (activity == null) return;
        activity.showWaitProgress("正在请求EasyCMS启动视频...");
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                String url = String.format("http://%s:%s/api/v1/startdevicestream?device=%s&channel=%s&reserve=1",
                        MyApplication.getInstance().getIp(),
                        MyApplication.getInstance().getPort(), serial, channel);
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
                    url = String.format("http://%s:%s/api/v1/getdevicestream?device=%s&channel=%s&protocol=RTSP&reserve=1",
                            IP,
                            PORT,
                            serial, channel);

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

                        Intent intent = new Intent(mContext, EasyPlayerActivity.class);
                        intent.putExtra(DarwinConfig.CAM_URL, streamUrl);
                        intent.putExtra(DarwinConfig.DEV_SERIAL, serial);
                        intent.putExtra(DarwinConfig.DEV_TYPE, "nvr");
                        intent.putExtra(DarwinConfig.CHANNEL_ID, channel);
                        mContext.startActivity(intent);
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                } catch (ExecutionException e) {
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
                            activity.hideWaitProgress();
                        }
                    });
                }
            }
        });
    }

    @Override
    public void onClick(View v) {
        Channel channel = (Channel) v.getTag();
        if ("online".equals(channel.getStatus())) {
            getDeviceRtspUrl(channel.getNVRSerial(), channel.getChannel());
        } else {
            Toast.makeText(v.getContext(), "通道不在线", Toast.LENGTH_SHORT).show();
        }
    }
}