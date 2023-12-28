package aprotech.ev.unimanager.fragment;

import android.app.Dialog;
import android.content.DialogInterface;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Message;
import android.util.JsonReader;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.DialogFragment;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;

import javax.net.ssl.HttpsURLConnection;

import aprotech.ev.unimanager.MainActivity;
import aprotech.ev.unimanager.R;
import aprotech.ev.unimanager.service.UnievHostApduService;

public class SigninDialogFragment extends DialogFragment {
    private static final String TAG = "UniCh";
    private boolean signin = false;

    public static SigninDialogFragment getInstance() {
        final SigninDialogFragment fragment = new SigninDialogFragment();
        final Bundle args = new Bundle();
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        Log.v(TAG, "onCreateDialog()");
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        // Get the layout inflater
        LayoutInflater inflater = requireActivity().getLayoutInflater();

        // Inflate and set the layout for the dialog
        // Pass null as the parent view because its going in the dialog layout
        builder.setView(inflater.inflate(R.layout.fragment_signin, null))
                // Add action buttons
                .setPositiveButton(R.string.signin, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int id) {

                    }
                })
                .setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dismiss();
                    }
                });
        return builder.create();
    }

    @Override
    public void onStart() {
        super.onStart();
        AlertDialog dialog = (AlertDialog) getDialog();
        if(dialog != null){
            Button posBtn = dialog.getButton(Dialog.BUTTON_POSITIVE);
            posBtn.setOnClickListener(v -> {
                // sign in the user ...
                final EditText username= dialog.findViewById(R.id.username);
                final EditText password= dialog.findViewById(R.id.password);
                Log.i(TAG, "manager_id: " + username.getText().toString());
                Log.i(TAG, "manager_pw: " + password.getText().toString());
                requestLogin(username.getText().toString(), password.getText().toString());
            });
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if(signin == false)
        {
            System.exit(0);
        }
    }

    private void requestLogin(String userName, String password){
        AsyncTask.execute(() -> {

            try {
                JSONObject obj = new JSONObject();
                obj.put("userId", userName);
                obj.put("password", password);
                URL endpoint = new URL("https://mgr");
                HttpsURLConnection connection = (HttpsURLConnection) endpoint.openConnection();
                connection.setDefaultUseCaches(false);
                //connection.setDoInput(true); // 서버에서 읽기 모드 지정
                connection.setDoOutput(true); // 서버로 쓰기 모드 지정
                connection.setRequestMethod("POST"); // 전송 방식은 POST

                connection.setRequestProperty("content-type", "application/json; charset=UTF-8");

                Log.d(TAG, obj.toString());

                try(OutputStream os=connection.getOutputStream()){
                    byte[] input = obj.toString().getBytes("utf-8");
                    os.write(input, 0, input.length);
                }

                StringBuilder response = new StringBuilder();
                String result = "";

                Log.d(TAG, connection.getResponseMessage() +", " + connection.getResponseCode());
                Message msg = new Message();
                msg.what = 0;
                switch(connection.getResponseCode()){
                    case HttpURLConnection.HTTP_OK:
                        Log.d(TAG,"SUCCESS");
                        JsonReader reader = new JsonReader(new InputStreamReader(connection.getInputStream(), "utf-8"));
                        reader.beginObject();
                        while(reader.hasNext()){
                            String key = reader.nextName();
                            if(key.equals("message")){
                                String value = reader.nextString();
                                if(value.contains("성공")){
                                    result = "success";

                                }else{
                                    result = value;
                                }
                                break;
                            }
                        }

                        if(result.equals("success")){
                            msg.obj = "로그인 되었습니다.";
                        }else{
                            msg.obj = result;
                        }
                        Log.i(TAG, response.toString());


                        ((MainActivity)getActivity()).sendMessage(msg);

                        if(result.equals("success")){
                            signin = true;
                            UnievHostApduService.createNdefMessage("BLEONOFFTAG");
                            dismiss();
                        }
                        break;
                    case HttpURLConnection.HTTP_UNAUTHORIZED:
                        msg.obj = "아이디 또는 비밀번호가 틀렸습니다.";
                        ((MainActivity)getActivity()).sendMessage(msg);
                        break;
                    default:
                        msg.obj = "로그인에 실패 하였습니다. " + connection.getResponseCode();

                }

            } catch (IOException | JSONException e) {
                e.printStackTrace();
            }
        });
    }
}
