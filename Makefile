EXENAME = elevator
OBJS = main.o elevator.o 

CXX = clang++
CXXFLAGS = -std=c++1y -stdlib=libc++ -c -g -O0 -Wextra -pedantic -pthread
LD = clang++
LDFLAGS = -std=c++1y -stdlib=libc++ -lc++abi

all : $(EXENAME)

$(EXENAME) : $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXENAME)

main.o : main.cpp elevator.h
	$(CXX) $(CXXFLAGS) main.cpp

elevator.o : elevator.cpp elevator.h
	$(CXX) $(CXXFLAGS) elevator.cpp

clean :
	-rm -f *.o $(EXENAME)