package com.bench.ramamodz;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStreamReader;

public class RootHelper {
    private static Process su;
    private static DataOutputStream out;
    private static BufferedReader in;

    static { System.loadLibrary("ramaroot"); }

    public static native String nativeAttachStatus(String pkg);
    public static native boolean nativeAttach(int pid);
    public static native long nativeGetIl2CppBase();
    public static native long nativeReadLong(long addr);
    public static native boolean nativeWriteInt(long addr, int val);
    public static native int nativeFindPid(String pkg);

    public static boolean init() {
        try {
            su = Runtime.getRuntime().exec("su");
            out = new DataOutputStream(su.getOutputStream());
            in = new BufferedReader(new InputStreamReader(su.getInputStream()));
            out.writeBytes("id\n"); out.flush();
            String line = in.readLine();
            return line != null && line.contains("uid=0");
        } catch (Exception e) { return false; }
    }

    public static String exec(String cmd) {
        try {
            out.writeBytes(cmd + "\n"); out.flush();
            Thread.sleep(50);
            StringBuilder sb = new StringBuilder();
            while (in.ready()) sb.append(in.readLine()).append('\n');
            return sb.toString().trim();
        } catch (Exception e) { return ""; }
    }

    public static int findPidRoot(String pkg) {
        String out = exec("pidof " + pkg);
        java.util.StringTokenizer st = new java.util.StringTokenizer(out == null ? "" : out);
        while (st.hasMoreTokens()) { try { return Integer.parseInt(st.nextToken()); } catch (Exception e) {} }
        return -1;
    }

    public static void launch(String pkg) { exec("monkey -p " + pkg + " -c android.intent.category.LAUNCHER 1"); }

    public static boolean mapsHas(int pid, String name) {
        String out = exec("cat /proc/" + pid + "/maps | grep -c " + name);
        try { return Integer.parseInt(out.trim()) > 0; } catch (Exception e) { return false; }
    }

    public static String injectSo(android.content.Context ctx, String pkg) {
        try {
            int pid = findPidRoot(pkg);
            if (pid <= 0) return "game belum jalan (pid tidak ada): " + pkg;
            String libDir = ctx.getApplicationInfo().nativeLibraryDir;
            String payload = libDir + "/libRAMA_MODZ.so";
            String inj = libDir + "/librama_inject.so";
            String tmp = "/data/local/tmp/libRAMA_MODZ.so";
            String log = "/data/local/tmp/rama_inject.log";
            exec("rm -f " + log);
            exec("cp " + payload + " " + tmp);
            exec("chmod 755 " + tmp);
            exec("chcon u:object_r:apk_data_file:s0 " + tmp);
            exec("( " + inj + " " + pid + " " + tmp + " > " + log + " 2>&1; echo DONE_$? >> " + log + " ) &");
            for (int i = 0; i < 60; i++) {
                Thread.sleep(500);
                String o = exec("cat " + log);
                if (o.contains("DONE_")) return o;
            }
            return "timeout inject; log:\n" + exec("cat " + log);
        } catch (Exception e) { return "inject error: " + e; }
    }

    public static String autoInject(android.content.Context ctx, String pkg) {
        if (findPidRoot(pkg) <= 0) launch(pkg);
        String last = "";
        for (int i = 0; i < 60; i++) {
            int pid = findPidRoot(pkg);
            if (pid > 0 && mapsHas(pid, "libil2cpp.so")) {
                last = injectSo(ctx, pkg);
                if (last.contains("INJECT_OK") || last.contains("ALREADY_LOADED")) { launch(pkg); return last; }
            }
            try { Thread.sleep(2000); } catch (InterruptedException e) { break; }
        }
        launch(pkg);
        return last.isEmpty() ? "game tidak siap (lobby?)" : last;
    }

    public static String dumpAC(android.content.Context ctx, int pid) {
        String inj = ctx.getApplicationInfo().nativeLibraryDir + "/librama_inject.so";
        exec("mkdir -p /data/local/tmp/rama_dump");
        exec("( " + inj + " dump " + pid + " /data/local/tmp/rama_dump > /data/local/tmp/rama_dump.log 2>&1; echo DONE_$? >> /data/local/tmp/rama_dump.log ) &");
        for (int i = 0; i < 40; i++) {
            try { Thread.sleep(500); } catch (InterruptedException e) { break; }
            String o = exec("cat /data/local/tmp/rama_dump.log");
            if (o.contains("DONE_")) return o;
        }
        return "timeout dump";
    }
}
