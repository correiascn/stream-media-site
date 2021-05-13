/*
CIS*2750
Assignment 4 - Message Management and Viewer (Online with database)
Nelson Correia
0930748
correian@mail.uoguelph.ca
19 March 2017
Handles a message posting and viewing application, uses a database.
See README and A4 specs for limitations and usage.
*/

/* Please see the header files for function tags! */

/* Importing standard C libraries. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

/* Including user-defined libraries. */
#include "stream.h"

int main (int argc, char* argv[])
{
	char desired_streams[601];
	strcpy(desired_streams, argv[1]); /* Copy second argument into desired_streams. */
	assert(desired_streams != NULL);

	bool to_remove = false;

	/* Error check for invalid input. */
	if (argc < 3)
	{
		/* At least 3 arguments needed (4 for remove). */
		printf("\nINTERNAL ERROR\nNot enough command line arguments provided! Exiting program.\n\n");
		int i;
		for (i = 0; i < argc; i++)
		{
			printf("%s ", argv[i]);
		}
		printf("\n");
		exit(0);
	}
	else if (strcmp(argv[2], "-r") == 0) /* second last argument is remove or add. */
	{
		if (argc == 3)
		{
			/* No author specified. Bad input. */
			printf("\nINTERNAL ERROR\nNo author for removal specified! Exiting program.\n\n");
			exit(0);
		}
		/* User is choosing to remove a user. Valid case this time. */
		else to_remove = true;
	}

	/* Tokenize the stream list. */
	char* streams = strtok(desired_streams, ",");
	assert(streams != NULL);
	char username[MAX_USERNAME_LENGTH + 1];

	/* If the username contains spaces, we must concatenate all arguments after "-r" (if it exists) into a single string. */
	if (strcmp(argv[2], "-r") == 0)
	{
		strcpy(username, argv[3]);
		if (argc > 4)
		{
			int i;
			for (i = 4; i < argc; i++)
			{
				strcat(username, " ");
				strcat(username, argv[i]);
			}
		}
	}
	/* -r not specified. */
	else
	{
		strcpy(username, argv[2]);
		if (argc > 3)
		{
			int i;
			for (i = 3; i < argc; i++)
			{
				strcat(username, " ");
				strcat(username, argv[i]);
			}
		}
	}

	/* Seperately store tokens. */
	int counter = 0;
	char toks[20][MAX_STREAMNAME_LENGTH + 1];
	while (streams != NULL)
	{
		strcpy(toks[counter], streams);
		counter++;
		streams = strtok(NULL, ",");
	}

	int c;
	for (c = 0; c < counter; c++)
	{
		bool valid = true;
		
		if (strcmp(toks[c], "all") == 0)
		{
			printf("Stream name [all] is prohibited!\n");
			valid = false;
		}
		else
		{
			int j;
			for (j = 0; j < strlen(toks[c]); j++)
			{
				if (toks[c][j] == ' ')
				{
					printf("Stream names containing spaces are prohibited!\n");
					valid = false;
				}
			}
		}

		if (valid == true)
		{
			if (to_remove == true)
			{
				/* Remove author from specified stream. */
				removeUser(username, toks[c]);
			}
			else
			{
				/* Add author from specified stream. */
				addUser(username, toks[c]);
			}
		}
	}
	
	return 0;
}