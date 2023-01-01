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
    XSetErrorHandler(errorHandler);

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

EggWM::EggWM(int argc, char** argv) : QApplication(argc, argv) {


    if (checkAnotherWM()) {
        qDebug() << "Detect another WM, exit";
        ::exit(100);
    }

    // Inicializamos los atributos
    this->wmCheckWindow = new WMCheckWindow;
    this->windowList    = XWindowList::getInstance();
    this->eventFactory  = EventFactory::getInstance();
    this->eventFactory->initialize();

    // cargamos la configuración
    Config* cfg = Config::getInstance();
    cfg->loadConfig();

    // TODO Añadir a la lista de ventanas las ventanas que ya existan


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
    XFlush(QX11Info::display());

    xev = new XcbEventFilter(this->eventFactory);
    installNativeEventFilter(xev);

}

EggWM::~EggWM() {
    delete this->wmCheckWindow;
    delete this->windowList;
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
