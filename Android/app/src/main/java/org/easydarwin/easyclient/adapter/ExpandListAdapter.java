/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.adapter;

import android.content.Context;
import android.content.Intent;
import android.text.TextUtils;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.BaseExpandableListAdapter;
import android.widget.TextView;
import android.widget.Toast;

import org.easydarwin.easyclient.MyApplication;
import org.easydarwin.easyclient.R;
import org.easydarwin.easyclient.activity.EasyPlayerActivity;
import org.easydarwin.easyclient.activity.MainActivity;
import org.easydarwin.easyclient.callback.DeviceInfoCallback;
import org.easydarwin.easyclient.callback.LiveVOCallback;
import org.easydarwin.easyclient.config.DarwinConfig;
import org.easydarwin.easyclient.domain.Channels;
import org.easydarwin.easyclient.domain.Device;
import org.easydarwin.easyclient.domain.DeviceHeader;
import org.easydarwin.easyclient.domain.DeviceInfoWrapper;
import org.easydarwin.easyclient.domain.LiveVO;
import org.easydarwin.easyclient.view.NoScrollGridView;
import org.easydarwin.okhttplibrary.OkHttpUtils;

import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;
import okhttp3.Request;

/**
 * Created by Kim on 6/17/2016.
 */
public class ExpandListAdapter extends BaseExpandableListAdapter implements
        AdapterView.OnItemClickListener
{
    private static final String TAG = "ExpandListAdapter";
    public static final int ItemHeight = 100;// 每项的高度
    public static final int PaddingLeft = 36;// 每项的高度
    private int myPaddingLeft = 50;
    private NoScrollGridView toolbarGrid;
    private OnlineChannelAdapter liveVOAdapter;

    private List<TreeNode> treeNodes = new ArrayList<TreeNode>();
    private Context mContext;
    private LayoutInflater layoutInflater;
    private List<Device> mDevice;
    private String mServerIp;
    private String mServerPort;

    static public class TreeNode
    {
        Device parent;
        List<Channels> childs = new ArrayList<Channels>();
    }

    public ExpandListAdapter(List<Device> devices) {
        this.mDevice = devices;
        for(int i = 0 ; i < devices.size(); i++) {
            TreeNode node = new TreeNode();
            node.parent = this.mDevice.get(i);
            treeNodes.add(node);
        }
        mContext = MainActivity.instance;
    }

    public ExpandListAdapter(Context view, int myPaddingLeft)
    {
        mContext = view;
        this.myPaddingLeft = myPaddingLeft;
    }

    public List<TreeNode> GetTreeNode()
    {
        return treeNodes;
    }

    public void UpdateTreeNode(List<TreeNode> nodes)
    {
        treeNodes = nodes;
    }

    public void RemoveAll()
    {
        treeNodes.clear();
    }

    public Object getChild(int groupPosition, int childPosition)
    {
        return treeNodes.get(groupPosition).childs.get(childPosition);
    }

    public int getChildrenCount(int groupPosition)
    {
        //return treeNodes.get(groupPosition).childs.size();
        return 1;
    }

    static public TextView getTextView(Context context)
    {
        AbsListView.LayoutParams lp = new AbsListView.LayoutParams(
                ViewGroup.LayoutParams.FILL_PARENT, ItemHeight);

        TextView textView = new TextView(context);
        textView.setLayoutParams(lp);
        textView.setGravity(Gravity.CENTER_VERTICAL | Gravity.LEFT);
        return textView;
    }

    /**
     * 可自定义ExpandableListView
     */
    public View getChildView(int groupPosition, int childPosition,
                             boolean isLastChild, View convertView, ViewGroup parent)
    {
//        if (convertView == null)
        {
            layoutInflater = (LayoutInflater) mContext.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = layoutInflater.inflate(R.layout.gridview, null);

            toolbarGrid = (NoScrollGridView) convertView
                    .findViewById(R.id.gridview_toolbar);
//            toolbarGrid.setNumColumns(2);// 设置每行列数
            toolbarGrid.setGravity(Gravity.CENTER);// 位置居中
//            toolbarGrid.setHorizontalSpacing(10);// 水平间隔
            toolbarGrid.setOnItemClickListener(this);

            treeNodes.get(groupPosition).childs.clear();
            String serial = treeNodes.get(groupPosition).parent.getSerial();
            getChannels(serial, groupPosition, childPosition);
        }
        return convertView;
    }

    /**
     * 可自定义list
     */
    public View getGroupView(int groupPosition, boolean isExpanded,
                             View convertView, ViewGroup parent)
    {
        Log.d(TAG, "kim getGroupView groupPosition="+groupPosition);
        TextView textView = getTextView(mContext);//this.parentContext
        Device nvr = (Device)getGroup(groupPosition);
        String text = String.format("%s:%s:%s", nvr.getAppType(), nvr.getName(), nvr.getSerial());
        textView.setText(text);
        textView.setPadding(myPaddingLeft + PaddingLeft, 0, 0, 0);
        return textView;
    }

    public long getChildId(int groupPosition, int childPosition)
    {
        return childPosition;
    }

    public Object getGroup(int groupPosition)
    {
        return treeNodes.get(groupPosition).parent;
    }

    public int getGroupCount()
    {
        return treeNodes.size();
    }

    public long getGroupId(int groupPosition)
    {
        return groupPosition;
    }

    public boolean isChildSelectable(int groupPosition, int childPosition)
    {
        return true;
    }

    public boolean hasStableIds()
    {
        return true;
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position,
                            long id)
    {
        Channels channel = (Channels) parent.getAdapter().getItem(position);
        getDeviceRtspUrl(channel.getNVRSerial(), channel.getChannel());
    }

    /////////////////////////////////////////////////////////////

    private void getChannels(String serial, int groupPosition, int childPosition) {
        mServerIp = MyApplication.getInstance().getIp();
        mServerPort = MyApplication.getInstance().getPort();
        getChannels(mServerIp, mServerPort, serial, groupPosition, childPosition);
    }

    /**
     * 获取直播地址列表
     *
     * @param ip   服务器地址
     * @param port 服务器端口号
     */
    private void getChannels(String ip, String port, String serial, final int groupPosition, int childPosition) {
        if (TextUtils.isEmpty(ip) || TextUtils.isEmpty(port)) {
            return;
        }

        String url = String.format("http://%s:%s/api/getdeviceinfo?device=%s", ip, port, serial);
        Log.d(TAG, "camera url="+url);
        OkHttpUtils.post().url(url).build().execute(new LiveVOCallback() {

            @Override
            public void onBefore(Request request) {
                MainActivity.instance.showWaitProgress("");
            }

            @Override
            public void onAfter() {
                MainActivity.instance.hideWaitProgress();
            }

            @Override
            public void onError(Call call, Exception e) {
                Toast.makeText(MainActivity.instance, "onError:" + e.toString(), Toast.LENGTH_LONG).show();
            }

            @Override
            public void onResponse(LiveVO liveVO) {
                List<Channels> channels = liveVO.getEasyDarwin().getBody().getChannels();
                if (channels.size() == 0) {
                    MainActivity.instance.showToadMessage("暂无直播信息");
                    liveVOAdapter = new OnlineChannelAdapter(new ArrayList<Channels>());
                } else {
                    liveVOAdapter = new OnlineChannelAdapter(channels);
                    int screenW = mContext.getResources().getDisplayMetrics().widthPixels;
                    int columnum = toolbarGrid.getNumColumns();
                    int space = (int)(mContext.getResources().getDimension(R.dimen.gridview_horizontalspacing));
                    int itemWidth = (int)((screenW-(columnum-1)*space)/columnum);
                    int itemHeight = (int) (itemWidth * 9 / 16.0 + 0.5f);
                    liveVOAdapter.setmSnapshotWidth(itemWidth);
                    liveVOAdapter.setmSnapshotHeight(itemHeight);
                }

                for(int i = 0; i < channels.size(); i++) {
                    treeNodes.get(groupPosition).childs.add(channels.get(i));
                }

                toolbarGrid.setAdapter(liveVOAdapter);
            }
        });
    }

    private void getDeviceRtspUrl(final String serial, final String channel){
        String url=String.format("http://%s:%s/api/getdevicestream?device=%s&channel=%s&protocol=RTSP",
                MyApplication.getInstance().getIp(),
                MyApplication.getInstance().getPort(),serial,channel);
        OkHttpUtils.post().url(url).build().execute(new DeviceInfoCallback(){
            @Override
            public void onBefore(Request request) {
                MainActivity.instance.showWaitProgress("");
            }

            @Override
            public void onAfter() {
                MainActivity.instance.hideWaitProgress();
            }
            @Override
            public void onError(Call call, Exception e) {
                Toast.makeText(MainActivity.instance, "onError:" + e.toString(), Toast.LENGTH_LONG).show();
            }

            @Override
            public void onResponse(DeviceInfoWrapper deviceInfoWrapper) {
                if(deviceInfoWrapper.getEasyDarwin().getBody()==null){
                    DeviceHeader header=deviceInfoWrapper.getEasyDarwin().getHeader();
                    Toast.makeText(MainActivity.instance, header.getErrorString()+"(" +header.getErrorNum()+")",
                            Toast.LENGTH_LONG).show();
                    return;
                }
                Intent intent = new Intent(MainActivity.instance, EasyPlayerActivity.class);
                intent.putExtra(DarwinConfig.CAM_URL, deviceInfoWrapper.getEasyDarwin().getBody().getURL());
                intent.putExtra(DarwinConfig.DEV_SERIAL, serial);
                intent.putExtra(DarwinConfig.DEV_TYPE, "nvr");
                intent.putExtra(DarwinConfig.CHANNEL_ID, channel);
                MainActivity.instance.startActivity(intent);
            }
        });
    }
}