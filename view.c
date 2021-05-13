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

/* Importing standard C libraries. */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

/* Including user-defined libraries. */
#include "stream.h"

void open_stream(char* chosen_stream_name, char* username, char index_change, int index, char sort_by_author, sql_vars vars);
bool check_stream_selection(char* username, char* chosen_stream_name, char index_change, int index, char sort_by_author, sql_vars vars);

int main (int argc, char* argv[])
{
	/* View.py already handles some input validation. */
	if (argc < 6)
	{
		printf("\nView.py: Not enough command-line args!\n");
		exit(1);
	}

	char chosen_stream_name[MAX_STREAMNAME_LENGTH];
	strcpy(chosen_stream_name, argv[1]);
	char index_change = argv[2][0];
	int index = atoi(argv[3]);
	char sort_by_author = argv[4][0];
	char username[MAX_USERNAME_LENGTH];
	strcpy(username, argv[5]);

	/* If the username contains spaces (therefore taking up multiple command-line args)... */
	if (argc > 6)
	{
		int i;
		for (i = 6; i < argc; i++)
		{
			/* Append every other argument onto the variable username. */
			strcat(username, " ");
			strcat(username, argv[i]);
		}
	}

	sql_vars vars;
	mysql_init(&(vars.mysql));
	mysql_options(&(vars.mysql), MYSQL_READ_DEFAULT_GROUP, "mydb");
	if (!mysql_real_connect(&(vars.mysql), HOSTNAME, USERNAME, PASSWORD, DATABASE, 0, NULL, 0))
	{
		/* Connection to host failed. */
		perror_sql("Could not connect to host.\n", &(vars.mysql));
		printf("\n");
	}

	bool stream_is_valid = check_stream_selection(username, chosen_stream_name, index_change, index, sort_by_author, vars);
	if (stream_is_valid == false)
	{
		printf("\nStream [%s] cannot be viewed because the current user is not subscribed to that stream. Please subscribe to access this stream.\n", chosen_stream_name);
		exit(1);
	}
	open_stream(chosen_stream_name, username, index_change, index, sort_by_author, vars);

	terminate_sql(vars);
	return 0;
}

bool check_stream_selection(char* username, char* chosen_stream_name, char index_change, int index, char sort_by_author, sql_vars vars)
{
	/* This function is used to check if the stream name provided in the command-line arguments includes the specified author. */
	bool is_valid = false;

	if (strcmp(chosen_stream_name, "all") == 0)
	{
		return true;
	}

	char full_stream_name[MAX_STREAMNAME_LENGTH + strlen("Users") + 1];
	strcpy(full_stream_name, chosen_stream_name);
	strcat(full_stream_name, "Users");

	sprintf(vars.query, "select 1 from %s where author = '%s'", full_stream_name, username);
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

	/* If not 0, a match was found. The user is in the stream. */
	if (mysql_num_rows(vars.result) > 0)
	{
		is_valid = true;
	}

	return is_valid;
}

