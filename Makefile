OPENCVINC=-I/usr/include/opencv
OPENCVLIB=-L/usr/lib
OPENCVLINK=-lopencv -lhighgui

test: test.o
	g++ -g ${OPENCVLIB} -o $@ $< ${OPENCVLINK}

%.o: %.cc
	g++ -g ${OPENCVINC} -o $@ -c $<