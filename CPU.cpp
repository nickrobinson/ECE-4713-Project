#include<iostream>
#include<string>
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
};

//Structure for Immediate type instructions
struct I_Instruction
{
	int opCode;
	int RS;
	int RT;
	int immediateValue;		//
};

//Structure for Jump type instructions
struct J_Instruction
{
	int opCode;
	int jumpAddress;		// The address that should be jumped to
};


struct ControlOut
{
	bool branch;
	bool regDest;
	bool regWrite;
	bool memRead;
	bool memToReg;
	bool memWrite;
	bool aluSRC;
	int aluOP[2];
};


//Function for analyzing opcode
//returns struct with status of each control line
ControlOut ControlUnit(string inputInstruction);

int main()
{
	int cpuClock = 0;
	
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
	

	//setup
	//allocate mem for registers/memory/buffers

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


	system("pause");
	return 0;

	
	//init global clock
	
	//main while loop
		//maintain clock
		//execute each portion at least once
		


	//cleanup?
}


//Function for analyzing opcode
//returns struct with status of each control line
ControlOut ControlUnit(string inputInstruction)
{
	ControlOut tempControlOut = {false, false, false, false, false, false, false, {0,0}};
	string opCode;
	opCode.assign(inputInstruction.begin(), inputInstruction.begin()+4);

	//init opint
	int OPint = -1;
	//create array with all possible string-based opcodes
	string possibleOps[12] = {"0000", "0001", "0010", "0011", "0100", "0101", 
							  "0110", "0111", "1000", "1100", "1011", "1110"};
	
	//find which one matches
	//i will correspond to ennumerated list of functions
	for(int i = 0; i < 13; i++)
	{
		if(opCode == possibleOps[i])
		{
			OPint = i;
		}
	}

	//deal with each case and set control bits
	switch(OPint)
	{
		case NOP:
			break;
		case ARITHEMATIC:
			break;
		case LOAD:
			break;
		case STORE:
			break;
		case LOGICAL:
			break;
		case ShiftLeft:
			break;
		case ShiftRight:
			break;
		case BranchIfEqual:
			break;
		case BranchIfLessThan:
			break;
		case Jump:
			break;
		case AddIm:
			break;
		case SubIm:
			break;
		default:
			//no opcode was found
			cout << "Error finding opcode" << endl;
			break;
	}

	return tempControlOut;
}