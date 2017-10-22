cmake_minimum_required(VERSION 3.7)

FIND_PACKAGE(Qt5Core REQUIRED HINTS ${Qt5_PATH})
FIND_PACKAGE(Qt5Gui REQUIRED HINTS ${Qt5_PATH})
FIND_PACKAGE(Qt5Widgets REQUIRED HINTS ${Qt5_PATH})
#FIND_PACKAGE(Qt5Network REQUIRED HINTS ${Qt5_PATH})
#FIND_PACKAGE(Qt5OpenGL REQUIRED HINTS ${Qt5_PATH})
#FIND_PACKAGE(Qt5Concurrent REQUIRED HINTS ${Qt5_PATH})

IF(Qt5Core_FOUND)
  MESSAGE(STATUS "Found Qt version: ${Qt5Core_VERSION_STRING}")
  INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${Qt5Gui_INCLUDE_DIRS})
  ADD_DEFINITIONS(${Qt5Core_DEFINITIONS})
  ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})
  ADD_DEFINITIONS(${Qt5Gui_DEFINITIONS})
ELSE()
  MESSAGE(FATAL_ERROR "Qt5 is required for building the SCIRun GUI")
ENDIF()

IF(Qt5Core_FOUND)
  INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
  #INCLUDE(${QT_USE_FILE})
  ADD_DEFINITIONS(${QT_DEFINITIONS})
ELSE()
  MESSAGE(FATAL_ERROR "QT 5.8 or greater is required to build SCIRun")
ENDIF()

ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fPIC")

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(EclipseSimulatorGUI_SOURCES
  EclipseMainWindow.cpp
  Main.cpp
  ShipGraphicsManager.cpp
  ShipSpecEditorDialog.cpp
  ShipWidgetController.cpp
  Simulation.cpp
)

SET(EclipseSimulatorGUI_HEADERS
  EclipseMainWindow.hpp
  ShipSpec.hpp
  Simulation.hpp
  ShipGraphicsManager.hpp
  ShipSpecEditorDialog.hpp
  ShipWidgetController.hpp
)

SET(EclipseSimulatorGUI_FORMS
  EclipseSimulatorMainWindow.ui
  ShipSpecEditor.ui
)

#SET(Interface_Application_RESOURCES
#scirun5.qrc
#)

QT5_WRAP_UI(EclipseSimulatorGUI_FORMS_HEADERS ${EclipseSimulatorGUI_FORMS})
QT5_WRAP_CPP(EclipseSimulatorGUI_HEADERS_MOC ${EclipseSimulatorGUI_HEADERS})
#QT5_ADD_RESOURCES(Interface_Application_RESOURCES_RCC ${Interface_Application_RESOURCES})

add_executable(EclipseSimulatorGUI
  ${EclipseSimulatorGUI_SOURCES}
  ${EclipseSimulatorGUI_HEADERS}
  ${EclipseSimulatorGUI_HEADERS_MOC}
  ${EclipseSimulatorGUI_FORMS_HEADERS}
  #${Interface_Application_RESOURCES_RCC}
)

qt5_use_modules(EclipseSimulatorGUI Widgets)

TARGET_LINK_LIBRARIES(EclipseSimulatorGUI
  ${QT_LIBRARIES}
)
