OPENCVINC=`params --opencv-inc`
OPENCVLIB=`params --opencv-lib`
OPENCVLINK=`params --opencv-link`
CPPFLAGS=`params --opencv-cxx-flags`
FLAGS=-g -o3 


all: triptest2  drawtriptag 

triptest2: triptest2.o Coder.o ellipsetoxy.o Location2D.o GrayScaleFileImageSource.o adaptivethreshold.o findellipses.o
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}

drawtriptag: Location2D.o Coder.o ellipsetoxy.o drawtriptag.o 
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVLIB} -o $@ $^ ${OPENCVLINK}


%.o: %.cc
	g++ ${FLAGS} ${CPPFLAGS} ${OPENCVINC} -o $@ -c $<

clean:
	-rm -f *.o triptest2 drawtriptag

depend:
	makedepend -Y. ${CPPFLAGS} *.cc 2>/dev/null

# DO NOT DELETE THIS LINE

3DLocation.o: 3DLocation.hh Config.hh
adaptivethreshold.o: adaptivethreshold.hh Drawing.hh Config.hh
Coder.o: Coder.hh Exception.hh Config.hh
concentricellipse.o: concentricellipse.hh
drawtriptag.o: Config.hh Drawing.hh TripOriginalCoder.hh Coder.hh
drawtriptag.o: Exception.hh RingTag.hh Tag.hh Location2D.hh ellipsetoxy.hh
ellipsetoxy.o: ellipsetoxy.hh Config.hh Drawing.hh
findellipses.o: findellipses.hh Config.hh Location2D.hh Drawing.hh
GrayScaleFileImageSource.o: GrayScaleFileImageSource.hh Drawing.hh Config.hh
GrayScaleFileImageSource.o: ImageSource.hh
Location2D.o: Location2D.hh Config.hh
posefromcircle.o: posefromcircle.hh
Tag.o: Tag.hh Config.hh Drawing.hh Location2D.hh
triptest2.o: Config.hh Drawing.hh TripOriginalCoder.hh Coder.hh Exception.hh
triptest2.o: RingTag.hh Tag.hh Location2D.hh ellipsetoxy.hh
triptest2.o: GrayScaleFileImageSource.hh ImageSource.hh adaptivethreshold.hh
triptest2.o: findellipses.hh
triptest.o: Tag.hh Config.hh Drawing.hh Location2D.hh
