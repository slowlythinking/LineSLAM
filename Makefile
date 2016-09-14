LineDetectAndMatch : LineDetectAndMatch.o LineSLAMSystem.o LineDetector.o LineTracking.o Drawer.o Viewer.o
	g++ LineDetectAndMatch.o LineSLAMSystem.o LineDetector.o LineTracking.o Drawer.o Viewer.o -o LineDetectAndMatch EDLinesLib.a `pkg-config --cflags --libs opencv`
LineDetectAndMatch.o : LineDetectAndMatch.cpp LineSLAMSystem.h LS.h
	g++ -c LineDetectAndMatch.cpp LineSLAMSystem.h LS.h -std=c++11
LineSLAMSystem.o : LineSLAMSystem.cpp LineSLAMSystem.h LineTracking.h Timer.h LS.h Viewer.h Drawer.h
	g++ -c LineSLAMSystem.cpp LS.h LineSLAMSystem.h LineTracking.h Timer.h Viewer.h Drawer.h -std=c++11
LineDetector.o :LineDetector.cpp LineDetector.h LS.h Timer.h EDLinesLib.a
	g++ -c LineDetector.cpp LineDetector.h LS.h Timer.h EDLinesLib.a -std=c++11
LineTracking.o :LineTracking.cpp LineTracking.h Timer.h LS.h EDLinesLib.a
	g++ -c LineTracking.cpp LineTracking.h Timer.h LS.h EDLinesLib.a -std=c++11
Drawer.o :Drawer.cpp Drawer.h LS.h
	g++ -c Drawer.cpp Drawer.h LS.h -std=c++11
Viewer.o :Viewer.cpp Viewer.h Drawer.h LS.h
	g++ -c Viewer.cpp Viewer.h Drawer.h LS.h -std=c++11

clean:
	rm LineDetectAndMatch *.o

