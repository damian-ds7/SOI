CXX = g++
CXXFLAGS = -Wall -std=c++17 -pthread

TARGET = semaphores_threads

SRCS = thread.cpp
OBJS = $(SRCS1:.cpp=.o)

sem: $(TARGET)
all: threads procs

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp monitor.h
	$(CXX) $(CXXFLAGS) -DDEBUG -g -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
