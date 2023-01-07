/**
 * Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * @file EggWM.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  EggWM
 * @author José Expósito
 */
#include "EggWM.h"
#include "util/util.h"

#include <QLocalSocket>
#include <QTimer>

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

bool EggWM::checkAnotherWM() {

    bool ret;

    uint32_t values[] = {
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |    /* CreateNotify, DestroyNotify,
                                                   MapNotify, UnmapNotify,
                                                   ReparentNotify, GravityNotify,
                                                   ConfigureNotify, CirculateNotify */
        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |  /* MapRequest, ConfigureRequest,
                                                   CirculateRequest */
        XCB_EVENT_MASK_BUTTON_PRESS             /* ButtonPress */
    };
    auto cookie = xcb_change_window_attributes_checked(QX11Info::connection(),
        QX11Info::appRootWindow(QX11Info::appScreen()),
        XCB_CW_EVENT_MASK,
        values);
    if (xcb_generic_error_t *error;
        (error = xcb_request_check(QX11Info::connection(), cookie))) {
            dumpXCBError(error);
            qDebug() << "Detected another window manager on display "
                     << XDisplayString(QX11Info::display());
            ret = true;
            free(error);
    }

    return ret;
}

EggWM::EggWM() {

    //XSetErrorHandler(errorHandler);

    // Inicializamos los atributos
    this->wmCheckWindow = new WMCheckWindow;
    this->windowList    = XWindowList::getInstance();

    xev = new XcbEventFilter(this->windowList);
}

bool EggWM::init() {

    // cargamos la configuración
    Config* cfg = Config::getInstance();
    cfg->loadConfig();

    socketServerSetup();
    // TODO Añadir a la lista de ventanas las ventanas que ya existan
    reparentOrphans();

    // Establecemos diversas propiedades requeridas por el estándar EWMH
    this->sendHints();

    uint32_t values[] = {
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |    /* CreateNotify, DestroyNotify,
                                                   MapNotify, UnmapNotify,
                                                   ReparentNotify, GravityNotify,
                                                   ConfigureNotify, CirculateNotify */
        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |  /* MapRequest, ConfigureRequest,
                                                   CirculateRequest */
        XCB_EVENT_MASK_BUTTON_PRESS             /* ButtonPress */
    };
    auto cookie = xcb_change_window_attributes_checked(QX11Info::connection(),
        QX11Info::appRootWindow(QX11Info::appScreen()),
        XCB_CW_EVENT_MASK,
        values);
    if (xcb_generic_error_t *error;
        (error = xcb_request_check(QX11Info::connection(), cookie))) {
        qDebug() << "Cannot reset window attribute"
                 << XDisplayString(QX11Info::display());
        dumpXCBError(error);
        free(error);
        ::exit(100);
    }

    xcb_grab_button(QX11Info::connection(),
        1,
        QX11Info::appRootWindow(QX11Info::appScreen()),
        XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
        XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_SYNC,
        None, None, XCB_BUTTON_INDEX_1, XCB_MOD_MASK_ANY);

    QApplication::instance()->installNativeEventFilter(xev);

    return true;
}

EggWM::~EggWM() {
    delete this->wmCheckWindow;
}


// ************************************************************************** //
// **********                    PRIVATE METHODS                   ********** //
// ************************************************************************** //

void EggWM::sendHints() {
    EWMHRoot ewmhRoot;

    ewmhRoot.sendSupportedHints();
    ewmhRoot.sendActiveWindow(None);
    ewmhRoot.sendSupportingWmCheck(this->wmCheckWindow->winId());

    ewmhRoot.sendNumberOfDesktops(1.0);
    ewmhRoot.sendDesktopNames("Egg Desktop");
    ewmhRoot.sendDesktopGeometry((long)QApplication::desktop()->width(),
            (long)QApplication::desktop()->height());
    ewmhRoot.sendDesktopViewport(0.0, 0.0);
    ewmhRoot.sendWorkarea(0.0, 0.0, (long)QApplication::desktop()->width(),
            (long)QApplication::desktop()->height());
    ewmhRoot.sendCurrentDesktop(0.0);
    ewmhRoot.sendSupportShowingdesktop(0.0);
}

void EggWM::socketServerSetup() {
    if (socketPath.size() == 0)
        socketPath = Config::getInstance()->getSocketName() +
            "-" + displayServer;

    socketServer = new QLocalServer(this);

    socketServer->removeServer(socketPath);
    socketServer->listen(socketPath);
    connect(socketServer, &QLocalServer::newConnection, this,
        &EggWM::socketServerReceiveCommand);
}

