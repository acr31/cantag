#define RING_COUNT 2

/* The radii of each code ring as a factor of the radius of the outer
   edge of the bullseye.  Multiplying the radius of the outer edge of
   the bullseye by each RADII should give a radius in the middle of
   the code ring */
double radii_centre[] = {1.2,1.8};
double radii_outer[] = {1.4,2};
double radii_inner[] = {1.1,1.5};

/* The angle of each sector in radians */
#define SECTOR_COUNT 16
double sector_angles[] = {0,
			  0.392699081698724,
			  0.785398163397448,
			  1.17809724509617,
			  1.5707963267949,
			  1.96349540849362,
			  2.35619449019234,
			  2.74889357189107,
			  3.14159265358979,
			  -2.74889357189107,
			  -2.35619449019234,
			  -1.96349540849362,
			  -1.5707963267949,
			  -1.17809724509617,
			  -0.785398163397448,
			  -0.392699081698725};

/* The angles to try when looking for synchronization sector */
#define SYNC_COUNT 24
double sync_angles[] = {0,
			0.261799387799149,
			0.523598775598299,
			0.785398163397448,
			1.0471975511966,
			1.30899693899575,
			1.5707963267949,
			1.83259571459405,
			2.0943951023932,
			2.35619449019234,
			2.61799387799149,
			2.87979326579064,
			3.14159265358979,
			-2.87979326579064,
			-2.61799387799149,
			-2.35619449019234,
			-2.0943951023932,
			-1.83259571459405,
			-1.5707963267949,
			-1.30899693899575,
			-1.0471975511966,
			-0.785398163397448,
			-0.523598775598299,
			-0.261799387799149};
