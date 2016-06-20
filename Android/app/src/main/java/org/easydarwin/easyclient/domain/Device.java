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
public class Device {

    private String AppType;
    private String Name;
    private String Serial;
    private String SnapURL;
    private String Tag;
    private String TerminalType;

    public String getAppType() {
        return AppType;
    }

    public void setAppType(String appType) {
        AppType = appType;
    }

    public String getName() {
        return Name;
    }

    public void setName(String name) {
        Name = name;
    }

    public String getSerial() {
        return Serial;
    }

    public void setSerial(String serial) {
        Serial = serial;
    }

    public String getSnapURL() {
        return SnapURL;
    }

    public void setSnapURL(String snapURL) {
        SnapURL = snapURL;
    }

    public String getTag() {
        return Tag;
    }

    public void setTag(String tag) {
        Tag = tag;
    }

    public String getTerminalType() {
        return TerminalType;
    }

    public void setTerminalType(String terminalType) {
        TerminalType = terminalType;
    }
}
