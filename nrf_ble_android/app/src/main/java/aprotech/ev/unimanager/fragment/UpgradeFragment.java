package aprotech.ev.unimanager.fragment;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.ParcelFileDescriptor;
import android.provider.MediaStore;
import android.provider.OpenableColumns;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.MimeTypeMap;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import com.squareup.otto.Subscribe;

import org.json.JSONException;

import aprotech.ev.unimanager.MainActivity;
import aprotech.ev.unimanager.R;
import aprotech.ev.unimanager.binary.BinaryHandler;
import aprotech.ev.unimanager.event.EventData;
import aprotech.ev.unimanager.event.EventHandler;
import aprotech.ev.unimanager.json.JsonHandler;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.util.Timer;
import java.util.TimerTask;

public class UpgradeFragment extends Fragment {
    private static final String TAG = "UniMgrUpgrade";

    private TimerTask timerTask;
    private Timer timer = new Timer();

    private static final int SELECT_FILE_REQ = 1;
    private static final int SELECT_INIT_FILE_REQ = 2;

    private static final int FWUP_STATE_IDLE = 0;
    private static final int FWUP_STATE_START = 1;
    private static final int FWUP_STATE_SEND = 2;
    private static final int FWUP_STATE_DONE = 3;

    // response code
    private static final int FW_SUCCESS         = 0;
    private static final int FW_ERROR           = 1;
    private static final int FW_ERROR_FRAME     = 2;
    private static final int FW_ERROR_CRC       = 3;
    private static final int FW_ERROR_VERIFY    = 4;
    private static final int FW_ERROR_FLASH     = 5;

    private static final int READ_SZ = 235;
    private static final int TRANSFER_BOOSTER = 32;

    private static final int MSG_UPDATE_PROGRESS = 1;
    private static final int MSG_TIMEOUT = 2;

    private BoosterSendThread runnable;

    private TextView fileNameView;
    private TextView fileSizeView;
    private TextView fileStatusView;
    private TextView textPercentage;
    private TextView textUploading;
    private ProgressBar progressBar;
    private Button selectFileButton;
    private Button uploadButton;
    private Button uploadButtonOld;

    private File fileFirmware;
    private boolean statusOk;

    private int fileSize;
    private int fileFragment;
    private int fileFragmentIndex;

    private int prevState;
    private int prevCmd;
    private int prevCode;
    private int prevIndex;
    private int upgradeState;
    private int packetIndex;
    private int timeoutCnt;
    private int errorCnt;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        runnable = new BoosterSendThread();
        Thread thread = new Thread(runnable);
        thread.setDaemon(true);
        thread.start();
        EventHandler.getInstance().register(this);

