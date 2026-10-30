package com.bench.ramamodz;
import android.app.*; import android.content.Intent; import android.graphics.*; import android.os.*;
import android.view.*; import android.widget.*;
public class OverlayService extends Service {
    private WindowManager wm; private LinearLayout menu; private boolean attached;
    @Override public void onCreate() {
        super.onCreate();
        String ch = "rama_root";
        if (Build.VERSION.SDK_INT >= 26) {
            ((NotificationManager) getSystemService(NOTIFICATION_SERVICE))
                .createNotificationChannel(new NotificationChannel(ch, "RAMA Root", NotificationManager.IMPORTANCE_LOW));
        }
        Notification.Builder b = Build.VERSION.SDK_INT >= 26 ? new Notification.Builder(this, ch) : new Notification.Builder(this);
        startForeground(1, b.setContentTitle("RAMA Root").setContentText("menu")
            .setSmallIcon(android.R.drawable.ic_menu_manage).setOngoing(true).build());
        buildMenu();
    }
    private void buildMenu() {
        wm = (WindowManager) getSystemService(WINDOW_SERVICE);
        menu = new LinearLayout(this); menu.setOrientation(LinearLayout.VERTICAL);
        menu.setPadding(24,24,24,24); menu.setBackgroundColor(Color.parseColor("#CC111111"));
        TextView t = new TextView(this); t.setText("RAMA Root Injector"); t.setTextColor(Color.WHITE); t.setTextSize(16); menu.addView(t);
        Button attach = new Button(this); attach.setText("Attach FF Max / TH");
        attach.setOnClickListener(v -> {
            int pid = RootHelper.getPid("com.dts.freefiremax");
            if (pid <= 0) pid = RootHelper.getPid("com.dts.freefireth");
            if (pid <= 0) { Toast.makeText(this, "game not running", Toast.LENGTH_SHORT).show(); return; }
            attached = RootHelper.nativeAttach(pid);
            Toast.makeText(this, attached ? "attached " + pid : "attach fail", Toast.LENGTH_SHORT).show();
        });
        menu.addView(attach);
        Switch sw = new Switch(this); sw.setText("Feature (fill after dump)"); sw.setTextColor(Color.WHITE);
        sw.setOnCheckedChangeListener((b, on) -> {
            if (!attached) { Toast.makeText(this, "attach first", Toast.LENGTH_SHORT).show(); sw.setChecked(false); return; }
            Toast.makeText(this, on ? "on" : "off", Toast.LENGTH_SHORT).show();
        });
        menu.addView(sw);
        Button close = new Button(this); close.setText("Close"); close.setOnClickListener(v -> stopSelf()); menu.addView(close);
        final WindowManager.LayoutParams lp = new WindowManager.LayoutParams(
            WindowManager.LayoutParams.WRAP_CONTENT, WindowManager.LayoutParams.WRAP_CONTENT,
            Build.VERSION.SDK_INT >= 26 ? WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY : WindowManager.LayoutParams.TYPE_PHONE,
            WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE, PixelFormat.TRANSLUCENT);
        lp.gravity = Gravity.TOP | Gravity.START; lp.x = 80; lp.y = 200;
        menu.setOnTouchListener(new View.OnTouchListener() {
            int lx, ly; float tx, ty;
            @Override public boolean onTouch(View v, MotionEvent e) {
                if (e.getAction() == MotionEvent.ACTION_DOWN) { lx = lp.x; ly = lp.y; tx = e.getRawX(); ty = e.getRawY(); return true; }
                if (e.getAction() == MotionEvent.ACTION_MOVE) {
                    lp.x = lx + (int)(e.getRawX() - tx); lp.y = ly + (int)(e.getRawY() - ty);
                    wm.updateViewLayout(menu, lp); return true;
                }
                return false;
            }
        });
        wm.addView(menu, lp);
    }
    @Override public void onDestroy() { if (menu != null && wm != null) try { wm.removeView(menu); } catch (Exception ignored) {} super.onDestroy(); }
    @Override public IBinder onBind(Intent i) { return null; }
}
