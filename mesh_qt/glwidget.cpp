#include "glwidget.hpp"

const std::string mesh_file("../mesh.1.1.1.0.8.8.sdm");

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GLWidget::GLWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////

GLWidget::GLWidget(QWidget *parent) 
  : QGLWidget(parent), show_wireframe(true), show_ids(true)
{

  sdm_file = new SDM_MeshFile(mesh_file);
  sdm_file->read_sdm_file();

  //render all element types and all entries by default
  for (size_t i = 0; i < sdm_file->number_vec_elements; i++)
  {
    Render_options_t opt;
    opt.index = -1;
    opt.render = true;
    this->render_vec_elements.push_back(opt);
  }

  x_rotation = 20.0f;
  y_rotation = 30.0f;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GLWidget::~GLWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////

GLWidget::~GLWidget()
{
  delete sdm_file;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GLWidget::resizeGL
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GLWidget::resizeGL(int w, int h)
{
  this->reshape(w, h);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GLWidget::initializeGL
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GLWidget::initializeGL()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GLWidget::paintGL
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GLWidget::paintGL()
{
  this->display();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GLWidget::reshape
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GLWidget::reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho (-1.0*(GLfloat)w/(GLfloat)h, 1.0*(GLfloat)w/(GLfloat)h, -1.0, 1.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GLWidget::display
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GLWidget::display(void) 
{  
  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
  glClear(GL_COLOR_BUFFER_BIT); 
  glLoadIdentity();
  glRotatef(x_rotation, 1.0f, 0.0f, 0.0f);
  glRotatef(y_rotation, 0.0f, 1.0f, 0.0f);
  glTranslatef(-0.5f, -0.5f, 0.0f); 
  render_mesh();
}  

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GLWidget::keyPressEvent
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GLWidget::keyPressEvent(QKeyEvent *e)
{
  switch (e->key())
  {
  case Qt::Key_I:
    show_ids = ! show_ids;
    updateGL();
    break;
  case Qt::Key_W:
    show_wireframe = ! show_wireframe;
    updateGL();
    break;
  case Qt::Key_F1:
    if(this->render_vec_elements.size() >= 1)
    {
      this->render_vec_elements[ 0 ].render = ! this->render_vec_elements[ 0 ].render;
      updateGL();
    }
    break;
  case Qt::Key_F2:
    if(this->render_vec_elements.size() >= 2)
    {
      this->render_vec_elements[ 1 ].render = ! this->render_vec_elements[ 1 ].render;
      updateGL();
    }
    break;
  case Qt::Key_Right:
    increase_rendered_element();
    updateGL();
    break;
  default:
    QWidget::keyPressEvent(e);
  }


}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GLWidget::increase_rendered_element
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GLWidget::increase_rendered_element(void)
{
  for (size_t k = 0; k < sdm_file->number_vec_elements; k++)
  {
    if (this->render_vec_elements[ k ].render)
    {
      this->render_vec_elements[ k ].index++;

      if(this->render_vec_elements[ k ].index >= sdm_file->vec_elements[ k ].size())
      {
        this->render_vec_elements[ k ].index = 0;
      }

      size_t current_element = this->render_vec_elements[ k ].index;
      for (size_t i = 0; i < sdm_file->vec_elements[ k ].size(); i++)
      {
        if ( i != current_element )
        {
          sdm_file->vec_elements[ k ].at(i).render = false;
        }
        else
        {
          sdm_file->vec_elements[ k ].at(i).render = true;
        }
      }
    }
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GLWidget::mousePressEvent
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GLWidget::mousePressEvent(QMouseEvent *e)
{
  last_position = e->pos();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GLWidget::mouseMoveEvent
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
  int dx = e->x() - last_position.x();
  int dy = e->y() - last_position.y();

  if (e->buttons() & Qt::LeftButton) 
  {
    x_rotation += dy;
    y_rotation += dx;
    updateGL();
  } 
  else if (e->buttons() & Qt::RightButton) 
  {

  }

  last_position = e->pos();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//GLWidget::render_mesh
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GLWidget::render_mesh(void)
{
  for (size_t k = 0; k < sdm_file->number_vec_elements; k++)
  {
    if (this->render_vec_elements[ k ].render)
    {
      for (size_t i = 0; i < sdm_file->vec_elements[k].size(); i++)
      {
        ELEMENT elm = sdm_file->vec_elements[k].at(i);
        if (elm.render)
        {
          std::vector<NODE> vertices(elm.number_nodes);
          for (size_t j = 0; j < elm.number_nodes; j++)
          {
            NODE node = sdm_file->vec_nodes.at(elm.nodes[j] - 1); //node ids are the zero based index
            vertices[j] = node;
          }
          render_element(vertices);
        }
      }
    }
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//render_element
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GLWidget::render_ids(const std::vector<NODE> &v) 
{
  //render text (enable GL_FILL)
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  for (size_t i = 0; i < v.size(); i++)
  {
    char buf[2];
    sprintf(buf, "%d", v[i].id);
    this->renderText(v[i].coord[0], v[i].coord[1], v[i].coord[2], buf);
  }

}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//render_element
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GLWidget::render_element(const std::vector<NODE> &v) 
{

  if ( show_wireframe )
  {
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  }
  else 
  {
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //hex08
  /////////////////////////////////////////////////////////////////////////////////////////////////////


  if ( v.size() == 8)
  {
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);

    // x, y back
    glBegin(GL_POLYGON);  
    glVertex3f(v[0].coord[0], v[0].coord[1], v[0].coord[2]);
    glVertex3f(v[1].coord[0], v[1].coord[1], v[1].coord[2]);
    glVertex3f(v[2].coord[0], v[2].coord[1], v[2].coord[2]);
    glVertex3f(v[3].coord[0], v[3].coord[1], v[3].coord[2]);
    glEnd();

    // x, y front
    glBegin(GL_POLYGON);
    glVertex3f(v[4].coord[0], v[4].coord[1], v[4].coord[2]);
    glVertex3f(v[5].coord[0], v[5].coord[1], v[5].coord[2]);
    glVertex3f(v[6].coord[0], v[6].coord[1], v[6].coord[2]);
    glVertex3f(v[7].coord[0], v[7].coord[1], v[7].coord[2]);
    glEnd();

    // x, z right
    glBegin(GL_POLYGON);
    glVertex3f(v[5].coord[0], v[5].coord[1], v[5].coord[2]);
    glVertex3f(v[1].coord[0], v[1].coord[1], v[1].coord[2]);
    glVertex3f(v[2].coord[0], v[2].coord[1], v[2].coord[2]);
    glVertex3f(v[6].coord[0], v[6].coord[1], v[6].coord[2]);
    glEnd();

    // x, z left
    glBegin(GL_POLYGON);
    glVertex3f(v[4].coord[0], v[4].coord[1], v[4].coord[2]);
    glVertex3f(v[0].coord[0], v[0].coord[1], v[0].coord[2]);
    glVertex3f(v[3].coord[0], v[3].coord[1], v[3].coord[2]);
    glVertex3f(v[7].coord[0], v[7].coord[1], v[7].coord[2]);
    glEnd();

    if ( show_ids )
    {
      this->render_ids(v);
    }

  } //hex08


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //psm06
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if ( v.size() == 6)
  {
    glColor4f(0.0f, 1.0f, 0.0f, 0.5f);

    //bottom triangle
    glBegin(GL_POLYGON);
    glVertex3f(v[0].coord[0], v[0].coord[1], v[0].coord[2]);
    glVertex3f(v[1].coord[0], v[1].coord[1], v[1].coord[2]);
    glVertex3f(v[2].coord[0], v[2].coord[1], v[2].coord[2]);
    glEnd();

    //top triangle
    glBegin(GL_POLYGON);
    glVertex3f(v[3].coord[0], v[3].coord[1], v[3].coord[2]);
    glVertex3f(v[4].coord[0], v[4].coord[1], v[4].coord[2]);
    glVertex3f(v[5].coord[0], v[5].coord[1], v[5].coord[2]);
    glEnd();

    //face
    glBegin(GL_POLYGON);
    glVertex3f(v[5].coord[0], v[5].coord[1], v[5].coord[2]);
    glVertex3f(v[4].coord[0], v[4].coord[1], v[4].coord[2]);
    glVertex3f(v[1].coord[0], v[1].coord[1], v[1].coord[2]);
    glVertex3f(v[2].coord[0], v[2].coord[1], v[2].coord[2]);
    glEnd();

    //face
    glBegin(GL_POLYGON);
    glVertex3f(v[5].coord[0], v[5].coord[1], v[5].coord[2]);
    glVertex3f(v[3].coord[0], v[3].coord[1], v[3].coord[2]);
    glVertex3f(v[0].coord[0], v[0].coord[1], v[0].coord[2]);
    glVertex3f(v[2].coord[0], v[2].coord[1], v[2].coord[2]);
    glEnd();

    if ( show_ids )
    {
      this->render_ids(v);
    }

  } //psm06


}



