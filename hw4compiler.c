// John Cunningham
// NID: jo242562 PID: 4012336

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// constants from previous homeworks
const int MAX_STACK_HEIGHT = 40;
const int MAX_LEXI_LEVELS = 3;
const int MAX_CODE_LENGTH = 200;

// used to determine what values are printed to screen/
int printTokens = 0;
int printAssembly = 0;
int printVM = 0;

// keeps track of how many lines of code there is
int cx = 0;

// array that contains code
int code[200][4];

// string that is tokenized to read into code
char *tokenStream;

// variable that contains a token from tokenStream
char *tokenTarget1;

// utility variable for testing parser
char *lastToken;
int symbolTableLocation = 1;
int STACCLVL = 0;
int addr = 4;
int RegTracker = 0;
FILE *inputfilepointer;
FILE *outputfilepointer;
int lookAhead = 0;
int skipProcedures = 0;

// These are the enumerations provided in hw2 document
typedef enum { 
nulsym = 1, 
identsym,	 //2
numbersym, 	//3
plussym, 	//4
minussym, 	//5
multsym, 	//6
slashsym,	//7
oddsym, 	//8
eqsym, 		//9
neqsym, 	//10
lessym, 	//11
leqsym,		//12
gtrsym, 	//13
geqsym, 	//14
lparentsym, //15
rparentsym, //16
commasym, 	//17
semicolonsym, //18
periodsym, 	//19
becomessym, //20
beginsym, 	//21
endsym, 	//22
ifsym, 		//23
thensym, 	//24
whilesym, 	//25
dosym, 		//26
callsym, 	//27
constsym, 	//28
varsym, 	//29
procsym, 	//30
writesym,	//31
readsym , 	//32
elsesym 	//33
} token_type;

// Linked list structure
struct LexEntryType
{
	char *lexeme; // contains a lexeme entry
	int tokenType; // contains the token type
	struct LexEntryType *nextSym;

}; 

// Linked list header that keeps track of first and last element
struct lexHead
{
	struct LexEntryType *firstEntry;
	struct LexEntryType *lastEntry;
	int size;
};

struct symbol
{
	int kind;		// const = 1; var = 2, proc = 3
	char name[12];	// name up to 11 chars
	int val;		// number (ASCII value)
	int level;		// L level
	int addr;		// M address
	int mark;
};

struct symbol symboltable[150];


// returns the location inside symbol table
int inSymbolTable(char *target)
{
	for(int i = symbolTableLocation; i != 0; i--)
	{
		if (strcmp(target, symboltable[i].name) == 0 && symboltable[i].mark != 1)
		{
			return i;
		}

	}

	printf("Error 10, undeclared symbol \"%s\" is detected\n", target);
	exit(EXIT_FAILURE);

	return 0;
}

// this function will test to see if a string matches a reserved word or symbol
int LexToken(char *test)
{
	//printf("\ntest is: |%s|\n", test);
	if (strcmp(test,"const") == 0)
		return constsym;

	else if (strcmp(test,"var") == 0)
		return varsym;

	else if (strcmp(test,"begin") == 0)
		return beginsym;

	else if (strcmp(test,"end") == 0)
		return endsym;

	else if (strcmp(test,"if") == 0)
		return ifsym;

	else if (strcmp(test,"then") == 0)
		return thensym;

	else if (strcmp(test,"while") == 0)
		return whilesym;

	else if (strcmp(test,"do") == 0)
		return dosym;

	else if (strcmp(test,"read") == 0)
		return readsym;

	else if (strcmp(test,"write") == 0)
		return writesym;

	else if (strcmp(test,"odd") == 0)
		return oddsym;

	else if (strcmp(test,"procedure") == 0)
		return procsym;

	else if (strcmp(test,"call") == 0)
		return callsym;

	else if (strcmp(test,"else") == 0)
		return elsesym;

	else if (strcmp(test,"+") == 0)
		return plussym;

	else if (strcmp(test,"-") == 0)
		return minussym;

	else if (strcmp(test,"*") == 0)
		return multsym;

	else if (strcmp(test,"/") == 0)
		return slashsym;

	else if (strcmp(test,"(") == 0)
		return lparentsym;

	else if (strcmp(test,")") == 0)
		return rparentsym;

	else if (strcmp(test,":=") == 0)
		return becomessym;

	else if (strcmp(test,">") == 0)
		return gtrsym;

	else if (strcmp(test,"<") == 0)
		return lessym;

	else if (strcmp(test,">=") == 0)
		return geqsym;

	else if (strcmp(test,"=") == 0)
		return eqsym;

	else if (strcmp(test,"<=") == 0)
		return leqsym;

	else if (strcmp(test,".") == 0)
		return periodsym;

	else if (strcmp(test,",") == 0)
		return commasym;

	else if (strcmp(test,";") == 0)
		return semicolonsym;

	else if (strcmp(test,"<>") == 0)
		return neqsym;

	return -1;
}

void LexTokenPrint(int test)
{
	//printf("\ntest is: |%s|\n", test);
	if(constsym == test)
	{
		printf("constsym ");
		return;
	}

	if(procsym == test)
	{
		printf("procsym ");
		return;
	}

	if(callsym == test)
	{
		printf("callsym ");
		return;
	}

	if(elsesym == test)
	{
		printf("elsesym ");
		return;
	}


	else if(varsym == test)
	{
		printf("varsym ");
		return;
	}

	else if(beginsym == test)
	{
		printf("beginsym ");
		return;
	}

	else if(endsym == test)
	{
		printf("endsym ");
		return;
	}
	
	else if(ifsym == test)
	{
		printf("ifsym ");
		return;
	}
	
	else if(thensym == test)
	{
		printf("thensym ");
		return;
	}

	else if(whilesym == test)
	{
		printf("whilesym ");
		return;
	}
	
	else if(dosym == test)
	{
		printf("dosym ");
		return;
	}
	
	else if(readsym == test)
	{
		printf("readsym ");
		return;
	}
	
	else if(writesym == test)
	{
		printf("writesym ");
		return;
	}
	
	else if(oddsym == test)
	{
		printf("oddsym ");
		return;
	}
	
	else if(plussym == test)
	{
		printf("plussym ");
		return;
	}
	
	else if(minussym == test)
	{
		printf("minussym ");
		return;
	}
	
	else if(multsym == test)
	{
		printf("multsym ");
		return;
	}
	
	else if(slashsym == test)
	{
		printf("slashsym ");
		return;
	}
	
	else if(lparentsym == test)
	{
		printf("lparentsym ");
		return;
	}
	
	else if(rparentsym == test)
	{
		printf("rparentsym ");
		return;
	}

	else if(becomessym == test)
	{
		printf("becomessym ");
		return;
	}
	
	else if(gtrsym == test)
	{
		printf("gtrsym ");
		return;
	}
	
	else if(lessym == test)
	{
		printf("lessym ");
		return;
	}
	
	else if(geqsym == test)
	{
		printf("geqsym ");
		return;
	}
	
	else if(eqsym == test)
	{
		printf("eqsym ");
		return;
	}
	
	else if(leqsym == test)
	{
		printf("leqsym ");
		return;
	}
	
	else if(periodsym == test)
	{
		printf("periodsym ");
		return;
	}
	
	else if(commasym == test)
	{
		printf("commasym ");
		return;
	}
	
	else if(semicolonsym == test)
	{
		printf("semicolonsym ");
		return;
	}
	
	else if(neqsym == test)
	{
		printf("neqsym ");
		return;
	}
	
	else if(identsym == test)
	{
		printf("identsym ");
		return;
	}

	return;
}

void isTooBig(int a)
{
	if (a > 99999 || a < -99999)
	{
		printf("Error, value overflow\n");
		exit(EXIT_FAILURE);
	}
}

