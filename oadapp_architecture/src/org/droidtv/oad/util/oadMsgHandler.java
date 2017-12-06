package org.droidtv.oad.util;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;

public class oadMsgHandler extends Handler{
	
	public oadMsgHandler(Looper looper, Callback cb) {
		super(looper,cb);
	}

	public oadMsgHandler(Callback cb) {
		super(cb);
	}

	public static Looper getLooper(String threadname) {
		try {
			HandlerThread thread = new HandlerThread(threadname, android.os.Process.THREAD_PRIORITY_BACKGROUND);
			thread.start();
			return thread.getLooper();
		}
		catch(Exception e) {
			return null;
		}
	}
	
	public static Looper getLooper(String threadname,int priority) {
		try {
			HandlerThread thread = new HandlerThread(threadname, priority);
			thread.start();
			return thread.getLooper();
		}
		catch(Exception e) {
			return null;
		}
	}
}
