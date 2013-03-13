package org.fmod.dspeffectperspeaker;

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
			float pan = 0.0f;
			
			cUpdate();
			
			((TextView)findViewById(R.id.txtChans)).setText(String.format("%d", cGetChannelsPlaying()));

			pan = (float)(((SeekBar)findViewById(R.id.seekBar1)).getProgress() - 50) / 50.0f;
			cSetPan(pan);
			
			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};
	
    public void toggleLowpass(View view)
    {
    	cToggleLowpass();
    }
    
    public void toggleChorus(View view)
    {
    	cToggleChorus();
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        ((SeekBar)findViewById(R.id.seekBar1)).setMax(100);
        ((SeekBar)findViewById(R.id.seekBar1)).setProgress(50); 
    }
    
    @Override
    public void onStart()
    {
    	super.onStart();   	
    	mFMODAudioDevice.start();
    	cBegin();
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
        System.loadLibrary("main");
    }
    
	public native void cBegin();
	public native void cUpdate();
	public native void cEnd();

	public native int  cGetChannelsPlaying();
	public native void cToggleLowpass();
	public native void cToggleChorus();
	public native void cSetPan(float pan);
}
