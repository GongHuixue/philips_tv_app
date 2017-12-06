package org.droidtv.oad.services;

import java.util.Calendar;
import java.util.HashMap;
import java.util.Map;
import android.os.RemoteException;
import android.content.BroadcastReceiver;
import android.content.Intent;
import android.content.IntentFilter;
import org.droidtv.tv.media.TvMediaPlayer;
import org.droidtv.tv.media.TvURI.Medium;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Handler;
import android.os.Handler.Callback;
import android.os.Binder;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;
import java.util.ArrayList;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnKeyListener;
import android.view.Window;
import android.view.ViewGroup;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.EOFException;
import java.io.IOException;
import java.util.Arrays;
import android.database.DatabaseUtils;
import java.util.TimeZone;
import java.util.List;
import android.os.Bundle;
import org.droidtv.oad.util.FSM;
import org.droidtv.oad.util.oadMsgHandler;
import android.media.tv.TvInputInfo;
import android.media.tv.TvInputManager;
import android.media.tv.TvInputManager.SessionCallback;
import android.media.tv.TvInputManager.TvInputCallback;
import org.droidtv.tv.tvinput.ITVSessionContext;
import org.droidtv.tv.tvinput.ITVTunerSession;
import android.media.tv.TvInputManager.Session;
import org.droidtv.tv.tvinput.ITVTunerSession.ITunerSessionCallbacks;
import org.droidtv.tv.tvinput.ITVSessionContext.ISessionContextCallbacks;
import android.media.tv.TvContract;
import org.droidtv.tv.tvinput.ITvContractExtras;
import org.droidtv.tv.context.TvIntent;
import android.content.ServiceConnection;

public class OADFSMService extends Service {
	private static final String tag = "OADFSMService";
	private FSM[][] statemachine = null;
	private oadMsgHandler mOADHandler = null;
	private Handler timer_handler = new Handler();
	private TimerRunnable timerRunnable = new TimerRunnable();
    private final int interval = 90000;
	private static boolean isPrefScanCompleted = false;
	private static int signleTuneResult = 0;
	private boolean mFirstScanning = false;
	private Context mContext = this;
	private final IBinder mSvcBinder = new OADFSMServiceBinder();
	private Handler mHandler = null;
	
    private TvInputManager mTvInputManager = null;
	private ObserverSessionCallback observerCallback = null;
	private Handler mSessionHandler = new Handler();
	
    private static final int NONE = 0;
	private static final int PREFFERED = 1;
	private static final int FREQUENCYLIST = 2;

	private static enum CbmhgOadScanEvents {
		CbmhgOadPrefferedScanStart,
		CbmhgOadPrefferedScanTsLocked,
		CbmhgOadPrefferedScanTsNotFound, 
		CbmhgOadPrefferedScanNextTs, 
		CbmhgOadPrefferedScanComplete, 
		CbmhgOadPrefferedScanStop, 
		CbmhgOadFrequencyScanStart, 
		CbmhgOadFrequencyScanTsLocked, 
		CbmhgOadFrequencyScanTsNotFound, 
		CbmhgOadFrequencyScanNextTs, 
		CbmhgOadFrequencyScanComplete, 
		CbmhgOadFrequencyScanStop, 
		CbmhgOadScanMaxEvents
	}

	private static enum CbmhgOadStates {
		CbmhgOadScanIdle, CbmhgOadScanInProgress, CbmhgOadScanStopping, CbmhgOadScanMaxStates;
	}

	private CbmhgOadStates mOadScanState = CbmhgOadStates.CbmhgOadScanIdle;
	private int mOadScanModeState = NONE;

