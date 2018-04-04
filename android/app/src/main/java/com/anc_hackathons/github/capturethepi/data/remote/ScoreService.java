package com.anc_hackathons.github.capturethepi.data.remote;

/**
 * Created by nathanthompson on 3/11/18.
 */

import com.anc_hackathons.github.capturethepi.data.model.Score;

import java.io.IOException;

import retrofit2.Call;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;
import retrofit2.http.GET;
import retrofit2.http.PUT;
import retrofit2.http.Path;

public class ScoreService {

    public interface ScoreServer {
        @GET("/score")
        Call<Score> getScore();

        @PUT("/score/red/{score}")
        Call<String> incrementRedScore(
                @Path("score") String score);

        @PUT("/score/blue/{score}")
        Call<String> incrementBlueScore(
                @Path("score") String score);

        @PUT("/newKill")
        Call<String> newKill();
    }

}
