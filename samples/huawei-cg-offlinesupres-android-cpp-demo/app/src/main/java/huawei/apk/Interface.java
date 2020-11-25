package huawei.apk;

import android.util.Log;

public class Interface {

    static {
        System.loadLibrary("main-lib");
    }

    private static native boolean Execute(int mode, String imageFilePath);
    public boolean NativeExecute(int mode, String imageFilePath){
        Log.d("NativeExecute", "enter!");
        return Execute(mode, imageFilePath);
    }

    //check image press
    private static native char[] Query(int mode, String imageFilePath);
    public char[] NativeQuery(int mode, String imageFilePath){
        Log.d("NativeQuery", "enter!");
        return Query(mode, imageFilePath);
    }

    private static native boolean Initialize();
    public boolean NativeInitialize(){
        Log.d("NativeInitialize", "enter!");
        return Initialize();
    }

    private static native boolean Uninitialize();
    public boolean NativeUninitialize() {
        Log.d("NativeUninitialize", "enter!");
        return Uninitialize();
    }

    private static native void SetSharpness(float s);
    public void NativeSetSharpness(float s) {
        Log.d("NativeSetSharpness", "enter!");
        SetSharpness(s);
        return ;
    }

    private static native boolean SwitchToneMapping();
    public boolean NativeSwitchToneMapping() {
        Log.d("NativeSwitchToneMapping", "enter!");
        return SwitchToneMapping();
    }
}
