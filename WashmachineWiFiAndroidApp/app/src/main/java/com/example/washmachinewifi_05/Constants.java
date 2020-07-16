package com.example.washmachinewifi_05;

class Constants {
    // port number
    static int washmashinePortNumber = 5045;

    // output commands
    static String washmashineName = "iwsd51252";
    static String powerOnStr = washmashineName + "_power_on";
    static String powerOffStr = washmashineName + "_power_off";
    static String startStr = washmashineName + "_start";
    static String pauseStr = washmashineName + "_pause";

    // input commands
    static final String washLedStatusInput = "ledwash";
    static final String rinseLedStatusInput = "ledrinse";
    static final String runLedStatusInput = "ledrun";
    static final String pauseLedStatusInput = "ledpause";
    static final String spinLedStatusInput = "ledspin";
    static final String drainLedStatusInput = "leddrain";
    static final String endOfWashLedStatusInput = "ledendofwash";
    static final String lockLedStatusInput = "ledlock";

    //
    static final String connectedStr = "Connected";
    static final String notConnectedStr = " Not connected";
}
