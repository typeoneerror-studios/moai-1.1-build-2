package org.fmod.effects;

import org.fmod.FMODAudioDevice;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
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
		
			((TextView)findViewById(R.id.txtParam)).setText(String.format("%.04f", cGetParamValue()));	
			
			param = (float)((SeekBar)findViewById(R.id.seekBar1)).getProgress();
			param /= 100.0f;
			param += cGetParamMinValue();
			
			cSetParamValue(param);
			
			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};	
	
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
    	
        ((SeekBar)findViewById(R.id.seekBar1)).setMax((int)((cGetParamMaxValue() - cGetParamMinValue()) * 100.0f));
        
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
	public native void cSetParamValue(float value);
	public native float cGetParamValue();
	public native float cGetParamMaxValue();
	public native float cGetParamMinValue();	
}