        if (ContextCompat.checkSelfPermission(getActivity(), Manifest.permission.READ_EXTERNAL_STORAGE)!= PackageManager.PERMISSION_GRANTED) {
            if (ActivityCompat.shouldShowRequestPermissionRationale(getActivity(),Manifest.permission.READ_EXTERNAL_STORAGE)) {
            } else {
                ActivityCompat.requestPermissions(getActivity(),
                        new String[]{Manifest.permission.READ_EXTERNAL_STORAGE},
                        1);
            }
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        ((MainActivity) getActivity()).getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        // Inflate the layout for this fragment
        final View view = inflater.inflate(R.layout.fragment_upgrade, container, false);

        try {
            String jsonData = JsonHandler.CreateFrameFormatChangeCmd(1);
            MainActivity.getInstance().sendData(jsonData);
            MainActivity.getInstance().setFrameFormat(1);
        } catch (UnsupportedEncodingException | JSONException e) {
            e.printStackTrace();
        }

        fileNameView = view.findViewById(R.id.file_name);
        fileSizeView = view.findViewById(R.id.file_size);
        fileStatusView = view.findViewById(R.id.file_status);
        selectFileButton = view.findViewById(R.id.button_local);
        uploadButton = view.findViewById(R.id.button_upgrade);
        uploadButtonOld = view.findViewById(R.id.button_upgrade_old);
        textPercentage = view.findViewById(R.id.textviewProgress);
        textUploading = view.findViewById(R.id.textviewUploading);
        progressBar = view.findViewById(R.id.progressbar_file);

        upgradeState = FWUP_STATE_IDLE;
        packetIndex = 5000;
        timeoutCnt = 0;
        errorCnt = 0;

        selectFileButton.setOnClickListener(v -> {
            Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
            //intent.setType("*/*");
            intent.setType("application/octet-stream");
            startActivityForResult(intent,SELECT_FILE_REQ);
        });

        uploadButton.setOnClickListener(v -> {
            if(MainActivity.getInstance().uartState() != 20) { // UART_PROFILE_DISCONNECTED

                progressBar.setIndeterminate(true);
                progressBar.setProgress(0);
                textUploading.setText(R.string.dfu_status_aborting);
                textPercentage.setText(null);

                final DisconnectDialogFragment fragment = DisconnectDialogFragment.getInstance(R.string.do_ble_connect);
                fragment.show(MainActivity.getInstance().getSupportFragmentManager(), null);
                return;
            }

            // Check whether the selected file is a HEX file (we are just checking the extension)
            if (!statusOk) {
                Toast.makeText(getActivity(), R.string.dfu_file_status_invalid_message, Toast.LENGTH_LONG).show();
                return;
            }
            BinaryHandler.setDefaultMode(true);
            if(uploadButton.getText().equals(getString(R.string.dfu_action_upload))) {
                upgradeState = FWUP_STATE_IDLE;
                packetIndex = 5000;
                timeoutCnt = 0;
                errorCnt = 0;
                fwUpgrade(BinaryHandler.CMD_NONE, 0, packetIndex);
            } else {
                fwErrorCmd(R.string.dfu_status_aborting);
                upgradeState = FWUP_STATE_IDLE;
                timeoutCnt = 0;
                errorCnt = 0;
                new AlertDialog.Builder(getActivity())
                        .setTitle(R.string.dfu_cancel_title)
                        .setMessage(R.string.dfu_canceled)
                        .setPositiveButton(R.string.yes, (dialog, which) -> {
                        })
                        .show();
            }
        });
        uploadButtonOld.setOnClickListener(v -> {
            if(MainActivity.getInstance().uartState() != 20) { // UART_PROFILE_DISCONNECTED

                progressBar.setIndeterminate(true);
                progressBar.setProgress(0);
                textUploading.setText(R.string.dfu_status_aborting);
                textPercentage.setText(null);

                final DisconnectDialogFragment fragment = DisconnectDialogFragment.getInstance(R.string.do_ble_connect);
                fragment.show(MainActivity.getInstance().getSupportFragmentManager(), null);
                return;
            }
            BinaryHandler.setDefaultMode(false);
            // Check whether the selected file is a HEX file (we are just checking the extension)
            if (!statusOk) {
                Toast.makeText(getActivity(), R.string.dfu_file_status_invalid_message, Toast.LENGTH_LONG).show();
                return;
            }

            if(uploadButtonOld.getText().equals(getString(R.string.dfu_action_upload_old))) {
                upgradeState = FWUP_STATE_IDLE;
                packetIndex = 5000;
                timeoutCnt = 0;
                errorCnt = 0;
                fwUpgrade(BinaryHandler.CMD_NONE, 0, packetIndex);
            } else {
                fwErrorCmd(R.string.dfu_status_aborting);
                upgradeState = FWUP_STATE_IDLE;
                timeoutCnt = 0;
                errorCnt = 0;
                new AlertDialog.Builder(getActivity())
                        .setTitle(R.string.dfu_cancel_title)
                        .setMessage(R.string.dfu_canceled)
                        .setPositiveButton(R.string.yes, (dialog, which) -> {
                        })
                        .show();
            }
        });
        return view;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        EventHandler.getInstance().unregister(this);
        stopTimerTask();
        runnable.sendStop();
        runnable.stopThread();
        //if(MainActivity.getInstance().getUartRxService() == true) {
            BinaryHandler.binarySend(BinaryHandler.CMD_FRAME_CHANGE, 0, (char) 0);
        //}
        MainActivity.getInstance().setFrameFormat(0);
     }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public void onActivityResult(final int requestCode, final int resultCode, final Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode != Activity.RESULT_OK)
            return;

