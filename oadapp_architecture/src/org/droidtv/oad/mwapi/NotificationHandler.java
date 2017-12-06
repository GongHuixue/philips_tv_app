package org.droidtv.oad.mwapi;

import java.util.Observable;
import java.util.Observer;

import android.util.Log;

public class NotificationHandler extends Observable {
	private static NotificationHandler notificationHandler;
	
	private NotificationHandler(){
	}
	
	public static NotificationHandler getInstance(){
		if(notificationHandler == null){
			notificationHandler = new NotificationHandler();
		}
		return notificationHandler;
	}
	
	public void registerForNotifications(Observer observer){
		addObserver(observer);
	}
	
	public void unregisterFromNotifications(Observer observer){
		deleteObserver(observer);
	}
	
	public void removeAllNotification(){
		deleteObservers();
	}
	
	public void notifyAllObservers(int actionID,int  id)
	{
		setChanged();
		Log.i("NotificationHandler:","message:"+id);
		Log.i("NotificationHandler:","actionID:"+actionID);
		notificationHandler.notifyObservers(new NotificationInfoObject(actionID, id));
	}
}
