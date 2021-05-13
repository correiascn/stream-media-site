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

int main (int argc, char* argv[])
{
	if (argv[1] == NULL)
	{
		printf("No username provided!\n");
		exit(0);
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
	/* print all results */
	while ((vars.row = mysql_fetch_row(vars.result)))
	{
		int i;
		for (i = 0; i < mysql_num_fields(vars.result); i++)
		{
			printf("%s\n", vars.row[i]);
		}
	}

	return 0;
}
