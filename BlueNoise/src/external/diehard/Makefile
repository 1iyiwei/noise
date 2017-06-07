#
# Makefile for diehard
#

CC=gcc -O

OBJS = diehard.o d3sphere.o kstest.o squeez.o cdbday.o cdosum.o df.o \
       mindist.o cdbinrnk.o cdpark.o cdoperm5.o cdbitst.o cnt1s.o \
       funct.o cdomso.o craptest.o runtest.o

all: diehard.exe
clean:
	rm -f $(OBJS) diehard.exe

diehard.exe: $(OBJS)
	$(CC) -lm -o diehard $(OBJS)

diehard.o: diehard.c header.h macro.h
	$(CC) -c diehard.c

d3sphere.o: d3sphere.c header.h macro.h
	$(CC) -c d3sphere.c

kstest.o: kstest.c header.h macro.h	 
	$(CC) -c kstest.c

cdoperm5.o: cdoperm5.c header.h
	$(CC) -c cdoperm5.c

squeez.o: squeez.c header.h macro.h
	$(CC) -c squeez.c

cdbday.o: cdbday.c header.h
	$(CC) -c cdbday.c

cdosum.o: cdosum.c header.h
	$(CC) -c cdosum.c

df.o: df.c 
	$(CC) -c df.c

funct.o: funct.c macro.h
	$(CC) -c funct.c

mindist.o: mindist.c header.h macro.h
	$(CC) -c mindist.c

cdbinrnk.o: cdbinrnk.c header.h macro.h
	$(CC) -c cdbinrnk.c

cdpark.o: cdpark.c header.h macro.h
	$(CC) -c cdpark.c

cdbitst.o: cdbitst.c header.h
	$(CC) -c cdbitst.c

cnt1s.o: cnt1s.c header.h
	$(CC) -c cnt1s.c

cdomso.o: cdomso.c header.h
	$(CC) -c cdomso.c

craptest.o: craptest.c header.h macro.h
	$(CC) -c craptest.c

runtest.o: runtest.c header.h macro.h
	$(CC) -c runtest.c
