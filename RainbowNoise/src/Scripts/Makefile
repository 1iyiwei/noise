SOURCES = normalize.cpp  normalize_hist.cpp  siblingr.cpp  txt1dat.cpp  txt2dat.cpp

EXES = $(SOURCES:.cpp=.exe)

all: $(EXES)

%.exe : %.cpp
	g++ -O $< -o $@

clean:
	rm -f $(EXES)
