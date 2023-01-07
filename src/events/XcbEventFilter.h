/**
 * Copyright (C) 2023 Goffredo Baroncelli <kreijack@libero.it>
 *
 * @file events/EventsHandler.h
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  XcbEventFilter
 * @author Goffredo Baroncelli
 */
#include "util/Include.h"

class XWindowList;

class XcbEventFilter : public QAbstractNativeEventFilter                         
{                                                                                  
public:
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE;
    XcbEventFilter(XWindowList *wl) : wl(wl) {}

private:
    /**
     * @~english
     * the list of the XWindow
     */
    XWindowList *wl;

    /**
     * @~english
     * the handler for specific event
     * @param event The event to be processed.
     * @return true if you want to stop the event from being processed, ie
     *         when we treat ourselves, false for normal event dispatching.
     */
    bool buttonPressHandler(xcb_generic_event_t* event);
    bool clientMessageHandler(xcb_generic_event_t* event);
    bool configureRequestHandler(xcb_generic_event_t* event);
    bool createNotifyHandler(xcb_generic_event_t* event);
    bool destroyNotifyHandler(xcb_generic_event_t* event);
    bool mapRequestHandler(xcb_generic_event_t* event);
    bool unmapNotifyHandler(xcb_generic_event_t* event);
};   
