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

public class SystemFragment extends Fragment {
    private static final String TAG = "UniMgrFragment";

    TimerTask timerTask;
    Timer timer = new Timer();

    private TextView modelName;
    private TextView serial;
    private TextView swVersion;
    private TextView hwVersion;
    private TextView port1State;
    private TextView port1Voltage;
    private TextView port1Current;
    private TextView port1Temperature;
    private TextView port2State;
    private TextView port2Voltage;
    private TextView port2Current;
    private TextView port2Temperature;
    private TextView uptime;
    private TextView memInfo;
    private Button btnReload;

    @Override
    public View onCreateView(
            LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState
    ) {
        EventHandler.getInstance().register(this);
        sendSystemRequest(JsonHandler.BODY_READ1);
        //startTimerTask();
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_system, container, false);
    }

    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        ((MainActivity) getActivity()).getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        modelName = view.findViewById(R.id.system_name);
        serial = view.findViewById(R.id.system_serial);
        swVersion = view.findViewById(R.id.system_sw);
        hwVersion = view.findViewById(R.id.system_hw);
        port1State = view.findViewById(R.id.system_port1);
        port1Voltage = view.findViewById(R.id.system_voltage1);
        port1Current = view.findViewById(R.id.system_current1);
        port1Temperature = view.findViewById(R.id.system_temp1);
        port2State = view.findViewById(R.id.system_port2);
        port2Voltage = view.findViewById(R.id.system_voltage2);
        port2Current = view.findViewById(R.id.system_current2);
        port2Temperature = view.findViewById(R.id.system_temp2);
        uptime = view.findViewById(R.id.system_uptime);
        memInfo = view.findViewById(R.id.system_memory);
        btnReload = view.findViewById(R.id.button_reload);

        btnReload.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                sendSystemRequest(JsonHandler.BODY_READ1);
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
        if(eventData.getCmd() != JsonHandler.CMD_SYSTEM)
            return;

        for(int i = 0; i < eventData.getBodySize(); i++) {
            switch(eventData.getBody(i).getKey()) {
                case JsonHandler.BODY_MODEL:
                    modelName.setText((String) eventData.getBody(i).getValue());
                    sendSystemRequest(JsonHandler.BODY_READ2);
                    break;
                case JsonHandler.BODY_SERIAL:
                    serial.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_SW:
                    swVersion.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_HW:
                    hwVersion.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_PORT1:
                    port1State.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_VOLT1:
                    port1Voltage.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_CURR1:
                    port1Current.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_TEMP1:
                    port1Temperature.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_PORT2:
                    port2State.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_VOLT2:
                    port2Voltage.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_CURR2:
                    port2Current.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_TEMP2:
                    port2Temperature.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_UPTIME:
                    uptime.setText((String) eventData.getBody(i).getValue());
                    break;
                case JsonHandler.BODY_MEM:
                    memInfo.setText((String) eventData.getBody(i).getValue());
                    break;
                default:
                    throw new IllegalStateException("Unexpected value: " + eventData.getBody(i).getKey());
            }
        }
    }

    void sendSystemRequest(String cmd) {
        try {
            String jsonData = JsonHandler.createSystem(cmd);
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
                sendSystemRequest(JsonHandler.BODY_READ1);
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