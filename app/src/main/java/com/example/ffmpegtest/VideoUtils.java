package com.example.ffmpegtest;

public class VideoUtils {
    public native static void decode(String input, String output);

    //1.加载动态库
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avutil-54");
        System.loadLibrary("swresample-1");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avformat-56");
        System.loadLibrary("swscale-3");
        System.loadLibrary("postproc-53");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avdevice-56");
    }
}
