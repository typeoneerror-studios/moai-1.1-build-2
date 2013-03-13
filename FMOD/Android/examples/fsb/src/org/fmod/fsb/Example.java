package org.fmod.fsb;

import org.fmod.FMODAudioDevice;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.TextView;

public class Example extends Activity 
{
    private FMODAudioDevice mFMODAudioDevice = new FMODAudioDevice();
    
	private Handler mUpdateHandler = new Handler()
	{
		public void handleMessage(Message msg)
		{		
			cUpdate();
			
			int position = cGetPosition();
			int length = cGetLength();
			int nextsubsound = cGetNextSubsound();
			int totalsubsounds = cGetNumSubsounds();
			
			((TextView)findViewById(R.id.txtNextSubsound)).setText(String.format("%d", nextsubsound));	
			((TextView)findViewById(R.id.txtTotalSubsounds)).setText(String.format("%d", totalsubsounds));		
			((TextView)findViewById(R.id.txtState)).setText(cGetPlaying() ? "Playing" : "Stopped");	
			((TextView)findViewById(R.id.txtPos)).setText(String.format("%02d:%02d:%02d / %02d:%02d:%02d", position / 1000 / 60, position / 1000 % 60, position / 10 % 100, length / 1000 / 60, length / 1000 % 60, length / 10 % 100));

			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};
	
    public void playNextSound(View view)
    {
    	cPlayNextSound();
    }    
	
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
	public native void cPlayNextSound();
	public native int cGetNumSubsounds();
	public native int cGetNextSubsound();
	public native int cGetLength();
	public native int cGetPosition();
	public native boolean cGetPlaying();
}
