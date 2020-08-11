

#include <iostream>
#include <memory>

#include "moab/Core.hpp"

#include "timer.h"

int main(){

  std::shared_ptr<moab::Interface> MBI = std::make_shared<moab::Core>();

  // load file
  std::cout << "Loading MOAB model...";
  moab::ErrorCode rval = MBI->load_file("dagmc.h5m");
  MB_CHK_SET_ERR(rval, "Failed to load the file");
  std::cout << "done." << std::endl;

  // get all the triangles in the model
  moab::Range all_tris;
  rval = MBI->get_entities_by_type(0, moab::MBTRI, all_tris);
  MB_CHK_SET_ERR(rval, "Failed to get all triangles");

  std::cout << "Model contains " << all_tris.size() << " triangles." << std::endl;

  moab::EntityHandle* conn_ptr;
  moab::EntityHandle first_element = all_tris.front();
  int stride, n_tris;
  rval = MBI->connect_iterate(all_tris.begin(), all_tris.end(), conn_ptr, stride, n_tris);
  MB_CHK_SET_ERR(rval, "Failed to get connectivity pointer");

  double* x_vals;
  double* y_vals;
  double* z_vals;

  moab::Range all_verts;
  rval = MBI->get_entities_by_dimension(0, 0, all_verts);
  MB_CHK_SET_ERR(rval, "Failed to get all vertices in the model");

  int n_vertices;
  rval = MBI->coords_iterate(all_verts.begin(), all_verts.end(), x_vals, y_vals, z_vals, n_vertices);
  MB_CHK_SET_ERR(rval, "Failed to get coordinate pointers");

  size_t n_samples = 100000;

  Timer t;
  std::cout << "Running " << n_samples << " samples." << std::endl;
  t.start();
  #pragma omp parallel for
  for(size_t i = 0; i < n_samples; i++) {

    // generate a random index
    size_t idx = rand() % all_tris.size();

    // query the triangle connectivity
    moab::Range conn;
    size_t v0 = conn_ptr[idx];
    size_t v1 = conn_ptr[idx + 1];
    size_t v2 = conn_ptr[idx + 2];

    double coords[3][3];
    coords[0][0] = x_vals[v0];
    coords[0][1] = y_vals[v0];
    coords[0][2] = z_vals[v0];

    coords[1][0] = x_vals[v1];
    coords[1][1] = y_vals[v1];
    coords[1][2] = z_vals[v1];

    coords[2][0] = x_vals[v2];
    coords[2][1] = y_vals[v2];
    coords[2][2] = z_vals[v2];

  }
  t.stop();

  std::cout << "Completed in " << t.elapsed() << " seconds." << std::endl;

  return 0;
}
