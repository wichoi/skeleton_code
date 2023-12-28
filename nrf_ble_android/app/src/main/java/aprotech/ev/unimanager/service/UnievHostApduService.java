package aprotech.ev.unimanager.service;

import android.content.Intent;
import android.nfc.NdefMessage;
import android.nfc.NdefRecord;
import android.nfc.cardemulation.HostApduService;
import android.os.Bundle;
import android.util.Log;

import java.math.BigInteger;
import java.nio.charset.Charset;
import java.util.Arrays;

public class UnievHostApduService extends HostApduService {
    private static String TAG = "HostApduService";

    public static final int APP_STATE_RUNNING = 1001;
    public static final int APP_STATE_TERMINATED = 1002;

    private static int _currState = APP_STATE_TERMINATED;
    public static int getCurrentAppState(){
        return _currState;
    }

    public static void setCurrentAppState(int state){
        _currState = state;
    }

    @Override
    public void onCreate() {
        super.onCreate();
//        if(_currState == APP_STATE_TERMINATED)
//            startApp();
    }

    private void startApp() {
        Intent intent = new Intent(Intent.ACTION_MAIN);
        intent.setClassName("com.uniev.charger", "com.uniev.mainapp.SplashActivity")
                .addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
                .addFlags(Intent.FLAG_FROM_BACKGROUND);

        getApplicationContext().startActivity(intent);
    }

    @Override
    public byte[] processCommandApdu(byte[] commandApdu, Bundle extras) {

        if(Arrays.equals(commandApdu, APDU_SELECT)){
            Log.d(TAG,"APDU_SELECTED\n");
            return A_OKAY;
        }

        if(Arrays.equals(commandApdu, CAPABILITY_CONTAINER_OK)){
            Log.d(TAG,"CC_SELECT\n");
            return A_OKAY;
        }

        if (Arrays.equals(
                READ_CAPABILITY_CONTAINER,
                commandApdu
        ) && !READ_CAPABILITY_CONTAINER_CHECK
        ) {
            Log.d(TAG,"READ_CAPABILITY_CONTAINER_CHECK\n");
            READ_CAPABILITY_CONTAINER_CHECK = true;
            return READ_CAPABILITY_CONTAINER_RESPONSE;
        }
        if (Arrays.equals(NDEF_SELECT_OK, commandApdu)) {
            Log.d(TAG,"NDEF_SELECT_OK\n");
            return A_OKAY;
        }

        if (Arrays.equals(NDEF_READ_BINARY_NLEN, commandApdu)) {
            // Build our response
            if(NDEF_URI_LEN != null) {
                Log.d(TAG, "NDEF_READ_BINARY_NLEN " + Arrays.toString(NDEF_URI_LEN) + "\n");
                READ_CAPABILITY_CONTAINER_CHECK = false;
                return ConcatArrays(ConcatArrays(new byte[]{0x00}, NDEF_URI_LEN), A_OKAY);
            }
            return A_NOK;
        }

        if (checkNdefReadBinary(commandApdu)){
            Log.d(TAG,"NDEF_READ_BINARY\n");
            //Toast.makeText(getApplication(), "NDEF READ", Toast.LENGTH_SHORT).show();
            byte[] ndefContent = ConcatArrays(new byte[]{0x00}, NDEF_URI_BYTES);
            //Log.d(TAG,Arrays.toString(NDEF_URI_BYTES) + "\n" + Arrays.toString(ndefContent) + "\n");
            if(_currState == APP_STATE_RUNNING){

            }
            return ConcatArrays(ndefContent, A_OKAY);
        }

        return A_NOK;

    }

    private boolean checkNdefReadBinary(byte[] commandApdu) {
        if(NDEF_URI_LEN != null) {
            byte[] a = Arrays.copyOfRange(commandApdu, 0, 2);
            byte[] b = Arrays.copyOfRange(commandApdu, commandApdu.length - 1, commandApdu.length);
            return Arrays.equals(a, STARTOF_NDEF_READ_BINARY_GET_NDEF) && Arrays.equals(b, NDEF_URI_LEN);
        }
        return false;
    }

    @Override
    public void onDeactivated(int reason) {

    }



    public static byte[] HexStringToByteArray(String s) throws IllegalArgumentException {
        int len = s.length();
        if (len % 2 == 1) {
            throw new IllegalArgumentException("Hex string must have even number of characters");
        }
        byte[] data = new byte[len / 2]; // Allocate 1 byte per 2 hex characters
        for (int i = 0; i < len; i += 2) {
            // Convert each character into a integer (base-16), then bit-shift into place
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i+1), 16));
        }
        return data;
    }

    public static byte[] ConcatArrays(byte[] first, byte[] second) {
        byte[] c = new byte[first.length + second.length];
        System.arraycopy(first, 0, c, 0, first.length);
        System.arraycopy(second, 0, c, first.length, second.length);
        return c;
    }

    static byte[] NDEF_ID ={
            (byte)0xE1,
            (byte)0x04
    };
    private static NdefMessage NDEF_URI;
    private static byte[] NDEF_URI_BYTES;
    private static byte[] NDEF_URI_LEN;

    public static void createNdefMessage(String data) {
        if (NDEF_URI != null) {
            NDEF_URI = null;
            NDEF_URI_BYTES = null;
            NDEF_URI_LEN = null;
        }
        NdefRecord NDEF_URI_REC = new NdefRecord(
                NdefRecord.TNF_WELL_KNOWN,
                NdefRecord.RTD_TEXT,
                NDEF_ID,
                data.getBytes(Charset.forName("UTF-8"))
        );
        NDEF_URI = new NdefMessage(NDEF_URI_REC);
        NDEF_URI_BYTES = NDEF_URI.toByteArray();
        NDEF_URI_LEN = BigInteger.valueOf(NDEF_URI_BYTES.length).toByteArray();
        Log.d(TAG, "Tag has been created : " + data + " " + NDEF_URI_LEN);
        Log.d(TAG, "URI_BYTES : " + NDEF_URI_BYTES);
    }

    public static void createDefaultMessage() {
        if (NDEF_URI != null) {
            NDEF_URI = null;
            NDEF_URI_BYTES = null;
            NDEF_URI_LEN = null;
        }
        String data = "00000000000000";
        NdefRecord NDEF_URI_REC = new NdefRecord(
                NdefRecord.TNF_WELL_KNOWN,
                NdefRecord.RTD_TEXT,
                NDEF_ID,
                data.getBytes(Charset.forName("UTF-8"))
        );
        NDEF_URI = new NdefMessage(NDEF_URI_REC);
        NDEF_URI_BYTES = NDEF_URI.toByteArray();
        NDEF_URI_LEN = BigInteger.valueOf(NDEF_URI_BYTES.length).toByteArray();
    }

}
