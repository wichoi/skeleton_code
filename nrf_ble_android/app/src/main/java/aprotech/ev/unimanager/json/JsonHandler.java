package aprotech.ev.unimanager.json;

import android.os.Build;

import org.json.JSONException;
import org.json.JSONObject;

import java.time.Instant;
import java.util.ArrayList;

import aprotech.ev.unimanager.event.EventData;
import aprotech.ev.unimanager.event.EventHandler;

public class JsonHandler {
    // key
    public static final String KEY_CMD = "cmd";
    public static final String KEY_TYPE = "type";
    public static final String KEY_DATETIME = "datetime";
    public static final String KEY_ACTION = "action";
    public static final String KEY_BODY = "body";

    // command
    public static final String CMD_HELLO = "hello";

    // type
    public static final String TYPE_REQUEST = "request";
    public static final String TYPE_RESPONSE = "response";

    // action
    public static final String ACTION_READ = "read";
    public static final String ACTION_WRITE = "write";
    public static final String ACTION_OK = "ok";
    public static final String ACTION_FAIL = "fail";
    public static final String ACTION_NONE = "none";
    public static final String ACTION_DEFAULT = "default";

    // body
    public static final String BODY_READ = "read";

    public static final String BODY_MODE = "mode";

    public static final String BODY_REBOOT = "reboot";
    public static final String BODY_RESET = "reset";

    public static void jsonParser(String text) throws JSONException {
        JSONObject obj = new JSONObject(text);
        if(obj != null){
            String cmd = obj.getString(KEY_CMD);
            switch(cmd){
                case CMD_HELLO:
                    parserHello(obj);
                    break;
                case CMD_SYSTEM:
                    parserSystem(obj);
                    break;
                case CMD_MODEM:
                    parserModem(obj);
                    break;
                case CMD_CONF:
                    parserConf(obj);
                    break;
                case CMD_STATUS:
                    parserState(obj);
                    break;
                case CMD_CONSOLE:
                    parserConsole(obj);
                    break;
                case CMD_CHANGE:
                    parserFrameFormatChangeCmd(obj);
                    break;
                case CMD_REBOOT:
                    break;
            }
        }
    }

    private static void valueStr(EventData eventData, JSONObject obj, String key) {
        if(obj.has(key)){
            String value = null;
            try {
                value = obj.getString(key);
            } catch (JSONException e) {
                e.printStackTrace();
            }
            if(value.length() > 0)
                eventData.addBody(key, value);
        }
    }

    private static void valueInt(EventData eventData, JSONObject obj, String key) {
        if(obj.has(key)){
            int value = 0;
            try {
                value = obj.getInt(key);
            } catch (JSONException e) {
                e.printStackTrace();
            }
            eventData.addBody(key, value);
        }
    }

    private static void parserHello(JSONObject obj){
    }

