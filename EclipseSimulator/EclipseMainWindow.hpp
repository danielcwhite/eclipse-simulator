#ifndef INTERFACE_APPLICATION_SCIRUN_MAIN_WINDOW_H
#define INTERFACE_APPLICATION_SCIRUN_MAIN_WINDOW_H

#include <QMainWindow>
#include "ui_EclipseSimulatorMainWindow.h"

class EclipseMainWindow : public QMainWindow, public Ui::EclipseSimMainWindow
{
    Q_OBJECT

public:
    EclipseMainWindow();
};


#endif
