# EasyClient #

EasyClient是EasyDarwin开源流媒体团队提供的一套直观、功能丰富的客户端项目，目前支持Windows、Android版本，后续将支持ios版本，PC版本的EasyClient支持多种源接入，包括Windows摄像头、麦克风、RTSP摄像机、屏幕桌面等，采集后经过x264编码、EasyPusher推送到EasyDarwin流媒体服务器进行转发，同时EasyClient还支持通过EasyRTSPClient拉取EasyDarwin直播流，进行显示、播放，非常稳定、易用，用户可以基于EasyClient项目，开发类似于课堂直播、视频对讲等项目！

##功能描述##

###1、DShow采集与音视频编码###
EasyClient实现DShow ISampleGrabberCB接口，分别采集Windows摄像头的视/音频，通过回调来保存每一个buffer，在回调中，我们用x264进行视频数据编码成H.264，用FAAC进行音频编码成AAC.再通过[EasyPusher](https://github.com/EasyDarwin/EasyPusher "EasyPusher")将实时编码的音视频数据，推送到[EasyDarwin](https://github.com/EasyDarwin/EasyDarwin "EasyDarwin")开源流媒体服务器进行直播；
![EasyClient DShow](http://pic002.cnblogs.com/images/2011/254714/2011111012212148.jpg)

![EasyDarwin EasyClient](http://www.easydarwin.org/github/images/easyclient_dshow.png)

###2、IPCamera源播放与推送###
EasyClient支持获取IPCamera摄像机的RTSP直播流，通过[EasyRTSPClient](https://github.com/EasyDarwin/EasyRTSPClient "EasyRTSPClient")拉取到本地，进行解码、显示；

同时还支持将拉取到的IPC RTSP流，通过[EasyPusher](https://github.com/EasyDarwin/EasyPusher "EasyPusher")将实时编码的音视频数据，推送到[EasyDarwin](https://github.com/EasyDarwin/EasyDarwin "EasyDarwin")开源流媒体服务器进行直播；
![EasyDarwin EasyClient](http://www.easydarwin.org/github/images/easyclient_ipc.png)

##更新记录##

### next version (2015-12-10)

* 多窗口显示与播放
* MP4录像功能

### v1.1.1.5 (2015-12-3) ###

* 开放Dshow采集库，目的是为了方便大家调试以及修复里面现在已知的部分BUG，增加外部显示模式;
* 删减x264和faac重复的较为复杂的编码器库，让代码调理更清晰，更有可读性，为下个版本多窗口做准备;
* 增加本地采集YUY2和RGB24两种色彩格式的兼容支持;
* 如果大家对EasyClient测试出任何问题或者有什么好的建议，欢迎反馈开源团队，我们将在第一时间修复和采纳！


### v1.1.1.4 (2015-11-27) ###

* 增加本地视频采集宽、高、FPS、码率，推送缓冲区界面设置;
* 增加EasyLogo，修复界面窗口部分区域刷新不正常的问题;
* 界面布局进行部分调整;
* 修复推送EasyDarwin服务器,音频导致HLS直播不能正常播放的问题;


### v1.1.1.3 (2015-11-20) ###

* 修复界面窗口最小化还原时，文本框出现不刷新的问题;
* 增加DShow音视频采集库，修复本地音视频推送服务器出现播放不同步的问题;
* 增加EasyDarwin官网地址和EasyClient的Github访问地址;
* 界面布局进行部分调整.

### v1.0.1.2 (2015-11-17) ###

* 添加EasySkinUI轻量级皮肤库，优化界面;
* 皮肤库调用详见：界面管理类CEasySkinManager;

### v1.0.0.2 (2015-11-10) ###

* 修复采集Rtsp流格式判断错误导致采集失败的问题，现在流格式可任意；
* 增加版本信息和界面显示，窗口暂时固定；
* 测试：接收公共网络rtsp流进行推送EasyDarwin服务器，推送成功，但是可能出现直播无视频显示的问题；

### v1.0.0.1 (2015-11-9) ###

* EasyClient初始版本By SwordTwevle


## 获取更多信息 ##

邮件：[support@easydarwin.org](mailto:support@easydarwin.org) 

WEB：[www.EasyDarwin.org](http://www.easydarwin.org)

Author：SwordTwevle@EasyDarwin.org

Copyright &copy; EasyDarwin.org 2012-2015

![EasyDarwin](http://www.easydarwin.org/skin/easydarwin/images/wx_qrcode.jpg)
