package org.fmod.parameters;

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
			float param;
			
			cUpdate();
		
			((TextView)findViewById(R.id.txtRPM)).setText(String.format("%.02f", cGetRPM()));	
			((TextView)findViewById(R.id.txtLoad)).setText(String.format("%.02f", cGetLoad()));	
						
			param = (float)((SeekBar)findViewById(R.id.seekBarRPM)).getProgress();
			param += cGetRPMMin();		
			cSetRPM(param);

			param = (float)((SeekBar)findViewById(R.id.seekBarLoad)).getProgress();
			param /= 100.0f;
			param += cGetLoadMin();		
			cSetLoad(param);
			
			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};	
	
    public void togglePause(View view)
    {
    	cTogglePause();
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
    	
        ((SeekBar)findViewById(R.id.seekBarRPM)).setMax((int)(cGetRPMMax() - cGetRPMMin()));
        ((SeekBar)findViewById(R.id.seekBarLoad)).setMax((int)((cGetLoadMax() - cGetLoadMin()) * 100.0f));
        
        ((SeekBar)findViewById(R.id.seekBarRPM)).setProgress(1000);
        
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
	public native void cSetRPM(float rpm);
	public native float cGetRPM();
	public native float cGetRPMMin();
	public native float cGetRPMMax();
	public native void cSetLoad(float load);
	public native float cGetLoad();
	public native float cGetLoadMin();
	public native float cGetLoadMax();
	public native void cTogglePause();
}
