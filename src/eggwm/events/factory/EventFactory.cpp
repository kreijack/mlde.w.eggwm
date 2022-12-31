/**
 * Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * @file /src/eggwm/events/factory/EventFactory.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  EventFactory
 * @author José Expósito
 */
#include "EventFactory.h"

// ************************************************************************** //
// **********             STATIC METHODS AND VARIABLES             ********** //
// ************************************************************************** //

EventFactory* EventFactory::instance = NULL;

EventFactory* EventFactory::getInstance() {
    if(EventFactory::instance == NULL)
        EventFactory::instance = new EventFactory();

    return EventFactory::instance;
}


// ************************************************************************** //
// **********                    PUBLIC METHODS                    ********** //
// ************************************************************************** //

void EventFactory::initialize(XWindowList* windowList) {
    this->eventHandlerHash.insert(XCB_MAP_REQUEST, new MapRequestHandler(windowList));
    this->eventHandlerHash.insert(XCB_CONFIGURE_REQUEST, new ConfigureRequestHandler(windowList));
    this->eventHandlerHash.insert(XCB_CREATE_NOTIFY, new CreateNotifyHandler(windowList));
    this->eventHandlerHash.insert(XCB_DESTROY_NOTIFY, new DestroyNotifyHandler(windowList));
    this->eventHandlerHash.insert(XCB_UNMAP_NOTIFY, new UnmapNotifyHandler(windowList));
    this->eventHandlerHash.insert(XCB_BUTTON_PRESS, new ButtonPressHandler(windowList));
    this->eventHandlerHash.insert(XCB_CLIENT_MESSAGE, new ClientMessageHandler(windowList));
}

EventHandler* EventFactory::getEventHandler(int eventType) const {
    if(this->eventHandlerHash.contains(eventType))
        return this->eventHandlerHash.value(eventType);
    else
        return NULL;
}
