LineDetectAndMatch : LineDetectAndMatch.o
	g++ LineDetectAndMatch.o LineSLAMSystem.o -o LineDetectAndMatch `pkg-config --cflags --libs opencv`
LineDetectAndMatch.o : LineDetectAndMatch.cpp LineSLAMSystem.cpp LineSLAMSystem.h
	g++ -c LineSLAMSystem.h LineDetectAndMatch.cpp LineSLAMSystem.cpp -std=c++11
clean:
	rm LineDetectAndMatch LineDetectAndMatch.o

