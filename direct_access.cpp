#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "moab/Core.hpp"

#include "timer.h"

int main(int argc, char** argv){

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

  // get direct access pointers
  moab::EntityHandle* conn_ptr;
  moab::EntityHandle first_tri = all_tris.front();
  int stride, n_tris;
  rval = MBI->connect_iterate(all_tris.begin(), all_tris.end(), conn_ptr, stride, n_tris);
  MB_CHK_SET_ERR(rval, "Failed to get connectivity pointer");

  double* x_vals;
  double* y_vals;
  double* z_vals;

  moab::Range all_verts;
  rval = MBI->get_entities_by_dimension(0, 0, all_verts);
  MB_CHK_SET_ERR(rval, "Failed to get all vertices in the model");
  moab::EntityHandle first_vert = all_verts.front();

  int n_vertices;
  rval = MBI->coords_iterate(all_verts.begin(), all_verts.end(), x_vals, y_vals, z_vals, n_vertices);
  MB_CHK_SET_ERR(rval, "Failed to get coordinate pointers");

  size_t n_samples = 10000000;

  // write binary file if indicated by "-w" flag
  bool write_vals = argc > 1 && std::string(argv[1]) == "-w";

  Timer t;
  std::cout << "Running " << n_samples << " samples." << std::endl;

  // vector holding a random set of indices to use as samples
  std::vector<size_t> random_indices(n_samples);
  srand(100); // ensure random number sequence is the same
  for(int i = 0; i < n_samples; i++) {
    random_indices.at(i) = rand() % all_tris.size();
  }

  std::vector<std::string> results(n_samples);

  t.start();
#pragma omp parallel for shared(MBI, write_vals, random_indices, results)
  for(size_t i = 0; i < n_samples; i++) {

    // get random index for this sample
    size_t idx = random_indices[i];

    moab::EntityHandle eh = first_tri + idx;

    size_t offset = stride * (eh - first_tri);

    moab::EntityHandle v0 = conn_ptr[offset];
    moab::EntityHandle v1 = conn_ptr[offset + 1];
    moab::EntityHandle v2 = conn_ptr[offset + 2];

    moab::EntityHandle v0idx = v0 - first_vert;
    moab::EntityHandle v1idx = v1 - first_vert;
    moab::EntityHandle v2idx = v2 - first_vert;

    double coords[3][3];
    coords[0][0] = x_vals[v0idx];
    coords[0][1] = y_vals[v0idx];
    coords[0][2] = z_vals[v0idx];

    coords[1][0] = x_vals[v1idx];
    coords[1][1] = y_vals[v1idx];
    coords[1][2] = z_vals[v1idx];

    coords[2][0] = x_vals[v2idx];
    coords[2][1] = y_vals[v2idx];
    coords[2][2] = z_vals[v2idx];

    // write values if requested
    if (write_vals) {
      std::stringstream ss;

      // the entity we're querying
      ss.write((const char*)&eh, sizeof(moab::EntityHandle));

      // the connectivity of that entity
      ss.write((const char*)&v0, sizeof(moab::EntityHandle));
      ss.write((const char*)&v1, sizeof(moab::EntityHandle));
      ss.write((const char*)&v2, sizeof(moab::EntityHandle));

      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          ss.write((const char*)&coords[i][j], sizeof(double));
        }
      }

      results[i] = ss.str();
    }
  }
  t.stop();

  if (write_vals) {
    // open file to write to in binary
    auto outfile = std::ofstream("direct_access_values.bin", std::ios::out | std::ios::binary);

    for(const auto& entry : results) { outfile << entry; }

    outfile.close();
  }

  std::cout << "Queries per second " << (double)n_samples / t.elapsed() << std::endl;

  return 0;
}
