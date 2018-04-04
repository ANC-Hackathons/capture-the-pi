package com.anc_hackathons.github.capturethepi;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import com.anc_hackathons.github.capturethepi.data.model.Score;
import com.anc_hackathons.github.capturethepi.data.remote.ScoreService;
import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import java.io.IOException;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class MainActivity extends AppCompatActivity {

    private static final String API_URL = "https://fathomless-inlet-46417.herokuapp.com";
    private BroadcastReceiver receiver;

    private ScoreService.ScoreServer scoreServer;
    private int redScore = 0;
    private int blueScore = 0;
    private boolean killedRecently = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Create a very simple REST adapter which points the GitHub API.
        Retrofit retrofit = new Retrofit.Builder()
                .baseUrl(API_URL)
                .addConverterFactory(GsonConverterFactory.create())
                .build();

        // Create an instance of our GitHub API interface.
        scoreServer = retrofit.create(ScoreService.ScoreServer.class);

        // Create a call instance for looking up Retrofit contributors.
        Call<Score> call = scoreServer.getScore();

        // Fetch and print the current score
        getScore();
    }

    @Override
    public void onResume() {
        super.onResume();

        // Register the receiver
        PebbleKit.registerReceivedDataHandler(getApplicationContext(), dataReceiver);
    }

    public void getScore() {
        scoreServer.getScore().enqueue(new Callback<Score>() {
            @Override
            public void onResponse(Call<Score> call, Response<Score> response) {

                if(response.isSuccessful()) {
                    if (redScore != response.body().getRed() || blueScore != response.body().getBlue()) {
                        redScore = response.body().getRed();
                        blueScore = response.body().getBlue();

                        // Create a new dictionary
                        PebbleDictionary dict = new PebbleDictionary();

                        dict.addInt32(Constants.RED_SCORE, redScore);
                        dict.addInt32(Constants.BLUE_SCORE, blueScore);

                        // Send the dictionary
                        PebbleKit.sendDataToPebble(getApplicationContext(), Constants.PEBBLE_SIFTER_UUID, dict);
                    }

                    if (killedRecently != response.body().getKilledRecently() && !killedRecently) {
                        // Create a new dictionary
                        PebbleDictionary dict = new PebbleDictionary();
                        dict.addInt32(Constants.NEW_KILL, 0);

                        // Send the dictionary
                        PebbleKit.sendDataToPebble(getApplicationContext(), Constants.PEBBLE_SIFTER_UUID, dict);
                    }
                }else {
                    int statusCode  = response.code();
                    // handle request errors depending on status code
                    Log.d("MainActivity", "error getting scores");
                    Log.d("MainActivity", String.valueOf(statusCode));
                }

                getScore();
            }

            @Override
            public void onFailure(Call<Score> call, Throwable t) {
                Log.d("MainActivity", "error loading from API");
                Log.d("MainActivity", t.getMessage());

                getScore();
            }
        });
    }

    public void incrementRedScore(String score) {
        scoreServer.incrementRedScore(score).enqueue(new Callback<String>() {
            @Override
            public void onResponse(Call<String> call, Response<String> response) {

                if(response.isSuccessful()) {
                    Log.d("MainActivity", "Red score put successful");
                }else {
                    int statusCode  = response.code();
                    Log.d("MainActivity", "error getting scores");
                    Log.d("MainActivity", String.valueOf(statusCode));
                }
            }

            @Override
            public void onFailure(Call<String> call, Throwable t) {
                Log.d("MainActivity", "error putting Red score");
                Log.d("MainActivity", t.getMessage());
            }
        });
    }

    public void incrementBlueScore(String score) {
        scoreServer.incrementBlueScore(score).enqueue(new Callback<String>() {
            @Override
            public void onResponse(Call<String> call, Response<String> response) {

                if(response.isSuccessful()) {
                    Log.d("MainActivity", "Blue score put successful");
                }else {
                    int statusCode  = response.code();
                    Log.d("MainActivity", "error getting scores");
                    Log.d("MainActivity", String.valueOf(statusCode));
                }
            }

            @Override
            public void onFailure(Call<String> call, Throwable t) {
                Log.d("MainActivity", "error putting Blue score");
                Log.d("MainActivity", t.getMessage());
            }
        });
    }

    public void postNewKill() {
        scoreServer.newKill().enqueue(new Callback<String>() {
            @Override
            public void onResponse(Call<String> call, Response<String> response) {

                if(response.isSuccessful()) {
                    Log.d("MainActivity", "post new kill successful");
                }else {
                    int statusCode  = response.code();
                    Log.d("MainActivity", "error posting new kill");
                    Log.d("MainActivity", String.valueOf(statusCode));
                }
            }

            @Override
            public void onFailure(Call<String> call, Throwable t) {
                Log.d("MainActivity", "error posting new kill");
                Log.d("MainActivity", t.getMessage());
            }
        });
    }

    // Create a new receiver to get AppMessages from the C app
    PebbleKit.PebbleDataReceiver dataReceiver = new PebbleKit.PebbleDataReceiver(Constants.PEBBLE_SIFTER_UUID) {

        @Override
        public void receiveData(Context context, int transaction_id,
                                PebbleDictionary dict) {
            // A new AppMessage was received, tell Pebble
            PebbleKit.sendAckToPebble(context, transaction_id);
            Log.d("MainActivity", "Received score update from pebble");

            // If the tuple is present...
            Long redScoreValue = dict.getInteger(Constants.RED_SCORE);
            if(redScoreValue != null) {
                Log.d("MainActivity", "Received red score update from pebble");

                // Read the integer value
                int red = redScoreValue.intValue();
                Log.d("MainActivity", String.valueOf(red));
                incrementRedScore(String.valueOf(red));
            }

            Long blueScoreValue = dict.getInteger(Constants.BLUE_SCORE);
            if(blueScoreValue != null) {
                Log.d("MainActivity", "Received blue score update from pebble");

                // Read the integer value
                int blue = blueScoreValue.intValue();
                Log.d("MainActivity", String.valueOf(blue));
                incrementBlueScore(String.valueOf(blue));
            }

            Long newKill = dict.getInteger(Constants.NEW_KILL);
            if(newKill != null) {
                Log.d("MainActivity", "Received new kill update from pebble");
                postNewKill();
            }
        }

    };
}
