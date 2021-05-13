/*
CIS*2750
Assignment 3 - WPML Parser
Nelson Correia
0930748
correian@mail.uoguelph.ca
19 March 2017
Handles a message posting and viewing application.
See README and A3 specs for limitations and usage.
*/

/* Importing standard C libraries. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

/* Definitions for program control. */
#define LINE_SIZE 255
#define MAX_LINES 128
#define MAX_NAME_LENGTH 95
#define MAX_INPUT_FIELDS 10

void parse_wpml(char data[][LINE_SIZE + 1], int size);

/**
 * main
 * Main method for the interpretation of a wpml configuration file.
 * IN: int argc, char* argv[]. See README for operation.
 * OUT: N/A. See POST.
 * POST: The parsed contents of the input file will be outputted.
 * ERROR: Command line input must be as specified in the README. Anything else will ruin it.
 * See README for what the input file should be like. Bad input will not be compensated for.
 */
int main (int argc, char* argv[])
{
	/* Only one argument is needed -- the configuration file directory. */
	assert(argc != 1);
	assert(argv[1] != NULL);

	FILE* fp = fopen(argv[1], "r");
	assert(fp != NULL);

	/* Copy file contents into memory. */
	char line[LINE_SIZE + 1]; /* + 1 for '\0'. */
	char data[MAX_LINES][LINE_SIZE + 1];
	int counter = 0;

	while(fgets(line, LINE_SIZE + 1, fp) != NULL)
	{
		/* Make sure the counter does not exceed the bounds of the array. Otherwise, a segmentation fault may occur. */
		assert(counter < MAX_LINES);
		strcpy(data[counter], line);
		counter++;
	}
	fclose(fp);

	/* Now parse and output the data from the contents of the file that were copied into memory. */
	parse_wpml(data, counter);

	return 0;
}

