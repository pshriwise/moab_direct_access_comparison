
all: interface direct_access

moab_dir:
ifndef MOAB_DIR
	$(error "Please set the MOAB_DIR variable to the root directory of a MOAB installation.")
endif

timer: timer.cpp
	g++ -c timer.cpp

interface: interface.cpp timer moab_dir
	g++ interface.cpp timer.cpp -I${MOAB_DIR}/include -L${MOAB_DIR}/lib -lMOAB -fopenmp -o interface

direct_access: direct_access.cpp timer moab_dir
	g++ direct_access.cpp timer.cpp -I${MOAB_DIR}/include -L${MOAB_DIR}/lib -lMOAB -fopenmp -o direct_access
