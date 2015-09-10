package se.dose.netpaint;

import java.util.Vector;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Point;

public class FreehandTool extends Tool {

	Vector<Point> points;
	boolean isPainting = false;
	
	public FreehandTool() {
		points = new Vector<Point>();
	}
	
	@Override
	public void startPainting(int x, int y) {
		isPainting = true;
		points.clear();
		points.add(new Point(x, y));
	}

	@Override
	public void finishPainting(int x, int y) {
		isPainting = false;
		createImage();
	}
	
	public void move(int x, int y)
	{
		points.add(new Point(x ,y));
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

			// draw a line onto the canvas
			for(int i = 0; i < points.size() - 1; i++)
				canvas.drawLine(points.get(i).x, points.get(i).y, points.get(i + 1).x, points.get(i + 1). y, p);
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
