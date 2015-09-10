package se.dose.netpaint;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.zip.Deflater;
import android.graphics.Bitmap;
import android.util.Log;
import android.graphics.Color;

public abstract class Tool {
	
	protected Socket serverSocket = null;
	protected int imageWidth = 0;
	protected int imageHeight = 0;
	protected int strokeWidth = 1;
	protected int color = Color.BLUE;
	
	public abstract void startPainting(int x, int y); // Must be implemented!
	public abstract void finishPainting(int x, int y); // Mandatory implementation!
	public void move(int x, int y) {}		// Some tools don't need this one, don't make it abstract
	public void setServerSocket(Socket socket){
		serverSocket = socket;
	}
	
	public void setStrokeWidth(int w)
	{
		strokeWidth = w;
	}

	public void setColor(int c)
	{
		color = c;
	}
	
	public void setImageSize(int w, int h)
	{
		imageWidth = w;
		imageHeight = h;
	}
	
	
	protected void sendImage(Bitmap tmpImage)
	{
		// Put the image in an int array
		int[] intBuffer = new int[imageWidth * imageHeight];
		tmpImage.getPixels(intBuffer, 0, imageWidth, 0, 0, imageHeight, imageHeight);
		
		// Read that int array into a byte array
		byte[] uBuffer = new byte[imageWidth * imageHeight * 4];
		ByteBuffer bb = ByteBuffer.wrap(uBuffer).order(ByteOrder.LITTLE_ENDIAN);
		for(int i = 0; i < imageWidth * imageHeight; i++)
			bb.putInt(intBuffer[i]);

		// Compress the byte array into a new byte array
		Deflater deflater = new Deflater();
		deflater.setInput(uBuffer);
		deflater.finish();
		
		ByteArrayOutputStream baos = new ByteArrayOutputStream();
		byte[] buf = new byte[imageWidth * imageHeight * 4]; // Waay too large!
		int byteCount = 0;
		while (!deflater.finished()) {
	         byteCount = deflater.deflate(buf);
	         baos.write(buf, 0, byteCount);
	    }
		deflater.end();
		byte cBuffer[] = baos.toByteArray();
		
		int messageSize = byteCount + 20;
		byte[] sendBuffer = new byte[messageSize];
		bb = ByteBuffer.wrap(sendBuffer);
		bb.putInt(messageSize);
		bb.putInt(2); //Protocol number
		bb.putInt(imageWidth);
		bb.putInt(imageHeight);
		bb.putInt(byteCount);
		bb.put(cBuffer);

		// Send everything to the server
		try {
			OutputStream os = serverSocket.getOutputStream();
			os.write(sendBuffer, 0, messageSize);
		} catch (IOException e) {
			Log.v("Tool: ", "IOException " + e);	
		}

	}
}
