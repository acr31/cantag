OPENCVINC=`params --opencv-inc`
OPENCVLIB=`params --opencv-lib`
OPENCVLINK=`params --opencv-link`
CPPFLAGS=`params --opencv-cxx-flags`
FLAGS=-g -o3


all: tripover DrawTripOriginal


tripover: tripover.o Tag.o concentricellipse.o
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}


DrawTripOriginal: DrawTripOriginal.o concentricellipse.o Tag.o
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}


%.o: %.cc
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVINC} -o $@ -c $<

clean:
	-rm -f TripOriginalTag.o tripover.o tripover DrawTripOriginal.o DrawTripOriginal Tag.o

depend:
	makedepend -Y. ${CPPFLAGS} *.cc 2>/dev/null

# DO NOT DELETE THIS LINE

concentricellipse.o: concentricellipse.hh
DrawTripOriginal.o: TripOriginalTag.hh Tag.hh concentricellipse.hh
Tag.o: Tag.hh
tripover.o: TripOriginalTag.hh Tag.hh concentricellipse.hh
