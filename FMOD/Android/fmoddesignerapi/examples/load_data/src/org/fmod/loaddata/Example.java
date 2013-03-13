package org.fmod.loaddata;

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
		
			((TextView)findViewById(R.id.txtMemCurrent)).setText(String.format("%d", cGetMemoryCurrent()));	
			((TextView)findViewById(R.id.txtMemMax)).setText(String.format("%d", cGetMemoryMax()));	

			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};	
	
    public void loadEventData(View view)
    {        
    	cLoadEventData();	
    }
    
    public void unLoadEventData(View view)
    {
    	cUnLoadEventData();    	
    }
    
    public void loadGroupData(View view)
    {
    	cLoadGroupData();	
    }
    
    public void unLoadGroupData(View view)
    {
    	cUnLoadGroupData();	
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
	public native void cLoadEventData();
	public native void cUnLoadEventData();
	public native void cLoadGroupData();
	public native void cUnLoadGroupData();
	public native int cGetMemoryCurrent();
	public native int cGetMemoryMax();
}
