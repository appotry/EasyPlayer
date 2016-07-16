package org.easydarwin.easyclient.audio;

/**
 * Created by Kim on 2016/7/9.
 */
/**
 * RecordStrategy 录音策略接口
 * @author acer
 */
public interface RecordStrategy {

    /**
     * 在这里进行录音准备工作，重置录音文件名等
     */
    public void ready();
    /**
     * 开始录音
     */
    public void start();
    /**
     * 录音结束
     */
    public void stop();
}