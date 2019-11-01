package com.example.ffmpegtest;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    Button btnDecode;
    Button btnMusic;
    NullRoutinePlayer player;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        player = new NullRoutinePlayer();
        // Example of a call to a native method
        btnDecode = findViewById(R.id.btn_decode);
        btnMusic = findViewById(R.id.btn_music);
        btnDecode.setOnClickListener(this);
        btnMusic.setOnClickListener(this);
    }

    //    final String path_in = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separatorChar + "test.mp4";
    final String path_in = new File(Environment.getExternalStorageDirectory(), "test.mp4").getAbsolutePath();
    final String path_out = new File(Environment.getExternalStorageDirectory(), "outTest.yuv").getAbsolutePath();

    //    final String path_out = Environment.getExternalStorageDirectory().getAbsolutePath() + File.separatorChar + "out.yuv";
    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_decode:
//                new Thread(new Runnable() {
//                    @Override
//                    public void run() {
//                        VideoUtils.decode(path_in, path_out);
//                    }
//                }).start();
                Intent intent = new Intent(MainActivity.this, VideoPlayerActivity.class);
                startActivity(intent);
                break;
            case R.id.btn_music:
                String input = new File(Environment.getExternalStorageDirectory(), "test.mp3").getAbsolutePath();
                String output = new File(Environment.getExternalStorageDirectory(), "test.pcm").getAbsolutePath();
                player.sound(input, output);
                break;

        }
    }
}