	private void initFSM() {
		Log.d(tag, "Initializing finite State machine");
		statemachine = new FSM[][] {
			    /*******************************     preffered mux scanning process    **********************************/
			    /*                                   CbmhgOadScanIdle          CbmhgOadScanInProgress     CbmhgOadScanStopping */
			    /*CbmhgOadPrefferedScanStart     */  {StartPrefferedScan      ,null                      ,null                 },
				/*CbmhgOadPrefferedScanTsLocked  */  {null                    ,PrefferedTsLocked         ,null                 },
				/*CbmhgOadPrefferedScanTsNotFound*/  {null                    ,PrefferedTsNotFound       ,null                 },
				/*CbmhgOadPrefferedScanNextTs    */  {null                    ,PrefferedScanNextTs       ,null                 },
				/*CbmhgOadPrefferedScanComplete  */  {null                    ,ScanComplete              ,ScanComplete         },
				/*CbmhgOadPrefferedScanStop      */  {null                    ,StopScan                  ,null                 },


				/******************************************************************************************************************/
				/*********************************   frequencyList scanning process      *********************************/
				/*                                   CbmhgOadScanIdle         CbmhgOadScanInProgress     CbmhgOadScanStopping */
				/*CbmhgOadFrequencyScanStart     */  {StartFrequencyListScan   ,null                      ,null                  },
				/*CbmhgOadFrequencyScanTsLocked  */  {null                     ,FrequencyListTsLocked     ,null                  },
				/*CbmhgOadFrequencyScanTsNotFound*/  {null                     ,FrequencyListTsNotFound   ,null                  },
				/*CbmhgOadFrequencyScanNextTs    */  {null                     ,FrequencyListScanNextTs   ,null                  },
				/*CbmhgOadFrequencyScanComplete  */  {null                     ,ScanComplete              ,ScanComplete          },
				/*CbmhgOadFrequencyScanStop      */  {null                     ,StopScan                  ,null                  }
			    /******************************************************************************************************************/
		};
	}

	private void OADFSMServiceInit() {

		Log.d(tag, "Initializing OADFSMService ");
		try {
			initFSM();
			Callback msgCallback = new Callback() {
				@Override
					public boolean handleMessage(Message msg) {
						// TODO Auto-generated method stub
						Log.d(tag, "inside handler to handle Message for value = "	+ msg.arg1);

						try {
							FSM fsm = statemachine[msg.arg1][mOadScanState.ordinal()];

							if (fsm != null) {
								fsm.action();
							}else{
								Log.d(tag, "statemachine is fsm is null");
							}
						} catch (Exception e) {
							Log.d(tag, "Exception on statemachine action: " + e.toString());
						}

						return false;
					}
			};
			mOADHandler = new oadMsgHandler(oadMsgHandler.getLooper("OADFSMService"), msgCallback);

            /*Create TIF Session*/
            {
				mTvInputManager = (TvInputManager) getSystemService(Context.TV_INPUT_SERVICE);
				if (mTvInputManager != null) {
					observerCallback = new ObserverSessionCallback();
					List inputlist = mTvInputManager.getTvInputList();
					if (inputlist != null) {
						for (int i = 0; i < inputlist.size(); i++) {
							TvInputInfo tinfo = (TvInputInfo) inputlist.get(i);
							if ((tinfo.getType() == TvInputInfo.TYPE_TUNER) && 
								(tinfo.getComponent().getPackageName().equals(ITVSessionContext.TUNERSERVICE_PKG_ID))) {
								tunerServiceId = tinfo.getId();
								Log.d(tag, "TunerService ID = " + tunerServiceId);
								break;
							}
						}
					}
					if (mTvInputManager.getInputState(tunerServiceId) == TvInputManager.INPUT_STATE_CONNECTED) {
						Log.d(tag, "creating the session,waiting for call back");
						mTvInputManager.createSession(tunerServiceId, observerCallback, mSessionHandler);
					} else {
						Log.d(tag, "input tuner not added yet,waiting for call back");
					}
				} else {
					Log.d(tag, "Could not get system service TV_INPUT_SERVICE!!");
				}
            }
		} catch (Exception e) {
			e.printStackTrace();
		}
	}


