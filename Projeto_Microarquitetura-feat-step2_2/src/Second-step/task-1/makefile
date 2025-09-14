CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g -I utils
TARGET = out

SRCS = main.cpp include/core/ULA.cpp include/utils/arquivo.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean