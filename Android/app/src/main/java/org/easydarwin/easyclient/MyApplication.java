/*
 * 	Copyright (c) 2012-2017 EasyDarwin.ORG.  All rights reserved.
 * 	Github: https://github.com/EasyDarwin
 * 	WEChat: EasyDarwin
 *	Website: http://www.easydarwin.org
 */
package org.easydarwin.easyclient;

import android.app.Application;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.text.TextUtils;
import android.util.Log;

import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import org.easydarwin.easyclient.callback.CallbackWrapper;
import org.easydarwin.easyclient.config.DarwinConfig;

import java.io.IOException;
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;

import okhttp3.FormBody;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class MyApplication extends Application {

    private static final String TAG = "EasyClient";
    private static MyApplication instance;

    @Override
    public void onCreate() {
        super.onCreate();
        instance = this;
        initServerInfo();
    }

    private void initServerInfo() {
        SharedPreferences sharedPreferences = getSharedPreferences(DarwinConfig.SETTING_PREF_NAME, MODE_PRIVATE);
        SharedPreferences.Editor editor = sharedPreferences.edit();

        String serverIp = sharedPreferences.getString(DarwinConfig.SERVER_IP, "");
        if (TextUtils.isEmpty(serverIp)) {
            editor.putString(DarwinConfig.SERVER_IP, DarwinConfig.DEFAULT_SERVER_IP);
            editor.apply();
        }

        String port = sharedPreferences.getString(DarwinConfig.SERVER_PORT, "");
        if (TextUtils.isEmpty(port)) {
            editor.putString(DarwinConfig.SERVER_PORT, DarwinConfig.DEFAULT_SERVER_PORT);
            editor.apply();
        }

    }

    public String getIp() {
        SharedPreferences sharedPreferences = getSharedPreferences(DarwinConfig.SETTING_PREF_NAME, MODE_PRIVATE);
        return sharedPreferences.getString(DarwinConfig.SERVER_IP, DarwinConfig.DEFAULT_SERVER_IP);
    }

    public String getPort() {
        SharedPreferences sharedPreferences = getSharedPreferences(DarwinConfig.SETTING_PREF_NAME, MODE_PRIVATE);
        return sharedPreferences.getString(DarwinConfig.SERVER_PORT, DarwinConfig.DEFAULT_SERVER_PORT);
    }


    /**
     * @return the main context of the Application
     */
    public static MyApplication getInstance() {
        return instance;
    }

    public static Resources getAppResources() {
        return instance.getResources();
    }


    private static final OkHttpClient client = new OkHttpClient();

    public static String get(String url) throws IOException {
        Request request = new Request.Builder()
                .url(url)
                .build();

        Response response = client.newCall(request).execute();
        return response.body().string();
    }

    public static void asyncGet(String url, CallbackWrapper callback) {
        Request request = new Request.Builder()
                .url(url)
                .build();

        client.newCall(request).enqueue(callback);
        callback.onBefore(request);
    }


    public static void asyncPost(String url, String content, CallbackWrapper callback) {
        MediaType MEDIA_TYPE_PLAIN = MediaType.parse("text/plain;charset=utf-8");


        Request request = new Request.Builder()
                .url(url)
                .post(RequestBody.create(MEDIA_TYPE_PLAIN, content))
                .build();

        client.newCall(request).enqueue(callback);
        callback.onBefore(request);
    }

    public static void asyncPost(String url, CallbackWrapper callback) {
        FormBody.Builder builder = new FormBody.Builder();
        Request request = new Request.Builder()
                .url(url)
                .post(builder.build())
                .build();

        client.newCall(request).enqueue(callback);
        callback.onBefore(request);
    }

    public static FutureTask<JsonObject> syncGet(final String url) {
        return new FutureTask<>(new Callable<JsonObject>() {
            @Override
            public JsonObject call() throws Exception {
                Request request = new Request.Builder()
                        .url(url)
                        .build();

                return parseBody(client.newCall(request).execute());
            }
        });
    }


    public static JsonObject parseBody(Response response) throws IOException {
        if (response.isSuccessful()) {
            String resp = response.body().string();
            Log.i(TAG, String.format("%s %s result:\n%s", response.request().method(),response.request().url(),resp));
            JsonObject o = new JsonParser().parse(resp).getAsJsonObject();
            JsonObject EasyDarwin = o.getAsJsonObject("EasyDarwin");
            JsonObject header = EasyDarwin.getAsJsonObject("Header");
            final int code = header.getAsJsonPrimitive("ErrorNum").getAsInt();
            final String error = header.getAsJsonPrimitive("ErrorString").getAsString();
            if (code == 200) {
                return EasyDarwin.getAsJsonObject("Body");
            }
            throw new CallbackWrapper.HttpCodeErrorExcepetion(code, error);
        }
        throw new CallbackWrapper.HttpCodeErrorExcepetion(response.code());
    }
}
