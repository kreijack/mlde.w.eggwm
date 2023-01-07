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
#include "events/XcbEventFilter.h"

#include "xwindows/XWindowList.h"
#include "xwindows/XWindow.h"
#include "standards/EWMHRoot.h"

struct EventType {
    const char *descr;
    unsigned long id;
};
static EventType event_type_list[] = {
        {"XCB_KEY_PRESS", 2},
        {"XCB_BUTTON_PRESS", 4},
        {"XCB_BUTTON_RELEASE", 5},
        {"XCB_MOTION_NOTIFY", 6},
        {"XCB_ENTER_NOTIFY", 7},
        {"XCB_LEAVE_NOTIFY", 8},
        {"XCB_FOCUS_IN", 9},
        {"XCB_FOCUS_OUT", 10},
        {"XCB_KEYMAP_NOTIFY", 11},
        {"XCB_EXPOSE", 12},
        {"XCB_GRAPHICS_EXPOSURE", 13},
        {"XCB_NO_EXPOSURE", 14},
        {"XCB_VISIBILITY_NOTIFY", 15},
        {"XCB_CREATE_NOTIFY", 16},
        {"XCB_DESTROY_NOTIFY", 17},
        {"XCB_UNMAP_NOTIFY", 18},
        {"XCB_MAP_NOTIFY", 19},
        {"XCB_MAP_REQUEST", 20},
        {"XCB_REPARENT_NOTIFY", 21},
        {"XCB_CONFIGURE_NOTIFY", 22},
        {"XCB_CONFIGURE_REQUEST", 23},
        {"XCB_GRAVITY_NOTIFY", 24},
        {"XCB_RESIZE_REQUEST", 25},
        {"XCB_CIRCULATE_NOTIFY", 26},
        {"XCB_CIRCULATE_REQUEST", 27},
        {"XCB_PROPERTY_NOTIFY", 28},
        {"XCB_SELECTION_CLEAR", 29},
        {"XCB_SELECTION_REQUEST", 30},
        {"XCB_SELECTION_NOTIFY", 31},
        {"XCB_COLORMAP_NOTIFY", 32},
        {"XCB_CLIENT_MESSAGE", 33},
        {"XCB_MAPPING_NOTIFY", 34},
        {"XCB_GE_GENERIC", 35},

        {nullptr, 0}
};

static EventType input_event_type_list[] = {
        {"XCB_INPUT_DEVICE_CHANGED", 1},
        {"XCB_INPUT_KEY_PRESS", 2},
        {"XCB_INPUT_KEY_RELEASE", 3},
        {"XCB_INPUT_BUTTON_PRESS", 4},
        {"XCB_INPUT_BUTTON_RELEASE", 5},
        {"XCB_INPUT_MOTION", 6},
        {"XCB_INPUT_ENTER", 7},
        {"XCB_INPUT_LEAVE", 8},
        {"XCB_INPUT_FOCUS_IN", 9},
        {"XCB_INPUT_FOCUS_OUT", 10},
        {"XCB_INPUT_HIERARCHY", 11},
        {"XCB_INPUT_PROPERTY", 12},
        {"XCB_INPUT_RAW_KEY_PRESS", 13},
        {"XCB_INPUT_RAW_KEY_RELEASE", 14},
        {"XCB_INPUT_RAW_BUTTON_PRESS", 15},
        {"XCB_INPUT_RAW_BUTTON_RELEASE", 16},
        {"XCB_INPUT_RAW_MOTION", 17},
        {"XCB_INPUT_TOUCH_BEGIN", 18},
        {"XCB_INPUT_TOUCH_UPDATE", 19},
        {"XCB_INPUT_TOUCH_END", 20},
        {"XCB_INPUT_TOUCH_OWNERSHIP", 21},
        {"XCB_INPUT_RAW_TOUCH_BEGIN", 22},
        {"XCB_INPUT_RAW_TOUCH_UPDATE", 23},
        {"XCB_INPUT_RAW_TOUCH_END", 24},
        {"XCB_INPUT_BARRIER_HIT", 25},
        {"XCB_INPUT_BARRIER_LEAVE", 26},
        {"XCB_INPUT_GESTURE_PINCH_BEGIN", 27},
        {"XCB_INPUT_GESTURE_PINCH_UPDATE", 28},
        {"XCB_INPUT_GESTURE_PINCH_END", 29},
        {"XCB_INPUT_GESTURE_SWIPE_BEGIN", 30},
        {"XCB_INPUT_GESTURE_SWIPE_UPDATE", 31},
        {"XCB_INPUT_GESTURE_SWIPE_END", 32},

        {nullptr, 0}
};

