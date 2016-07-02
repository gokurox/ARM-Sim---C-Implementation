/* 

The project is developed as part of Computer Architecture class
Project Name: Functional Simulator for subset of ARM Processor

Developer's Name: Mandeep Singh & Gursimran Singh
Developer's Email id: mandeep14145@iiitd.ac.in  &  gursimran14041@iiitd.ac.in
Date: 14 April 2015

*/


/* myARMSim.h
   Purpose of this file: header file for myARMSim
*/

void reset_proc ();
int read_word (unsigned int);
void swi_exit ();
void write_word (unsigned int, unsigned int);
void load_program_memory (char *);
void write_data_memory ();
void write_back ();
void fetch ();
void binary (char [], char []);
int decimal (char [], int);
void code (int);
void decode (unsigned int);
void execute ();
void mem ();
void run_armsim ();
