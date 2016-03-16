package org.easydarwin.easypusherlive.activity;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.Toast;

import org.easydarwin.easypusherlive.R;
import org.easydarwin.easypusherlive.adapter.LiveVOAdapter;
import org.easydarwin.easypusherlive.callback.LiveVOCallback;
import org.easydarwin.easypusherlive.domain.LiveSession;
import org.easydarwin.easypusherlive.domain.LiveVO;
import org.easydarwin.okhttplibrary.OkHttpUtils;

import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;

public class MainActivity extends AppCompatActivity implements AdapterView.OnItemClickListener{

    ListView listView;
    LiveVOAdapter liveVOAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        listView= (ListView) findViewById(R.id.list_live);
        liveVOAdapter=new LiveVOAdapter(new ArrayList<LiveSession>());
        listView.setAdapter(liveVOAdapter);
        listView.setOnItemClickListener(this);
        getSessions();
    }

    private void getSessions(){
        OkHttpUtils
                .post()
                .url("http://www.easydss.com:18080/api/getrtsppushsessions")
                .build()
                .execute(new LiveVOCallback() {
                    @Override
                    public void onError(Call call, Exception e) {
                        Toast.makeText(MainActivity.this, "onError:" + e.getMessage(), Toast.LENGTH_LONG).show();
                    }

                    @Override
                    public void onResponse(LiveVO liveVO) {
                        List<LiveSession> sessions = liveVO.getEasyDarwin().getBody().getSessions();
                        liveVOAdapter.add(sessions);

                    }
                });
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        LiveSession session= (LiveSession) parent.getAdapter().getItem(position);
        Intent intent=new Intent(this,LivePlayActivity.class);
        intent.putExtra("rtsp",session.getUrl());
        startActivity(intent);
    }
}
