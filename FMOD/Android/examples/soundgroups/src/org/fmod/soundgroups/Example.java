package org.fmod.soundgroups;

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
			String behaviour = new String();
			
			cUpdate();

			switch(cGetBehaviour())
			{
			case 0:
				behaviour = "FAIL";
				break;
			case 1:
				behaviour = "MUTE";
				break;
			case 2:
				behaviour = "STEALLOWEST";
				break;
			}

			((TextView)findViewById(R.id.txtMode)).setText(behaviour);	
			((TextView)findViewById(R.id.txtSound1State)).setText(cGetChannelState(0));	
			((TextView)findViewById(R.id.txtSound2State)).setText(cGetChannelState(1));
			((TextView)findViewById(R.id.txtSound3State)).setText(cGetChannelState(2));			

			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};
	
    public void setBehaviourFail(View view)
    {
    	cSetBehaviourFail();
    }
  
    public void setBehaviourMute(View view)
    {
    	cSetBehaviourMute();
    }
    
    public void setBehaviourSteal(View view)
    {
    	cSetBehaviourSteal();
    }    

    public void playDrumloop(View view)
    {
    	cPlayDrumloop();
    }
    
    public void playJaguar(View view)
    {
    	cPlayJaguar();
    }
    
    public void playSwish(View view)
    {
    	cPlaySwish();
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
	public native void cSetBehaviourFail();
	public native void cSetBehaviourMute();
	public native void cSetBehaviourSteal();
	public native void cPlayDrumloop();
	public native void cPlayJaguar();
	public native void cPlaySwish();
	public native int cGetBehaviour();
	public native String cGetChannelState(int channel);
}