static const char *event2Text(unsigned long id, EventType *types) {
    static char buf[1024];
    snprintf(buf, 1023, "<UNKNOWN event: %lu>", id);
    const char *ret = buf;

    for (int i = 0 ; types[i].descr ; i++) {
        if (types[i].id == id) {
            ret = types[i].descr;
            break;
        }
    }
    return ret;
}

bool XcbEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    (void)eventType;

    auto event = static_cast<xcb_generic_event_t*>(message);

    if (Config::getInstance()->getDebugDumpEvents())
        qDebug() << "Got event " << event2Text(event->response_type, event_type_list);
    bool ret;
    switch (event->response_type) {
        case XCB_MAP_REQUEST:
            ret = mapRequestHandler(event);
            break;
        case XCB_CONFIGURE_REQUEST:
            ret = configureRequestHandler(event);
            break;
        case XCB_CREATE_NOTIFY:
            ret = createNotifyHandler(event);
            break;
        case XCB_DESTROY_NOTIFY:
            ret = destroyNotifyHandler(event);
            break;
        case XCB_UNMAP_NOTIFY:
            ret = unmapNotifyHandler(event);
            break;
        case XCB_BUTTON_PRESS:
            ret = buttonPressHandler(event);
            break;
        case XCB_CLIENT_MESSAGE:
            ret = clientMessageHandler(event);
            break;
        default:
            ret = false;
            break;
    }
    xcb_allow_events (QX11Info::connection(),
        XCB_ALLOW_REPLAY_POINTER,
        XCB_TIME_CURRENT_TIME);
    xcb_flush (QX11Info::connection());
    return ret;
}

bool XcbEventFilter::buttonPressHandler(xcb_generic_event_t* event)
{
    xcb_button_press_event_t* button = reinterpret_cast<xcb_button_press_event_t*>(event);
    Window windowID = button->child;
    //qDebug() << "[+] ButtonPress event 0x" << Qt:hex << windowID;
    if (this->wl->existFrame(windowID)) {
        XWindow* xwindow = wl->getXWindowByFrameID(windowID);
        this->wl->restackManagedWindow(xwindow);
        this->wl->setActiveWindow(xwindow);
    }

    return false;
}

bool XcbEventFilter::clientMessageHandler(xcb_generic_event_t* event)
{
    xcb_client_message_event_t* client = reinterpret_cast<xcb_client_message_event_t*>(event);
    Window windowID = client->window;
    //qDebug() << "[+] ClientMessage event 0x" << Qt::hex << windowID;
    AtomList* al = AtomList::getInstance();
    if (this->wl->existClient(windowID)) {
        XWindow* xwindow = this->wl->getXWindowByClientID(windowID);
	    if (client->type == al->getAtom("WM_CHANGE_STATE") && 
            client->format == 32 && client->data.data8[0] == IconicState) {
            xwindow->setState(IconicState);
            return true;
        } else if (client->type == al->getAtom("_NET_ACTIVE_WINDOW")) {
            EWMHRoot ewmh;
            ewmh.receivedActiveWindow(this->wl, xwindow);
        } else if (client->type == al->getAtom("_NET_CLOSE_WINDOW")) {
            EWMHRoot ewmh;
            ewmh.receivedCloseWindow(xwindow);
        }
    }
    return false;
}

