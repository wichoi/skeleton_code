package aprotech.ev.unimanager.event;

import java.util.ArrayList;

public class EventData {
    public final static int VALUE_NONE = 0;
    public final static int VALUE_STRING = 1;
    public final static int VALUE_INT = 2;
    public final static int VALUE_DOUBLE = 3;

    String cmd;
    //String type;
    String action;
    //int datetime;
    ArrayList<Body> body;

    public EventData(String cmd) {
        this.cmd = cmd;
        body = new ArrayList<>();
    }

    public String getCmd() {
        return cmd;
    }

    public void setAction(String action) {
        this.action = action;
    }

    public String getAction() {
        return action;
    }

    public void addBody(String key, Object value) {
        body.add(new Body(key, value));
    }

    public void addBody(String key, Object value, int valType) {
        body.add(new Body(key, value, valType));
    }

    public int getBodySize() {
        return body.size();
    }

    public Body getBody(int index) {
        return body.get(index);
    }

    // only for json <-> binary switching [START]
    int cmdIx;
    public EventData(int cmd) {
        this.cmdIx = cmd;
        body = new ArrayList<>();
    }
    public int getCmdIx() {
        return cmdIx;
    }
    // only for json <-> binary switching [END]

    public static class Body {
        String key;
        Object value;
        int valType; // VALUE_STRING, VALUE_INT, VALUE_DOUBLE

        public Body(String key, Object value) {
            this.key = key;
            this.value = value;
            this.valType = VALUE_NONE;
        }

        public Body(String key, Object value, int valType) {
            this.key = key;
            this.value = value;
            this.valType = valType;
        }

        public String getKey() {
            return key;
        }

        public Object getValue() {
            return value;
        }

        public Object getValType() {
            return valType;
        }
    }
}