void open_stream(char* chosen_stream_name, char* username, char index_change, int index, char sort_by_author, sql_vars vars)
{
	int newest_post_read = 0;

	/* Create temporary table to store posts, contains all posts if "all" is selected, otherwise, it contains just one stream. */
	sprintf(vars.query, "create table temp_posts (id int, author char(%d), date char(%d), post text(%d))", MAX_USERNAME_LENGTH, 30, CHARACTERS_PER_POST);
	if(mysql_query(&(vars.mysql), vars.query))
	{
		sprintf(vars.query, "drop table temp_posts");
		if(mysql_query(&(vars.mysql), vars.query))
		{
			printf("Attempted Query: %s\n", vars.query);
			perror_sql("", &(vars.mysql));
		}
		sprintf(vars.query, "create table temp_posts (id int, author char(%d), date char(%d), post text(%d))", MAX_USERNAME_LENGTH, 30, CHARACTERS_PER_POST);
		if(mysql_query(&(vars.mysql), vars.query))
		{
			printf("Attempted Query: %s\n", vars.query);
			perror_sql("", &(vars.mysql));
		}
	}

	/* Add the specific stream specified into the table (if not all). Also get the last post read. */
	if (strcmp(chosen_stream_name, "all") != 0)
	{
		/* Retrieve last read post number. */
		char streamUsers_directory[MAX_STREAMNAME_LENGTH + strlen("Users") + 1];
		strcpy(streamUsers_directory, chosen_stream_name);
		strcat(streamUsers_directory, "Users");

		sprintf(vars.query, "select 1 from %s where author = '%s'", streamUsers_directory, username);
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
		vars.row = mysql_fetch_row(vars.result);
		/* row[0] = username, while row[1] = last_post (int). */
		newest_post_read = atoi(vars.row[1]);
		if (index_change == '0' || index_change == 'm')
		{
			index = newest_post_read;
		}

		/* Now insert the contents of the selected table into the new table. */
		sprintf(vars.query, "insert into temp_posts select * from %s", chosen_stream_name);
		if(mysql_query(&(vars.mysql), vars.query))
		{
			printf("Attempted Query: %s\n", vars.query);
			perror_sql("", &(vars.mysql));
		}
	}
	else
	{
		/* "all" specified. Add all streams to the table. First, find all streams that the user is already in. */
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
		while ((vars.row = mysql_fetch_row(vars.result)))
		{
			size_t stream_length = strlen(vars.row[0]);
			size_t end_length = strlen("StreamUsers");

			if (strncmp(vars.row[0] + stream_length - end_length, "StreamUsers", end_length) == 0)
			{
				char new_query[MAX_QUERY];

				char streamname[MAX_STREAMNAME_LENGTH + strlen("Stream") + 1];
				memset(streamname, '\0', MAX_STREAMNAME_LENGTH + strlen("Stream") + 1);
				strncpy(streamname, vars.row[0], strlen(vars.row[0]) - strlen("Users"));

				sprintf(new_query, "insert into temp_posts select * from %s", streamname);
				if(mysql_query(&(vars.mysql), new_query))
				{
					printf("Attempted Query: %s\n", new_query);
					perror_sql("",&(vars.mysql));
				}
			}
		}
	}

	/* Check if the index is going to cause an out-of-bounds error. If so, prevent that. */
	strcpy(vars.query, "select * from temp_posts");
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
	if (index >= mysql_num_rows(vars.result))
	{
		if (mysql_num_rows(vars.result) <= 1)
		{
			index = 1;
		}
		else
		{
			index = mysql_num_rows(vars.result) - 1;
		}
	}
	if (index < 0) index = 0;

	/* Update the users file if not all. */
	if (strcmp(chosen_stream_name, "all") != 0)
	{
		/* Only change if the index is larger than what was stored there. */
		if (index > newest_post_read)
		{
			char streamUsers_directory[MAX_STREAMNAME_LENGTH + strlen("Users") + 1];
			strcpy(streamUsers_directory, chosen_stream_name);
			strcat(streamUsers_directory, "Users");

			sprintf(vars.query, "update %s set last_post = %d where author = '%s'", streamUsers_directory, index, username);
			if(mysql_query(&(vars.mysql), vars.query))
			{
				printf("Attempted Query: %s\n", vars.query);
				perror_sql("", &(vars.mysql));
			}
		}
	}

	/* Sort by date or author, depending on the user's input. */
	if (sort_by_author == 'f')
	{
		/* Sort by time. */
		strcpy(vars.query, "select * from temp_posts order by date");
	}
	else if (sort_by_author == 't')
	{
		/* Sort by author. */
		strcpy(vars.query, "select * from temp_posts order by author");
	}
	
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
	int i;
	for (i = 0; i <= index; i++)
	{
		vars.row = mysql_fetch_row(vars.result);
	}

	/* Now we print out the selected stream post. */
	printf("Author: %s\n", vars.row[1]);
	printf("Date: %s\n", vars.row[2]);
	printf("%s\n", vars.row[3]);

	/* Drop temporary table. */
	strcpy(vars.query, "drop table temp_posts");
	if(mysql_query(&(vars.mysql), vars.query))
	{
		printf("Attempted Query: %s\n", vars.query);
		perror_sql("", &(vars.mysql));
	}

	return;
}
