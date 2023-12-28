package aprotech.ev.unimanager.binary;

import android.util.Log;

import java.util.Arrays;

import aprotech.ev.unimanager.MainActivity;
import aprotech.ev.unimanager.event.EventData;
import aprotech.ev.unimanager.event.EventHandler;

public class BinaryHandler {
    private static final String TAG = "BinaryHandler";

    public static final char RX_ST_SYNC_1       = 0;
    public static final char RX_ST_SYNC_2       = 1;
    public static final char RX_ST_LEN          = 2;
    public static final char RX_ST_DATA         = 3;

    public static final char CMD_NONE           = 0;
    public static final char CMD_FW_START       = 1;
    public static final char CMD_FW_RESTART     = 2;
    public static final char CMD_FW_SEND        = 3;
    public static final char CMD_FW_DONE        = 4;
    public static final char CMD_FW_ERROR       = 5;

    public static final char CMD_FW_START_OLD       = 1;
    public static final char CMD_FW_SEND_OLD        = 2;
    public static final char CMD_FW_DONE_OLD        = 3;
    public static final char CMD_FW_ERROR_OLD       = 4;

    public static final char CMD_ROOT_CA_START		= 11;
    public static final char CMD_ROOT_CA_RESTART 	= 12;
    public static final char CMD_ROOT_CA_SEND		= 13;
    public static final char CMD_ROOT_CA_DONE		= 14;
    public static final char CMD_ROOT_CA_ERROR		= 15;

    public static final char CMD_FACTORY_CA_START 	= 21;
    public static final char CMD_FACTORY_CA_RESTART	= 22;
    public static final char CMD_FACTORY_CA_SEND	= 23;
    public static final char CMD_FACTORY_CA_DONE	= 24;
    public static final char CMD_FACTORY_CA_ERROR 	= 25;

    public static final char CMD_FRAME_CHANGE   = 100;

    public static final char FW_SUCCESS         = 0;
    public static final char FW_ERROR           = 1;
    public static final char FW_ERROR_FRAME     = 2;
    public static final char FW_ERROR_CRC       = 3;
    public static final char FW_ERROR_VERIFY    = 4;
    public static final char FW_ERROR_FLASH     = 5;

    private static char rx_st = RX_ST_SYNC_1;
    private static int rx_len = 0;
    private static int rx_ix = 0;
    private static byte[] rx_dat = new byte[256];

    private static boolean _isDefault = true;

    public static boolean isDefaultMode() {
        return _isDefault;
    }

    public static void setDefaultMode(boolean _isDefault) {
        BinaryHandler._isDefault = _isDefault;
    }

    public static void binaryParser(byte[] rxValue) {
        for(int i = 0; i < rxValue.length; i++) {
            switch(rx_st) {
                case RX_ST_SYNC_1:
                    if(rxValue[i] == 's') {
                        rx_st = RX_ST_SYNC_2;
                    }
                    break;
                case RX_ST_SYNC_2:
                    if(rxValue[i] == 'y') {
                        rx_st = RX_ST_LEN;
                    } else if(rxValue[i] == 's') {
                        rx_st = RX_ST_SYNC_2;
                    } else {
                        rx_st = RX_ST_SYNC_1;
                    }
                    break;
                case RX_ST_LEN:
                    rx_len = rxValue[i];
                    rx_ix = 0;
                    Arrays.fill(rx_dat, (byte)0);
                    rx_st = RX_ST_DATA;
                    break;
                case RX_ST_DATA:
                    if(rx_len > 0) {
                        rx_len--;
                        rx_dat[rx_ix++] = rxValue[i];
                    }

                    if(rx_len == 0) {
                        binaryProc();
                        rx_st = RX_ST_SYNC_1;
                    }
                    break;
                default:
                    rx_st = RX_ST_SYNC_1;
                    break;
            }
        }
    }

