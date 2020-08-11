

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
    moab::EntityHandle eh = all_tris[idx];
    rval = MBI->get_connectivity(&eh, 1, conn);

    double coords[9];
    double* coord_ptr = coords;
    rval = MBI->get_coords(conn, coord_ptr);
  }
  t.stop();

  std::cout << "Completed in " << t.elapsed() << " seconds." << std::endl;

  return 0;

}
