package se.dose.netpaint;

import java.io.IOException;
import java.io.InputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.zip.Inflater;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
	
	public boolean connected = false;
//	public GestureDetectorCompat detector;
	int activeTool = ToolConstants.FREEHAND;
	CanvasListener listener = null;
	private static final int SETUP_TOOL = 0x01;
	private static final int SETUP_NETWORK = 0x02;
	private static final int DEBUG_ACTIVITY = 0x03;
	private String debugMessages;
	
	Socket serverSocket;
	String serverIpAddress = "192.168.1.142";
	int port = 1234;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
//		detector = new GestureDetectorCompat(this, new MainGestureListener());		
		NetPaintCanvas canvas = (NetPaintCanvas) findViewById(R.id.canvas);
		canvas.setActiveTool(ToolConstants.FREEHAND);
	}
	
	@Override
	protected void onStart()
	{
		super.onStart();
		connectToServer();
	}
	
	@Override
	protected void onPause()
	{
		super.onPause();
		
		try {
			if(serverSocket != null)
			{
				serverSocket.close();
				serverSocket = null;
			}
		} catch (IOException e) {
		}
		listener.stop = true;
		listener.cancel(true);
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		super.onActivityResult(requestCode, resultCode, data);
		
		/* In case the user didn't actually choose a new tool, just return */
		if(data == null)
			return;
		
		switch(requestCode)
		{
		case SETUP_TOOL:
			activeTool = data.getIntExtra(ToolConstants.TOOL, 0);
			NetPaintCanvas c = (NetPaintCanvas) findViewById(R.id.canvas);
			c.setActiveTool(activeTool);
			break;
		
		case SETUP_NETWORK:
			serverIpAddress = data.getStringExtra(NetworkConstants.IP);
			port = data.getIntExtra(NetworkConstants.PORT, 1234);
			break;
		
		case DEBUG_ACTIVITY:
			// Yeah, the only message we can receive from this activity is to
			// clear the debug messages
			debugMessages = null;
			break;
		
		default:
			break;
		
		}
		
	}
	/*
	public void chooseTool(View v)
	{
		Intent intent = new Intent(this, ChooseToolActivity.class);
		intent.putExtra(ToolConstants.TOOL, activeTool);
		startActivityForResult(intent, CHOOSE_TOOL);
	}
	*/
	
