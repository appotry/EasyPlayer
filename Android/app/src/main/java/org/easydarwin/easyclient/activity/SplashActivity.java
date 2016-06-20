/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.activity;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.view.Window;
import android.widget.TextView;

import org.easydarwin.easyclient.R;

public class SplashActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.splash_activity);
        new Handler().postDelayed(new Runnable() {

            @Override
            public void run() {
                startActivity(new Intent(SplashActivity.this, MainActivity.class));
                SplashActivity.this.finish();
            }
        }, 2000);

        String versionname;
        try {
            versionname = getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
        } catch (PackageManager.NameNotFoundException e) {
            versionname = "";
        }

        if (TextUtils.isEmpty(versionname)) {
            return;
        }
        TextView txtVersion = (TextView) findViewById(R.id.txt_version);
        txtVersion.setText(String.format("EasyClient %s", versionname));

    }

}
