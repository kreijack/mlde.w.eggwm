/**
 * Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * @file /src/eggwm/events/handlers/CreateNotifyHandler.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  CreateNotifyHandler
 * @author José Expósito
 */
#include "CreateNotifyHandler.h"

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

CreateNotifyHandler::CreateNotifyHandler(XWindowList* wl)
        : EventHandler(wl) {}


// ************************************************************************** //
// **********                    PUBLIC METHODS                    ********** //
// ************************************************************************** //

bool CreateNotifyHandler::processEvent(xcb_generic_event_t* event) 
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

