
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <GL/glut.h>

const std::string mesh_file("../mesh.1.1.1.0.8.8.sdm");

struct NODE
{
  int id;
  double coord[3];
};

struct ELEMENT
{
  int *nodes;
  size_t number_nodes;
  std::string name;
};

size_t number_vec_elements = 0;
std::vector<ELEMENT> *vec_elements;
std::vector<NODE> vec_nodes;

void display(void);
void reshape(int w, int h); 
void key (unsigned char key, int x, int y);
void render_mesh(void);
void clear(void);
void read_input(const std::string& file_name);
void render_element(const std::vector<NODE> &v);
void print_bitmap_string(char* s);
bool show_wireframe = true;
bool show_ids = true;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) 
{
  atexit(clear);
  read_input(mesh_file);
  glutInit(&argc, argv); 
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100,100);
  glutInitWindowSize(1440,900);
  glutCreateWindow ("mesh"); 
  glutDisplayFunc(display); 
  glutReshapeFunc(reshape);
  glutKeyboardFunc(key); 
  glutMainLoop(); 
  return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//reshape
/////////////////////////////////////////////////////////////////////////////////////////////////////

void reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho (-1.0*(GLfloat)w/(GLfloat)h, 1.0*(GLfloat)w/(GLfloat)h, -1.0, 1.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//clear
/////////////////////////////////////////////////////////////////////////////////////////////////////

void clear(void) 
{  
  vec_nodes.clear();

  for (size_t k = 0; k < number_vec_elements; k++)
  {
    for (size_t i = 0; i < vec_elements[k].size(); i++)
    {
      ELEMENT elm = vec_elements[k].at(i);
      free ( elm.nodes );
    }
    vec_elements[k].clear();
  }

  delete[] vec_elements;

}  



/////////////////////////////////////////////////////////////////////////////////////////////////////
//read_input
/////////////////////////////////////////////////////////////////////////////////////////////////////

void read_input(const std::string& file_name)
{
  FILE *file;
  char str[256];
  size_t size_nodes;
  size_t size_elements;
  size_t number_nodes;

  //dimensions of the block
  int lx,ly,lz;

  //node bias flag
  int nb;

  //total number of domains (8/64/128/256)
  int dtot;

  //number of elements per domain (8/64/512/4096/15625)
  int eld;

  file = fopen(file_name.c_str(), "r");

  fscanf(file, "%d %d %d %d %d %d", &lx, &ly, &lz, &nb, &dtot, &eld);
  fscanf(file, "%s %d", str, &number_vec_elements);
  printf("%s %d\n", str, number_vec_elements);
  fscanf(file, "%s %d", str, &size_nodes);
  printf("%s %d\n", str, size_nodes);

  vec_elements = new std::vector<ELEMENT>[number_vec_elements];

  vec_nodes.reserve(size_nodes);

  for (size_t i = 0; i < size_nodes; i++)
  {
    NODE node;
    fscanf(file, "%d %lf %lf %lf", &node.id, &node.coord[0], &node.coord[1], &node.coord[2]);
    printf("%d %lf %lf %lf\n", node.id, node.coord[0], node.coord[1], node.coord[2]);
    vec_nodes.push_back(node);
  }

  for (size_t k = 0; k < number_vec_elements; k++)
  {
    fscanf(file, "%s %d %d", str, &number_nodes, &size_elements);
    printf("%s %d %d\n", str, number_nodes, size_elements);

    vec_elements[k].reserve(size_elements);

    for (size_t i = 0; i < size_elements; i++)
    {
      ELEMENT elm;
      int n;
      elm.nodes = (int*) malloc ( number_nodes * sizeof(int) );
      elm.number_nodes = number_nodes;
      elm.name = str;

      fscanf(file, "%d", &n);
      printf("%d ", n);

      for (size_t j = 0; j < number_nodes; j++)
      {
        fscanf(file, "%d", &elm.nodes[j]);
        printf("%d ", elm.nodes[j]);
      }

      printf("\n");

      vec_elements[k].push_back(elm);
    }

  }

  fclose(file);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//key
/////////////////////////////////////////////////////////////////////////////////////////////////////


void key (unsigned char key, int, int)
{
  switch (key)
  {
  case 'w':
    show_wireframe = ! show_wireframe;
    break;
  case 'i':
    show_ids = ! show_ids;
    break;

  }

  display();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//display
/////////////////////////////////////////////////////////////////////////////////////////////////////

void display(void) 
{  
  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
  glClear(GL_COLOR_BUFFER_BIT); 
  glLoadIdentity();
  glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
  glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
  glTranslatef(-0.5f, -0.5f, 0.0f); 
  render_mesh();
  glutSwapBuffers();
}  


/////////////////////////////////////////////////////////////////////////////////////////////////////
//render_mesh
/////////////////////////////////////////////////////////////////////////////////////////////////////

void render_mesh(void)
{

  if ( show_wireframe )
  {
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
  }

  else 
  {
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  }


  for (size_t k = 0; k < number_vec_elements; k++)
  {
    for (size_t i = 0; i < vec_elements[k].size(); i++)
    {
      ELEMENT elm = vec_elements[k].at(i);
      std::vector<NODE> vertices(elm.number_nodes);
      for (size_t j = 0; j < elm.number_nodes; j++)
      {
        NODE node = vec_nodes.at(elm.nodes[j] - 1); //node ids are the zero based index
        vertices[j] = node;
      }
      render_element(vertices);
    }
  }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//print_bitmap_string
/////////////////////////////////////////////////////////////////////////////////////////////////////

void print_bitmap_string(char* s)
{
  while (*s) 
  {
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, *s);
    s++;
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//render_element
/////////////////////////////////////////////////////////////////////////////////////////////////////

void render_element(const std::vector<NODE> &v) 
{
  char buf[2];

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
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      for (size_t i = 0; i < v.size(); i++)
      {
        glRasterPos3f(v[i].coord[0], v[i].coord[1], v[i].coord[2]);
        sprintf(buf, "%d", v[i].id);
        print_bitmap_string(buf);
      }
    }

  }

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
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      for (size_t i = 0; i < v.size(); i++)
      {
        glRasterPos3f(v[i].coord[0], v[i].coord[1], v[i].coord[2]);
        sprintf(buf, "%d", v[i].id);
        print_bitmap_string(buf);
      }
    }
  }


}


