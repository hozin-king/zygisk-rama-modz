package com.bench.ramamodz;
import java.io.*;
public class RootHelper {
    private static Process su; private static DataOutputStream out; private static BufferedReader in;
    static { System.loadLibrary("ramaroot"); }
    public static native boolean nativeAttach(int pid);
    public static native long nativeReadLong(long addr);
    public static native boolean nativeWriteInt(long addr, int val);
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
            out.writeBytes(cmd + "\n"); out.flush(); Thread.sleep(80);
            StringBuilder sb = new StringBuilder();
            while (in.ready()) sb.append(in.readLine()).append('\n');
            return sb.toString().trim();
        } catch (Exception e) { return ""; }
    }
    public static int getPid(String pkg) {
        String res = exec("pidof " + pkg);
        if (res == null || res.isEmpty()) return -1;
        try { return Integer.parseInt(res.split("\\s+")[0]); } catch (Exception e) { return -1; }
    }
}
