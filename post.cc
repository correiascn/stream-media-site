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
#include <time.h>
#include <assert.h>
#include <mysql/mysql.h>

/* Including user-defined libraries. */
#include "stream.h"

#define character char

void promptInput (char* username, char* streamname, char* user_input)
{
	/* Check if specified stream exists. If it does not, exit. Also check if user is included. If not, exit for that as well. */
	sql_vars vars;
	mysql_init(&(vars.mysql));
	mysql_options(&(vars.mysql), MYSQL_READ_DEFAULT_GROUP, "mydb");
	if (!mysql_real_connect(&(vars.mysql), HOSTNAME, USERNAME, PASSWORD, DATABASE, 0, NULL, 0))
	{
		/* Connection to host failed. */
		perror_sql("Could not connect to host.\n", &(vars.mysql));
		printf("\n");
	}

	bool stream_exists = false;
	character full_streamname[MAX_STREAMNAME_LENGTH + strlen("StreamUsers") + 1];
	sprintf(full_streamname, "%sStreamUsers", streamname);

	/* Get all tables and compare their names. */
	strcpy(vars.query, "show tables");
	if(mysql_query(&(vars.mysql), vars.query))
	{
		printf("Attempted Query: %s\n", vars.query);
		perror_sql("", &(vars.mysql));
	}
	if (!(vars.result = mysql_store_result(&(vars.mysql))))
	{
		printf("Attempted Query: %s\n", vars.query);
		perror_sql("Failed to store results in result.\n", &(vars.mysql));
	}

	/* Iterate through results. */
	while ((vars.row = mysql_fetch_row(vars.result)))
	{
		int i;
		for (i = 0; i < mysql_num_fields(vars.result); i++)
		{
			if (strcmp(vars.row[i], full_streamname) == 0)
			{
				/* Match found. Stream table exists. */
				stream_exists = true;
			}
		}
	}
	if (stream_exists == true)
	{
		sprintf(vars.query, "select 1 from %s where author = '%s'", full_streamname, username);
		if(mysql_query(&(vars.mysql), vars.query))
		{
			printf("Attempted Query: %s\n", vars.query);
			perror_sql("", &(vars.mysql));
		}

		/* Store results from query into result structure. */
		if (!(vars.result = mysql_store_result(&(vars.mysql))))
		{
			printf("Attempted Query: %s\n", vars.query);
			perror_sql("Failed to store results in result.\n", &(vars.mysql));
		}

		/* If 0, nothing was found, ergo, the user is not in that stream. */
		if (mysql_num_rows(vars.result) == 0)
		{
			printf("Could not post to stream [%s] because \"%s\" is not subscribed to that Stream!\n", streamname, username);
			terminate_sql(vars);
		}
	}
	else
	{
		printf("Could not print to stream [%s] because it does not exist!\n", streamname);
		terminate_sql(vars);
	}

	mysql_close(&(vars.mysql));
	mysql_library_end();
	return;
}

class PostEntry {

	void getTimeDate(char* str) /* determines the current time and date and formats it as a string */
	{
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		char date[30];

		sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
		strcpy(str, date);
		return;
	}

	void submitPost(UserPost * userpost) /* calls the updateStream() function */
	{
		updateStream(userpost);
		printf("Post made!\n");

		/* Free data used by the UserPost object. */
		free(userpost->username);
		free(userpost->streamname);
		free(userpost->date);
		free(userpost->text);

		return;
	}

	void formatEntry(UserPost * userpost, char* username, char* streamname, char* user_input) /* creates the struct used by the updateStream() function */
	{
		userpost->date = malloc(sizeof(char) * (30));
		assert(userpost->date != NULL);

		/* See stream.h for constant values. */
		userpost->streamname = malloc(sizeof(char) * (MAX_STREAMNAME_LENGTH + 1));
		assert(userpost->streamname != NULL);
		userpost->username = malloc(sizeof(char) * (MAX_USERNAME_LENGTH + 1));
		assert(userpost->username != NULL);
		userpost->text = malloc(sizeof(char) * (CHARACTERS_PER_POST + 1));
		assert(userpost->text != NULL);
		
		getTimeDate(userpost->date);
		strcpy(userpost->username, username);
		strcpy(userpost->streamname, streamname);
		strcpy(userpost->text, user_input);

		submitPost(userpost);
		return;
	}

	void readInput(char* username, char* streamname, char* input) /* reads the user's input to the post program */
	{
		promptInput(username, streamname, input);
		UserPost userpost;
		formatEntry(&userpost, username, streamname, input);
		return;
	}
};

int main (int argc, char* argv[])
{
	/* Input Format: ./post $stream $username -n $post_text */

	/* Error check for invalid input. */
	if (argc < 5)
	{
		printf("\nInsufficient arguments provided! Exiting program.\n\n");
		printf("argc = %d\n", argc);
		printf("args = ");
		int p;
		for (p = 0; p < argc; p++)
		{
			printf("%s ", argv[p]);
		}
		printf("\n");
		exit(1);
	}

	char streamname[MAX_STREAMNAME_LENGTH + 1];
	strcpy(streamname, argv[1]);

	char username[MAX_USERNAME_LENGTH + 1];
	strcpy(username, argv[2]);

	char input[CHARACTERS_PER_POST + 1];
	strcpy(input, "");

	if (argc > 3)
	{
		int i;
		for (i = 3; i < argc; i++)
		{
			/* Stop reading name once the -n flag is encountered. */
			if (strcmp(argv[i], "-n") != 0)
			{
				strcat(username, " ");
				strcat(username, argv[i]);
			}
			else 
			{
				int j;
				for (j = i + 1; j < argc; j++)
				{
					if (j != i + 1) strcat(input, " ");
					strcat(input, argv[j]);
				}
				break;
			}
		}
	}

	class PostEntry newpost;
	newpost.readInput(username, streamname, input);
	
	return 0;
}