    private static void parserSystem(JSONObject obj){
        try {
            String emptyCheck = obj.getString(KEY_BODY);
            if(emptyCheck.length() == 0) {
                return;
            }

            JSONObject body = obj.getJSONObject(KEY_BODY);
            EventData eventData = new EventData(CMD_SYSTEM);
            valueStr(eventData, body, BODY_MODEL);
            valueStr(eventData, body, BODY_MEM);
            EventHandler.getInstance().post(eventData);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private static void parserModem(JSONObject obj){
        try {
            String emptyCheck = obj.getString(KEY_BODY);
            if(emptyCheck.length() == 0) {
                return;
            }

            JSONObject body = obj.getJSONObject(KEY_BODY);
            EventData eventData = new EventData(CMD_MODEM);
            valueStr(eventData, body, BODY_IP_ADDR);
            valueStr(eventData, body, BODY_IP_TYPE);
            EventHandler.getInstance().post(eventData);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private static void parserConf(JSONObject obj){
        try {
            String emptyCheck = obj.getString(KEY_BODY);
            if(emptyCheck.length() == 0) {
                return;
            }

            JSONObject body = obj.getJSONObject(KEY_BODY);
            EventData eventData = new EventData(CMD_CONF);

            String action = obj.getString(KEY_ACTION);
            eventData.setAction(action);

            EventHandler.getInstance().post(eventData);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private static void parserState(JSONObject obj){
        try {
            String emptyCheck = obj.getString(KEY_BODY);
            if(emptyCheck.length() == 0) {
                return;
            }

            JSONObject body = obj.getJSONObject(KEY_BODY);
            EventData eventData = new EventData(CMD_STATUS);
            valueStr(eventData, body, BODY_ST_UI);
            EventHandler.getInstance().post(eventData);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private static void parserConsole(JSONObject obj){
        try {
            String emptyCheck = obj.getString(KEY_BODY);
            if(emptyCheck.length() == 0) {
                return;
            }

            JSONObject body = obj.getJSONObject(KEY_BODY);
            EventData eventData = new EventData(CMD_CONSOLE);
            valueStr(eventData, body, KEY_CMD);
            EventHandler.getInstance().post(eventData);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private static void parserFrameFormatChangeCmd(JSONObject obj){
        try {
            String emptyCheck = obj.getString(KEY_BODY);
            if(emptyCheck.length() == 0) {
                return;
            }

            JSONObject body = obj.getJSONObject(KEY_BODY);
            EventData eventData = new EventData(CMD_CHANGE);
            valueInt(eventData, body, BODY_MODE);
            EventHandler.getInstance().post(eventData);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public static String createHello() throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put(KEY_CMD, "hello");
        obj.put(KEY_TYPE, TYPE_REQUEST);
        obj.put(KEY_ACTION, ACTION_NONE);
        obj.put(KEY_DATETIME, getTimeInSecond());
        return obj.toString();
    }

    public static String createSystem(String str) throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put(KEY_CMD, CMD_SYSTEM);
        obj.put(KEY_TYPE, TYPE_REQUEST);
        obj.put(KEY_ACTION, ACTION_OK);
        obj.put(KEY_DATETIME, getTimeInSecond());

        JSONObject body = new JSONObject();
        body.put(KEY_CMD, str);
        obj.put(KEY_BODY, body);

        return obj.toString();
    }

    public static String createModem(String str) throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put(KEY_CMD, CMD_MODEM);
        obj.put(KEY_TYPE, TYPE_REQUEST);
        obj.put(KEY_ACTION, ACTION_OK);
        obj.put(KEY_DATETIME, getTimeInSecond());

        JSONObject body = new JSONObject();
        body.put(KEY_CMD, str);
        obj.put(KEY_BODY, body);

        return obj.toString();
    }

    public static String createConfig(String action, ArrayList<EventData.Body> paramList) throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put(KEY_CMD, CMD_CONF);
        obj.put(KEY_TYPE, TYPE_REQUEST);
        obj.put(KEY_ACTION, action);
        obj.put(KEY_DATETIME, getTimeInSecond());

        JSONObject body = new JSONObject();
        for(int i = 0; i < paramList.size(); i++){
            body.put(paramList.get(i).getKey(), (String)paramList.get(i).getValue());
        }
        obj.put(KEY_BODY, body);

        return obj.toString();
    }

    public static String createState(String str) throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put(KEY_CMD, CMD_STATUS);
        obj.put(KEY_TYPE, TYPE_REQUEST);
        obj.put(KEY_ACTION, ACTION_OK);
        obj.put(KEY_DATETIME, getTimeInSecond());

        JSONObject body = new JSONObject();
        body.put(KEY_CMD, str);
        obj.put(KEY_BODY, body);

        return obj.toString();
    }

    public static String createConsole(String str) throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put(KEY_CMD, CMD_CONSOLE);
        obj.put(KEY_TYPE, TYPE_REQUEST);
        obj.put(KEY_ACTION, ACTION_OK);
        obj.put(KEY_DATETIME, getTimeInSecond());

        JSONObject body = new JSONObject();
        body.put(KEY_CMD, str);
        obj.put(KEY_BODY, body);

        return obj.toString();
    }

    public static String CreateFrameFormatChangeCmd(int mode) throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put(KEY_CMD, CMD_CHANGE);
        obj.put(KEY_TYPE, TYPE_REQUEST);
        obj.put(KEY_ACTION, ACTION_OK);
        obj.put(KEY_DATETIME, getTimeInSecond());

        JSONObject body = new JSONObject();
        body.put(BODY_MODE, mode); // 0(json), 1(binary)
        obj.put(KEY_BODY, body);

        return obj.toString();
    }

    public static String createReboot() throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put(KEY_CMD, CMD_REBOOT);
        obj.put(KEY_TYPE, TYPE_REQUEST);
        obj.put(KEY_ACTION, ACTION_WRITE);
        obj.put(KEY_DATETIME, getTimeInSecond());

        JSONObject body = new JSONObject();
        body.put(KEY_CMD, BODY_REBOOT);
        obj.put(KEY_BODY, body);

        return obj.toString();
    }

    public static String createFactoryReset() throws JSONException {
        JSONObject obj = new JSONObject();
        obj.put(KEY_CMD, CMD_REBOOT);
        obj.put(KEY_TYPE, TYPE_REQUEST);
        obj.put(KEY_ACTION, ACTION_OK);
        obj.put(KEY_DATETIME, getTimeInSecond());

        JSONObject body = new JSONObject();
        body.put(KEY_CMD, BODY_RESET);
        obj.put(KEY_BODY, body);

        return obj.toString();
    }

    private static long getTimeInSecond(){
        long result = 0;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            result = Instant.now().getEpochSecond() + (9 * 60 * 60);
        }
        return result;
    }
}
