package org.easydarwin.easypusherlive.domain;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Helong on 16/3/15-22:23.
 */
public class Body {

    private String SessionCount;
    private List<LiveSession> Sessions;

    public String getSessionCount() {
        return SessionCount;
    }

    public void setSessionCount(String sessionCount) {
        SessionCount = sessionCount;
    }

    public List<LiveSession> getSessions() {
        return Sessions==null?new ArrayList<LiveSession>():Sessions;
    }

    public void setSessions(List<LiveSession> sessions) {
        Sessions = sessions;
    }
}
