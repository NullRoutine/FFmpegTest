package com.example.ffmpegtest;
/**
 * native视频播放
 */

import android.os.Bundle;
import android.os.Environment;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.example.ffmpegtest.view.VideoView;

import java.io.File;

public class VideoPlayerActivity extends AppCompatActivity {
    NullRoutinePlayer player;
    VideoView videoView;
    final String path_in = new File(Environment.getExternalStorageDirectory(), "test.mp4").getAbsolutePath();

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_videoplayer);
        player = new NullRoutinePlayer();
        videoView = findViewById(R.id.video_view);
        findViewById(R.id.btn_start).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        player.init(path_in, videoView.getHolder().getSurface());
                    }
                }).start();
            }
        });
    }
}
