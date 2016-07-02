/************************************************************************************************/
/***********************************  DEVELOPER INFORMATION   ***********************************/
//
//			Developer's Name (1) :  Mandeep Singh
//			Email ID	     :  mandeep14145@iiitd.ac.in
//			Developer's Name (2) :  Gursimran Singh
//			Email ID 	     :  gursimran14041@iiitd.ac.in
//
/************************************************************************************************/

/*********************************     HEADER FILES     *****************************************/

#include "myARMSim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/************************************************************************************************/
/***************************************   GLOBAL VARIABLES     *********************************/

static unsigned int R[16];	// Register File
int N,C,V,Z;			// Flags
static unsigned char MEM[4000];	// Memory

/************************    INTERMEDIATE DATAPATH AND CONTROLS   *******************************/

static unsigned int instruction;
static unsigned int operand1;
static unsigned int operand2;

/***************************************         OTHERS           *******************************/

char opchar[20], rot [10];
int tmp;
int count=0;
int op1;
int op2;
int op3;
int rot_amt;
int opcode;
int imm;
int format;
int s;
int cond;
int sign;

/************************************************************************************************/
/***************************************      PROTOTYPES        *********************************/

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

/************************************************************************************************/

void reset_proc ()
{
	memset (R, 0, sizeof(R));
	memset (MEM, 0, sizeof(MEM));
}

int read_word (unsigned int address)
{
	int *data;
	data = (int *)(MEM + address);
	return *data;
}

void swi_exit ()
{
	write_data_memory ();
	printf ("\nEXITING NOW .....................\n");
	printf ("\n*************************************************************************\n");
	exit (0);
}

void write_word (unsigned int address, unsigned int data)
{
	int *data_p;
	data_p = (int *)(MEM + address);
	*data_p = data;
}

void load_program_memory (char *file_name)
{
	FILE *fp;
	unsigned int address, instruction;
	fp = fopen (file_name, "r");
	if (fp == NULL)
	{
		printf ("Error Opening MEM File !!\n");
		exit(1);
	}
	while (fscanf (fp, "%X %X", &address, &instruction) != EOF)
	{
		write_word (address, instruction);
	}
	fclose (fp);
}

void write_data_memory ()
{
	FILE *fp;
	unsigned int i;
	fp = fopen ("Memory.mem", "w");
	if (fp == NULL)
	{
		printf ("Error Opening Data_Out.MEM !!\n");
		return;
	}
	for(i = 0; i < 4000; i += 4)
	{
		fprintf (fp, "0x%X 0x%X\n", i, read_word (i));
	}
	fclose (fp);
}

void write_back ()
{
	if ((format != 2) && strcmp(opchar, "STR") && strcmp(opchar, "CMP"))
	{
		if (format == 3)
		{
			if (op2 == 106)
			{
				R[0] = tmp;
				printf ("WRITEBACK : Writing %d to R0\n", tmp);
			}
		}
		R[op3] = tmp;
		printf ("WRITEBACK : Writing %d to R%d\n", tmp, op3);
	}
	else
		printf ("WRITEBACK : No Writeback for this Operation\n");
	count += 4;
}

void fetch ()
{
	int *data;
	data= (int *)(MEM + count);
	instruction = *data;
	printf ("FETCH : Fetching INSTRUCTION --> 0x%X, from ADDRESS --> 0x%X\n", instruction, count);
	R[15] = count;
	//count += 4;
}

void binary (char hexaDecimal[], char bin[])
{
	int i = 0;
	strcpy (bin, "\0");
	while (hexaDecimal[i])
	{
		switch (hexaDecimal[i])
		{
			case '0': strcat (bin, "0000"); 
				  break;
			case '1': strcat (bin, "0001"); 
				  break;
			case '2': strcat (bin, "0010"); 
			  	  break;
			case '3': strcat (bin, "0011"); 
			 	  break;
			case '4': strcat (bin, "0100"); 
				  break;
			case '5': strcat (bin, "0101"); 
				  break;
			case '6': strcat (bin, "0110"); 
				  break;
			case '7': strcat (bin, "0111"); 
				  break;
			case '8': strcat (bin, "1000"); 
				  break;
			case '9': strcat (bin, "1001"); 
				  break;
			case 'a':
			case 'A': strcat (bin, "1010"); 
				  break;
			case 'b':
			case 'B': strcat (bin, "1011"); 
				  break;
			case 'c':
			case 'C': strcat (bin, "1100"); 
				  break;
			case 'd':
			case 'D': strcat (bin, "1101"); 
				  break;
			case 'e':
			case 'E': strcat (bin, "1110"); 
				  break;
			case 'f':
			case 'F': strcat (bin, "1111"); 
				  break;
			default: break;
		}
		i++;
	}
}

