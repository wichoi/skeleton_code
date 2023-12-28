package aprotech.ev.unimanager.fragment;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import com.squareup.otto.Subscribe;

import org.json.JSONException;

import java.io.UnsupportedEncodingException;
import java.util.Timer;
import java.util.TimerTask;

import aprotech.ev.unimanager.MainActivity;
import aprotech.ev.unimanager.R;
import aprotech.ev.unimanager.event.EventData;
import aprotech.ev.unimanager.event.EventHandler;
import aprotech.ev.unimanager.json.JsonHandler;

public class StatusFragment extends Fragment {
    private static final String TAG = "UniMgrFragment";

    TimerTask timerTask;
    Timer timer = new Timer();

    private TextView deviceStatus;
    private TextView deviceStPort1;
    private TextView deviceStPort2;
    private TextView ocppStatus;
    private TextView ocppStPort1;
    private TextView ocppStPort2;
    private TextView modemStatus;
    private TextView serviceStatus;
    private TextView offModeStatus;
    private Button btnReload;

    @Override
    public View onCreateView(
            LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState
    ) {
        EventHandler.getInstance().register(this);
        sendStatusRequest(JsonHandler.BODY_READ);
        //startTimerTask();
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_status, container, false);
    }

    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        ((MainActivity) getActivity()).getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        deviceStatus = view.findViewById(R.id.device_status);
        deviceStPort1 = view.findViewById(R.id.device_port1);
        deviceStPort2 = view.findViewById(R.id.device_port2);
        ocppStatus = view.findViewById(R.id.ocpp_status);
        ocppStPort1 = view.findViewById(R.id.ocpp_port1);
        ocppStPort2 = view.findViewById(R.id.ocpp_port2);
        modemStatus = view.findViewById(R.id.modem_status);
        serviceStatus = view.findViewById(R.id.conn_status);
        offModeStatus = view.findViewById(R.id.offmode_status);
        btnReload = view.findViewById(R.id.button_reload);

        btnReload.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                sendStatusRequest(JsonHandler.BODY_READ);
            }
        });
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        stopTimerTask();
        EventHandler.getInstance().unregister(this);
    }

    @Subscribe
    public void eventListener(EventData eventData) {
        if(eventData.getCmd() != JsonHandler.CMD_STATUS)
            return;

        for(int i = 0; i < eventData.getBodySize(); i++) {
            switch(eventData.getBody(i).getKey()) {
                case JsonHandler.BODY_ST_UI:
                    //deviceStatus.setText((String) eventData.getBody(i).getValue());
                    convText(deviceStatus, (String) eventData.getBody(i).getValue());
                    sendStatusRequest(JsonHandler.BODY_READ2);
                    break;
                case JsonHandler.BODY_ST_UCN1:
                    //deviceStPort1.setText((String) eventData.getBody(i).getValue());
                    convText(deviceStPort1, (String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_ST_UCN2:
                    //deviceStPort2.setText((String) eventData.getBody(i).getValue());
                    convText(deviceStPort2, (String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_ST_OCS:
                    //ocppStatus.setText((String) eventData.getBody(i).getValue());
                    convText(ocppStatus, (String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_ST_OCN1:
                    //ocppStPort1.setText((String) eventData.getBody(i).getValue());
                    convText(ocppStPort1, (String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_ST_OCN2:
                    //ocppStPort2.setText((String) eventData.getBody(i).getValue());
                    convText(ocppStPort2, (String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_ST_MST:
                    //modemStatus.setText((String) eventData.getBody(i).getValue());
                    convText(modemStatus, (String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_ST_SVR:
                    //serviceStatus.setText((String) eventData.getBody(i).getValue());
                    convText(serviceStatus, (String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_ST_OFF:
                    //offModeStatus.setText((String) eventData.getBody(i).getValue());
                    convText(offModeStatus, (String) eventData.getBody(i).getValue());
                    break;
                default:
                    throw new IllegalStateException("Unexpected value: " + eventData.getBody(i).getKey());
            }
        }
    }

    void sendStatusRequest(String cmd) {
        try {
            String jsonData = JsonHandler.createState(cmd);
            MainActivity.getInstance().sendData(jsonData);
        } catch (UnsupportedEncodingException | JSONException e) {
            e.printStackTrace();
        }
    }

    private void startTimerTask()
    {
        stopTimerTask();

        timerTask = new TimerTask() {
            @Override
            public void run() {
                sendStatusRequest(JsonHandler.BODY_READ);
            }
        };
        timer.schedule(timerTask,0 ,1000 * 10);
    }

    private void stopTimerTask() {
        if(timerTask != null) {
            timerTask.cancel();
            timerTask = null;
        }
    }

    private void convText(TextView tv, String str) {
        switch(str) {
            case "idle":
                tv.setText("Idle");
                break;
            case "falt":
                tv.setText("Fault");
                break;
            case "wait":
                tv.setText("Waiting");
                break;
            case "auth":
                tv.setText("Auth");
                break;
            case "prep":
                tv.setText("Preparing");
                break;
            case "chrg":
                tv.setText("Charging");
                break;
            case "fwup":
                tv.setText("FirmwareUpdate");
                break;
            case "card":
                tv.setText("CardRegiMode");
                break;
            case "usrg":
                tv.setText("UserGuide");
                break;
            case "rest":
                tv.setText("Reset");
                break;
            case "prod":
                tv.setText("ProductTestMode");
                break;

            case "init":
                tv.setText("Init");
                break;
            case "rese":
                tv.setText("Reserved");
                break;
            case "susp":
                tv.setText("Suspended");
                break;
            case "fini":
                tv.setText("Finishing");
                break;
            case "comp":
                tv.setText("Complete");
                break;

            case "cont":
                tv.setText("Connecting");
                break;
            case "prov":
                tv.setText("Provision");
                break;
            case "chrgauth":
                tv.setText("ChargingAuth");
                break;
            case "ltet":
                tv.setText("LteTest");
                break;

            case "dcnt":
                tv.setText("Disconnected");
                break;
            case "cnting":
                tv.setText("Connecting");
                break;
            case "cnt":
                tv.setText("Connected");
                break;
            case "off":
                tv.setText("Offline");
                break;
            case "on":
                tv.setText("Online");
                break;

            default:
                tv.setText(str);
                break;
        }
    }
}