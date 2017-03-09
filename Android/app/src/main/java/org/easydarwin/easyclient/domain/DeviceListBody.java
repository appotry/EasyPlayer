package org.easydarwin.easyclient.domain;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by jiaozebo on 2016/12/21.
 */

public class DeviceListBody extends RespSpec{

    int DeviceCount;
    List<Device> Devices = new ArrayList<>();

    public List<Device> getDevices() {
        return Devices;
    }

    public static class Device {

        private String AppType;
        private String Name;
        private String Serial;
        private String SnapURL;
        private String Tag;
        private String TerminalType;

        public String getAppType() {
            return AppType;
        }

        public String getName() {
            return Name;
        }

        public String getSerial() {
            return Serial;
        }

        public String getSnapURL() {
            return SnapURL;
        }

        public String getTag() {
            return Tag;
        }

        public String getTerminalType() {
            return TerminalType;
        }

        public List<Channel> channels = new ArrayList<>();
    }
}
