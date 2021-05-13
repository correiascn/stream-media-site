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

#ifndef __CORREIAN_STREAM__
#define __CORREIAN_STREAM__

/* Importing standard C libraries. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mysql/mysql.h>

/* Restrictions on posts, usernames, & stream names. */
#define CHARACTERS_PER_POST 512 /* Trying to define a reasonable character limit for posts. But hey, it's way more than a tweet. */
#define CHARACTERS_PER_LINE 255 /* Buffer size for inputting text for a post. Restriction based on limits of view.py (stated in A2 specs). */
#define MAX_STREAMNAME_LENGTH 128 /* Prevents filenames becoming too large. This varies among operating systems. Here's a reasonable limit. */
#define MAX_USERNAME_LENGTH 48 /* 48 characters is far more than a fairly standard amount of characters in a username for most sites. */
#define MAX_SUBSCRIBERS 1024 /* Maximum number of subscribers a Stream may have. */
#define MAX_POSITION_DIGITS 19 /* Maximum number of digits storable in a position from a StreamUsers file. */

/* Predefined constants for mySQL utilities. */
#define MAX_QUERY 512
#define HOSTNAME  "dursley.socs.uoguelph.ca"
#define USERNAME  "correian"
#define PASSWORD  "0930748"
#define DATABASE  "correian"

typedef struct userPost
{
	char *username;
	char *streamname;
	char *date;
	char *text;
}UserPost;


typedef struct sql_vars
{
	MYSQL mysql;
	MYSQL_RES* result;
	MYSQL_ROW row;
	MYSQL_FIELD* field;

	char query[MAX_QUERY];
}sql_vars;

/*
	A simple utility function that prints out the user's error message (stored in the parameter msg), as well as the applicable mySQL message.
*/
void perror_sql(char* msg, MYSQL* mysql);

/*
	Clears a string. That's what the name literally says, but just abbreviated. clr = clear, str = string.
	First character is set to NULL terminator ('\0').
*/
void clrstr(char *buf);

/*
	The updateStream() function adds a new posting to the stream specified in streamname. If the stream does not exist then it creates the files 
	necessary for the stream. This includes the StreamData and StreamUsers files.
*/
void updateStream(struct userPost *st);

/*
	The addUser() function adds a user with username to all of the specified stream. If the stream does not exist then a new stream with that 
	name should be created.
*/
void addUser(char *username, char *list);

/*
	The removeUser() function removes the username from the stream(s) in the list. If the file doesn't exist, or if the user is not included 
	in its corresponding stream, an error message will be printed.
*/
void removeUser(char *username, char *list);

void terminate_sql(sql_vars vars);

#endif
