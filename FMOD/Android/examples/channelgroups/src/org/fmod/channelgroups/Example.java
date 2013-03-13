package org.fmod.channelgroups;

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
			
			int channels = cGetChannelsPlaying();
			
			((TextView)findViewById(R.id.txtChans)).setText(String.format("%d", channels));

			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};
	
    public void muteGroupA(View view)
    {
    	cMuteGroupA();
    }
    
    public void muteGroupB(View view)
    {
    	cMuteGroupB();
    }
    
    public void muteMasterGroup(View view)
    {
    	cMuteMasterGroup();
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
	public native void cMuteGroupA();
	public native void cMuteGroupB();
	public native void cMuteMasterGroup();
	public native int  cGetChannelsPlaying();
}
