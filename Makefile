
MOAB_DIR=${HOME}/opt/moab


interface: interface.cpp
	g++ interface.cpp -I${MOAB_DIR}/include -L${MOAB_DIR}/lib -lMOAB -fopenmp -o interface
