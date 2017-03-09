/*
	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.fragment;


import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.widget.SwipeRefreshLayout;
import android.text.SpannableString;
import android.text.Spanned;
import android.text.method.LinkMovementMethod;
import android.text.style.ForegroundColorSpan;
import android.text.style.URLSpan;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.GridView;
import android.widget.TextView;

import org.easydarwin.easyclient.R;

/**
 * Created by Kim on 2016/6/16
 */
public class AndroidFragment extends CameraFragment {
    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mType = "Android|iOS";
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

        View view = inflater.inflate(R.layout.fragment_android, container, false);
        mSwipeRefreshLayout = (SwipeRefreshLayout) view.findViewById(R.id.swip_refersh);
        mSwipeRefreshLayout.setOnRefreshListener(this);
        mGrid_live = (GridView) view.findViewById(R.id.adapter_view);
        mGrid_live.setOnItemClickListener(this);
        return view;
    }

    protected int getItemHeight(int itemWidth) {
        return (int) (itemWidth * 4 / 3.0 + 0.5f);
    }


    protected void onSetTipContent(TextView tipContent) {
        /*
        以 Android,iOS 手机做为摄像机视频源,接入到 EasyDarwin 云平台对外提供音视频服务!

EasyCamera 移动端版本下载:
1、Android：http://fir.im/EasyCamera
2、iOS：在AppStore搜索'EasyCamera'

或者通过手机扫码下载:"
        * */
        tipContent.append("以 Android,iOS 手机做为摄像机视频源,接入到 EasyDarwin 云平台对外提供音视频服务!\n\nEasyCamera 移动端版本下载:1、Android：");
        tipContent.setMovementMethod(LinkMovementMethod.getInstance());
        SpannableString spannableString = new SpannableString("http://fir.im/EasyCamera");
        //设置下划线文字
        spannableString.setSpan(new URLSpan("http://fir.im/EasyCamera"), 0, spannableString.length(), Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
        //设置文字的前景色
        spannableString.setSpan(new ForegroundColorSpan(Color.RED), 0, spannableString.length(), Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
        tipContent.append(spannableString);
        tipContent.append("\n2、iOS：在AppStore搜索'EasyCamera'\n\n或者通过手机扫码下载:");
    }
}
