#include<iostream>
using namespace std;

//Global Variables
int DATA_MEMORY[512];
int INST_MEMORY[100][16];
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