void EggWM::socketServerReceiveCommand() {
    char data[1024];

    auto *clientConnection = socketServer->nextPendingConnection();
    if( !clientConnection->waitForConnected() )
        return;
    if( !clientConnection->waitForReadyRead() )
        return;
    connect(clientConnection, &QLocalSocket::disconnected,
            clientConnection, &QLocalSocket::deleteLater);

    unsigned long l = 0;
    for(;;) {
        auto r = clientConnection->read(data + l, 1023 - l);
        if (r <= 0)
            break;
        l += r;
    }

    if (l < 1) {
        clientConnection->disconnectFromServer();
        return;
    }

    data[l] = 0;
    auto fields = QString(data).trimmed().split(" ");

    if (fields.count() > 0) {
        if (fields[0] == "ping") {
            auto a = "  pong\n0:OK\n";
            clientConnection->write(a, strlen(a));
        } else if (fields[0] == "reparent-orphans") {
            reparentOrphans();
            auto a = "  pong\n0:OK\n";
            clientConnection->write(a, strlen(a));
        } else if (fields[0] == "reload-config") {
            Config::getInstance()->loadConfig();
            this->windowList->refreshStyle();
            auto a = "0:OK\n";
            clientConnection->write(a, strlen(a));
        } else if (fields[0] == "debug-dump-events" && fields.count() == 2) {
            Config::getInstance()->setDebugDumpEvents(fields[1].toInt() != 0);
        } else {
            auto a = "1:ERROR:Unknown command\n";
            clientConnection->write(a, strlen(a));
        }
    } else {
        auto a = "2:ERROR:Empty command\n";
        clientConnection->write(a, strlen(a));
    }
    clientConnection->flush();
    clientConnection->disconnectFromServer();
}

void EggWM::reparentOrphans() {
    Window root, parent;
    Window* windows;
    unsigned int windows_count;
    struct x11window {
        Window  id;
        int x, y, w, h;
    };

    QList<x11window> x11windowsList;

    XGrabServer(QX11Info::display());
    XQueryTree(QX11Info::display(),
            QX11Info::appRootWindow(QX11Info::appScreen()),
            &root,
            &parent,
            &windows,
            &windows_count);
    assert(root == QX11Info::appRootWindow(QX11Info::appScreen()));

    for (unsigned int i = 0; i < windows_count; ++i) {
        auto windowID = windows[i];

        XWindowAttributes attrs;
        XGetWindowAttributes(QX11Info::display(), windowID, &attrs);

        if (attrs.override_redirect || attrs.map_state != IsViewable)
            continue;

        if (this->windowList->existClient(windowID))
            continue;
        if (this->windowList->existFrame(windowID))
            continue;

        XWindow* xwindow = new XWindow(windowID);
        this->windowList->addClient(windowID, xwindow);
        if (xwindow->bypassWM())
            continue;

        assert(xwindow->getState() == WithdrawnState);

        if (xwindow->needFrame()) {
            xwindow->addFrame();
            this->windowList->addFrame(xwindow->getFrameID(),
                                    xwindow);
        }

        xwindow->setState(NormalState);

        this->windowList->addToManagedWindows(xwindow);
        this->windowList->setActiveWindow(this->windowList->getTopWindow());

        x11windowsList.append({
            .id =   windowID,
            .x =    attrs.x,
            .y =    attrs.y,
            .w =    attrs.width,
            .h =    attrs.height
        });
    }

    XFree(windows);
    XUngrabServer(QX11Info::display());
    XSync(QX11Info::display(), true);

    // defer the refresh after processing the pending events
    // otherwise the system is confused by spurious XUnmapEvent
    QTimer::singleShot(500, [x11windowsList](){
        auto wl = XWindowList::getInstance();

        for (auto x11w : x11windowsList) {
            auto w = wl->getXWindowByClientID(x11w.id);
            if (!w)
                continue;
            w->setState(NormalState);
            w->setX(x11w.x);
            w->setY(x11w.y);
            w->setWidth(x11w.w);
            w->setHeight(x11w.h);
            wl->addToManagedWindows(w);
        }

        wl->setActiveWindow(wl->getTopWindow());
    });
}

void EggWM::killWM() {
    auto al = AtomList::getInstance();

    unsigned long* prop;
    unsigned long num;

    Atom atom;
    int size;
    unsigned long bytesAfter;

    int ret = XGetWindowProperty(QX11Info::display(),
            QX11Info::appRootWindow(QX11Info::appScreen()),
            al->getAtom("_NET_SUPPORTING_WM_CHECK"),
            0, 1, false, XA_WINDOW,
            &atom, &size, &num,
            &bytesAfter, (unsigned char**)&prop);

    if (ret == Success) {
        XKillClient(QX11Info::display(), prop[0]);
    }
    XFree(prop);

}
