package org.fmod.readtags;

import org.fmod.FMODAudioDevice;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class Example extends Activity 
{
    private FMODAudioDevice mFMODAudioDevice = new FMODAudioDevice();   
    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
    }
    
    @Override
    public void onStart()
    {
    	super.onStart();   	
    	mFMODAudioDevice.start();
    	cBegin();
    
		((TextView)findViewById(R.id.txtArtistTag)).setText(cGetArtist());
		((TextView)findViewById(R.id.txtTitleTag)).setText(cGetTitle());	
    }
    
    @Override
    public void onStop()
    {
    	cEnd();
    	mFMODAudioDevice.stop();
    	super.onStop();
    }
    	
    static 
    {
    	System.loadLibrary("fmodex");
        System.loadLibrary("main");
    }
    
	public native void cBegin();
	public native void cUpdate();
	public native void cEnd();
	public native String cGetArtist();
	public native String cGetTitle();	
}
