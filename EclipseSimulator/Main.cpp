#include <iostream>
#include <EclipseMainWindow.hpp>

int main(int argc, char** argv)
{
  QApplication app(argc, const_cast<char**>(argv));

  QCoreApplication::setOrganizationName("Dan LLC");
  QCoreApplication::setApplicationName("Eclipse Battle Simulator");

  EclipseMainWindow mainWin;
  mainWin.show();

  return app.exec();
}
