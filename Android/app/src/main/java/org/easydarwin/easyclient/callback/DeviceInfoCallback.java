/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.callback;

import android.util.Log;

import com.google.gson.Gson;

import org.easydarwin.easyclient.domain.DeviceInfoWrapper;
import org.easydarwin.easyclient.domain.LiveVO;
import org.easydarwin.okhttplibrary.callback.Callback;

import java.io.IOException;

import okhttp3.Response;

/**
 * Created by Helong on 16/3/15-22:26.
 */
public abstract class DeviceInfoCallback extends Callback<DeviceInfoWrapper> {
    @Override
    public DeviceInfoWrapper parseNetworkResponse(Response response) throws IOException
    {
        String string = response.body().string();
        Log.d("DeviceInfoWrapper", "response="+string);
        DeviceInfoWrapper deviceInfoWrapper = new Gson().fromJson(string, DeviceInfoWrapper.class);
        return deviceInfoWrapper;
    }
}
