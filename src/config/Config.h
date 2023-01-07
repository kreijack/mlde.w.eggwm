/**
 * @file config/Config.h
 *
 * @~spanish
 * Este archivo es parte del proyecto Egg Window Manager, usted puede
 * redistribuirlo y/o modificarlo bajo los téminos de la licencia GNU GPL v3.
 *
 * @~english
 * This file is part of the Egg Window Manager project, you can redistribute it
 * and/or modify it under the terms of the GNU GPL v3.
 *
 * @class  Config
 * @author José Expósito
 */
#ifndef CONFIG_H
#define CONFIG_H

#include "util/Include.h"


/**
 * @~spanish
 * Singleton que facilita el acceso a la configuración del WM.
 *
 * @~english
 * Singleton that provides access to the configuration of WM.
 */
class Config {

    public:

        /**
         * @~spanish
         * Distintas posiciones que pueden tomar los elementos del tema: áreas
         * dentro de la barra de título (LEFT, RIGHT, CENTER) o la alineación
         * vertical de los iconos (CENTER, TOP, BOTTOM).
         *
         * @~english
         * Different positions that can take the elements of the theme: areas in
         * the title bar (LEFT, RIGHT, CENTER) or vertical alignment of icons
         * (CENTER, TOP, BOTTOM).
         */
        enum Aling { LEFT, RIGHT, CENTER, TOP, BOTTOM };        

    private:

        // Constantes para acceder a la configuración
        static inline const char* USR_CONFIG_DIR       = "/usr/share/eggwm";
        static inline const char* HOME_CONFIG_DIR      = ".eggwm";

        static inline const char* CONFIG_FILE_PATH     = "/.eggwm/eggwm.conf";
        static inline const char* USED_THEME           = "theme/name";

        // Carpetas usadas por el WM
        static inline const char* THEMES_PATH          = "/.eggwm/themes/";

        // Nombre de los archivos que componen el tema de ventanas
        static inline const char* THEME_INFO           = "/theme.inf";
        static inline const char* STYLE_QSS            = "/style.qss";
        static inline const char* FOCUSED_STYLE_QSS    = "/focused_style.qss";

        // Constantes para acceder a las propiedades del tema de ventana
        static inline const char* SHOW_ICON             = "theme/show_icon";

        static inline const char* TITLEBAR_WIDTH        = "theme/titlebar_width";
        static inline const char* TOP_BORDER_WIDTH      = "theme/top_border_width";
        static inline const char* BOTTOM_BORDER_WIDTH   = "theme/bottom_border_width";
        static inline const char* LEFT_BORDER_WIDTH     = "theme/left_border_width";
        static inline const char* RIGHT_BORDER_WIDTH    = "theme/right_border_width";
        static inline const char* ICON_SIZE             = "theme/icon_size";

        static inline const char* MINIMIZE_BUTTON_POS   = "theme/minimize_button_pos";
        static inline const char* MAXIMIZE_BUTTON_POS   = "theme/maximize_button_pos";
        static inline const char* EXIT_BUTTON_POS       = "theme/exit_button_pos";
        static inline const char* ICON_POS              = "theme/icon_pos";
        static inline const char* TITLE_POS             = "theme/title_pos";

        static inline const char* MINIMIZE_BUTTON_ALING = "theme/minimize_button_aling";
        static inline const char* MAXIMIZE_BUTTON_ALING = "theme/maximize_button_aling";
        static inline const char* EXIT_BUTTON_ALING     = "theme/exit_button_aling";

        static inline const char* SOCKET_NAME           = "socket_name";

        //----------------------------------------------------------------------

        // Variables para almacenar la configuración
        bool showIcon;

        int titlebarWidth;
        int topBorderWidth;
        int bottomBorderWidth;
        int leftBorderWidth;
        int rightBorderWidth;
        int iconSize;

        QPair<Aling, int> minimizeButtonPos;
        QPair<Aling, int> maximizeButtonPos;
        QPair<Aling, int> exitButtonPos;
        QPair<Aling, int> iconPos;
        QPair<Aling, int> titlePos;

