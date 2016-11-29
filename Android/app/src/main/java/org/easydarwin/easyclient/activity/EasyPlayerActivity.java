/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easyclient.activity;

import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.os.ResultReceiver;
import android.support.v4.view.GestureDetectorCompat;
import android.support.v4.view.ViewConfigurationCompat;
import android.text.TextUtils;
import android.util.Log;
import android.view.GestureDetector;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import org.easydarwin.easyclient.MyApplication;
import org.easydarwin.easyclient.R;
import org.easydarwin.easyclient.audio.AudioRecorder;
import org.easydarwin.easyclient.audio.RecordButton;
import org.easydarwin.easyclient.callback.DeviceInfoCallback;
import org.easydarwin.easyclient.config.DarwinConfig;
import org.easydarwin.easyclient.domain.DeviceHeader;
import org.easydarwin.easyclient.domain.DeviceInfoWrapper;
import org.easydarwin.okhttplibrary.OkHttpUtils;
import org.easydarwin.video.EasyRTSPClient;
import org.easydarwin.video.RTSPClient;

import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;

public class EasyPlayerActivity extends BaseActivity implements SurfaceHolder.Callback, View.OnTouchListener, AudioRecorder.RecordListener {
    private String mRTSPUrl;
    private String mDevSerial;
    private String mDevType; //camera  android   nvr
    private int mChannelId = 0;
    private EasyRTSPClient mStreamRender;
    private ResultReceiver mResultReceiver;
    private GestureDetectorCompat mDetector;
    private int mWidth;
    private int mHeight;
    private String TAG = "EasyPlayerActivity";
    private RelativeLayout mRlControl;
    private Button mBtnMoveUp;
    private Button mBtnMoveDown;
    private Button mBtnMoveLeft;
    private Button mBtnMoveRight;
    private RecordButton mRecordBtn;
    private EasyPlayerActivity mContext;
    private double mLastLen = 0;
    private int mZoomingMode = 0;//0 - not zooming  1 - Zoomin  2 - Zoomout
    private long mSeq = 1;
    private static boolean mSendingTalkData;
    private List<String> mTalkCmdDataList = new ArrayList<String>();
    private boolean mIsTalking = false;
    private LinearLayout mRecordDlg;
    private ImageView mRecordImg;
    private TextView mRecordText;

    class MyGestureListener extends GestureDetector.SimpleOnGestureListener {

        @Override
        public boolean onDown(MotionEvent event) {

            return true;
        }

