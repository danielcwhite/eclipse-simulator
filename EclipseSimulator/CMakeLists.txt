cmake_minimum_required(VERSION 3.16)

project(EclipseSimulator)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
MESSAGE(STATUS "Found Qt version: ${Qt6Core_VERSION}")

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
  BattleStateMachine.cpp
  GuiShip.cpp
  DamageApplier.cpp
)

SET(EclipseSimulatorGUI_HEADERS
  ShipSpec.hpp
  Simulation.hpp
  BattleStateMachine.hpp
  GuiShip.hpp
  ShipInterface.hpp
  DamageApplier.hpp
)

SET(EclipseSimulatorGUI_HEADERS_TO_MOC
  EclipseMainWindow.hpp
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

qt_wrap_ui(EclipseSimulatorGUI_FORMS_HEADERS ${EclipseSimulatorGUI_FORMS})
qt_wrap_cpp(EclipseSimulatorGUI_HEADERS_MOC ${EclipseSimulatorGUI_HEADERS_TO_MOC})
#qt_add_resources(EclipseSimulatorGUI_RESOURCES_RCC ${Interface_Application_RESOURCES})

add_executable(EclipseSimulatorGUI
  ${EclipseSimulatorGUI_SOURCES}
  ${EclipseSimulatorGUI_HEADERS}
  ${EclipseSimulatorGUI_HEADERS_TO_MOC}
  ${EclipseSimulatorGUI_HEADERS_MOC}
  ${EclipseSimulatorGUI_FORMS_HEADERS}
  #${Interface_Application_RESOURCES_RCC}
)

TARGET_LINK_LIBRARIES(EclipseSimulatorGUI
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
)
