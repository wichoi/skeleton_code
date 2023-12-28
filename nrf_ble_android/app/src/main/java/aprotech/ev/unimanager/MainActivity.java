package aprotech.ev.unimanager;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import org.json.JSONException;

import java.io.UnsupportedEncodingException;

import aprotech.ev.unimanager.actvity.ScanDeviceListActivity;
import aprotech.ev.unimanager.binary.BinaryHandler;
import aprotech.ev.unimanager.fragment.DisconnectDialogFragment;
import aprotech.ev.unimanager.fragment.MainFragment;
import aprotech.ev.unimanager.fragment.SigninDialogFragment;
import aprotech.ev.unimanager.json.JsonHandler;
import aprotech.ev.unimanager.service.UartService;
import aprotech.ev.unimanager.service.UnievHostApduService;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "UniMgr";

    private static final int REQUEST_SELECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;
    private static final int UART_PROFILE_CONNECTED = 20;
    private static final int UART_PROFILE_DISCONNECTED = 21;
    private static final int PERMISSION_REQUEST_LOCATION = 1;

    private int mState = UART_PROFILE_DISCONNECTED;
    private UartService mService = null;
    private BluetoothDevice mDevice = null;
    private BluetoothAdapter mBtAdapter = null;

    private static Toast toast;
    private static MainActivity appInstance;
    public static MainActivity getInstance(){
        return appInstance;
    }

    private String devAddr = "";
    private String devName = "";

    private int frameFormat = 0; // 0(json) 1(binary)

    private Handler taskHandler = new Handler(Looper.getMainLooper()){
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            switch(msg.what){
                case 0:
                    toastMessage(msg.obj.toString());
                    break;
            }
        }
    };

    public void sendMessage(Message msg){
        taskHandler.sendMessage(msg);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        appInstance = this;

        mBtAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBtAdapter == null) {
            toastMessage("Bluetooth is not available");
            finish();
            return;
        }

        Intent bindIntent = new Intent(this, UartService.class);
        bindService(bindIntent, mServiceConnection, Context.BIND_AUTO_CREATE);
        LocalBroadcastManager.getInstance(this).registerReceiver(UARTStatusChangeReceiver, makeGattUpdateIntentFilter());

        UnievHostApduService.createDefaultMessage();
        UnievHostApduService.setCurrentAppState(UnievHostApduService.APP_STATE_RUNNING);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED
                || checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED
                    || checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED
                    || checkSelfPermission(Manifest.permission.MANAGE_EXTERNAL_STORAGE)!= PackageManager.PERMISSION_GRANTED
            ) {
                requestPermissions(new String[] { Manifest.permission.ACCESS_FINE_LOCATION,
                                        Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.MANAGE_EXTERNAL_STORAGE,
                }, PERMISSION_REQUEST_LOCATION);
                return;
            }
        }
