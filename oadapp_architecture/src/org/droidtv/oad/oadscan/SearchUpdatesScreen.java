package org.droidtv.oad.oadscan;

import java.util.Observable;
import org.droidtv.ui.tvwidget2k15.dialog.ModalDialog;
import org.droidtv.ui.tvwidget2k15.dialog.ModalDialogFooterButtonProp;
import org.droidtv.ui.tvwidget2k15.dialog.ModalDialogInterface;
import org.droidtv.ui.tvwidget2k15.dialog.ModalDialogInterface.OnClickListener;
import org.droidtv.ui.tvwidget2k15.wizardframework.WizardStep;
import org.droidtv.ui.tvwidget2k15.wizardframework.common.SimpleText;
import org.droidtv.oad.mwapi.NotificationHandler;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.widget.PopupWindow;
import android.widget.ProgressBar;
import org.droidtv.oad.util.*;

public class SearchUpdatesScreen extends WizardStep implements OADInterface{
	private Context mContext;
	private static final String TAG = SearchUpdatesScreen.class.getSimpleName();
	private org.droidtv.oad.mwapi.NotificationHandler mNotificationHandler;
	
	public SearchUpdatesScreen(Context context) {
		this(context,null,0);
		// TODO Auto-generated constructor stub
	}

	public SearchUpdatesScreen(Context context, AttributeSet attr) {
		this(context, attr,0);
		// TODO Auto-generated constructor stub
	}

	public SearchUpdatesScreen(Context context, AttributeSet attr, int defstyl) {
		super(context, attr, defstyl);
		// TODO Auto-generated constructor stub
        mContext = context;
		// TODO Auto-generated constructor stub
	}

	
	@Override
	public void screenIntialization() 
	{
		Log.i(TAG, "screenIntialization");
		mNotificationHandler = org.droidtv.oad.mwapi.NotificationHandler.getInstance();
		registerForNotifications();
	}

	
	@Override
	public void update(Observable observable, Object data) { }

	@Override
	public boolean onKey(View view, int keyCode, KeyEvent event) 
	{
		Log.d(TAG, "OnKey");
		super.onKey(view, keyCode, event);
		return true;
	}

	// Registering for notifications 
	private void registerForNotifications() 
	{
		mNotificationHandler.registerForNotifications(this);
	}

	//unregister
	private void unregisterForNotifications() 
	{
		mNotificationHandler.unregisterFromNotifications(this);
	}
}
