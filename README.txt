	CIS*2750
	Assignment 4 - Message Management and Viewer (Online with database)
	Nelson Correia
	0930748
	correian@mail.uoguelph.ca
	31 March 2017


	--- KNOWN LIMITATIONS: ---

***USER NAMES ARE CASE SENSITIVE! e.g. username "nelson" != username "Nelson"
***USER NAMES MAY CONTAIN SPACES. STREAMS CANNOT.

wpmlp may fail if certain tags are used on the same line. It is recommended to use tags on different lines.
the .e() tag does not function.

	When viewing posts, if sort by author is selected, the application will not update the corresponding StreamData file if you view a new post.
The same restriction also applies when viewing all streams.

Mark all is not supported if you are viewing all streams.

	At any point, the user must never enter nothing into a text field. The application does not account for empty input. Doing so will result in 
undefined behaviour.

It is strictly prohibited to create a Stream named "all".

Due to the nature of the WPML parser, and the fact that newline characters do not work for command-line arguments, only single-line posts are 
allowed in this application.

	The amount of characters allowed per post, as well as username and stream length are all predefined constants that can be found in stream.h. 
Those values are based off of research and are there to not allow the user to enter ridiculously long posts, or have absurdly long usernames. 
These rules are based off of what most websites (notably social media) impose upon users. It is necessary to impose reasonable limitations. Stream 
name limitations are based off the fact that different operating systems vary in what the maximum file name length is.

When making a post, it is strictly prohibited to start the post with "Author: ".

	When viewing streams, the application will prevent you from reading previous post if you are at the first post, but the program can not prevent
the user from continually increasing the counter past the last post. This does not mean that the program will crash or output a blank message, or
otherwise have an error, but the counter will have to be set back using "previous post" repeatedly.

Any input user text input for this application may not contain SQL code or double quotation marks (").

	--- PROPER USE OF THIS PROGRAM: ---

To compile the program, type "make" into the command line. It is necessary to be in the same directory as the makefile for the command to work.
Make must be run before using the web application, as it is needed to compile the WPML parser as well as the server-end applications for 
outputting results from the addauthor, post, and view programs.

At any point in the application after the login page (index.php), the current user's name will be displayed. This is to remind the user of which 
account is logged in, as any social media site or any site requiring a login would do.

	MAKING A POST:

NOTE: post relies on user input. See KNOWN LIMITATIONS for restrictions on user input.

A prompt will appear to enter which stream will be posted to. Enter the stream name. The stream name must not contain any special 
characters that are invalid for a filename. If the stream specified does not exist, then no post can be made. To create a stream, the user must 
click the "Add Author" button to add themselves to a stream. If the user is not part of a valid specified stream then the request to post will 
similarly be denied.

Once a valid existant stream is selected, a message may be typed for that post. Once a satisfactory post is made, pressing submit will create the 
post. The message will be sent and posted in the appropriate stream and the application will redirect you to a page showing the results of your 
action.

	ADDING AND REMOVING AN AUTHOR:

NOTE: addauthor relies on user input. See KNOWN LIMITATIONS for restrictions on user input.

	addauthor is the sole application to create new Streams. Authors can be added to a stream that does not exist at the time the program is 
run, this way, users do not have to wait for the Stream's content to become available to subscribe to its content.

	A prompt will appear to enter a list of which stream(s) an author will be added to or removed from (depending on which button was clicked). If
multiple streams are desired, it is neccesary to separate the streamnames by commas (','). Spaces are not allowed since Stream names with spaces 
are invalid. If the user chooses to add an author to a stream that does not exist, it will be created and the author specified will be added. A 
user should be wary to what they type in, as a user may accidentally create a stream they did not wish to create in the first place.

	Attempting to remove an author from a non-existant stream, or removing an author not inside a specified existing stream will result in an 
error message. Obviously, these requests are impossible. Adding an author to a stream in which the author was already included will deny that 
request, and print out an appropriate error message. Newly added users to any stream will be directed to the first stream posting once view.py 
is run. No other prompts are needed from the user. If the addition or removal of an author was successful, a message summarizing your action 
will be displayed.

	VIEW (VIEW.PY):

	Immediately upon execution, the user will be presented with a list of streams they are subscribed to, including a final option "all". The 
stream options appear as buttons on the view.php page. Click the button if you wish to visit that stream.

	On view.php, streams that the user is not subscribed to will still appear. Clicking on one will display a message indicating this once 
redirected. This serves as a reminder for the user to register for any streams they take interest in.

	If a blank stream (i.e. a stream which has never had a post made in it) is opened, there will not be any output. This would look the same as 
if there was an error with the application, but this is expected as the application has nothing to output.

	Ordering does not correctly function, much less while all streams are selected.

	Once the user selects an option, the program will print out the contents of the specified stream(s). The program will keep track of what post
in the stream the user has last read. A post is considered read if it is ever displayed on the screen. The application will not keep track 
of which post was last read if sort by author or all streams mode is active.

	DB (DATABASE ACCESS/MODIFICATION TOOL):
	
	This program requires at least one argument. To understand what the arguments are, run the program with the -help flag. You could also see the A4
specifications. When using the -reset or -clear arguments, only one argument must be provided. Adding anything else is not allowed.

	For the "display" options (-posts, -streams, and -users) the order in which these flags are entered does not matter. Any combination of the three can be
used simultaneously. If the user so desires, they can even repeat flags.


ALL OTHER PROGRAMS FOUND IN THE PROJECT ARE NOT MEANT TO BE USED BY THE USER. THEY ARE USED BY OTHER PROGRAMS AS WELL AS THE WEBPAGES TO ASSISTS WITH FORMATTING.