        // and read new one
        Uri uri = data.getData();
        String path = uri.getPath();
        Log.i(TAG, "uri = " + uri.toString());
        if(path.contains("document/raw:")){
            path = path.replace("/document/raw:","");
        }
        Log.i(TAG, "filePath : " + path);
        Log.i(TAG, "fileStreamUri : " + uri.toString());
        String displayName ="";
        Cursor cursor = getActivity().getContentResolver().query(uri, null, null, null, null);
        if (Build.VERSION.SDK_INT >= 29) {
            if (cursor != null && cursor.moveToFirst()) {
                displayName = cursor.getString(cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME));
                fileSize = (int)cursor.getLong(cursor.getColumnIndex(OpenableColumns.SIZE));
                path = cursor.getString(cursor.getColumnIndex(MediaStore.Files.FileColumns.RELATIVE_PATH));
            }
            Log.i(TAG, "displayName: " + displayName);

            String currPath = Environment.getExternalStorageDirectory() + File.separator + path + displayName;

            InputStream in = null;//src
            try {
                ParcelFileDescriptor desc = getActivity().getContentResolver().openFileDescriptor(uri, "r", null);
                in = new FileInputStream(desc.getFileDescriptor());
            } catch (FileNotFoundException e) {
                e.printStackTrace();
                return;
            }

            File localImgFile = new File(getContext().getFilesDir(), displayName);
            if(localImgFile.exists()) localImgFile.delete();
            path = localImgFile.getPath();
            Log.i(TAG, "path: " + path);

