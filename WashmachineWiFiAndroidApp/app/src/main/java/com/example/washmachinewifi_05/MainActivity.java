package com.example.washmachinewifi_05;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.Bundle;
import android.os.StrictMode;
import android.widget.*;
import android.view.View;
import androidx.core.view.GravityCompat;
import androidx.drawerlayout.widget.DrawerLayout;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;

import java.io.*;
import java.net.*;

// https://www.instructables.com/id/NodeMCU-ESP8266-Details-and-Pinout/

public class MainActivity extends AppCompatActivity
{
    //    IP nad PORT  ;   DDNS: https://www.duckdns.org   slaherpralka.duckdns.org
    private String adresIp = "192.168.0.129";
    private int port = 5045;

    //    Socket
    private Socket socket = null;
    private SocketAddress socketAddress ;

    //     washmachine name
    private String name_machine = "iwsd51252";

    //    output commands
    private String power_on_str = name_machine + "_power_on";
    private String power_off_str = name_machine + "_power_off";
    private String start_str = name_machine + "_start";
    private String pause_str = name_machine + "_pause";

    //    buttons initialization
    private Button power_on_btn;
    private Button power_off_btn;
    private Button start_btn;
    private Button pause_btn;
    private ImageView refresh_btn;

    //    input commands
    static final private String wash_led_status_input = "ledwash";
    static final private String rinse_led_status_input = "ledrinse";
    static final private String run_led_status_input = "ledrun";
    static final private String pause_led_status_input = "ledpause";
    static final private String spin_led_status_input = "ledspin";
    static final private String drain_led_status_input = "leddrain";
    static final private String end_of_wash_led_status_input = "ledendofwash";
    static final private String lock_led_status_input = "ledlock";

    //    address field
    private TextView txt_conn;
    private TextView txt_address_field;

    //    images initialization
    private ImageView wash_image_view;
    private ImageView rinse_image_view;
    private ImageView run_image_view;
    private ImageView spin_image_view;
    private ImageView drain_image_view;
    private ImageView end_of_wash_image_view;
    private ImageView lock_image_view;

    //    handler - processing message
    private Handler handler = new Handler();

    //    run and pause statet values for showing proper icon
    private Boolean run_state = false;
    private Boolean pause_state = false;

    //    Shared preferences - remembers address after closing app
    private SharedPreferences sharedPref;
    private Thread thr;

    public MainActivity() {}

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        //    Shared preferences - remembers address after closing app
        sharedPref  = this.getPreferences(Context.MODE_PRIVATE);
        String washmashine_ip_address_sharedPref = sharedPref .getString("washmashine_ip_address", "");

        if (washmashine_ip_address_sharedPref.equals(""))
        {
            SharedPreferences.Editor editor = sharedPref.edit();
            editor.putString("washmashine_ip_address", adresIp);
            editor.apply();
        }
        else{
            adresIp = washmashine_ip_address_sharedPref;
        }

        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        //        buttons initialization
        power_on_btn = (Button) findViewById(R.id.btn_power_on);
        power_off_btn = (Button) findViewById(R.id.btn_power_off);
        start_btn = (Button) findViewById(R.id.btn_start);
        pause_btn = (Button) findViewById(R.id.btn_pause);
        refresh_btn = (ImageView) findViewById(R.id.refresh_image);

        //        enable buttons when connected, disable when disconnected
        SetEnableDisableButtons(false);

        txt_conn = (TextView) findViewById(R.id.txt_connection_state);
        txt_conn.setText("Not Connected");

        //        images initialization
        wash_image_view = (ImageView) findViewById(R.id.wash_image);
        rinse_image_view = (ImageView) findViewById(R.id.rinse_image);
        run_image_view = (ImageView) findViewById(R.id.play_image);
        spin_image_view = (ImageView) findViewById(R.id.spin_image);
        drain_image_view = (ImageView) findViewById(R.id.drain_image);
        end_of_wash_image_view = (ImageView) findViewById(R.id.end_image);
        lock_image_view = (ImageView) findViewById(R.id.lock_image);

