/*
CIS*2750
Assignment 1 - C++ "lite" to C converter
Nelson Correia
0930748
correian@mail.uoguelph.ca
27 January 2017
Converts a .cc (C++ lite) file to a .c (C) file.
Functionality is limited to C++ "lite".
See README and A1 specs for limitations and usage.
*/

/* Please see the header files for function tags! */

/* Importing standard C libraries. */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/* Including user-defined libraries. */
#include "a1.h"

/**
 * main
 * Main method for the conversion of C++ file to C.
 * IN: int argc, char* argv[]. See README for operation.
 * OUT: N/A. See POST.
 * POST: The newly created .cc file will be made in the root directory of this application.
 * ERROR: Command line input must be as specified in the README. Anything else will ruin it.
 * See README for what the input file should be like. Bad input will not be compensated for.
 */
int main (int argc, char* argv[])
{
	/* The program is entirely useless without an expression. */
	if (argv[1] == NULL)
	{
		printf("\nNo input file was provided to convert! Exiting...\n");
		exit(0);
	}

	InputFile* file;
	file = calloc(1, sizeof(InputFile));

	if (file == NULL)
	{
		printf("\nNot enough memory! Exiting...\n");
		exit(0);
	}

	strcpy(file->filename, argv[1]);
	parseInputSourceFile(file);
	outputCFile(file);

	return 0;
}

int getNumLines(FILE* fp)
{
	int numLines = 0;
	char line[LINE_SIZE];

	while(fgets(line, LINE_SIZE, fp) != NULL)
	{
		numLines++;
	}

	return numLines;
}

