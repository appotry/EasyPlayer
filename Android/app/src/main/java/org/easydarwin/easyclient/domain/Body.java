/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
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
    private String ChannelCount;
    private List<Channels> Channels;
    private String Serial;

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

    public String getChannelCount() {
        return ChannelCount;
    }

    public void setChannelCount(String channelCount) {
        ChannelCount = channelCount;
    }

    public List<Channels> getChannels() {
        if (Channels != null){
            setNVRSerialForChannels();
        }
        return Channels == null ? new ArrayList<Channels>() : Channels;
    }

    public void setChannels(List<Channels> channels) {
        Channels = channels;
    }

    public String getSerial() {
        return Serial;
    }

    public void setSerial(String serial) {
        Serial = serial;
    }

    private void setNVRSerialForChannels(){
        for(int i = 0; i < Channels.size(); i++){
            Channels.get(i).setNVRSerial(Serial);
        }
    }
}
