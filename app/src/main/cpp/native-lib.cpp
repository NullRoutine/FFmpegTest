#include <jni.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "com_example_ffmpegtest_VideoUtils.h"
#include "com_example_ffmpegtest_NullRoutinePlayer.h"
#include "android/log.h"
#include <unistd.h>

#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"liaowj",FORMAT,##__VA_ARGS__);
#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"liaowj",FORMAT,##__VA_ARGS__);
#define MAX_AUDIO_FRME_SIZE 48000 * 4

extern "C" {
//封装格式
#include <libavformat/avformat.h>
//解码
#include <libavcodec/avcodec.h>
//缩放
#include <libswscale/swscale.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "libyuv.h"
#include "libyuv/convert_argb.h"
#include <libswresample/swresample.h>
}
/**
 * 视频解码
 */
extern "C" JNIEXPORT void JNICALL Java_com_example_ffmpegtest_VideoUtils_decode
        (JNIEnv *env, jclass jcls, jstring input_jstr, jstring output_jstr) {
    const char *input_cstr = env->GetStringUTFChars(input_jstr, NULL);
    const char *output_cstr = env->GetStringUTFChars(output_jstr, NULL);
    //1.注册组件
    av_register_all();
    //2.封装格式上下文
    AVFormatContext *avFormatContext = avformat_alloc_context();
    //打开输入视频文件
    if (avformat_open_input(&avFormatContext, input_cstr, NULL, NULL) < 0) {
        LOGE("%s", "打开输入视频文件失败");
        return;
    }
    //3.获取视频信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("%s", "获取视频信息失败");
        return;;
    }
    //视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
    int i = 0;
    int video_stream_idx = -1;
    for (; i < avFormatContext->nb_streams; i++) {
        //根据类型判断是不是视频流
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }
    //4.获取视频解码器
    AVCodecContext *avCodecContext = avFormatContext->streams[video_stream_idx]->codec;
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (avCodec == NULL) {
        LOGE("%s", "无法解码");
        return;
    }
    //5.打开解码器
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGE("%s", "解码器无法打开");
        return;
    }
    //编码数据
    AVPacket *packet = static_cast<AVPacket *>(av_malloc(sizeof(AVPacket)));
//    AVPacket *packet = NULL;
//    av_init_packet(packet);
    //像素数据(解码数据)
    AVFrame *pFrame = av_frame_alloc();
    AVFrame *yuvFrame = av_frame_alloc();

    //只有指定了AVFrame的像素格式、画面大小才能真正分配内存
    //缓冲区分配内存
    uint8_t *out_buffer = (uint8_t *) av_malloc(
            avpicture_get_size(AV_PIX_FMT_YUV420P, avCodecContext->width, avCodecContext->height));
    //初始化缓冲区
    avpicture_fill((AVPicture *) yuvFrame, out_buffer, AV_PIX_FMT_YUV420P, avCodecContext->width,
                   avCodecContext->height);

    // 6.一帧一帧读取压缩的视频数据AVPacket
    FILE *fp_yuv = fopen(output_cstr, "wbe");
    //用于像素格式转换或者缩放
    struct SwsContext *sws_ctx = sws_getContext(avCodecContext->width, avCodecContext->height,
                                                avCodecContext->pix_fmt,
                                                avCodecContext->width, avCodecContext->height,
                                                AV_PIX_FMT_YUV420P,
                                                SWS_BILINEAR, NULL, NULL, NULL);

    int len, got_frame, framecount = 0;
    while (av_read_frame(avFormatContext, packet) >= 0) {
        //解码AVPacket->AVFrame
        len = avcodec_decode_video2(avCodecContext, pFrame, &got_frame, packet);
        //非零，正在解码
        if (got_frame) {
            //转为指定的YUV420P像素帧
            sws_scale(sws_ctx, pFrame->data, pFrame->linesize, 0, pFrame->height,
                      yuvFrame->data, yuvFrame->linesize);
            int y_size = avCodecContext->width * avCodecContext->height;
            fwrite(pFrame->data[0], 1, y_size, fp_yuv);
            fwrite(pFrame->data[1], 1, y_size / 4, fp_yuv);
            fwrite(pFrame->data[2], 1, y_size / 4, fp_yuv);
            LOGI("解码%d帧", framecount++);
        }
        av_free_packet(packet);
    }
    fclose(fp_yuv);
    av_frame_free(&pFrame);
    avcodec_close(avCodecContext);
    avformat_free_context(avFormatContext);
    env->ReleaseStringUTFChars(input_jstr, input_cstr);
    env->ReleaseStringUTFChars(output_jstr, output_cstr);
}


