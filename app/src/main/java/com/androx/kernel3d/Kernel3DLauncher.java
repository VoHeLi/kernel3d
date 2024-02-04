package com.androx.kernel3d;

import android.app.Activity;
import android.app.ActivityOptions;
import android.app.Application;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.graphics.SurfaceTexture;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.hardware.input.InputManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.SystemClock;
import android.util.Log;
import android.view.Display;
import android.view.InputEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import android.app.ActivityManager;

import java.io.IOException;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.security.Provider;
import java.util.Objects;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresPermission;
import androidx.core.util.Consumer;

public class Kernel3DLauncher extends Activity {

    static {
       System.loadLibrary("kernel3d");
    }

    public Kernel3DLauncher() throws ClassNotFoundException, InvocationTargetException, IllegalAccessException, NoSuchMethodException {
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
                //destroyNativeWindow();
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
        //super.onPause();

        //pause();
        //TODO Pause Update
    }

    @Override
    protected void onStop(){
        //super.onStop();
        //stop();
        //TODO UNLOAD
    }

    private DisplayManager mDisplayManager;
    private VirtualDisplay mVirtualDisplay;
    private Display mPresentationDisplay;
    private Handler mHandler;
    private HandlerThread mHandlerThread;
    @RequiresPermission("android.permission.CAPTURE_SECURE_VIDEO_OUTPUT")
    private SurfaceTexture createVirtualDisplay(int textureId, int appId) {
        int width = 480;
        int height = 854;

        if(appId == 1){
            width = 1280;
            height = 720;
        }

        mDisplayManager = (DisplayManager) getSystemService(Context.DISPLAY_SERVICE);
        mHandlerThread = new HandlerThread("VirtualDisplayThread");
        mHandlerThread.start();
        mHandler = new Handler(mHandlerThread.getLooper());

        /*getHolder().addCallback(this);
        setFocusable(true);*/

        SurfaceTexture surfaceTexture = new SurfaceTexture(textureId);
        surfaceTexture.setDefaultBufferSize(width, height);

        Surface surface = new Surface(surfaceTexture);

        Log.d("Androx Kernel3D", "Java method test3!");
        mVirtualDisplay = mDisplayManager.createVirtualDisplay(
                "VirtualDisplayTest",
                width, height, getResources().getDisplayMetrics().densityDpi,
                surface,
                DisplayManager.VIRTUAL_DISPLAY_FLAG_PUBLIC |
                        DisplayManager.VIRTUAL_DISPLAY_FLAG_SECURE |
                        DisplayManager.VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY |
                        DisplayManager.VIRTUAL_DISPLAY_FLAG_PRESENTATION,
                null,
                mHandler
        );

        //mVirtualDisplay.getDisplay().trusted; todo use reflections

        mPresentationDisplay = mVirtualDisplay.getDisplay();

        Log.d("Androx Kernel3D", "Java method test4!");

        return surfaceTexture;
    }

    SurfaceTexture surfaceTexture1;
    SurfaceTexture surfaceTexture2;
    SurfaceTexture surfaceTexture3;
    public void intentTest(int displayTextureId, int appId) throws InterruptedException {
        Log.d("Androx Kernel3D", "Java method called! " + displayTextureId);

        String packageName2 = "com.studio501.canvasrun";
        String packageName3 = "state.balls.io";
        String packageName1 = "com.robtopx.geometryjump";
        String packageName = "org.dolphinemu.dolphinemu";

        if(appId != 1) return;

        SurfaceTexture surfaceTexture = createVirtualDisplay(displayTextureId, appId);

        switch(appId){
            case 1:
                surfaceTexture1 = surfaceTexture;
                packageName = packageName1;
                break;
            case 2:
                surfaceTexture2 = surfaceTexture;
                packageName = packageName2;
                break;
            case 3:
                surfaceTexture3 = surfaceTexture;
                packageName = packageName3;
                break;
        }

        Log.d("Androx Kernel3D", "Java method test!");

        PackageManager packageManager = getPackageManager();
        Intent intent = packageManager.getLaunchIntentForPackage(packageName);

        Log.d("Androx Kernel3D", "Java method A!");
        if (intent != null) {
            // Si une intention est trouvée, lancez l'application
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.setFlags(Intent.FLAG_ACTIVITY_MULTIPLE_TASK);



            Log.d("Androx Kernel3D", "Java method B!");


            Log.d("Androx Kernel3D", "Java method C!");
            int id = mPresentationDisplay.getDisplayId();
            //int id = 1;
            Log.d("Androx Kernel3D", "Java method D! id : " + id);
            ActivityOptions options = ActivityOptions.makeBasic().setLaunchDisplayId(id);
            options.setLaunchDisplayId(id);
            //intent.putExtra("android.intent.extra.DISPLAY_ID", id);


            Log.d("Androx Kernel3D", "launching App!");
            startActivity(intent, options.toBundle());

            Log.d("Androx Kernel3D", "after launching App!");
            Thread.sleep(2000);
            Log.d("Androx Kernel3D", "after sleep");
            surfaceTexture.updateTexImage();
            Log.d("Androx Kernel3D", "after updating texture!");

        } else {
            // Si l'intention est nulle, l'application n'est pas installée
            Log.e("Androx Kernel3D", "app not found!");
            // Vous pouvez ajouter ici un code pour gérer cette situation
        }

    }