/*
	@Override
	public boolean onTouchEvent(MotionEvent e) {
//		detector.onTouchEvent(e);
		debugMessage("Touch in MainActivity!");
		return true;
	}
	*/
	
	private void connectToServer()
	{
		listener = new CanvasListener();
		listener.execute();
	}
	
	/*
	private class MainGestureListener extends GestureDetector.SimpleOnGestureListener
	{
	       @Override
	        public boolean onDown(MotionEvent event) { 
	        	String str = String.format("down event!");
	        	debugMessage(str);
	            return true;
	        }

	        @Override
	        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX,
	                float distanceY) {
	            String str = "onScroll: " + e1.toString()+e2.toString();
	            debugMessage(str);
	            return true;
	        }

	}
	*/
	private class CanvasListener extends AsyncTask<Void, String, Void>
	{
		byte[] inBuffer;
		byte[] uBuffer;
		int canvasWidth;
		int canvasHeight;
		int protocol_type;
		int[] imageBuffer;
		public boolean stop;
		
		@Override
		protected Void doInBackground(Void... args) {
			try {

				InetAddress address = InetAddress.getByName(serverIpAddress);
				
				serverSocket = new Socket(address, port);
				publishProgress("We have a socket!");
				NetPaintCanvas canvas = (NetPaintCanvas) findViewById(R.id.canvas);
				canvas.setServerSocket(serverSocket);

				InputStream is = serverSocket.getInputStream();

				while(!stop)
				{
					/*
					 * Start by reading the first four bytes. It indicates the size
					 * of the message
					 */
					inBuffer = new byte[4];
					/*int bytesRead = */is.read(inBuffer, 0, 4);
					ByteBuffer bb = ByteBuffer.wrap(inBuffer);
					int messageSize = bb.getInt();
					publishProgress(String.format("Message size: %d", messageSize));
					if(messageSize == 0)
						break;
					/*
					 * Since we now know how much to read, read the rest of the
					 * message.
					 */
					inBuffer = new byte[messageSize - 4];

					int total = 0;
					int bytesLeft = messageSize - 4;
					int bytesReceived;
					while(bytesLeft > 0)
					{
						bytesReceived = is.read(inBuffer, total, bytesLeft);
						if(bytesReceived == -1){
							publishProgress(String.format("ERROR while reading data!"));
							break;
						}
						total += bytesReceived;
						bytesLeft -= bytesReceived;
					}
					
					publishProgress(String.format("Read %d bytes of data", total + 4));
					
					/*
					 * Interpret the next three ints according to the protocol
					 */
					bb = ByteBuffer.wrap(inBuffer);
					protocol_type = bb.getInt();
					publishProgress(String.format("protocol type: %d", protocol_type));
					canvasWidth = bb.getInt();
					publishProgress(String.format("canvas width: %d", canvasWidth));
					canvasHeight = bb.getInt();
					publishProgress(String.format("canvas height: %d", canvasHeight));

					/*
					 * The rest is the canvas
					 */
					Inflater inflater = new Inflater();
					int byteCount = 4 * canvasWidth * canvasHeight;
					inflater.setInput(inBuffer, 16, messageSize - 20);	// The start of the compressed canvas is 20 bytes into the message
					uBuffer = new byte[byteCount];
					inflater.inflate(uBuffer);
					publishProgress(String.format(inflater.finished() ?
							"Inflation completed!" : "Inflation NOT completed!"));
					
					// Codeword to update canvas instead of actually publishing the message!
					publishProgress("NEW_CANVAS");
				} // while(!stop)
				serverSocket.close();
				serverSocket = null;

				finish();
			} catch (Exception e) {
				publishProgress("ERROR!");
				publishProgress(e.toString());
				if(serverSocket != null)
					try {
						serverSocket.close();
						serverSocket = null;
					} catch (IOException e1) {
				}
				//finish();
			}
			return null;
		}
		
		protected void onProgressUpdate(String... strings)
		{
			if(!strings[0].equals("NEW_CANVAS"))
			{
				debugMessage(strings[0]);
				return;
			}
			Bitmap bitmap = Bitmap.createBitmap(canvasWidth, canvasHeight, Bitmap.Config.ARGB_8888);
			imageBuffer = new int[canvasWidth * canvasHeight];
			/*
			 * FIXME!!! This is just... wrong. RGB values ought to be sent on
			 * the network in network endian, naturally!
			 */
			ByteBuffer bb = ByteBuffer.wrap(uBuffer).order(ByteOrder.LITTLE_ENDIAN);
			for(int i = 0; i < canvasWidth * canvasHeight; i++)
				imageBuffer[i] = bb.getInt();
			bitmap.setPixels(imageBuffer, 0, canvasWidth, 0, 0, canvasWidth, canvasHeight);
			NetPaintCanvas c = (NetPaintCanvas) findViewById(R.id.canvas);
			c.setBitmap(bitmap);
		}
		protected void onPostExecute(Void arg) {
			Toast.makeText(getApplicationContext(), "Disconnected!", Toast.LENGTH_LONG).show();
		}
	} // class CanvasListener
	
	
	private void debugMessage(String string)
	{
		debugMessages = string + '\n' + debugMessages;
	}


	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.activity_main, menu);
		return true;
	}
	
	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		switch(item.getItemId())
		{
		case R.id.menu_tools:
			Intent intentTool = new Intent(this, ChooseToolActivity.class);
			intentTool.putExtra(ToolConstants.TOOL, activeTool);
			startActivityForResult(intentTool, SETUP_TOOL);
			return true;
		
		case R.id.menu_debug:
			Intent intentDebug = new Intent(this, DebugActivity.class);
			intentDebug.putExtra("DEBUG", debugMessages);
			startActivityForResult(intentDebug, DEBUG_ACTIVITY);
			return true;

		case R.id.menu_settings:
			Intent intentNetwork = new Intent(this, SetupNetworkActivity.class);
			intentNetwork.putExtra(NetworkConstants.IP, serverIpAddress);
			intentNetwork.putExtra(NetworkConstants.PORT, port);
			startActivityForResult(intentNetwork, SETUP_NETWORK);
			return true;
			
		default:
			return super.onOptionsItemSelected(item);
		
		}
	}
}