    public static void binaryProc() {
        if(rx_ix > 0) {
            Log.v(TAG, "recv_dat: " + byteArrayToHex(rx_dat, rx_ix));
            char cmd = (char) rx_dat[0];
            int frame_ix = (rx_dat[1] << 8) & 0xFF00;
            frame_ix |= rx_dat[2] & 0xFF;
            char code = (char) rx_dat[3];
            int rx_crc = (rx_dat[rx_ix - 2] << 8) & 0xFF00;
            rx_crc |= rx_dat[rx_ix - 1] & 0xFF;

            int calCrc = crc16(rx_dat, 0, rx_ix - 2, 0xE1E1);
            Log.i(TAG, "cmd: " + (int)cmd + " len: " + rx_ix + " ix: " +  frame_ix + " rx_crc: " + rx_crc + " cal_crc: " + calCrc);
            if(rx_crc == calCrc) {
                if(!_isDefault){
                    switch (cmd) {
                        case CMD_FW_START_OLD:
                        case CMD_FW_SEND_OLD:
                        case CMD_FW_DONE_OLD:
                        case CMD_FW_ERROR_OLD:
                        case CMD_FACTORY_CA_START:
                        case CMD_FACTORY_CA_SEND:
                        case CMD_FACTORY_CA_DONE:
                        case CMD_FACTORY_CA_ERROR:
                            EventData busData = new EventData(cmd);
                            busData.addBody("code", (int) code);
                            busData.addBody("index", frame_ix);
                            EventHandler.getInstance().post(busData);
                            break;
                        case CMD_FRAME_CHANGE:
                            break;
                        default:
                            break;
                    }
                }else {
                    switch (cmd) {
                        case CMD_FW_START:
                        case CMD_FW_SEND:
                        case CMD_FW_DONE:
                        case CMD_FW_ERROR:
                        case CMD_ROOT_CA_START:
                        case CMD_ROOT_CA_SEND:
                        case CMD_ROOT_CA_DONE:
                        case CMD_ROOT_CA_ERROR:
                            EventData busData = new EventData(cmd);
                            busData.addBody("code", (int) code);
                            busData.addBody("index", frame_ix);
                            EventHandler.getInstance().post(busData);
                            break;
                        case CMD_FRAME_CHANGE:
                            break;
                        default:
                            break;
                    }
                }
            } else {
                Log.i(TAG, "Binary Data CRC Error");
                binarySend(CMD_FW_ERROR, 0, FW_ERROR);
            }
        } else {
            Log.i(TAG, "Binary Data Frame Error");
            binarySend(CMD_FW_ERROR, 0, FW_ERROR);
        }
    }

    public static void binarySend(char cmd, int frame_ix, byte[] dat, int len) {
        byte[] buf = new byte[256];
        char ix = 0;
        buf[ix++] = 's';                                // sync word 1
        buf[ix++] = 'y';                                // sync word 2
        buf[ix++] = (byte) ((len + 5) & 0xFF);          // frame len
        buf[ix++] = (byte) cmd;                         // command
        buf[ix++] = (byte) ((frame_ix >> 8) & 0xFF);    // frame index 1
        buf[ix++] = (byte) (frame_ix & 0xFF);           // frame index 2

        for(int i = 0; i < len; i++) {
            buf[ix++] = dat[i];                         // data
        }

        int calCrc = crc16(buf, 3, ix, 0xE1E1);
        buf[ix++] = (byte) ((calCrc >> 8) & 0xFF);      // crc16 1
        buf[ix++] = (byte) (calCrc & 0xFF);             // crc16 2

        byte[] tx_buf = Arrays.copyOf(buf, ix);
        MainActivity.getInstance().sendData(tx_buf);
        Log.v(TAG, "send_dat: " + byteArrayToHex(tx_buf, tx_buf.length));
    }

    public static void binarySend(char cmd, int frame_ix, char dat) {
        byte[] buf = new byte[256];
        char ix = 0;
        buf[ix++] = 's';                                // sync word 1
        buf[ix++] = 'y';                                // sync word 2
        buf[ix++] = 6;                                  // frame len
        buf[ix++] = (byte) cmd;                         // command
        buf[ix++] = (byte) ((frame_ix >> 8) & 0xFF);    // frame index 1
        buf[ix++] = (byte) (frame_ix & 0xFF);           // frame index 2
        buf[ix++] = (byte) dat;                         // data

        int calCrc = crc16(buf, 3, ix, 0xE1E1);
        buf[ix++] = (byte) ((calCrc >> 8) & 0xFF);      // crc16 1
        buf[ix++] = (byte) (calCrc & 0xFF);             // crc16 2

        byte[] tx_buf = Arrays.copyOf(buf, ix);
        MainActivity.getInstance().sendData(tx_buf);
        Log.v(TAG, "send_dat: " + byteArrayToHex(tx_buf, tx_buf.length));
    }

    private static int crc16(byte[] buff, int offset, int len, int crcKey) {
        int crcShort = crcKey;
        for (int i = offset; i < len; i++) {
            crcShort = ((crcShort  >>> 8) | (crcShort  << 8) )& 0xffff;
            crcShort ^= (buff[i] & 0xff);
            crcShort ^= ((crcShort & 0xff) >> 4);
            crcShort ^= (crcShort << 12) & 0xffff;
            crcShort ^= ((crcShort & 0xFF) << 5) & 0xffff;
        }
        crcShort &= 0xffff;
        //return new byte[] {(byte) (crcShort & 0xff), (byte) ((crcShort >> 8) & 0xff)};
        //Log.v(TAG, "crc16: " + crcShort);
        return crcShort;
    }

    public static String byteArrayToHex(byte[] a, int len) {
        StringBuilder sb = new StringBuilder();
        for(int i = 0; i < len; i++)
            sb.append(String.format("%02X", a[i]));
        return sb.toString();
    }
}