            if (in != null) {
                try {
                    OutputStream out = new FileOutputStream(localImgFile);//dst
                    try {
                        // Transfer bytes from in to out
                        byte[] buf = new byte[1024];
                        int len;
                        while ((len = in.read(buf)) > 0) {
                            out.write(buf, 0, len);
                        }
                    } finally {
                        out.close();
                    }
                }catch(IOException e){
                    Toast.makeText(getContext(), e.getLocalizedMessage(), Toast.LENGTH_SHORT).show();
                    return;
                } finally {
                    try {
                        in.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
            fileFirmware = new File(path);

            //fileSize = (int) cursor.getLong(cursor.getColumnIndex(MediaStore.MediaColumns.SIZE));// 1 SIZE
//            fileFragment = fileSize / READ_SZ + (fileSize % READ_SZ > 0 ? 1 : 0);
            fileNameView.setText(displayName);
            fileSizeView.setText(getString(R.string.dfu_file_size_text, fileSize));
            statusOk = MimeTypeMap.getFileExtensionFromUrl(displayName).matches("(?i)IMG");
        }else{
            cursor.moveToPosition(0);
            fileFirmware = new File(path);
            fileNameView.setText(fileFirmware.getName());
            statusOk = MimeTypeMap.getFileExtensionFromUrl(fileFirmware.getName()).matches("(?i)IMG");
        }
        fileSize = (int) cursor.getLong(cursor.getColumnIndex(MediaStore.MediaColumns.SIZE));// 1 SIZE
        fileFragment = fileSize / READ_SZ + (fileSize % READ_SZ > 0 ? 1 : 0);
        fileSizeView.setText(getString(R.string.dfu_file_size_text, fileSize));

        fileStatusView.setText(statusOk ? R.string.dfu_file_status_ok : R.string.dfu_file_status_invalid);

        if(MainActivity.getInstance().getUartRxService() == true) {
            fwErrorCmd(R.string.dfu_status_uploading);
            upgradeState = FWUP_STATE_IDLE;
            timeoutCnt = 0;
            errorCnt = 0;
        }
    }

    @Subscribe
    public void eventListener(EventData eventData) {
        if(BinaryHandler.isDefaultMode() && !(eventData.getCmdIx() == BinaryHandler.CMD_FW_START ||
                eventData.getCmdIx() == BinaryHandler.CMD_FW_SEND ||
                eventData.getCmdIx() == BinaryHandler.CMD_FW_DONE ||
                eventData.getCmdIx() == BinaryHandler.CMD_FW_ERROR ) )
            return;
        if(!BinaryHandler.isDefaultMode() && !(eventData.getCmdIx() == BinaryHandler.CMD_FW_START_OLD ||
                eventData.getCmdIx() == BinaryHandler.CMD_FW_SEND_OLD ||
                eventData.getCmdIx() == BinaryHandler.CMD_FW_DONE_OLD ||
                eventData.getCmdIx() == BinaryHandler.CMD_FW_ERROR_OLD ) )
            return;

        int cmd = eventData.getCmdIx();
        int resultCode = 0;
        int packetIx = 0;

        for(int i = 0; i < eventData.getBodySize(); i++) {
            switch(eventData.getBody(i).getKey()) {
                case "code":
                    resultCode = (int) eventData.getBody(i).getValue();
                    break;
                case "index":
                    packetIx = (int) eventData.getBody(i).getValue();
                    break;
            }
        }

        if(cmd != BinaryHandler.CMD_NONE) {
            fwUpgrade(cmd, resultCode, packetIx);
        }
    }

    private void fwUpgrade(int cmd, int code, int index) {
        stopTimerTask();

        prevState = upgradeState;
        prevCmd = cmd;
        prevCode = code;
        prevIndex = index;

        Log.i(TAG, "state: " + upgradeState + " cmd: " + cmd + " code: " + code + " ix: " + index);

        int ret = 0;
        switch(upgradeState) {
            case FWUP_STATE_IDLE:
                if(cmd == BinaryHandler.CMD_NONE) {
                    ret = fwStartCmd();
                    upgradeState = FWUP_STATE_START;
                } else {
                    upgradeState = FWUP_STATE_IDLE;
                }
                break;
            case FWUP_STATE_START:
                System.out.println("FWUP_STATE_START " + cmd + " " + BinaryHandler.isDefaultMode());
                if(!BinaryHandler.isDefaultMode()){
                    if(cmd == BinaryHandler.CMD_FW_START_OLD) {

                        if(code == 0) {
                            upgradeState = FWUP_STATE_SEND;
                            ret = fwSendCmd(0);
                        } else {
                            upgradeState = FWUP_STATE_IDLE;
                            ret = fwErrorCmd(R.string.dfu_status_aborting);
                        }
                    } else if(cmd == BinaryHandler.CMD_FW_ERROR) {
                        upgradeState = FWUP_STATE_IDLE;
                    } else {
                        upgradeState = FWUP_STATE_IDLE;
                        ret = fwErrorCmd(R.string.dfu_status_aborting);
                    }
                }else{
                    if(cmd == BinaryHandler.CMD_FW_START) {
                        if(code == 0) {
                            upgradeState = FWUP_STATE_SEND;
                            ret = fwSendCmd(0);
                        } else {
                            upgradeState = FWUP_STATE_IDLE;
                            ret = fwErrorCmd(R.string.dfu_status_aborting);
                        }
                    } else if(cmd == BinaryHandler.CMD_FW_ERROR) {
                        upgradeState = FWUP_STATE_IDLE;
                    } else {
                        upgradeState = FWUP_STATE_IDLE;
                        ret = fwErrorCmd(R.string.dfu_status_aborting);
                    }
                }

                break;
            case FWUP_STATE_SEND:
                System.out.println("FWUP_STATE_SEND " + cmd + " " + BinaryHandler.isDefaultMode());
                runnable.sendStop();
                if(!BinaryHandler.isDefaultMode()){
                    if(cmd == BinaryHandler.CMD_FW_SEND_OLD) {
                        if(code == 0) {
                            errorCnt = 0;
                            if (index == fileFragment) {
                                upgradeState = FWUP_STATE_DONE;
                                ret = fwDoneCmd();
                            } else {
                                upgradeState = FWUP_STATE_SEND;
                                ret = fwSendCmd(index);
                            }
                        } else {
                            if(errorCnt++ > 1) {
                                upgradeState = FWUP_STATE_IDLE;
                                ret = fwErrorCmd(R.string.dfu_status_aborting);
                            } else {
                                upgradeState = FWUP_STATE_SEND;
                                ret = fwSendCmd(index - 1);
                            }
                        }
                    } else if(cmd == BinaryHandler.CMD_FW_ERROR_OLD) {
                        upgradeState = FWUP_STATE_IDLE;
                        fwErrorPopup(code);
                    } else {
                        upgradeState = FWUP_STATE_IDLE;
                        ret = fwErrorCmd(R.string.dfu_status_aborting);
                    }
                }else{
                    if(cmd == BinaryHandler.CMD_FW_SEND) {
                        if(code == 0) {
                            errorCnt = 0;
                            if (index == fileFragment) {
                                upgradeState = FWUP_STATE_DONE;
                                ret = fwDoneCmd();
                            } else {
                                upgradeState = FWUP_STATE_SEND;
                                ret = fwSendCmd(index);
                            }
                        } else {
                            if(errorCnt++ > 1) {
                                upgradeState = FWUP_STATE_IDLE;
                                ret = fwErrorCmd(R.string.dfu_status_aborting);
                            } else {
                                upgradeState = FWUP_STATE_SEND;
                                ret = fwSendCmd(index - 1);
                            }
                        }
                    } else if(cmd == BinaryHandler.CMD_FW_ERROR) {
                        upgradeState = FWUP_STATE_IDLE;
                        fwErrorPopup(code);
                    } else {
                        upgradeState = FWUP_STATE_IDLE;
                        ret = fwErrorCmd(R.string.dfu_status_aborting);
                    }
                }

                break;
            case FWUP_STATE_DONE:
                System.out.println("FWUP_STATE_DONE " + cmd + " " + BinaryHandler.isDefaultMode());
                if(!BinaryHandler.isDefaultMode()){
                    if (cmd == BinaryHandler.CMD_FW_DONE_OLD) {
                        if (code == 0) {
                            upgradeState = FWUP_STATE_IDLE;
                        } else {
                            upgradeState = FWUP_STATE_IDLE;
                            ret = fwErrorCmd(R.string.dfu_status_aborting);
                        }
                    } else if (cmd == BinaryHandler.CMD_FW_ERROR_OLD) {
                        upgradeState = FWUP_STATE_IDLE;
                    } else {
                        upgradeState = FWUP_STATE_IDLE;
                        ret = fwErrorCmd(R.string.dfu_status_aborting);
                    }
                }else {
                    if (cmd == BinaryHandler.CMD_FW_DONE) {
                        if (code == 0) {
                            upgradeState = FWUP_STATE_IDLE;
                        } else {
                            upgradeState = FWUP_STATE_IDLE;
                            ret = fwErrorCmd(R.string.dfu_status_aborting);
                        }
                    } else if (cmd == BinaryHandler.CMD_FW_ERROR) {
                        upgradeState = FWUP_STATE_IDLE;
                    } else {
                        upgradeState = FWUP_STATE_IDLE;
                        ret = fwErrorCmd(R.string.dfu_status_aborting);
                    }
                }
                break;
            default:
                upgradeState = FWUP_STATE_IDLE;
                break;
        }

        if(ret == 1 && timeoutCnt == 0) {
            startTimerTask();
        }
        timeoutCnt = 0;
    }

    private int fwStartCmd() {
        int ret = 1;
        byte[] buf = new byte[32];
        char ix = 0;
        buf[ix++] = 1;                                  // major version
        buf[ix++] = 1;                                  // minor version
        buf[ix++] = 1;                                  // build version
        buf[ix++] = 0;                                  // crypto
        buf[ix++] = (byte) ((fileSize >> 24) & 0xff);   // file size
        buf[ix++] = (byte) ((fileSize >> 16) & 0xff);   // file size
        buf[ix++] = (byte) ((fileSize >> 8) & 0xff);    // file size
        buf[ix++] = (byte) (fileSize & 0xff);           // file size
        buf[ix++] = (byte)((fileFragment >> 8) & 0xff); // file fragment
        buf[ix++] = (byte)(fileFragment & 0xff);        // file fragment
        buf[ix++] = (byte) 0xE1;                        // magic number

        BinaryHandler.binarySend(BinaryHandler.isDefaultMode() ? BinaryHandler.CMD_FW_START : BinaryHandler.CMD_FW_START_OLD, ++packetIndex, buf, ix);
        Log.i(TAG, "fwStartCmd() ver: " + (int)buf[0] + (int)buf[1] + (int)buf[2] +
                " fileSz: " + fileSize + " fragment: " + fileFragment);

        //progressBar.setVisibility(View.VISIBLE);
        //textPercentage.setVisibility(View.VISIBLE);
        textPercentage.setText(R.string.dfu_uploading_percentage_label);
        textUploading.setText(R.string.dfu_status_uploading);
        //textUploading.setVisibility(View.VISIBLE);
        selectFileButton.setEnabled(false);
        if(!BinaryHandler.isDefaultMode())
            uploadButtonOld.setText(R.string.dfu_action_upload_cancel);
        else
            uploadButton.setText(R.string.dfu_action_upload_cancel);

        return ret;
    }

    private int fwSendCmd(int send_index) {
        int ret = 1;
        runnable.sendStart(send_index);
        return ret;
    }

    private int fwDoneCmd() {
        int ret = 1;
        BinaryHandler.binarySend(!BinaryHandler.isDefaultMode()? BinaryHandler.CMD_FW_DONE_OLD : BinaryHandler.CMD_FW_DONE, ++packetIndex, BinaryHandler.FW_SUCCESS);
        Log.i(TAG, "fwDoneCmd:");

        textUploading.setText("Install...");
        selectFileButton.setEnabled(true);
        if(!BinaryHandler.isDefaultMode()){
            if(uploadButtonOld.getText().equals(getString(R.string.dfu_action_upload_cancel))) {
                new AlertDialog.Builder(getActivity())
                        .setTitle(R.string.dfu_success_title)
                        .setMessage(R.string.dfu_success)
                        .setPositiveButton(R.string.yes, (dialog, which) -> {
                        })
                        .show();
            }
            uploadButtonOld.setText(R.string.dfu_action_upload_old);
        }
        else{
            if(uploadButton.getText().equals(getString(R.string.dfu_action_upload_cancel))) {
                new AlertDialog.Builder(getActivity())
                        .setTitle(R.string.dfu_success_title)
                        .setMessage(R.string.dfu_success)
                        .setPositiveButton(R.string.yes, (dialog, which) -> {
                        })
                        .show();
            }
            uploadButton.setText(R.string.dfu_action_upload);
        }


        return ret;
    }

    private int fwErrorCmd(int resid) {
        int ret = 1;
        BinaryHandler.binarySend(BinaryHandler.isDefaultMode()? BinaryHandler.CMD_FW_ERROR : BinaryHandler.CMD_FW_ERROR_OLD,
                0, BinaryHandler.FW_ERROR);
        Log.i(TAG, "fwErrorCmd:");
        //progressBar.setVisibility(View.VISIBLE);
        //textPercentage.setVisibility(View.VISIBLE);
        textPercentage.setText(R.string.dfu_uploading_percentage_label);
        textUploading.setText(resid);
        //textUploading.setVisibility(View.VISIBLE);
        selectFileButton.setEnabled(true);
        progressBar.setIndeterminate(true);
        if(!BinaryHandler.isDefaultMode()){
            uploadButtonOld.setText(R.string.dfu_action_upload_old);
        }else {
            uploadButton.setText(R.string.dfu_action_upload);
        }
        return ret;
    }

    private void fwErrorPopup(int code) {
        int messageId = R.string.dfu_error;
        System.out.println("ERROR " + code + " " + BinaryHandler.isDefaultMode());
        switch(code) {
            case FW_ERROR_CRC:
                messageId = R.string.dfu_error_invalid;
                break;
            case FW_ERROR_VERIFY:
                messageId = R.string.dfu_error_invalid;
                break;
            default:
                messageId = R.string.dfu_error;
                break;
            }
        if(!BinaryHandler.isDefaultMode()){
            if (uploadButtonOld.getText().equals(getString(R.string.dfu_action_upload_cancel))) {
                new AlertDialog.Builder(getActivity())
                        .setTitle(R.string.dfu_error_title)
                        .setMessage(messageId)
                        .setPositiveButton(R.string.yes, (dialog, which) -> {
                        })
                        .show();
            }
            uploadButtonOld.setText(R.string.dfu_action_upload_old);
        }else {
            if (uploadButton.getText().equals(getString(R.string.dfu_action_upload_cancel))) {
                new AlertDialog.Builder(getActivity())
                        .setTitle(R.string.dfu_error_title)
                        .setMessage(messageId)
                        .setPositiveButton(R.string.yes, (dialog, which) -> {
                        })
                        .show();
            }
            uploadButton.setText(R.string.dfu_action_upload);
        }
        progressBar.setIndeterminate(true);
        progressBar.setProgress(0);
        textUploading.setText(R.string.dfu_status_aborting);
        textPercentage.setText(null);
        selectFileButton.setEnabled(true);
    }

    final Handler handler = new Handler() {
        public void handleMessage(Message msg) {
            switch(msg.arg1) {
                case MSG_UPDATE_PROGRESS:
                    textPercentage.setText((fileFragmentIndex * 100) / fileFragment + "%");
                    progressBar.setIndeterminate(false);
                    progressBar.setProgress((fileFragmentIndex * 100) / fileFragment);
                    break;
                case MSG_TIMEOUT:
                    upgradeState = prevState;
                    timeoutCnt = 1;
                    fwUpgrade(prevCmd, prevCode, prevIndex);
                    break;
            }
        }
    };

    private void startTimerTask()
    {
        stopTimerTask();

        timerTask = new TimerTask() {
            @Override
            public void run() {
                if(MainActivity.getInstance().getUartRxService() == true) {
                    Message msg = handler.obtainMessage();
                    msg.arg1 = MSG_TIMEOUT;
                    handler.sendMessage(msg);
                }
            }
        };
        timer.schedule(timerTask,1000 * 5);
    }

    private void stopTimerTask() {
        if(timerTask != null) {
            timerTask.cancel();
            timerTask = null;
        }
    }

    class BoosterSendThread extends Thread {
        int startFlag = 0;
        int send_index = 0;
        boolean running = true;

        public void run() {
            while(running) {
                try {
                    if(startFlag == 0) {
                        Thread.sleep(10);
                        continue;
                    }

                    int boosterCnt = send_index + TRANSFER_BOOSTER;
                    boosterCnt -= boosterCnt % TRANSFER_BOOSTER;
                    for (int i = send_index; i < boosterCnt; i++) {
                        if(startFlag == 0) {
                            break;
                        }

                        if (i == fileFragment) {
                            break;
                        }

                        fileFragmentIndex = i;
                        byte[] buf = new byte[READ_SZ];
                        FileInputStream fis = new FileInputStream(fileFirmware);
                        fis.skip(fileFragmentIndex * READ_SZ);
                        int size = fis.read(buf);
                        //Log.i(TAG, "read size:" + size);
                        //Log.v(TAG, "dat: " + BinaryHandler.byteArrayToHex(buf, size));
                        fis.close();

                        packetIndex = ++fileFragmentIndex;
                        BinaryHandler.binarySend(BinaryHandler.isDefaultMode() ? BinaryHandler.CMD_FW_SEND : BinaryHandler.CMD_FW_SEND_OLD, packetIndex, buf, size);
                        Log.i(TAG, "fwSendCmd: packetIndex " + packetIndex);
                        Thread.sleep(40);
                    }
                    startFlag = 0;

                    Message msg = handler.obtainMessage();
                    msg.arg1 = MSG_UPDATE_PROGRESS;
                    handler.sendMessage(msg);
                } catch (IOException | InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

        public void sendStart(int sendIx) {
            send_index = sendIx;
            startFlag = 1;
        }

        public void sendStop() {
            startFlag = 0;
        }

        public void stopThread() {
            running = false;
        }
    }
}