package com.anc_hackathons.github.capturethepi;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import com.anc_hackathons.github.capturethepi.data.model.Score;
import com.anc_hackathons.github.capturethepi.data.remote.ScoreService;

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
}