void parseInputSourceFile(InputFile* file)
{
	file->file = fopen(file->filename, "r");

	if (file->file == NULL)
	{
		printf("\nSpecified file could not be opened! Exiting...\n");
		exit(0);
	}

	char line[LINE_SIZE];
	file->numLines = getNumLines(file->file);
	fclose(file->file);

	/* Initialize the 2d array that is data, then fill it with the contents of the input file. */
	file->data = calloc(file->numLines, sizeof(char*));

	if (file->data == NULL)
	{
		printf("\nNot enough memory! Exiting...\n");
		exit(0);
	}

	file->file = fopen(file->filename, "r");
	if (file->file == NULL)
	{
		printf("\nSpecified file could not be opened! Exiting...\n");
		exit(0);
	}

	int counter = 0;

	while(fgets(line, LINE_SIZE, file->file) != NULL)
	{
		/* Store the line in the File struct. */
		file->data[counter] = calloc(strlen(line) + 1, sizeof(char));

		if (file->data[counter] == NULL)
		{
			printf("\nNot enough memory! Exiting...\n");
			exit(0);
		}

		/*strcpy(file->data[counter], line);*/
		memcpy(file->data[counter], line, strlen(line) + 1);
		counter++;
	}

	/* "Closing" up the program -- specifically freeing up memory no longer in use. */
	fclose(file->file);

	/* Token parsing variables. Must be kept outside of any loops. */
	file->numTokens = 1;
	file->tokens = calloc(file->numTokens, sizeof(Token*));
	file->tokens[0] = calloc(1, sizeof(Token));
	file->currentToken = file->tokens[0];
	file->currentToken->text = calloc(LINE_SIZE, sizeof(char));
	file->currentToken->postwhitespace = calloc(LINE_SIZE, sizeof(char));
	file->currentTokenIndex = 0;
	file->currentTokenTextIndex = 0;
	file->currentTokenWhitespaceIndex = 0;

	bool inQuotes = false;				/* Within a string, enclosed by "quotes". */
	bool inANSIComment = false;			/* Within an ANSI comment -- just like this comment. */
	bool inCPPComment = false;			/* Within a C++ style comment, starting with "//" and ending at EOL. */
	bool inCharQuotes = false;			/* Within single quotation marks, like a character: 'x'. */
	bool inIncludeDefine = false;		/* Within an #include or #define statement, which go up to the end of line. */

	/* Read in the contents of the file into the corresponding struct member "data". */
	int j;
	for (j = 0; j < file->numLines; j++)
	{
		/* Tokenize the current line. We will do this through character-by-character iteration. */
		int i;
		for (i = 0; i < strlen(file->data[j]); i++)
		{
			switch (file->data[j][i])
			{
				case '#':

					/* Assume that anytime a '#' appears in code it is for #define or #include. If not in a String, comment, etc. it is a new token. */
					if (inQuotes == false && inANSIComment == false && inCPPComment == false && inCharQuotes == false)
					{
						if (file->currentTokenIndex != 0)
						{
							updateCurrentToken(file);
						}
						
						inIncludeDefine = true;
					}
					
					file->currentToken->text[file->currentTokenTextIndex] = file->data[j][i];
					file->currentTokenTextIndex = file->currentTokenTextIndex + 1;
					break;

				case '/':

					if (inQuotes == false && inANSIComment == false && inCPPComment == false && inIncludeDefine == false)
					{
						if (file->currentTokenIndex != 0)
						{
							updateCurrentToken(file);
						}

						/* We are reading the start of an ANSI comment. */
						if (file->data[j][i + 1] == '*')
						{
							inANSIComment = true;
						}
						/* We are reading the start of a c99 comment. */
						else if (file->data[j][i + 1] == '/')
						{
							inCPPComment = true;
						}
					}
					else if (inANSIComment == true)
					{
						if (i != 0)
						{
							/* End of ANSI comment. */
							if (file->data[j][i - 1] == '*')
							{
								file->currentToken->text[file->currentTokenTextIndex] = file->data[j][i];
								file->currentTokenTextIndex = file->currentTokenTextIndex + 1;
								inANSIComment = false;
								break;
							}
						}
					}

					file->currentToken->text[file->currentTokenTextIndex] = file->data[j][i];
					file->currentTokenTextIndex = file->currentTokenTextIndex + 1;
					break;

				case '"':

					if (inQuotes == false && inANSIComment == false && inCPPComment == false && inCharQuotes == false)
					{
						if (file->currentTokenIndex != 0)
						{
							updateCurrentToken(file);
						}
						
						inQuotes = true;
					}
					else if (inQuotes == true)
					{
						if (i != 0)
						{
							/* End of string. */
							if (file->data[j][i - 1] != '\\')
							{
								file->currentToken->text[file->currentTokenTextIndex] = file->data[j][i];
								file->currentTokenTextIndex = file->currentTokenTextIndex + 1;
								inQuotes = false;
								break;
							}
						}
					}
					
					file->currentToken->text[file->currentTokenTextIndex] = file->data[j][i];
					file->currentTokenTextIndex = file->currentTokenTextIndex + 1;
					break;

				/* CHARACTER IS PUNCTUATION: Treat as new token, unless not in code. */
				case ',':
				case ':':
				case ';':
				case '-':
				case '+':
				case '&':
				case '|':
				case '*':
				case '.':
				case '(':
				case ')':
				case '[':
				case ']':
				case '{':
				case '}':

					/* We are in code. This indicates that there is a new token. */
					if (inQuotes == false && inANSIComment == false && inCPPComment == false && inIncludeDefine == false)
					{
						if (file->currentTokenTextIndex != 0)
						{
							/* There is no syntactically correct way to start a program source file with a punctuation character. */
							updateCurrentToken(file);
						}
					}

					file->currentToken->text[file->currentTokenTextIndex] = file->data[j][i];
					file->currentTokenTextIndex = file->currentTokenTextIndex + 1;
					break;

				/* CHARACTER IS WHITESPACE: append to the current token's whitespace attribute. */
				case ' ':
				case '\t':

					if (inQuotes == false && inANSIComment == false && inCPPComment == false && inIncludeDefine == false)
					{
						/* Add current character to token's whitespace attribute. */
						file->currentToken->postwhitespace[file->currentTokenWhitespaceIndex] = file->data[j][i];
						file->currentTokenWhitespaceIndex = file->currentTokenWhitespaceIndex + 1;
						break;
					}
					
					/* Add current character to token, if in a string, comment, or preprocessor directive. */
					file->currentToken->text[file->currentTokenTextIndex] = file->data[j][i];
					file->currentTokenTextIndex = file->currentTokenTextIndex + 1;
					break;

				case '\n':

					if (inQuotes == false && inANSIComment == false && inCPPComment == false && inIncludeDefine == false)
					{
						/* Add current character to token's whitespace attribute. */
						file->currentToken->postwhitespace[file->currentTokenWhitespaceIndex] = file->data[j][i];
						file->currentTokenWhitespaceIndex = file->currentTokenWhitespaceIndex + 1;
						break;
					}

					/* End of C99-style comment. They span from the double slash ("//") to the end-of-line. */
					if (inCPPComment == true)
					{
						inCPPComment = false;
					}
					/* End of #include/#define statement. They span from the hashtag ('#') to the end-of-line. */
					else if (inIncludeDefine == true)
					{
						inIncludeDefine = false;
					}

					/* Add current character to token, if in a string, comment, or preprocessor directive. */
					file->currentToken->text[file->currentTokenTextIndex] = file->data[j][i];
					file->currentTokenTextIndex = file->currentTokenTextIndex + 1;
					break;

				/* default case: regular character (all letters, numbers, and other insignificant characters), no special steps needed. */
				default:

					if (inQuotes == false && inANSIComment == false && inCPPComment == false && inIncludeDefine == false)
					{
						if (file->currentTokenWhitespaceIndex != 0)
						{
							/* There was whitespace before encountering this character, therefore, it must be a new token. */
							updateCurrentToken(file);
						}
						else if (file->currentTokenTextIndex != 0)
						{
							if (isalpha(file->currentToken->text[file->currentTokenTextIndex - 1]) == false && isdigit(file->currentToken->text[file->currentTokenTextIndex - 1]) == false)
							{
								/* There was punctuation before encountering this character, therefore, it must be a new token. */
								updateCurrentToken(file);
							}
						}
					}

					file->currentToken->text[file->currentTokenTextIndex] = file->data[j][i];
					file->currentTokenTextIndex = file->currentTokenTextIndex + 1;
					break;
			}
		}
	}

	/* Necessary to properly store the last token. */
	file->currentToken->text[file->currentTokenTextIndex] = '\0';
	file->currentToken->postwhitespace[file->currentTokenWhitespaceIndex] = '\0';

	freeInputFileData(file, 1);
}

