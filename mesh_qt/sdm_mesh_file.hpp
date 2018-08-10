#ifndef SDM_MESH_H
#define SDM_MESH_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

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
  bool render; //render this element
};


class SDM_MeshFile 
{

public:
  SDM_MeshFile(const std::string& name);
  ~SDM_MeshFile();
  void read_sdm_file();

  std::string file_name;
  int number_vec_elements;
  std::vector<ELEMENT> *vec_elements;
  std::vector<NODE> vec_nodes;

private:
  void clear(void);

};

#endif