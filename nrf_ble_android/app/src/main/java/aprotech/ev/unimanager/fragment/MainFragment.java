package aprotech.ev.unimanager.fragment;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.navigation.fragment.NavHostFragment;

import aprotech.ev.unimanager.MainActivity;
import aprotech.ev.unimanager.R;

public class MainFragment extends Fragment implements Button.OnClickListener{
    private static final String TAG = "UniMgrFragment";

    private boolean bleIsConn;

    private Button bntSystem;
    private Button btnModem;
    private Button btnConfig;
    private Button bntStatus;
    private Button btnConsole;
    private Button btnUpgrade;
    private Button btnReboot;
    private Button btnCert;
    private Button btnBleConn;
    private Button btnReqAccept;

    private static MainFragment fragment;
    public static MainFragment getInstance(){
        return fragment;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        fragment = this;
    }

    @Override
    public View onCreateView(
            LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState
    ) {
        // Inflate the layout for this fragment
        final View view = inflater.inflate(R.layout.fragment_main, container, false);

        ((MainActivity) getActivity()).getSupportActionBar().setDisplayHomeAsUpEnabled(false);

        bleIsConn = false;

        bntSystem = view.findViewById(R.id.button_system);
        btnModem = view.findViewById(R.id.button_modem);
        btnConfig = view.findViewById(R.id.button_config);
        bntStatus = view.findViewById(R.id.button_status);
        btnConsole = view.findViewById(R.id.button_console);
        btnUpgrade = view.findViewById(R.id.button_upgrade);
        btnReboot = view.findViewById(R.id.button_reboot);
        btnCert = view.findViewById(R.id.button_cert);
        btnBleConn = view.findViewById(R.id.button_ble);
        btnReqAccept = view.findViewById(R.id.btnReqAccept);

        bntSystem.setOnClickListener((View.OnClickListener) this);
        btnModem.setOnClickListener((View.OnClickListener) this);
        btnConfig.setOnClickListener((View.OnClickListener) this);
        bntStatus.setOnClickListener((View.OnClickListener) this);
        btnConsole.setOnClickListener((View.OnClickListener) this);
        btnUpgrade.setOnClickListener((View.OnClickListener) this);
        btnReboot.setOnClickListener((View.OnClickListener) this);
        btnCert.setOnClickListener((View.OnClickListener) this);
        btnBleConn.setOnClickListener((View.OnClickListener) this);

        return view;
    }

    @Override
    public void onResume() {
        super.onResume();
        if(MainActivity.getInstance().uartState() == 20) {
            // UART_PROFILE_CONNECTED
            setConnectStatus(true);
        } else {
            // UART_PROFILE_DISCONNECTED
            setConnectStatus(false);
        }
    }

    @Override
    public void onClick(View v) {
        Log.d(TAG, "setOnClickListener");

        if(bleIsConn == false){
            if(v.getId() != R.id.button_ble) {
                final DisconnectDialogFragment fragment = DisconnectDialogFragment.getInstance(R.string.do_ble_connect);
                fragment.show(MainActivity.getInstance().getSupportFragmentManager(), null);
                return;
            }
        }

        if(MainActivity.getInstance().getUartRxService() == false && v.getId() != R.id.button_ble) {
            return;
        }

        switch(v.getId()) {
            case R.id.button_system:
                NavHostFragment.findNavController(MainFragment.this)
                        .navigate(R.id.action_MainFragment_to_SystemFragment);
                break;
            case R.id.button_modem:
                NavHostFragment.findNavController(MainFragment.this)
                        .navigate(R.id.action_MainFragment_to_ModemFragment);
                break;
            case R.id.button_config:
                NavHostFragment.findNavController(MainFragment.this)
                        .navigate(R.id.action_MainFragment_to_ConfigFragment);
                break;
            case R.id.button_status:
                NavHostFragment.findNavController(MainFragment.this)
                        .navigate(R.id.action_MainFragment_to_StatusFragment);
                break;
            case R.id.button_console:
                NavHostFragment.findNavController(MainFragment.this)
                        .navigate(R.id.action_MainFragment_to_ConsoleFragment);
                break;
            case R.id.button_upgrade:
                NavHostFragment.findNavController(MainFragment.this)
                        .navigate(R.id.action_MainFragment_to_UpgradeFragment);
                break;
            case R.id.button_reboot:
                NavHostFragment.findNavController(MainFragment.this)
                        .navigate(R.id.action_MainFragment_to_RebootFragment);
                break;
            case R.id.button_cert:
                NavHostFragment.findNavController(MainFragment.this)
                        .navigate(R.id.action_MainFragment_to_CertFragment);
                break;
            case R.id.button_ble:
                if(btnBleConn.getText().equals(getString(R.string.ble_connect))){
                    ((MainActivity) getActivity()).startConnect();
                }else{
                    ((MainActivity) getActivity()).startDisconnect();
                }
                break;
            case R.id.btnReqAccept:
                doRequestAccept();
                break;
        }
    }

    private void doRequestAccept() {
    }

    public void setConnectStatus(boolean b) {
        if(b){
            bleIsConn = true;
            btnBleConn.setText(getString(R.string.ble_disconnect));
        }else{
            bleIsConn = false;
            btnBleConn.setText(getString(R.string.ble_connect));
        }
    }
}