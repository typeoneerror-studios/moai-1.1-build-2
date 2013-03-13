package org.fmod.generatetone;

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
			cUpdate();
			
			if (cGetIsChannelPlaying())
			{
				float volume = cGetChannelVolume();	
				float frequency = cGetChannelFrequency();
				float pan = cGetChannelPan();
				
				((TextView)findViewById(R.id.txtVolume)).setText(String.format("%.02f", volume));
				((TextView)findViewById(R.id.txtFrequency)).setText(String.format("%.02f", frequency));
				((TextView)findViewById(R.id.txtPan)).setText(String.format("%.02f", pan));

				volume = (float)((SeekBar)findViewById(R.id.seekBarVolume)).getProgress() / 100.0f;
				cSetChannelVolume(volume);
				
				frequency = (float)((SeekBar)findViewById(R.id.seekBarFrequency)).getProgress();
				cSetChannelFrequency(frequency);
				
				pan = (float)(((SeekBar)findViewById(R.id.seekBarPan)).getProgress() - 50) / 50.0f;
				cSetChannelPan(pan);
			}
			
			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};
	
    public void playSine(View view)
    {
    	cPlayDSPSine();
    	
		float volume = cGetChannelVolume();	
		float frequency = cGetChannelFrequency();
		float pan = cGetChannelPan();
		
		((SeekBar)findViewById(R.id.seekBarVolume)).setProgress((int)(volume * 100));
        ((SeekBar)findViewById(R.id.seekBarFrequency)).setProgress((int)frequency);
        ((SeekBar)findViewById(R.id.seekBarPan)).setProgress((int)(pan * 50) + 50);	
    }
   
    public void playSquare(View view)
    {
    	cPlayDSPSquare();
    	
    	float volume = cGetChannelVolume();	
		float frequency = cGetChannelFrequency();
		float pan = cGetChannelPan();
		
		((SeekBar)findViewById(R.id.seekBarVolume)).setProgress((int)(volume * 100));
        ((SeekBar)findViewById(R.id.seekBarFrequency)).setProgress((int)frequency);
        ((SeekBar)findViewById(R.id.seekBarPan)).setProgress((int)(pan * 50) + 50);	
    }
   
    public void playSawUp(View view)
    {
    	cPlayDSPSawUp();
    	
    	float volume = cGetChannelVolume();	
		float frequency = cGetChannelFrequency();
		float pan = cGetChannelPan();
		
		((SeekBar)findViewById(R.id.seekBarVolume)).setProgress((int)(volume * 100));
        ((SeekBar)findViewById(R.id.seekBarFrequency)).setProgress((int)frequency);
        ((SeekBar)findViewById(R.id.seekBarPan)).setProgress((int)(pan * 50) + 50);	
    }
   
    public void playTriangle(View view)
    {
    	cPlayDSPTriangle();
    	
    	float volume = cGetChannelVolume();	
		float frequency = cGetChannelFrequency();
		float pan = cGetChannelPan();
		
		((SeekBar)findViewById(R.id.seekBarVolume)).setProgress((int)(volume * 100));
        ((SeekBar)findViewById(R.id.seekBarFrequency)).setProgress((int)frequency);
        ((SeekBar)findViewById(R.id.seekBarPan)).setProgress((int)(pan * 50) + 50);	
    }
   
    public void playNoise(View view)
    {
    	cPlayDSPNoise();
    	
    	float volume = cGetChannelVolume();	
		float frequency = cGetChannelFrequency();
		float pan = cGetChannelPan();
		
		((SeekBar)findViewById(R.id.seekBarVolume)).setProgress((int)(volume * 100));
        ((SeekBar)findViewById(R.id.seekBarFrequency)).setProgress((int)frequency);
        ((SeekBar)findViewById(R.id.seekBarPan)).setProgress((int)(pan * 50) + 50);	
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
    	
        ((SeekBar)findViewById(R.id.seekBarVolume)).setMax(100);
        ((SeekBar)findViewById(R.id.seekBarFrequency)).setMax(88200);
        ((SeekBar)findViewById(R.id.seekBarPan)).setMax(100);
        
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
	
	public native void cPlayDSPSine();
	public native void cPlayDSPSquare();
	public native void cPlayDSPSawUp();
	public native void cPlayDSPTriangle();
	public native void cPlayDSPNoise();	
	
	public native boolean cGetIsChannelPlaying();
	public native float cGetChannelFrequency();
	public native float cGetChannelVolume();
	public native float cGetChannelPan();
	
	public native void cSetChannelFrequency(float frequency);
	public native void cSetChannelVolume(float volume);
	public native void cSetChannelPan(float pan);	
}
