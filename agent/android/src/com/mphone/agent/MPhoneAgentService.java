package com.mphone.agent;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.PowerManager;
import android.util.Log;

public final class MPhoneAgentService extends Service {
    private static final String TAG = "MPhoneAgent";
    private static final String CHANNEL_ID = "mphone_agent";
    private static final int NOTIFICATION_ID = 8628;
    private static final long HEARTBEAT_MS = 30_000L;

    private final Handler handler = new Handler(Looper.getMainLooper());
    private PowerManager.WakeLock wakeLock;
    private PowerManager.WakeLock screenWakeLock;
    private BroadcastReceiver screenReceiver;

    private final Runnable heartbeat = new Runnable() {
        @Override
        public void run() {
            Log.i(TAG, "heartbeat");
            ensureWakeLock();
            handler.postDelayed(this, HEARTBEAT_MS);
        }
    };

    public static void start(Context context) {
        Intent intent = new Intent(context, MPhoneAgentService.class);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            context.startForegroundService(intent);
        } else {
            context.startService(intent);
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
        createNotificationChannel();
        startForeground(NOTIFICATION_ID, buildNotification());
        ensureWakeLock();
        registerScreenReceiver();
        handler.removeCallbacks(heartbeat);
        handler.post(heartbeat);
        Log.i(TAG, "service started");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        ensureWakeLock();
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        handler.removeCallbacks(heartbeat);
        unregisterScreenReceiver();
        releaseWakeLock();
        releaseScreenWakeLock();
        Log.w(TAG, "service destroyed");
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void ensureWakeLock() {
        if (wakeLock != null && wakeLock.isHeld()) {
            return;
        }
        PowerManager powerManager = (PowerManager) getSystemService(POWER_SERVICE);
        if (powerManager == null) {
            return;
        }
        wakeLock = powerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "MPhone:AgentWakeLock");
        wakeLock.setReferenceCounted(false);
        wakeLock.acquire();
    }

    private void pulseScreenWakeLock() {
        PowerManager powerManager = (PowerManager) getSystemService(POWER_SERVICE);
        if (powerManager == null) {
            return;
        }
        releaseScreenWakeLock();
        int flags = PowerManager.ACQUIRE_CAUSES_WAKEUP | PowerManager.ON_AFTER_RELEASE;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
            flags |= PowerManager.SCREEN_BRIGHT_WAKE_LOCK;
        } else {
            flags |= PowerManager.FULL_WAKE_LOCK;
        }
        screenWakeLock = powerManager.newWakeLock(flags, "MPhone:ScreenGuardWakeLock");
        screenWakeLock.setReferenceCounted(false);
        screenWakeLock.acquire(10_000L);
        Log.i(TAG, "screen wake pulse");
    }

    private void releaseWakeLock() {
        if (wakeLock != null && wakeLock.isHeld()) {
            wakeLock.release();
        }
        wakeLock = null;
    }

    private void releaseScreenWakeLock() {
        if (screenWakeLock != null && screenWakeLock.isHeld()) {
            screenWakeLock.release();
        }
        screenWakeLock = null;
    }

    private void registerScreenReceiver() {
        if (screenReceiver != null) {
            return;
        }
        screenReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent != null ? intent.getAction() : "";
                Log.i(TAG, "screen event: " + action);
                ensureWakeLock();
                if (Intent.ACTION_SCREEN_OFF.equals(action)) {
                    pulseScreenWakeLock();
                }
            }
        };
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_SCREEN_OFF);
        filter.addAction(Intent.ACTION_SCREEN_ON);
        filter.addAction(Intent.ACTION_USER_PRESENT);
        registerReceiver(screenReceiver, filter);
    }

    private void unregisterScreenReceiver() {
        if (screenReceiver == null) {
            return;
        }
        try {
            unregisterReceiver(screenReceiver);
        } catch (Throwable ignored) {
        }
        screenReceiver = null;
    }

    private Notification buildNotification() {
        Intent intent = new Intent(this, MainActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(
                this,
                0,
                intent,
                Build.VERSION.SDK_INT >= Build.VERSION_CODES.M
                        ? PendingIntent.FLAG_IMMUTABLE
                        : 0);

        Notification.Builder builder = Build.VERSION.SDK_INT >= Build.VERSION_CODES.O
                ? new Notification.Builder(this, CHANNEL_ID)
                : new Notification.Builder(this);
        return builder
                .setContentTitle("MPhone Agent")
                .setContentText("Keeping phone farm device online")
                .setSmallIcon(android.R.drawable.stat_sys_upload_done)
                .setOngoing(true)
                .setContentIntent(pendingIntent)
                .build();
    }

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            return;
        }
        NotificationChannel channel = new NotificationChannel(
                CHANNEL_ID,
                "MPhone Agent",
                NotificationManager.IMPORTANCE_MIN);
        channel.setShowBadge(false);
        NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        if (manager != null) {
            manager.createNotificationChannel(channel);
        }
    }
}
