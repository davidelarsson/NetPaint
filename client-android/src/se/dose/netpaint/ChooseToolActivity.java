package se.dose.netpaint;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.NavUtils;
import android.view.MenuItem;
import android.view.View;
import android.widget.ToggleButton;

public class ChooseToolActivity extends Activity {
	
	private int activeTool;
	
	@SuppressLint("NewApi")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.choose_tool_activity);
		
		Intent intent = getIntent();
		activeTool = intent.getIntExtra(ToolConstants.TOOL, 0);
		
		ToggleButton button = null;
		switch(activeTool) {
		case ToolConstants.FREEHAND:
			 button = (ToggleButton) findViewById(R.id.buttonFreehand);
			 break;
		case ToolConstants.LINE:
			 button = (ToggleButton) findViewById(R.id.buttonLine);
			 break;
		case ToolConstants.CIRCLE:
			 button = (ToggleButton) findViewById(R.id.buttonCircle);
			 break;
		case ToolConstants.RECTANGLE:
			 button = (ToggleButton) findViewById(R.id.buttonRectangle);
			 break;
		}
		button.setChecked(true);
	}
	
	public void buttonFreehandClicked(View v)
	{
		activeTool = ToolConstants.FREEHAND;
		Intent intent = new Intent();
		intent.putExtra(ToolConstants.TOOL, activeTool);
		setResult(Activity.RESULT_OK, intent);

		((ToggleButton) findViewById(R.id.buttonFreehand)).setChecked(true);
		((ToggleButton) findViewById(R.id.buttonLine)).setChecked(false);
		((ToggleButton) findViewById(R.id.buttonCircle)).setChecked(false);
		((ToggleButton) findViewById(R.id.buttonRectangle)).setChecked(false);
		finish();
	}

	public void buttonLineClicked(View v)
	{
		activeTool = ToolConstants.LINE;
		Intent intent = new Intent();
		intent.putExtra(ToolConstants.TOOL, activeTool);
		setResult(Activity.RESULT_OK, intent);
		
		((ToggleButton) findViewById(R.id.buttonFreehand)).setChecked(false);
		((ToggleButton) findViewById(R.id.buttonLine)).setChecked(true);
		((ToggleButton) findViewById(R.id.buttonCircle)).setChecked(false);
		((ToggleButton) findViewById(R.id.buttonRectangle)).setChecked(false);
		finish();
	}
	
	public void buttonCircleClicked(View v)
	{
		activeTool = ToolConstants.CIRCLE;
		Intent intent = new Intent();
		intent.putExtra(ToolConstants.TOOL, activeTool);
		setResult(Activity.RESULT_OK, intent);

		((ToggleButton) findViewById(R.id.buttonFreehand)).setChecked(false);
		((ToggleButton) findViewById(R.id.buttonLine)).setChecked(false);
		((ToggleButton) findViewById(R.id.buttonCircle)).setChecked(true);
		((ToggleButton) findViewById(R.id.buttonRectangle)).setChecked(false);
		finish();
	}
	
	public void buttonRectangleClicked(View v)
	{
		activeTool = ToolConstants.RECTANGLE;
		Intent intent = new Intent();
		intent.putExtra(ToolConstants.TOOL, activeTool);
		setResult(Activity.RESULT_OK, intent);

		((ToggleButton) findViewById(R.id.buttonFreehand)).setChecked(false);
		((ToggleButton) findViewById(R.id.buttonLine)).setChecked(false);
		((ToggleButton) findViewById(R.id.buttonCircle)).setChecked(false);
		((ToggleButton) findViewById(R.id.buttonRectangle)).setChecked(true);
		finish();
	}
	
	
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case android.R.id.home:
            NavUtils.navigateUpFromSameTask(this);
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
