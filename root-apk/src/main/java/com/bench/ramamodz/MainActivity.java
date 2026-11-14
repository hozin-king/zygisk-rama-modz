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
        root.setPadding(40,40,40,40);
        root.setBackgroundColor(Color.parseColor("#0A0A18"));
        TextView title = new TextView(this);
        title.setText("RAMA MODZ — Root Injector (OB55)\nCredit fitur: RAMA MODZ / @RAMA_MODZ");
        title.setTextColor(Color.WHITE); title.setTextSize(16f); root.addView(title);
        RadioGroup rg = new RadioGroup(this);
        RadioButton max = new RadioButton(this); max.setText("Free Fire MAX (com.dts.freefiremax)"); max.setTextColor(Color.WHITE); max.setChecked(true);
        RadioButton th = new RadioButton(this); th.setText("Free Fire TH (com.dts.freefireth)"); th.setTextColor(Color.WHITE);
        rg.addView(max); rg.addView(th);
        rg.setOnCheckedChangeListener((g,id)-> pkg = (id==max.getId())?"com.dts.freefiremax":"com.dts.freefireth");
        root.addView(rg);
        Button auto = new Button(this); auto.setText("AUTO (launch + inject + floating)"); root.addView(auto);
        Button dump = new Button(this); dump.setText("DUMP AC (read-only)"); root.addView(dump);
        status = new TextView(this); status.setTextColor(Color.parseColor("#9FB4FF"));
        status.setText("AUTO: launch kalau belum jalan, tunggu lobby, inject, balik ke game.\nFloating muncul sendiri setelah inject.\nDUMP AC: read-only /proc/pid/mem, tanpa ptrace.");
        ScrollView sv = new ScrollView(this); sv.addView(status); root.addView(sv);
        setContentView(root);
        if (!RootHelper.init()) { status.setText("ROOT TIDAK ADA / ditolak."); auto.setEnabled(false); dump.setEnabled(false); return; }
        auto.setOnClickListener(v->{ auto.setEnabled(false); status.setText("auto berjalan (launch + tunggu lobby)...");
            final String t=pkg; new Thread(()->{ String r=RootHelper.autoInject(getApplicationContext(),t);
                runOnUiThread(()->{status.setText(r);auto.setEnabled(true);}); }).start(); });
        dump.setOnClickListener(v->{ dump.setEnabled(false); status.setText("dump AC (read-only)...");
            final String t=pkg; new Thread(()->{ int pid=RootHelper.findPidRoot(t);
                String r = pid>0? RootHelper.dumpAC(getApplicationContext(),pid):"game belum jalan";
                runOnUiThread(()->{status.setText(r+"\nhasil: /data/local/tmp/rama_dump");dump.setEnabled(true);}); }).start(); });
    }
}
