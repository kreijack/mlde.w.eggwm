/**
 * Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * @file /src/eggwm/events/handlers/ClientMessageHandler.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  ClientMessageHandler
 * @author José Expósito
 */
#include "ClientMessageHandler.h"

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

ClientMessageHandler::ClientMessageHandler(XWindowList* wl)
        : EventHandler(wl) {}


// ************************************************************************** //
// **********                    PUBLIC METHODS                    ********** //
// ************************************************************************** //

bool ClientMessageHandler::processEvent(xcb_generic_event_t* event) 
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