void pregister()
{
	
	// This is the register array, used for simulating a physical register
	int registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	// This is the instruction register, this mimicks a physical instruction register
	int ir[4] = {0, 0, 0, 0}; 

	// This is a buffer char array that is used to read the input file.
	char line[30];

	// This is an integer array that simulates the stack.
	int stack[MAX_STACK_HEIGHT];

	// These are all 
	int i = 0, o = 0, halt = 0, pc = 0, sp = 0, bp = 1, lexiHelper = 0, numofentries = 0;
	int OP, R, L, M, DigitCount, DigitTarget, CodeLength;
	char *toCodeS2I;

	// Test variable to prevent infinte loop, removable.
	int emergency = 0;

	// Variable used in sto to ask for values from user in sio.
	int addressTracker = 4;

	// Utility function to print out stack
	int entryLength[MAX_STACK_HEIGHT];

	// Open file pointers
	// printf("%s\n", FileName);

	CodeLength = cx;

	/*printf("There are %d number of lines.\n", i);
	printf("Numbers of line 1 is\n");
	for(o = 0; o < CodeLength; o++)
	{
		for(i = 0; i < 4; i++)
		{
			printf("%d, ", code[o][i]);
		}

		printf("\n");
	}
		

	printf("Line  OP    R  L  M\n");*/

	// printf("0     jmp   5  3  2\n"); //test line

	// Read file, put into an array
	if (printAssembly == 1)
	printf("Line  OP    R  L  M\n");
	//fprintf(outputfilepointer,"Line  OP    R  L  M\n");

	DigitCount = 0;
	// Here is where the code array is read and displayed as well as saved to output terminal.
	for (i = 0; i < CodeLength; i++)
	{
		OP = code[i][0];
		R = code[i][1];
		L = code[i][2];
		M =  code[i][3];

		// DigitTarget and DigitCount are used to properly display
		// the output of the function
		for (DigitTarget = i; DigitTarget > 0;)
		{
			DigitTarget = DigitTarget / 10;
			DigitCount++;
		}

		if (DigitCount <= 1)
		{
			if (printAssembly == 1)
			printf("%d     ", i);
			//fprintf(outputfilepointer, "%d     ", i + 1);
		}

		else if (DigitCount == 2)
		{
			if (printAssembly == 1)
			printf("%d    ", i);
			//fprintf(outputfilepointer,"%d    ", i + 1);
		}

		else if (printAssembly == 3 + 1)
		{
			if (printVM == 1)
			printf("%d   ", i);
			//fprintf(outputfilepointer,"%d   ", i + 1);
		}


		
		DigitCount = 0;
		
		switch (OP)
		{
			case 1:
				if (printAssembly == 1)
				printf("LIT   ");
				//fprintf(outputfilepointer,"LIT   ");
			
			break;
			
			case 2:
				if (printAssembly == 1)
				printf("RTN   ");
				//fprintf(outputfilepointer,"RTN   ");
				
			break;

			case 3:
				if (printAssembly == 1)
				printf("LOD   ");
				//fprintf(outputfilepointer,"LOD   ");
			
			break;

			case 4:
				if (printAssembly == 1)
				printf("STO   ");
				//fprintf(outputfilepointer,"STO   ");
			break;

			case 5:
				if (printAssembly == 1)
				printf("CAL   ");
				//fprintf(outputfilepointer,"CAL   ");
			break;

			case 6:
				if (printAssembly == 1)
				printf("INC   ");
				//fprintf(outputfilepointer,"INC   ");
			break;

			case 7:
				if (printAssembly == 1)
				printf("JMP   ");
				//fprintf(outputfilepointer,"JMP   ");
			break;

			case 8:
				if (printAssembly == 1)
				printf("JPC   ");
				//fprintf(outputfilepointer,"JPC   ");
			break;

			case 9:
				if (printAssembly == 1)
				printf("SIO   ");
				//fprintf(outputfilepointer,"SIO   ");
			break;

			case 10:
				if (printAssembly == 1)
				printf("SIO   ");
				//fprintf(outputfilepointer,"SIO   ");
			break;

			case 11:
				if (printAssembly == 1)
				printf("SIO   ");
				//fprintf(outputfilepointer,"SIO   ");
			break;

			case 12:
				if (printAssembly == 1)
				printf("NEG   ");
				//fprintf(outputfilepointer,"NEG   ");
			break;

			case 13:
				if (printAssembly == 1)
				printf("ADD   ");
				//fprintf(outputfilepointer,"ADD   ");
			break;

			case 14:
				if (printAssembly == 1)
				printf("SUB   ");
				//fprintf(outputfilepointer,"SUB   ");
			break;

			case 15:
				if (printAssembly == 1)
				printf("MUL   ");
				//fprintf(outputfilepointer,"MUL   ");
			break;

			case 16:
				if (printAssembly == 1)
				printf("DIV   ");
				//fprintf(outputfilepointer,"DIV   ");
			break;

			case 17:
				if (printAssembly == 1)
				printf("ODD   ");
				//fprintf(outputfilepointer,"ODD   ");
			break;

			case 18:
				if (printAssembly == 1)
				printf("MOD   ");
				//fprintf(outputfilepointer,"MOD   ");
			break;

			case 19:
				if (printAssembly == 1)
				printf("EQL   ");
				//fprintf(outputfilepointer,"EQL   ");
			break;

			case 20:
				if (printAssembly == 1)
				printf("NEQ   ");
				//fprintf(outputfilepointer,"NEQ   ");
			break;

			case 21:
				if (printAssembly == 1)
				printf("LSS   ");
				//fprintf(outputfilepointer,"LSS   ");
			break;

			case 22:
				if (printAssembly == 1)
				printf("LEQ   ");
				//fprintf(outputfilepointer,"LEQ   ");
			break;

			case 23:
				if (printAssembly == 1)
				printf("GTR   ");
				//fprintf(outputfilepointer,"GTR   ");
			break;

			case 24:
				if (printAssembly == 1)
				printf("GEQ   ");
				//fprintf(outputfilepointer,"GEQ   ");
			break;

		}

		if (printAssembly == 1)
		printf("%d  ", R);
		//fprintf(outputfilepointer,"%d  ", R);

		if (printAssembly == 1)
		printf("%d  ", L);
		//fprintf(outputfilepointer,"%d  ", L);

		if (printAssembly == 1)
		printf("%d\n", M);
		//fprintf(outputfilepointer,"%d\n", M);

	}

	for(int p = 0; p < cx; p++)
	{
		fprintf(outputfilepointer, "%d ", code[p][0]);
		fprintf(outputfilepointer, "%d ", code[p][1]);
		fprintf(outputfilepointer, "%d ", code[p][2]);
		fprintf(outputfilepointer, "%d\n", code[p][3]);
	}
	fprintf(outputfilepointer, "\n");


	// Initialize the stack.
	for (o = 0; o < MAX_STACK_HEIGHT; o++)
	{
		stack[o] = 0;
	}

	CodeLength = 1;
	
	// Here is where we begin to print the rest of the output.
	if (printVM == 1)
	printf("\n                 pc	 bp	 sp  	    registers\n");
	fprintf(outputfilepointer,"                  pc	 bp	 sp  	    registers\n");

	if (printVM == 1)
	printf("Initial values   0 	 1	 0	    0  0  0  0  0  0  0  0\n");
	fprintf(outputfilepointer,"Initial values    0	 1	 0	    0  0  0  0  0  0  0  0\n");
	
	if (printVM == 1)
	printf("Stack: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 \n\n");
	fprintf(outputfilepointer,"Stack: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 \n\n");
	
	if (printVM == 1)
	printf("                 pc	 bp	 sp  	    registers\n");
	fprintf(outputfilepointer,"                 pc	 bp	 sp  	    registers\n");
	
	// PC BP SP 
	// Main instruction
	while (halt == 0)
	{

		// fetch
		OP = code[pc][0];
		R = code[pc][1];  // i
		L = code[pc][2];  // j
		M =  code[pc][3]; // k
		pc++;

		//printf("\nPC = %d", pc);
		//printf("OP = %d", OP);
		//printf("R = %d", R);
		//printf("L = %d", L);
		//printf("M = %d\n", M);

		// execute
		for (DigitTarget = pc - 1; DigitTarget > 0;)
		{
			DigitTarget = DigitTarget / 10;
			DigitCount++;
		}

		if (DigitCount <= 1)
		{
			if (printVM == 1)
			printf(" %d ", pc - 1);
			fprintf(outputfilepointer," %d ", pc);
		}

		else if (DigitCount == 2)
		{
			if (printVM == 1)
			printf("%d ", pc - 1);
			fprintf(outputfilepointer,"%d ", pc);
		}

		else if (DigitCount == 3)
		{
			if (printVM == 1)
			printf("%d ", pc - 1);
			fprintf(outputfilepointer,"%d ", pc);
		}
		
		DigitCount = 0;

		// The big switch: does what is described per the directions.
		switch (OP)
		{
			case 1:
				registers[R] = M;
				if (printVM == 1)
				printf("LIT");
				fprintf(outputfilepointer,"LIT");
			break;
			
			case 2:
				sp = bp - 1;
				bp = stack[sp + 3];
				pc = stack[sp + 4];


				entryLength[numofentries--] = 0;
				if (printVM == 1)
				printf("RTN");
				fprintf(outputfilepointer,"RTN");
			break;

			case 3:
				lexiHelper = bp;
				for(o = L; o > 0; o--)
				{
					lexiHelper = stack[lexiHelper + 1];
				}

				registers[R] = stack[lexiHelper + M];
				
				addressTracker = lexiHelper + M;

				if (printVM == 1)
				printf("LOD");
				fprintf(outputfilepointer,"LOD");
			break;

			case 4:
				lexiHelper = bp;
				for(o = L; o > 0; o--)
				{
					lexiHelper = stack[lexiHelper + 1];
				}

				stack[lexiHelper + M] = registers[R];
				


				if (printVM == 1)
				printf("STO");
				fprintf(outputfilepointer,"STO");
			break;

			case 5:
				stack[sp + 1] = 0;

				lexiHelper = bp;
				for(o = L; o > 0; o--)
				{
					lexiHelper = stack[lexiHelper + 1];
				}
				
				stack[sp + 2] = lexiHelper;
				stack[sp + 3] = bp;
				stack[sp + 4] = pc;
				bp = sp + 1;
				pc = M;
				entryLength[numofentries++] = M;

				if (printVM == 1)
				printf("CAL");
				fprintf(outputfilepointer,"CAL");
			break;

			case 6:

				sp = sp + M;
				entryLength[numofentries++] = M;

				if (printVM == 1)
				printf("INC");
				fprintf(outputfilepointer,"INC");
			break;

			case 7:

				pc = M;
				printf("PC IS %d\n", pc);
				if (printVM == 1)
				printf("JMP");
				fprintf(outputfilepointer,"JMP");
			break;

			case 8:

				if(registers[R] == 0)
					pc = M;

				if (printVM == 1)
				printf("JPC");
				fprintf(outputfilepointer,"JPC");
			break;

			case 9:
				printf("\nThe result is %d\n", registers[R]);
				
				if (printVM == 1)
				printf("SIO");
				fprintf(outputfilepointer,"SIO");
			break;

			case 10:
				printf("\nEnter a number for variable %s:\n", symboltable[addressTracker - 4].name);
				scanf("%5d", &registers[R]);

				if (printVM == 1)
				printf("SIO");
				fprintf(outputfilepointer,"SIO");
			break;

			case 11:
				halt = 1;

				if (printVM == 1)
				printf("SIO");
				fprintf(outputfilepointer,"SIO");
			break;

			case 12:
				registers[R] = -1 * registers[R];

				if (printVM == 1)
				printf("NEG");
				fprintf(outputfilepointer,"NEG");
			break;

			case 13:
				registers[R] = registers[L] + registers[M];
				isTooBig(registers[R]);
				
				if (printVM == 1)
				printf("ADD");
				fprintf(outputfilepointer,"ADD");
			break;

			case 14:
				registers[R] = registers[L] - registers[M];
				isTooBig(registers[R]);
				
				if (printVM == 1)
				printf("SUB");
				fprintf(outputfilepointer,"SUB");
			break;

			case 15:
				registers[R] = registers[L] * registers[M];
				isTooBig(registers[R]);
				
				if (printVM == 1)
				printf("MUL");
				fprintf(outputfilepointer,"MUL");
			break;

			case 16:
				registers[R] = registers[L] / registers[M];
				isTooBig(registers[R]);
				
				if (printVM == 1)
				printf("DIV");
				fprintf(outputfilepointer,"DIV");
			break;

			case 17:
				registers[R] = registers[R] % 2;
				
				if (printVM == 1)
				printf("ODD");
				fprintf(outputfilepointer,"ODD");
			break;

			case 18:
				registers[R] = registers[L] % registers[M];
				
				if (printVM == 1)
				printf("MOD");
				fprintf(outputfilepointer,"MOD");
			break;

			case 19:
				registers[R] = (registers[L] == registers[M]);
				
				if (printVM == 1)
				printf("EQL");
				fprintf(outputfilepointer,"EQL");
			break;

			case 20:
				registers[R] = registers[L] != registers[M];
				
				if (printVM == 1)
				printf("NEQ");
				fprintf(outputfilepointer,"NEQ");
			break;

			case 21:
				registers[R] = registers[L] < registers[M];
				
				if (printVM == 1)
				printf("LSS");
				fprintf(outputfilepointer,"LSS");
			break;

			case 22:
				registers[R] = registers[L] <= registers[M];
				
				if (printVM == 1)
				printf("LEQ");
				fprintf(outputfilepointer,"LEQ");
			break;

			case 23:
				registers[R] = registers[L] > registers[M];
				
				if (printVM == 1)
				printf("GTR");
				fprintf(outputfilepointer,"GTR");
			break;

			case 24:
				registers[R] = registers[L] >= registers[M];
				
				if (printVM == 1)
				printf("GEQ");
				fprintf(outputfilepointer,"GEQ");
			break;
		}


		// the following functions are for printing to the output file and printing to the terminal
		for(DigitTarget = R; DigitTarget > 0;)
		{
			DigitTarget = DigitTarget / 10;
			DigitCount++;
		}

		if (DigitCount <= 1)
		{
				
			if (printVM == 1)
			printf(" %d", R);
			fprintf(outputfilepointer," %d", R);
		}

		else if (DigitCount == 2)
		{
			if (printVM == 1)
			printf(" %d", R);
			fprintf(outputfilepointer," %d", R);
		}

		else if (DigitCount == 3)
		{
			if (printVM == 1)
			printf(" %d", R);
			fprintf(outputfilepointer," %d", R);
		}
		
		DigitCount = 0;


		for(DigitTarget = L; DigitTarget > 0;)
		{
			DigitTarget = DigitTarget / 10;
			DigitCount++;
		}

		if (DigitCount <= 1)
		{
			if (printVM == 1)
			printf(" %d ", L);
			fprintf(outputfilepointer," %d ", L);
		}

		else if (DigitCount == 2)
		{
			if (printVM == 1)
			printf(" %d ", L);
			fprintf(outputfilepointer," %d ", L);
		}

		else if (DigitCount == 3)
		{
			if (printVM == 1)
			printf(" %d ", L);
			fprintf(outputfilepointer," %d ", L);
		}
		
		DigitCount = 0;


		for(DigitTarget = M; DigitTarget > 0;)
		{
			DigitTarget = DigitTarget / 10;
			DigitCount++;
		}

		if (DigitCount <= 1)
		{
			if (printVM == 1)
			printf("%d     ", M);
			fprintf(outputfilepointer,"%d     ", M);
		}

		else if (DigitCount == 2)
		{
			if (printVM == 1)
			printf("%d    ", M);
			fprintf(outputfilepointer,"%d    ", M);
		}

		else if (DigitCount == 3)
		{
			if (printVM == 1)
			printf("%d   ", M);
			fprintf(outputfilepointer,"%d   ", M);
		}
		
		DigitCount = 0;


		for(DigitTarget = pc; DigitTarget > 0;)
		{
			DigitTarget = DigitTarget / 10;
			DigitCount++;
		}

		if (DigitCount <= 1)
		{
			if (printVM == 1)
			printf("%d       ", pc);
			fprintf(outputfilepointer,"%d       ", pc);
		}

		else if (DigitCount == 2)
		{
			if (printVM == 1)
			printf("%d      ", pc);
			fprintf(outputfilepointer,"%d      ", pc);
		}

		else if (DigitCount == 3)
		{
			if (printVM == 1)
			printf("%d     ", pc);
			fprintf(outputfilepointer,"%d     ", pc);
		}
		
		DigitCount = 0;

		for (DigitTarget = bp; DigitTarget > 0;)
		{
			DigitTarget = DigitTarget / 10;
			DigitCount++;
		}

		if (DigitCount <= 1)
		{
			if (printVM == 1)
			printf("%d       ", bp);
			fprintf(outputfilepointer,"%d       ", bp);
		}

		else if (DigitCount == 2)
		{
			if (printVM == 1)
			printf("%d      ", bp);
			fprintf(outputfilepointer,"%d      ", bp);
		}

		else if (DigitCount == 3)
		{
			if (printVM == 1)
			printf("%d     ", bp);
			fprintf(outputfilepointer,"%d     ", bp);
		}
		
		DigitCount = 0;

		for (DigitTarget = sp; DigitTarget > 0;)
		{
			DigitTarget = DigitTarget / 10;
			DigitCount++;
		}

		if (DigitCount <= 1)
		{
			if (printVM == 1)
			printf("%d          ", sp);
			fprintf(outputfilepointer,"%d          ", sp);
		}

		else if (DigitCount == 2)
		{
			if (printVM == 1)
			printf("%d         ", sp);
			fprintf(outputfilepointer,"%d         ", sp);
		}

		else if (DigitCount == 3)
		{
			if (printVM == 1)
			printf("%d        ", sp);
			fprintf(outputfilepointer,"%d        ", sp);
		}
		
		DigitCount = 0;
		i = 0;

		for (o = 0; o < 8; o++)
		{
			if (printVM == 1)
			printf("%d ", registers[o]);
			fprintf(outputfilepointer,"%d ", registers[o]);
		}
		
		if (printVM == 1)
		printf("\n");
		fprintf(outputfilepointer,"\n");

		if (printVM == 1)
		printf("Stack: ");
		fprintf(outputfilepointer,"Stack: ");

		for (o = 1; o < sp + 1; o++)
		{ 
			if (printVM == 1)
			printf("%d ", stack[o]);
			fprintf(outputfilepointer,"%d ", stack[o]);
			
			if (o == entryLength[i] && numofentries > 1 && sp + 1 > o + 1)
			{
				if (printVM == 1)
				printf("| ");
				fprintf(outputfilepointer,"| ");
				i++;
			}
		}

		if (halt == 0)
		{
			if (printVM == 1)
			printf("\n\n");
			fprintf(outputfilepointer,"\n\n");
		}

		else
		{
			if (printVM == 1)
			printf("\n");
			fprintf(outputfilepointer,"\n");
		}

		//emergency++;
		//if (emergency == 90000)
		//{
		//	break;
		//}

	}

	return;
}


