package aprotech.ev.unimanager.event;

import com.squareup.otto.Bus;
import com.squareup.otto.ThreadEnforcer;

public class EventHandler {
    public static final Bus bus = new Bus(ThreadEnforcer.ANY);
    public static Bus getInstance(){
        return bus;
    }
    public EventHandler() {
    }
}
