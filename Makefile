all: release

COMPILER=mpicxx
ARGS=-Wall -O3 -g 
STANDARD=-std=c++11
OUTFILE=bin/n-body_sim

release: obj/Body.o obj/File_IO.o  obj/Calculations.o obj/N-Body_Sim.o
	$(COMPILER) $(ARGS) $(STANDARD) -o $(OUTFILE) obj/*.o 

obj/N-Body_Sim.o: src/N-Body_Sim.cpp
	$(COMPILER) $(ARGS) $(STANDARD) -c -o obj/N-Body_Sim.o src/N-Body_Sim.cpp

obj/Body.o: src/Body.cpp
	$(COMPILER) $(ARGS) $(STANDARD) -c -o obj/Body.o src/Body.cpp

obj/File_IO.o: src/File_IO.cpp
	$(COMPILER) $(ARGS) $(STANDARD) -c -o obj/File_IO.o src/File_IO.cpp

obj/Calculations.o: src/Calculations.cpp
	$(COMPILER) $(ARGS) $(STANDARD) -c -o obj/Calculations.o src/Calculations.cpp

clean:
	rm bin/* obj/*
