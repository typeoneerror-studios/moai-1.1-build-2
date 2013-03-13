package org.fmod.recording;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.TextView;

public class Example extends Activity 
{  
	private Handler mUpdateHandler = new Handler()
	{
		public void handleMessage(Message msg)
		{		
			cUpdate();
			
			int recPos = cGetRecordPosition();
			int playPos = cGetPlayPosition();
			
			((TextView)findViewById(R.id.txtRecPos)).setText(String.format("%d", recPos));
			((TextView)findViewById(R.id.txtPlayPos)).setText(String.format("%d", playPos));
			((TextView)findViewById(R.id.txtLoop)).setText(String.format(cGetLooping() ? "On" : "Off"));
			((TextView)findViewById(R.id.txtState)).setText(cGetRecording() ? (cGetPlaying() ? "Recording / Playing" : "Recording") : (cGetPlaying() ? "Playing" : "Idle"));	
			
			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};
	
    public void recordSound(View view)
    {
    	cRecordSound();
    }	
	
    public void playSound(View view)
    {
    	cPlaySound();
    }
    
    public void toggleLoop(View view)
    {
    	cToggleLoop();
    }
    
    public void stopAll(View view)
    {
    	cStopAll();
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
    	cBegin();
    	mUpdateHandler.sendMessageDelayed(mUpdateHandler.obtainMessage(0), 0);
    }
    
    @Override
    public void onStop()
    {
    	mUpdateHandler.removeMessages(0);
    	cEnd();
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
	public native void cRecordSound();
	public native void cPlaySound();
	public native void cToggleLoop();
	public native void cStopAll();
	public native boolean cGetRecording();
	public native boolean cGetPlaying();
	public native boolean cGetLooping();	
	public native int cGetRecordPosition();
	public native int cGetPlayPosition();
}
