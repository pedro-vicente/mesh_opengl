#include "sdm_mesh_file.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////////////
//SDM_MeshFile::SDM_MeshFile
/////////////////////////////////////////////////////////////////////////////////////////////////////

SDM_MeshFile::SDM_MeshFile(const std::string& name) 
  : file_name(name), number_vec_elements(0), vec_elements(NULL)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//SDM_MeshFile::~SDM_MeshFile
/////////////////////////////////////////////////////////////////////////////////////////////////////

SDM_MeshFile::~SDM_MeshFile()
{
  this->clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//SDM_MeshFile::clear
/////////////////////////////////////////////////////////////////////////////////////////////////////

void SDM_MeshFile::clear(void) 
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
//SDM_MeshFile::read_sdm_file
/////////////////////////////////////////////////////////////////////////////////////////////////////

void SDM_MeshFile::read_sdm_file()
{

  FILE *file;
  char str[256];
  int size_nodes;
  int size_elements;
  int number_nodes;

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
      elm.render = true;

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