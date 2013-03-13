package org.fmod.netstream;

import org.fmod.FMODAudioDevice;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.TextView;
import android.widget.EditText;

public class Example extends Activity 
{
    private FMODAudioDevice mFMODAudioDevice = new FMODAudioDevice();   
    
	private Handler mUpdateHandler = new Handler()
	{
		public void handleMessage(Message msg)
		{		
			cUpdate();
			

			String state;
			
			switch (cGetOpenState())
			{
			case 0:
				state = "READY";
				break;
			case 1:
				state = "LOADING";
				break;
			case 2:
				state = "ERROR";
				break;
			case 3:
				state = "CONNECTING";
				break;
			case 4:
				state = "BUFFERING";
				break;
			case 5:
				state = "SEEKING";
				break;
			case 6:
				state = "PLAYING";
				break;
			case 7:
				state = "SETPOSITION";
				break;
			default:
				state = "UNKNOWN";
				break;
			}
			
			((TextView)findViewById(R.id.txtState)).setText(state);
			
			int position = cGetPosition();
			int buffered = cGetBuffered();
			
			((TextView)findViewById(R.id.txtPos)).setText(String.format("%02d:%02d:%02d", position / 1000 / 60, position / 1000 % 60, position / 10 % 100));
			((TextView)findViewById(R.id.txtBuffered)).setText(String.format("%d %%", buffered));
			
			removeMessages(0);
		    sendMessageDelayed(obtainMessage(0), 50);
		}
	};
	
    public void playUrl(View view)
    {
    	String url = ((EditText)findViewById(R.id.editText1)).getText().toString();
    	
    	cPlayNetStream(url);
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

	public native int cGetPosition();
	public native int cGetOpenState();
	public native int cGetBuffered();
	public native boolean cGetStarving();
	public native boolean cGetIsPlaying();
	
	public native void cPlayNetStream(String url);
}
