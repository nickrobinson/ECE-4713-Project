#include<iostream>
#include<fstream>
#include<string>
using namespace std;

#define DEBUG

//Global Variables
int DATA_MEMORY[100];
string INSTRUCTION_MEMORY[100];
int REG_ARRAY[8];
int PC = 0x00;
int ALU_Zero = 0;

int branchValue;
int jumpValue;
bool jumpControl;
bool PCSRC;				//Program counter source
int WB_write_data;
int WB_destination_register;
bool WB_control_bit;

#ifdef DEBUG
	ofstream logFile;
#endif

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
	SetIfGreaterThan,
	Jump,
	AddIm,
	SubIm
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
	//int aluOP[2];
	int aluOP;
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
	int branchValue;
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
    bool isZero;
	int destRegister;
	int regOut1;
	int regOut2;
	int signExtendedVal;
	int currentPC;
	int jumpValue;
	int branchValue;
	string functionCode;
	string opCode;
	ControlOut controlBits;
	int ALUResult;
	//32 for sign extended inst
};

struct MEM_WB_Buffer
{
	bool WriteBack;
	bool MemAccess;
	bool EX;					
	int address;
	int ALUResult;				//The result from the ALU in the EX stage. Either Calculation or Address.
	int currentPC;				//self explanatory
	string functionCode;		//self explanatory
	string opCode;				//self explanatory
	ControlOut controlBits;		//Carried over from previous stages
	int memReadData;			//The data stored by a memRead (lw) from the mem stage
	int destinationRegister;	//Which register has been chosen
	//32 for sign extended inst
};

//Global buffer declarations
IF_ID_Buffer FETCH_DECODE;
ID_EX_Buffer DECODE_EX;
EX_MEM_Buffer EX_MEM;
MEM_WB_Buffer MEM_WB;

//Function definitions
void fetch();
void decode();
void ControlUnit(string inputInstruction, ControlOut&);
void execute();
int funcALU(int, int, int, int);
void memory();
void writeBack();



int main()
{
	int cpuClock = 0;
	int numberOfInstructions = 0;
	int counter;
	
#ifdef DEBUG
	logFile.open("log.txt");
#endif
	
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
	

	
	//allocate mem for registers/memory/buffers
	
	//Initialize data memory
	DATA_MEMORY[0x0010] = 0x0101;
	DATA_MEMORY[0x0012] = 0x0110;
	DATA_MEMORY[0x0014] = 0x0011;
	DATA_MEMORY[0x0016] = 0x00F0;
	DATA_MEMORY[0x0018] = 0x00FF;
	
	//Initialize Registers
	REG_ARRAY[0] = 0x0000;	//Zero Register
	REG_ARRAY[1] = 0x0040;
	REG_ARRAY[2] = 0x1010;
	REG_ARRAY[3] = 0x000F;
	REG_ARRAY[4] = 0x00F0;
	REG_ARRAY[5] = 0x0000;
	REG_ARRAY[6] = 0x0010;
	REG_ARRAY[7] = 0x0005;

	//Initialize Instruction memory for testing purposes
	//INSTRUCTION_MEMORY[0] = "0111111110000000";//sgti $7,$6,0
	//INSTRUCTION_MEMORY[1] = "1110111001001100";//go to ENDLOOP bez $7, 7	
	
	ifstream inputInstructions;
	inputInstructions.open("instructions.txt");

	for(int i = 0; inputInstructions.good(); i++)
	{
		getline(inputInstructions, INSTRUCTION_MEMORY[i]);
		numberOfInstructions++;
	}
	inputInstructions.close();
	
	////Let's run the fetch/decode twice to test two instructions
	//fetch();
	//decode();
	//execute();
	//memory();
	//writeBack();

	//fetch();
	//decode();
	//execute();
	//memory();
	//writeBack();

	for(int i = 0; i < numberOfInstructions; i++)
	{
		switch(cpuClock)
		{
		case 0:
			fetch();
			break;
		case 1:
			decode();
			fetch();
			break;
		case 2:
			execute();
			decode();
			fetch();
			break;
		case 3:
			memory();
			execute();
			decode();
			fetch();
			break;
		default:
			writeBack();
			memory();
			execute();
			decode();
			fetch();
			
			logFile << "REG[0]: " << REG_ARRAY[0] << endl;
			logFile << "REG[1]: " << REG_ARRAY[1] << endl;
			logFile << "REG[2]: " << REG_ARRAY[2] << endl;
			logFile << "REG[3]: " << REG_ARRAY[3] << endl;
			logFile << "REG[4]: " << REG_ARRAY[4] << endl;
			logFile << "REG[5]: " << REG_ARRAY[5] << endl;
			logFile << "REG[6]: " << REG_ARRAY[6] << endl;
			logFile << "REG[7]: " << REG_ARRAY[7] << endl << endl;
			break;
		}
		cpuClock++;
	}
	
	//finishes the current instructions' cycles
	writeBack();
	memory();
	execute();
	decode();

	writeBack();
	memory();
	execute();

	writeBack();
	memory();

	writeBack();

	

	system("pause");
}


