#include<iostream>
#include<string>
using namespace std;

#define DEBUG

//Function definitions
void fetch();
void decode();
void execute();
int funcALU();

//Global Variables
int DATA_MEMORY[100];
string INSTRUCTION_MEMORY[100];
int REG_ARRAY[8];
int PC = 0x00;
int ALU_Zero = 0;

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
	bool jump;
	int aluOP[2];
};

//Structure for the IF/ID buffer
struct IF_ID_Buffer
{
	string instruction;
	int PC;
};

struct ID_EX_Buffer
{
	bool WriteBack;
	bool MemAccess;
	bool EX;
	int registerRD;
	int registerRT;
	int registerRS;
	int regOut1;
	int regOut2;
	int signExtendedVal;
	int currentPC;
	int jumpValue;
	string functionCode;
	string opCode;
	ControlOut controlBits;
	//32 for sign extended inst

};

struct EX_MEM_Buffer
{
	bool WriteBack;
	bool MemAccess;
	bool EX;
	int registerRD;
	int registerRT;
	int registerRS;
	int regOut1;
	int regOut2;
	int signExtendedVal;
	int currentPC;
	int jumpValue;
	string functionCode;
	string opCode;
	ControlOut controlBits;
	//32 for sign extended inst
};

struct MEM_WB_Buffer
{
	bool WriteBack;
	bool MemAccess;
	bool EX;
	int registerRD;
	int registerRT;
	int registerRS;
	int regOut1;
	int regOut2;
	int signExtendedVal;
	int currentPC;
	int jumpValue;
	string functionCode;
	string opCode;
	ControlOut controlBits;
	//32 for sign extended inst
};

IF_ID_Buffer FETCH_DECODE;
ID_EX_Buffer DECODE_EX;
EX_MEM_Buffer EX_MEM;


//Function for analyzing opcode
//returns struct with status of each control line
void ControlUnit(string inputInstruction, ControlOut&);