void updateCurrentToken(InputFile* file)
{
	/* More tokens needed. Allocate more memory for extra tokens. Resize token array. */
	file->numTokens++;
	file->tokens = realloc(file->tokens, sizeof(Token*) * file->numTokens);

	/* New token. Terminate current token correctly, and create new one. */
	file->currentToken->text[file->currentTokenTextIndex] = '\0';
	file->currentToken->postwhitespace[file->currentTokenWhitespaceIndex] = '\0';

	file->currentTokenIndex = file->currentTokenIndex + 1;
	file->tokens[file->currentTokenIndex] = calloc(1, sizeof(Token));
	file->currentToken = file->tokens[file->currentTokenIndex];
	file->currentToken->text = calloc(LINE_SIZE, sizeof(char));
	file->currentToken->postwhitespace = calloc(LINE_SIZE, sizeof(char));
	file->currentTokenTextIndex = 0;
	file->currentTokenWhitespaceIndex = 0;
}

void freeInputFileData(InputFile* file, int phase)
{
	switch (phase)
	{
		int i;
		int j;

		/* Free data needed for part 1 of the program runtime: the parser. */
		case 1:

			for (i = 0; i < file->numLines; i++)
			{
				if (file->data[i] == NULL) break;
				else free(file->data[i]);
			}
			free(file->data);
			break;

		case 2:

			/* Free file tokens. */
			for (i = 0; i < file->numTokens; i++)
			{
				if (file->tokens[i] == NULL) break;
				else 
				{
					if (file->tokens[i]->text != NULL) free(file->tokens[i]->text);
					if (file->tokens[i]->postwhitespace != NULL) free(file->tokens[i]->postwhitespace);
				}
			}

			for (i = 0; i < file->numClasses; i++)
			{
				if (file->classes[i] != NULL)
				{
					/* Free functions. */
					for (j = 0; j < file->classes[i]->numFunctions; j++)
					{
						if (file->classes[i]->functions != NULL)
						{
							if (file->classes[i]->functions[j] != NULL) 
							{
								/*if (file->classes[i]->functions[j]->tokens != NULL) free(file->classes[i]->functions[j]->tokens);
								else break;*/
								/*free(file->classes[i]->functions[j]);*/
							}
							else break;

							/*free(file->classes[i]->functions);*/
						}
						else break;
					}
					free(file->classes[i]);
				}
				else break;
			}
			for (i = 0; i < file->numClasses; i++)
			{
				if (file->classes[i] != NULL)
				{
					/* Free variables. */
					for (j = 0; j < file->classes[i]->numVariables; j++)
					{
						if (file->classes[i]->variables != NULL)
						{
							if (file->classes[i]->variables[j] != NULL) free(file->classes[i]->variables[j]);
							else break;

							/*free(file->classes[i]->variables);*/
						}
						else break;
					}
				}
			}
			for (i = 0; i < file->numClasses; i++)
			{
				if (file->classes[i] != NULL)
				{
					/* Free class variables. */
					for (j = 0; j < file->classes[i]->numClassVariables; j++)
					{
						if (file->classes[i]->classVariables != NULL)
						{
							if (file->classes[i]->classVariables[j] != NULL) free(file->classes[i]->classVariables[j]);
							else break;

							/*free(file->classes[i]->classVariables);*/
						}
						else break;
					}
				}
			}
			free(file->classes);
			break;
	}
}

