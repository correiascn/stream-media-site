a4:	addauthor post a1f stream wpmlp db view displayoptions
	chmod +x view.py

addauthor:	addauthor.c stream
	gcc addauthor.c -c -ansi -Wall -o addauthor.o -g;\
	gcc addauthor.o -o addauthor -lstream -L. -lmysqlclient -L/usr/lib/x86_64-linux-gnu/

post:	a1f post.cc stream
	./a1 post.cc ;\
	gcc post.c -c -ansi -Wall -o post.o -g ;\
	gcc post.o -o post -lstream -L. -lmysqlclient -L/usr/lib/x86_64-linux-gnu/ ;\

a1f:	a1.c
	gcc a1.c -ansi -Wall -o a1 -g

stream:	stream.c
	gcc stream.c -ansi -Wall -o stream.o -g -c -lmysqlclient -L/usr/lib/x86_64-linux-gnu/ ;\
	ar cr libstream.a stream.o ;\
	
wpmlp: wpmlp.c
	gcc wpmlp.c -ansi -Wall -o wpmlp -g

db:	db.c stream
	gcc db.c -c -ansi -Wall -o db.o -g ;\
	gcc db.o -o db -lstream -L. -lmysqlclient -L/usr/lib/x86_64-linux-gnu/ ;\
	
view: view.c stream
	gcc view.c -c -ansi -Wall -o view.o -g;\
	gcc view.o -o view -lstream -L. -lmysqlclient -L/usr/lib/x86_64-linux-gnu/
	
displayoptions: displayoptions.c stream
	gcc displayoptions.c -c -ansi -Wall -o displayoptions.o -g;\
	gcc displayoptions.o -o displayoptions -lstream -L. -lmysqlclient -L/usr/lib/x86_64-linux-gnu/
	
clean:
	rm stream.o
	rm libstream.a
	rm post.c
	rm post.o
	rm post
	rm addauthor
	rm addauthor.o
	rm a1
	rm db.o
	rm db
	rm view.o
	rm view
	rm displayoptions.o
	rm displayoptions
	rm wpmlp