LineDetectAndMatch : LineDetectAndMatch.o LineSLAMSystem.o
	g++ LineDetectAndMatch.o LineSLAMSystem.o -o LineDetectAndMatch `pkg-config --cflags --libs opencv`
LineDetectAndMatch.o : LineDetectAndMatch.cpp LineSLAMSystem.h
	g++ -c LineDetectAndMatch.cpp LineSLAMSystem.h -std=c++11
LineSLAMSystem.o : LineSLAMSystem.cpp LineSLAMSystem.h
	g++ -c LineSLAMSystem.cpp LineSLAMSystem.h -std=c++11

clean:
	rm LineDetectAndMatch LineDetectAndMatch.o LineSLAMSystem.o

