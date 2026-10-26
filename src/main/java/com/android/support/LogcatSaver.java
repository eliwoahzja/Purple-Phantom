
import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;

public class LogcatSaver {

    private static final String TAG = "LogcatSaver";
    private static final String LOG_FILE_NAME = "crash_log.txt";
    private static final String LOG_DIRECTORY = "/storage/emulated/0/";

    public static void startLogging(final Context context) {
        new AsyncTask<Void, Void, Void>() {
            @Override
            protected Void doInBackground(Void... params) {
                try {
                    Process process = Runtime.getRuntime().exec("logcat -d");
                    BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                    String line;
                    StringBuilder log = new StringBuilder();
                    while ((line = reader.readLine()) != null) {
                        log.append(line).append("\n");
                    }
                    saveLogToFile(log.toString());
                } catch (IOException e) {
                    Log.e(TAG, "Error capturing logcat logs", e);
                }
                return null;
            }
        }.execute();
    }

    private static void saveLogToFile(String log) {
        try {
            File dir = new File(LOG_DIRECTORY);
            if (!dir.exists()) {
                dir.mkdirs();
            }
            File logFile = new File(dir, LOG_FILE_NAME);
            FileOutputStream fos = new FileOutputStream(logFile, true);
            fos.write(log.getBytes());
            fos.close();
        } catch (IOException e) {
            Log.e(TAG, "Error saving log file", e);
        }
    }
}

