package aprotech.ev.unimanager.fragment;

import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

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

public class ModemFragment extends Fragment {
    private static final String TAG = "UniModemFragment";

    TimerTask timerTask;
    Timer timer = new Timer();

    private TextView modemVersion;
    private TextView modemIpAddr;
    private TextView modemIpType;
    private TextView modemUsim;
    private TextView modemRegi;
    private TextView modemNum;
    private TextView modemImei;
    private TextView modemRssi;
    private TextView modemRsrp;
    private TextView modemRsrq;
    private TextView modemEmm;
    private Button btnReload;
    private int requestStage;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EventHandler.getInstance().register(this);
        requestStage = 0;
        sendModemRequest(JsonHandler.BODY_READ, true);
        //startTimerTask();
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        ((MainActivity) getActivity()).getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        // Inflate the layout for this fragment
        final View view = inflater.inflate(R.layout.fragment_modem, container, false);
        modemVersion = view.findViewById(R.id.modem_version);
        modemIpAddr = view.findViewById(R.id.modem_ipaddr);
        modemIpType = view.findViewById(R.id.modem_iptype);
        modemUsim = view.findViewById(R.id.modem_usim);
        modemRegi = view.findViewById(R.id.modem_active);
        modemNum = view.findViewById(R.id.modem_number);
        modemImei = view.findViewById(R.id.modem_imei);
        modemRssi = view.findViewById(R.id.modem_rssi);
        modemRsrp = view.findViewById(R.id.modem_rsrp);
        modemRsrq = view.findViewById(R.id.modem_rsrq);
        modemEmm = view.findViewById(R.id.modem_emm);
        btnReload = view.findViewById(R.id.button_reload);

        btnReload.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                requestStage = 0;
                sendModemRequest(JsonHandler.BODY_READ, true);
            }
        });

        return view;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        stopTimerTask();
        EventHandler.getInstance().unregister(this);
    }

    @Subscribe
    public void eventListener(EventData eventData) {
        if(eventData.getCmd() != JsonHandler.CMD_MODEM)
            return;

        for(int i = 0; i < eventData.getBodySize(); i++) {
            switch(eventData.getBody(i).getKey()) {
                case JsonHandler.BODY_IP_ADDR:
                    modemIpAddr.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_IP_TYPE:
                    modemIpType.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_USIM:
                    modemUsim.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_REGI:
                    modemRegi.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_NUM:
                    modemNum.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_VER:
                    modemVersion.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_IMEI:
                    modemImei.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_RSSI:
                    modemRssi.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_RSRP:
                    modemRsrp.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_RSRQ:
                    modemRsrq.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_EMM:
                    modemEmm.setText(Integer.toString((Integer) eventData.getBody(i).getValue()));
                    break;
                default:
                    throw new IllegalStateException("Unexpected value: " + eventData.getBody(i).getKey());
            }
        }

        sendModemRequest(JsonHandler.BODY_READ_RSSI, false);
    }

    void sendModemRequest(String cmd, boolean isStart) {
        if(isStart == true){
            requestStage = 0;
        }

        try {
            switch(requestStage++) {
                case 0:
                    MainActivity.getInstance().sendData(JsonHandler.createModem(cmd));
                    break;
                case 1:
                    MainActivity.getInstance().sendData(JsonHandler.createModem(cmd));
                    break;
            }
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
                sendModemRequest(JsonHandler.BODY_READ, true);
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
}