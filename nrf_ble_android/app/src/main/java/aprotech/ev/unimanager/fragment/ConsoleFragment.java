package aprotech.ev.unimanager.fragment;

import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.squareup.otto.Subscribe;

import org.json.JSONException;

import java.io.UnsupportedEncodingException;

import aprotech.ev.unimanager.json.JsonHandler;
import aprotech.ev.unimanager.MainActivity;
import aprotech.ev.unimanager.R;
import aprotech.ev.unimanager.event.EventData;
import aprotech.ev.unimanager.event.EventHandler;

public class ConsoleFragment extends Fragment {
    private static final String TAG = "ConsoleFrag";

    private TextView empty;
    private EditText field;
    private Button sendButton;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EventHandler.getInstance().register(this);

        try {
            String jsonData = JsonHandler.createConsole("ble ota_en");
            MainActivity.getInstance().sendData(jsonData);
        } catch (UnsupportedEncodingException | JSONException e) {
            e.printStackTrace();
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        ((MainActivity) getActivity()).getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        // Inflate the layout for this fragment
        final View view = inflater.inflate(R.layout.fragment_console, container, false);

        final TextView emptyView = this.empty = view.findViewById(R.id.empty);
        emptyView.setMovementMethod(new ScrollingMovementMethod());

        final EditText field = this.field = view.findViewById(R.id.field);
        field.setOnEditorActionListener((v, actionId, event) -> {
            if (actionId == EditorInfo.IME_ACTION_SEND) {
                try {
                    onSendClicked();
                } catch (UnsupportedEncodingException | JSONException e) {
                    e.printStackTrace();
                }
                return true;
            }
            return false;
        });

        final Button sendButton = this.sendButton = view.findViewById(R.id.action_send);
        sendButton.setOnClickListener(v -> {
            try {
                onSendClicked();
            } catch (UnsupportedEncodingException | JSONException e) {
                e.printStackTrace();
            }
        });

        if (getArguments() != null) {
            String userId = getArguments().getString("userId");
            Log.i(TAG, "userId: " + userId);
        }

        return view;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        EventHandler.getInstance().unregister(this);

        try {
            String jsonData = JsonHandler.createConsole("ble ota_dis");
            MainActivity.getInstance().sendData(jsonData);
        } catch (UnsupportedEncodingException | JSONException e) {
            e.printStackTrace();
        }
    }

    @Subscribe
    public void eventListener(EventData eventData) {
        if(eventData.getCmd() != JsonHandler.CMD_CONSOLE) {
            return;
        }

        for(int i = 0; i < eventData.getBodySize(); i++) {
            switch(eventData.getBody(i).getKey()) {
                case JsonHandler.KEY_CMD:
                    //empty.append((CharSequence) "< ");

                    //String value = (String) eventData.getBody(i).getValue();
                    //byte[] strBuf = value.getBytes(Charset.forName("utf-8"));
                    //String str2 = new String(strBuf);
                    //empty.append(str2);

                    empty.append((CharSequence) eventData.getBody(i).getValue());
                    scrollBottom(empty);
                    break;
            }
        }
    }

    private void onSendClicked() throws UnsupportedEncodingException, JSONException {
        if(MainActivity.getInstance().uartState() != 20) {// UART_PROFILE_CONNECTED
            final DisconnectDialogFragment fragment = DisconnectDialogFragment.getInstance(R.string.do_ble_connect);
            fragment.show(MainActivity.getInstance().getSupportFragmentManager(), null);
            return;
        }

        String text = field.getText().toString();
        if(text.length() == 0)
            return;

        //uartInterface.send(text);
        String jsonData = JsonHandler.createConsole(text);
        MainActivity.getInstance().sendData(jsonData);

        if(!text.endsWith("\n")){
            text += "\n";
        }
        //empty.append((CharSequence) "> ");
        empty.append((CharSequence) text);
        scrollBottom(empty);

        field.setText(null);
        field.requestFocus();
    }

    private void scrollBottom(TextView textView) {
        int lineTop =  textView.getLayout().getLineTop(textView.getLineCount()) ;
        int scrollY = lineTop - textView.getHeight();
        if (scrollY > 0) {
            textView.scrollTo(0, scrollY);
        } else {
            textView.scrollTo(0, 0);
        }
    }
}