// This function acts as a tokenizer, you input a string and a symbol,
// and it seperates it into 3 parts: what comes before the symbol, what comes
// after the symbol, and the symbol. It then place those tokens into the 
// linked list in proper order
void seperator(struct lexHead *lexDHead, char *symbol)
{

	// variables required
	struct LexEntryType *lexData = NULL;
	struct LexEntryType *lexData2 = NULL;
	struct LexEntryType *lexData3 = NULL;
	struct LexEntryType *lexData4 = NULL;
	struct LexEntryType *lexData5 = NULL;
	char *checker1 = NULL;
	char *seperator1 = NULL;
	char *seperator2 = NULL;
	int lengthof1 = 0;
	int lengthof2 = 0;
	int i = 0;
	int extrasymbols;
	

	for (lexData = lexDHead->firstEntry; lexData->nextSym != NULL; lexData = lexData->nextSym)
	{
		if (lexData->tokenType == -1)
		{
			// determines if target symbol is detected
			// then detects if there is text before the symbol
			// and after the symbol.
			checker1 = strstr(lexData->lexeme, symbol);

			if (checker1 != NULL)
			{
				lengthof2 = strlen(checker1);
				lengthof1 = strlen(lexData->lexeme) - strlen(checker1);

				seperator2 = (char *) malloc(sizeof(char) * lengthof2);
				seperator1 = (char *) malloc(sizeof(char) * lengthof1);

				for (i = 0; i < lengthof1; i++)
				{
					seperator1[i] = lexData->lexeme[i];
				}
				
				for (i = 0; i < lengthof2; i++)
				{
					seperator2[i] = lexData->lexeme[i + lengthof1 + 1];
				}

				
				seperator1[lengthof1] = 0;
				seperator2[lengthof2] = 0;
				extrasymbols = strlen(symbol) - 1;
				if (extrasymbols != 0)
				{
					memmove(seperator2, seperator2+extrasymbols, strlen(seperator2));
				}

				// After detection, several cases are considered when inputting
				// back into array.

				// when there is text before and after symbol
				if ((int)strlen(seperator1) != 0 && (int)strlen(seperator2) != 0)
				{
					lexData4 = (struct LexEntryType *) malloc(sizeof(struct LexEntryType));
					lexData4->lexeme = (char *) malloc(sizeof(char));
					lexData4->lexeme = strcpy(lexData4->lexeme, symbol);
					lexData4->tokenType = LexToken(lexData4->lexeme);
					
					

					lexData2 = (struct LexEntryType *) malloc(sizeof(struct LexEntryType));
					lexData2->lexeme = (char *) malloc(sizeof(char) * lengthof2);
					strcpy(lexData2->lexeme, seperator2);
					lexData2->tokenType = LexToken(lexData2->lexeme);
					lexData2->nextSym = lexData->nextSym;

					lexData4->nextSym = lexData2;

					free(lexData->lexeme);
					lexData->lexeme = (char *) malloc(sizeof(char) * lengthof1);
					strcpy(lexData->lexeme, seperator1);
					lexData->tokenType = LexToken(lexData->lexeme);
					lexData->nextSym = lexData4;

				}

				// when there is text before but not after symbol
				else if ((int)strlen(seperator1) != 0 && (int)strlen(seperator2) == 0)
				{				

					lexData4 = (struct LexEntryType *) malloc(sizeof(struct LexEntryType));
					lexData4->lexeme = (char *) malloc(sizeof(char));
					lexData4->lexeme = strcpy(lexData4->lexeme, symbol);
					lexData4->tokenType = LexToken(lexData4->lexeme);
					lexData4->nextSym = lexData->nextSym;

					free(lexData->lexeme);
					lexData->lexeme = (char *) malloc(sizeof(char) * lengthof1);
					strcpy(lexData->lexeme, seperator1);
					lexData->tokenType = LexToken(lexData->lexeme);
					lexData->nextSym = lexData4;
				}

				// when there is text after but not before symbol
				else if ((int)strlen(seperator1) == 0 && (int)strlen(seperator2) != 0)
				{ 

					lexData4 = (struct LexEntryType *) malloc(sizeof(struct LexEntryType));
					lexData4->lexeme = seperator2;
					lexData4->tokenType = LexToken(lexData4->lexeme);
					lexData4->nextSym = lexData->nextSym;

					free(lexData->lexeme);
					lexData->lexeme = (char *) malloc(sizeof(char));
					lexData->lexeme = strcpy(lexData->lexeme, symbol);
					lexData->tokenType = LexToken(lexData->lexeme);
					lexData->nextSym = lexData4;
				}

				// if there is not text before or after symbol.
				else if ((int)strlen(seperator1) == 0 && (int)strlen(seperator2) == 0)
				{
					break;
				}


				lexData = lexDHead->firstEntry;

			}


		}

		for (lexData5 = lexDHead->firstEntry; lexData5->nextSym != NULL; lexData5 = lexData5->nextSym)
		{
			lexData5->tokenType = LexToken(lexData5->lexeme);
		}

	
	}

}

