/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.callback;

import com.google.gson.Gson;

import org.easydarwin.easyclient.domain.RemoteVersionInfo;
import org.easydarwin.okhttplibrary.callback.Callback;

import okhttp3.Call;
import okhttp3.Response;

/**
 * Created by dell on 2016/6/11 011.
 */
public abstract class VersionCallback extends Callback<RemoteVersionInfo> {
    @Override
    public RemoteVersionInfo parseNetworkResponse(Response response) throws Exception {
        String string = response.body().string();
        RemoteVersionInfo versionInfo = new Gson().fromJson(string, RemoteVersionInfo.class);
        return versionInfo;
    }

}
