package com.geomobile.BarcodeService;

import java.io.IOException;
import java.io.UnsupportedEncodingException;

import android.content.Intent;
import android.content.Context;
import android.app.Service;
import android.os.IBinder;
import android.provider.Settings;
import android.view.KeyEvent;
import android.view.inputmethod.InputMethodInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.EditorInfo;
import android.inputmethodservice.InputMethodService;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Handler;
import android.os.Message;
import android.hardware.SerialPort;
import android.hardware.SerialManager;
import android.os.SystemProperties;
import android.util.Log;
import java.util.List;
import android.os.Vibrator;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemClock;
import android.util.Log;
import android.hardware.input.InputManager;
import android.view.InputDevice;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.File;

public class BarcodeService extends Service{ //InputMethodService {
    /** Called when the activity is first created. */
	
	private static final String TAG = "SerialPort";
	private static final String SERIAL_SERVICE = "serial";
	private SerialManager mSerialManager = null;
	private SerialPort mSerialPort = null;
	private ReadThread mReadThread;
	private Handler handler = null;
	private SoundPool soundPool;
	private Vibrator vibrator;
	final long[] pattern = {10, 250, 50, 250}; // OFF/ON/OFF/ON
	private	int soundId;
	private int inputSource;
	private BroadcastReceiver mDisplayReceiver;
	private String readstr = new String();
	private byte[] tmpbuf = new byte[1024];
	private int readed = 0;

	@Override
	public IBinder onBind(Intent intent){
		// TODO Auto-generate method stub
		return null;
	}

