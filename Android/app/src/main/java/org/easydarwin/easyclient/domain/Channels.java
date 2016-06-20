/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.domain;

/**
 * Created by Kim on 2016/6/18.
 */
public class Channels {
    private String Channel;
    private String Name;
    private String SnapURL;
    private String Status;
    private String NVRSerial;

    public String getName() {
        return Name;
    }

    public void setName(String name) {
        Name = name;
    }

    public String getSnapURL() {
        return SnapURL;
    }

    public void setSnapURL(String snapURL) {
        SnapURL = snapURL;
    }

    public String getChannel() {
        return Channel;
    }

    public void setChannel(String channel) {
        Channel = channel;
    }

    public String getStatus() {
        return Status;
    }

    public void setStatus(String status) {
        Status = status;
    }

    public String getNVRSerial() {
        return NVRSerial;
    }

    public void setNVRSerial(String NVRSerial) {
        this.NVRSerial = NVRSerial;
    }
}
