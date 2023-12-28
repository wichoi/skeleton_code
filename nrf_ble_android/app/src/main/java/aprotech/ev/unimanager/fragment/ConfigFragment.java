package aprotech.ev.unimanager.fragment;

import android.app.AlertDialog;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.squareup.otto.Subscribe;

import org.json.JSONException;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

import aprotech.ev.unimanager.MainActivity;
import aprotech.ev.unimanager.R;
import aprotech.ev.unimanager.event.EventData;
import aprotech.ev.unimanager.event.EventHandler;
import aprotech.ev.unimanager.json.JsonHandler;

public class ConfigFragment extends Fragment {
    private static final String TAG = "UniConfigFragment";

    private TextView confVolOver;
    private TextView confVolUnder;
    private TextView confCurOver;
    private TextView confTempOver;
    private TextView confUrl;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EventHandler.getInstance().register(this);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        ((MainActivity) getActivity()).getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        // Inflate the layout for this fragment
        final View view = inflater.inflate(R.layout.fragment_config, container, false);
        confVolOver = view.findViewById(R.id.config_vol_over);
        confVolUnder = view.findViewById(R.id.config_vol_under);
        confCurOver = view.findViewById(R.id.config_cur_over);
        confTempOver = view.findViewById(R.id.config_temp_over);
        confUrl = view.findViewById(R.id.config_url);

        sendConfigRequest(JsonHandler.ACTION_READ);

        final Button uploadBtn = view.findViewById(R.id.button_upload);
        uploadBtn.setOnClickListener(v -> {
            if(verifyCheck().length() > 0) {
                MainActivity.getInstance().toastMessage(verifyCheck());
                return;
            }

            new AlertDialog.Builder(getActivity())
                    .setTitle(R.string.config_title)
                    .setMessage(R.string.config_message)
                    .setNegativeButton(R.string.no, (dialog, which) -> {

                    })
                    .setPositiveButton(R.string.yes, (dialog, which) -> {
                        sendConfigRequest(JsonHandler.ACTION_WRITE);
                    })
                    .show();
        });

        final Button defaultBtn = view.findViewById(R.id.button_default);
        defaultBtn.setOnClickListener(v -> {
            new AlertDialog.Builder(getActivity())
                    .setTitle(R.string.config_title)
                    .setMessage(R.string.config_default)
                    .setNegativeButton(R.string.no, (dialog, which) -> {

                    })
                    .setPositiveButton(R.string.yes, (dialog, which) -> {
                        sendConfigRequest(JsonHandler.ACTION_DEFAULT);
                    })
                    .show();
        });

        return view;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        EventHandler.getInstance().unregister(this);
    }

    @Subscribe
    public void eventListener(EventData eventData) {
        if(! (eventData.getCmd() == JsonHandler.CMD_CONF) )
            return;

        for(int i = 0; i < eventData.getBodySize(); i++) {
            switch(eventData.getBody(i).getKey()) {
                case JsonHandler.BODY_VOL_H:
                    confVolOver.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_VOL_L:
                    confVolUnder.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_CUR_H:
                    confCurOver.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_TEMP_H:
                    confTempOver.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_URL:
                    confUrl.setText((String) eventData.getBody(i).getValue());
                    break;
                default:
                    throw new IllegalStateException("Unexpected value: " + eventData.getBody(i).getKey());
            }
        }
    }

    private void sendConfigRequest(String action) {
        ArrayList<EventData.Body> paramList = new ArrayList<>();
        paramList.add(new EventData.Body(JsonHandler.BODY_VOL_H, confVolOver.getText().toString()));
        paramList.add(new EventData.Body(JsonHandler.BODY_VOL_L, confVolUnder.getText().toString()));
        paramList.add(new EventData.Body(JsonHandler.BODY_CUR_H, confCurOver.getText().toString()));
        paramList.add(new EventData.Body(JsonHandler.BODY_TEMP_H, confTempOver.getText().toString()));
        paramList.add(new EventData.Body(JsonHandler.BODY_URL, confUrl.getText().toString()));
        try {
            if(!paramList.isEmpty()) {
                String jsonData = JsonHandler.createConfig(action, paramList);
                MainActivity.getInstance().sendData(jsonData.replace("\\",""));
            }
        } catch (UnsupportedEncodingException | JSONException e) {
            e.printStackTrace();
        }
    }

    private String verifyCheck() {
        String errReason = "";

        int overVol = -1;
        if(!confVolOver.getText().toString().trim().isEmpty()) {
            overVol = Integer.parseInt(confVolOver.getText().toString());
        }

        int underVol = -1;
        if(!confVolUnder.getText().toString().trim().isEmpty()) {
            underVol = Integer.parseInt(confVolUnder.getText().toString());
        }

        int overCur = -1;
        if(!confCurOver.getText().toString().trim().isEmpty()) {
            overCur = Integer.parseInt(confCurOver.getText().toString());
        }

        int overTemp = -1;
        if(!confTempOver.getText().toString().trim().isEmpty()) {
            overTemp = Integer.parseInt(confTempOver.getText().toString());
        }

        if(confUrl.getText().toString().trim().isEmpty()) {
            errReason = "Invalid Input [url(ocpp) is null] !!!";
        }

        if(overVol < 230 || overVol > 400) {
            errReason = "Invalid Input [Over Voltage Range 230 ~ 400] !!!";
        }

        if(underVol < 0 || underVol > 200) {
            errReason = "Invalid Input [Under Voltage Range 0 ~ 200] !!!";
        }

        if(overCur < 15 || overCur > 100) {
            errReason = "Invalid Input [Over Current Range 15 ~ 100] !!!";
        }

        if(overTemp < 50 || overTemp > 200) {
            errReason = "Invalid Input [Over Temperature Range 50 ~ 200] !!!";
        }

        return errReason;
    }
}