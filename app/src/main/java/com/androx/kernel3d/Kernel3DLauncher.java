package com.androx.kernel3d;

import android.app.Activity;
import android.app.ActivityOptions;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.graphics.SurfaceTexture;
import android.hardware.display.DisplayManager;
import android.hardware.display.VirtualDisplay;
import android.hardware.input.InputManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.SystemClock;
import android.util.Log;
import android.view.Display;
import android.view.InputEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import java.lang.reflect.InvocationTargetException;

import androidx.annotation.RequiresPermission;

public class Kernel3DLauncher extends Activity{

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

    private static native void mirageInit();
    private static native void mirageCreateAppListener();

    private boolean isBound = false;
    private MirageService mirageService;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
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

    private MirageService mBinder;

    @Override
    protected void onStart() {
        super.onStart();

        //STARTING RUNTIME SERVICE : TODO PLACE THIS SOMEWHERE ELSE
        Intent intent = new Intent(this, MirageService.class);
        bindService(intent, connection, Context.BIND_AUTO_CREATE);

        mirageInit();

        start();
        //TODO LOAD
    }

    @Override
    protected void onResume() {
        super.onResume();

        resume();
        //TODO Continue Update
    }

    @Override
    protected void onPause() {
        super.onPause();

        //pause();
        //TODO Pause Update
    }

    @Override
    protected void onStop() {
        super.onStop();
        //stop();
        //TODO UNLOAD
        Log.d("Androx Alert", "The app was asked to be stopped!");
    }

    private DisplayManager mDisplayManager;
    private VirtualDisplay mVirtualDisplay;
    private Display mPresentationDisplay;
    private Handler mHandler;
    private HandlerThread mHandlerThread;

    private int displayWidth = 1920;
    private int displayHeight = 1080;

    @RequiresPermission("android.permission.CAPTURE_SECURE_VIDEO_OUTPUT")
    private SurfaceTexture createVirtualDisplay(int textureId, int appId) {
        int width = 480;
        int height = 854;

        if (appId == 1) {
//            width = 1280;
//            height = 720;
            width = displayWidth;
            height = displayHeight;
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
        //String packageName1 = "com.robtopx.geometryjump";
        String packageName1 = "com.vrchat.android";
        //String packageName1 = "cm.aptoide.pt";
        String packageName = "org.dolphinemu.dolphinemu";
        //String packageName1 = "com.miHoYo.GenshinImpact";

        if (appId != 1) return;

        //SurfaceTexture surfaceTexture = createVirtualDisplay(displayTextureId, appId);

        packageName = packageName1; //DEBUG

        /*switch (appId) {
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
        }*/

        Log.d("Androx Kernel3D", "Java method test!");

        PackageManager packageManager = getPackageManager();
        Intent intent = packageManager.getLaunchIntentForPackage(packageName);

        Log.d("Androx Kernel3D", "Java method A!");
        if (intent != null) {
            // Si une intention est trouvée, lancez l'application
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK | Intent.FLAG_ACTIVITY_NEW_TASK);


            Log.d("Androx Kernel3D", "Java method B!");


            Log.d("Androx Kernel3D", "Java method C!");
            //int id = mPresentationDisplay.getDisplayId();
            //int id = 1;
            //Log.d("Androx Kernel3D", "Java method D! id : " + id);
            ActivityOptions options = ActivityOptions.makeBasic();//.setLaunchDisplayId(id);
            //options.setLaunchDisplayId(id);
            //intent.putExtra("android.intent.extra.DISPLAY_ID", id);

            //Thread.sleep(10000);
            intent.putExtra("AndroxUnstoppable", true);
            getIntent().putExtra("AndroxUnstoppable", true);

            mirageCreateAppListener();
            //getIntent().putExtra("MirageAppListenerFD", fd);

            Log.d("Androx Kernel3D", "launching App!");
            startActivity(intent, options.toBundle());


            Log.d("Androx Kernel3D", "after launching App!");
            //Thread.sleep(1000);
            Log.d("Androx Kernel3D", "after sleep");
            //surfaceTexture.updateTexImage();

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

    //TODO DISSOCIATE INPUT FROM DISPLAY
    public void updateDisplayTexture(boolean pinching, float cx, float cy) {

        //DEBUG
        return;

        //Log.d("Androx Kernel3D", "CX : " + cx +" CY : " + cy);

        //Touch the screen in the middle
//        MotionEvent.PointerCoords[] pointerCoordsArr = new MotionEvent.PointerCoords[1];
//        pointerCoordsArr[0] = new MotionEvent.PointerCoords();
//        pointerCoordsArr[0].x = displayHeight * cx + displayWidth/2;
//        pointerCoordsArr[0].y = displayHeight - (displayHeight * cy + displayHeight/2);
//        pointerCoordsArr[0].pressure = 1.0f;
//        pointerCoordsArr[0].size = 1.0f;
//        int currentAction = getCurrentAction(pinching);
//
//        MotionEvent.PointerProperties[] arr1 = new MotionEvent.PointerProperties[1];
//        MotionEvent.PointerProperties pointerProperties = new MotionEvent.PointerProperties();
//        pointerProperties.toolType = 1;
//        pointerProperties.id = MotionEvent.ACTION_BUTTON_RELEASE; //MotionEvent.ACTION_BUTTON_RELEASE
//        arr1[0] = pointerProperties;
//
//        MotionEvent obtain = MotionEvent.obtain(this.downTime, SystemClock.uptimeMillis(), currentAction, 1, arr1, pointerCoordsArr, 0, 0, 1.0f, 1.0f, 0, 0, 4098, 0);
//
//        try {
//            obtain.getClass().getDeclaredMethod("setLynxDisplayId", new Class[]{Integer.TYPE}).invoke(obtain, new Object[]{Integer.valueOf(mVirtualDisplay.getDisplay().getDisplayId())});
//            ((Boolean) this.im.getClass().getDeclaredMethod("injectInputEvent", new Class[]{InputEvent.class, Integer.TYPE}).invoke(this.im, new Object[]{obtain, 1})).booleanValue();
//
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
//
//
//        this.lastAction = currentAction;
        //Update surfaces
        //surfaceTexture1.updateTexImage();
        /*surfaceTexture2.updateTexImage();
        surfaceTexture3.updateTexImage();*/
        //Log.d("Androx Kernel3D", "after updating tex!");
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

    private final ServiceConnection connection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName className, IBinder ibinder) {
            Log.d("Androx Kernel3D", "Alert service connected!");
            MirageService.MirageBinder binder = (MirageService.MirageBinder) ibinder;
            mirageService = binder.getService();
            isBound = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            isBound = false;
        }
    };

}