/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.domain;

/**
 * Created by dell on 2016/5/19 019.
 */
public class DeviceInfoBody {
    private String Channel;
    private String Protocol;
    private String Reserve;
    private String Serial;
    private String URL;

    public String getChannel() {
        return Channel;
    }

    public void setChannel(String channel) {
        Channel = channel;
    }

    public String getProtocol() {
        return Protocol;
    }

    public void setProtocol(String protocol) {
        Protocol = protocol;
    }

    public String getReserve() {
        return Reserve;
    }

    public void setReserve(String reserve) {
        Reserve = reserve;
    }

    public String getSerial() {
        return Serial;
    }

    public void setSerial(String serial) {
        Serial = serial;
    }

    public String getURL() {
        return URL;
    }

    public void setURL(String URL) {
        this.URL = URL;
    }
}
