OPENCVINC=`params --opencv-inc`
OPENCVLIB=`params --opencv-lib`
OPENCVLINK=`params --opencv-link`
CPPFLAGS=`params --opencv-cxx-flags`
FLAGS=-g


all: tripover DrawTripOriginal


tripover: tripover.o Tag.o TripOriginalTag.o concentricellipse.o
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}


DrawTripOriginal: DrawTripOriginal.o
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}


%.o: %.cc
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVINC} -o $@ -c $<

clean:
	-rm -f TripOriginalTag.o tripover.o tripover DrawTripOriginal.o

depend:
	makedepend ${OPENCVINC} ${CPPFLAGS} *.cc

# DO NOT DELETE THIS LINE
