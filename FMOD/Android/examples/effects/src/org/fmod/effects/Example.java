package org.fmod.effects;

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
					
			((TextView)findViewById(R.id.txtState)).setText(cGetPaused() ? "Paused" : "Playing");	

			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};
	
    public void pauseStream(View view)
    {
    	cPause();
    }	
	
    public void toggleLowpass(View view)
    {
    	cLowpassToggle();
    }

    public void toggleHighpass(View view)
    {
    	cHighpassToggle();
    }
    
    public void toggleEcho(View view)
    {
    	cEchoToggle();
    }
    
    public void toggleFlange(View view)
    {
    	cFlangeToggle();
    }
    
    public void toggleDistortion(View view)
    {
    	cDistortionToggle();
    }    
    
    public void toggleChorus(View view)
    {
    	cChorusToggle();
    }    
     
    public void toggleParameq(View view)
    {
    	cParameqToggle();
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
	public native void cPause();
	public native boolean cGetPaused();
	
	public native void cLowpassToggle();
	public native void cHighpassToggle();
	public native void cEchoToggle();
	public native void cFlangeToggle();
	public native void cDistortionToggle();
	public native void cChorusToggle();
	public native void cParameqToggle();	
}