int decimal (char temp[], int len)
{
	int sum = 0, i, j, powe, t;
	len--;
	for(i = 0; i <= len; i++)
	{
		t = temp[len -i] - 48;
		powe = (int)(pow((float)(2), (float)(i)));
		sum += t * powe;
	} 
	return sum;
}

void code (int opcode)
{
	switch(opcode)
	{
		case 0: strcpy (opchar, "AND");
			break;
		case 1: strcpy (opchar, "EOR"); 
			break;
		case 2: strcpy (opchar, "SUB"); 
			break;
		case 4: strcpy (opchar, "ADD"); 
			break;
		case 5: strcpy (opchar, "ADC"); 
			break;
		case 10: strcpy (opchar, "CMP"); 
			 break;
		case 12: strcpy (opchar, "ORR"); 
			 break;
		case 13: strcpy (opchar, "MOV"); 
			 break;
		case 15: strcpy (opchar, "MNV"); 
			 break;
		default: break;
	}
}

void decode (unsigned int instruction)
{
	char bin[33], str[33], temp[32], shift[5], temp22[10];
	int i, j = 0, dsh;
	sprintf (str, "%X", instruction);

	binary (str, bin);

	if (str[1] == '1' || str[1] == '2' || str[1] == '3' || str[1] == '0')
	{
		format = 0;
		if (bin[6] == '1')
		{
			imm = 1;
			j = 0;
			for (i = 24; i <= 31; i++)
				temp[j++] = bin[i];
			temp[j] = '\0';
			op2 = decimal (temp, j);

			j = 0;
		}
		else
		{
			imm = 0;
			j = 0;
			for (i = 28; i <= 31; i++)
				temp[j++] = bin[i];
			temp[j] = '\0';
			op2 = decimal (temp, j);
			j = 0;
			if (bin[27] == '0')
			{
				strcpy (shift, "");
				sprintf (temp22, "%c", bin[25]);
				strcat (shift, temp22);
				sprintf (temp22, "%c", bin[26]);
				strcat (shift, temp22);
				dsh = decimal (shift, strlen(shift));
				switch (dsh)
				{
					case 0: strcpy (rot, "LSL");
						  break;
					case 1: strcpy (rot, "LSR");
						  break;
					case 2: strcpy (rot, "ASR");
						  break;
					case 3: strcpy (rot, "ROR");
						  break;
					default: strcpy (rot, "");
						 break;
				}
				strcpy (shift, "");
				sprintf (temp22, "%c", bin[20]);
				strcat (shift, temp22);
				sprintf (temp22, "%c", bin[21]);
				strcat (shift, temp22);
				sprintf (temp22, "%c", bin[22]);
				strcat (shift, temp22);
				sprintf (temp22, "%c", bin[23]);
				strcat (shift, temp22);
				sprintf (temp22, "%c", bin[24]);
				strcat (shift, temp22);
				rot_amt = decimal (shift, strlen(shift));
			}
		}
		j = 0;
		for (i = 16; i <= 19; i++)
		{
			temp[j] = bin[i];
			j++;
		}
		temp[j] = '\0';
		op3 = decimal (temp, j);
		j = 0;
		for (i = 12; i <= 15; i++)
		{
			temp[j] = bin[i];
			j++;
		}
		temp[j] = '\0';
		op1 = decimal (temp, j);
		j = 0;
		for (i = 7; i <= 10; i++)
		{
			temp[j] = bin[i];
			j++;
		}
		temp[j] = '\0';
		opcode = decimal (temp, j);

		code (opcode);
		j = 0;
		if (!strcmp (opchar, "MOV") || !strcmp (opchar,"MNV"))
		{
			if (imm == 1)
			{
				printf ("DECODE : Operation is %s, Operand2 is %d,  Immediate Operand is %d\n", opchar, op2, op3);
			}
			else
			{
				printf ("DECODE : Operation is %s, Operand2 is %d,  Destination is R%d\n", opchar, op2, op3);
			}
				
		}
		else
		{
			if(imm == 0)
			{
				printf ("DECODE : Operation is %s, Operand1 is R%d, Operand2 is R%d,  Destination is %d\n", opchar, op1, op2, op3);
			}
			else
			{
				printf ("DECODE : Operation is %s, Operand1 is R%d, Operand2 is R%d,  Immediate Operand is %d\n", opchar, op1, op2, op3);
			}
		}
	}
	else if (str[1] == '7' || str[1] == '4' || str[1] == '6' || str[1] == '5')
	{
		j = 0;
		format = 1;
		for (i = 6; i <= 11; i++)
		{
			temp[j] = bin[i];
			j++;
		}
		temp[j] = '\0';
		opcode = decimal (temp, j);
		if (opcode == 25)
			strcpy (opchar, "LDR");
		else if (opcode == 24)
			strcpy (opchar, "STR");
		j = 0;
		for (i = 12; i <= 15; i++)
		{
			temp[j] = bin[i];
			j++;
		}
		temp[j] = '\0';
		op1 = decimal (temp, j);
		j = 0;
		for (i = 16; i <= 19; i++)
		{
			temp[j] = bin[i];
			j++;
		}
		temp[j] = '\0';
		op3 = decimal (temp, j);
		j = 0;
		for (i = 20; i <= 31; i++)
		{
			temp[j++] = bin[i];
		}
		temp[j] = '\0';
		op2 = decimal (temp, j);
		j = 0;
		printf ("DECODE : Operation is %s, Operand1 is R%d, Immediate Operand is %d, Destination is R%d \n", opchar, op1, op2, op3);
	}
	else if (str[1] == 'A' || str[1] == 'a')
	{
		sign = 0;
		
		j = 0;
		
		for (i = 0; i <= 3; i++)
		{
			temp[j] = bin[i];
			j++;
		}
		
		temp[j] = '\0';
		cond = decimal (temp, j);
		format = 2;
		opcode = 2;
		strcpy (opchar, "BRANCH");
		j = 0;
		if (str[3] == 'F' || str[3] == 'f')
		{
			sign = -1;
			for (j = 0; j < strlen(bin); j++)
			{
				if (bin[j] == '1')
					bin[j] = '0';
				else
					bin[j] = '1';
			}
		}
		else 
			sign = 1;
		j = 0;
		for (i = 8; i <= 31; i++)
		{
			temp[j] = bin[i];
			j++;
		}
		temp[j] = '\0';
		op2 = decimal (temp, j);
		op2 += 1;		
		j=0;
		printf ("DECODE: Operation is %s and offset is 0x%X --> dec(%d) \n", opchar, op2, op2);
		
	}
	else if (str[1] == 'F' || str[1] == 'f')
	{
		format = 3;
		j = 0;
		for(i = 8; i <= 31; i++)
		{
			temp[j] = bin[i];
			j++;
		}
		temp[j] = '\0';
		op2 = decimal(temp,j);
		j = 0;
		printf ("SWI Instruction encountered...\n");
	}
}