	private boolean isUTF8(byte[] sx){
		Log.d(TAG, "begian to set codeset");
		for(int i = 0; i < sx.length; )
		{
			if(sx[i] < 0)
			{
				if((sx[i]>>>5) == 0x7FFFFFE)
				{
					if(((i + 1) < sx.length) && ((sx[i + 1]>>>6) == 0x3FFFFFE))
					{
						i = i + 2;
					}
					else
					{
						return false;
					}
				}
				else if((sx[i]>>>4) == 0xFFFFFFE)
				{
					if(((i + 2) < sx.length) && ((sx[i + 1]>>>6) == 0x3FFFFFE) && ((sx[i + 2]>>>6) == 0x3FFFFFE))
					{
						i = i + 3;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				i++;
			}
		}
		return true;
	}

	@Override
	public void onCreate()
	{
        super.onCreate();

        soundPool = new SoundPool(1, AudioManager.STREAM_MUSIC, 0);
        soundId = soundPool.load("/system/media/audio/ui/VideoRecord.ogg", 0);
	vibrator = (Vibrator) getSystemService(VIBRATOR_SERVICE);
	inputSource = InputDevice.SOURCE_KEYBOARD;
	handler = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			if(msg.what == 1){
				try{
					//`if(SystemProperties.get("persist.sys.startscan").equals("true")){
					if(SystemProperties.get("spda.sys.startscan").equals("true")){
						String sendstr = (String)msg.obj;
						Intent intent = new Intent("com.se4500.onDecodeComplete");
						intent.putExtra("se4500", sendstr);

						sendBroadcast(intent);
					}
				} catch(Exception e){
					//Log.i(TAG,e.getMessage());
				}
			}
		}
	};
	
	mSerialManager = (SerialManager)getSystemService(SERIAL_SERVICE);
	String[] ports = mSerialManager.getSerialPorts();
	if(ports != null && ports.length > 0){
		try{
			mSerialPort = mSerialManager.openSerialPort(ports[2],9600,8,1,0);	//jshli
			if(mSerialPort != null){
				Log.d(TAG,"open SerialPort success");
				mReadThread = new ReadThread();
				mReadThread.start();
			}
		}catch(IOException e){
		
		}
	}else{
		Log.d(TAG,"ports == null");
		soundPool.release();
	}


	mDisplayReceiver = new BroadcastReceiver(){
		@Override
		public void onReceive(Context context, Intent intent){
			String action = intent.getAction();
                        Log.d(TAG, "hehj act=" + action);
			if(action.equals("com.se4500.displayok")){
				if(SystemProperties.get("persist.sys.playscanmusic").equals("true")){
					soundPool.play(soundId, 1, 1, 0, 0, 1);
				}
				if(SystemProperties.get("persist.sys.scanvibrate").equals("true")){
					vibrator.vibrate(pattern, -1);
				}
			}else if(action.equals("com.geomobile.se4500barcode")){ 
				try {
					File ScanDeviceName = new File("proc/driver/scan");
					BufferedWriter ScanCtrlFileWrite = new BufferedWriter(new FileWriter(ScanDeviceName, false));
					ScanCtrlFileWrite.write("on");
					ScanCtrlFileWrite.flush();

					android.os.SystemProperties.set("persist.sys.startscan", "true");

					ScanCtrlFileWrite.write("trig");
					ScanCtrlFileWrite.flush();
					ScanCtrlFileWrite.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}else if(action.equals("com.geomobile.se4500barcode.poweroff")) {
				try {
					File ScanDeviceName = new File("proc/driver/scan");
					BufferedWriter ScanCtrlFileWrite = new BufferedWriter(new FileWriter(ScanDeviceName, false));
					ScanCtrlFileWrite.write("off");
					ScanCtrlFileWrite.flush();

					android.os.SystemProperties.set("persist.sys.startscan", "false");

					ScanCtrlFileWrite.write("trigoff");
					ScanCtrlFileWrite.flush();
					ScanCtrlFileWrite.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}else if(action.equals(Intent.ACTION_SCREEN_OFF)){
				try {
					File ScanDeviceName = new File("proc/driver/scan");
					BufferedWriter ScanCtrlFileWrite = new BufferedWriter(new FileWriter(ScanDeviceName, false));
					ScanCtrlFileWrite.write("off");
					ScanCtrlFileWrite.flush();

					android.os.SystemProperties.set("persist.sys.startscan", "false");

					ScanCtrlFileWrite.write("trigoff");
					ScanCtrlFileWrite.flush();
					ScanCtrlFileWrite.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			} else if(action.equals("scanner_service_pause")) {
                            mReadThread.pause(true);
                        } else if(action.equals("scanner_service_resume")) {
                            mReadThread.pause(false);
                        }
		}
	};
	final IntentFilter se4500dispfilter = new IntentFilter();
	se4500dispfilter.addAction("com.se4500.displayok");
	se4500dispfilter.addAction("com.geomobile.se4500barcode");
	se4500dispfilter.addAction("com.geomobile.se4500barcode.poweroff");
	se4500dispfilter.addAction(Intent.ACTION_SCREEN_OFF);
	se4500dispfilter.addAction("scanner_service_pause");
	se4500dispfilter.addAction("scanner_service_resume");
	registerReceiver(mDisplayReceiver,se4500dispfilter);
    }

    public void onDestroy() {
		soundPool.release();
		mReadThread.interrupt();
		try {
			mSerialPort.close();

			File ScanDeviceName = new File("proc/driver/scan");
			BufferedWriter ScanCtrlFileWrite = new BufferedWriter(new FileWriter(ScanDeviceName, false));
			ScanCtrlFileWrite.write("off");
			ScanCtrlFileWrite.flush();

			android.os.SystemProperties.set("persist.sys.startscan", "false");

			ScanCtrlFileWrite.write("trigoff");
			ScanCtrlFileWrite.flush();
			ScanCtrlFileWrite.close();
		}catch(IOException e){
			e.printStackTrace();
    		}
		unregisterReceiver(mDisplayReceiver);

		super.onDestroy();
    }

    private class ReadThread extends Thread {

          private Object mPauseLock = new Object();;
          private boolean mPaused = false;

          public void pause(boolean p) {
              if (p) {
                  mPaused = true;
              }
              else {
                  synchronized (mPauseLock) {
                      mPaused = false;
                      mPauseLock.notifyAll();
                  }
              }
          }

		@Override
		public void run() {
			super.run();
			while(!isInterrupted()) {
				try {
                                    synchronized (mPauseLock) {
                                        while (mPaused) {
                                            try {
                                                Log.d(TAG, "hehj paused !!");
                                                mPauseLock.wait();
                                            } catch (InterruptedException e) {
                                            }
                                        }
                                    }

					try {
						readed = mSerialPort.read_byte(tmpbuf,1024);
					} catch (IOException e){
						e.printStackTrace();
					}

					if(readed > 0){

						/*int striped = 0
						if(tmpbuf[readed - 1] == 0xA)
						{
							striped++;
							if((readed > 1) && (tmpbuf[readed - 2] == 0xD))
							{
								striped++;
							}
						}

						readed -= striped;*/

						byte[] readbuf = new byte[readed];
						System.arraycopy(tmpbuf,0,readbuf,0, readed);
						if(isUTF8(readbuf))
						{
							readstr = new String(readbuf, "utf8");
							Log.d(TAG, "is a utf8 string");	
						}
						else
						{
							readstr = new String(readbuf, "gbk");
							Log.d(TAG, "is a gbk string");	
						}

						if(readstr.endsWith("\n"))
						{
							if(readstr.endsWith("\r\n"))
							{
								readstr = readstr.substring(0, readstr.length() - 2);
							}
							else
							{
								readstr = readstr.substring(0, readstr.length() - 1);
							}
						}
						else if(readstr.endsWith("\r"))
						{
							if(readstr.endsWith("\n\r"))
							{
								readstr = readstr.substring(0, readstr.length() - 2);
							}
							else
							{
								readstr = readstr.substring(0, readstr.length() - 1);
							}
						}

						if(readstr != null){
							Message msg = new Message();
							msg.what = 1;
							msg.obj = readstr;
							handler.sendMessage(msg);
						}
					}
				} catch (SecurityException e) {
					e.printStackTrace();
				} catch (UnsupportedEncodingException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
    }
}
