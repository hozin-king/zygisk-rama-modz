package com.bench.ramamodz;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

public class OverlayService extends Service {
    private WindowManager wm;
    private LinearLayout menu;
    private TextView status;
    private boolean attached;
    private final Handler main = new Handler(Looper.getMainLooper());
    private boolean attaching;

    @Override
    public void onCreate() {
        super.onCreate();
        String ch = "rama_root";
        if (Build.VERSION.SDK_INT >= 26) {
            NotificationChannel c = new NotificationChannel(ch, "RAMA Root",
                    NotificationManager.IMPORTANCE_MIN);
            c.setShowBadge(false);
            ((NotificationManager) getSystemService(NOTIFICATION_SERVICE)).createNotificationChannel(c);
        }
        Notification.Builder b = Build.VERSION.SDK_INT >= 26
                ? new Notification.Builder(this, ch) : new Notification.Builder(this);
        startForeground(1, b.setContentTitle("RAMA Root").setContentText("idle")
                .setSmallIcon(android.R.drawable.ic_menu_manage)
                .setOngoing(true).build());
        buildMenu();
    }

    private void buildMenu() {
        wm = (WindowManager) getSystemService(WINDOW_SERVICE);
        menu = new LinearLayout(this);
        menu.setOrientation(LinearLayout.VERTICAL);
        menu.setPadding(20, 16, 20, 16);
        menu.setBackgroundColor(Color.parseColor("#E0111111"));
        // reduce overdraw cost
        menu.setLayerType(View.LAYER_TYPE_HARDWARE, null);

        TextView title = new TextView(this);
        title.setText("RAMA Root");
        title.setTextColor(Color.WHITE);
        title.setTextSize(14);
        menu.addView(title);

        status = new TextView(this);
        status.setText("idle — open lobby then Attach");
        status.setTextColor(Color.LTGRAY);
        status.setTextSize(11);
        status.setPadding(0, 6, 0, 10);
        menu.addView(status);

        Button attach = new Button(this);
        attach.setText("Attach FF Max / TH");
        attach.setOnClickListener(v -> {
            if (attaching) return;
            attaching = true;
            status.setText("attaching…");
            // heavy work off UI thread — avoids lag freeze
            new Thread(() -> {
                String r1 = RootHelper.nativeAttachStatus("com.dts.freefiremax");
                final String result;
                if (r1 != null && r1.startsWith("OK")) {
                    result = r1;
                } else {
                    String r2 = RootHelper.nativeAttachStatus("com.dts.freefireth");
                    result = r2 != null ? r2 : r1;
                }
                main.post(() -> {
                    attaching = false;
                    attached = result != null && result.startsWith("OK");
                    status.setText(result != null ? result : "fail");
                    status.setTextColor(attached ? Color.GREEN : Color.rgb(255, 160, 160));
                    Toast.makeText(this, attached ? "attached" : "attach failed — see status",
                            Toast.LENGTH_SHORT).show();
                });
            }).start();
        });
        menu.addView(attach);

        Button close = new Button(this);
        close.setText("Close");
        close.setOnClickListener(v -> stopSelf());
        menu.addView(close);

        final WindowManager.LayoutParams lp = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT,
                Build.VERSION.SDK_INT >= 26
                        ? WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY
                        : WindowManager.LayoutParams.TYPE_PHONE,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                        | WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN
                        | WindowManager.LayoutParams.FLAG_HARDWARE_ACCELERATED,
                PixelFormat.TRANSLUCENT);
        lp.gravity = Gravity.TOP | Gravity.START;
        lp.x = 40;
        lp.y = 180;
        // don't steal continuous input from game when not dragging
        lp.alpha = 0.92f;

        menu.setOnTouchListener(new View.OnTouchListener() {
            int lx, ly;
            float tx, ty;
            @Override
            public boolean onTouch(View v, MotionEvent e) {
                switch (e.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        lx = lp.x; ly = lp.y; tx = e.getRawX(); ty = e.getRawY();
                        return true;
                    case MotionEvent.ACTION_MOVE:
                        lp.x = lx + (int) (e.getRawX() - tx);
                        lp.y = ly + (int) (e.getRawY() - ty);
                        wm.updateViewLayout(menu, lp);
                        return true;
                }
                return false;
            }
        });
        wm.addView(menu, lp);
    }

    @Override
    public void onDestroy() {
        if (menu != null && wm != null) {
            try { wm.removeView(menu); } catch (Exception ignored) {}
        }
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}
