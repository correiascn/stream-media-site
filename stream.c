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
#include <mysql/mysql.h>

/* Including user-defined libraries. */
#include "stream.h"

void perror_sql(char* msg, MYSQL* mysql)
{
	/* Print the user's message (msg), as well as the error that mySQL provides. */
	printf("%s\n%s\n", msg, mysql_error(mysql));
	mysql_close(mysql);
	mysql_library_end();
	exit(1);
}

void clrstr(char *buf)
{
	/* Immediately terminating the string will suffice as "clearing". */
	buf[0] = '\0';
}

void updateStream(UserPost* userpost)
{
	assert(userpost != NULL);
	sql_vars vars;
	mysql_init(&(vars.mysql));
	mysql_options(&(vars.mysql), MYSQL_READ_DEFAULT_GROUP, "mydb");
	if (!mysql_real_connect(&(vars.mysql), HOSTNAME, USERNAME, PASSWORD, DATABASE, 0, NULL, 0))
	{
		/* Connection to host failed. */
		perror_sql("Could not connect to host.\n", &(vars.mysql));
	}

	/* Check if the desired stream exists. */
	bool stream_exists = false;
	char streamname[MAX_STREAMNAME_LENGTH + strlen("Stream") + 1];
	sprintf(streamname, "%sStream", userpost->streamname);

	/* Get all tables and compare their names. */
	strcpy(vars.query, "show tables");
	if(mysql_query(&(vars.mysql), vars.query))
	{
		perror_sql("Command failed: show tables\n", &(vars.mysql));
	}

	/* Store results from query into result structure. */
	if (!(vars.result = mysql_store_result(&(vars.mysql))))
	{
		perror_sql("Failed to store results in result.\n", &(vars.mysql));
	}

	/* Iterate through results. */
	while ((vars.row = mysql_fetch_row(vars.result)))
	{
		int i;
		for (i = 0; i < mysql_num_fields(vars.result); i++)
		{
			if (strcmp(vars.row[i], streamname) == 0)
			{
				/* Match found. Stream table already exists. */
				stream_exists = true;
			}
		}
	}

	if (stream_exists == false)
	{
		/* Create the table for that stream. */
		sprintf(vars.query, "create table %s (id int not null auto_increment, author char(%d), date char(%d), post text(%d), primary key(id))", streamname, MAX_USERNAME_LENGTH, 30, CHARACTERS_PER_POST);
		if(mysql_query(&(vars.mysql), vars.query))
		{
			perror_sql("Command failed: Stream table could not be created.\n", &(vars.mysql));
		}
		clrstr(vars.query);
	}
	
	/* Insert record into table. Auto-increment will handle the ID. */
	sprintf(vars.query, "insert into %s values (0, '%s', '%s', '%s')", streamname, userpost->username, userpost->date, userpost->text);
	if(mysql_query(&(vars.mysql), vars.query))
	{
		printf("Attempted Query: %s\n", vars.query);
		perror_sql("", &(vars.mysql));
	}
	return;
}

