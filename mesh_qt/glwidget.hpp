#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtWidgets>
#include <QGLWidget>
#include "sdm_mesh_file.hpp"

struct Render_options_t
{
  bool render;  //render this element group
  int  index; //current index for selective elemnet render
};


class GLWidget : public QGLWidget
{
  Q_OBJECT
public:
  explicit GLWidget(QWidget *parent = 0);
  ~GLWidget();

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);

private:
  void reshape(int w, int h);
  void display(void);
  void render_mesh(void);
  void render_element(const std::vector<NODE> &v);
  void render_ids(const std::vector<NODE> &v);
  SDM_MeshFile *sdm_file;
  bool show_wireframe;
  bool show_ids;
  std::vector<Render_options_t> render_vec_elements;
  void increase_rendered_element(void);

  //mouse
  QPoint last_position;
  float x_rotation;
  float y_rotation;


};

#endif // GLWIDGET_H