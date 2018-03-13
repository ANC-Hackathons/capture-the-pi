package com.anc_hackathons.github.capturethepi;

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

    private ScoreService.ScoreServer scoreServer;
    private int redScore = 0;
    private int blueScore = 0;

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

    @Override
    public void onPause() {
        super.onResume();

        // Register the receiver
        unregisterReceiver(dataReceiver);
    }

    public void getScore() {
        scoreServer.getScore().enqueue(new Callback<Score>() {
            @Override
            public void onResponse(Call<Score> call, Response<Score> response) {

                if(response.isSuccessful()) {
                    Log.d("MainActivity", "posts loaded from API");
                    Log.d("MainActivity", response.body().getRed().toString());
                    Log.d("MainActivity", response.body().getBlue().toString());
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
                }else {
                    int statusCode  = response.code();
                    // handle request errors depending on status code
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

    // Create a new receiver to get AppMessages from the C app
    PebbleKit.PebbleDataReceiver dataReceiver = new PebbleKit.PebbleDataReceiver(Constants.PEBBLE_SIFTER_UUID) {

        @Override
        public void receiveData(Context context, int transaction_id,
                                PebbleDictionary dict) {
            // A new AppMessage was received, tell Pebble
            PebbleKit.sendAckToPebble(context, transaction_id);

            // If the tuple is present...
            Long redScoreValue = dict.getInteger(Constants.RED_SCORE);
            if(redScoreValue != null) {
                // Read the integer value
                int red = redScoreValue.intValue();
            }

            Long blueScoreValue = dict.getInteger(Constants.BLUE_SCORE);
            if(blueScoreValue != null) {
                // Read the integer value
                int blue = blueScoreValue.intValue();
            }

            Long newKill = dict.getInteger(Constants.NEW_KILL);
            if(newKill != null) {
                // Read the integer value
                int kill = newKill.intValue();
            }
        }

    };
}
