package org.easydarwin.easyclient.audio;

/**
 * Created by Kim on 2016/7/9.
 */
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.drawable.AnimationDrawable;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import org.easydarwin.easyclient.R;

public class RecordButton extends Button {

    private static final int MIN_RECORD_TIME = 1; // 最短录音时间，单位秒
    private static final int RECORD_OFF = 0; // 不在录音
    private static final int RECORD_ON = 1; // 正在录音

    private RecordStrategy mAudioRecorder;
    private Thread mRecordThread;

    private int recordState = 0; // 录音状态
    private float recodeTime = 0.0f; // 录音时长，如果录音时间太短则录音失败
    private boolean isCanceled = false; // 是否取消录音
    private float downY;

    private Context mContext;
    private LinearLayout mRecordDlg;
    private TextView dialogTextView;
    private ImageView dialogImg;
    private AnimationDrawable animationDrawable;
    private static int mAudioRecordPermissionCheck = 0;  //-1：禁用权限   0：尚未申请权限   1：已获取到权限
    private static final String TAG = "RecordButton";

    public RecordButton(Context context) {
        super(context);
        init(context);
    }

    public RecordButton(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init(context);
    }

    public RecordButton(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    private void init(Context context) {
        mContext = context;
    }

    public void init(Context context, LinearLayout dlg, ImageView image, TextView text){
        mContext = context;
        mRecordDlg = dlg;
        dialogImg = image;
        dialogTextView = text;
        dialogImg.setImageResource(R.drawable.speaking);
        animationDrawable = (AnimationDrawable)dialogImg.getDrawable();
        mRecordDlg.setVisibility(INVISIBLE);
    }

    public void setAudioRecord(RecordStrategy record) {
        this.mAudioRecorder = record;
    }

    // 录音时显示Dialog
    private void showVoiceDialog(int flag) {
        if(mAudioRecordPermissionCheck == -1)
            return;
        cancelToast();
        mRecordDlg.setVisibility(View.VISIBLE);
        animationDrawable.start();
        switch (flag) {
            case 1:
                dialogTextView.setText(R.string.audio_record_action_up_tip);
                break;

            default:
                dialogTextView.setText(R.string.audio_record_move_up_tip);
                break;
        }
        dialogTextView.setTextSize(14);
    }

    // 录音时间太短时Toast显示
    private Toast mToast;
    public void showWarnToast(int stringId) {
        if(mToast == null) {
            mToast = new Toast(mContext);
            View warnView = LayoutInflater.from(mContext).inflate(
                    R.layout.toast_warn, null);
            mToast.setView(warnView);
            mToast.setGravity(Gravity.CENTER, 0, 0);// 起点位置为中间
            TextView text = (TextView)warnView.findViewById(R.id.tvWarnText);
            text.setText(stringId);
        }
        mToast.show();
    }

    public void cancelToast() {
        if (mToast != null) {
            mToast.cancel();
        }
    }

    // 开启录音计时线程
    private void callRecordTimeThread() {
        mRecordThread = new Thread(recordThread);
        mRecordThread.start();
    }

    // 录音线程
    private Runnable recordThread = new Runnable() {
        @Override
        public void run() {
            recodeTime = 0.0f;
            while (recordState == RECORD_ON) {
                try {
                    Thread.sleep(100);
                    recodeTime += 0.1;
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    };

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN: // 按下按钮
                if (mAudioRecordPermissionCheck == -1){
                    showWarnToast(R.string.audio_record_no_permission);
                    break;
                }

                if (recordState != RECORD_ON) {
                    if (mAudioRecorder != null) {
                        mAudioRecorder.ready();
                        recordState = RECORD_ON;
                        mAudioRecorder.start();
                        callRecordTimeThread();
                    }

                    PackageManager pm = mContext.getPackageManager();
                    String packagename = mContext.getPackageName();
                    boolean permission = (PackageManager.PERMISSION_GRANTED ==
                            pm.checkPermission("android.permission.RECORD_AUDIO", mContext.getPackageName()));

                    if (permission) {
                        showVoiceDialog(0);
                        downY = event.getY();
                    }
                }
                break;
            case MotionEvent.ACTION_CANCEL:
            case MotionEvent.ACTION_UP: // 松开手指
                if (recordState == RECORD_ON) {
                    recordState = RECORD_OFF;
                    animationDrawable.stop();
                    mRecordDlg.setVisibility(INVISIBLE);
                    mAudioRecorder.stop();
                    mRecordThread.interrupt();
                    if (!isCanceled) {
                        if (recodeTime < MIN_RECORD_TIME) {
                            showWarnToast(R.string.audio_record_time_too_short);
                        }
                    }
                    isCanceled = false;

//                    if (mAudioRecordPermissionCheck == 0 && MotionEvent.ACTION_UP == event.getAction()) {
//                        PackageManager pm = mContext.getPackageManager();
//                        boolean permission = (PackageManager.PERMISSION_GRANTED ==
//                                pm.checkPermission("android.permission.RECORD_AUDIO", mContext.getPackageName()));
//
//                        if (permission) {
//                            mAudioRecordPermissionCheck = 1;
//                        } else {
//                            mAudioRecordPermissionCheck = -1;
//                        }
//
//                        if(mAudioRecordPermissionCheck == -1){
//                            showWarnToast(R.string.audio_record_no_permission);
//                        }
//                    }
                }
                break;
            case MotionEvent.ACTION_MOVE: // 滑动手指
                float moveY = event.getY();
                if (downY - moveY > 50) {
                    isCanceled = true;
                    showVoiceDialog(1);
                }
                if (downY - moveY < 20) {
                    isCanceled = false;
                    showVoiceDialog(0);
                }
                break;
        }
        return true;
    }
}