        @Override
        public boolean onDoubleTap(MotionEvent motionEvent) {
            setRequestedOrientation((getRequestedOrientation() == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT || getRequestedOrientation() == ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT) ?
                    ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE : ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
            return true;
        }

        @Override
        public boolean onSingleTapConfirmed(MotionEvent motionEvent){
            if(mRlControl.getVisibility() == View.VISIBLE)
                mRlControl.setVisibility(View.GONE);
            else
                mRlControl.setVisibility(View.VISIBLE);
            return true;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "kim onCreate");
        if (getSupportActionBar() != null) {
            getSupportActionBar().hide();
        }

        mRTSPUrl = getIntent().getStringExtra(DarwinConfig.CAM_URL);
        if (TextUtils.isEmpty(mRTSPUrl)) {
            finish();
            return;
        }

        mDevSerial = getIntent().getStringExtra(DarwinConfig.DEV_SERIAL);

        mDevType = getIntent().getStringExtra(DarwinConfig.DEV_TYPE);
        if (TextUtils.isEmpty(mDevType)) {
            finish();
            return;
        }

        String channel = getIntent().getStringExtra(DarwinConfig.CHANNEL_ID);
        if(!TextUtils.isEmpty(channel)){
            mChannelId = Integer.parseInt(channel);
        }

        mContext = this;

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        if (isLandscape()) {
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        }
        setContentView(R.layout.activity_easyplayer);

        final SurfaceView surfaceView = (SurfaceView) findViewById(R.id.surface_view);
        surfaceView.getHolder().addCallback(this);
        mDetector = new GestureDetectorCompat(this, new MyGestureListener());

        mResultReceiver = new ResultReceiver(new Handler()) {
            @Override
            protected void onReceiveResult(int resultCode, Bundle resultData) {
                super.onReceiveResult(resultCode, resultData);
                if (resultCode == EasyRTSPClient.RESULT_VIDEO_DISPLAYED) {
                    findViewById(android.R.id.progress).setVisibility(View.GONE);
                } else if (resultCode == EasyRTSPClient.RESULT_VIDEO_SIZE) {
                    mWidth = resultData.getInt(EasyRTSPClient.EXTRA_VIDEO_WIDTH);
                    mHeight = resultData.getInt(EasyRTSPClient.EXTRA_VIDEO_HEIGHT);
                    //if (!isLandscape()) {

                        fixPlayerRatio(surfaceView, getResources().getDisplayMetrics().widthPixels, getResources().getDisplayMetrics().heightPixels);
                    //}
                } else if (resultCode == EasyRTSPClient.RESULT_TIMEOUT) {
                    Toast.makeText(EasyPlayerActivity.this, "试播时间到", Toast.LENGTH_SHORT).show();
                }
            }
        };

        mRlControl = (RelativeLayout)this.findViewById(R.id.rlControl);
        mBtnMoveUp = (Button)this.findViewById(R.id.btMoveUp);
        mBtnMoveUp.setOnTouchListener(this);
        mBtnMoveDown = (Button)this.findViewById(R.id.btMoveDown);
        mBtnMoveDown.setOnTouchListener(this);
        mBtnMoveLeft = (Button)this.findViewById(R.id.btMoveLeft);
        mBtnMoveLeft.setOnTouchListener(this);
        mBtnMoveRight = (Button)this.findViewById(R.id.btMoveRight);
        mBtnMoveRight.setOnTouchListener(this);

        mRecordImg = (ImageView)this.findViewById(R.id.record_dialog_img);
        mRecordText = (TextView)this.findViewById(R.id.record_dialog_txt);
        mRecordDlg = (LinearLayout)this.findViewById(R.id.dlgRecord);

        mRecordBtn = (RecordButton)this.findViewById(R.id.btRecordBtn);
        mRecordBtn.init(this, mRecordDlg, mRecordImg, mRecordText);
        AudioRecorder audioRecord = new AudioRecorder();
        audioRecord.setRecordListener(this);
        mRecordBtn.setAudioRecord(audioRecord);

        //EasyNVR will support this function soon
        if(mDevType.equals("nvr")){
            mRecordBtn.setVisibility(View.INVISIBLE);
        }else if(mDevType.equals("android")){
            setPtzControlVisiable(false);
        } else {
            setPtzControlVisiable(true);
        }

        if (isLandscape()){//横屏
            RelativeLayout.LayoutParams moveParam = (RelativeLayout.LayoutParams) mRlControl.getLayoutParams();
            moveParam.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM, 0);
            moveParam.addRule(RelativeLayout.ALIGN_PARENT_RIGHT, 1);
            moveParam.addRule(RelativeLayout.CENTER_HORIZONTAL, 0);
            moveParam.addRule(RelativeLayout.CENTER_VERTICAL, 1);
            moveParam.setMargins(0,0,50,0);
            mRlControl.setLayoutParams(moveParam);
        } else {//竖屏
            RelativeLayout.LayoutParams moveParam = (RelativeLayout.LayoutParams) mRlControl.getLayoutParams();
            moveParam.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM, 1);
            moveParam.addRule(RelativeLayout.ALIGN_PARENT_RIGHT, 0);
            moveParam.addRule(RelativeLayout.CENTER_HORIZONTAL, 1);
            moveParam.addRule(RelativeLayout.CENTER_VERTICAL, 0);
            moveParam.setMargins(0,0,0,80);
            mRlControl.setLayoutParams(moveParam);
        }
    }

    private void setPtzControlVisiable(boolean visiable){
        if(visiable){
            mBtnMoveUp.setVisibility(View.VISIBLE);
            mBtnMoveDown.setVisibility(View.VISIBLE);
            mBtnMoveLeft.setVisibility(View.VISIBLE);
            mBtnMoveRight.setVisibility(View.VISIBLE);
        } else {
            mBtnMoveUp.setVisibility(View.INVISIBLE);
            mBtnMoveDown.setVisibility(View.INVISIBLE);
            mBtnMoveLeft.setVisibility(View.INVISIBLE);
            mBtnMoveRight.setVisibility(View.INVISIBLE);
        }
    }

    private void fixPlayerRatio(View renderView, int maxWidth, int maxHeight) {
        int widthSize = maxWidth;
        int heightSize = maxHeight;
        int width = mWidth, height = mHeight;
        float aspectRatio = width * 1.0f / height;

        if (widthSize > heightSize * aspectRatio) {
            height = heightSize;
            width = (int) (height * aspectRatio);
        } else {
            width = widthSize;
            height = (int) (width / aspectRatio);
        }
        renderView.getLayoutParams().width = width;
        renderView.getLayoutParams().height = height;
        renderView.requestLayout();
    }

    private boolean isLandscape() {
        int ori = getRequestedOrientation();
        return getRequestedOrientation() == ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE || getRequestedOrientation() == ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE;
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        final View render = findViewById(R.id.surface_view);
        if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
            fixPlayerRatio(render, getResources().getDisplayMetrics().widthPixels, getResources().getDisplayMetrics().heightPixels);
            setNavVisibility(true);
            final RelativeLayout.LayoutParams moveParam = (RelativeLayout.LayoutParams) mRlControl.getLayoutParams();
            moveParam.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM, 0);
            moveParam.addRule(RelativeLayout.ALIGN_PARENT_RIGHT, 1);
            moveParam.addRule(RelativeLayout.CENTER_HORIZONTAL, 0);
            moveParam.addRule(RelativeLayout.CENTER_VERTICAL, 1);
            moveParam.setMargins(0,0,50,0);
            mRlControl.setLayoutParams(moveParam);
        } else {
            getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);

            fixPlayerRatio(render, getResources().getDisplayMetrics().widthPixels, getResources().getDisplayMetrics().heightPixels);
            setNavVisibility(false);

            final RelativeLayout.LayoutParams moveParam = (RelativeLayout.LayoutParams) mRlControl.getLayoutParams();
            moveParam.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM, 1);
            moveParam.addRule(RelativeLayout.ALIGN_PARENT_RIGHT, 0);
            moveParam.addRule(RelativeLayout.CENTER_HORIZONTAL, 1);
            moveParam.addRule(RelativeLayout.CENTER_VERTICAL, 0);
            moveParam.setMargins(0,0,0,80);
            mRlControl.setLayoutParams(moveParam);
        }
    }

    public void setNavVisibility(boolean visible) {
        if (!ViewConfigurationCompat.hasPermanentMenuKey(ViewConfiguration.get(this))) {
            int newVis = View.SYSTEM_UI_FLAG_LAYOUT_STABLE;
            if (!visible) {
                newVis |= View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_IMMERSIVE;
            }
            getWindow().getDecorView().setSystemUiVisibility(newVis);
        }
    }

    @Override
    public void surfaceCreated(final SurfaceHolder surfaceHolder) {
        startRending(surfaceHolder.getSurface());
    }

    private void startRending(Surface surface) {
        mStreamRender = new EasyRTSPClient(this, "1F7A71441EC837799152DB76E2C38022", surface, mResultReceiver);
        mStreamRender.start(mRTSPUrl, RTSPClient.TRANSTYPE_TCP, RTSPClient.EASY_SDK_VIDEO_FRAME_FLAG | RTSPClient.EASY_SDK_AUDIO_FRAME_FLAG, "admin", "admin");
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        if (mStreamRender != null) {
            mStreamRender.stop();
            mStreamRender = null;
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if(TextUtils.isEmpty(mDevSerial)){
            return true;
        }

        switch (event.getAction()){
            case MotionEvent.ACTION_DOWN:
                switch (v.getId()){
                    case R.id.btMoveUp:
                        sendControlCommand(ControlCmd.CMD_MOVEUP, ControlType.TYPE_CONTINUE);
                        break;
                    case R.id.btMoveDown:
                        sendControlCommand(ControlCmd.CMD_MOVEDOWN, ControlType.TYPE_CONTINUE);
                        break;
                    case R.id.btMoveLeft:
                        sendControlCommand(ControlCmd.CMD_MOVELEFT, ControlType.TYPE_CONTINUE);
                        break;
                    case R.id.btMoveRight:
                        sendControlCommand(ControlCmd.CMD_MOVERIGHT, ControlType.TYPE_CONTINUE);
                        break;
                }
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                sendControlCommand(ControlCmd.CMD_MOVESTOP, ControlType.TYPE_CONTINUE);
                break;
            default:
                break;
        }

        return false;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int nCnt = event.getPointerCount();

        if( (event.getAction()&MotionEvent.ACTION_MASK) == MotionEvent.ACTION_POINTER_DOWN && 2 <= nCnt)
        {
            int xlen = Math.abs((int)event.getX(0) - (int)event.getX(1));
            int ylen = Math.abs((int)event.getY(0) - (int)event.getY(1));

            mLastLen = Math.sqrt((double)xlen*xlen + (double)ylen * ylen);
            mZoomingMode = 0;
            return false;
        }
        else if( ((event.getAction()&MotionEvent.ACTION_MASK) == MotionEvent.ACTION_MOVE) && (2 <= nCnt) && (mLastLen > 0)){
            int xlen = Math.abs((int)event.getX(0) - (int)event.getX(1));
            int ylen = Math.abs((int)event.getY(0) - (int)event.getY(1));
            double nLenEnd = Math.sqrt((double)xlen*xlen + (double)ylen * ylen);
            if(nLenEnd > mLastLen)//通过两个手指开始距离和结束距离，来判断放大缩小
            {
                if(mZoomingMode != 1) {
                    mZoomingMode = 1;
                    sendControlCommand(ControlCmd.CMD_ZOMEIN, ControlType.TYPE_CONTINUE);
                }
            }
            else
            {
                if(mZoomingMode != 2) {
                    mZoomingMode = 2;
                    sendControlCommand(ControlCmd.CMD_ZOMEOUT, ControlType.TYPE_CONTINUE);
                }
            }
        }
        else if( (event.getAction()&MotionEvent.ACTION_MASK) == MotionEvent.ACTION_POINTER_UP  && 2 <= nCnt)
        {
            if(mZoomingMode != 0) {
                mLastLen = -1;
                mZoomingMode = 0;
                sendControlCommand(ControlCmd.CMD_MOVESTOP, ControlType.TYPE_CONTINUE);
            }
            return false;
        }

        return mDetector.onTouchEvent(event);
    }

    void sendControlCommand(ControlCmd cmd, ControlType type){
        String url=String.format("http://%s:%s/api/ptzcontrol?device=%s&channel=%d&actiontype=%s&command=%s&speed=5&protocol=onvif",
                MyApplication.getInstance().getIp(),
                MyApplication.getInstance().getPort(),
                mDevSerial,
                mChannelId,
                type.GetDes(),
                cmd.GetDes());

        OkHttpUtils.post().url(url).build().execute(new DeviceInfoCallback() {
            @Override
            public void onError(Call call, Exception e) {
                mContext.showToadMessage("onError:" + e.toString());
            }

            @Override
            public void onResponse(DeviceInfoWrapper response) {
                if(response.getEasyDarwin().getBody()==null){
                    DeviceHeader header=response.getEasyDarwin().getHeader();
                    mContext.showToadMessage(header.getErrorString()+"(" +header.getErrorNum()+")");
                    return;
                }
            }
        });
    }

    private void buildTalkBackCommand(String cmd, long pts, String data){
        String json = String.format("{\n" +
                "  \"EasyDarwin\": {\n" +
                "    \"Body\": {\n" +
                "      \"Channel\": \"%d\",\n" +
                "      \"Protocol\": \"ONVIF\",\n" +
                "      \"Reserve\": \"1\",\n" +
                "      \"Command\": \"%s\",\n" +
                "      \"AudioType\": \"G711A\",\n" +
                "      \"Pts\": \"%d\",\n" +
                "      \"AudioData\": \"%s\",\n" +
                "      \"Serial\": \"%s\"\n" +
                "    },\n" +
                "    \"Header\": {\n" +
                "      \"CSeq\": \"%d\",\n" +
                "      \"MessageType\": \"MSG_CS_TALKBACK_CONTROL_REQ\",\n" +
                "      \"Version\": \"1.0\"\n" +
                "    }\n" +
                "  }\n" +
                "}", mChannelId, cmd, pts, data,mDevSerial, mSeq++);

        synchronized (mTalkCmdDataList) {
            mTalkCmdDataList.add(json);
        }

        return;
    }

    private void startSendTalkCmdData(){
        Thread th = new Thread(new Runnable() {
            @Override
            public void run() {
                String url=String.format("http://%s:%s",
                        MyApplication.getInstance().getIp(),
                        MyApplication.getInstance().getPort());
                EasyPlayerActivity.mSendingTalkData = false;

                int i = 0;
                while (mIsTalking || i < mTalkCmdDataList.size()){
                    if(false == EasyPlayerActivity.mSendingTalkData && i < mTalkCmdDataList.size()) {
                        String data;
                        synchronized (mTalkCmdDataList) {
                            data = mTalkCmdDataList.get(i++);
                        }
//                        Log.d(TAG, "kim send data : "+data);
                        EasyPlayerActivity.mSendingTalkData = true;
                        OkHttpUtils.postString().url(url).content(data).build().execute(new DeviceInfoCallback() {
                            @Override
                            public void onError(Call call, Exception e) {
                                Log.d(TAG, "kim Start onError");
                                EasyPlayerActivity.mSendingTalkData = false;
                            }

                            @Override
                            public void onResponse(DeviceInfoWrapper response) {
                                Log.d(TAG, "kim Start onResponse");
                                EasyPlayerActivity.mSendingTalkData = false;
                            }
                        });
                    }
                }

                mTalkCmdDataList.clear();
            }
        });
        th.start();
    }

    @Override
    public void recordStart(long presentationTimeStamp){
        mSeq = 1;
        mTalkCmdDataList.clear();
        mIsTalking = true;
        buildTalkBackCommand("START", presentationTimeStamp, "");
        startSendTalkCmdData();
    }

    @Override
    public void recordSendData(String data, long presentationTimeStamp){
        buildTalkBackCommand("SENDDATA", presentationTimeStamp, data);
    }

    @Override
    public void recordEnd(long presentationTimeStamp){
        buildTalkBackCommand("STOP", presentationTimeStamp, "");
        mIsTalking = false;
    }
}

enum ControlType{
    TYPE_SINGLE("single"),
    TYPE_CONTINUE("continuous");
    private String des;

    private ControlType(String string) {
        des=string;
    }

    public String GetDes() {
        return des;
    }
}

enum ControlCmd{
    CMD_MOVESTOP("stop"),
    CMD_MOVEUP("up"),
    CMD_MOVEDOWN("down"),
    CMD_MOVELEFT("left"),
    CMD_MOVERIGHT("right"),
    CMD_ZOMEIN("zoomin"),
    CMD_ZOMEOUT("zoomout");
    private String des;

    private ControlCmd(String string) {
        des=string;
    }

    public String GetDes() {
        return des;
    }
}