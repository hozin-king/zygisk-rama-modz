package com.bench.ramamodz;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.ScrollView;
import android.widget.TextView;

public class MainActivity extends Activity {
    private TextView status;
    private String pkg = "com.dts.freefiremax";

    @Override protected void onCreate(Bundle s) {
        super.onCreate(s);
        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setPadding(40, 40, 40, 40);
        root.setBackgroundColor(Color.parseColor("#0A0A18"));

        TextView title = new TextView(this);
        title.setText("RAMA MODZ — Root Injector (OB55)\nCredit fitur: RAMA MODZ / @RAMA_MODZ");
        title.setTextColor(Color.WHITE);
        title.setTextSize(16f);
        root.addView(title);

        RadioGroup rg = new RadioGroup(this);
        RadioButton max = new RadioButton(this);
        max.setText("Free Fire MAX (com.dts.freefiremax)");
        max.setTextColor(Color.WHITE);
        max.setChecked(true);
        RadioButton th = new RadioButton(this);
        th.setText("Free Fire TH (com.dts.freefireth)");
        th.setTextColor(Color.WHITE);
        rg.addView(max); rg.addView(th);
        rg.setOnCheckedChangeListener((g, id) ->
                pkg = (id == max.getId()) ? "com.dts.freefiremax" : "com.dts.freefireth");
        root.addView(rg);

        Button inject = new Button(this);
        inject.setText("INJECT (setelah masuk lobby)");
        root.addView(inject);

        status = new TextView(this);
        status.setTextColor(Color.parseColor("#9FB4FF"));
        status.setText("1. Buka FF sampai lobby\n2. Balik ke sini, tekan INJECT\n"
                + "3. Balik ke game — menu ImGui muncul ±10-20 detik\n"
                + "Butuh root (Magisk/KernelSU). Tanpa Zygisk/LSPosed.");
        ScrollView sv = new ScrollView(this);
        sv.addView(status);
        root.addView(sv);
        setContentView(root);

        if (!RootHelper.init()) {
            status.setText("ROOT TIDAK ADA / ditolak. Grant root untuk app ini.");
            inject.setEnabled(false);
            return;
        }

        inject.setOnClickListener(v -> {
            inject.setEnabled(false);
            status.setText("inject berjalan... (tunggu, bisa ~30 detik)");
            final String target = pkg;
            new Thread(() -> {
                String res = RootHelper.injectSo(getApplicationContext(), target);
                runOnUiThread(() -> { status.setText(res); inject.setEnabled(true); });
            }).start();
        });
    }
}
