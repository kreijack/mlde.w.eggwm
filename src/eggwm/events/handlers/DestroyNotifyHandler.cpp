/**
 * Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * @file /src/eggwm/events/handlers/DestroyNotifyHandler.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  DestroyNotifyHandler
 * @author José Expósito
 */
#include "DestroyNotifyHandler.h"

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

DestroyNotifyHandler::DestroyNotifyHandler() {}


// ************************************************************************** //
// **********                    PUBLIC METHODS                    ********** //
// ************************************************************************** //

bool DestroyNotifyHandler::processEvent(xcb_generic_event_t* event) 
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