int isConstant(char *target)
{
	for(int i = symbolTableLocation; i != 0; i--)
	{
		if (strcmp(target, symboltable[i].name) == 0)
		{
			if (symboltable[i].kind == 1)
			{
				printf("Error 11, %s is a constant and cannot be overwritten\n", target);
				exit(EXIT_FAILURE);
			}
		}

	}

}

int isProcedure(char *target)
{
	for(int i = symbolTableLocation; i != 0; i--)
	{
		if (strcmp(target, symboltable[i].name) == 0)
		{
			if (symboltable[i].kind == 3)
			{
				printf("Error 23, %s is a procedure and cannot be overwritten\n", target);
				exit(EXIT_FAILURE);
			}
		}

	}

}

void isProcedure2(char *target)
{
	for(int i = symbolTableLocation; i != 0; i--)
	{
		if (strcmp(target, symboltable[i].name) == 0 && symboltable[i].mark == 0)
		{
			if (symboltable[i].kind == 3)
			{
				return;
			}

			printf("Error 24: callsym must be followed by an identifier\n");
			exit(EXIT_FAILURE);

		}

	}

}

// function prototypes to help with recursive decent.
void expression();
void procDeclaration();

void emit(int Type, int R, int L, int M)
{

	if (cx > MAX_CODE_LENGTH - 1)
	{
		printf("Error, Code length too long\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		code[cx][0] = Type;
		code[cx][1] = R;
		code[cx][2] = L;
		code[cx++][3] = M;
	}

}

int compare(char *target, int symbol)
{
	return atoi(target) == symbol;
}

void factor()
{
	int location;
	int number;

	// if the identifier symbol is detected
	if (compare(tokenTarget1, identsym))
	{
		// grabs the actual identifier
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		location = inSymbolTable(tokenTarget1);

		// put value into a register
		emit(3, RegTracker, 0, symboltable[location].addr);

		// grabs the next token
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
	}

	else if (compare(tokenTarget1, numbersym))
	{
		// grabs the actual number
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		number = atoi(tokenTarget1);

		emit(1, RegTracker, 0, number);
		// grabs the next token
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
	}

	else if (compare(tokenTarget1, lparentsym))
	{
		// beginning of contained expression (hopefully)
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		expression();
		if (!compare(tokenTarget1, rparentsym))
		{
			printf("error 17 in factor, left parethesis not ended with right parethesis\n");
			exit(EXIT_FAILURE);
		}

		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
	}

	else
	{
		printf("error 19, invalid symbol detected\n");
		exit(EXIT_FAILURE);
	}

	lastToken = tokenTarget1;

	return;
}

void term()
{
	char *multop;
	factor();
	while (compare(tokenTarget1, multsym) || compare(tokenTarget1, slashsym))
	{
		// save operator symbol
		multop = tokenTarget1;
		RegTracker++;
		// grab the first token of next factor
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		factor();

		// multiply
		if (compare(multop, multsym))
		{
			emit(15, RegTracker - 1, RegTracker - 1, RegTracker);
		}

		// divide
		else 
		{
			emit(16, RegTracker - 1, RegTracker - 1, RegTracker);
		}

		RegTracker--;

	}

	lastToken = tokenTarget1;

	return;
}

// basically stores values in whatever register regTracker is pointing to
void expression()
{
	char *addop;

	// if symbol is + or - getting into expression
	if (compare(tokenTarget1, plussym) || compare(tokenTarget1, minussym))
	{
		// saves the + || - symbol
		addop = tokenTarget1;

		// grabs the first term.
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		

		// term takes care of mult and div, should then be added or subtracted.
		// after this term the result of term is stored in RegTracker location.
		term();

		// negation of symbol in register
		if(compare(addop, minussym))
		{
			emit(12, RegTracker, 0, 0);
		}
	}

	else
	{
		
		// term takes care of mult and div, should then be added or subtracted.
		// after this term the result of term is stored in RegTracker location.
		term();
	}
	
	

	while (compare(tokenTarget1, plussym) || compare(tokenTarget1, minussym))
	{
		addop = tokenTarget1;
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		RegTracker++;
		term();

		// RegTracker - 1 should contain the answer
		// subtract 
		if(compare(addop, minussym))
		{
			emit(14, RegTracker - 1, RegTracker - 1, RegTracker);
			RegTracker--;
		}

		// add
		else if(compare(addop, plussym))
		{
			emit(13, RegTracker - 1, RegTracker - 1, RegTracker);
			RegTracker--;
		}

	}


	return;
}

void condition()
{
	int baseRegister;
	char *relationalOperator;

	// baseRegister is the register that contains the result of the comparison.
	baseRegister = RegTracker;

	if (compare(tokenTarget1, oddsym))
	{
		// this next token can only be an identifier, +, or minus symbol
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		expression();
		emit(17, RegTracker, 0, 0);

	}

	else
	{
		// baseRegister contains first term
		expression(); 


		// checks for relational operators > = < <> <= >=
		if(!compare(tokenTarget1, gtrsym) && !compare(tokenTarget1, eqsym) &&
		   !compare(tokenTarget1, lessym) && !compare(tokenTarget1, neqsym) &&
		   !compare(tokenTarget1, leqsym) && !compare(tokenTarget1, geqsym) )
		{
			printf("Error 16 in condition, relational operation missing in a conditional statement\n");
			exit(EXIT_FAILURE);
		}

		// save symbol
		relationalOperator = tokenTarget1;

		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);	

		// baseRegister + 2 contains the second term after execution.
		RegTracker++;
		expression();

		// implement the commands line args
		// move on with your life.

		if(compare(relationalOperator, gtrsym))
		{
			emit(23, baseRegister, baseRegister, RegTracker--);
		}

		else if(compare(relationalOperator, eqsym))
		{
			emit(19, baseRegister, baseRegister, RegTracker--);
		}

		else if(compare(relationalOperator, lessym))
		{
			emit(21, baseRegister, baseRegister, RegTracker--);
		}

		else if(compare(relationalOperator, neqsym))
		{
			emit(23, baseRegister, baseRegister, RegTracker--);
		}

		else if(compare(relationalOperator, leqsym))
		{
			emit(22, baseRegister, baseRegister, RegTracker--);
		}	

		else if(compare(relationalOperator, geqsym))
		{
			emit(24, baseRegister, baseRegister, RegTracker--);
		}


	}
	
	return;
}


