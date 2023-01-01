/**
 * Copyright (C) 2023 Goffredo Baroncelli <kreijack@libero.it>
 *
 * @file /src/eggwm/events/EventsHandler.h
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  XcbEventFilter
 * @author Goffredo Baroncelli
 */
#include "src/eggwm/events/XcbEventFilter.h"

#include "src/eggwm/xwindows/XWindowList.h"
#include "src/eggwm/xwindows/XWindow.h"
#include "src/eggwm/standards/EWMHRoot.h"

static const char* event_str[] = {
    "ERROR",
    "SUCCESS",
    "XCB_KEY_PRESS",
    "XCB_KEY_RELEASE",
    "XCB_BUTTON_PRESS",
    "XCB_BUTTON_RELEASE",
    "XCB_MOTION_NOTIFY",
    "XCB_ENTER_NOTIFY",
    "XCB_LEAVE_NOTIFY",
    "XCB_FOCUS_IN",
    "XCB_FOCUS_OUT",
    "XCB_KEYMAP_NOTIFY",
    "XCB_EXPOSE",
    "XCB_GRAPHICS_EXPOSURE",
    "XCB_NO_EXPOSURE",
    "XCB_VISIBILITY_NOTIFY",
    "XCB_CREATE_NOTIFY",
    "XCB_DESTROY_NOTIFY",
    "XCB_UNMAP_NOTIFY",
    "XCB_MAP_NOTIFY",
    "XCB_MAP_REQUEST",
    "XCB_REPARENT_NOTIFY",
    "XCB_CONFIGURE_NOTIFY",
    "XCB_CONFIGURE_REQUEST",
    "XCB_GRAVITY_NOTIFY",
    "XCB_RESIZE_REQUEST",
    "XCB_CIRCULATE_NOTIFY",
    "XCB_CIRCULATE_REQUEST",
    "XCB_PROPERTY_NOTIFY",
    "XCB_SELECTION_CLEAR",
    "XCB_SELECTION_REQUEST",
    "XCB_SELECTION_NOTIFY",
    "XCB_COLORMAP_NOTIFY",
    "XCB_CLIENT_MESSAGE",
    "XCB_MAPPING_NOTIFY",
    "XCB_GE_GENERIC",
    "XCB_REQUEST",
    "XCB_VALUE",
    "XCB_WINDOW",
    "XCB_PIXMAP",
    "XCB_ATOM",
    "XCB_CURSOR",
    "XCB_FONT",
    "XCB_MATCH",
    "XCB_DRAWABLE",
    "XCB_ACCESS",
    "XCB_ALLOC",
    "XCB_COLORMAP",
    "XCB_G_CONTEXT",
    "XCB_ID_CHOICE",
    "XCB_NAME",
    "XCB_LENGTH",
    "XCB_IMPLEMENTATION",
    "XCB_CREATE_WINDOW",
    "XCB_CHANGE_WINDOW_ATTRIBUTES",
    "XCB_GET_WINDOW_ATTRIBUTES",
    "XCB_DESTROY_WINDOW",
    "XCB_DESTROY_SUBWINDOWS",
    "XCB_CHANGE_SAVE_SET",
    "XCB_REPARENT_WINDOW",
    "XCB_MAP_WINDOW",
    "XCB_MAP_SUBWINDOWS",
    "XCB_UNMAP_WINDOW",
    "XCB_UNMAP_SUBWINDOWS",
    "XCB_CONFIGURE_WINDOW",
    "XCB_CIRCULATE_WINDOW",
    "XCB_GET_GEOMETRY",
    "XCB_QUERY_TREE",
    "XCB_INTERN_ATOM",
    "XCB_GET_ATOM_NAME",
    "XCB_CHANGE_PROPERTY",
    "XCB_DELETE_PROPERTY",
    "XCB_GET_PROPERTY",
    "XCB_LIST_PROPERTIES",
    "XCB_SET_SELECTION_OWNER",
    "XCB_GET_SELECTION_OWNER",
    "XCB_CONVERT_SELECTION",
    "XCB_SEND_EVENT",
    "XCB_GRAB_POINTER",
    "XCB_UNGRAB_POINTER",
    "XCB_GRAB_BUTTON",
    "XCB_UNGRAB_BUTTON",
    "XCB_CHANGE_ACTIVE_POINTER_GRAB",
    "XCB_GRAB_KEYBOARD",
    "XCB_UNGRAB_KEYBOARD",
    "XCB_GRAB_KEY",
    "XCB_UNGRAB_KEY",
    "XCB_ALLOW_EVENTS",
    "XCB_GRAB_SERVER",
    "XCB_UNGRAB_SERVER",
    "XCB_QUERY_POINTER",
    "XCB_GET_MOTION_EVENTS",
    "XCB_TRANSLATE_COORDINATES",
    "XCB_WARP_POINTER",
    "XCB_SET_INPUT_FOCUS",
    "XCB_GET_INPUT_FOCUS",
    "XCB_QUERY_KEYMAP",
    "XCB_OPEN_FONT",
    "XCB_CLOSE_FONT",
    "XCB_QUERY_FONT",
    "XCB_QUERY_TEXT_EXTENTS",
    "XCB_LIST_FONTS",
    "XCB_LIST_FONTS_WITH_INFO",
    "XCB_SET_FONT_PATH",
    "XCB_GET_FONT_PATH",
    "XCB_CREATE_PIXMAP",
    "XCB_FREE_PIXMAP",
    "XCB_CREATE_GC",
    "XCB_CHANGE_GC",
    "XCB_COPY_GC",
    "XCB_SET_DASHES",
    "XCB_SET_CLIP_RECTANGLES",
    "XCB_FREE_GC",
    "XCB_CLEAR_AREA",
    "XCB_COPY_AREA",
    "XCB_COPY_PLANE",
    "XCB_POLY_POINT",
    "XCB_POLY_LINE",
    "XCB_POLY_SEGMENT",
    "XCB_POLY_RECTANGLE",
    "XCB_POLY_ARC",
    "XCB_FILL_POLY",
    "XCB_POLY_FILL_RECTANGLE",
    "XCB_POLY_FILL_ARC",
    "XCB_PUT_IMAGE",
    "XCB_GET_IMAGE",
    "XCB_POLY_TEXT_8",
    "XCB_POLY_TEXT_16",
    "XCB_IMAGE_TEXT_8",
    "XCB_IMAGE_TEXT_16",
    "XCB_CREATE_COLORMAP",
    "XCB_FREE_COLORMAP",
    "XCB_COPY_COLORMAP_AND_FREE",
    "XCB_INSTALL_COLORMAP",
    "XCB_UNINSTALL_COLORMAP",
    "XCB_LIST_INSTALLED_COLORMAPS",
    "XCB_ALLOC_COLOR",
    "XCB_ALLOC_NAMED_COLOR",
    "XCB_ALLOC_COLOR_CELLS",
    "XCB_ALLOC_COLOR_PLANES",
    "XCB_FREE_COLORS",
    "XCB_STORE_COLORS",
    "XCB_STORE_NAMED_COLOR",
    "XCB_QUERY_COLORS",
    "XCB_LOOKUP_COLOR",
    "XCB_CREATE_CURSOR",
    "XCB_CREATE_GLYPH_CURSOR",
    "XCB_FREE_CURSOR",
    "XCB_RECOLOR_CURSOR",
    "XCB_QUERY_BEST_SIZE",
    "XCB_QUERY_EXTENSION",
    "XCB_LIST_EXTENSIONS",
    "XCB_CHANGE_KEYBOARD_MAPPING",
    "XCB_GET_KEYBOARD_MAPPING",
    "XCB_CHANGE_KEYBOARD_CONTROL",
    "XCB_GET_KEYBOARD_CONTROL",
    "XCB_BELL",
    "XCB_CHANGE_POINTER_CONTROL",
    "XCB_GET_POINTER_CONTROL",
    "XCB_SET_SCREEN_SAVER",
    "XCB_GET_SCREEN_SAVER",
    "XCB_CHANGE_HOSTS",
    "XCB_LIST_HOSTS",
    "XCB_SET_ACCESS_CONTROL",
    "XCB_SET_CLOSE_DOWN_MODE",
    "XCB_KILL_CLIENT",
    "XCB_ROTATE_PROPERTIES",
    "XCB_FORCE_SCREEN_SAVER",
    "XCB_SET_POINTER_MAPPING",
    "XCB_GET_POINTER_MAPPING",
    "XCB_SET_MODIFIER_MAPPING",
    "XCB_GET_MODIFIER_MAPPING",
    "XCB_NO_OPERATION",
};

bool XcbEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    (void)eventType;

    auto event = static_cast<xcb_generic_event_t*>(message);

    /*
    if (event->response_type < sizeof(event_str)/sizeof(event_str[0]))
        qDebug() << "Got event" << event_str[event->response_type];
    else
        qDebug() << "Got UNKNOWN event" << event->response_type;
    */

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
    return ret;
}

bool XcbEventFilter::buttonPressHandler(xcb_generic_event_t* event)
{
    xcb_button_press_event_t* button = reinterpret_cast<xcb_button_press_event_t*>(event);
    Window windowID = button->root;
    //qDebug() << "[+] ButtonPress event 0x" << hex << windowID;
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
    //qDebug() << "[+] ClientMessage event 0x" << hex << windowID;
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
    //qDebug() << "[+] ConfigureRequest event 0x" << hex << windowID;
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
    //qDebug() << "[+] CreateNotify event 0x" << hex << windowID;
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
    //qDebug() << "[+] DestroyNotify event 0x" << hex << windowID;
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
    //qDebug() << "[+] MapRequest event 0x" << hex << windowID;
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
    //qDebug() << "[+] UnmapNotify event 0x" << hex << windowID;
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
