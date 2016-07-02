/* 

The project is developed as part of Computer Architecture class
Project Name: Functional Simulator for subset of ARM Processor

Developer's Name:
Developer's Email id:
Date: 

*/


/* main.cpp 
   Purpose of this file: The file handles the input and output, and
   invokes the simulator
*/


#include "myARMSim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char** argv)
{
	char* prog_mem_file; 
	if (argc < 2)
	{
		printf ("Incorrect number of arguments. Please invoke the simulator \n\t./myARMSim <input mem file> \n");
		exit (1);
	}

	reset_proc ();

	load_program_memory (argv[1]);

	run_armsim ();


	return 1;
}