bool XcbEventFilter::configureRequestHandler(xcb_generic_event_t* event)
{
    xcb_configure_request_event_t *configure = reinterpret_cast<xcb_configure_request_event_t *>(event);
    Window windowID = configure->window;
    //qDebug() << "[+] ConfigureRequest event 0x" << Qt::hex << windowID;
    if (this->wl->existClient(windowID)) {
        XWindow* xwindow = this->wl->getXWindowByClientID(windowID);
        if (!xwindow->bypassWM()) {
            if (configure->value_mask & CWX) xwindow->setX(configure->x);
            if (configure->value_mask & CWY) xwindow->setY(configure->y);
            Config* cfg = Config::getInstance();
            if (configure->value_mask & CWWidth) {
                if (xwindow->haveFrame()) {
                    xwindow->setWidth(
                        configure->width + cfg->getLeftBorderWidth() + 
                        cfg->getRightBorderWidth());
                } else xwindow->setWidth(configure->width);
            }
            if (configure->value_mask & CWHeight) {
                if(xwindow->haveFrame()) {
                    xwindow->setHeight(
                        configure->height + cfg->getTitlebarWidth() + 
                        cfg->getTopBorderWidth() + cfg->getBottomBorderWidth());
                } else xwindow->setHeight(configure->height);
            }
            return true;
        }
    }
    return false;
}

bool XcbEventFilter::createNotifyHandler(xcb_generic_event_t* event)
{
    xcb_create_notify_event_t* create = reinterpret_cast<xcb_create_notify_event_t*>(event);
    Window windowID = create->window;
    //qDebug() << "[+] CreateNotify event 0x" << Qt::hex << windowID;
    if (!this->wl->existFrame(windowID)) {
        XWindow* xwindow = new XWindow(windowID);
        this->wl->addClient(windowID, xwindow);
    }
    return false;
}


bool XcbEventFilter::destroyNotifyHandler(xcb_generic_event_t* event)
{
    xcb_destroy_notify_event_t* destroy = reinterpret_cast<xcb_destroy_notify_event_t*>(event);
    Window windowID = destroy->window;
    //qDebug() << "[+] DestroyNotify event 0x" << Qt::hex << windowID;
    if (this->wl->existClient(windowID)) {
        XWindow* xwindow = this->wl->getXWindowByClientID(windowID);
        wl->removeClient(xwindow->getClientID());
        this->wl->removeFromManagedWindow(xwindow);
        if (!xwindow->haveFrame()) delete xwindow;
        else xwindow->removeFrame();
        return true;
    } else if(wl->existFrame(windowID)) {
        XWindow* xwindow = this->wl->getXWindowByFrameID(windowID);
        if (destroy->event == destroy->window) return true;
        this->wl->removeFrame(xwindow->getFrameID());
        delete xwindow;
        return true;
    }
    return false;
}

bool XcbEventFilter::mapRequestHandler(xcb_generic_event_t* event)
{
    xcb_map_request_event_t* map = reinterpret_cast<xcb_map_request_event_t*>(event);
    Window windowID = map->window;
    //qDebug() << "[+] MapRequest event 0x" << Qt::hex << windowID;
    if (this->wl->existClient(windowID)) {
        XWindow* xwindow = this->wl->getXWindowByClientID(windowID);
        if(xwindow->bypassWM()) return false;
        else {
            if (xwindow->getState() == WithdrawnState) {
                if (xwindow->needFrame()) {
                    xwindow->addFrame();
                    wl->addFrame(xwindow->getFrameID(), xwindow);
                }
                xwindow->setState(NormalState);
                this->wl->addToManagedWindows(xwindow);
                this->wl->setActiveWindow(this->wl->getTopWindow());
            }
            return true;
        }
    }
    return false;
}

bool XcbEventFilter::unmapNotifyHandler(xcb_generic_event_t* event)
{
    xcb_unmap_notify_event_t* unmap = reinterpret_cast<xcb_unmap_notify_event_t*>(event);
    Window windowID = unmap->window;
    //qDebug() << "[+] UnmapNotify event 0x" << Qt::hex << windowID;
    if (this->wl->existClient(windowID)) {
        XWindow* xwindow = wl->getXWindowByClientID(windowID);
        if (xwindow->getState() != IconicState) {
            xwindow->setState(WithdrawnState);
            this->wl->removeFromManagedWindow(xwindow);
        }
        this->wl->setActiveWindow(this->wl->getTopWindow());
    }
    return false;
}
