package com.bench.ramamodz;
import android.app.Activity; import android.content.Intent; import android.net.Uri;
import android.os.Build; import android.os.Bundle; import android.provider.Settings; import android.widget.Toast;
public class MainActivity extends Activity {
    private static final int REQ = 1001;
    @Override protected void onCreate(Bundle s) {
        super.onCreate(s);
        if (!RootHelper.init()) { Toast.makeText(this, "root required", Toast.LENGTH_LONG).show(); finish(); return; }
        if (Build.VERSION.SDK_INT >= 23 && !Settings.canDrawOverlays(this)) {
            startActivityForResult(new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION, Uri.parse("package:" + getPackageName())), REQ);
            return;
        }
        startService(new Intent(this, OverlayService.class));
        Toast.makeText(this, "menu started", Toast.LENGTH_SHORT).show();
        finish();
    }
    @Override protected void onActivityResult(int req, int res, Intent data) {
        if (req == REQ && Build.VERSION.SDK_INT >= 23 && Settings.canDrawOverlays(this)) {
            startService(new Intent(this, OverlayService.class)); finish();
        } else if (req == REQ) { Toast.makeText(this, "overlay required", Toast.LENGTH_LONG).show(); finish(); }
    }
}
