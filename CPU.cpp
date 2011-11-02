#include<iostream>
using namespace std;

void fetch();

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

//Structure for the IF/ID buffer
struct IF_ID_Buffer
{
	int instruction[16];
}

struct ID_EX_Buffer
{
	bool WriteBack;
	bool MemAccess;
	bool EX;
	int registerRD;
	int registerRT;
	int registerRS;
	int currentPC;
	//32 for sign extended inst

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
	
	//main while loop
	while(true)
	{
		//maintain clock
		//execute each portion at least once
		fetch();
	}

	//cleanup?

	return 0;
}

void fetch()
{
	//read in instruction
	//get opcode
	//switch for i/j/r type instructions
	

}