int main()
{
	int cpuClock = 0;
	
	int counter;
	
	//Wipe Instruction Memory
	for (counter = 0; counter < 100; counter++)
	{
		INSTRUCTION_MEMORY[counter] = "";
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

	//Initialize Instruction memory for testing purposes
	INSTRUCTION_MEMORY[0] = "0111111110000000";//sgti $7,$6,0
	INSTRUCTION_MEMORY[1] = "1110111001001100";//go to ENDLOOP bez $7, 7	

	//Let's run the fetch/decode twice to test two instructions
	fetch();
	decode();
	fetch();
	decode();
	
	//init global clock

	//setup
		//init global clock
		//allocate mem for registers/memory/buffers

	
	//main while loop
	/*while(true)
	{
		//maintain clock
		//execute each portion at least once
		fetch();
	}*/

	//cleanup?

}

//ALU Function accept 3-bit function code and inputs A & B
//then perform the correct operation.
int funcALU (int ALU_OP, int ALU_A, int ALU_B) {
	int ALU_Result = 0;
	
	switch (ALU_OP) {
		case 0: //addition
			ALU_Result = ALU_A + ALU_B;
			break;

		case 1: //and
			ALU_Result = ALU_A & ALU_B;
			break;

		case 2: //or
			ALU_Result = ALU_A | ALU_B;
			break;
			
		case 3: //Nor
			ALU_Result = ~(ALU_A | ALU_B);
			break;
		
		case 4: //set greater than
			if (ALU_A > ALU_B) 
				ALU_Result = 1;
			else
				ALU_Result = 0;
			break;
			
		case 5: //set on less than
			if (ALU_A < ALU_B){
				ALU_Result = 1;
				ALU_Zero = 1;
			}
			else{
				ALU_Result = 0;
				ALU_Zero = 0;
			}
			break;
			
		case 6: //subtract
			ALU_Result = ALU_A - ALU_B;
			if (ALU_Result == 0)
				ALU_Zero = 1;
			else ALU_Zero = 0;
			break;
	}
	
	return ALU_Result;
}


//Function for analyzing opcode
//returns struct with status of each control line
void ControlUnit(string inputOpCode, ControlOut& inputStruct)
{
	ControlOut tempControlOut = {false, false, false, false, false, false, false, false, {0,0}};
	
	int OPint = -1;
	//create array with all possible string-based opcodes
	string possibleOps[12] = {"0000", "0001", "0010", "0011", "0100", "0101", 
							  "0110", "0111", "1000", "1100", "1011", "1110"};
	
	//find which one matches
	//i will correspond to ennumerated list of functions
	for(int i = 0; i < 13; i++)
	{
		if(inputOpCode.compare(possibleOps[i]) == 0)
		{
			OPint = i;
			break;
		}
	}

	//deal with each case and set control bits
	switch(OPint)
	{
		case NOP:
			break;
		case ARITHEMATIC:
			tempControlOut.aluOP[0] = 0;
			tempControlOut.aluOP[1] = 1;
			tempControlOut.aluSRC = 0;
			tempControlOut.branch = 0;
			tempControlOut.jump = 0;
			tempControlOut.memRead = 0;
			tempControlOut.memToReg = 0;
			tempControlOut.memWrite = 0;
			tempControlOut.regDest = 1;
			tempControlOut.regWrite = 1;
			break;
		case LOAD:
			tempControlOut.aluOP[0] = 0;
			tempControlOut.aluOP[1] = 0;
			tempControlOut.aluSRC = 1;
			tempControlOut.branch = 0;
			tempControlOut.jump = 0;
			tempControlOut.memRead = 1;
			tempControlOut.memToReg = 1;
			tempControlOut.memWrite = 0;
			tempControlOut.regDest = 0;
			tempControlOut.regWrite = 1;
			break;
		case STORE:
			tempControlOut.aluOP[0] = 0;
			tempControlOut.aluOP[1] = 0;
			tempControlOut.aluSRC = 1;
			tempControlOut.branch = 0;
			tempControlOut.jump = 0;
			tempControlOut.memRead = 0;
			tempControlOut.memToReg = 0;	//Don't care
			tempControlOut.memWrite = 1;
			tempControlOut.regDest = 0;		//Don't care
			tempControlOut.regWrite = 0;
			break;
		case LOGICAL:
			tempControlOut.aluOP[0] = 0;
			tempControlOut.aluOP[1] = 1;
			tempControlOut.aluSRC = 0;
			tempControlOut.branch = 0;
			tempControlOut.jump = 0;
			tempControlOut.memRead = 0;
			tempControlOut.memToReg = 0;
			tempControlOut.memWrite = 0;
			tempControlOut.regDest = 1;
			tempControlOut.regWrite = 1;
			break;
		case ShiftLeft:
			tempControlOut.aluOP[0] = 1;
			tempControlOut.aluOP[1] = 1;
			tempControlOut.aluSRC = 1;
			tempControlOut.branch = 0;
			tempControlOut.jump = 0;
			tempControlOut.memRead = 0;
			tempControlOut.memToReg = 0;
			tempControlOut.memWrite = 0;
			tempControlOut.regDest = 0;
			tempControlOut.regWrite = 1;
			break;
		case ShiftRight:
			tempControlOut.aluOP[0] = 1;
			tempControlOut.aluOP[1] = 1;
			tempControlOut.aluSRC = 1;
			tempControlOut.branch = 0;
			tempControlOut.jump = 0;
			tempControlOut.memRead = 0;
			tempControlOut.memToReg = 0;
			tempControlOut.memWrite = 0;
			tempControlOut.regDest = 0;
			tempControlOut.regWrite = 1;
			break;
		case BranchIfEqual:
			tempControlOut.aluOP[0] = 1;
			tempControlOut.aluOP[1] = 0;
			tempControlOut.aluSRC = 0;
			tempControlOut.branch = 1;
			tempControlOut.jump = 0;
			tempControlOut.memRead = 0;
			tempControlOut.memToReg = 0;
			tempControlOut.memWrite = 0;
			tempControlOut.regDest = 0;
			tempControlOut.regWrite = 0;
			break;
		case SetIfLessThan:
			tempControlOut.aluOP[0] = 1;
			tempControlOut.aluOP[1] = 0;
			tempControlOut.aluSRC = 0;
			tempControlOut.branch = 0;
			tempControlOut.jump = 0;
			tempControlOut.memRead = 0;
			tempControlOut.memToReg = 0;
			tempControlOut.memWrite = 0;
			tempControlOut.regDest = 1;
			tempControlOut.regWrite = 1;
			break;
		case Jump:
			tempControlOut.aluOP[0] = 0; 
			tempControlOut.aluOP[1] = 0;
			tempControlOut.aluSRC = 0;
			tempControlOut.branch = 0;
			tempControlOut.jump = 1;
			tempControlOut.memRead = 0;
			tempControlOut.memToReg = 0;
			tempControlOut.memWrite = 0;
			tempControlOut.regDest = 0;
			tempControlOut.regWrite = 0;
			break;
		case AddIm:
			tempControlOut.aluOP[0] = 0;
			tempControlOut.aluOP[1] = 0;
			tempControlOut.aluSRC = 1;
			tempControlOut.branch = 0;
			tempControlOut.jump = 0;
			tempControlOut.memRead = 0;
			tempControlOut.memToReg = 0;
			tempControlOut.memWrite = 0;
			tempControlOut.regDest = 1;
			tempControlOut.regWrite = 1;
			
			break;
		case SubIm:
			tempControlOut.aluOP[0] = 1;
			tempControlOut.aluOP[1] = 0;
			tempControlOut.aluSRC = 1;
			tempControlOut.branch = 0;
			tempControlOut.jump = 0;
			tempControlOut.memRead = 0;
			tempControlOut.memToReg = 0;
			tempControlOut.memWrite = 0;
			tempControlOut.regDest = 1;
			tempControlOut.regWrite = 1;
			break;
	}
}

void fetch()
{
	//read in instruction
	//get opcode
	//switch for i/j/r type instructions
	
	//Get the current instruction from memory
	FETCH_DECODE.instruction = INSTRUCTION_MEMORY[PC];
	
	//Store incremented PC value in the FETCH_DECODE Buffer
	FETCH_DECODE.PC = PC + 1;
}

void decode()
{
	bool WriteBack;
	bool MemAccess;
	bool EX;
	int registerRD;
	int registerRT;
	int registerRS;
	int signExtendedVal;
	int currentPC;
	int opCode;
	string functionCode;
	ControlOut controlBits;
	char * pEnd;
	
	//Strip the function code from the current instruction
	string funcCode = FETCH_DECODE.instruction.substr(13,3);
	
#ifdef DEBUG
	cout << "CURRENT FUNCTION CODE: " << funcCode << endl;
#endif
	
	//Store current function code in DECODE_EX buffer
	DECODE_EX.functionCode = funcCode;
	
	//Store RD field
	DECODE_EX.registerRD = strtol(FETCH_DECODE.instruction.substr(10,3).c_str(), &pEnd, 2);
	
#ifdef DEBUG
	cout << "CURRENT REGISTER RD: " << DECODE_EX.registerRD << endl;
#endif

	//Store RT field
	DECODE_EX.registerRT = strtol(FETCH_DECODE.instruction.substr(7,3).c_str(), &pEnd, 2);
	
#ifdef DEBUG
	cout << "CURRENT REGISTER RT: " << DECODE_EX.registerRT << endl;
#endif

	//Store RS field
	DECODE_EX.registerRS = strtol(FETCH_DECODE.instruction.substr(5,3).c_str(), &pEnd, 2);
	
#ifdef DEBUG
	cout << "CURRENT REGISTER RS: " << DECODE_EX.registerRS << endl;
#endif

	//Store Sign Extended Value
	DECODE_EX.signExtendedVal = strtol(FETCH_DECODE.instruction.substr(11,5).c_str(), &pEnd, 2);
	
#ifdef DEBUG
	cout << "CURRENT SIGN EXTENDED VAL: " << DECODE_EX.signExtendedVal << endl;
#endif

	//Read and store register values
	DECODE_EX.regOut1 = REG_ARRAY[DECODE_EX.registerRS];
	DECODE_EX.regOut2 = REG_ARRAY[DECODE_EX.registerRT];
	
#ifdef DEBUG
	cout << "REG OUT 1: " << DECODE_EX.regOut1 << endl;
	cout << "REG OUT 2: " << DECODE_EX.regOut2 << endl;
#endif
	
	//Update PC value
	DECODE_EX.currentPC = FETCH_DECODE.PC;
	
#ifdef DEBUG
	cout << "CURRENT PC: " << DECODE_EX.currentPC << endl;
#endif
	
	//Get the opcode and call the control unit
	DECODE_EX.opCode = FETCH_DECODE.instruction.substr(0,4);
	
#ifdef DEBUG
	cout << "OP CODE: " << DECODE_EX.opCode << endl;
#endif
	
	//Call the control unit here
	//ContolUnit(DECODE_EX.opCode, DECODE_EX.controlBits);
	
	//Make sure to comment this out later. Only using for testing purposes
	PC = PC + 1;

	//Add shift left 2 on 11-0i
	int tempJumpValue = strtol(FETCH_DECODE.instruction.substr(11,0).c_str(), &pEnd, 2);
	DECODE_EX.jumpValue = tempJumpValue << 2;

#ifdef DEBUG
	cout << "JUMP VALUE: " << DECODE_EX.jumpValue << endl << endl;
#endif

}

void execute()
{
	//Make sure to do the shift left 2
}

//takes in address, write data bit, mem write bit, mem read bit, then reads data, and places it in the mem/wb buffer
void memoryFunction(EX_MEM_Buffer inputEX_MEM_Buffer, MEM_WB_Buffer& inputMEMWBBuffer)
{
	//psuedo code

	//Save word instruction
	//Writes the value in the input register to the data memory
	//Data memory is specified by the EX_MEM_Buffer ALU result from sign extension and ALUOPs
	if(inputCtrlBits.memWrite)
	{
		DATA_MEMORY[EX_MEM_Buffer.] = inputMEMWBBuffer.;
	}
	
	//Load word instruction
	//Reads a value from memory at address specified by the result from ALU that represents sign extended address
	//Stores data in the input EX_MEM_Buffer
	else if(inputCtrlBits.memRead)
	{
		inputMEMWBBuffer. = DATA_MEMORY[EX_MEM_Buffer.signExtendedVal];
	}

	//Branch or R-type instruction
	else
	{
		//Doesnt do anything?

	}
}