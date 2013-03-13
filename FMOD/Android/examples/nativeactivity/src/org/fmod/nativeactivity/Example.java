package org.fmod.nativeactivity;

public class Example extends android.app.NativeActivity 
{
    static 
    {
    	System.loadLibrary("fmodex");
        System.loadLibrary("main");
    }    
}
