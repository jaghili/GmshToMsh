CC := g++
EIGENDIR := -I/usr/include/eigen3/
MSHTOFRAC := mshtofrac.exe

all: $(MSHTOFRAC)

%.o : %.cpp
	$(CC) $(IDIR) -c $< -o $@

$(MSHTOFRAC): MshToFrac.cpp
	g++ $(IDIR) $< -o $@

clean :
	rm -rf $(MSHTOFRAC)
	rm -rf *.o
