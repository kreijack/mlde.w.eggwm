/**
 * Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * @file /src/eggwm/events/handlers/ConfigureRequestHandler.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  ConfigureRequestHandler
 * @author José Expósito
 */
#include "ConfigureRequestHandler.h"

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

ConfigureRequestHandler::ConfigureRequestHandler(XWindowList* wl)
        : EventHandler(wl) {}


// ************************************************************************** //
// **********                    PUBLIC METHODS                    ********** //
// ************************************************************************** //

bool ConfigureRequestHandler::processEvent(xcb_generic_event_t* event) 
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