    long downTime;
    int lastAction = Integer.MIN_VALUE;

    InputManager im = ((InputManager) Class.forName("android.hardware.input.InputManager").getDeclaredMethod("getInstance", new Class[0]).invoke((Object) null, new Object[0]));

    public void updateDisplayTexture(boolean pinching){

        //Touch the screen in the middle
        MotionEvent.PointerCoords[] pointerCoordsArr = new MotionEvent.PointerCoords[1];
        pointerCoordsArr[0]  = new MotionEvent.PointerCoords();
        pointerCoordsArr[0].x = 1280/2;
        pointerCoordsArr[0].y = 300;
        pointerCoordsArr[0].pressure = 1.0f;
        pointerCoordsArr[0].size = 1.0f;
        int currentAction = getCurrentAction(pinching);

        MotionEvent.PointerProperties[] arr1 = new MotionEvent.PointerProperties[1];
        MotionEvent.PointerProperties pointerProperties = new MotionEvent.PointerProperties();
        pointerProperties.toolType = 1;
        pointerProperties.id = MotionEvent.ACTION_BUTTON_RELEASE; //MotionEvent.ACTION_BUTTON_RELEASE
        arr1[0] = pointerProperties;

        MotionEvent obtain = MotionEvent.obtain(this.downTime, SystemClock.uptimeMillis(), currentAction, 1, arr1, pointerCoordsArr, 0, 0, 1.0f, 1.0f, 0, 0, 4098, 0);

        try{
            obtain.getClass().getDeclaredMethod("setLynxDisplayId", new Class[]{Integer.TYPE}).invoke(obtain, new Object[]{Integer.valueOf(mVirtualDisplay.getDisplay().getDisplayId())});
            ((Boolean) this.im.getClass().getDeclaredMethod("injectInputEvent", new Class[]{InputEvent.class, Integer.TYPE}).invoke(this.im, new Object[]{obtain, 1})).booleanValue();

        } catch(Exception e){
            e.printStackTrace();
        }



        this.lastAction = currentAction;
        //Update surfaces
        surfaceTexture1.updateTexImage();
        /*surfaceTexture2.updateTexImage();
        surfaceTexture3.updateTexImage();*/
        Log.d("Androx Kernel3D", "after updating tex!");
    }

    private int getCurrentAction(boolean z) {
        if (!z && this.lastAction == 0) {
            return 2;
        } else if (z || this.lastAction == 0) {
            return 1;
        } else {
            this.downTime = SystemClock.uptimeMillis();
            return 0;
        }
    }

    /*public void reflectPermissions() throws NoSuchMethodException, NoSuchFieldException, IllegalAccessException, IOException {
        //Launch code
        int displayId = 1;
        String packageName = "";

        ActivityOptions activityOptions = ActivityOptions.makeBasic();
        activityOptions.setLaunchDisplayId(displayId);

        //Un log
        Intent intent = getPackageManager().getLaunchIntentForPackage(packageName);
        startActivity(intent, activityOptions.toBundle());
    }*/


    /*public String launch2DAppLynx(Context context, Activity activity, Consumer consumer){
        Bundle bundle = activity.getIntent().getExtras();

        String returnValue = null;
        String tag = "LynxVirtualDisplayTag";

        if(bundle != null){
            Log.i(tag, " Intent bundle is : " + bundle);

            String packageNameVirtualDisplayKey = "packageNameVirtualDisplayKey";
            String value = bundle.getString(packageNameVirtualDisplayKey);

            if(value == null){
                Log.w(tag, "bundle.getString(...) return null, impossible to launch the targeted application in virtual display");
                return returnValue;
            }

            int length = value.length();

            if(length == 0){
                Log.w(tag, "bundle.getString(...) return null, impossible to launch the targeted application in virtual display");
                return returnValue;
            }

            PackageManager pm = context.getPackageManager();
            int flag = 0x80; //TODO REPLACE with enum


            ApplicationInfo appInfo;
            String appLabel;
            try{
                appInfo = pm.getApplicationInfo(value, flag);
                appLabel = (String) pm.getApplicationLabel(appInfo);
            } catch (Exception e){
                Log.e(tag, "Error in start2DApplication, impossible to get app name from package name " + e);
            }

            Log.i(tag, "    Package Name to Launch in Virtual display is " + value);

            try{
                VirtualDisplayController controller = sController;
                Objects.requireNonNull(consumer);

                lambda.init()
            } catch (ClassNotFoundException e){
                Log.e(tag, "Error in start2DApplication, impossible to get app name from package name " + e);
            } catch (NoSuchMethodException e){
                Log.e(tag, "Error in start2DApplication, impossible to get app name from package name " + e);
            } catch (InvocationTargetException e){
                Log.e(tag, "Error in start2DApplication, impossible to get app name from package name " + e);
            } catch (IllegalAccessException e){
                Log.e(tag, "Error in start2DApplication, impossible to get app name from package name " + e);
            }

        }

        Log.w(tag, "extras Bundle in Intent is NULL : Impossible to launch virtual display");
        return returnValue;
    }

    static VirtualDisplayController sController;*/
}

class VirtualDisplayController implements SurfaceHolder.Callback{

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {

    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {

    }
}
