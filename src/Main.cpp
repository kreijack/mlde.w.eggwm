/**
 * @file Main.cpp
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @author José Expósito
 */
#include "EggWM.h"

#include <QCommandLineParser>
#include <QCommandLineOption>
/**
 * @~spanish
 * Función de inicio de Egg Window Manager.
 * @param argc Número de argumentos recibidos por consola.
 * @param argv Lista de argumentos.
 *
 * @~english
 * Start function for Egg Window Manager.
 * @param argc Number of arguments received by console.
 * @param argv Argument list.
 */
int main(int argc, char** argv) {
    assert(QT_VERSION >= 0x050000);

    QString display_server = getenv("DISPLAY");
    for (int i = 1 ; i < argc -1 ; i++)
        if (!strcmp("--display", argv[i]))
            display_server = argv[i+1];

    QApplication app(argc, argv);
    app.setApplicationName("eggWM");
    app.setApplicationVersion("0.3");

    QCommandLineParser parser;
    parser.setApplicationDescription("eggWM X Window Manager");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption replaceWM("replace",
        "Replace the currently running window manager");
    parser.addOption(replaceWM);

    QCommandLineOption socketPath("socket-path",
        "Set an alternative socket path",
        "socket-path");
    parser.addOption(socketPath);

    parser.process(app);

    EggWM eggWM;

    if (eggWM.checkAnotherWM()) {
        if (parser.isSet(replaceWM)) {
            qDebug() << "Detect another WM, kill it";
            eggWM.killWM();
        } else {
            qDebug() << "Detect another WM, exit";
            return 100;
        }
    }

    if (parser.isSet(socketPath))
        eggWM.setSocketPath(parser.value(socketPath));

    eggWM.setDisplayServer(display_server);

    if (!eggWM.init())
        return 100;

    return app.exec();
}

/*
 * Para debuggear:
 *
 * $ Xephyr :1 -ac -screen 900x700 &
 * En "Proyectos -> Entorno de construcción -> DISPLAY" poner ":1" o lanzar con
 * la opción "-display :1".
 *
 * Para ver el árbol de ventanas:
 * $ xwininfo -display :1 -tree -root
 *
 * Ver propiedades de una ventana:
 * $ xwininfo -display :1
 *
 * Se le puede añadir una composición muy simple con xcompmg:
 * $ xcompmgr -cf -d :1
 *
 * Para debugear el estándar EWMH/NetWM:
 * La herramienta wmctrl nos proporciona diversa información:
 * $ man wmctrl
 * Para ver las propiedades que va estableciendo el WM sobre la root window:
 * $ xprop -root -display :1
 *
 * NOTA -> Mirar QStyle para ver como crear un estilo para los widgets
 */
