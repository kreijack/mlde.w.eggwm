/**
 * Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * @file /src/eggwm/events/handlers/ButtonPressHandler.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  ButtonPressHandler
 * @author José Expósito
 */
#include "ButtonPressHandler.h"

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

ButtonPressHandler::ButtonPressHandler() {}


// ************************************************************************** //
// **********                    PUBLIC METHODS                    ********** //
// ************************************************************************** //

bool ButtonPressHandler::processEvent(xcb_generic_event_t* event) 
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
