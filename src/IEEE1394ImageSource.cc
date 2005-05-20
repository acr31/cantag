/**
 * $Header$
 */

#include <total/IEEE1394ImageSource.hh>

extern "C" {
#include <sys/types.h>  // for open()
#include <sys/stat.h>   // for open()
#include <fcntl.h>      // for open()
#include <sys/ioctl.h>  // for ioctl()
#include <unistd.h>     // for mmap()
#include <sys/mman.h>   // for mmap()
}

#include <cerrno>
#include <iostream>

#undef IEEE1394_DEBUG

namespace Total {


  IEEE1394ImageSource::IEEE1394ImageSource(
					   char *device_name,
					   int port,
					   u_int64_t guid
					   ) :mImage(0)
  {
    
    // Create a handle to the specified port
     mHandle = dc1394_create_handle(port);

     int nCam = 0;
     nodeid_t * camera_nodes = dc1394_get_camera_nodes(mHandle, &nCam, 0);
     /*
       \todo
       if (nCam==0) ERROR
     */
     if (nCam>1) {
       // More than one camera - use guid to select
       dc1394_camerainfo info;
       for (int i=0; i<nCam; i++) {
	 if (dc1394_get_camera_info(mHandle, camera_nodes[i], &info) == DC1394_SUCCESS) {
	   if (info.euid_64 == guid) {
	     dc1394_print_camera_info(&info);
	     mCamera.node = camera_nodes[i];
	     break;
	   }
	 }
       }
     } 
     else {
       mCamera.node = camera_nodes[0];
     }
     dc1394_free_camera_nodes(camera_nodes);
     
     // Now we want to set up the capture parameters
     // \todo check this call!
    //  dc1394_setup_capture(mHandle, mCamera.node,
// 			  0, /* channel */ 
// 			  FORMAT_VGA_NONCOMPRESSED,
// 			  MODE_640x480_MONO,
// 			  SPEED_400,
// 			  FRAMERATE_7_5,
// 			  &mCamera);
     dc1394_dma_setup_capture(mHandle, mCamera.node,
			      0, /* channel */ 
			      FORMAT_VGA_NONCOMPRESSED,
			      MODE_640x480_MONO,
			      SPEED_400,
			      FRAMERATE_30,
			      8,1,  
			      device_name,
			      &mCamera);

     // Start the transmission
     dc1394_start_iso_transmission(mHandle, mCamera.node);
  }

  
  IEEE1394ImageSource::~IEEE1394ImageSource() {
    dc1394_dma_release_camera(mHandle,&mCamera);
    dc1394_destroy_handle(mHandle);
  }


  Image * IEEE1394ImageSource::Next() {
    /*
     * This is what we would like,
     * but we can't write to the memory
     * yet  \todo
     
     if (mImage) {
     dc1394_dma_done_with_buffer(&mCamera);
     delete mImage;
     }
     dc1394_dma_single_capture(&mCamera);
     mImage = new Image(640,480,640,(unsigned char *)mCamera.capture_buffer);
     return mImage;
    */

    // Doing a copy instead at the moment!!
    if (!mImage) {
      mImage = new Image(640,480);
    }
    dc1394_dma_single_capture(&mCamera);
    unsigned char *img = (unsigned char*) mCamera.capture_buffer;
    for (int i=0; i<640; i++) {
      for (int j=0; j<480; j++) {
	mImage->DrawPixelNoCheck(i,j,img[i+j*640]);
      }
    }
    dc1394_dma_done_with_buffer(&mCamera);
    return mImage;
  }



}
