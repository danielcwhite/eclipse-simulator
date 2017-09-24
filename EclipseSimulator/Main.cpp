#include <iostream>
#include <EclipseMainWindow.hpp>

int main(int argc, char** argv)
{
  QApplication app(argc, const_cast<char**>(argv));

  EclipseMainWindow mainWin;
  mainWin.show();

  return app.exec();
}