extern "C" JNIEXPORT void JNICALL Java_com_example_ffmpegtest_NullRoutinePlayer_init
        (JNIEnv *env, jobject jObj, jstring input_jstr, jobject surface) {
    const char *input_cstr = env->GetStringUTFChars(input_jstr, NULL);
    //1.注册组件
    av_register_all();
    //2.封装格式上下文
    AVFormatContext *avFormatContext = avformat_alloc_context();
    //打开输入视频文件
    if (avformat_open_input(&avFormatContext, input_cstr, NULL, NULL) < 0) {
        LOGE("%s", "打开输入视频文件失败");
        return;
    }
    //3.获取视频信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("%s", "获取视频信息失败");
        return;;
    }
    //视频解码，需要找到视频对应的AVStream所在pFormatCtx->streams的索引位置
    int i = 0;
    int video_stream_idx = -1;
    for (; i < avFormatContext->nb_streams; i++) {
        //根据类型判断是不是视频流
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }
    //4.获取视频解码器
    AVCodecContext *avCodecContext = avFormatContext->streams[video_stream_idx]->codec;
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (avCodec == NULL) {
        LOGE("%s", "无法解码");
        return;
    }
    //5.打开解码器
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGE("%s", "解码器无法打开");
        return;
    }
    //编码数据
    AVPacket *packet = static_cast<AVPacket *>(av_malloc(sizeof(AVPacket)));
    //像素数据(解码数据)
    AVFrame *yuv_frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();
    //native绘制
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    //绘制时的缓冲区
    ANativeWindow_Buffer outBuffer;
    int len, got_frame, framecount = 0;
    while (av_read_frame(avFormatContext, packet) >= 0) {
        //解码AVPacket->AVFrame
        len = avcodec_decode_video2(avCodecContext, yuv_frame, &got_frame, packet);
        //非零，正在解码
        if (got_frame) {
            //lock
            //设置缓冲区的属性（宽、高、像素格式）
            ANativeWindow_setBuffersGeometry(nativeWindow, avCodecContext->width,
                                             avCodecContext->height, WINDOW_FORMAT_RGBA_8888);
            ANativeWindow_lock(nativeWindow, &outBuffer, NULL);
            //设置rgb_frame的属性（像素格式、宽高）和缓冲区
            //rgb_frame缓冲区与outBuffer.bits是同一块内存
            avpicture_fill((AVPicture *) rgb_frame, static_cast<const uint8_t *>(outBuffer.bits),
                           PIX_FMT_RGBA, avCodecContext->width, avCodecContext->height);
            //YUV->RGBA_8888
            libyuv::I420ToARGB(yuv_frame->data[0], yuv_frame->linesize[0],
                               yuv_frame->data[2], yuv_frame->linesize[2],
                               yuv_frame->data[1], yuv_frame->linesize[1],
                               rgb_frame->data[0], rgb_frame->linesize[0],
                               avCodecContext->width, avCodecContext->height);

            //unlock
            ANativeWindow_unlockAndPost(nativeWindow);
//            usleep(1000 * 16);
            LOGI("解码%d帧", framecount++);
        }
        av_free_packet(packet);
    }
    ANativeWindow_release(nativeWindow);
    av_frame_free(&yuv_frame);
    avcodec_close(avCodecContext);
    avformat_free_context(avFormatContext);
    env->ReleaseStringUTFChars(input_jstr, input_cstr);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_ffmpegtest_NullRoutinePlayer_sound
        (JNIEnv *env, jobject jObj, jstring input_jstr, jstring output_jstr) {
    const char *input_cstr = env->GetStringUTFChars(input_jstr, NULL);
    const char *output_cstr = env->GetStringUTFChars(output_jstr, NULL);
    //1.注册组件
    av_register_all();
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    //打开输入视频文件
    if (avformat_open_input(&pFormatCtx, input_cstr, NULL, NULL) < 0) {
        LOGE("%s", "打开音频文件失败");
        return;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("%s", "无法获取输入文件信息");
        return;
    }
    int i = 0, audio_stream_idx = -1;
    for (; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_idx = i;
            break;
        }
    }
    //获取解码器
    AVCodecContext *codecCtx = pFormatCtx->streams[audio_stream_idx]->codec;
    AVCodec *codec = avcodec_find_decoder(codecCtx->codec_id);
    if (codec == NULL) {
        LOGE("%s", "无法获取解码器");
        return;
    }
    int got_frame = 0, index = 0, ret;
    if (avcodec_open2(codecCtx, codec, NULL) < 0) {
        LOGE("%s", "无法打开解码器");
        return;
    }
    AVPacket *packet = static_cast<AVPacket *>(av_malloc(sizeof(AVPacket)));
    AVFrame *frame = av_frame_alloc();
    SwrContext *swrCtx = swr_alloc();
    enum AVSampleFormat in_sample_fmt = static_cast<AVSampleFormat>(codecCtx->sample_fmt);
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    int in_sample_rate = codecCtx->sample_rate;
    //输出采样率
    int out_sample_rate = in_sample_rate;
    //获取输入的声道布局
    //根据声道个数获取默认的声道布局（默认立体声）
    uint64_t in_ch_layout = codecCtx->channel_layout;
    //输出的声道布局
    int64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    swr_alloc_set_opts(swrCtx,
                       out_ch_layout, out_sample_fmt, out_sample_rate,
                       in_ch_layout, in_sample_fmt, in_sample_rate,
                       0, NULL);
    swr_init(swrCtx);
    int out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
    //开始调用java函数
    jclass playerClass = env->GetObjectClass(jObj);
    jmethodID createAudioTrackMid = env->GetMethodID(playerClass, "createAudioTrack",
                                                     "(II)Landroid/media/AudioTrack;");
    jobject audioTrack = env->CallObjectMethod(jObj, createAudioTrackMid, out_sample_rate,
                                               out_channel_nb);
    //调用play方法
    jclass audioTrackClass = env->GetObjectClass(audioTrack);
    jmethodID audioTrack_play_mid = env->GetMethodID(audioTrackClass, "play", "()V");
    env->CallVoidMethod(audioTrack, audioTrack_play_mid);

    //AudioTrack.write
    jmethodID audio_track_write_mid = env->GetMethodID(audioTrackClass, "write", "([BII)I");
    uint8_t *out_buffer = static_cast<uint8_t *>(av_malloc(MAX_AUDIO_FRME_SIZE));
    FILE *fp_pcm = fopen(output_cstr, "wb");
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == audio_stream_idx) {
            //解码
            ret = avcodec_decode_audio4(codecCtx, frame, &got_frame, packet);
            if (ret < 0) {
                LOGI("%s", "解码完成");
            }
            if (got_frame > 0) {
                LOGI("解码：%d", index++);
                swr_convert(swrCtx, &out_buffer, MAX_AUDIO_FRME_SIZE,
                            frame->data, frame->nb_samples);
                int out_buffer_size = av_samples_get_buffer_size(NULL, out_channel_nb,
                                                                 frame->nb_samples, out_sample_fmt,
                                                                 1);
                //fwrite(out_buffer, 1, out_buffer_size, fp_pcm);
                jbyteArray audio_sample_array = env->NewByteArray(out_buffer_size);
                jbyte *sample_bytep = env->GetByteArrayElements(audio_sample_array, NULL);
                memcpy(sample_bytep, out_buffer, out_buffer_size);
                env->ReleaseByteArrayElements(audio_sample_array, sample_bytep, 0);
                env->CallIntMethod(audioTrack, audio_track_write_mid, audio_sample_array, 0,
                                   out_buffer_size);
                //释放局部引用
                env->DeleteLocalRef(audio_sample_array);
                usleep(16 * 1000);
            }
        }
        av_free_packet(packet);
    }
    fclose(fp_pcm);
    av_frame_free(&frame);
    av_free(out_buffer);

    swr_free(&swrCtx);
    avcodec_close(codecCtx);
    avformat_close_input(&pFormatCtx);
    env->ReleaseStringUTFChars(input_jstr, input_cstr);
    env->ReleaseStringUTFChars(output_jstr, output_cstr);
}