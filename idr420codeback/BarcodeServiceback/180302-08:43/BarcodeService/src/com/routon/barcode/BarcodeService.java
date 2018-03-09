package com.routon.barcode;

import android.app.Service;
import android.content.Intent;
import android.content.Context;
import android.content.IntentFilter;
import android.os.IBinder;
import android.util.Log;
import android.media.SoundPool;
import android.content.BroadcastReceiver;
import android.os.Handler;
import android.os.Message;
import android.media.AudioManager;

import com.routon.scanner.RoutonScanner;
import com.routon.scanner.RoutonScanner.ScannerCallback;

import android.view.InputDevice;


public class BarcodeService extends Service {
	private final static String TAG="BarcodeServcie";
	private int soundId;
	private SoundPool soundPool;
	private BroadcastReceiver mDisplayReceiver;
	private int inputSource;
	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}
	public static final int ROUTON_SCAN_RESULT=1;
	RoutonScanner routonScanner;
	private Handler mHandler = null;
	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
		inputSource = InputDevice.SOURCE_KEYBOARD;
		//Log.d(TAG,"Success  in BarcodeService-20180301!");
		soundPool = new SoundPool(1, AudioManager.STREAM_MUSIC, 0);
        	soundId = soundPool.load("/system/media/audio/ui/VideoRecord.ogg", 0);
		 mHandler = new Handler() {
                @Override
                public void handleMessage(Message msg) {
                    switch (msg.what) {
                        case ROUTON_SCAN_RESULT:
                            //Log.d(TAG,"fenmgc180123-16:54");
                            Log.d(TAG, "[msg]" + msg.obj);
                            //fengmc add 180115-16:54
                            String sendstr=(String)msg.obj+"\n";
                            Intent intent=new Intent("com.se4500.onDecodeComplete");
                            intent.putExtra("se4500",sendstr);
                            sendBroadcast(intent);
                            //Log.d(TAG, "sendstr=" + sendstr);
                            //add
                            break;
                    }
                }
               };
		ScannerCallback cb = new ScannerCallback() {
		    @Override  
		    public void onMessage(String msg) { 
		        //Log.d(TAG,"fengmc180123-16:33");
		        Log.e(TAG, "[MSG]" + msg);
		        mHandler.sendMessage(mHandler.obtainMessage(ROUTON_SCAN_RESULT, msg));
		        
		    }  
		};
		routonScanner = new RoutonScanner(cb);
		routonScanner.start();
		
		mDisplayReceiver = new  BroadcastReceiver(){
		        @Override
		        public void onReceive(Context context, Intent intent){
				String action = intent.getAction();
				//Log.d(TAG,"fengmc action="+action);
				if(action.equals("com.se4500.displayok"))
				{
					soundPool.play(soundId, 1, 1, 0, 0, 1);
				}
				
			}
		};
		final IntentFilter se4500dispfilter = new IntentFilter();
		se4500dispfilter.addAction("com.se4500.displayok");
		registerReceiver(mDisplayReceiver,se4500dispfilter);
		
                

	}
	 public void onDestroy() {
	 	super.onDestroy();
		//Log.d(TAG, "onDestroyBarcodeService");
		soundPool.release();
		routonScanner.stop(); 
	}

}
