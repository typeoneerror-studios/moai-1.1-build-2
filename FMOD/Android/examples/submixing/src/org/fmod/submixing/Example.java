package org.fmod.submixing;

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
			
			((TextView)findViewById(R.id.txtChannelsPlaying)).setText(String.format("%d", cGetChannelsPlaying()));	

			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};
	
    public void toggleMuteGroupA(View view)
    {
    	cToggleMuteGroupA();
    }

    public void toggleMuteGroupB(View view)
    {
    	cToggleMuteGroupB();
    }
    
    public void toggleHighPassGroupA(View view)
    {
    	cToggleHighPassGroupA();
    }
    
    public void toggleChorusGroupB(View view)
    {
    	cToggleChorusGroupB();
    }
    
    public void toggleLowpassMaster(View view)
    {
    	cToggleLowpassMasterGroup();
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
	
	public native int  cGetChannelsPlaying();
	public native void cToggleMuteGroupA();
	public native void cToggleMuteGroupB();
	public native void cToggleHighPassGroupA();
	public native void cToggleChorusGroupB();
	public native void cToggleLowpassMasterGroup();
}
