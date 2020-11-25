package com.huawei.textures;

// Load the native C++ libraries

public class GL2TexturesLib {

    static {
        System.loadLibrary("textures");
    }

    // JNI functions load from C++
    public static native void init(int width, int height);
    public static native void step();
}
