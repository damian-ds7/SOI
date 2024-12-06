CXX = g++
CXXFLAGS = -Wall -std=c++17 -pthread

TARGET = semaphores
SRCS = semaphores.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp monitor.h
	$(CXX) $(CXXFLAGS) -DDEBUG -g -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
