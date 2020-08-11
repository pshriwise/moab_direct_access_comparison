
MOAB_DIR=${HOME}/opt/moab


timer: timer.cpp
	g++ -c timer.cpp

interface: interface.cpp timer
	g++ interface.cpp timer.cpp -I${MOAB_DIR}/include -L${MOAB_DIR}/lib -lMOAB -fopenmp -o interface
