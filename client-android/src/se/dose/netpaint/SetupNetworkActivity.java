package se.dose.netpaint;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;

public class SetupNetworkActivity extends Activity {
	
	private String serverIpAddress;
	private int port;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_setup_network);
		
		Intent intent = getIntent();
		serverIpAddress = intent.getStringExtra(NetworkConstants.IP);
		port = intent.getIntExtra(NetworkConstants.PORT, 1234);
		
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.setup_network, menu);
		return true;
	}
	
	public void done(View v)
	{
		EditText serverIpEditText = (EditText) findViewById(R.id.serverIpAddressInput);
		serverIpAddress = serverIpEditText.getText().toString();
		
		EditText portEditText = (EditText) findViewById(R.id.serverPortInput);
		// FIXME: Make sure this is valid
		port = Integer.parseInt(portEditText.getText().toString());
		
		Intent intent = new Intent();
		intent.putExtra(NetworkConstants.IP, serverIpAddress);
		
		intent.putExtra(NetworkConstants.PORT, port);
		setResult(Activity.RESULT_OK, intent);
		finish();
	}
	
}
