GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`
GTK_LIBS=`pkg-config --libs gtk+-2.0` 
GDKPIXBUF_LIBS=-lgdk_pixbuf-2.0
XML_CFLAGS=`xml2-config --cflags` 
XML_LIBS=`xml2-config --libs`
PNG_LIBS=-lpng -lm

all: gtk commandline

gtk: pfantedit

commandline: pf-extract pf-patch

pf-extract: pf-extract.o load-prg.o save-prg.o parsexml.o filename.o png.o
	gcc -o pf-extract pf-extract.o load-prg.o save-prg.o parsexml.o filename.o png.o $(XML_LIBS) $(PNG_LIBS) $(GDKPIXBUF_LIBS)

pf-patch: pf-patch.o load-prg.o save-prg.o parsexml.o filename.o png.o
	gcc -o pf-patch pf-patch.o load-prg.o save-prg.o parsexml.o filename.o png.o $(XML_LIBS) $(PNG_LIBS) $(GDKPIXBUF_LIBS)

pfantedit: pfantedit.o load-prg.o save-prg.o parsexml.o filename.o png.o
	gcc -o pfantedit pfantedit.o load-prg.o save-prg.o parsexml.o filename.o png.o $(XML_LIBS) $(GTK_LIBS) $(PNG_LIBS)



pf-extract.o: pf-extract.c
	gcc -g -c pf-extract.c

pf-patch.o: pf-patch.c
	gcc -g -c pf-patch.c

pfantedit.o: pfantedit.c prg.h
	gcc -g -c pfantedit.c $(GTK_CFLAGS)

load-prg.o: load-prg.c prg.h
	gcc -g -c load-prg.c $(GTK_CFLAGS)

save-prg.o: save-prg.c prg.h
	gcc -g -c save-prg.c $(GTK_CFLAGS)

parsexml.o: parsexml.c prg.h
	gcc -g -c parsexml.c $(XML_CFLAGS) $(GTK_CFLAGS)

filename.o: filename.c
	gcc -g -c filename.c

png.o: png.c
	gcc -g -c png.c $(GTK_CFLAGS)

clean:
	rm pfantedit
	rm pf-extract
	rm pf-patch
	rm *.o

