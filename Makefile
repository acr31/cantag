OPENCVINC=`params --opencv-inc`
OPENCVLIB=`params --opencv-lib`
OPENCVLINK=`params --opencv-link`
CPPFLAGS=`params --opencv-cxx-flags`
FLAGS=-g -o3 


all: tripover drawtriptag triptest


tripover: tripover.o Tag.o concentricellipse.o posefromcircle.o
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}

tripover_clean:
	-rm -f tripover tripover.o Tag.o concentricellipse.o posefromcircle.o

drawtriptag: drawtriptag.o concentricellipse.o Tag.o posefromcircle.o
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}

drawtriptag_clean:
	-rm -f drawtriptag drawtriptag.o Tag.o concentricellipse.o posefromcircle.o

triptest: triptest.o Tag.o concentricellipse.o posefromcircle.o
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}

triptest_clean:
	-rm -f triptest triptest.o Tag.o concentricellipse.o posefromcircle.o

%.o: %.cc
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVINC} -o $@ -c $<

clean: tripover_clean drawtriptag_clean triptest_clean

depend:
	makedepend -Y. ${CPPFLAGS} *.cc 2>/dev/null

# DO NOT DELETE THIS LINE

concentricellipse.o: concentricellipse.hh
drawtriptag.o: TripOriginalTag.hh Tag.hh concentricellipse.hh
drawtriptag.o: posefromcircle.hh
Tag.o: Tag.hh
tripover.o: TripOriginalTag.hh Tag.hh concentricellipse.hh posefromcircle.hh
triptest.o: TripOriginalTag.hh Tag.hh concentricellipse.hh posefromcircle.hh
