/**
 * Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * @file /src/eggwm/events/handlers/MapRequestHandler.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  MapRequestHandler
 * @author José Expósito
 */
#include "MapRequestHandler.h"

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

MapRequestHandler::MapRequestHandler(XWindowList* wl)
        : EventHandler(wl) {}


// ************************************************************************** //
// **********                    PUBLIC METHODS                    ********** //
// ************************************************************************** //

bool MapRequestHandler::processEvent(xcb_generic_event_t* event) 
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
