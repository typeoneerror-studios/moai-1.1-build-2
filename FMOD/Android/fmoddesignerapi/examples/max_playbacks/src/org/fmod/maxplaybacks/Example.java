package org.fmod.maxplaybacks;

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
		
			((TextView)findViewById(R.id.txtSound)).setText(String.format("%d", cGetSound()));
			((TextView)findViewById(R.id.txtDistance)).setText(String.format("%.02f", cGetDistance()));
			((TextView)findViewById(R.id.txtBehaviour)).setText(cGetEventBehaviour());
			
			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};	
	
    public void changeEventBehaviour(View view)
    {        
    	cChangeEventBehaviour();	
    }
    
    public void startEvent(View view)
    {
    	cStartEvent();    	
    }
    
    public void stopAllEvents(View view)
    {
    	cStopAllEvents();	
    }
    
    public void increaseEventDistance(View view)
    {
    	cIncreaseEventDistance();	
    }
    
    public void decreaseEventDistance(View view)
    {
    	cDecreaseEventDistance();	
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
	public native void cChangeEventBehaviour();
	public native void cStartEvent();
	public native void cStopAllEvents();
	public native void cIncreaseEventDistance();
	public native void cDecreaseEventDistance();
	public native int cGetSound();
	public native float cGetDistance();
	public native String cGetEventBehaviour();
}