void parse_wpml(char data[][LINE_SIZE + 1], int size)
{
	/* Line-by-line iteration. */
	int i;
	for (i = 0; i < size; i++)
	{
		/* Default variables for sizes, or other attributes that may change when specified in a line. They reset for every line. */
		int text_size = 3;

		int j;
		for (j = 0; j < strlen(data[i]); j++)
		{
			if (data[i][j] == '.')
			{
				/* Draw horizontal line. */ 
				if (data[i][j + 1] == 'd')
				{
					printf("<hr>\n");
					/* Skip the d and brackets. */
					j += 3;
				}
				else if (data[i][j + 1] == 'e')
				{
					char exe[MAX_NAME_LENGTH + 1];
					int name_counter = 0;

					int p;
					for (p = j + 1; p < strlen(data[i]); p++)
					{
						/* Parse the tags. e only expects a single tag named exe. */
						if (data[i][p] == '"')
						{
							/* Copy contents until '"' is found again. */
							int k;
							for (k = p + 1; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									exe[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									exe[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									break;
								}
							}
						}
					}
					printf("<?php\n\texec(%s, $output, $status);\n\tforeach($output as $line) echo $line . '<br>';\n?>\n", exe);
				}
				else if (data[i][j + 1] == 't')
				{
					char argument[MAX_NAME_LENGTH + 1];
					int name_counter = 0;
					bool file_mode = false;

					if (data[i][j + 3] == 'f')
					{
						file_mode = true;
					}

					int p;
					for (p = j + 1; p < strlen(data[i]); p++)
					{
						/* Parse the tags. e only expects a single tag named exe. */
						if (data[i][p] == '"')
						{
							/* Copy contents until '"' is found again. */
							int k;
							for (k = p + 1; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									argument[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									argument[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									break;
								}
							}
						}
					}
					if (file_mode == true)
					{
						/* PHP code must be run with eval in the calling .php page. */
						printf("<?php\n\t$fp = '%s';\n\tif (file_exists($fp))\n\t{\n\t\techo file_get_contents($fp);\n\t}\n?>\n", argument);
					}
					else
					{
						/* If the argument was plain text rather than a file, directly output the text as is. Append a <br> to be neat. */
						printf("%s<br>\n", argument);
					}
				}
				else if (data[i][j + 1] == 'b')
				{
					char button_name[MAX_NAME_LENGTH + 1];
					char button_link[MAX_NAME_LENGTH + 1];
					char button_value[MAX_NAME_LENGTH + 1];

					strcpy(button_name, "");
					strcpy(button_link, "");
					strcpy(button_value, "");

					int p;
					for (p = j + 2; p < strlen(data[i]); p++)
					{
						/* Parse the tags. Two attributes expected: name or link. */
						if (data[i][p] == 'n' && data[i][p + 1] == 'a' && data[i][p + 2] == 'm' && data[i][p + 3] == 'e' && data[i][p + 4] == '=' && data[i][p + 5] == '"')
						{
							/* Current attribute is name. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 6; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									button_name[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									button_name[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									break;
								}
							}
						}
						if (data[i][p] == 'l' && data[i][p + 1] == 'i' && data[i][p + 2] == 'n' && data[i][p + 3] == 'k' && data[i][p + 4] == '=' && data[i][p + 5] == '"')
						{
							/* Current attribute is link. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 6; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									button_link[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									button_link[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									break;
								}
							}
						}
						if (data[i][p] == 'v' && data[i][p + 1] == 'a' && data[i][p + 2] == 'l' && data[i][p + 3] == 'u' && data[i][p + 4] == 'e' && data[i][p + 5] == '=' && data[i][p + 6] == '"')
						{
							/* Current attribute is link. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 7; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									button_value[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									button_value[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									break;
								}
							}
						}
					}
					printf("<form action=\"%s\" method = \"POST\">\n\t<button type = 'Submit' name = \"%s\" value = \"%s\">%s</button>\n</form>\n", button_link, button_name, button_value, button_value);
				}
				else if (data[i][j + 1] == 'h')
				{
					char heading_size_str[2]; /* 1 - 6, plus null terminator. */
					char heading_text[MAX_NAME_LENGTH + 1];
					strcpy(heading_text, " ");

					int p;
					for (p = j + 2; p < strlen(data[i]); p++)
					{
						/* Parse the tags. Two attributes expected: name or link. */
						if (data[i][p] == 's' && data[i][p + 1] == 'i' && data[i][p + 2] == 'z' && data[i][p + 3] == 'e' && data[i][p + 4] == '=' && data[i][p + 5] == '<')
						{
							/* Current attribute is name. Take number in between angled braces, and convert to int. */
							heading_size_str[0] = data[i][p + 6];
							heading_size_str[1] = '\0';
							text_size = atoi(heading_size_str);
							p += 7;
							j += 7;
						}
						else if (data[i][p] == 't' && data[i][p + 1] == 'e' && data[i][p + 2] == 'x' && data[i][p + 3] == 't' && data[i][p + 4] == '=' && data[i][p + 5] == '"')
						{
							/* Current attribute is link. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 6; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									heading_text[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									heading_text[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									break;
								}
							}
						}
					}
					
					if (strcmp(heading_text, " ") == 0)
					{
						strcpy(heading_text, "HEADING");
					}
					printf("<h%d>%s</h%d><br>\n", text_size, heading_text, text_size);
				}
				else if (data[i][j + 1] == 'l')
				{
					char link_text[MAX_NAME_LENGTH + 1];
					char link_link[MAX_NAME_LENGTH + 1];

					int p;
					for (p = j + 2; p < strlen(data[i]); p++)
					{
						/* Parse the tags. Two attributes expected: name or link. */
						if (data[i][p] == 't' && data[i][p + 1] == 'e' && data[i][p + 2] == 'x' && data[i][p + 3] == 't' && data[i][p + 4] == '=' && data[i][p + 5] == '"')
						{
							/* Current attribute is name. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 6; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									link_text[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									link_text[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									break;
								}
							}
						}
						if (data[i][p] == 'l' && data[i][p + 1] == 'i' && data[i][p + 2] == 'n' && data[i][p + 3] == 'k' && data[i][p + 4] == '=' && data[i][p + 5] == '"')
						{
							/* Current attribute is link. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 6; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									link_link[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									link_link[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									break;
								}
							}
						}
					}
					printf("<a href=\"%s\">%s</a><br>\n", link_link, link_text);
				}
				else if (data[i][j + 1] == 'i')
				{
					char action[MAX_NAME_LENGTH + 1]; /* Action applies to all of the input fields (shared value). */
					char text[MAX_NAME_LENGTH + 1];
					char name[MAX_NAME_LENGTH + 1];
					char value[MAX_NAME_LENGTH + 1];

					int p;
					for (p = j + 2; p < strlen(data[i]); p++)
					{
						/* Parse the tags. Three attributes expected per input field: . */
						if (data[i][p] == 'a' && data[i][p + 1] == 'c' && data[i][p + 2] == 't' && data[i][p + 3] == 'i' && data[i][p + 4] == 'o' && data[i][p + 5] == 'n'&& data[i][p + 6] == '=' && data[i][p + 7] == '"')
						{
							/* Current attribute is action, which is only expected once. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 8; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									action[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									action[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									printf("<form action = \"%s\" method = \"POST\">\n", action);
									break;
								}
							}
						}
						if (data[i][p] == 't' && data[i][p + 1] == 'e' && data[i][p + 2] == 'x' && data[i][p + 3] == 't' && data[i][p + 4] == '=' && data[i][p + 5] == '"')
						{
							/* Current attribute is link. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 6; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									text[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									text[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									break;
								}
							}
						}
						if (data[i][p] == 'n' && data[i][p + 1] == 'a' && data[i][p + 2] == 'm' && data[i][p + 3] == 'e' && data[i][p + 4] == '=' && data[i][p + 5] == '"')
						{
							/* Current attribute is link. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 6; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									name[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									name[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									break;
								}
							}
						}
						if (data[i][p] == 'v' && data[i][p + 1] == 'a' && data[i][p + 2] == 'l' && data[i][p + 3] == 'u' && data[i][p + 4] == 'e' && data[i][p + 5] == '=' && data[i][p + 6] == '"')
						{
							/* Current attribute is link. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 7; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									value[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									value[name_counter] = '\0';
									j = k + 1;
									p = k + 1;

									/* Print out current attributes read so far. */
									printf("\t%s<input type = \"text\" name = \"%s\" value = \"%s\"><br>\n", text, name, value);
									strcpy(text, "");
									strcpy(name, "");
									strcpy(value, "");

									break;
								}
							}
						}
					}
					printf("\t<input type = \"Submit\" value = \"Submit\">\n</form>\n");
				}
				else if (data[i][j + 1] == 'r')
				{
					char action[MAX_NAME_LENGTH + 1]; /* Action applies to all of the input fields (shared value). */
					char name[MAX_NAME_LENGTH + 1]; /* Name is also shared. */
					char value[MAX_NAME_LENGTH + 1];

					int num_radios = 0;

					int p;
					for (p = j + 2; p < strlen(data[i]); p++)
					{
						/* Parse the tags. Three attributes expected per input field: . */
						if (data[i][p] == 'a' && data[i][p + 1] == 'c' && data[i][p + 2] == 't' && data[i][p + 3] == 'i' && data[i][p + 4] == 'o' && data[i][p + 5] == 'n'&& data[i][p + 6] == '=' && data[i][p + 7] == '"')
						{
							/* Current attribute is action, which is only expected once. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 8; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									action[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									action[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									printf("<form action = \"%s\" method = \"POST\">\n", action);
									break;
								}
							}
						}
						if (data[i][p] == 'n' && data[i][p + 1] == 'a' && data[i][p + 2] == 'm' && data[i][p + 3] == 'e' && data[i][p + 4] == '=' && data[i][p + 5] == '"')
						{
							/* Current attribute is link. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 6; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									name[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									name[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									break;
								}
							}
						}
						if (data[i][p] == 'v' && data[i][p + 1] == 'a' && data[i][p + 2] == 'l' && data[i][p + 3] == 'u' && data[i][p + 4] == 'e' && data[i][p + 5] == '=' && data[i][p + 6] == '"')
						{
							/* Current attribute is link. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 7; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									value[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									value[name_counter] = '\0';
									j = k + 1;
									p = k + 1;

									/* Print out current attributes read so far. */
									if (num_radios != 0)
									{
										printf("\t<input type = \"radio\" name = \"%s\" value = \"%s\"> %s<br>\n", name, value, value);
									}
									else
									{
										printf("\t<input type = \"radio\" name = \"%s\" value = \"%s\" checked> %s<br>\n", name, value, value);
									}
									num_radios++;
									strcpy(name, "");
									strcpy(value, "");

									break;
								}
							}
						}
					}
					printf("\t<input type = \"Submit\" value = \"Submit\">\n</form>\n");
				}
				else if (data[i][j + 1] == 'p')
				{
					char image[MAX_NAME_LENGTH + 1];
					char size[MAX_NAME_LENGTH + 1];
					char width_str[MAX_NAME_LENGTH + 1];
					char height_str[MAX_NAME_LENGTH + 1];

					int width = 100;
					int height = 100;

					int p;
					for (p = j + 2; p < strlen(data[i]); p++)
					{
						/* Parse the tags. Three attributes expected per input field: . */
						if (data[i][p] == 'i' && data[i][p + 1] == 'm' && data[i][p + 2] == 'a' && data[i][p + 3] == 'g' && data[i][p + 4] == 'e' && data[i][p + 5] == '='&& data[i][p + 6] == '"')
						{
							/* Current attribute is image, which is only expected once. Copy contents until '"' is found again. */
							int k;
							int name_counter = 0;
							for (k = p + 7; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									image[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									image[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									break;
								}
							}
						}
						if (data[i][p] == 's' && data[i][p + 1] == 'i' && data[i][p + 2] == 'z' && data[i][p + 3] == 'e' && data[i][p + 4] == '=' && data[i][p + 5] == '<')
						{
							/* Current attribute is size. First width then height <w>x<h>. */
							int k;
							int name_counter = 0;
							for (k = p + 6; k < strlen(data[i]); k++)
							{
								if (data[i][k] != '"')
								{
									size[name_counter] = data[i][k];
									name_counter++;
								}
								else
								{
									size[name_counter] = '\0';
									j = k + 1;
									p = k + 1;
									char* tokens = strtok(size, "x");

									int z;
									for (z = 0; z < strlen(tokens) - 1; z++)
									{
										width_str[z] = tokens[z];
									}
									width_str[z + 1] = '\0';

									tokens = strtok(NULL, "x");
									for (z = 0; z < strlen(tokens) - 1; z++)
									{
										height_str[z] = tokens[z];
									}
									height_str[z + 1] = '\0';

									width = atoi(width_str);
									height = atoi(height_str);

									break;
								}
							}
						}
					}
					printf("<img src = \"%s\" style = \"width:%dpx;height:%dpx;\">\n", image, width, height);
				}
			}
		}
	}

	return;
}
