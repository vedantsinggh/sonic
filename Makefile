CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

app: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o app
