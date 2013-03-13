package org.fmod.threed;

import org.fmod.FMODAudioDevice;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.SeekBar;

public class Example extends Activity 
{
    private FMODAudioDevice mFMODAudioDevice = new FMODAudioDevice();
    
	private Handler mUpdateHandler = new Handler()
	{
		public void handleMessage(Message msg)
		{		
			cUpdate();
			
			if (cGetListenerMovement())
			{
				((SeekBar)findViewById(R.id.seekBar1)).setProgress((int)cGetListenerPosition() + 35);
			}
			else
			{
				cSetListenerPosition((float)(((SeekBar)findViewById(R.id.seekBar1)).getProgress() - 35));
			}
			
			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};

	
    public void togglePause16bit3d(View view)
    {
    	cTogglePause16bit3d();
    }
    
    public void togglePause8bit3d(View view)
    {
    	cTogglePause8bit3d();
    }
    
    public void play16bit2d(View view)
    {
    	cPlay16bit2d();
    }    
	
    public void toggleListenerMovement(View view)
    {
    	cToggleListenerMovement();
    }
    
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        ((SeekBar)findViewById(R.id.seekBar1)).setMax(70);
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
	public native void cTogglePause16bit3d();
	public native void cTogglePause8bit3d();
	public native void cPlay16bit2d();
	public native void cToggleListenerMovement();	
	public native boolean cGetListenerMovement();
	public native float   cGetListenerPosition();
	public native void    cSetListenerPosition(float position);
}