void execute()
{
	int x, y, z;
	printf ("EXECUTE: ");
	if (format == 0)
	{
		if (!strcmp (opchar, "ADD") || !strcmp (opchar, "ADC"))
		{
			if (imm == 1)
			{
				tmp = R[op1] + op2;
				printf ("ADD %d and %d\n", R[op1], op2);
			}
			if(imm == 0)
			{
				tmp = R[op1] + R[op2];
				printf ("ADD %d and %d\n", R[op1], R[op2]);
			}
		}
		if (!strcmp (opchar, "SUB"))
		{
			if(imm == 1)
			{
				tmp = R[op1] - op2;
				printf ("SUB %d and %d\n", R[op1], op2);
			}
			if (imm == 0)
			{
				tmp = R[op1] - R[op2];
				printf ("SUB %d and %d\n", R[op1], R[op2]);
			}
		}
		if(!strcmp (opchar, "EOR"))
		{
			if(imm == 1)
			{
				tmp = R[op1] ^ op2;
				printf ("EOR %d and %d\n", R[op1], op2);
			}
			if(imm == 0)
			{
				tmp = R[op1] ^ R[op2];
				printf ("ADD %d and %d\n", R[op1], R[op2]);
			}
		}
		if(!strcmp (opchar, "AND"))
		{
			if(imm == 1)
			{
				tmp = R[op1] & op2;
				printf ("AND %d and %d\n", R[op1], op2);
			}
			if(imm == 0)
			{
				tmp = R[op1] & R[op2];
				printf ("AND %d and %d\n", R[op1], R[op2]);
			}
		}
		if(!strcmp (opchar, "ORR"))
		{
			if(imm == 1)
			{
				tmp = R[op1] | op2;
				printf ("ORR %d and %d\n", R[op1], op2);
			}
			if(imm == 0)
			{
				tmp = R[op1] | R[op2];
				printf ("ORR %d and %d\n", R[op1], R[op2]);
			}
		}
		if(!strcmp (opchar, "MOV"))
		{
			if(imm == 1)
			{
				tmp = op2;
				printf ("MOV %d to R%d\n", tmp, op3);
			}
			if(imm == 0)
			{
				if (!strcmp (rot, "LSL"))
					R[op2] = R[op2] << rot_amt;
				else if (!strcmp (rot, "LSR"))
					R[op2] = R[op2] >> rot_amt;
				else if (!strcmp (rot, "ASR"))
					op2 = op2;
				else if (!strcmp (rot, "ROR"))
					op2 = op2;
					
				tmp = R[op2];
				printf ("MOV %d to R%d\n", tmp, op3);
			}
		}
		if(!strcmp (opchar, "MVN"))
		{
			if(imm == 1)
			{
				tmp = ~op2;
				printf ("MVN %d to R%d\n", tmp, op3);
			}
			if(imm == 0)
			{
				tmp = ~R[op2];
				printf ("MVN %d to R%d\n", tmp, op3);
			}
		}
		if(!strcmp (opchar, "CMP"))
		{
			if(imm == 1)
			{
				if(R[op1] > op2)
				{
					N = 0;
					Z = 0;
				}
				if(R[op1] < op2)
				{
					N = 1;
					Z = 0;
				}
				if(R[op1] == op2)
				{
					N = 0;
					Z = 1;
				}
				printf ("Compare %d and %d", R[op1], op2);
			}
			if(imm == 0)
			{
				if(R[op1] > R[op2])
				{
					N = 0;
					Z = 0;
				}
				if(R[op1] < R[op2])
				{
					N = 1;
					Z = 0;
				}
				if(R[op1] == R[op2])
				{
					N = 0;
					Z = 1;
				}
				printf ("Compare %d and %d\n", R[op1], R[op2]);
			}
		}
	}
	if (format == 1)
	{
		tmp = R[op1] + op2;
		if (!strcmp (opchar, "LDR"))
			printf ("Load from %d\n", tmp);
		if (!strcmp (opchar, "STR"))
			printf ("Store into %d\n", tmp);
	}
	if (format == 2)
	{
		op2 *= 4;
		if (sign == -1)
		{
			op2 -= 8;
			op2 *= -1;
		}
		else
			op2 += 8;
		if (cond == 0)
		{
			if (Z == 1)
			{
				count += op2;
				printf ("Branching at %d\n", count);
				printf ("\n*************************************************************************\n");
				run_armsim ();
			}
		}
		else if (cond == 1)
		{
			if (Z == 0)
			{
				count += op2;
				printf ("Branching at %d\n", count);
				printf ("\n*************************************************************************\n");
				run_armsim ();
			}
		}
		else if (cond == 11)
		{
			if (N == 1)
			{
				count += op2;
				printf ("Branching at %d\n", count);
				printf ("\n*************************************************************************\n");
				run_armsim ();
			}
		}
		else if (cond == 13)
		{
			if (Z == 1 || N == 1)
			{
				count += op2;
				printf ("Branching at %d\n", count);
				printf ("\n*************************************************************************\n");
				run_armsim ();
			}
		}
		else if (cond == 12)
		{
			if (N == 0 && Z == 0)
			{
				count += op2;
				printf ("Branching at %d\n", count);
				printf ("\n*************************************************************************\n");
				run_armsim ();
			}
		}
		else if (cond == 10)
		{
			if ((N == 0 && Z == 0) || (Z == 1))
			{
				count += op2;
				printf ("Branching at %d\n", count);
				printf ("\n*************************************************************************\n");
				run_armsim ();
			}
		}
		
		printf ("No Branch\n");
	}
	if (format == 3)
	{
		printf ("SWI :: ");
		if (op2 == 17)
		{
			printf ("EXIT\n");
			swi_exit ();
		}
		if (op2 == 107)
		{
			printf ("PRINT INTEGER\n");
			printf ("#####   %d   #####\n", R[1]);
		}
		if (op2 == 0)
		{
			printf ("PRINT SPACE\n");
			printf ("#####       #####\n");
		}
		if (op2 == 108)
		{
			printf ("READ INTEGER\nENTER INTEGER :: ");
			scanf ("%d", &tmp);
		}
	}
}

void mem()
{
	if (!strcmp (opchar, "STR"))
	{
		write_word (tmp, R[op3]);
		printf ("MEMORY: Stored R%d into the memory at %d\n", op3, tmp);
	}
	else if (!strcmp (opchar, "LDR"))
	{
		tmp = read_word (tmp);
		printf ("MEMORY: Loaded %d\n", tmp);
	}
	else
		printf ("MEMORY: No Memory Operation\n");
}

void run_armsim()
{
	while (1)
	{
		printf ("*************************************************************************\n\n");
		fetch (MEM);
		printf ("\n");
		decode (instruction);
		printf ("\n");
		execute ();
		printf ("\n");		
		mem ();
		printf ("\n");
		write_back ();
		printf ("\n");
		printf ("*************************************************************************\n");
	}
}
