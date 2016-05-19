package org.easydarwin.easyclient.domain;

/**
 * Created by dell on 2016/5/19 019.
 */
public class DeviceInfoWrapper {

    DeviceInfo EasyDarwin;

    public DeviceInfo getEasyDarwin() {
        return EasyDarwin;
    }

    public void setEasyDarwin(DeviceInfo easyDarwin) {
        EasyDarwin = easyDarwin;
    }

   public class DeviceInfo {
        DeviceInfoBody Body;

        public DeviceInfoBody getBody() {
            return Body;
        }

        public void setBody(DeviceInfoBody body) {
            Body = body;
        }
    }
}
