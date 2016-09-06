LineDetectAndMatch : LineDetectAndMatch.o LineSLAMSystem.o LineDetector.o
	g++ LineDetectAndMatch.o LineSLAMSystem.o LineDetector.o -o LineDetectAndMatch EDLinesLib.a `pkg-config --cflags --libs opencv`
LineDetectAndMatch.o : LineDetectAndMatch.cpp LineSLAMSystem.h
	g++ -c LineDetectAndMatch.cpp LineSLAMSystem.h -std=c++11
LineSLAMSystem.o : LineSLAMSystem.cpp LineSLAMSystem.h
	g++ -c LineSLAMSystem.cpp LineSLAMSystem.h -std=c++11
LineDetector.a :LineDetector.o 
	g++ -c LineDetector.cpp LineDetector.h LS.h Timer.h EDLinesLib.a -std=c++11

clean:
	rm LineDetectAndMatch LineDetectAndMatch.o LineSLAMSystem.o

