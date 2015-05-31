



CXX=g++
CFLAGS = -g3
SRCS = TestSuite.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = testsuite

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^


.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $^

