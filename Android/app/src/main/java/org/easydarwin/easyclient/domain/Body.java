/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.domain;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Helong on 16/3/15-22:23.
 */
public class Body {

    private String DeviceCount;
    private List<Device> Devices;

    public String getDeviceCount() {
        return DeviceCount;
    }

    public void setDeviceCount(String deviceCount) {
        DeviceCount = deviceCount;
    }

    public List<Device> getDevices() {
        return Devices == null ? new ArrayList<Device>() : Devices;
    }

    public void setDevices(List<Device> devices) {
        Devices = devices;
    }
}
