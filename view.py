#!/usr/bin/python3

'''
CIS*2750
Assignment 3 - Message Management and Viewer (Online)
Nelson Correia
0930748
correian@mail.uoguelph.ca
19 March 2017
Handles a message posting and viewing application.
See README and A3 specs for limitations and usage.
'''

# Importing standard packages.
import sys
import os

# Main method:
if __name__ == '__main__':

	if len(sys.argv) < 6:
		print('\nView.py: Not enough command-line args!\n')
		print('\nNumber of arguments:', len(sys.argv), 'arguments.')
		print('Argument List:', str(sys.argv), '\n')
		sys.exit()

	# Copy down chosen stream.
	chosen_stream_name = sys.argv[1]

	# Copy down index_change.
	index_change = sys.argv[2]

	# Copy down index.
	index = int(sys.argv[3])

	# Copy down sort_by_author.
	sort_by_author = sys.argv[4]

	# Copy down username.
	username = sys.argv[5]

	# If the username contains spaces (therefore taking up multiple command-line args)...
	if len(sys.argv) > 6:
		for i in range(6, len(sys.argv)):
			# Append every other argument onto the variable username.
			username += " " + sys.argv[i]

	c_args = './view'
	for i in range(1, len(sys.argv)):
		c_args = c_args + ' ' + sys.argv[i]

	os.system(c_args)
