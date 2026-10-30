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
}