        Aling minimizeButtonAling;
        Aling maximizeButtonAling;
        Aling exitButtonAling;

        QString style;
        QString focusedStyle;

        QString socketName;

        //----------------------------------------------------------------------

        /**
         * @~spanish
         * Copia el directorio indicado.
         * @param  src  Directorio a copiar.
         * @param  dest Destino donde copiarlo.
         * @return Si se ha podido copiar el directorio.
         *
         * @~english
         * Copy the specified directory.
         * @param  src  Directoy to copy.
         * @param  dest Place to copy it.
         * @return If it can be copied or not.
         */
        bool copyDir(QDir src, QDir dest) const;

        /**
         * @~spanish
         * Devuelve un QPair<Aling, int> con la posición del elemento indicado,
         * siendo el primer elemento del par el área de la barra de título donde
         * se encuentra y el segundo su posición respecto al resto de elementos.
         * @param themeSettings Configuración de la que se leerá la propiedad.
         * @param key           Propiedad a leer.
         * @return La posición.
         *
         * @~english
         * Returns a <Aling, QPair int> with the position of the item, the first
         * element of the pair is the area in the title bar and the second is
         * its position relative to other elements.
         * @param themeSettings QSettings where the property will be read.
         * @param key           Property to read.
         * @return The position
         */
        static QPair<Aling, int> getItemPos(const QSettings& themeSettings,
                const QString& key);

        /**
         * @~spanish
         * Devuelve la alineación vertical del icono indicado.
         * @param themeSettings Configuración de la que se leerá la propiedad.
         * @param key           Propiedad a leer.
         *
         * @~english
         * Returns the vertical alignment of the icon shown.
         * @param themeSettings QSettings where the property will be read.
         * @param key           Property to read.
         */
        static Aling getIconAling(const QSettings& themeSettings,
                const QString& key);

        //----------------------------------------------------------------------

        // Ocultamos las constructoras
        Config(){}
        Config(const Config&){}
        const Config& operator = (const Config& c){return c;}

    public:

        /**
         * @~spanish
         * Único método para obtener una instancia de la clase.
         * @return La única instancia de la clase.
         *
         * @~english
         * Only method to get an instance of the class.
         * @return The single instance of the class.
         */
        static Config* getInstance();

        /**
         * @~spanish
         * (Re)carga la configuración.
         *
         * @~english
         * (Re)load the config.
         */
        void loadConfig();

        //----------------------------------------------------------------------

        /**
         * @~spanish
         * Devuelve el ancho de la barra de título de las ventanas (en pixels).
         * @return El ancho.
         *
         * @~english
         *
         */
        int getTitlebarWidth() const;

        /**
         * @~spanish
         * Devuelve el ancho del borde superior de las ventanas (en pixels).
         * @return El ancho.
         *
         * @~english
         * Returns the width of the top border of windows (in pixels).
         * @return The width.
         */
        int getTopBorderWidth() const;

        /**
         * @~spanish
         * Devuelve el ancho del borde inferior de las ventanas (en pixels).
         * @return El ancho.
         *
         * @~english
         * Returns the width of the bottom border of windows (in pixels).
         * @return The width.
         */
        int getBottomBorderWidth() const;

        /**
         * @~spanish
         * Devuelve el ancho del borde izquierdo de las ventanas (en pixels).
         * @return El ancho.
         *
         * @~english
         * Returns the width of the left border of windows (in pixels).
         * @return The width.
         */
        int getLeftBorderWidth() const;

        /**
         * @~spanish
         * Devuelve el ancho del borde derecho de las ventanas (en pixels).
         * @return El ancho.
         *
         * @~english
         * Returns the width of the right border of windows (in pixels).
         * @return The width.
         */
        int getRightBorderWidth() const;

        /**
         * @~spanish
         * Devuelve el tamaño del icono.
         * @return El tamaño.
         *
         * @~english
         * Returns the size of the window icon.
         * @return The size.
         */
        int getIconSize() const;

