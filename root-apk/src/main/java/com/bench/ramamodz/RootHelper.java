package com.bench.ramamodz;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStreamReader;

public class RootHelper {
    private static Process su;
    private static DataOutputStream out;
    private static BufferedReader in;

    static {
        System.loadLibrary("ramaroot");
    }

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
            out.writeBytes("id\n");
            out.flush();
            String line = in.readLine();
            return line != null && line.contains("uid=0");
        } catch (Exception e) {
            return false;
        }
    }

    public static String exec(String cmd) {
        try {
            out.writeBytes(cmd + "\n");
            out.flush();
            Thread.sleep(50);
            StringBuilder sb = new StringBuilder();
            while (in.ready()) sb.append(in.readLine()).append('\n');
            return sb.toString().trim();
        } catch (Exception e) {
            return "";
        }
    }

    // ==================== Root injector flow ====================
    // Salin payload ke /data/local/tmp + label SELinux agar bisa dibaca+dieksekusi game,
    // lalu jalankan injector ptrace (executable librama_inject.so) sebagai root.
    public static String injectSo(android.content.Context ctx, String pkg) {
        try {
            int pid = nativeFindPid(pkg);
            if (pid <= 0) return "game belum jalan (pid tidak ada): " + pkg;
            String libDir = ctx.getApplicationInfo().nativeLibraryDir;
            String payload = libDir + "/libRAMA_MODZ.so";
            String inj = libDir + "/librama_inject.so";
            String tmp = "/data/local/tmp/libRAMA_MODZ.so";
            String log = "/data/local/tmp/rama_inject.log";
            exec("rm -f " + log);
            exec("cp " + payload + " " + tmp);
            exec("chmod 755 " + tmp);
            // Label apk_data_file: untrusted_app (game) boleh read+execute. Tanpa ini dlopen EACCES.
            exec("chcon u:object_r:apk_data_file:s0 " + tmp);
            exec("( " + inj + " " + pid + " " + tmp + " > " + log + " 2>&1; echo DONE_$? >> " + log + " ) &");
            for (int i = 0; i < 60; i++) {
                Thread.sleep(500);
                String out = exec("cat " + log);
                if (out.contains("DONE_")) return out;
            }
            return "timeout inject; log:\n" + exec("cat " + log);
        } catch (Exception e) {
            return "inject error: " + e;
        }
    }
}
