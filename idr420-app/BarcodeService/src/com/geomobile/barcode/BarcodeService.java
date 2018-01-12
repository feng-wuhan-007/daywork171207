//fengmc 180112 new project
package com.geomobile.barcode;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.media.SoundPool;
import android.os.Handler;
import android.os.Message;

//import com.routon.scanner.RoutonScanner;  
//import com.routon.scanner.RoutonScanner.ScannerCallback;
import android.hardware.RoutonScanner;//fengmc add 171226     180104
import android.hardware.RoutonScanner.ScannerCallback;//fengmc 171226  180104
public class BarcodeService extends Service {
	private final static String TAG="BarcodeServcie-China";
	private int soundId;
	private SoundPool soundPool;
	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}
	public static final int SCAN_RESULT=1;
	RoutonScanner routonScanner;
	 private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case SCAN_RESULT:
                    Log.d(TAG,"fenmgcchina-180112");
                    Log.d(TAG, "[msg]" + msg.obj);
                    //soundPool.play(soundId, 1, 1, 0, 0, 1);
                    break;
            }
        }
    };

	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		super.onCreate();
		Log.d(TAG,"Success enter in BarcodeService!");
		
		//soundPool.play(soundId, 1, 1, 0, 0, 1);
		 ScannerCallback cb = new ScannerCallback() {
		    @Override  
		    public void onMessage(String msg) { 
			//soundId = soundPool.load("/system/media/audio/ui/VideoRecord.ogg", 0); 
		        Log.d(TAG,"fengmcchina180112");
		        Log.e(TAG, "[MSG]" + msg);
		        mHandler.sendMessage(mHandler.obtainMessage(SCAN_RESULT, msg));
		       // soundPool.play(soundId, 1, 1, 0, 0, 1);
		    }  
		};
        routonScanner = new RoutonScanner(cb);
	routonScanner.start();
	}
}