	@Override
		public void onCreate() {
			Log.d(tag, "Inside oncreate");
			mHandler = new Handler();
			if (mHandler != null) {
				mHandler.post(new Runnable() {
					@Override
					public void run() {
					    OADFSMServiceInit();
					}
				});
			}
		}

	@Override
		public IBinder onBind(Intent arg0) {
			Log.d(tag, "Inside onBind");
			return mSvcBinder;
		}

	public class OADFSMServiceBinder extends Binder {
		public OADFSMService getService() {
			Log.d(tag, "inside OADFSMService getService");
			return OADFSMService.this;
		}
	}

	public class ObserverSessionCallback extends SessionCallback {
		Session mySession = null;

		public void onSessionCreated(Session session) {
			Log.d(tag, "onSessionCreated ");
			if (session != null) {
				Log.d(tag, "Session creation successful");
				mySession = session;
				String action = ITVSessionContext.APP_PRIVATE_COMMAND_SESSION_TYPE;
				Bundle bundle = new Bundle();
				bundle.putString(ITVSessionContext.KEY_SESSION_TYPE, ITVSessionContext.SESSION_TYPE_MAIN_OBSERVER);
				mySession.sendAppPrivateCommand(action, bundle);
			} else {
				Log.d(tag, "Session creation failed");
			}
		}

		public void onSessionReleased(Session session) {
			Log.d(tag, "onSessionReleased called");
			mySession = null;
			mObserverSessionContext = null;
		}

		public void onChannelRetuned(Session session, Uri channelUri) {
		}

		public void onVideoUnavailable(Session session, int reason) {
		}

		public void onSessionEvent(Session session, String eventType, Bundle eventArgs) {
			Log.i(tag, "onSessionEvent " + this);
			if (eventType.equals(ITVSessionContext.EVENT_SESSION_CONTEXT_CREATED)) {
				IBinder binder = eventArgs.getBinder(ITVSessionContext.SESSION_CONTEXT);
				mObserverSessionContext = ITVSessionContext.instance.asInterface(binder);
				ObserverSessionContextCallbacks mainSessionContextCallbacks = new ObserverSessionContextCallbacks();
				mObserverSessionContext.registerSessionContextCallbacks(mainSessionContextCallbacks);
			}
		}
	}

    class TimerRunnable implements Runnable {

        public TimerRunnable() {
        }

        @Override
            public void run() {
                Log.d(tag, "Timer expire");
                Message msg = mOADHandler.obtainMessage();
                /*As ts lock set 90s timer for searching oad*/
                if (mOadScanModeState == PREFFERED) {
                    msg.arg1 = CbmhgOadScanEvents.CbmhgOadPrefferedScanStop.ordinal();
                }else if (mOadScanModeState == FREQUENCYLIST) {
                    msg.arg1 = CbmhgOadScanEvents.CbmhgOadFrequencyScanNextTs.ordinal();
                }
                mOADHandler.sendMessage(msg);
            }
    };

	private static int GetNextScanMode(int mode) {
		Log.d(tag, "Inside GetNextScanMode ");
		int retval = NONE;
		if (mode == NONE) {
			retval = PREFFERED;
		} else if (mode == PREFFERED) {
			retval = FREQUENCYLIST;
		} else if (mode == FREQUENCYLIST) {
			retval = NONE;
		}
		return retval;
	}

	private final FSM StartPrefferedScan = new FSM() {
		@Override
			public void action() {
				Log.d(tag, "Inside StartPrefferedScan action");

				if ( (mFirstScanning == false) ) {
                    /*Start Tune Single Freq*/
						/*    oadCallback.tuneSpecialRF();    */
					mFirstScanning = true;
					mOadScanState = CbmhgOadStates.CbmhgOadScanInProgress;
				}
			}
	};

