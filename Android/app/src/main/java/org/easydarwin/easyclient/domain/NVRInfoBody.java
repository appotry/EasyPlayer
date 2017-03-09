package org.easydarwin.easyclient.domain;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by John on 2016/12/22.
 */

public class NVRInfoBody extends RespSpec {
    public int ChannelCount = 0;
    public List<Channel> Channels = new ArrayList<>();
    public String Serial;
}
