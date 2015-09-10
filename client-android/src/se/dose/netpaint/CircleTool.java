package se.dose.netpaint;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Style;

public class CircleTool extends Tool {
	private int startX = 0, startY = 0, endX = 0, endY = 0;
	private boolean isPainting = false;
	
	public void startPainting(int x, int y)
	{
		startX = x;
		startY = y;
		isPainting = true;
	}
	public void finishPainting(int x, int y)
	{
		if(!isPainting)
			return;
		isPainting = false;
		endX = x;
		endY = y;
		createImage();
	}
	
	private class CreateImageThread extends Thread
	{

		@Override
		public void run() {
			Bitmap tmpImage = Bitmap.createBitmap(imageWidth, imageHeight, Bitmap.Config.ARGB_8888);

			// Do painting
			Canvas canvas = new Canvas(tmpImage);
			Paint p = new Paint();
			p.setColor(color);
			p.setStrokeWidth(strokeWidth);
			p.setStyle(Style.STROKE);

			// Calculate the radius
			int dx = endX - startX;
			int dy = endY - startY;

			int r = (int) Math.sqrt(dx*dx + dy*dy);
			canvas.drawCircle(startX, startY, r, p);
			sendImage(tmpImage);
			
			return;
		}
	}
	
	private void createImage()
	{
		CreateImageThread thread = new CreateImageThread();
		thread.start();
		
	}

}
