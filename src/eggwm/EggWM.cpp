/**
 * Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * @file /src/eggwm/EggWM.cpp
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

#include <QLocalSocket>
#include <QTimer>

// ************************************************************************** //
// **********              CONSTRUCTORS AND DESTRUCTOR             ********** //
// ************************************************************************** //

const char * requestCodeToText(unsigned char rc) {
    static const char* const names[] = {
        "",
        "CreateWindow",
        "ChangeWindowAttributes",
        "GetWindowAttributes",
        "DestroyWindow",
        "DestroySubwindows",
        "ChangeSaveSet",
        "ReparentWindow",
        "MapWindow",
        "MapSubwindows",
        "UnmapWindow",
        "UnmapSubwindows",
        "ConfigureWindow",
        "CirculateWindow",
        "GetGeometry",
        "QueryTree",
        "InternAtom",
        "GetAtomName",
        "ChangeProperty",
        "DeleteProperty",
        "GetProperty",
        "ListProperties",
        "SetSelectionOwner",
        "GetSelectionOwner",
        "ConvertSelection",
        "SendEvent",
        "GrabPointer",
        "UngrabPointer",
        "GrabButton",
        "UngrabButton",
        "ChangeActivePointerGrab",
        "GrabKeyboard",
        "UngrabKeyboard",
        "GrabKey",
        "UngrabKey",
        "AllowEvents",
        "GrabServer",
        "UngrabServer",
        "QueryPointer",
        "GetMotionEvents",
        "TranslateCoords",
        "WarpPointer",
        "SetInputFocus",
        "GetInputFocus",
        "QueryKeymap",
        "OpenFont",
        "CloseFont",
        "QueryFont",
        "QueryTextExtents",
        "ListFonts",
        "ListFontsWithInfo",
        "SetFontPath",
        "GetFontPath",
        "CreatePixmap",
        "FreePixmap",
        "CreateGC",
        "ChangeGC",
        "CopyGC",
        "SetDashes",
        "SetClipRectangles",
        "FreeGC",
        "ClearArea",
        "CopyArea",
        "CopyPlane",
        "PolyPoint",
        "PolyLine",
        "PolySegment",
        "PolyRectangle",
        "PolyArc",
        "FillPoly",
        "PolyFillRectangle",
        "PolyFillArc",
        "PutImage",
        "GetImage",
        "PolyText8",
        "PolyText16",
        "ImageText8",
        "ImageText16",
        "CreateColormap",
        "FreeColormap",
        "CopyColormapAndFree",
        "InstallColormap",
        "UninstallColormap",
        "ListInstalledColormaps",
        "AllocColor",
        "AllocNamedColor",
        "AllocColorCells",
        "AllocColorPlanes",
        "FreeColors",
        "StoreColors",
        "StoreNamedColor",
        "QueryColors",
        "LookupColor",
        "CreateCursor",
        "CreateGlyphCursor",
        "FreeCursor",
        "RecolorCursor",
        "QueryBestSize",
        "QueryExtension",
        "ListExtensions",
        "ChangeKeyboardMapping",
        "GetKeyboardMapping",
        "ChangeKeyboardControl",
        "GetKeyboardControl",
        "Bell",
        "ChangePointerControl",
        "GetPointerControl",
        "SetScreenSaver",
        "GetScreenSaver",
        "ChangeHosts",
        "ListHosts",
        "SetAccessControl",
        "SetCloseDownMode",
        "KillClient",
        "RotateProperties",
        "ForceScreenSaver",
        "SetPointerMapping",
        "GetPointerMapping",
        "SetModifierMapping",
        "GetModifierMapping",
        "NoOperation",
    };

    if (rc < 1 || rc > (sizeof(names)/sizeof(names[0])))
        return "<UnknownName>";

    return names[rc];
}

static bool bad_alloc = false;

static int errorHandler(Display* display, XErrorEvent* e) {
    char txt[1024];
    XGetErrorText(display, e->error_code, txt, sizeof(txt));
    qDebug() << "Received X error:\n"
         << "    Request: " << int(e->request_code)
         << " - " << requestCodeToText(e->request_code) << "\n"
         << "    Error code: " << int(e->error_code)
         << " - " << txt << "\n"
         << "    Resource ID: " << e->resourceid;
    bad_alloc = static_cast<int>(e->error_code) == BadAccess;

    // The return value is ignored.
    return 0;
}

bool EggWM::checkAnotherWM() {
    bad_alloc = false;

    XSelectInput(QX11Info::display(),
        QX11Info::appRootWindow(QX11Info::appScreen()),
        SubstructureRedirectMask | SubstructureNotifyMask);
    XSync(QX11Info::display(), false);

    if (bad_alloc) {
        qDebug() << "Detected another window manager on display "
                 << XDisplayString(QX11Info::display());
        return true;
    }

    return false;
}

EggWM::EggWM() {

    XSetErrorHandler(errorHandler);

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

    // Establecemos que eventos queremos recibir
    XSelectInput(QX11Info::display(),
            QX11Info::appRootWindow(QX11Info::appScreen()),
              SubstructureRedirectMask /* MapRequest, ConfigureRequest,
                                          CirculateRequest */
            | SubstructureNotifyMask   /* CreateNotify, DestroyNotify,
                                          MapNotify, UnmapNotify,
                                          ReparentNotify, GravityNotify,
                                          ConfigureNotify, CirculateNotify */
            | ButtonPressMask);        /* ButtonPress */

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
    int i = 0;
    for (i = 0 ; data[i] && (isalnum(data[i]) || strchr("-_", data[i])); i++)
        /* empty */;

    data[i] = 0;
    //qDebug() << "received command: '" << data << "'";
    if (!strcmp(data, "ping")) {
        auto a = "  pong\n0:OK\n";
        clientConnection->write(a, strlen(a));
    } else if (!strcmp(data, "reparent-orphans")) {
        reparentOrphans();
        auto a = "  pong\n0:OK\n";
        clientConnection->write(a, strlen(a));
    } else if (!strcmp(data, "reload-config")) {
        Config::getInstance()->loadConfig();
        this->windowList->refreshStyle();
        auto a = "0:OK\n";
        clientConnection->write(a, strlen(a));
    } else {
        auto a = "1:ERROR:Unknown command\n";
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
