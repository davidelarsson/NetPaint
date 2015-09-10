package se.dose.netpaint;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.widget.TextView;

public class DebugActivity extends Activity {
	
	String debugMessages;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_debug);
		
		Intent intent = getIntent();
		debugMessages = intent.getStringExtra("DEBUG");
		TextView v = (TextView) findViewById(R.id.debugOutputTextView);
		v.setText(debugMessages);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.debug, menu);
		return true;
	}
	
	public void clearMessages(View v)
	{
		Intent intent = new Intent();
		setResult(Activity.RESULT_OK, intent);
		finish();
	}

}