/******************************************************************************
		FETCH FUNCTION
******************************************************************************/
void fetch()
{
	//Get the current instruction from memory
	FETCH_DECODE.instruction = INSTRUCTION_MEMORY[PC];
	
	//Store incremented PC value in the FETCH_DECODE Buffer
	FETCH_DECODE.PC = PC + 1;
}

/******************************************************************************
		DECODE FUNCTION
******************************************************************************/
void decode()
{
	char * pEnd;
	
	//Strip the function code from the current instruction
	string funcCode = FETCH_DECODE.instruction.substr(13,3);
	
#ifdef DEBUG
	logFile << "CURRENT FUNCTION CODE: " << funcCode << endl;
#endif
	
	//Store current function code in DECODE_EX buffer
	DECODE_EX.functionCode = funcCode;
	
	//Store RD field
	DECODE_EX.registerRD = strtol(FETCH_DECODE.instruction.substr(10,3).c_str(), &pEnd, 2);
	
#ifdef DEBUG
	logFile << "CURRENT REGISTER RD: " << DECODE_EX.registerRD << endl;
#endif

	//Store RT field
	DECODE_EX.registerRT = strtol(FETCH_DECODE.instruction.substr(7,3).c_str(), &pEnd, 2);
	
#ifdef DEBUG
	logFile << "CURRENT REGISTER RT: " << DECODE_EX.registerRT << endl;
#endif

	//Store RS field
	DECODE_EX.registerRS = strtol(FETCH_DECODE.instruction.substr(5,3).c_str(), &pEnd, 2);
	
#ifdef DEBUG
	logFile << "CURRENT REGISTER RS: " << DECODE_EX.registerRS << endl;
#endif

	//Store Sign Extended Value
	DECODE_EX.signExtendedVal = strtol(FETCH_DECODE.instruction.substr(11,5).c_str(), &pEnd, 2);
	
#ifdef DEBUG
	logFile << "CURRENT SIGN EXTENDED VAL: " << DECODE_EX.signExtendedVal << endl;
#endif

	//Read and store register values
	DECODE_EX.regOut1 = REG_ARRAY[DECODE_EX.registerRS];
	DECODE_EX.regOut2 = REG_ARRAY[DECODE_EX.registerRT];
	
#ifdef DEBUG
	logFile << "REG OUT 1: " << DECODE_EX.regOut1 << endl;
	logFile << "REG OUT 2: " << DECODE_EX.regOut2 << endl;
#endif
	
	//Update PC value
	DECODE_EX.currentPC = FETCH_DECODE.PC;
	
#ifdef DEBUG
	logFile << "CURRENT PC: " << DECODE_EX.currentPC << endl;
#endif
	
	//Get the opcode and call the control unit
	DECODE_EX.opCode = FETCH_DECODE.instruction.substr(0,4);
	
#ifdef DEBUG
	logFile << "OP CODE: " << DECODE_EX.opCode << endl;
#endif
	
	//Call the control unit here
	ControlUnit(DECODE_EX.opCode, DECODE_EX.controlBits);
	
	//Make sure to comment this out later. Only using for testing purposes
	PC = PC + 1;

	//Add shift left 2 on 11-0i
	int tempJumpValue = strtol(FETCH_DECODE.instruction.substr(11,0).c_str(), &pEnd, 2);
	DECODE_EX.jumpValue = tempJumpValue << 2;

#ifdef DEBUG
	logFile << "JUMP VALUE: " << DECODE_EX.jumpValue << endl << endl;
#endif

	//Store the whole instruction value for the branch value
	int tempBranchValue = strtol(FETCH_DECODE.instruction.substr(15,0).c_str(), &pEnd, 2);
	DECODE_EX.branchValue = tempBranchValue;
}

