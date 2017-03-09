/*
	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.adapter;

import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.bumptech.glide.Glide;

import org.easydarwin.easyclient.R;
import org.easydarwin.easyclient.domain.DeviceListBody.Device;

import java.util.List;

public class OnlineCameraAdapter extends BaseAdapter {
    private static final String TAG = "OnlineCameraAdapter";
    private List<Device> mDevices;
    private int mSnapshotWidth;
    private int mSnapshotHeight;

    public OnlineCameraAdapter(List<Device> mDevices) {
        this.mDevices = mDevices;
    }

    public void add(List<Device> mDevices) {
        this.mDevices.addAll(mDevices);
        notifyDataSetChanged();
    }

    @Override
    public int getCount() {
        return mDevices.size();
    }

    @Override
    public Object getItem(int position) {
        return mDevices.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        LiveViewHolder viewHolder;
        if (convertView == null) {
            convertView = View.inflate(parent.getContext(), R.layout.list_live_item_withphoto, null);
            viewHolder = new LiveViewHolder();
            viewHolder.relativeLayout= (RelativeLayout) convertView.findViewById(R.id.rlayout_device);
            viewHolder.txtTitle = (TextView) convertView.findViewById(R.id.txt_title);
//            viewHolder.txtAppType = (TextView) convertView.findViewById(R.id.txt_apptype);
            viewHolder.txtTermialType = (TextView) convertView.findViewById(R.id.txt_terminal_type);
            viewHolder.imvSnapshot = (ImageView) convertView.findViewById(R.id.imv_photo);

            LinearLayout.LayoutParams layoutParams=new LinearLayout.LayoutParams(getmSnapshotWidth(),getmSnapshotHeight());

            viewHolder.relativeLayout.setLayoutParams(layoutParams);

            convertView.setTag(viewHolder);
        } else {
            viewHolder = (LiveViewHolder) convertView.getTag();
        }


        Device device = mDevices.get(position);
        viewHolder.txtTitle.setText(String.format("%s（%s）", device.getName(), device.getSerial()));
//        viewHolder.txtAppType.setText(device.getAppType());
        viewHolder.txtTermialType.setText(device.getTerminalType());
        Glide.with(parent.getContext())
                .load(device.getSnapURL())
                .centerCrop()
                .placeholder(R.drawable.snap)
                .into(viewHolder.imvSnapshot);
        return convertView;
    }

    public int getmSnapshotWidth() {
        return mSnapshotWidth;
    }

    public void setmSnapshotWidth(int mSnapshotWidth) {
        this.mSnapshotWidth = mSnapshotWidth;
    }

    public int getmSnapshotHeight() {
        return mSnapshotHeight;
    }

    public void setmSnapshotHeight(int mSnapshotHeight) {
        this.mSnapshotHeight = mSnapshotHeight;
    }

    static class LiveViewHolder {
        TextView txtTitle;
        TextView txtAppType;
        TextView txtTermialType;
        ImageView imvSnapshot;
        RelativeLayout relativeLayout;
    }

}
