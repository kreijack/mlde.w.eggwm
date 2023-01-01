/**
 * Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * @file /src/eggwm/events/handlers/UnmapNotifyHandler.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  UnmapNotifyHandler
 * @author José Expósito
 */
#include "UnmapNotifyHandler.h"

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

UnmapNotifyHandler::UnmapNotifyHandler() {}


// ************************************************************************** //
// **********                    PUBLIC METHODS                    ********** //
// ************************************************************************** //

bool UnmapNotifyHandler::processEvent(xcb_generic_event_t* event) 
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