void constDeclaration()
{
	int firsttime = 0;

	while (compare(tokenTarget1, semicolonsym) != 1)
	{

		// Token Symbol identifier found
		if (!compare(tokenTarget1, identsym))
		{	
			if (firsttime == 0)
			{
				printf("Error in constants, Const symbol not followed by identfier.\n");
			}

			else
			{
				printf("error in constants, identsym not found.\n");		
				printf("found %s instead\n", tokenTarget1);
			}

			exit(EXIT_FAILURE);
		}

		// Identifier grabbed
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);


		// Add identifier to the symbol table entry 
		symboltable[symbolTableLocation].kind = 1;
		symboltable[symbolTableLocation].mark = 0;

		strcpy(symboltable[symbolTableLocation].name, tokenTarget1);

		// grab the equals symbol
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		// check the equals symbol
		if (compare(tokenTarget1, eqsym) != 1 && compare(tokenTarget1, becomessym))
		{
			printf("Error in constants, use of := instead of = not found!!\n");
			exit(EXIT_FAILURE);
		}

		else if(compare(tokenTarget1, eqsym) != 1)
		{
			printf("Error in constants, \"=\" not found!!\n");
			exit(EXIT_FAILURE);
		}

		// grab the number symbol
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		// check the number symbol
		if (compare(tokenTarget1, numbersym) != 1)
		{
			printf("Error 2 in constants declaration, \"=\" must be follwed by a number\n");
			exit(EXIT_FAILURE);
		}

		// grab the number
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		// add number to the symbol entry
		symboltable[symbolTableLocation].val = atoi(tokenTarget1);
		symboltable[symbolTableLocation].addr = addr++;

		// grab the comma symbol or semicolon symbol
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		// check the comma symbol
		if (compare(tokenTarget1, commasym) != 1)
		{
			// checks for a semicolon instead
			if (compare(tokenTarget1, semicolonsym) == 1)
			{
				// just continue no need to do anything fancy.
			}

			// If no semicolon or comma detected, error is pushed 
			else
			{
				if (compare(tokenTarget1, identsym) == 1)
				{
					printf("Error 5, missing a comma in Variable declaration.\n");
					exit(EXIT_FAILURE);
				}

				else if (compare(tokenTarget1, semicolonsym) != 1)
				{
					printf("Error 5, missing a semicolon in Variable declaration.\n");
					exit(EXIT_FAILURE);
				}
			}

		}


		symbolTableLocation++;
		firsttime++;
		
		if (!compare(tokenTarget1, semicolonsym))
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);


	} // end of while loop



	tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

	return;
}

/*

struct symbol
{
	int kind;		// const = 1; var = 2, proc = 3
	char name[12];	// name up to 11 chars
	int val;		// number (ASCII value)
	int level;		// L level
	int addr;		// M address
};

*/


void varDeclaration()
{
	int firsttime = 0;

	while (!compare(tokenTarget1, semicolonsym))
	{
		// Token Symbol identifier found
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);


		symboltable[symbolTableLocation].mark = 0;
		
		if (!compare(tokenTarget1, identsym))
		{	
			if (firsttime++ == 0)
			{
				printf("error 4 in variable declarations, var symbol not followed by identfier.\n");
			}

			else
			{
				printf("error, invalid symbols identified.\n");		
			}

			exit(EXIT_FAILURE);
		}

		// Identifier grabbed
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		// Add identifier to the symbol table entry 
		symboltable[symbolTableLocation].kind = 2;
		symboltable[symbolTableLocation].val = 0;
		symboltable[symbolTableLocation].level = STACCLVL;
		symboltable[symbolTableLocation].addr = addr++;
		symboltable[symbolTableLocation].mark = 0;


		strcpy(symboltable[symbolTableLocation].name, tokenTarget1);


		// grab the comma symbol or semicolon symbol
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		// check the comma symbol
		if (!compare(tokenTarget1, commasym))
		{
			// checks for a semicolon instead
			if (compare(tokenTarget1, semicolonsym))
			{

			}

			// If no semicolon or comma detected, error is pushed 
			else
			{
				if (compare(tokenTarget1, identsym))
				{
					printf("Error 5 in variable declarations, missing a comma in Var declaration.\n");
					exit(EXIT_FAILURE);
				}

				else if (!compare(tokenTarget1, semicolonsym))
				{
					printf("Error 5 in variable declarations, missing a semicolon in Var declaration.\n");
					exit(EXIT_FAILURE);
				}
			}				

		}

		symbolTableLocation++;

	} // end of while loop

	lastToken = tokenTarget1;

	return;
}

