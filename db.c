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

/* Importing libraries. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mysql/mysql.h>

/* User-defined libraries. */
#include "stream.h"

/* Prototypes since I do not want to clutter my project folder with even more source files, since code subdirectories aren't allowed. */
void check_input(int argc, char* argv[], sql_vars vars);
void clear_data(sql_vars vars);
void reset_data(sql_vars vars);
void dump_posts(sql_vars vars);
void dump_users(sql_vars vars);
void dump_streams(sql_vars vars);

int main(int argc, char* argv[])
{
	sql_vars vars;
	
	mysql_init(&(vars.mysql));
	mysql_options(&(vars.mysql), MYSQL_READ_DEFAULT_GROUP, "mydb");
	if (!mysql_real_connect(&(vars.mysql), HOSTNAME, USERNAME, PASSWORD, DATABASE, 0, NULL, 0))
	{
		/* Connection to host failed. */
		perror_sql("Could not connect to host.\n", &(vars.mysql));
		printf("\n");
	}

	check_input(argc, argv, vars);

	/* Finally close connection to server. */
	terminate_sql(vars);
	return 0;
}

void check_input(int argc, char* argv[], sql_vars vars)
{
	/* Array of all possible valid command line arguments. */
	char* valid_args[] = {"-clear", "-reset", "-posts", "-users", "-streams", "-help"}; /* Length = 6. */

	/* If no arguments were provided, the program can not do anything. */
	if (argc == 1 || argv[1] == NULL)
	{
		printf("\nNo arguments provided. This application requires at least one argument to run.\n");
		printf("For information on what arguments are applicable (and what they do), rerun this application with the -help flag.\n\n");
		terminate_sql(vars);
	}

	/* Check for undefined arguments (i.e. those not in valid_args). */
	if (argc > 1)
	{
		int i;
		for (i = 1; i < argc; i++)
		{
			int j;
			bool is_defined_command = false;

			for (j = 0; j < 6; j++)
			{
				if (strcmp(argv[i], valid_args[j]) == 0)
				{
					is_defined_command = true;
					break;
				}
			}

			if (is_defined_command == false)
			{
				printf("\nArgument \"%s\" is not valid!\n\n", argv[i]);
				terminate_sql(vars);
			}
		}
	}

	if (strcmp(argv[1], "-help") == 0)
	{
		printf("The -clear flag removes all of the posts, users, streams and any other information from the tables in the database.\n");
		printf("The -reset flag deletes the tables from the database.\n");
		printf("The -posts flag prints out all posts stored in the database.\n");
		printf("The -users flag prints out all user names stored in the database.\n");
		printf("The -streams flag prints out all stream names stored in the database.\n\n");

		printf("The order of the -posts, -users, and -streams flags does not matter.\n");
		printf("Do not use -clear and -reset simultaneously.\n");
		printf("If using -clear or -reset, no more flags may be used.\n");
		printf("See README.txt if you have any concerns or questions about how to use this application.\n\n");
		
		/* Close connection to server. */
		terminate_sql(vars);
	}
	else if (strcmp(argv[1], "-clear") == 0)
	{
		if (argc > 2)
		{
			printf("\nNo additional arguments may be specified when -clear is active.\n\n");
			terminate_sql(vars);
		}
		clear_data(vars);
		
		/* Close connection to server. */
		terminate_sql(vars);
	}
	else if (strcmp(argv[1], "-reset") == 0)
	{
		if (argc > 2)
		{
			printf("\nNo additional arguments may be specified when -reset is active.\n\n");
			
			/* Close connection to server. */
			terminate_sql(vars);
		}
		reset_data(vars);
		
		/* Close connection to server. */
		terminate_sql(vars);
	}

	/* The other arguments (those that output the data) were specified. */
	int i;
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-posts") == 0)
		{
			dump_posts(vars);
		}
		else if (strcmp(argv[i], "-users") == 0)
		{
			dump_users(vars);
		}
		else if (strcmp(argv[i], "-streams") == 0)
		{
			dump_streams(vars);
		}
	}
}

void clear_data(sql_vars vars)
{
	/* Delete all rows in all tables. Leave tables intact, but not their contents. */
	printf("Removing all posts, users, & streams...\n");

	/* Get all tables. */
	strcpy(vars.query, "show tables");
	if(mysql_query(&(vars.mysql), vars.query))
	{
		perror_sql("Command failed: show tables\n", &(vars.mysql));
	}
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
			sprintf(vars.query, "delete from %s", vars.row[i]);
			if(mysql_query(&(vars.mysql), vars.query))
			{
				printf("Query: %s\n", vars.query);
				perror_sql("can't drop that",&(vars.mysql));
			}
		}
	}
	printf("All records from all tables removed.\n\n");
	return;
}

