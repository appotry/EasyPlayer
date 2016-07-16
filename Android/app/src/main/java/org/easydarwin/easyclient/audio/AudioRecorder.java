package org.easydarwin.easyclient.audio;

/**
 * Created by Kim on 2016/7/9.
 */
import java.io.File;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Base64;
import android.util.Log;


public class AudioRecorder implements RecordStrategy {
    // 音频获取源
    private int audioSource = MediaRecorder.AudioSource.MIC;
    // 设置音频采样率，44100是目前的标准，但是某些设备仍然支持22050，16000，11025
    private static int sampleRateInHz = 8000;
    // 设置音频的录制的声道CHANNEL_IN_STEREO为双声道，CHANNEL_CONFIGURATION_MONO为单声道
    private static int channelConfig = AudioFormat.CHANNEL_CONFIGURATION_MONO;
    // 音频数据格式:PCM 16位每个样本。保证设备支持。PCM 8位每个样本。不一定能得到设备支持。
    private static int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
    // 缓冲区字节大小
    private int bufferSizeInBytes = 0;
    private AudioRecord audioRecord;
    private boolean isRecord = false;// 设置正在录制的状态

    private G711A mG711;
    private static final String TAG = "AudioRecorder";

    private RecordListener listener;

    private int pts = 0;

    public AudioRecorder(){};

    @Override
    public void ready() {
        mG711 = new G711A();
        creatAudioRecord();
    }

    private void creatAudioRecord() {
        // 获得缓冲区字节大小
        bufferSizeInBytes = AudioRecord.getMinBufferSize(sampleRateInHz,
                channelConfig, audioFormat);
        Log.d(TAG, "kim creatAudioRecord bufferSizeInBytes="+bufferSizeInBytes);
        // 创建AudioRecord对象
        audioRecord = new AudioRecord(audioSource, sampleRateInHz,
                channelConfig, audioFormat, bufferSizeInBytes);
    }

    @Override
    public void start() {
        Log.d(TAG, "kim startRecord");
        audioRecord.startRecording();
        // 让录制状态为true
        isRecord = true;

        pts = 0;
        if (listener != null) {
            listener.recordStart(pts);
        }

        // 开启音频线程
        new Thread(new AudioRecordThread()).start();
    }

    @Override
    public void stop() {
        if (audioRecord != null) {
            Log.d(TAG, "kim stopRecord");
            isRecord = false;//停止录音
            audioRecord.stop();
            audioRecord.release();//释放资源
            audioRecord = null;

            if (listener != null) {
                listener.recordEnd(pts);
            }
        }
    }

    class AudioRecordThread implements Runnable {
        @Override
        public void run() {
            sendDataToServer();
        }
    }

    private void sendDataToServer() {
        //20ms读一次，每次发送200ms数据
        byte[] audiodata8 = new byte[320];
        int readsize = 0;
        int index = 0;
        byte[] g711data = new byte[160];
        byte[] fullG711 = new byte[1600];//200ms

        int g711size = 0;
        String base64Str = null;

        while (isRecord) {
            index = 0;
            for(int i = 0; i < 10 && isRecord; i++) {
                readsize = audioRecord.read(audiodata8, 0, 320);
                if(readsize > 0) {
                    g711size = mG711.encode(audiodata8, 0, readsize, g711data);
                    System.arraycopy(g711data, 0, fullG711, index, g711size);
                    index += g711size;
                }
            }

            if(index > 0) {
                base64Str = Base64.encodeToString(fullG711, 0, index, Base64.NO_WRAP);
                if (listener != null) {
                    listener.recordSendData(base64Str, pts);
                }
                pts += 200;//200ms
            }
        }
    }

    public void setRecordListener(RecordListener listener) {
        this.listener = listener;
    }

    public interface RecordListener {
        public void recordStart(long presentationTimeStamp);
        public void recordSendData(String data, long presentationTimeStamp);
        public void recordEnd(long presentationTimeStamp);
    }
}