bool isType(Token* token)
{
	/* Is the provided token a primitive type? */
	if (strcmp(token->text, "int") == 0 || strcmp(token->text, "double") == 0 || strcmp(token->text, "float") == 0
		|| strcmp(token->text, "char") == 0 || strcmp(token->text, "byte") == 0 || strcmp(token->text, "long") == 0 || strcmp(token->text, "short") == 0)
	{
		return true;
	}
	return false;
}

void outputCFile(InputFile* file)
{
	/* Make the length of this string the same as that of the input file's name. "Remove" last character. This gives the output file name. */
	char* outputName = calloc(strlen(file->filename) + 1, sizeof(char));
	if (outputName == NULL)
	{
		printf("\nNot enough memory! Exiting...\n");
		exit(0);
	}
	memcpy(outputName, file->filename, strlen(file->filename) + 1);
	outputName[strlen(outputName) - 1] = '\0';

	FILE* outputFile = fopen(outputName, "w+");
	if (outputFile == NULL)
	{
		printf("\nSystem denied file-write access! Exiting...\n");
		exit(0);
	}

	/* Some important variables. */
	bool inClass = false;
	bool inClassFunction = false;

	file->numClasses = 1;
	file->classes = calloc(file->numClasses, sizeof(classData*));
	file->classes[0] = calloc(1, sizeof(classData));
	file->currentClass = file->classes[0];
	strcpy(file->currentClass->type, "");
	file->currentClass->numVariables = 1;
	file->currentClass->variables = calloc(file->currentClass->numVariables, sizeof(Token*));
	file->currentClass->numClassVariables = 1;
	file->currentClass->classVariables = calloc(file->currentClass->numClassVariables, sizeof(Token*));
	file->currentClass->numFunctions = 1;
	file->currentClass->functions = calloc(file->currentClass->numFunctions, sizeof(Function*));
	file->currentClass->functions[0] = calloc(1, sizeof(Function));
	file->currentClass->functions[0]->numTokens = 1;
	file->currentClass->functions[0]->tokens = calloc(file->currentClass->functions[0]->numTokens, sizeof(Token*));
	file->currentClass->functions[0]->tokens[0] = NULL;
	file->currentClass->functions[0]->argIndex = 0;

	/* Iterate through the token array, depending on certain aspects of the token, we may wish to do something else. */
	int i;
	for (i = 0; i < file->numTokens - 1; i++)
	{
		if (file->tokens[i] != NULL)
		{
			if (file->tokens[i]->text != NULL)
			{
				/* Keyword "class" is found. */
				if (strcmp(file->tokens[i]->text, "class") == 0)
				{
					/* Replace the text content of that token with "struct" */
					strcpy(file->tokens[i]->text, "struct");

					/* What is the next-next token? Is it a "{"? If so, it's a class definition. */
					if (strcmp(file->tokens[i + 2]->text, "{") == 0)
					{
						/* We are about to enter a class definition. Store information about class. */
						inClass = true;
						strcpy(file->currentClass->type, file->tokens[i + 1]->text);

						fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
						fprintf(outputFile, "%s%s", file->tokens[i + 1]->text, file->tokens[i + 1]->postwhitespace);
						fprintf(outputFile, "%s%s", file->tokens[i + 2]->text, file->tokens[i + 2]->postwhitespace);
						i += 2;
						continue;
					}
					/* If it's a ";", a class variable declaration was found. */
					else if (strcmp(file->tokens[i + 3]->text, ";") == 0)
					{
						/* Print out the tokens, plus constructor. */
						fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
						fprintf(outputFile, "%s%s", file->tokens[i + 1]->text, file->tokens[i + 1]->postwhitespace);
						fprintf(outputFile, "%s%s", file->tokens[i + 2]->text, file->tokens[i + 2]->postwhitespace);
						fprintf(outputFile, "%s%s", file->tokens[i + 3]->text, file->tokens[i + 3]->postwhitespace);
						
						/* Print the constructor for that variable. */
						fprintf(outputFile, "\nconstructor%s(&%s);\n", file->tokens[i + 1]->text, file->tokens[i + 2]->text);

						/* Add the name of the created variable to its corresponding class list. */
						int j;
						for (j = 0; j < file->numClasses; j++)
						{
							if (strcmp(file->tokens[i + 1]->text, file->classes[j]->type) == 0)
							{
								/* Type matched. Add variable to list. */
								file->classes[j]->variables[file->classes[j]->numVariables - 1] = file->tokens[i + 1];
								file->classes[j]->numVariables = file->classes[j]->numVariables + 1;
								file->classes[j]->variables = realloc(file->classes[j]->variables, sizeof(Token*) * file->classes[j]->numVariables);
							}
						}
						i += 3;
						continue;
					}
				}
				if (inClass == false)
				{
					/* Token is a comment (c99 or ANSI) or preprocessor directive, just output it. */
					if (file->tokens[i]->text[0] == '/' || file->tokens[i]->text[0] == '#' || file->tokens[i]->text[0] == '"')
					{
						fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
						continue;
					}
					/* Is it a primitive type outside of a class? Print it! */
					if (isType(file->tokens[i]) == true  || strcmp(file->tokens[i]->text, "void") == 0)
					{
						/* What is the next-next token? If ";", it is a variable declaration. Output! */
						if (strcmp(file->tokens[i + 2]->text, ";") == 0)
						{
							fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
							fprintf(outputFile, "%s%s", file->tokens[i + 1]->text, file->tokens[i + 1]->postwhitespace);
							fprintf(outputFile, "%s%s", file->tokens[i + 2]->text, file->tokens[i + 2]->postwhitespace);
							i += 2;
							continue;
						}
						else if (strcmp(file->tokens[i + 2]->text, ",") == 0)
						{
							/* Multiple variable declarations. Example: int a, b, c; */
							while (strcmp(file->tokens[i]->text, ";") != 0)
							{
								fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
								i++;
							}
							fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
							continue;
						}
						/* Variable declaration is an array */
						else if (strcmp(file->tokens[i + 2]->text, "[") == 0)
						{
							while (file->tokens[i]->text[0] != ']')
							{
								fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
								i++;
							}
							fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
							continue;
						}
						/* Is it an external function? */
						else if (strcmp(file->tokens[i + 2]->text, "(") == 0)
						{
							/* If so, output the header. */							
							while (file->tokens[i]->text[0] != ')')
							{
								fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
								i++;
							}
							fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
							continue;
						}
					}
					else if (strcmp(file->tokens[i]->text, "class") != 0)
					{
						fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
						continue;
					}
				}
				if (inClass == true)
				{
					if (file->tokens[i]->text[0] == '}')
					{
						if (inClassFunction == true)
						{
							inClassFunction = false;
						}
						else if (inClass == true && file->tokens[i + 1]->text[0] == ';')
						{
							fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
							fprintf(outputFile, "%s%s", file->tokens[i + 1]->text, file->tokens[i + 1]->postwhitespace);
							inClass = false;
							printClassFunctions(file, outputFile);
							updateCurrentClass(file);
						}
					}
					/* Is it a primitive type? */
					if (isType(file->tokens[i]) || strcmp(file->tokens[i]->text, "void") == 0)
					{
						char functionType[10];
						strcpy(functionType, file->tokens[i]->text);

						/* Is it a class function? */
						if (strcmp(file->tokens[i + 2]->text, "(") == 0)
						{
							/* If so, we must store the entire header and everything into a class function struct. */							
							inClassFunction = true;
							while (file->tokens[i + 1]->text[0] != '}')
							{
								file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[file->currentClass->functions[file->currentClass->numFunctions - 1]->numTokens - 1] = file->tokens[i];
								file->currentClass->functions[file->currentClass->numFunctions - 1]->numTokens++;
								file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens = realloc(file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens, sizeof(Token*) * file->currentClass->functions[file->currentClass->numFunctions-1]->numTokens);
								file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[file->currentClass->functions[file->currentClass->numFunctions - 1]->numTokens - 1] = NULL;
								i++;
							}
							
							/* Update current function */
							condenseFunctionHeader(file);
							strcpy(file->currentClass->functions[file->currentClass->numFunctions - 1]->altName, file->currentClass->type);
							strcat(file->currentClass->functions[file->currentClass->numFunctions - 1]->altName, file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[1]->text);
							strcat(file->currentClass->functions[file->currentClass->numFunctions - 1]->altName, file->currentClass->functions[file->currentClass->numFunctions - 1]->argList);
							fprintf(outputFile, "\t%s (*%s)(", functionType, file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[1]->text);
							/* Print the parameters needed for the struct function pointer. */
							int k;
							for (k = 0; k < strlen(file->currentClass->functions[file->currentClass->numFunctions - 1]->argList); k++)
							{
								if (file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[k] == 'i') /* int */
								{
									fprintf(outputFile, "int");
								}
								else if (file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[k] == 'd') /* double */
								{
									fprintf(outputFile, "double");
								}
								else if (file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[k] == 'f') /* float */
								{
									fprintf(outputFile, "float");
								}
								else if (file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[k] == 'c') /* char */
								{
									fprintf(outputFile, "char*");
								}
								else if (file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[k] == 'b') /* byte */
								{
									fprintf(outputFile, "byte");
								}
								else if (file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[k] == 's') /* short */
								{
									fprintf(outputFile, "short");
								}
								else if (file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[k] == 'l') /* long */
								{
									fprintf(outputFile, "long");
								}
								else if (file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[k] == 'u') /* long */
								{
									fprintf(outputFile, "UserPost*");
								}

								if (k != strlen(file->currentClass->functions[file->currentClass->numFunctions - 1]->argList) - 1)
								{
									fprintf(outputFile, ", ");
								}
							}
							fprintf(outputFile, ");\n");
							updateCurrentFunction(file);
							continue;
						}
						/* Class variable declaration is an array. Output and store. */
						else if (strcmp(file->tokens[i + 2]->text, "[") == 0)
						{
							while (file->tokens[i]->text[0] != ']')
							{
								fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
								file->currentClass->classVariables[file->currentClass->numClassVariables - 1] = file->tokens[i + 1];
								file->currentClass->numClassVariables = file->currentClass->numClassVariables + 1;
								file->currentClass->classVariables = realloc(file->currentClass->classVariables, sizeof(Token*) * file->currentClass->numClassVariables);
								i++;
							}
							fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
							continue;
						}
						/* What is the next-next token? If ";", it is a variable declaration. Output and store! */
						else if (strcmp(file->tokens[i + 2]->text, ";") == 0)
						{
							fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
							fprintf(outputFile, "%s%s", file->tokens[i + 1]->text, file->tokens[i + 1]->postwhitespace);
							fprintf(outputFile, "%s%s", file->tokens[i + 2]->text, file->tokens[i + 2]->postwhitespace);
							file->currentClass->classVariables[file->currentClass->numClassVariables - 1] = file->tokens[i + 1];
							file->currentClass->numClassVariables = file->currentClass->numClassVariables + 1;
							file->currentClass->classVariables = realloc(file->currentClass->classVariables, sizeof(Token*) * file->currentClass->numClassVariables);

							i += 2;
							continue;
						}
						else if (strcmp(file->tokens[i + 2]->text, ",") == 0)
						{
							/* Multiple variable declarations. Example: int a, b, c; */
							while (strcmp(file->tokens[i]->text, ";") != 0)
							{
								fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
								/* Do not store commas as class variables. */
								if (strcmp(file->tokens[i]->text, ",") != 0)
								{
									file->currentClass->classVariables[file->currentClass->numClassVariables - 1] = file->tokens[i];
									file->currentClass->numClassVariables = file->currentClass->numClassVariables + 1;
									file->currentClass->classVariables = realloc(file->currentClass->classVariables, sizeof(Token*) * file->currentClass->numClassVariables);
								}
								i++;
							}
							fprintf(outputFile, "%s%s", file->tokens[i]->text, file->tokens[i]->postwhitespace);
							continue;
						}
					}
				}
			}
			else break;
		}
		else break;
	}
	fprintf(outputFile, "\n}\n");
	fclose(outputFile);
}