/*
        final SigninDialogFragment fragment = SigninDialogFragment.getInstance();
        fragment.show(getSupportFragmentManager(), null);
*/
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (!mBtAdapter.isEnabled()) {
            Log.i(TAG, "onResume - BT not enabled yet");
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy()");

        try {
            LocalBroadcastManager.getInstance(this).unregisterReceiver(UARTStatusChangeReceiver);
        } catch (Exception ignore) {
            Log.e(TAG, ignore.toString());
        }
        unbindService(mServiceConnection);
        UnievHostApduService.createDefaultMessage();
        mService.stopSelf();
        mService= null;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode) {

            case REQUEST_SELECT_DEVICE:
                //When the DeviceListActivity return, with the selected device address
                if (resultCode == Activity.RESULT_OK && data != null) {
                    String deviceAddress = data.getStringExtra(BluetoothDevice.EXTRA_DEVICE);
                    mDevice = BluetoothAdapter.getDefaultAdapter().getRemoteDevice(deviceAddress);
                    Log.d(TAG, "... onActivityResultdevice.address==" + mDevice + "mserviceValue" + mService);
                    //((TextView) findViewById(R.id.connectedDevice)).setText(mDevice.getName() + " - connecting");
                    devAddr = deviceAddress;
                    devName = mDevice.getName();
                    if (mService.connect(deviceAddress)) {
                    }
                }
                break;
            case REQUEST_ENABLE_BT:
                // When the request to enable Bluetooth returns
                if (resultCode == Activity.RESULT_OK) {
                    toastMessage("Bluetooth has turned on ");

                } else {
                    // User did not enable Bluetooth or an error occurred
                    Log.d(TAG, "BT not enabled");
                    toastMessage( "Problem in BT Turning ON ");
                    finish();
                }
                break;
            default:
                Log.e(TAG, "wrong request code");
                break;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode) {
            case PERMISSION_REQUEST_LOCATION:
                if(grantResults[0] != PackageManager.PERMISSION_GRANTED){
                    Toast.makeText(this, "All the permissions must be granted", Toast.LENGTH_SHORT).show();
                    finish();
                    return;
                }
/*
                final SigninDialogFragment fragment = SigninDialogFragment.getInstance();
                fragment.show(getSupportFragmentManager(), null);
*/
                break;
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()){
            case R.id.action_settings:{
                StringBuilder text = new StringBuilder(getString(R.string.about_text));
                try {
                    String version = getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
                    text.append(version);
                } catch (PackageManager.NameNotFoundException e) {
                    e.printStackTrace();
                }
                new AlertDialog.Builder(this)
                        .setTitle(R.string.about_title)
                        .setMessage(text)
                        .setPositiveButton(R.string.ok, null)
                        .show();
                return true;
            }
            case android.R.id.home:{
                //finish();
                onBackPressed();
                return true;
            }
        }
        return super.onOptionsItemSelected(item);
    }

    public void toastMessage(String msg) {
        if(toast == null) {
            toast = Toast.makeText(this, msg, Toast.LENGTH_SHORT);
        } else {
            toast.setText(msg);
        }
        toast.show();
    }

    //UART service connected/disconnected
    private ServiceConnection mServiceConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder rawBinder) {
            mService = ((UartService.LocalBinder) rawBinder).getService();
            Log.d(TAG, "onServiceConnected mService= " + mService);
            if (!mService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
        }

        public void onServiceDisconnected(ComponentName classname) {
            ////     mService.disconnect(mDevice);
            mService = null;
        }
    };

    private final BroadcastReceiver UARTStatusChangeReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            final Intent mIntent = intent;
            //*********************//
            if (action.equals(UartService.ACTION_GATT_CONNECTED)) {
                runOnUiThread(() -> {
                    Log.d(TAG, "UART_CONNECT_MSG");
                    MainFragment.getInstance().setConnectStatus(true);
                    //consoleFragment.setConnStatus(true, mDevice);
                    mState = UART_PROFILE_CONNECTED;
                    frameFormat = 0;
                });
            }

            //*********************//
            if (action.equals(UartService.ACTION_GATT_DISCONNECTED)) {
                runOnUiThread(() -> {
                    Log.d(TAG, "UART_DISCONNECT_MSG");
                    MainFragment.getInstance().setConnectStatus(false);
                    //consoleFragment.setConnStatus(false, mDevice);
                    mState = UART_PROFILE_DISCONNECTED;
                    mService.close();
                    //setUiState();
                    final DisconnectDialogFragment fragment = DisconnectDialogFragment.getInstance(R.string.disconnect_text);
                    fragment.show(getSupportFragmentManager(), null);
                    frameFormat = 0;
                });
            }

            //*********************//
            if (action.equals(UartService.ACTION_GATT_SERVICES_DISCOVERED)) {
                mService.enableTXNotification();
            }
            //*********************//
            if (action.equals(UartService.ACTION_DATA_AVAILABLE)) {

                final byte[] txValue = intent.getByteArrayExtra(UartService.EXTRA_DATA);
                runOnUiThread(() -> {
                    try {
                        if (frameFormat == 0) {
                            String text = new String(txValue, "UTF-8");
                            text.replace("\r", "").replace("\n", "").replace(" ", "");
                            //consoleFragment.addLog(text);
                            procData(text);
                        } else {
                            BinaryHandler.binaryParser(txValue);
                        }
                    } catch (Exception e) {
                        Log.e(TAG, e.toString());
                    }
                });
            }
            //*********************//
            if (action.equals(UartService.DEVICE_DOES_NOT_SUPPORT_UART)) {
                toastMessage("Device doesn't support UART. Disconnecting");
                mService.disconnect();
            }
        }
    };

    private void procData(String text) throws JSONException {
        Log.i(TAG, "uart_recv: " + text);
        if(text.length() > 0) {
            JsonHandler.jsonParser(text);
        }
    }

    public void sendData(String origin) throws UnsupportedEncodingException {
        if(mState == UART_PROFILE_CONNECTED) {
            if(origin.length() > 240) {
                toastMessage("json data too long :" + origin.length());
                return;
            }
            mService.writeRXCharacteristic(origin.getBytes("UTF-8"));
            //consoleFragment.addLog("\n>>>>Sent Start<<<<\n" + origin + "\n>>>>Sent end<<<<");
        } else {
            final DisconnectDialogFragment fragment = DisconnectDialogFragment.getInstance(R.string.disconnect_text);
            fragment.show(getSupportFragmentManager(), null);
        }
    }

    public void sendData(byte[] rxValue) {
        if(mState == UART_PROFILE_CONNECTED) {
            mService.writeRXCharacteristic(rxValue);
            //consoleFragment.addLog("\n>>>>Sent Start<<<<\n" + origin + "\n>>>>Sent end<<<<");
        }
    }

    public void startConnect() {
        if (!mBtAdapter.isEnabled()) {
            Log.i(TAG, "onClick - BT not enabled yet");
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        }
        else {
            Intent newIntent = new Intent(MainActivity.this, ScanDeviceListActivity.class);
            startActivityForResult(newIntent, REQUEST_SELECT_DEVICE);
        }
    }

    public void startDisconnect() {
        //Disconnect button pressed
        //eventLog.clear();
        if (mDevice!=null) {
            mService.disconnect();
        }
    }

    public String getDevAddr() {
        if (mDevice!=null) {
            return devAddr;
        }
        return null;
    }

    public String getDevName() {
        if (mDevice!=null) {
            return devName;
        }
        return null;
    }

    public int uartState(){
        return mState;
    }

    public boolean getUartRxService() {
        return mService.getRxService();
    }

    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(UartService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(UartService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(UartService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(UartService.ACTION_DATA_AVAILABLE);
        intentFilter.addAction(UartService.DEVICE_DOES_NOT_SUPPORT_UART);
        return intentFilter;
    }

    public int setFrameFormat(int value) {
        Log.i(TAG, "setFrameFormat: " + value);
        frameFormat = value;
        return frameFormat;
    }
}