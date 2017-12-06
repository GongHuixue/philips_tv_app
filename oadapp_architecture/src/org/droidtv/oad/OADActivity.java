package org.droidtv.oad;

import org.droidtv.oad.services.OADFSMService;
import org.droidtv.oad.services.OADFSMService.OADFSMServiceBinder;

import android.app.Activity;
import android.app.Fragment;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.KeyEvent;

public class OADActivity extends Activity {
	OADFSMService mService;
	private String TAG = this.getClass().getSimpleName();
	Intent mIntent;

	@Override
		protected void onCreate(Bundle savedInstanceState) {
			// TODO Auto-generated method stub
			super.onCreate(savedInstanceState);
			setContentView(R.layout.oadmsgactivity);
			mIntent = getIntent();
			bindService(new Intent("org.droidtv.oad.services.OADFSMService"),
					mConnection, Context.BIND_AUTO_CREATE);
		}

	@Override
		protected void onNewIntent(Intent intent) {
			// TODO Auto-generated method stub
			super.onNewIntent(intent);
			mIntent = intent;
			if(mService==null){
				bindService(new Intent("org.droidtv.oad.services.OADFSMService"),mConnection, Context.BIND_AUTO_CREATE);
			}
		}


	@Override
		protected void onStart() {
			// TODO Auto-generated method stub
			super.onStart();
		}

	@Override
		protected void onStop() {
			// TODO Auto-generated method stub
			Log.d(TAG, "onStop");
			super.onStop();
		}

	@Override
		protected void onDestroy() {
			Log.d(TAG,"onDestory called");
			if (mService != null ) {
				unbindService(mConnection);
			}
			super.onDestroy();        
		}

	private ServiceConnection mConnection = new ServiceConnection() {
		// Called when the connection with the service is established
		public void onServiceConnected(ComponentName className, IBinder service) {
			// Because we have bound to an explicit
			// service that is running in our own process, we can
			// cast its IBinder to a concrete class and directly access it.
			OADFSMServiceBinder binder = (OADFSMServiceBinder) service;
			mService = binder.getService();
		}

		// Called when the connection with the service disconnects unexpectedly
		public void onServiceDisconnected(ComponentName className) {
			Log.e(TAG, "onServiceDisconnected");
			mService = null;
		}
	};
}
