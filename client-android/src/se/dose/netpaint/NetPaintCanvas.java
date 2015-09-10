package se.dose.netpaint;

import java.net.Socket;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

/**
 * The Canvas
 */
public class NetPaintCanvas extends View {

	private Bitmap image;
	private Socket serverSocket = null;

	private Tool activeTool;
	private LineTool lineTool = null;
	private FreehandTool freehandTool = null;
	private CircleTool circleTool = null;
	private RectangleTool rectangleTool = null;

	public NetPaintCanvas(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}
	
	public void setActiveTool(int tool)
	{
		switch(tool) {
		case ToolConstants.LINE:
			activeTool = lineTool;
			break;
		case ToolConstants.FREEHAND:
			activeTool = freehandTool;
			break;
		case ToolConstants.CIRCLE:
			activeTool = circleTool;
			break;
		case ToolConstants.RECTANGLE:
			activeTool = rectangleTool;
			break;
		default:
			break;
		}
		
	}
	public void setServerSocket(Socket s)
	{
		serverSocket = s;
		lineTool.setServerSocket(serverSocket);
		freehandTool.setServerSocket(serverSocket);
		circleTool.setServerSocket(serverSocket);
		rectangleTool.setServerSocket(serverSocket);
	}
	
	@Override
	public boolean onTouchEvent(MotionEvent e)
	{
		if(e.getAction() == MotionEvent.ACTION_DOWN)
		{
			activeTool.startPainting((int) e.getX(), (int) e.getY());
		}
		else if(e.getAction() == MotionEvent.ACTION_UP)
		{
			activeTool.finishPainting((int) e.getX(), (int) e.getY());
		}
		else if(e.getAction() == MotionEvent.ACTION_MOVE)
		{
			activeTool.move((int) e.getX(), (int) e.getY());
		}
		return true;
	}

	
	public NetPaintCanvas(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		init();
	}

	public NetPaintCanvas(Context context) {
		super(context);
		init();
	}
	
	private void init()
	{
		/* Just to have something to start with... */
		image = Bitmap.createBitmap(320, 200, Bitmap.Config.ARGB_8888);
		lineTool = new LineTool();
		freehandTool = new FreehandTool();
		circleTool = new CircleTool();
		rectangleTool = new RectangleTool();
	}

	@Override
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		canvas.drawBitmap(image, 0, 0, null);
	}


	/**
	 * Sets the view's bitmap image
	 * Also updates the image
	 */
	public void setBitmap(Bitmap image) {
		this.image = image;
		invalidate();
		lineTool.setImageSize(image.getWidth(), image.getHeight());
		freehandTool.setImageSize(image.getWidth(), image.getHeight());
		circleTool.setImageSize(image.getWidth(), image.getHeight());
		rectangleTool.setImageSize(image.getWidth(), image.getHeight());
	}
	
}
