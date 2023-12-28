package aprotech.ev.unimanager.fragment;

import android.app.AlertDialog;
import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import org.json.JSONException;

import java.io.UnsupportedEncodingException;

import aprotech.ev.unimanager.MainActivity;
import aprotech.ev.unimanager.R;
import aprotech.ev.unimanager.event.EventHandler;
import aprotech.ev.unimanager.json.JsonHandler;

public class RebootFragment extends Fragment {
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
        final View view = inflater.inflate(R.layout.fragment_reboot, container, false);

        final Button btnReboot = view.findViewById(R.id.button_reboot);
        btnReboot.setOnClickListener(v -> {
            new AlertDialog.Builder(getActivity())
                    .setTitle(R.string.reboot_title)
                    .setMessage(R.string.reboot_message)
                    .setNegativeButton(R.string.no, (dialog, which) -> {

                    })
                    .setPositiveButton(R.string.yes, (dialog, which) -> {
                        try {
                            String jsonData = JsonHandler.createReboot();
                            MainActivity.getInstance().sendData(jsonData);
                        } catch (UnsupportedEncodingException | JSONException e) {
                            e.printStackTrace();
                        }
                    })
                    .show();
        });

        final Button btnReset = view.findViewById(R.id.button_factory);
        btnReset.setOnClickListener(v -> {
            new AlertDialog.Builder(getActivity())
                    .setTitle(R.string.reset_title)
                    .setMessage(R.string.reset_message)
                    .setNegativeButton(R.string.no, (dialog, which) -> {

                    })
                    .setPositiveButton(R.string.yes, (dialog, which) -> {
                        try {
                            String jsonData = JsonHandler.createFactoryReset();
                            MainActivity.getInstance().sendData(jsonData);
                        } catch (UnsupportedEncodingException | JSONException e) {
                            e.printStackTrace();
                        }
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
}