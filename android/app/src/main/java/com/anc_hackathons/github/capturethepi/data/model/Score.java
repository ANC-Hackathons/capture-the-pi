package com.anc_hackathons.github.capturethepi.data.model;

/**
 * Created by nathanthompson on 3/11/18.
 */

import com.google.gson.annotations.Expose;
import com.google.gson.annotations.SerializedName;

public class Score {

    @SerializedName("red")
    @Expose
    private Integer red;
    @SerializedName("blue")
    @Expose
    private Integer blue;

    public Integer getRed() {
        return red;
    }

    public void setRed(Integer red) {
        this.red = red;
    }

    public Integer getBlue() {
        return blue;
    }

    public void setBlue(Integer blue) {
        this.blue = blue;
    }

}
