OPENCVINC=`params --opencv-inc`
OPENCVLIB=`params --opencv-lib`
OPENCVLINK=`params --opencv-link`
CPPFLAGS=`params --opencv-cxx-flags`
FLAGS=-g -o3 

all: tripover2 drawtriptag 


tripover2: GrayScaleFileImageSource.o adaptivethreshold.o findellipses.o ellipsetoxy.o Coder.o Location2D.o tripover2.o 
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}


drawtriptag: Location2D.o Coder.o ellipsetoxy.o drawtriptag.o 
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}

tripover2-clean:
	-rm -f tripover2 tripover2.o GrayScaleFileImageSource.o adaptivethreshold.o findellipses.o  Coder.o Location2D.o ellipsetoxy.o

drawtriptag_clean:
	-rm -f drawtriptag drawtriptag.o Tag.o posefromcircle.o ellipsetoxy.o



tripover: tripover.o Tag.o concentricellipse.o posefromcircle.o
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}

tripover_clean:
	-rm -f tripover tripover.o Tag.o concentricellipse.o posefromcircle.o




triptest: triptest.o Tag.o concentricellipse.o posefromcircle.o
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}

triptest_clean:
	-rm -f triptest triptest.o Tag.o concentricellipse.o posefromcircle.o

%.o: %.cc
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVINC} -o $@ -c $<

clean: tripover_clean drawtriptag_clean triptest_clean tripover2-clean

depend:
	makedepend -Y. ${CPPFLAGS} *.cc 2>/dev/null

# DO NOT DELETE THIS LINE

adaptivethreshold.o: adaptivethreshold.hh Drawing.hh Config.hh
Coder.o: Coder.hh Exception.hh Config.hh
concentricellipse.o: concentricellipse.hh
drawtriptag.o: Config.hh TripOuterTag.hh Tag.hh Drawing.hh Location2D.hh
drawtriptag.o: ellipsetoxy.hh TripOriginalCoder.hh Coder.hh Exception.hh
ellipsetoxy.o: ellipsetoxy.hh Config.hh Drawing.hh
findellipses.o: findellipses.hh Config.hh Location2D.hh Drawing.hh
GrayScaleFileImageSource.o: GrayScaleFileImageSource.hh Drawing.hh Config.hh
GrayScaleFileImageSource.o: ImageSource.hh
Location2D.o: Location2D.hh Config.hh
posefromcircle.o: posefromcircle.hh
Tag.o: Tag.hh Config.hh Drawing.hh Location2D.hh
tripover2.o: GrayScaleFileImageSource.hh Drawing.hh Config.hh ImageSource.hh
tripover2.o: adaptivethreshold.hh findellipses.hh Location2D.hh
tripover2.o: TripOuterTag.hh Tag.hh ellipsetoxy.hh TripOriginalCoder.hh
tripover2.o: Coder.hh Exception.hh
tripover.o: TripOriginalTag.hh Tag.hh Config.hh Drawing.hh Location2D.hh
tripover.o: concentricellipse.hh posefromcircle.hh
triptest.o: TripOriginalTag.hh Tag.hh Config.hh Drawing.hh Location2D.hh
triptest.o: concentricellipse.hh posefromcircle.hh
