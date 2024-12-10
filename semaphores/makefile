CXX = g++
CXXFLAGS = -Wall -std=c++17 -pthread

TARGET1 = tests_empty
TARGET2 = tests_full

SRCS_EMPTY = semaphores.cpp tests_empty.cpp
SRCS_FULL = semaphores.cpp tests_full.cpp

OBJS_EMPTY = $(SRCS_EMPTY:.cpp=.o)
OBJS_FULL = $(SRCS_FULL:.cpp=.o)

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJS_EMPTY)
	$(CXX) $(CXXFLAGS) -o $(TARGET1) $(OBJS_EMPTY)

$(TARGET2): $(OBJS_FULL)
	$(CXX) $(CXXFLAGS) -o $(TARGET2) $(OBJS_FULL)

%.o: %.cpp monitor.h semaphores.h
	$(CXX) $(CXXFLAGS) -DDEBUG -g -c $< -o $@

clean:
	rm -f $(OBJS_EMPTY) $(OBJS_FULL) $(TARGET1) $(TARGET2)
