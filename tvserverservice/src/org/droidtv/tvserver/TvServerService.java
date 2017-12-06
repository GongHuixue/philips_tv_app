package org.droidtv.tvserver;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;

public class TvServerService extends Service {
	private static final String TAG = "Deepak: TvServerService";
	private boolean isTvServerStarted = false;
    private TvServerServiceHandler mTvserverHandler = null;

    private static final int StartTvserverService = 1;

    public TvServerService () {
        HandlerThread thread = new HandlerThread("TvServerServiceLooper");
		thread.start();
        mTvserverHandler = new TvServerServiceHandler(thread.getLooper());
    }

    private class TvServerServiceHandler extends Handler {
		public TvServerServiceHandler(Looper looper) {
			super(looper);
		}

        public synchronized void handleMessage(Message msg) {
            if (msg.arg1 == StartTvserverService) {
                Log.d(TAG, "Creating DB wrapper");
                Log.d(TAG, "onBind:startTvServer");
                startTvServer();
                isTvServerStarted = true;
            }
        }
    };

	private native void startTvServer();

	static {
		try {
			System.loadLibrary("TvMiddlewareCore");
			System.loadLibrary("tvserver");
		} catch (UnsatisfiedLinkError e) {
			Log.e(TAG, "cannot load library due to " + e.getLocalizedMessage());
		}
	}

	@Override
	public IBinder onBind(Intent intent) {
		Log.d(TAG, "onBind > isTvServerStarted: " + isTvServerStarted);
		if (isTvServerStarted == false) {
            Message     msg = new Message();
            msg.arg1 = StartTvserverService;
            mTvserverHandler.sendMessage(msg);
		}
		return null;
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		super.onStartCommand(intent, flags, startId);
		Log.d(TAG, "onStartCommand > isTvServerStarted:" + isTvServerStarted);
		if (isTvServerStarted == false) {
            Message     msg = new Message();
            msg.arg1 = StartTvserverService;
            mTvserverHandler.sendMessage(msg);
		}
		return Service.START_STICKY;
	}
}