        /**
         * @~spanish
         * Devuelve la posición del icono.
         * @return Par con la posición, siendo el primer elemento del par el
         *         área de la barra de título donde se encuentra y el segundo su
         *         posición respecto al resto de elementos.
         *
         * @~english
         * Returns the position of the icon.
         * @return Pair with the position, the first element of the pair is the
         *         area in the title bar and the second is its position relative
         *         to other elements.
         */
        QPair<Aling, int> getIconPos() const;

        /**
         * @~spanish
         * Devuelve la posición del botón de minimizar.
         * @return Par con la posición, siendo el primer elemento del par el
         *         área de la barra de título donde se encuentra y el segundo su
         *         posición respecto al resto de elementos.
         *
         * @~english
         * Returns the position of the minimize button.
         * @return Pair with the position, the first element of the pair is the
         *         area in the title bar and the second is its position relative
         *         to other elements.
         */
        QPair<Aling, int> getMinimizeButtonPos() const;

        /**
         * @~spanish
         * Devuelve la posición del botón de maximizar.
         * @return Par con la posición, siendo el primer elemento del par el
         *         área de la barra de título donde se encuentra y el segundo su
         *         posición respecto al resto de elementos.
         *
         * @~english
         * Returns the position of maximize button.
         * @return Pair with the position, the first element of the pair is the
         *         area in the title bar and the second is its position relative
         *         to other elements.
         */
        QPair<Aling, int> getMaximizeButtonPos() const;

        /**
         * @~spanish
         * Devuelve la posición del botón de cerrar.
         * @return Par con la posición, siendo el primer elemento del par el
         *         área de la barra de título donde se encuentra y el segundo su
         *         posición respecto al resto de elementos.
         *
         * @~english
         * Returns the position of the exit button.
         * @return Pair with the position, the first element of the pair is the
         *         area in the title bar and the second is its position relative
         *         to other elements.
         */
        QPair<Aling, int> getExitButtonPos() const;

        /**
         * @~spanish
         * Indica si el icono se mostrará en la barra de título.
         * @return true si se mostrará, false si no.
         *
         * @~english
         * Indicates whether the icon is displayed in the title bar.
         * @return true if whether to display, false if not.
         */
        bool isIconVisible() const;

        /**
         * @~spanish
         * Devuelve la posición del título.
         * @return Par con la posición, siendo el primer elemento del par el
         *         área de la barra de título donde se encuentra y el segundo su
         *         posición respecto al resto de elementos.
         *
         * @~english
         * Returns the position of the title.
         * @return Pair with the position, the first element of the pair is the
         *         area in the title bar and the second is its position relative
         *         to other elements.
         */
        QPair<Aling, int> getTitlePos() const;

        /**
         * @~spanish
         * Devuelve la alineación vertical del icono minimizar.
         * @return La alineación vertical.
         *
         * @~english
         * Returns the vertical align of the minimize button.
         * @return The vertical align.
         */
        Aling getMinimizeButtonAling() const;

        /**
         * @~spanish
         * Devuelve la alineación vertical del icono maximizar.
         * @return La alineación vertical.
         *
         * @~english
         * Returns the vertical align of the maximize button.
         * @return The vertical align.
         */
        Aling getMaximizeButtonAling() const;

        /**
         * @~spanish
         * Devuelve la alineación vertical del icono salir.
         * @return La alineación vertical.
         *
         * @~english
         * Returns the vertical align of the exit button.
         * @return The vertical align.
         */
        Aling getExitButtonAling() const;

        /**
         * @~spanish
         * Devuelve el estilo que se usará en las ventanas para decorarlas.
         * @return El estilo.
         *
         * @~english
         * Returns the style that will be used to decorate windows.
         * @return The style
         */
        QString getStyle() const;

        /**
         * @~english
         * Returns the style that will be used to decorate a focused windows.
         * It is based on the "default" style
         * @return The style
         */
        QString getFocusedStyle() const;

        /**
         * @~english
         * @return The socket name
         */
        QString getSocketName() const;
};

#endif // CONFIG_H
