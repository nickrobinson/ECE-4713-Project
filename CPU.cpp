#include<iostream>
using namespace std;

//Global Variables
int DATA_MEMORY[100];
int INSTRUCTION_MEMORY[100][16];
int REG_ARRAY[8];

typedef enum OPCODE
{ 
	NOP,
	ARITHEMATIC,
	LOAD,
	STORE,
	LOGICAL,
	ShiftLeft,
	ShiftRight,
	BranchIfEqual,
	BranchIfLessThan,
	SetIfLessThan,
	Jump,
	AddIm,
	SubIm
};

//Structure for R type instructions
struct R_Instruction
{
	int opCode;
	int RS;				//Registers
	int RT;				
	int RD;				
	int functionCode;	//The function code used for logical or arith opcode
}

//Structure for Immediate type instructions
struct I_Instruction
{
	int opCode;
	int RS;
	int RT;
	int immediate;		//
}

//Structure for Jump type instructions
struct J_Instruction
{
	int opCode;
	int Address;		// The address that should be jumped to
}


int main()
{
	int cpuClock = 0;
	int jumpAddress = 0;
	int rsLocation = 0;
	int rtLocation = 0;
	int rdLocation = 0;
	int opCode = 0;
	int functCode = 0;
	
	int counter;
	
	//Wipe Instruction Memory
	for (counter = 0; counter < 100; counter++)
	{
		for(int i=0; i<16; i++)
		{
			INSTRUCTION_MEMORY[counter][i] = 0;
		}
	}
	
	//Wipe instruction memory
	for (counter = 0; counter < 100; counter++)
	{
		DATA_MEMORY[counter] = 0;
	}
	
	//Wipe Registers
	for (counter = 0; counter < 8; counter++)
	{
		REG_ARRAY[counter] = 0;
	}
	
	//Initialize data memory
	DATA_MEMORY[0x0010] = 0x0101;
	DATA_MEMORY[0x0012] = 0x0110;
	DATA_MEMORY[0x0014] = 0x0011;
	DATA_MEMORY[0x0016] = 0x00F0;
	DATA_MEMORY[0x0018] = 0x00FF;
	
	//Initialize Registers
	REG_ARRAY[0] = 0x0040;
	REG_ARRAY[1] = 0x1010;
	REG_ARRAY[2] = 0x000F;
	REG_ARRAY[3] = 0x00F0;
	REG_ARRAY[4] = 0x0000;
	REG_ARRAY[5] = 0x0010;
	REG_ARRAY[6] = 0x0005;
	REG_ARRAY[7] = 0x0000;	//Zero Register



	//setup
		//init global clock
		//allocate mem for registers/memory/buffers
		//Initialize Memory
		//Initialize Regs

	//main while loop
		//maintain clock
		//execute each portion at least once
		


	//cleanup?

	return 0;
}