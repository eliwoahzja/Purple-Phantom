package com.android.support;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.view.Gravity;
import android.widget.TextView;

/** Loads the production native library. The standalone ImGui preview is opt-in. */
public class MainActivity extends Activity {
    private static final String REAL_LIBRARY = "whitecrowns";

    @Override public void onCreate(Bundle state) {
        super.onCreate(state);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_RESIZE);
        try {
            System.loadLibrary(REAL_LIBRARY);
            TextView status = new TextView(this);
            status.setGravity(Gravity.CENTER);
            status.setTextColor(Color.WHITE);
            status.setTextSize(16);
            status.setBackgroundColor(Color.rgb(17, 20, 23));
            status.setText("White Crowns production native library loaded");
            setContentView(status);
        } catch (UnsatisfiedLinkError error) {
            new AlertDialog.Builder(this).setTitle("Production library unavailable")
                .setMessage("The real White Crowns native library could not be loaded. Rebuild the production APK for this device.\n\n" + error.getMessage())
                .setPositiveButton("Close", new DialogInterface.OnClickListener() {
                    @Override public void onClick(DialogInterface dialog, int which) {
                        MainActivity.this.finish();
                    }
                })
                .setCancelable(false).show();
        }
    }

    @Override public void onWindowFocusChanged(boolean focused) {
        super.onWindowFocusChanged(focused);
        if (focused) getWindow().getDecorView().setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
    }

    @Override protected void onResume() {
        super.onResume();
    }

    @Override protected void onPause() {
        super.onPause();
    }
}