void updateCurrentFunction(InputFile* file)
{
	/* More functions needed. Resize Function array. */
	file->currentClass->numFunctions++;
	file->currentClass->functions = realloc(file->currentClass->functions, sizeof(Function*) * file->currentClass->numFunctions);

	/* Create new Function instance, make it current. */
	file->currentClass->functions[file->currentClass->numFunctions - 1] = calloc(1, sizeof(Function));
	file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex = 0;
	file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex] = '\0';

	file->currentClass->functions[file->currentClass->numFunctions - 1]->numTokens = 1;
	file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens = calloc(file->currentClass->functions[0]->numTokens, sizeof(Token*));
	file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[0] = NULL;
}

void condenseFunctionHeader(InputFile* file)
{
	/* Read in the first letter of every parameter type in a function header. Store it in argList. */
	int i, openParentheses;

	/* Look for the index of the first parentheses. */
	for (i = 0; file->currentClass->functions[file->currentClass->numFunctions - 1]->numTokens; i++)
	{
		if (file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[i]->text[0] == '(')
		{
			openParentheses = i;
			break;
		}
	}

	/* Start at opening parentheses, end when we find the closing one. */
	for (i = openParentheses; file->currentClass->functions[file->currentClass->numFunctions - 1]->numTokens; i++)
	{
		if (file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[i]->text[0] == ')') 
		{
			break;
		}
		/* Store the first letter of the parameter type */
		else if (strcmp(file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[i]->text, "int") == 0)
		{
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex] = 'i';
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex++;
		}
		else if (strcmp(file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[i]->text, "double") == 0)
		{
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex] = 'd';
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex++;
		}
		else if (strcmp(file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[i]->text, "float") == 0)
		{
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex] = 'f';
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex++;
		}
		else if (strcmp(file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[i]->text, "char") == 0)
		{
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex] = 'c';
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex++;
		}
		else if (strcmp(file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[i]->text, "short") == 0)
		{
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex] = 's';
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex++;
		}
		else if (strcmp(file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[i]->text, "long") == 0)
		{
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex] = 'l';
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex++;
		}
		else if (strcmp(file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[i]->text, "byte") == 0)
		{
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex] = 'b';
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex++;
		}
		else if (strcmp(file->currentClass->functions[file->currentClass->numFunctions - 1]->tokens[i]->text, "UserPost") == 0)
		{
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex] = 'u';
			file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex++;
		}
	}
	/* Terminate the string. */
	file->currentClass->functions[file->currentClass->numFunctions - 1]->argList[file->currentClass->functions[file->currentClass->numFunctions - 1]->argIndex] = '\0';
}