void addUser(char* username, char* list)
{
	/* Note: although this function's header has 'list' as the second argument, in reality, it is a single stream that has already been parsed. */
	assert(username != NULL && list != NULL);
	sql_vars vars;
	mysql_init(&(vars.mysql));
	mysql_options(&(vars.mysql), MYSQL_READ_DEFAULT_GROUP, "mydb");
	if (!mysql_real_connect(&(vars.mysql), HOSTNAME, USERNAME, PASSWORD, DATABASE, 0, NULL, 0))
	{
		/* Connection to host failed. */
		perror_sql("Could not connect to host.\n", &(vars.mysql));
	}

	/* First, open the file for reading to check if the author was already included. */
	char streamUsers_directory[MAX_STREAMNAME_LENGTH + strlen("StreamUsers") + 1]; /* +1 for \0. */
	strcpy(streamUsers_directory, list);
	strcat(streamUsers_directory, "StreamUsers");
	
	/* First, check if the stream exists. If it does not exist, then duplicate author entry is not possible. */
	bool stream_exists = false;

	/* Get all tables and compare their names. */
	strcpy(vars.query, "show tables");
	if(mysql_query(&(vars.mysql), vars.query))
	{
		perror_sql("Command failed: show tables\n", &(vars.mysql));
	}

	/* Store results from query into result structure. */
	if (!(vars.result = mysql_store_result(&(vars.mysql))))
	{
		perror_sql("Failed to store results in result.\n", &(vars.mysql));
	}

	/* Iterate through results. */
	while ((vars.row = mysql_fetch_row(vars.result)))
	{
		int i;
		for (i = 0; i < mysql_num_fields(vars.result); i++)
		{
			if (strcmp(vars.row[i], streamUsers_directory) == 0)
			{
				/* Match found. Stream table already exists. */
				stream_exists = true;
			}
		}
	}

	if (stream_exists == true)
	{
		/* Check if the stream contains the current author already. */
		sprintf(vars.query, "select * from %s where author = '%s' ", streamUsers_directory, username);
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
		if (mysql_num_rows(vars.result) > 0)
		{
			printf("User \"%s\" could not be added to stream [%s] because they are already subscribed!\n", username, list);
			mysql_close(&(vars.mysql));
			mysql_library_end();
			return;
		}
	}
	else
	{
		/* Stream does not yet exist. Create all tables. */
		char stream_directory[MAX_STREAMNAME_LENGTH + strlen("Stream") + 1];
		strcpy(stream_directory, list);
		strcat(stream_directory, "Stream");

		/* Create the table for that stream. */
		sprintf(vars.query, "create table %s (id int not null auto_increment, author char(%d), date char(%d), post text(%d), primary key(id))", stream_directory, MAX_USERNAME_LENGTH, 30, CHARACTERS_PER_POST);
		if(mysql_query(&(vars.mysql), vars.query))
		{
			printf("Attempted Query: %s\n", vars.query);
			perror_sql("Command failed: Stream table could not be created.\n", &(vars.mysql));
		}

		sprintf(vars.query, "create table %s (author char(%d), last_post int(5))", streamUsers_directory, MAX_USERNAME_LENGTH);
		if(mysql_query(&(vars.mysql), vars.query))
		{
			printf("Attempted Query: %s\n", vars.query);
			perror_sql("Command failed: StreamUsers table could not be created.\n", &(vars.mysql));
		}
	}

	/* Add author to table. */
	sprintf(vars.query, "insert into %s values ('%s', 0)", streamUsers_directory, username);
	if(mysql_query(&(vars.mysql), vars.query))
	{
		printf("Attempted Query: %s\n", vars.query);
		perror_sql("Command failed: Stream table could not be created.\n", &(vars.mysql));
	}
	printf("Successfully added user \"%s\" to [%s]!\n", username, list);
	mysql_close(&(vars.mysql));
	mysql_library_end();
	return;
}

void removeUser(char* username, char* list)
{
	assert(username != NULL && list != NULL);
	sql_vars vars;
	mysql_init(&(vars.mysql));
	mysql_options(&(vars.mysql), MYSQL_READ_DEFAULT_GROUP, "mydb");
	if (!mysql_real_connect(&(vars.mysql), HOSTNAME, USERNAME, PASSWORD, DATABASE, 0, NULL, 0))
	{
		/* Connection to host failed. */
		perror_sql("Could not connect to host.\n", &(vars.mysql));
	}

	char streamUsers_directory[MAX_STREAMNAME_LENGTH + strlen("StreamUsers") + 1];
	strcpy(streamUsers_directory, list);
	strcat(streamUsers_directory, "StreamUsers");
	
	bool stream_exists = false;

	/* Get all tables and compare their names. */
	strcpy(vars.query, "show tables");
	if(mysql_query(&(vars.mysql), vars.query))
	{
		printf("Attempted Query: %s\n", vars.query);
		perror_sql("Command failed: show tables\n", &(vars.mysql));
	}

	/* Store results from query into result structure. */
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
			if (strcmp(vars.row[i], streamUsers_directory) == 0)
			{
				/* Match found. Stream table already exists. */
				stream_exists = true;
			}
		}
	}

	if (stream_exists == true)
	{
		/* Check if author is in table. */
		sprintf(vars.query, "select 1 from %s where author = '%s'", streamUsers_directory, username);
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

		/* If 0, nothing was found, ergo, the user is not in that stream and can not be removed. */
		if (mysql_num_rows(vars.result) == 0)
		{
			printf("User \"%s\" could not be removed from stream [%s] because \"%s\" is not subscribed to that Stream!\n", username, list, username);
			mysql_close(&(vars.mysql));
			mysql_library_end();
			return;
		}
	}
	else
	{
		/* The stream does not exist. */
		printf("User \"%s\" could not be removed from stream [%s] because the stream does not exist!\n", username, list);
		return;
	}

	/* Table exists, and author is inside. Remove them. */
	sprintf(vars.query, "delete from %s where author = '%s'", streamUsers_directory, username);
	if(mysql_query(&(vars.mysql), vars.query))
	{
		printf("Error: could not remove author \"%s\" from stream[%s]\n", username, list);
		printf("Attempted Query: %s\n", vars.query);
		perror_sql("", &(vars.mysql));
	}
	
	printf("Successfully removed user \"%s\" from [%s]!\n", username, list);
	mysql_close(&(vars.mysql));
	mysql_library_end();
	return;
}

void terminate_sql(sql_vars vars)
{
	mysql_close(&(vars.mysql));
	mysql_library_end();
	exit(0);
}
