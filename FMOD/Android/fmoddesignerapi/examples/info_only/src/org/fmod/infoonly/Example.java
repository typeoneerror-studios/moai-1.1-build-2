package org.fmod.infoonly;

import org.fmod.FMODAudioDevice;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

public class Example extends Activity 
{
	private FMODAudioDevice mFMODAudioDevice = new FMODAudioDevice();
    
	private Handler mUpdateHandler = new Handler()
	{
		public void handleMessage(Message msg)
		{	
			float volume;
			
			cUpdate();
		
			((TextView)findViewById(R.id.txtParam)).setText(String.format("%.02f", cGetVolume()));	
			
			volume = (float)((SeekBar)findViewById(R.id.seekBar1)).getProgress();
			volume /= 100.0f;
			
			cSetVolume(volume);
			
			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};	
	
    public void getInfoOnlyEvent(View view)
    {        
    	cGetInfoOnlyEvent();
    
        ((SeekBar)findViewById(R.id.seekBar1)).setProgress((int)(100.0f * cGetVolume()));	
    }
    
    public void getEvent(View view)
    {
    	cGetEvent();
    	
        ((SeekBar)findViewById(R.id.seekBar1)).setProgress((int)(100.0f * cGetVolume()));    	
    }
    
    public void startStopEvent(View view)
    {
    	cStartStopEvent();
    	
    	((SeekBar)findViewById(R.id.seekBar1)).setProgress((int)(100.0f * cGetVolume()));	
    }
    
    public void unloadEventData(View view)
    {
    	cUnloadEventData();
    	
        ((SeekBar)findViewById(R.id.seekBar1)).setProgress((int)(100.0f * cGetVolume()));	
    }
    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        mUpdateHandler.sendMessageDelayed(mUpdateHandler.obtainMessage(0), 0);
    }
    
    @Override
    public void onStart()
    {
    	super.onStart();   	
    	mFMODAudioDevice.start();
    	cBegin();
    	
        ((SeekBar)findViewById(R.id.seekBar1)).setMax(100);
        ((SeekBar)findViewById(R.id.seekBar1)).setProgress(100);
        
    	mUpdateHandler.sendMessageDelayed(mUpdateHandler.obtainMessage(0), 0);
    }
    
    @Override
    public void onStop()
    {
    	mUpdateHandler.removeMessages(0);
    	cEnd();
    	mFMODAudioDevice.stop();
    	super.onStop();
    }
	
    static 
    {
    	System.loadLibrary("fmodex");
    	System.loadLibrary("fmodevent");
        System.loadLibrary("main");
    }
    
	public native void cBegin();
	public native void cUpdate();
	public native void cEnd();
	public native void cGetInfoOnlyEvent();
	public native void cGetEvent();
	public native void cStartStopEvent();
	public native void cUnloadEventData();
	public native void cSetVolume(float volume);
	public native float cGetVolume();
}
