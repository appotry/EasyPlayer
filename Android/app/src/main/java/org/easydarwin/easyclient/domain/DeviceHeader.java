/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.domain;

/**
 * Created by dell on 2016/5/31 031.
 */
public class DeviceHeader {

    private String ErrorNum;
    private String ErrorString;
    private String MessageType;

    public String getErrorNum() {
        return ErrorNum;
    }

    public void setErrorNum(String errorNum) {
        ErrorNum = errorNum;
    }

    public String getErrorString() {
        return ErrorString;
    }

    public void setErrorString(String errorString) {
        ErrorString = errorString;
    }

    public String getMessageType() {
        return MessageType;
    }

    public void setMessageType(String messageType) {
        MessageType = messageType;
    }
}
