/**
 * @file xwindows/XWindowList.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  XWindowList
 * @author José Expósito
 */
#include "XWindowList.h"

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

XWindowList::XWindowList() {
    this->clientHash   = new QHash<Window, XWindow*>;
    this->frameHash    = new QHash<Window, XWindow*>;
    this->mappingList  = new QList<XWindow*>;
    this->stackingList = new QList<XWindow*>;
    this->activeWindow = NULL;
}

XWindowList::~XWindowList() {
    /*delete this->clientHash;
    delete this->frameHash;
    delete this->mappingList;
    delete this->stackingList;*/
}


// ************************************************************************** //
// **********                    PRIVATE METHODS                   ********** //
// ************************************************************************** //

void XWindowList::restackWindows() const {
    int numWindows = this->stackingList->size();
    Window windows[numWindows];

    for(int n=numWindows-1; n>=0; n--) {
        const XWindow* aux = this->stackingList->at(n);
        windows[numWindows-1-n] = aux->haveFrame()
                ? aux->getFrameID()
                : aux->getClientID();
    }

    XRestackWindows(QX11Info::display(), windows, numWindows);
}


// ************************************************************************** //
// **********                    PUBLIC METHODS                    ********** //
// ************************************************************************** //

bool XWindowList::existClient(Window clientID) const {
    return this->clientHash->contains(clientID);
}

void XWindowList::addClient(Window clientID, XWindow* xwindow) {
    this->clientHash->insert(clientID, xwindow);
}

void XWindowList::removeClient(Window clientID) {
    this->clientHash->remove(clientID);
}

XWindow* XWindowList::getXWindowByClientID(Window clientID) const {
    return (XWindow*)this->clientHash->value(clientID, NULL);
}

//------------------------------------------------------------------------------

bool XWindowList::existFrame(Window frameID) const {
    return this->frameHash->contains(frameID);
}

void XWindowList::addFrame(Window frameID, XWindow* xwindow) {
    this->frameHash->insert(frameID, xwindow);
}

void XWindowList::removeFrame(Window frameID) {
    this->frameHash->remove(frameID);
}

XWindow* XWindowList::getXWindowByFrameID(Window frameID) const {
    return (XWindow*)this->frameHash->value(frameID, NULL);
}

//------------------------------------------------------------------------------

void XWindowList::addToManagedWindows(XWindow* xwindow) {
    this->mappingList->removeOne(xwindow);
    this->mappingList->append(xwindow);

    this->restackManagedWindow(xwindow);

    EWMHRoot ewmhRoot;
    ewmhRoot.sendMappingClientList(this->mappingList);
    ewmhRoot.sendStackingClientList(this->stackingList);
}

void XWindowList::removeFromManagedWindow(XWindow* xwindow) {
    this->mappingList->removeOne(xwindow);
    this->stackingList->removeOne(xwindow);

    EWMHRoot ewmhRoot;
    ewmhRoot.sendMappingClientList(this->mappingList);
    ewmhRoot.sendStackingClientList(this->stackingList);
}

void XWindowList::restackManagedWindow(XWindow* xwindow) {
    this->stackingList->removeOne(xwindow);
    int numWindows = this->stackingList->size();

    // Ventanas que siempre están arriba
    if(xwindow->isTopWindow()) {
        this->stackingList->append(xwindow);

    // Ventanas que siempre están abajo
    } else if(xwindow->isBottomWindow()) {
        this->stackingList->prepend(xwindow);

    // Resto de ventanas
    } else {
        int  pos = numWindows - 1;
        bool end = false;

        while(pos >= 0 && !end) {
            const XWindow* aux = this->stackingList->at(pos);

            if(aux->isTopWindow())
                pos--;
            else
                end = true;
        }

        this->stackingList->insert(pos+1, xwindow);
    }

    this->restackWindows();

    EWMHRoot ewmhRoot;
    ewmhRoot.sendStackingClientList(this->stackingList);
}

void XWindowList::setActiveWindow(XWindow* activeWindow) {

    if (this->activeWindow)
        this->activeWindow->setFocus(false);
    this->activeWindow = (XWindow*)activeWindow;

    // TODO Si se quiere cambiar la decoración de la ventana activa este es el
    //      sitio ;)

    EWMHRoot ewmhRoot;
    if(this->activeWindow == NULL) {
        ewmhRoot.sendActiveWindow(None);
    } else {
        ewmhRoot.sendActiveWindow(this->activeWindow->getClientID());
        this->activeWindow->setFocus(true);
    }
}

XWindow* XWindowList::getTopWindow() const {
    XWindow* ret = NULL;
    int pos = this->stackingList->size() - 1;

    while(pos >= 0 && ret == NULL) {
        XWindow* aux = this->stackingList->at(pos);

        if(!aux->isTopWindow() && !aux->isBottomWindow()
                && aux->getState() != IconicState)
            ret = aux;
        pos--;
    }

    return ret;
}

bool XWindowList::isTopWindow(XWindow* w) {
    return w == this->getTopWindow();
}

bool XWindowList::isFocused(XWindow* w) {
    return w == this->activeWindow;
}

XWindowList *XWindowList::getInstance() {
    static XWindowList theXWindowList;
    return &theXWindowList;
}

void XWindowList::refreshStyle() {
    for(int i = 0 ; i < this->mappingList->count() ; i++) {
        auto *w = this->mappingList->at(i);
        const_cast<XWindow *>(w)->refreshStyle();
    }
}
