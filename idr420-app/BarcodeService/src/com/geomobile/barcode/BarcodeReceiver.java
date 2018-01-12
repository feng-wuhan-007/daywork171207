package com.geomobile.barcode;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BarcodeReceiver extends BroadcastReceiver {
	private  final static  String TAG = "BarcodeReceiver";
	@Override
	public void onReceive(Context context, Intent intent) {
		// TODO Auto-generated method stub
		String action=intent.getAction();
		Log.d(TAG,"fengmcaction="+action);
		if(action.equals("android.intent.action.BOOT_COMPLETED"))
		{
			
			Intent service=new Intent(context,BarcodeService.class);
			context.startService(service);
			Log.d(TAG, "startBarcodeService-service");
		}
	}

}
