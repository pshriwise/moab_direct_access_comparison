
MOAB_DIR=${HOME}/opt/moab

all: interface direct_access

timer: timer.cpp
	g++ -c timer.cpp

interface: interface.cpp timer
	g++ interface.cpp timer.cpp -I${MOAB_DIR}/include -L${MOAB_DIR}/lib -lMOAB -fopenmp -o interface

direct_access: direct_access.cpp timer
	g++ direct_access.cpp timer.cpp -I${MOAB_DIR}/include -L${MOAB_DIR}/lib -lMOAB -fopenmp -o direct_access
