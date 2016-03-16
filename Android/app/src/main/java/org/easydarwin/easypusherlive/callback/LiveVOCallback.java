package org.easydarwin.easypusherlive.callback;

import com.google.gson.Gson;

import org.easydarwin.easypusherlive.domain.LiveVO;
import org.easydarwin.okhttplibrary.callback.Callback;

import java.io.IOException;

import okhttp3.Response;

/**
 * Created by Helong on 16/3/15-22:26.
 */
public abstract class LiveVOCallback extends Callback<LiveVO> {
    @Override
    public LiveVO parseNetworkResponse(Response response) throws IOException
    {
        String string = response.body().string();
        LiveVO liveVO = new Gson().fromJson(string, LiveVO.class);
        return liveVO;
    }
}
