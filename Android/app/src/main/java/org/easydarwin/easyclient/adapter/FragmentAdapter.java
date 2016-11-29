/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.adapter;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;

import org.easydarwin.easyclient.fragment.AndroidFragment;
import org.easydarwin.easyclient.fragment.CameraFragment;
import org.easydarwin.easyclient.fragment.NVRFragment;

/**
 * Created by Kim on 2016/6/16
 */
public class FragmentAdapter extends FragmentPagerAdapter {
    public FragmentAdapter(FragmentManager fm) {
        super(fm);
    }


    private CameraFragment fragmentCamera;
    private AndroidFragment fragmentAndroid;
    private NVRFragment fragmentNvr;

    private String[] titles = new String[]{"EasyCamera", "移动设备", "EasyNVR"};

    public void set(){};

    @Override
    public Fragment getItem(int position) {
        switch (position) {
            case 0:
                if (fragmentCamera == null) {
                    fragmentCamera = new CameraFragment();
                }
                return fragmentCamera;
            case 1:
                if (fragmentAndroid == null) {
                    fragmentAndroid = new AndroidFragment();
                }
                return fragmentAndroid;
            case 2:
                if(fragmentNvr == null) {
                    fragmentNvr = new NVRFragment();
                }
                return fragmentNvr;
            default:
                if (fragmentCamera == null) {
                    fragmentCamera = new CameraFragment();
                }
                return fragmentCamera;
        }
    }

    @Override
    public int getCount() {

        return titles.length;
    }

    @Override
    public String getPageTitle(int position) {
        return titles[position];
    }
}
