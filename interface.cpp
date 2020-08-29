
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

  // write binary file if indicated by "-w" flag
  bool write_vals = argc > 1 && std::string(argv[1]) == "-w";

  size_t n_samples = 10000000;

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

    // query the triangle connectivity
    moab::EntityHandle eh = all_tris[idx];
    std::vector<moab::EntityHandle> conn;
    rval = MBI->get_connectivity(&eh, 1, conn);

    double coords[9];
    double* coord_ptr = coords;
    rval = MBI->get_coords(&conn.front(), conn.size(), coord_ptr);

    // write values if requested
    if (write_vals) {
      std::stringstream ss;

      // the entity we're querying
      ss.write((const char*)&eh, sizeof(moab::EntityHandle));

      // the connectivity of that entity
      ss.write((const char*)&conn[0], sizeof(moab::EntityHandle));
      ss.write((const char*)&conn[1], sizeof(moab::EntityHandle));
      ss.write((const char*)&conn[2], sizeof(moab::EntityHandle));

      // the coordinates of that entity
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
          ss.write((const char*)&coords[3 * i + j], sizeof(double));
        }
      }

      results[i] = ss.str();
     }
  }
  t.stop();

  if (write_vals) {
    // open file to write to in binary
    std::ofstream outfile("interface_values.bin", std::ios::out | std::ios::binary);
    for(const auto& entry : results) { outfile << entry; }
    outfile.close();
  }

  std::cout << "Queries per second: " << (double)n_samples / t.elapsed() << std::endl;

  return 0;

}