void updateCurrentClass(InputFile* file)
{
	/* More classes needed. Resize class array. */
	file->numClasses++;
	file->classes = realloc(file->classes, sizeof(classData*) * file->numClasses);

	file->currentTokenIndex = file->currentTokenIndex + 1;
	file->classes[file->numClasses - 1] = calloc(1, sizeof(classData));
	file->currentClass = file->classes[file->numClasses - 1];
	strcpy(file->currentClass->type, "");
	file->currentClass->numVariables = 1;
	file->currentClass->variables = calloc(file->currentClass->numVariables, sizeof(Token*));
	file->currentClass->numClassVariables = 1;
	file->currentClass->classVariables = calloc(file->currentClass->numClassVariables, sizeof(Token*));
	file->currentClass->numFunctions = 1;
	file->currentClass->functions = calloc(file->currentClass->numFunctions, sizeof(Function*));
	file->currentClass->functions[0] = calloc(1, sizeof(Function));
	file->currentClass->functions[0]->numTokens = 1;
	file->currentClass->functions[0]->tokens = calloc(file->currentClass->functions[0]->numTokens, sizeof(Token*));
	file->currentClass->functions[0]->tokens[0] = NULL;
	file->currentClass->functions[0]->argIndex = 0;
}

void printClassFunctions(InputFile* file, FILE* outputFile)
{
	/* Iterate through the current class functions, printing/modifying them as necessary. */
	int i;
	int j;
	for (i = 0; i < file->currentClass->numFunctions - 1; i++)
	{
		fprintf(outputFile, "\n");
		for (j = 0; j <= file->currentClass->functions[i]->numTokens; j++)
		{
			if (file->currentClass->functions[i] != NULL)
			{
				if (file->currentClass->functions[i]->tokens != NULL)
				{
					if (file->currentClass->functions[i]->tokens[j] != NULL)
					{
						if (file->currentClass->functions[i]->tokens[j]->text != NULL && file->currentClass->functions[i]->tokens[j]->postwhitespace != NULL)
						{
							fprintf(outputFile, "%s%s", file->currentClass->functions[i]->tokens[j]->text, file->currentClass->functions[i]->tokens[j]->postwhitespace);
						}
						else break;
					}
					else break;
				}
				else break;
			}
			else break;
		}
		if (j != file->currentClass->functions[i]->numTokens - 2)
		{
			fprintf(outputFile, ";\n}\n");
		}
	}

	/* Print constructor definition. */
	fprintf(outputFile, "void constructor%s(struct %s* my%s)\n{\n", file->currentClass->type, file->currentClass->type, file->currentClass->type);
	for (i = 0; i < file->currentClass->numFunctions; i++)
	{
		if (strcmp(file->currentClass->functions[i]->altName, "") != 0)
		{
			fprintf(outputFile, "\tmy%s->%s = %s;\n", file->currentClass->type, file->currentClass->functions[i]->tokens[1]->text, file->currentClass->functions[i]->tokens[1]->text);
		}
	}
	fprintf(outputFile, "\treturn;\n}");
}