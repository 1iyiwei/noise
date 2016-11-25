SOURCES = txt1dat.cpp txt2dat.cpp behead.cpp

EXES = $(SOURCES:.cpp=.exe)

all: $(EXES)

%.exe : %.cpp
	g++ -O $< -o $@

clean:
	rm -f $(EXES)
