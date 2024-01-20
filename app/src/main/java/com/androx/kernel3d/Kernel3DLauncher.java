package com.androx.kernel3d;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

public class Kernel3DLauncher extends Activity {

    static {
       System.loadLibrary("kernel3d");
    }

    public native int createNativeWindow(Surface surface);
    public native int start();
    public native int resume();
    public native int pause();
    public native int stop();

    public native int destroyNativeWindow();

    @Override
    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);

        SurfaceView surfaceView = new SurfaceView(this);

        surfaceView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN);

        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                Surface surface = holder.getSurface();

                createNativeWindow(surface);
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                //TODO
            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
                //TODO
                destroyNativeWindow();
            }
        });

        setContentView(surfaceView);
    }

    @Override
    protected void onStart() {
        super.onStart();

        start();
        //TODO LOAD
    }

    @Override
    protected void onResume(){
        super.onResume();

        resume();
        //TODO Continue Update
    }

    @Override
    protected void onPause(){
        super.onPause();

        pause();
        //TODO Pause Update
    }

    @Override
    protected void onStop(){
        super.onStop();

        stop();
        //TODO UNLOAD
    }


}