void statement()
{
	int ctemp, cx1, cx2, ctemp2;
	int TABLELOCATION;

	// if the first token entered is an identifier,
	if(compare(tokenTarget1, identsym))
	{
		char * vartar;	

		// identifier is grabbed
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		vartar = tokenTarget1;

		TABLELOCATION = inSymbolTable(tokenTarget1);
		isConstant(vartar);
		isProcedure(vartar);

		// := symbol is grabbed
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		// if not the := symbol, 
		if (!compare(tokenTarget1, becomessym))
		{
			if (compare(tokenTarget1, eqsym))
			{
				printf("Error 1, use of \"=\" symbol instead of \":=\" symbol in statement\n");
			}
			else
			{
				printf("Error 12, Missing \":=\" symbol in statement\n");
			}

			exit(EXIT_FAILURE);
		}

		// this token is the start of the EXPRESSION.
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		if (compare(tokenTarget1, endsym) || compare(tokenTarget1, periodsym) || compare(tokenTarget1, semicolonsym))
		{
			printf("Error 6, expression expected for assignment of variable %s\n", vartar);
			exit(EXIT_FAILURE);
		}

		expression();
		emit(4, RegTracker, 0, symboltable[TABLELOCATION].addr);

	}

	// if the first token entered is the begin
	else if(compare(tokenTarget1, beginsym))
	{
		// first chracter of first statement.
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		if (compare(tokenTarget1, endsym) || compare(tokenTarget1, periodsym))
		{
			printf("Error 6, statement expected after begin\n");
			exit(EXIT_FAILURE);
		}

		statement();

		// statement returns with a semicolon, loop while detecting semicolons.
		while (compare(tokenTarget1, semicolonsym) || lookAhead == 1 && tokenTarget1 != NULL)
		{	
			if (lookAhead != 1)
			{
				tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
			}

			else 
			{
				lookAhead = 0;
			}

			if (tokenTarget1 == NULL)
			{
				return;
			}

			statement();

		}


		if(!compare(tokenTarget1, endsym))
		{
			printf("Error 9, semicolon is missing in statement.\n");
			tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

			exit(EXIT_FAILURE);
		}

		// should be a period.
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
	}

	// if statements
	else if(compare(tokenTarget1, ifsym))
	{
		// token that starts the condition statement
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		condition();

		// check to see if condition is followed by then symbol
		if (!compare(tokenTarget1, thensym))
		{
			printf("Error 13, if symbol not eventually followed by then symbol\n");
			exit(EXIT_FAILURE);
		}

		// grabs the next token to then go into statement.
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		if (compare(tokenTarget1, periodsym) || compare(tokenTarget1, endsym))
		{
			printf("Error 6, statement expected after \"then\"\n");
			exit(EXIT_FAILURE);
		}

		ctemp = cx;

		// CONDITIONAL CHECK
		emit(8, RegTracker, 0, cx);
		statement();

		// END OF CODE IF TRUE
		ctemp2 = cx;

		// IF CONDITIONAL CHECK IS TRUE, SKIP ELSE
		emit(7, 0, 0, cx + 1);

		// tokentarget1 here is semicolon

		// if false, code will jump to here
		code[ctemp][3] = cx;

		// either else, or another character, or end

		if (!compare(tokenTarget1, periodsym) && !compare(tokenTarget1, elsesym)&& tokenTarget1 != NULL)
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		if (compare(tokenTarget1, elsesym))
		{
			tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
			statement();
			// TO SKIP ELSE STATMENT AT END OF TRUE IF TRUE IS EXECUTED
			code[ctemp2][3] = cx;
		}

		else
		{
			lookAhead = 1;
		}


	}

	// while loops
	else if(compare(tokenTarget1, whilesym))
	{
		// location of the start of the conditional logic operators
		cx1 = cx;

		// tokentarget is beginning of the conditional statement
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		condition();
		
		// start of the statements that 
		cx2 = cx;

		// JPC PICK REGISTER
		emit(8, RegTracker, 0, 0);

		// check to see if the condition is followed by do symbol.
		if (!compare(tokenTarget1, dosym))
		{
			printf("error 15, while symbol not eventually followed by a do symbol\n");
			exit(EXIT_FAILURE);
		}
		
		// beginning of statement. 
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		if (compare(tokenTarget1, periodsym) || compare(tokenTarget1, endsym))
		{
			printf("Error 6, statement expected after \"do\"\n");
			exit(EXIT_FAILURE);
		}

		statement();
		
		// JMP
		emit(7, 0, 0, cx1);
		code[cx2][3] = cx;

	}

	// read symbol detected, read user input
	else if(compare(tokenTarget1, readsym))
	{
		char *trueIdentifier;
		int identfierLocation;
		int StaccLocation;

		// tokentarget is the identifier symbol
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		// tokentarget is true identifier
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		trueIdentifier = tokenTarget1;
		identfierLocation = inSymbolTable(trueIdentifier);
		StaccLocation = symboltable[identfierLocation].addr;

		// Ask user to put in value, save to register in current level.
		emit(10, RegTracker, 0, 2);
		emit(4, RegTracker, 0, StaccLocation);

		//this is the semicolon
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
	}

	// write symbol detected, write symbol to screen
	else if(compare(tokenTarget1, writesym))
	{
		char *trueIdentifier;
		int identfierLocation;
		int StaccLocation;

		// tokentarget is the identifier symbol
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		// tokentarget is true identifier
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

		// TrueIdentifier contains symbol that must be searched in table
		trueIdentifier = tokenTarget1;

		// find symbol in table
		identfierLocation = inSymbolTable(trueIdentifier);

		// find the location within the stacc
		StaccLocation = symboltable[identfierLocation].addr;

		// Put in value from address into the register.
		// LOD, into Register R, level L, with offset M. 
		emit(3, RegTracker, 0, StaccLocation);

		// Write to screen from that register
		emit(9, RegTracker, 0, 1);

		//this is the semicolon
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
	}

	else if (compare(tokenTarget1, callsym))
	{
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		if (!compare(tokenTarget1, identsym))
		{
			printf("Error 24, callsym must be followed by an identifier\n");
			exit(EXIT_FAILURE);
		}

		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		isProcedure2(tokenTarget1);
		TABLELOCATION = inSymbolTable(tokenTarget1);
		emit(5, 0, symboltable[TABLELOCATION].level, symboltable[TABLELOCATION].addr);
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

	}

	lastToken = tokenTarget1;
	return;
}

void block()
{
	tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

	if (compare(tokenTarget1, constsym))
	{
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		constDeclaration();
		if (tokenTarget1 == NULL)
		{
			return;
		}
	}

	if (compare(tokenTarget1, varsym))
	{
		varDeclaration();
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		if (tokenTarget1 == NULL)
		{
			return;
		}
				
	}

	if (compare(tokenTarget1, procsym))
	{

		emit(7, 0, 0, 0);

		while (compare(tokenTarget1, procsym))
		{
			procDeclaration();
		}
		

		code[0][3] = cx;

		if (tokenTarget1 == NULL)
		{
			return;
		}


	}
	
	int addr2 = 4;
	for (int i = 0; i < symbolTableLocation; i++)
	{
		if (STACCLVL == symboltable[i].level && symboltable[i].mark == 0)
			addr2++;
	}


	// initalize stack, symbolTableLocation also includes offset of initalization
	emit(6, 0, 0, addr2);

	// put in the constants
	for (int i = symbolTableLocation - addr; i < symbolTableLocation; i++)
	{
		if (symboltable[i].kind == 1)
		{	
			// put value into register
			emit(1, 0, 0, symboltable[i].val);

			// store value into S T A C C
			emit(4, 0, 0, symboltable[i].addr);
		}
	}

	statement();

	if (STACCLVL == 0)
	{
		emit(11, 0, 0, 3);
	}
	
	else
	{
		emit(2, 0, 0, 0);
	}

}

// enters with proc symbol
void procDeclaration()
{

	// get the identifier indicator
	tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
	if (!compare(tokenTarget1, identsym))
	{
		printf("Error 22, Missing identifier after procedure\n");
		exit(EXIT_FAILURE);
	}

	// get the actual identifier
	tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
	symboltable[symbolTableLocation].kind = 3;
	symboltable[symbolTableLocation].val = 0;
	symboltable[symbolTableLocation].level = STACCLVL++;
	symboltable[symbolTableLocation].addr = cx;
	symboltable[symbolTableLocation].mark = 0;

	strcpy(symboltable[symbolTableLocation++].name, tokenTarget1);

	tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
	if (!compare(tokenTarget1, semicolonsym))
	{
		printf("Error 9, Missing semicolon in procedure after name declaration\n");
		exit(EXIT_FAILURE);
	}

	block();

	// should be a semicolon
	if (!compare(tokenTarget1, semicolonsym))
	{
		printf("Error 9, missing semicolon at end of procedure\n");
		printf("instead, found %s\n", tokenTarget1);
		exit(EXIT_FAILURE);
	}

	// grab beginning of the next procedure, or a begin
	tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);

	STACCLVL--;
	for (int i = symbolTableLocation; i > symbolTableLocation - addr; i--)
	{
		symboltable[symbolTableLocation].mark = 1;
	}


	return;
}

