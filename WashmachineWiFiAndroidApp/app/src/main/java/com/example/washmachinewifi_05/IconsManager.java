package com.example.washmachinewifi_05;

import android.os.Handler;
import android.widget.ImageView;

class IconsManager {
    private Handler handler = new Handler();

    Boolean runState = false;
    Boolean pauseState = false;

    void  SetLedIconStatus(final String[] input_string_led,
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


    void SetLedIconRunPauseStatus(final String message, final ImageView runImage)
    {
        handler.post(new Runnable() {
            @Override
            public void run() {
                if (runState) {  // LED GREEN
                    runImage.setImageResource(R.drawable.play_green);
                    System.out.println(message + " GREEN_ON");
                }

                else if (pauseState) {  // LED ORANGE
                    runImage.setImageResource(R.drawable.play_orange);
                    System.out.println(message + " ORANGE_ON");
                }
                else {  // LED OFF
                    runImage.setImageResource(R.drawable.play);
                    System.out.println(message + " OFF");
                }
            }
        });
    }


}
