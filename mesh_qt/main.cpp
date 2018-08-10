#include <QApplication>
#include <QtOpenGL/QGLWidget>
#include "GLWidget.hpp"

int main(int argc, char *argv[]) 
{
  QApplication app(argc, argv);
  GLWidget window;
  window.setGeometry(100,100,1440,900);
  window.show();
  return app.exec();
}