	private final FSM PrefferedScanNextTs = new FSM() {
		@Override
			public void action() {
				Log.d(tag, "Inside PrefferedScanNextTs action"); 
				if( (signleTuneResult == 0)) {
					if (NONE != GetNextScanMode(PREFFERED)) {
						isPrefScanCompleted = true; 
						mOadScanState = CbmhgOadStates.CbmhgOadScanIdle;
						mOadScanModeState = FREQUENCYLIST;
						synchronized (mOADHandler) {
							Message msg = mOADHandler.obtainMessage();
							msg.arg1 = CbmhgOadScanEvents.CbmhgOadFrequencyScanStart.ordinal();
							mOADHandler.sendMessage(msg);
						}
					}
				}
			}
	};

	private final FSM PrefferedTsLocked = new FSM() {
		@Override
			public void action() {
				Log.d(tag, "Inside PrefferedTsLocked action");
				synchronized (timer_handler) {
					Log.d(tag, "Timer Start: interval = " + interval);
					timer_handler.postDelayed(timerRunnable, interval);
				}
			}
	};

	private final FSM PrefferedTsNotFound = new FSM() {
		@Override
			public void action() {
				Log.d(tag, "Inside PrefferedTsNotFound action, don't do anything in this action");
			}
	};

	private final FSM StartFrequencyListScan = new FSM() {

		@Override
			public void action() {
				Log.d(tag, "Inside StartFrequencyListScan action");
				if ( (signleTuneResult == 0) && (isPrefScanCompleted == true) ) {
                        /*Start Full Freq Scan*/
						/*    oadCallback.fullScanFreRF();    */
                }

				mOadScanState = CbmhgOadStates.CbmhgOadScanInProgress;
			}
	};

	private final FSM FrequencyListScanNextTs = new FSM() {
		@Override
			public void action() {
				Uri uri = null;
				boolean ispbsmode = false;
				Log.d(tag, "Inside FrequencyListScanNextTs action");

				/*Start Full Freq Scan*/
				/*    oadCallback.fullScanFreRF();    */
			}
	};

	class Timer_Runnable implements Runnable {

		public Timer_Runnable() {
		}

		@Override
			public void run() {
				Log.d(tag, "Timer expire");
				Message msg = mOADHandler.obtainMessage();
				msg.arg1 = CbmhgOadScanEvents.CbmhgOadFrequencyScanNextTs.ordinal();
				mOADHandler.sendMessage(msg);
			}

	};

	private final FSM FrequencyListTsLocked = new FSM() {
		@Override
			public void action() {
				Log.d(tag, "Inside FrequencyListTsLocked action");
				synchronized (timer_handler) {
					Log.d(tag, "Timer Start: interval = " + interval);
					timer_handler.postDelayed(timerRunnable, interval);
				}
			}
	};

	private final FSM FrequencyListTsNotFound = new FSM() {
		@Override
			public void action() {
				Log.d(tag, "Inside FrequencyListTsNotFound action");
				synchronized (mOADHandler) {
					Message msg = mOADHandler.obtainMessage();
					msg.arg1 = CbmhgOadScanEvents.CbmhgOadFrequencyScanNextTs.ordinal();
					mOADHandler.sendMessage(msg);
				}
			}
	};

	private final FSM ScanComplete = new FSM() {
		@Override
			public void action() {
				Log.d(tag, "Inside ScanComplete action");
				mOadScanState = CbmhgOadStates.CbmhgOadScanIdle;
			}
	};

	private final FSM StopScan = new FSM() {
		@Override
			public void action() {
				Log.d(tag, "Inside StopScan action");
				synchronized (timer_handler) {
					timer_handler.removeCallbacks(timerRunnable);
					Log.d(tag, "Timer Removed");
				}                
				Log.d(tag, "------>OAD Scan State Mechanism, [Stop Scan]<------");
				mOadScanState = CbmhgOadStates.CbmhgOadScanStopping;
			}
	};
}
