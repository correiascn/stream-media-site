/*
CIS*2750
Assignment 1 - C++ to C
Nelson Correia
0930748
correian@mail.uoguelph.ca
27 January 2017
Converts a .cc (C++) file to a .c (C) file.
Functionality is greatly limited.
See README for limitations and usage.
*/

#ifndef __CORREIAN_A1__
#define __CORREIAN_A1__

/* As stated in lecture, the max number of characters in an ANSI C program line allowed is 509. */
#define LINE_SIZE 509

typedef struct Token
{
	/* "Text" is the actual content of the token, while "postwhitespace" is used to store the whitespace characters that follow in the original input file. */
	char* text;
	char* postwhitespace;
}
Token;

typedef struct Function
{
	Token** tokens; /* The entirety of the function: it's header and body, up to the closing braces. */
	char argList[128]; /* The list of parameters, storing only the data types' first character -- int: i, double: d, float: f, etc... */
	char altName[128];

	int numTokens;
	int argIndex;
}
Function;

typedef struct classData
{
	char type[64];				/* The "type", or name of the struct/class. */
	Function** functions;
	Token** variables;			/* Variables that exist of this specified type. */
	Token** classVariables;		/* Class variables (variables declared inside the class). */

	int numFunctions;
	int numVariables;
	int numClassVariables;
}
classData;

typedef struct InputFile
{
	/* "Text" refers to the contents of the original input file. "tokens" stores the parsed data read in. */
	FILE* file;
	char filename[255]; /* According to Google, the maximum number of characters for a filename in Linux is 255 characters. */
	int numLines;
	char** data;

	/* Variables for "part 1" of development -- parsing. */
	Token** tokens;
	int numTokens;
	int currentTokenIndex;
	int currentTokenTextIndex;
	int currentTokenWhitespaceIndex;
	Token* currentToken;

	/* Variables for "part 2" of development -- conversion. */
	classData** classes; /* Stores the various classes contained. Dynamically resize. */
	int numClasses;
	classData* currentClass;
}
InputFile;

/*
 * getNumLines
 * Given a file, the number of lines it contains is returned.
 * IN: FILE* fp
 * OUT: int: number of lines within the file pointed by fp.
 * POST: N/A.
 * ERROR: fp must not be NULL.
 * Function only recommended for source code files, as they usually end with a newline.
 */
int getNumLines(FILE* fp);

/*
 * parseInputSourceFile
 * Parses the input C++ source file and splits it into tokens.
 * IN: InputFile* file
 * OUT: N/A.
 * POST: The other members of the struct file are modified.
 * The parsed file's contents are stored in file->tokens.
 * ERROR: file must not be NULL.
 */
void parseInputSourceFile(InputFile* file);

/*
 * updateCurrentToken
 * Updates the current token being read. Used by parseInputSourceFile.
 * IN: InputFile* file
 * OUT: N/A.
 * POST: Previous current token has it's string members null-terminated.
 * InputFile member "currentToken" is changed to a new Token.
 * ERROR: file must not be NULL.
 * Must be enough memory left, otherwise the program will exit.
 */
void updateCurrentToken(InputFile* file);

/*
 * freeInputFileData
 * Depending on what part, or "phase" of the program this function is called,
 * different members of the provided struct variable are freed.
 * IN: InputFile* file, int phase
 * OUT: N/A.
 * POST: Depends on what phase this function is invoked.
 *		Phase 1: The member "data" is entirely freed.
 * 		Phase 2: The member "classes" and submembers are all freed.
 * ERROR: file must not be NULL.
 */
void freeInputFileData(InputFile* file, int phase);

/*
 * outputCFile
 * Outputs the converted C file. Also handles the conversion process.
 * IN: InputFile* file
 * OUT: N/A.
 * POST: The output file (the same name as input file, but instead of type .c)
 * has its contents altered to be a C source file rather than one of C++ lite.
 * ERROR: file must not be NULL.
 */
void outputCFile(InputFile* file);

/*
 * updateCurrentFunction
 * Updates the current function being read.
 * IN: InputFile* file
 * OUT: N/A.
 * POST: currentFunction altered.
 * ERROR: file must not be NULL.
 */
void updateCurrentFunction(InputFile* file);

/*
 * condenseFunctionHeader
 * Takes the first letter of every parameter type, and stores it.
 * IN: InputFile* file
 * OUT: N/A.
 * POST: function->argList altered.
 * ERROR: file must not be NULL.
 */
void condenseFunctionHeader(InputFile* file);

/*
 * updateCurrentClass
 * Updates the current class being read.
 * IN: InputFile* file
 * OUT: N/A.
 * POST: currentClass altered.
 * ERROR: file must not be NULL.
 */
void updateCurrentClass (InputFile* file);

/*
 * printClassFunctions
 * Outputs and alters all functions outside of their class upon conversion.
 * IN: InputFile* file
 * OUT: N/A.
 * POST: Contents of class functions altered and printed.
 * ERROR: file must not be NULL.
 */
void printClassFunctions(InputFile* file, FILE* outputFile);

#endif