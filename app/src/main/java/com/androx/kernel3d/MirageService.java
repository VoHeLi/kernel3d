package com.androx.kernel3d;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.Build;
import android.os.IBinder;
import android.util.Log;

import androidx.core.app.NotificationCompat;

import java.io.File;
import java.io.FileWriter;

public class MirageService extends Service {

    static {
        System.loadLibrary("mirage");
    }

    private static native void mirageInit();
    private static native void mirageCreateAppListener();

    // Declare a native method sayHello() that receives no arguments and returns void
    //private native void loadRuntimeJNI();

    private static final String CHANNEL_ID = "MirageCanal";

    @Override
    public void onCreate(){
        Log.d("PICOREUR", "ON START!");


        afficherNotification();
        loadRuntimeJNI();

        writeLoaderToApp();

    }

    private final IBinder binder = new MirageBinder();

    public class MirageBinder extends Binder {
        MirageService getService() {
            return MirageService.this;
        }
    }

    private void loadRuntimeJNI() {
        mirageInit();
    }

//    @Override
//    public int onStartCommand(Intent intent, int flags, int startId) {
//        Log.d("MIRAGE", "ON START!");
//
//
//        afficherNotification();
//        //loadRuntimeJNI();
//
//        writeLoaderToApp();
//
//        return START_STICKY;
//    }

    public void writeLoaderToApp(){
        Log.d("MIRAGE", "Before writing...");
        String libDir = getApplicationInfo().nativeLibraryDir;
        String runtime_path = libDir + "/libruntime.so";

        String jsonData = "{\n\t\"file_format_version\": \"1.0.0\",\n\t\"runtime\": {\n\t\t\"name\": \"picoreur_runtime\",\n\t\t\"library_path\": \"" + runtime_path;
        jsonData += "\"\n\t}\n}";

        Log.d("MIRAGE", "Writing : \n" + jsonData);

        writeFileOnInternalStorage(getApplicationContext(), "picoreur_runtime.json", jsonData);
    }

    public void writeFileOnInternalStorage(Context mcoContext, String sFileName, String sBody){
        try {
            File gpxfile = new File("/storage/emulated/0", sFileName);
            FileWriter writer = new FileWriter(gpxfile);
            writer.append(sBody);
            writer.flush();
            writer.close();

            Log.d("MIRAGE", "Successfully wrote manifest file to : " + gpxfile.getAbsolutePath());
        } catch (Exception e){
            Log.e("MIRAGE", "Error when writing runtime manifest :" + e.toString());
        }
    }

    public void createAppListener(){
        mirageCreateAppListener();

        //On met dans un fichier, mais c'est une méthode temporaire pour l'instant
        Log.d("MIRAGE", "App listener created...");
//        String libDir = getApplicationInfo().nativeLibraryDir;
//        String runtime_path = libDir + "/libruntime.so";
//
//        String jsonData = Integer.toString(fd); //not safe
//
//        Log.d("MIRAGE", "Writing fd : \n" + jsonData);
//
//        writeFileOnInternalStorage(getApplicationContext(), "testAppFD.data", jsonData); //DEBUG NAME
    }

    private void afficherNotification() {
        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, "MirageCanal", NotificationManager.IMPORTANCE_DEFAULT);
            notificationManager.createNotificationChannel(channel);
        }

        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setSmallIcon(R.drawable.ic_launcher_foreground)
                .setContentTitle("Mirage Runtime Service")
                .setContentText("Mirage OpenXR Runtime Service for Standard Loader.")
                .setPriority(NotificationCompat.PRIORITY_DEFAULT);

        Notification notification = builder.build();
        startForeground(/*ID unique de la notification*/1, notification);
    }

    public String getNativeDir() {
        // Implementation
        return getApplicationContext().getApplicationInfo().nativeLibraryDir;
    }

    public Context getContext(){
        return getApplicationContext();
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.d("MIRAGE", "Binding...");
        return binder;
    }


}
