OPENCVINC=`params --opencv-inc`
OPENCVLIB=`params --opencv-lib`
OPENCVLINK=`params --opencv-link`
CPPFLAGS=`params --opencv-cxx-flags`
FLAGS=-g


all: test DrawTripOriginal

test: test.o ConcentricEllipse.o TripOriginalIdentify.o
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}


DrawTripOriginal: DrawTripOriginal.o
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}


%.o: %.cc
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVINC} -o $@ -c $<

clean:
	-rm -f test test.o ConcentricEllipse.o TripOriginalIdentify.o DrawTripOriginal DrawTripOriginal.o

depend:
	makedepend ${OPENCVINC} ${CPPFLAGS} *.cc

# DO NOT DELETE THIS LINE
ConcentricEllipse.o: ConcentricEllipse.hh
ConcentricEllipse.o: /home/andy/phd/build/include/opencv/cv.h
ConcentricEllipse.o: /home/andy/phd/build/include/opencv/cvtypes.h
ConcentricEllipse.o: /usr/include/assert.h /usr/include/features.h
ConcentricEllipse.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ConcentricEllipse.o: /usr/include/stdlib.h
ConcentricEllipse.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.2.3/include/stddef.h
ConcentricEllipse.o: /home/andy/phd/build/include/opencv/cverror.h
ConcentricEllipse.o: /home/andy/phd/build/include/opencv/cvcompat.h
ConcentricEllipse.o: /usr/include/string.h Tag.hh TripOriginalIdentify.hh
DrawTripOriginal.o: /home/andy/phd/build/include/opencv/cv.h
DrawTripOriginal.o: /home/andy/phd/build/include/opencv/cvtypes.h
DrawTripOriginal.o: /usr/include/assert.h /usr/include/features.h
DrawTripOriginal.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
DrawTripOriginal.o: /usr/include/stdlib.h
DrawTripOriginal.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.2.3/include/stddef.h
DrawTripOriginal.o: /home/andy/phd/build/include/opencv/cverror.h
DrawTripOriginal.o: /home/andy/phd/build/include/opencv/cvcompat.h
DrawTripOriginal.o: /usr/include/string.h
DrawTripOriginal.o: /home/andy/phd/build/include/opencv/highgui.h
DrawTripOriginal.o: TripOriginalParams.hh
test.o: /home/andy/phd/build/include/opencv/cv.h
test.o: /home/andy/phd/build/include/opencv/cvtypes.h /usr/include/assert.h
test.o: /usr/include/features.h /usr/include/sys/cdefs.h
test.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h
test.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.2.3/include/stddef.h
test.o: /home/andy/phd/build/include/opencv/cverror.h
test.o: /home/andy/phd/build/include/opencv/cvcompat.h /usr/include/string.h
test.o: /home/andy/phd/build/include/opencv/highgui.h ConcentricEllipse.hh
test.o: Tag.hh
TripOriginalIdentify.o: TripOriginalIdentify.hh
TripOriginalIdentify.o: /home/andy/phd/build/include/opencv/cv.h
TripOriginalIdentify.o: /home/andy/phd/build/include/opencv/cvtypes.h
TripOriginalIdentify.o: /usr/include/assert.h /usr/include/features.h
TripOriginalIdentify.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
TripOriginalIdentify.o: /usr/include/stdlib.h
TripOriginalIdentify.o: /usr/lib/gcc-lib/i686-pc-linux-gnu/3.2.3/include/stddef.h
TripOriginalIdentify.o: /home/andy/phd/build/include/opencv/cverror.h
TripOriginalIdentify.o: /home/andy/phd/build/include/opencv/cvcompat.h
TripOriginalIdentify.o: /usr/include/string.h Tag.hh TripOriginalParams.hh
