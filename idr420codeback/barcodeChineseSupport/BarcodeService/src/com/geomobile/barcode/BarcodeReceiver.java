package com.geomobile.barcode;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BarcodeReceiver extends BroadcastReceiver {
	private  final static  String TAG = "BarcodeReceiver";
	private static boolean DEBUG=true;
	
	@Override
	public void onReceive(Context context, Intent intent) {
		// TODO Auto-generated method stub
		
		String action = intent.getAction();
		Log.d(TAG,"fengmc180123="+action);
		
		final Intent intentservice=new Intent(context,BarcodeService.class);
		//final Intent intentservice = new Intent();
		//intentservice.setAction("com.geomobile.barcode.BCS");
		if(action.equals("android.intent.action.BOOT_COMPLETED"))
		{
			
			
			context.startService(intentservice);
			if(DEBUG)  Log.d(TAG, "start BarcodeService");
		}
		if(action.equals("scanner_service_pause"))//stop BarcodeService
		{
			context.stopService(intentservice);
			if(DEBUG)  Log.d(TAG, "stop BarcodeService");
		}
		if(action.equals("scanner_service_resume"))//start BarcodeService
		{
			context.startService(intentservice);
			if(DEBUG)  Log.d(TAG, "restart BarcodeService");
		}
	}

}
