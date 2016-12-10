all: release

COMPILER=g++
ARGS=-O3 
STANDARD=-std=c++11
OUTFILE=bin/n-body_sim

release: Octree Body File_IO N-Body_Sim
	$(COMPILER) $(ARGS) $(STANDARD) -o $(OUTFILE) obj/*.o 
N-Body_Sim: src/N-Body_Sim.cpp
	$(COMPILER) $(ARGS) $(STANDARD) -c -o obj/N-Body_Sim.o src/N-Body_Sim.cpp
Octree: src/Octree.cpp
	$(COMPILER) $(ARGS) $(STANDARD) -c -o obj/Octree.o src/Octree.cpp
Body: src/Body.cpp
	$(COMPILER) $(ARGS) $(STANDARD) -c -o obj/Body.o src/Body.cpp
File_IO: src/File_IO.cpp
	$(COMPILER) $(ARGS) $(STANDARD) -c -o obj/File_IO.o src/File_IO.cpp

clean:
	rm bin/* obj/*