        //        et address in textbox field
        txt_address_field = (TextView) findViewById(R.id.textViewAddressField);
        txt_address_field.setText(adresIp);

        //        On Click Listener for power_on_btn
        power_on_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    SendPowerOn();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        //        On Click Listener for power_off_btn
        power_off_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    SendPowerOff();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        //        On Click Listener for pause_btn
        pause_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    SendPause();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        //        On Click Listener for start_btn
        start_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    SendStart();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        //        On Click Listener for refresh_btn
        refresh_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    RefreshConnection();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        //        Start connection
        StartConnection();
    }

    private void StartThread(){
        thr = new Thread(new Runnable() {

            @SuppressLint("SetTextI18n")
            public void run() {
                try {
                    while(socket.isConnected()) {
                        byte[] resultBuff = new byte[0];
                        byte[] buff = new byte[1024];
                        int k = -1;
                        String[] parts_input_string;
                        while((k = socket.getInputStream().read(buff, 0, buff.length)) > -1 ) {
                            byte[] tbuff = new byte[resultBuff.length + k]; // temp buffer size = bytes already read + bytes last read
                            System.arraycopy(resultBuff, 0, tbuff, 0, resultBuff.length); // copy previous bytes
                            System.arraycopy(buff, 0, tbuff, resultBuff.length, k);  // copy current lot
                            resultBuff = tbuff;
                            String se = new String(resultBuff);
                            parts_input_string = se.split("_");
                            if ((parts_input_string.length == 3) && parts_input_string[0].equals(name_machine) ) {
                                switch (parts_input_string[1]) {
                                    case wash_led_status_input:
                                        SetLedIconStatus(parts_input_string,
                                                wash_image_view,
                                                R.drawable.wash,
                                                R.drawable.wash_green,
                                                wash_led_status_input);
                                        break;

                                    case rinse_led_status_input:
                                        SetLedIconStatus(parts_input_string,
                                                rinse_image_view,
                                                R.drawable.rinse,
                                                R.drawable.rinse_green,
                                                rinse_led_status_input);
                                        break;


                                    case spin_led_status_input :
                                        SetLedIconStatus(parts_input_string,
                                                spin_image_view,
                                                R.drawable.spin,
                                                R.drawable.spin_green,
                                                spin_led_status_input);
                                        break;

                                    case drain_led_status_input :
                                        SetLedIconStatus(parts_input_string,
                                                drain_image_view,
                                                R.drawable.drain,
                                                R.drawable.drain_green,
                                                drain_led_status_input);
                                        break;

                                    case end_of_wash_led_status_input:
                                        SetLedIconStatus(parts_input_string,
                                                end_of_wash_image_view,
                                                R.drawable.end,
                                                R.drawable.end_green,
                                                end_of_wash_led_status_input);
                                        break;

                                    case lock_led_status_input :
                                        SetLedIconStatus(parts_input_string,
                                                lock_image_view,
                                                R.drawable.lock,
                                                R.drawable.lock_green,
                                                lock_led_status_input);
                                        break;

                                    case pause_led_status_input :
                                        pause_state = Boolean.parseBoolean(parts_input_string[2]);
                                        SetLedIconRunPauseStatus(pause_led_status_input);
                                        break;

                                    case run_led_status_input :
                                        run_state = Boolean.parseBoolean(parts_input_string[2]);
                                        SetLedIconRunPauseStatus(run_led_status_input);
                                        break;

                                    default:
                                        System.out.println("no match");
                                }
                            }
                            resultBuff = new byte[0];
                            buff = new byte[1024];
                        }
                        Thread.sleep(100);
                    }

                } catch(IOException | InterruptedException ignored) {

                }
            }
        });
    }

    private void SetEnableDisableButtons(Boolean state) {
        power_on_btn.setEnabled(state);
        power_off_btn.setEnabled(state);
        start_btn.setEnabled(state);
        pause_btn.setEnabled(state);
    }

    private void SetLedIconStatus(final String[] input_string_led,
                                  final ImageView image_view,
                                  final int image_name_gray,
                                  final int image_name_green,
                                  final String message)
    {
        handler.post(new Runnable() {
            @Override
            public void run() {
                if (!Boolean.parseBoolean(input_string_led[2])) {  // LED OFF
                    image_view.setImageResource(image_name_gray);
                    System.out.println(message + " OFF");
                }
                else if (Boolean.parseBoolean(input_string_led[2])) {  // LED ON
                    image_view.setImageResource(image_name_green);
                    System.out.println(message + " ON");
                }
            }
        });
    }

    private void SetLedIconRunPauseStatus(final String message)
    {
        handler.post(new Runnable() {
            @Override
            public void run() {
                if (run_state) {  // LED GREEN
                    run_image_view.setImageResource(R.drawable.play_green);
                    System.out.println(message + " GREEN_ON");
                }

                else if (pause_state) {  // LED ORANGE
                    run_image_view.setImageResource(R.drawable.play_orange);
                    System.out.println(message + " ORANGE_ON");
                }
                else {  // LED OFF
                    run_image_view.setImageResource(R.drawable.play);
                    System.out.println(message + " OFF");
                }
            }
        });
    }

    private void RefreshConnection() throws IOException {
        if (socket.isConnected()) {
            thr.interrupt();
            thr = null;
            socket.close();
        }
        StartConnection();
    }

    private void SendPowerOn() throws IOException {
        SendMessage(power_on_str);
    }

    private void SendStart() throws IOException {
        SendMessage(start_str);
    }

    private void SendPowerOff() throws IOException {
        SendMessage(power_off_str);
    }

    private void SendPause() throws IOException {
        SendMessage(pause_str);
    }
    private void SendMessage(String message) throws IOException {
        if (socket.isConnected()) {
            OutputStream output = socket.getOutputStream();
            byte[] data = message.getBytes();
            output.write(data);
        }
        else {
            RefreshConnection();
        }
    }

    //    Start connection method
    private void StartConnection()
    {
        socket = null;
        socket = new Socket();
        txt_conn.setText("Not Connected");
        SetEnableDisableButtons(false);
        socketAddress = null;
        socketAddress = new InetSocketAddress(adresIp, port);

        try {
            socket.connect(socketAddress, 2000);
            Thread.sleep(1000);
        } catch (IOException | InterruptedException e) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException ie) {
                // Interrupted.
            }
        }
        if (socket.isConnected()){
            StartThread();
            thr.start();
            txt_conn.setText("Connected");
            SetEnableDisableButtons(true);
        }
        else
            txt_conn.setText("Refresh again - no connection");
    }

    @Override
    public void onBackPressed() {
    DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
    if (drawer.isDrawerOpen(GravityCompat.START))
        drawer.closeDrawer(GravityCompat.START);
    else
        super.onBackPressed();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        //        Settings - disconnect, set new address and connect again
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            try {
                socket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
            AlertDialog alertDialog = new AlertDialog.Builder(MainActivity.this).create();

            alertDialog.setTitle("Set address");

            final EditText input = new EditText(MainActivity.this);

            LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT,
                    LinearLayout.LayoutParams.MATCH_PARENT);
            input.setLayoutParams(lp);
            input.setText(adresIp);
            alertDialog.setView(input);
            alertDialog.setButton(AlertDialog.BUTTON_POSITIVE, "OK",
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            txt_address_field.setText(input.getText().toString());
                            adresIp = input.getText().toString();
                            SharedPreferences.Editor editor = sharedPref.edit();
                            editor.putString("washmashine_ip_address", adresIp);
                            editor.apply();
                            StartConnection();
                            dialog.dismiss();
                        }
                    });
            alertDialog.setButton(AlertDialog.BUTTON_NEGATIVE, "CANCEL",
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            StartConnection();
                            dialog.dismiss();
                        }
                    });
            alertDialog.show();
        }

        return super.onOptionsItemSelected(item);
    }
}
