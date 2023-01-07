/**
 * Copyright (C) 2014 Leslie Zhai <xiang.zhai@i-soft.com.cn>
 *
 * @file EggWM.h
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
#ifndef EGGWM_H
#define EGGWM_H

#include "util/Include.h"
#include "standards/EWMHRoot.h"
#include "standards/WMCheckWindow.h"
#include "events/XcbEventFilter.h"

#include <QLocalServer>

/**
 * @~spanish
 * Corazón del gestor de ventanas. Se encarga de procesar los distintos eventos
 * con ayuda de sus correspondientes handlers.
 *
 * @~english
 * Heart of the window manager. Is responsible for process the various events
 * using their associated handlers.
 */
class EggWM : public QObject {
    private:
        Q_OBJECT

        /**
         * @~spanish
         * Véntana para que los clientes sepan que hay un WM ejecutándose.
         *
         * @~english
         * Window used to indicate to the clients that a WM is running.
         */
        WMCheckWindow* wmCheckWindow;

        /**
         * @~spanish
         * Lista con las ventanas gestionadas por el WM.
         *
         * @~english
         * List of windows managed by the WM.
         */
        XWindowList* windowList;

        /**
         * @~english
         * Socket-path.
         */
        QString socketPath;

        /**
         * @~english
         * server for receiving commands
         */
        QLocalServer *socketServer;

        /**
         * @~english
         * name of the display server
         */
        QString displayServer;

        //----------------------------------------------------------------------

        /**
         * @~spanish
         * Envía diversos hints requeridos por el estándar EWMH a la ventana
         * raíz.
         *
         * @~english
         * Send various hints required by the EWMH standard to the root window.
         */
        void sendHints();

        /**
         * @~spanish
         * Asocia cada tipo de evento a su correspondiente handler. Es decir,
         * inicializa eventHandlerHash.
         *
         * @~english
         * Associate each event type with his handler. That is, initialized
         * eventHandlerHash.
         */
        void attachEventHandler();

        /**
         * @~english
         * setup the socket server
         */
        void socketServerSetup();

        /**
         * @~english
         * reparent orphaned window
         */
        void reparentOrphans();

    private slots:
        /**
         * @~english
         * command receiving
         */
        void socketServerReceiveCommand();

    public:

        /**
         * @~english
         * Constructor.
         * @param argc Number of arguments received by console.
         * @param argv Argument list.
         */
        EggWM();

        /**
         * @~english
         * init of the program. May fail
         */
        bool init();

        /**
         * @~spanish
         * Destructor.
         *
         * @~english
         * Destructor.
         */
        virtual ~EggWM();

        XcbEventFilter *xev;

        /**
         * @~english
         * return true if another WM exists
         */
        bool checkAnotherWM();

        /**
         * @~english
         * kill the currently running window manager
         */
        void killWM();

        /**
         * @~english
         * set the socket path
         */
        void setSocketPath(const QString &s) { socketPath = s; }

        /**
         * @~english
         * set display server
         */
        void setDisplayServer(const QString &s) { displayServer = s; }

};

#endif // EGGWM_H