/******************************************************************************
		CONTROL UNIT FUNCTION
******************************************************************************/
void ControlUnit(string inputOpCode, ControlOut& inputStruct)
{
	ControlOut tempControlOut = {false, false, false, false, false, false, false, false, 0};
	
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
			//??tempControlOut.aluOP = 0;
			break;
		case ARITHEMATIC:
			tempControlOut.aluOP = 2;
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
			tempControlOut.aluOP = 0;
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
			tempControlOut.aluOP = 0;
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
			tempControlOut.aluOP = 2;
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
			tempControlOut.aluOP = 2;
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
			tempControlOut.aluOP = 2;
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
/******************************************************************************
i dont know if this alu op is correct. also, attention grabbing
******************************************************************************/
			tempControlOut.aluOP = 0;
			tempControlOut.aluSRC = 0;
			tempControlOut.branch = 1;
			tempControlOut.jump = 0;
			tempControlOut.memRead = 0;
			tempControlOut.memToReg = 0;
			tempControlOut.memWrite = 0;
			tempControlOut.regDest = 0;
			tempControlOut.regWrite = 0;
			break;
		case SetIfGreaterThan:
			tempControlOut.aluOP = 2;
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
			tempControlOut.aluOP = 0;
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
			tempControlOut.aluOP = 0;
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
			tempControlOut.aluOP = 1;
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

/******************************************************************************
		EXECUTE FUNCTION
******************************************************************************/
void execute()
{
	char* pEnd;
	//Make sure to do the shift left 2
	
	//Copy the jump value
	EX_MEM.jumpValue = DECODE_EX.jumpValue;

	//Copy the function code
	EX_MEM.functionCode = DECODE_EX.functionCode;

	//Copy the Control bits
	EX_MEM.controlBits = DECODE_EX.controlBits;

	//Shift the sign extended value to send through the adder
	int tempSignExtend = DECODE_EX.signExtendedVal;
	tempSignExtend = tempSignExtend << 2;
	EX_MEM.signExtendedVal = tempSignExtend;

	//Calculate the branch value
	EX_MEM.branchValue = DECODE_EX.branchValue + EX_MEM.signExtendedVal;

	//Store destination register signal
	if(EX_MEM.controlBits.regDest == 0)
	{
		EX_MEM.destRegister = DECODE_EX.registerRT;
	}
	else if(EX_MEM.controlBits.regDest == 1)
	{
		EX_MEM.destRegister = DECODE_EX.registerRD;
	}

	/* Store ALU input number 2 which is either the
	// sign extended value from the fetch stage or the
	// second read register from the fetch stage based on
	// ALUSrc
	*/
	
	int tempALU_B;

	if(EX_MEM.controlBits.aluSRC == 0)
	{
		tempALU_B = DECODE_EX.regOut2;
	}
	else if(EX_MEM.controlBits.aluSRC == 1)
	{
		tempALU_B = DECODE_EX.signExtendedVal;
	}

	//Store the ALU_A variable in a temp variable then lets
	//call the funcALU to perform the ALU arithmetic
	int tempALU_A = DECODE_EX.regOut1;
	int tempFuncCode = strtol(EX_MEM.functionCode.c_str(), &pEnd, 2);

	EX_MEM.ALUResult = funcALU(EX_MEM.controlBits.aluOP, tempALU_A, tempALU_B, tempFuncCode); 
}

/******************************************************************************
		DECODE FUNCTION
			ALU Function accept 3-bit function code and inputs A & B
			then perform the correct operation.
******************************************************************************/
int funcALU (int ALU_OP, int ALU_A, int ALU_B, int ALU_FUNC) 
{
	int ALU_Result = 0;
	
	//If op indicates add
	if(ALU_OP == 0)
	{
		//Add A+B
		ALU_Result = ALU_A + ALU_B;
	}
	//if op indicates subtract
	else if(ALU_OP == 1)
	{
		//Subtract A-B
		ALU_Result = ALU_A - ALU_B;

		//If the result is zero, set the zero flag
		if (ALU_Result == 0)
		{
			ALU_Zero = 1;
		}
		else ALU_Zero = 0;
	}
	//Any other op
	else if(ALU_OP == 2)
	{
		switch(ALU_FUNC){

			case 0: //And
				ALU_Result = ALU_A & ALU_B;
				break;

			case 1: //or
				ALU_Result = ALU_A | ALU_B;
				break;

			case 2: //add
				ALU_Result = ALU_A + ALU_B;
				break;
			case 3: //xor
				ALU_Result = (ALU_A) * (~(ALU_B)) | ((~(ALU_A)) * (ALU_B));
			case 4: //Shift left logical
				//Must shift ALU_B to the left by three as the
				//shift amount we care about is stored in bits
				//[5-3]
				ALU_B = ALU_B >> 3;
				ALU_Result = ALU_A << ALU_B;
				break;
			case 5: //Shift right logical
				//Must shift ALU_B to the left by three as the
				//shift amount we care about is stored in bits
				//[5-3]
				ALU_B = ALU_B >> 3;
				ALU_Result = ALU_A >> ALU_B;
				break;
			case 6: //subtract
				ALU_Result = ALU_A - ALU_B;
				if (ALU_Result == 0)
					ALU_Zero = 1;
				else ALU_Zero = 0;
				break;
			case 7: //set greater than
				if (ALU_A > ALU_B){
					ALU_Result = 1;
				}
				else{
					ALU_Result = 0;
				}
				break;
		}
	}
	
	return ALU_Result;
}

/******************************************************************************
		MEMORY FUNCTION:
			takes in address, write data bit, mem write bit, mem read bit, 
			then reads data, and places it in the mem/wb buffer
*******************************************************************************/
void memory()
{
	//I am under the impression that this uses the 
	//aluresult to determine the address of a register 
	MEM_WB.address = EX_MEM.ALUResult;
	
	if(EX_MEM.controlBits.branch && EX_MEM.isZero)
	{
		//set PCSPC
	}

	//Save word instruction
	//Writes the value in the input register to the data memory
	//Data memory is specified by the EX_MEM_Buffer ALU result from sign extension and ALUOPs
	if(EX_MEM.controlBits.memWrite)
	{
		//I'm assuming that the ALUResult is basically the address
		//DATA_MEMORY[EX_MEM.ALUResult] = EX_MEM.dataToWrite;
		DATA_MEMORY[EX_MEM.ALUResult] = EX_MEM.regOut2;
	}
	
	//Load word instruction
	//Reads a value from memory at address specified by the result from ALU that represents sign extended address
	//Stores data in the input EX_MEM_Buffer
	else if(EX_MEM.controlBits.memRead)
	{
		//I'm assuming that the ALUResult is basically the address, again
		MEM_WB.memReadData = DATA_MEMORY[EX_MEM.ALUResult];
	}

	//Branch or R-type instruction
	else
	{
		//Im still not sure if this actually does anthing


	}
}

/******************************************************************************
		WRITE BACK FUNCTION
			Takes in the MEM_WB_Buffer and sets necessary parameters for proper
			register functionality including the write back data (either the 
			alu result or the data from registers), the write back destination
			register, and a global write back control bit
******************************************************************************/
void writeBack()
{
	if(MEM_WB.controlBits.memToReg)
	{
		//write the ALUresult to the registers
		WB_write_data = MEM_WB.ALUResult;
	}
	else
	{
		//write the loaded data to registers
		WB_write_data = MEM_WB.memReadData;
	}

	//Set the register destination
	WB_destination_register = MEM_WB.destinationRegister;

	//Set the global wb control bit to write to registers based on if 
	//the bit is set in the control bits of the mem/wb buffer
	WB_control_bit = MEM_WB.controlBits.regWrite;

#ifdef DEBUG
	logFile << "WB data:  " << WB_write_data << endl << endl;
#endif
}