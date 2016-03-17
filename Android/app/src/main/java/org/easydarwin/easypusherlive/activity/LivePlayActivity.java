/*
	Copyright (c) 2013-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
package org.easydarwin.easypusherlive.activity;

import android.content.res.Configuration;
import android.net.Uri;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.widget.TextView;

import org.easydarwin.easypusherlive.R;
import org.easydarwin.easypusherlive.config.DarwinConfig;
import org.easydarwin.easypusherlive.vlc.VLCInstance;
import org.easydarwin.easypusherlive.vlc.VLCOptions;
import org.videolan.libvlc.IVLCVout;
import org.videolan.libvlc.Media;
import org.videolan.libvlc.MediaPlayer;

public class LivePlayActivity extends AppCompatActivity implements IVLCVout.Callback{

    SurfaceView mSurfaceView;
    MediaPlayer mMediaPlayer;
    String TAG="LivePlayActivity";
    boolean isPause=false;
    TextView tvBuff;
    String rtspUrl="";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_live_play);
        mSurfaceView = (SurfaceView) findViewById(R.id.player_surface);
        tvBuff= (TextView) findViewById(R.id.tv_buff);
        mMediaPlayer = new MediaPlayer(VLCInstance.get());
        final IVLCVout vlcVout = mMediaPlayer.getVLCVout();
        vlcVout.setVideoView(mSurfaceView);
        vlcVout.addCallback(this);
        vlcVout.attachViews();
        mSurfaceView.setKeepScreenOn(true);
        rtspUrl=getIntent().getStringExtra(DarwinConfig.RTSP_ADDRESS);
        final Media media = new Media(VLCInstance.get(), Uri.parse(rtspUrl));
        VLCOptions.setMediaOptions(media, this, 1);
        media.setEventListener(mMediaListener);
        mMediaPlayer.setMedia(media);
        media.release();
        mMediaPlayer.setEqualizer(VLCOptions.getEqualizer(this));
        mMediaPlayer.setVideoTitleDisplay(MediaPlayer.Position.Disable, 0);
        mMediaPlayer.setEventListener(mMediaPlayerListener);
        mMediaPlayer.play();
    }

    Media.EventListener mMediaListener = new Media.EventListener() {
        @Override
        public void onEvent(Media.Event event) {
            Log.i(TAG, "mMediaListener__:" + event.type);
        }
    };

    MediaPlayer.EventListener mMediaPlayerListener = new MediaPlayer.EventListener() {
        @Override
        public void onEvent(MediaPlayer.Event event) {
            Log.i(TAG, "mMediaPlayerListener__:" + event.type);
            switch (event.type){
                case MediaPlayer.Event.Stopped:
                    Log.w(TAG,"Stopped");
                    break;
                case MediaPlayer.Event.EndReached:
                    Log.w(TAG,"EndReached");
                    break;
                case MediaPlayer.Event.EncounteredError:
                    Log.w(TAG,"EncounteredError");
                    break;
                case MediaPlayer.Event.Opening:
                    Log.w(TAG,"Opening");
                    break;
                case MediaPlayer.Event.Buffering:
                    tvBuff.setText(event.getBuffering()+"");
                    Log.e(TAG,"________________________******************Buffering:"+event.getBuffering());
                    break;
                case MediaPlayer.Event.Playing:
                    tvBuff.setText("playing");
                    Log.w(TAG,"Playing");
                    break;
                case MediaPlayer.Event.Paused:
                    Log.w(TAG,"Paused");
                    break;
                case MediaPlayer.Event.TimeChanged:
                    Log.w(TAG,"TimeChanged:"+event.getTimeChanged());
                    break;
                case MediaPlayer.Event.PositionChanged:
                    Log.w(TAG,"PositionChanged");
                    break;
                case MediaPlayer.Event.Vout:
                    Log.w(TAG,"Vout");
                    break;
                case MediaPlayer.Event.ESAdded:
                    Log.w(TAG, "ESAdded");
                    break;
                case MediaPlayer.Event.ESDeleted:
                    Log.w(TAG,"ESDeleted");
                    break;
            }
        }
    };

    @Override
    public void onNewLayout(IVLCVout vlcVout, int width, int height, int visibleWidth, int visibleHeight, int sarNum, int sarDen) {
        int sw = getWindow().getDecorView().getWidth();
        int sh = getWindow().getDecorView().getHeight();
        vlcVout.setWindowSize(sw, sh);
        ViewGroup.LayoutParams lp = mSurfaceView.getLayoutParams();
        double dw = sw, dh = sh;
        boolean isPortrait = getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT;
        if (sw > sh && isPortrait || sw < sh && !isPortrait) {
            dw = sh;
            dh = sw;
        }
        if (dw * dh == 0 || width * height == 0) {
            Log.e(TAG, "Invalid surface size");
            return;
        }
        double ar, vw;
        if (sarDen == sarNum) {
            ar = (double)visibleWidth / (double)visibleHeight;
        } else {
            vw = visibleWidth * (double)sarNum / sarDen;
            ar = vw / visibleHeight;
        }
        // compute the display aspect ratio
        double dar = dw / dh;
        if (dar < ar)
            dh = dw / ar;
        else
            dw = dh * ar;
        lp.width  = (int) Math.ceil(dw * width / visibleWidth);
        lp.height = (int) Math.ceil(dh * height / visibleHeight);
        mSurfaceView.setLayoutParams(lp);
        mSurfaceView.invalidate();
    }

    @Override
    public void onSurfacesCreated(IVLCVout vlcVout) {
        Log.i(TAG,"onSurfacesCreated____:");
    }

    @Override
    public void onSurfacesDestroyed(IVLCVout vlcVout) {
        Log.i(TAG,"onSurfacesDestroyed___:");
    }

    @Override
    protected void onPause() {
        if (mMediaPlayer != null&&mMediaPlayer.isPlaying()) {
            final IVLCVout vlcVout = mMediaPlayer.getVLCVout();
            vlcVout.removeCallback(this);
            vlcVout.detachViews();
            mSurfaceView.setKeepScreenOn(false);
            mMediaPlayer.pause();
            isPause=true;
        }
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if(mMediaPlayer!=null&&isPause){

            final IVLCVout vlcVout = mMediaPlayer.getVLCVout();
            vlcVout.setVideoView(mSurfaceView);
            vlcVout.addCallback(this);
            vlcVout.attachViews();

            mSurfaceView.setKeepScreenOn(true);
            mMediaPlayer.play();
            isPause=false;
        }
    }

    @Override
    protected void onDestroy() {
        if (mMediaPlayer != null) {
            mSurfaceView.setKeepScreenOn(false);

            final IVLCVout vlcVout = mMediaPlayer.getVLCVout();
            vlcVout.removeCallback(this);
            vlcVout.detachViews();

            final Media media = mMediaPlayer.getMedia();
            if (media != null) {
                media.setEventListener(null);
                mMediaPlayer.setEventListener(null);
                mMediaPlayer.stop();
                mMediaPlayer.setMedia(null);
                media.release();
            }

            mMediaPlayer.release();
        }
        super.onDestroy();
    }
}
