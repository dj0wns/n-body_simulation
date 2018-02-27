*N-Body_Sim*

Currently this readme is a placeholder for more cool stuff to come - but here is a gif of the current progress!

**Demonstration of N-Body Sim**
This demonstration consists of 4000 bodies with randomized initial positions and weights. There is a time step of 400000 seconds for 2000 iterations on 4 mpi threads with one frame per 10 iterations. Particle collision is currently not enabled because it does not work in multithreaded workloads yet. This simulation took 20 minutes to solve the 32 Billion gravity equations on my Intel Celeron N3160 Quadcore @ 1.6GHz.

![Demonstration of N-Body Sim](https://github.com/dj0wns/n-body_simulation/raw/master/visualization/animated.gif "Demonstration of N-Body sim with 4000 bodies and a timestep of 400000 seconds over 2000 iterations, 1 frame per 10 iterations")

**Usage**
  Usage:
    bin/n-body_sim -i [Input File] -o [Output File] -t [time step] -n [number of iterations] -O [number of iterations per filewrite] [flags]
	    Valid Flags:
		    -c : Enable Collision Detection