/*

struct symbol
{
	int kind;		// const = 1; var = 2, proc = 3
	char name[12];	// name up to 11 chars
	int val;		// number (ASCII value)
	int level;		// L level
	int addr;		// M address
	int mark;
};
*/



int main(int argc, char *argv[])
{
	// input file name
	char *inputfile = "input.txt";
	// output file name
	char *outputfile = "output.txt";
	// Contains line from input
	char line[300];

	// Contains all characters
	char *SOURCE = "";
	// Used to ensure no extra space is added to beginning of SOURCE
	int trigger = 0;

	// Used to find total amount of characters in input
	int numofchar = 0;

	// used to check values in 
	char array1[300] = "";
	char array2[300] = "";

	// used to cycle through SOURCE array
	int i = 0;

	// used to keep track of array1 length
	int len = 0;

	// Linked List structure variables
	struct lexHead lexDHead;
	struct LexEntryType *lexData;
	struct LexEntryType *lexData2 = NULL;
	struct LexEntryType *lexData3 = NULL;
	struct LexEntryType *lexData4 = NULL;
	struct LexEntryType *lexEmergency = NULL;

	struct lexHead *Pointer = NULL;


	// used for seperating strings to proper format
	char *checker1 = NULL;
	char *seperator1 = NULL;
	char *seperator2 = NULL;
	int lengthof1 = 0;
	int lengthof2 = 0;

	// used to help identify identifiers
	int varTrigger;

	// triggers if an assignment to an identifier isn't a proper number.
	int invalidAssignment = 0;
	int integerA;
	char *invalid;
	char test;
	// variables relating to comment detection

	int commentTrigger1 = 0;
	int commentTrigger2 = 0;

	// File pointers are used to access files
	// inputfilepointer corresnponds to *inputfile string
	// outputfilepointer corresponds to *outputfile string
	inputfilepointer = fopen(inputfile, "r");
	outputfilepointer = fopen(outputfile, "w");


/*
int printTokens = 0;
int printAssembly = 0;
int printVM = 0;
*/

	if (argc > 1)
	{
		for (int i = 0; i < argc; i++)
		{
			if (strcmp(argv[i], "-l") == 0)
			{
				printTokens = 1;
			}

			else if (strcmp(argv[i], "-a") == 0)
			{
				printAssembly = 1;
			}

			else if (strcmp(argv[i], "-v") == 0)
			{
				printVM = 1;
			}

		}

	}

	if (printTokens == 0 && printAssembly == 0 && printVM == 0)
	{
		printf("in\n");
	}


	// Used to create token array
	int h = 0;

	if (inputfilepointer == NULL)
	{
		fprintf(stderr, "Can't open input file!\n");
		return(1);
	}

	if (outputfilepointer == NULL)
	{
		fprintf(stderr, "Can't open output file!\n");
		return(1);
	}

	// Cycle through the input file to get number of entries
	// that shoud be in code.

	// print out Source program and count how many characters
	while (fgets(line, 300, inputfilepointer) != NULL)
	{
		numofchar += strlen(line);
	}

	//if no characters
	if (numofchar == 0)
	{
		printf("in\n");
		printf("out\n");
		return 0;
	}

	// move filepointer back to beginning
	rewind(inputfilepointer);

	// allocate just enough memory for the amount of characters
	SOURCE = malloc(numofchar * sizeof(int));

	int c;
	i = 0;

	// SOURCE now contains an array of all characters in the file
	while ((c = getc(inputfilepointer)) != EOF)
	{

		if (c == 10 || c == 9 || c == 13)
		{
			SOURCE[i++] = 32;
			continue;
		}

		SOURCE[i++] = c;
	}


	// This for loop initalizes the linked list.
	// Contents of SOURCE is used to initalize the size of each
	// char *lexeme in each entry.
	// the linked list is dynamically allocated memory like SOURCE


	// Here is where we tokenize the input string and put it into a linked list.
	lexDHead.size = 0;
	char *token = strtok(SOURCE, " ");

	// first character	
	lexData = (struct LexEntryType *) malloc(sizeof(struct LexEntryType));
	lexData->lexeme = (char *) malloc(sizeof(char) * strlen(token));
	strcpy(lexData->lexeme, token);
	lexData->nextSym = NULL;

	lexDHead.firstEntry = lexData;
	lexDHead.lastEntry = lexData;

	// the rest of the characters
	while (token != NULL)
	{
		token = strtok(NULL, " ");
		if (token == NULL)
			break;

		lexData2 = lexData;
		lexData = (struct LexEntryType *) malloc(sizeof(struct LexEntryType));
		lexData->lexeme = (char *) malloc(sizeof(char) * strlen(token));
		lexData->tokenType = -1;
		strcpy(lexData->lexeme, token);
		lexData2->nextSym = lexData;

	}

	lexData2 = lexData;
	lexData = (struct LexEntryType *) malloc(sizeof(struct LexEntryType));
	lexData->lexeme = (char *) malloc(sizeof(char));
	lexData->tokenType = -1;
	strcpy(lexData->lexeme, " ");
	lexData2->nextSym = lexData;



	// the grande parsing, each one checks for a symbol.
	Pointer = &lexDHead;

	seperator(Pointer, ":=");
	seperator(Pointer, ",");
	seperator(Pointer, "+");
	seperator(Pointer, "-");
	seperator(Pointer, "/");
	seperator(Pointer, "*");
	seperator(Pointer, "(");
	seperator(Pointer, ")");
	seperator(Pointer, "<");
	seperator(Pointer, "=");
	seperator(Pointer, ":");
	seperator(Pointer, ">");
	seperator(Pointer, ".");
	seperator(Pointer, ";");
	seperator(Pointer, ":");
	// at this point, the contents of the file are parsed.

	varTrigger = 0;
	invalidAssignment = 0;

	// check for assignments.
	for(lexData = lexDHead.firstEntry; lexData->nextSym != NULL; lexData = lexData->nextSym)
	{
		
		varTrigger = 1;


		i = 0;

		test = lexData->lexeme[0];
		while (isdigit(test) && i < strlen(lexData->lexeme))
		{
			test = lexData->lexeme[i++];
		}


		if (isdigit(test) == 0)
			varTrigger = 0;

		if (varTrigger == 1 && lexData->tokenType == -1)
		{
			if (strcmp(lexData->lexeme,"0") == 0)
			{
				lexData->tokenType = numbersym;
			}

			else
			{
				integerA = 0;
				integerA =(int)strtol(lexData->lexeme, &invalid, 10);

				if (strcmp(invalid, "") != 0 && integerA != 0)
				{	
					invalidAssignment = 1;
				}
				

				if (integerA > 99999 || integerA < -99999)
				{	
					invalidAssignment = 1;
					lexData->tokenType = -2;
				}
 				
 				if (invalidAssignment != 1)
 				{
 					lexData->tokenType = numbersym;
 				}
			}

		}

		if (isdigit(test) == 0)
			varTrigger = 1;

	}
	
	varTrigger = 0;

	// Check identifiers, labels identifiers with proper token type.
	for(lexData = lexDHead.firstEntry; lexData->nextSym != NULL; lexData = lexData->nextSym)
	{
		i = 0;

		if (lexData->tokenType == -1)
		{
			test = lexData->lexeme[0];
			if (isalpha(test))
			{
				test = lexData->lexeme[i++];
			}

			while ((isalpha(test) || isdigit(test)) && i < strlen(lexData->lexeme))
			{
				test = lexData->lexeme[i++];
				if (i > 11)
				{
					printf("Error: identifier longer than 11 characters detected.\n");
					fprintf(outputfilepointer, "Error: identifier longer than 11 characters detected.\n");
					return 1;
				}
			}

			if (isalpha(test) == 0)
			{
				varTrigger = 1;
			}

			else
			{
				lexData->tokenType = identsym;
			}
		}
			
	}

	// check for comments, also detects if there is no closing comment characters.
	for(lexData = lexDHead.firstEntry; lexData->nextSym != NULL; lexData = lexData->nextSym)
	{

		if( lexData->nextSym != NULL && strcmp(lexData->lexeme, "/") == 0 && commentTrigger1 == 0)
			lexData2 = lexData;

		if (strcmp(lexData->lexeme, "/") == 0)
		{
			commentTrigger1 = 1;

			if (strcmp(lexData->nextSym->lexeme, "*") == 0)
			{
				commentTrigger2 = 1;
				lexData->tokenType = -3;
				lexData->nextSym->tokenType = -3;
			}

		}

		if (commentTrigger2 == 1 && commentTrigger1 == 1)
		{
			lexData->tokenType = -3;

			if (strcmp(lexData->lexeme, "*") == 0 && strcmp(lexData->nextSym->lexeme, "/") == 0)
			{
				lexData->nextSym->tokenType = -3;
				commentTrigger1 = 0;
				commentTrigger2 = 0;
			}

		}
	}


	// Checking to see if any of the identifiers start with a number
	for(lexData = lexDHead.firstEntry; lexData->nextSym != NULL; lexData = lexData->nextSym)
	{
		if (lexData->tokenType != -3)
		{
			if (lexData->tokenType == identsym)
			{
				if (isdigit(lexData->lexeme[0]) != 0)
				{
					printf("Error: one variable starts with a digit\n");
					return(1);
				}

			}
		}

	}

	// Checking to see if any of the numbers are too long
	for(lexData = lexDHead.firstEntry; lexData->nextSym != NULL; lexData = lexData->nextSym)
	{
		if (lexData->tokenType != -3)
		{
			if (lexData->tokenType == numbersym || lexData->tokenType == -2)
			{
				
				if (strlen(lexData->lexeme) >= 6)
				{
					printf("Error 20, number too long detected.\n");
					return(1);
				}
						
			}
		}
	}


	// Check to see if any of the name is to long
	for(lexData = lexDHead.firstEntry; lexData->nextSym != NULL; lexData = lexData->nextSym)
	{
		if (lexData->tokenType != -3)
		{
			if (lexData->tokenType == identsym)
			{
				if (strlen(lexData->lexeme) > 11)
				{
					printf("Error 21: identifier longer than 11 characters detected.\n");
					return(1);
				}

			}
		}

	}



	// Check for bad symbols
	for(lexData = lexDHead.firstEntry; lexData->nextSym != NULL; lexData = lexData->nextSym)
	{
		// unidentified symbols encased within identifiers.
		if (lexData->tokenType == 2 || lexData->tokenType == -1)
		{
			if (isdigit(lexData->lexeme[0]))
			{
				printf("Error, identifier starts with number\n");
				return(1);
			}

			for (i = 0; i < strlen(lexData->lexeme); i++)
			{
				if (isdigit(lexData->lexeme[i]) == 0 && isalpha(lexData->lexeme[i]) == 0)
				{	
					printf("Error: invalid symbol in a identifier detected.\n");
					return(1);
				}
			}

			lexData->tokenType = 2;
		}
	}

	for(lexData = lexDHead.firstEntry; lexData->nextSym != NULL; lexData = lexData->nextSym)
	{
		if (lexData->tokenType == -1)
		{
			printf("Error: invalid symbol detected\n");
			return(1);
		}
	}
	

	// check to see if comments aren't terminated.
	if (commentTrigger1 == 1 && commentTrigger2 == 1)
	{
		printf("Error: closing comments not detected*/\n");
		return(1);
	}

	// At this point, the structure still contains comments, the following for loop will get rid of
	// any comments that exist within the program, as they are marked with the value -3.
	// Furthermore, at this point, we have also detected to see if there are any comments that aren't closed.
	
	lexData2 = lexDHead.firstEntry;
	for (lexData = lexDHead.firstEntry; lexData->nextSym != NULL;)
	{
		// detect start of a comment
		if (lexData == lexDHead.firstEntry && lexData->tokenType == -3)
		{
			lexDHead.firstEntry = lexDHead.firstEntry->nextSym;
			free(lexData->lexeme);
			free(lexData);
			lexData = lexDHead.firstEntry;
		}

		else
		{
			lexData = lexData->nextSym;
		}


	}

	lexData2 = lexDHead.firstEntry;
	for (lexData = lexDHead.firstEntry; lexData->nextSym != NULL;)
	{
		
		if (lexData->nextSym->tokenType == -3)
		{
			lexData3 = lexData->nextSym;
			lexData2 = lexData->nextSym->nextSym;
			lexData->nextSym = lexData2;
			free(lexData3->lexeme);
			free(lexData3);
			
		}

		else
		{
			lexData = lexData->nextSym;
		}

	}



	// At this point, there are no comments within the linked list and now a stream of 
	// tokens can be streamed to output.

	// Beginning of printing output to console and file.
	rewind(inputfilepointer);

	/*
	printf("Source Program:\n");
	fprintf(outputfilepointer, "Source Program:\n");

	while (fgets(line, 300, inputfilepointer) != NULL)
	{
		printf("%s", line);
		fprintf(outputfilepointer, "%s", line);
	}

	*/

	char *tokenString;
	char *tokenStringSave;
	char tokenBuffer[60];

	if (printTokens == 1)
	{
		printf("Lexeme List:\n");
		//fprintf(outputfilepointer, "\nLexeme List:\n");

		for (lexData = lexDHead.firstEntry; lexData->nextSym != NULL; lexData = lexData->nextSym)
		{
			if (lexData->tokenType != -3)
			{
				h++;
				printf("%d ", lexData->tokenType);
				//fprintf(outputfilepointer, "%d ", lexData->tokenType);

				if (lexData->tokenType == identsym || lexData->tokenType == numbersym)
				{
					printf("%s ", lexData->lexeme);
					//fprintf(outputfilepointer, "%s ", lexData->lexeme);
					h++;
				}
				
				else
				{
					// nothing is done, this is just here so its easier to follow.
				}

			}	

		}

		printf("\n\nSymbolic Representation:\n");
		//fprintf(outputfilepointer, "\n\nSymbolic Representation:\n");

		for (lexData = lexDHead.firstEntry; lexData->nextSym != NULL; lexData = lexData->nextSym)
		{
			if (lexData->tokenType != -3)
			{
				h++;
				LexTokenPrint(lexData->tokenType);
				// fprintf(outputfilepointer, "%d ", lexData->tokenType);

				if (lexData->tokenType == identsym || lexData->tokenType == numbersym)
				{
					printf("%s ", lexData->lexeme);
					// fprintf(outputfilepointer, "%s ", lexData->lexeme);
					h++;
				}
				
				else
				{
					// nothing is done, this is just here so its easier to follow.
				}

			}	

		}

		printf("\n\n");
	}




	//fprintf(outputfilepointer, "\n");

	tokenString = malloc(sizeof(char) * h);
	tokenStringSave = tokenString;

	// Count number of entries to put into main token string
	for (lexData = lexDHead.firstEntry; lexData->nextSym != NULL; lexData = lexData->nextSym)
	{
		if (lexData->tokenType != -3)
		{
			sprintf(tokenBuffer, "%d", lexData->tokenType);
			strcat(tokenString, tokenBuffer);
			strcat(tokenString, " ");
			
			if (lexData->tokenType == identsym || lexData->tokenType == numbersym)
			{
				strcat(tokenString, lexData->lexeme);
				strcat(tokenString, " ");
			}
			
			else
			{
				// nothing is done, this is just here so its easier to follow.
			}

		}	

	}

	tokenStream = tokenString;
	// tokenStream contains the tokens that we need.

	block();


	if (tokenTarget1 == NULL || !compare(tokenTarget1, periodsym))
	{
		printf("error 8, program doesn't end with \".\" symbol\n");
		printf("instead ends with %s\n", tokenTarget1);
		exit(EXIT_FAILURE);
	}


	if (tokenTarget1 != NULL)
	{
		tokenTarget1 = strtok_r(tokenStream, " ", &tokenStream);
		if (tokenTarget1 != NULL)
		{
			printf("error 7, incorrect symbol after statement\n");
			exit(EXIT_FAILURE);
		}
	}

	if (printTokens == 1 || printAssembly == 1 || printVM == 1)
		printf("\n");

	printf("No errors, program is syntactically correct\n");

	if (printTokens == 1 || printAssembly == 1|| printVM == 1)
		printf("\n");

	/*
	printf("last token is %s\n", lastToken);

	printf("the symbol table contains:\n");
	for(int p = 1; p < symbolTableLocation; p++)
	{
		printf("\nsymbol %d\n", p);
		printf("kind: %d\n", symboltable[p].kind);
		printf("name: %s\n", symboltable[p].name);
		printf("value: %d\n", symboltable[p].val);
		printf("level: %d\n", symboltable[p].level);
		printf("address: %d\n\n", symboltable[p].addr);
	}


	// check for period
	// continue;
	*/


	pregister();

	free(SOURCE);

	for (lexData = lexDHead.firstEntry; lexData->nextSym != NULL;)
	{
		free(lexData->lexeme);
		lexData2 = lexData;
		lexData = lexData->nextSym;
		free(lexData2);
	}


	if (printTokens == 0 && printAssembly == 0 && printVM == 0)
	{
		printf("out\n");
	}

	return 0;
}