void reset_data(sql_vars vars)
{
	printf("Removing all tables...\n");

	/* Get all tables. */
	strcpy(vars.query, "show tables");
	if(mysql_query(&(vars.mysql), vars.query))
	{
		perror_sql("Command failed: show tables\n", &(vars.mysql));
	}
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
			/*printf("%s ", row[i]);*/
			sprintf(vars.query, "drop table %s", vars.row[i]);
			if(mysql_query(&(vars.mysql), vars.query))
			{
				printf("Query: %s\n", vars.query);
				perror_sql("can't drop that",&(vars.mysql));
			}
		}
	}
	printf("All tables removed.\n\n");
	return;
}

void dump_posts(sql_vars vars)
{
	printf("All posts:\n\n");

	/* Get all tables. */
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
			size_t stream_length = strlen(vars.row[i]);
			size_t end_length = strlen("Stream");

			if (strncmp(vars.row[i] + stream_length - end_length, "Stream", end_length) == 0)
			{
				MYSQL_RES* result;
				MYSQL_ROW row;
				char new_query[MAX_QUERY];

				sprintf(new_query, "select * from %s", vars.row[i]);
				if(mysql_query(&(vars.mysql), new_query))
				{
					perror_sql("",&(vars.mysql));
				}
				if (!(result = mysql_store_result(&(vars.mysql))))
				{
					perror_sql("",&(vars.mysql));
				}

				/* Print all results. */
				while ((row = mysql_fetch_row(result)))
				{
					printf("Author: %s\n", row[1]);
					printf("Date: %s\n", row[2]);
					printf("%s\n", row[3]);
				}
			}
		}
	}

	printf("\n\n");
	return;
}

void dump_users(sql_vars vars)
{
	printf("All registered users:\n\n");

	/* Create a temporary table to store names. Set the key as names so identical entries do not exist. */
	sprintf(vars.query, "create table temp_usernames (author char(%d) primary key)", MAX_USERNAME_LENGTH);
	if(mysql_query(&(vars.mysql), vars.query))
	{
		printf("Attempted Query: %s\n", vars.query);
		perror_sql("Command failed: Temporary usernames table could not be created.\n", &(vars.mysql));
	}

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
		/* Select first column (username) from current table. */
		int i;
		for (i = 0; i < mysql_num_fields(vars.result); i++)
		{
			size_t stream_length = strlen(vars.row[i]);
			size_t end_length = strlen("StreamUsers");

			if (strncmp(vars.row[i] + stream_length - end_length, "StreamUsers", end_length) == 0)
			{
				char new_query[MAX_QUERY];
				sprintf(new_query, "insert into temp_usernames select author from %s where author not in (select author from temp_usernames)", vars.row[i]);
				
				if(mysql_query(&(vars.mysql), new_query))
				{
					perror_sql("", &(vars.mysql));
				}
			}
		}
	}

	/* Print & Drop the temporary table. */
	strcpy(vars.query, "select * from temp_usernames");
	if(mysql_query(&(vars.mysql), vars.query))
	{
		printf("Attempted Query: %s\n", vars.query);
		perror_sql("", &(vars.mysql));
	}
	/* Store results from query into result structure. */
	if (!(vars.result = mysql_store_result(&(vars.mysql))))
	{
		perror_sql("Failed to store results in result.\n", &(vars.mysql));
	}
	while ((vars.row = mysql_fetch_row(vars.result)))
	{
		printf("%s\n", vars.row[0]);
	}

	strcpy(vars.query, "drop table temp_usernames");
	if(mysql_query(&(vars.mysql), vars.query))
	{
		printf("Attempted Query: %s\n", vars.query);
		perror_sql("Command failed: Temporary usernames table could not be dropped.\n", &(vars.mysql));
	}
	printf("\n\n");
	return;
}

void dump_streams(sql_vars vars)
{
	printf("All created streams:\n\n");

	/* Get all tables. */
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
			size_t stream_length = strlen(vars.row[i]);
			size_t end_length = strlen("Stream");

			if (strncmp(vars.row[i] + stream_length - end_length, "Stream", end_length) == 0)
			{
				printf("%s\n", vars.row[i]);
			}
		}
	}
	printf("\n\n");
	return;
}
