package com.gamesparks.GameSparksTests;

import android.graphics.Color;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.Spannable;
import android.text.style.ForegroundColorSpan;
import android.widget.TextView;
import android.os.Handler;

/*
* This shows just the output of the test-suite,
* to run a specific test, look at native-lib.cpp
* in Java_com_gamesparks_GameSparksTests_MainActivity_startTestSuite()
*
*
* */


public class MainActivity extends AppCompatActivity {

    //runs without a timer by reposting this handler at the end of the runnable
    Handler timerHandler = new Handler();
    Runnable timerRunnable = new Runnable() {

        @Override
        public void run() {
            if(log != null)
            {
                String msg = getNextLogEntry();

                while(!msg.contentEquals(""))
                {
                    int colour = 0;
                    switch(getLastLogEntryColor())
                    {
                        case 0: colour = Color.WHITE; break;
                        case 1: colour = Color.GREEN; break;
                        case 2: colour = Color.YELLOW; break;
                        case 3: colour = Color.RED; break;
                        case 4: colour = Color.GRAY; break;
                    }

                    appendColoredText(log, msg, colour);
                    msg = getNextLogEntry();
                }
            }
            timerHandler.postDelayed(this, 100);
        }
    };

    public static void appendColoredText(TextView tv, String text, int color) {
        int start = tv.getText().length();
        tv.append(text);
        int end = tv.getText().length();

        Spannable spannableText = (Spannable) tv.getText();
        spannableText.setSpan(new ForegroundColorSpan(color), start, end, 0);
    }

    TextView log;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        startTestSuite();

        log = (TextView)findViewById(R.id.log);
        timerHandler.postDelayed(timerRunnable, 0);
    }


    public native void startTestSuite();

    public native String getNextLogEntry();
    public native int getLastLogEntryColor();

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
}
