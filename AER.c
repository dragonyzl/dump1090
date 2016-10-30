

#include "AER.h"

#define RAD2DEG 57.295779513082323
//static double ELLIPSOID[2] = { 6.378137e6, 8.181919084262149e-02 };

/*  In order to Select the certain airplane and send the AER to the servo for guiding the antenna. I use the tmpfile
 *  to store the printed info in the interactive mode. the file content is updated every screen refresh. and a bash file
 *  is called by system() function and grep the selected airplane number and send the AER to the servo.
 *  the airenumber can be written to a file in the guiding by hand.  This maybe not a perfect method. but I have no time
 *  to optimize it.    Draognyzl 2014.09.01
 * */

/*  For display the current time in interactive mode  */
//time_t CurrentTime;
//long long TimeIndex;		/*TimeIndex is using for representing the time offset to the start time of the program. */
//struct tm *CurrentTime_p;


/*
 function [x, y, z] = geodetic2ecef(phi, lambda, h, ellipsoid)
%GEODETIC2ECEF Convert geodetic to geocentric (ECEF) coordinates
%
%   [X, Y, Z] = GEODETIC2ECEF(PHI, LAMBDA, H, ELLIPSOID) converts geodetic
%   point locations specified by the coordinate arrays PHI (geodetic
%   latitude in radians), LAMBDA (longitude in radians), and H (ellipsoidal
%   height) to geocentric Cartesian coordinates X, Y, and Z.  The geodetic
%   coordinates refer to the reference ellipsoid specified by ELLIPSOID (a
%   row vector with the form [semimajor axis, eccentricity]).  H must use
%   the same units as the semimajor axis;  X, Y, and Z will be expressed in
%   these units also.
%
%   The geocentric Cartesian coordinate system is fixed with respect to the
%   Earth, with its origin at the center of the ellipsoid and its X-, Y-,
%   and Z-axes intersecting the surface at geodetic coordinate (0,0) --
%   equator at the prime meridian, (0, pi/2) -- equator at 90-degrees east,
%   and (pi/2, 0) -- north pole, respectively.  A common synonym is
%   Earth-Centered, Earth-Fixed coordinates, or ECEF.
%
%   See also ECEF2GEODETIC, ECEF2LV, GEODETIC2GEOCENTRICLAT, LV2ECEF.

% Copyright 2004-2008 The MathWorks, Inc.
% $Revision: 1.1.6.3 $  $Date: 2008/06/16 17:02:05 $

% Reference
% ---------
% Paul R. Wolf and Bon A. Dewitt, "Elements of Photogrammetry with
% Applications in GIS," 3rd Ed., McGraw-Hill, 2000 (Appendix F-3).
*/

void geodetic2ecef (double phi, double lambda, double h, double *ellipsoid,double *x, double *y, double *z)
{

  double a = ellipsoid[0];
  double e2 = powf (ellipsoid[1], 2.0);
  double sinphi = sin (phi);
  double cosphi = cos (phi);
  double N = a / sqrt (1 - e2 * powf (sinphi, 2.));
  *x = (N + h) * cosphi * cos (lambda);
  *y = (N + h) * cosphi * sin (lambda);
  *z = (N * (1 - e2) + h) * sinphi;
}



/*
function [x, y, z] = ecef2lv(x, y, z, phi0, lambda0, h0, ellipsoid)
%ECEF2LV Convert geocentric (ECEF) to local vertical coordinates
%
%   [XL, YL, ZL] = ECEF2LV(X, Y, Z, PHI0, LAMBDA0, H0, ELLIPSOID) converts
%   geocentric point locations specified by the coordinate arrays X, Y, and
%   Z to the local vertical coordinate system with its origin at geodetic
%   latitude PHI0, longitude LAMBDA0, and ellipsoidal height H0.  X, Y, and
%   Z may be arrays of any shape, as long as they all match in size. PHI0,
%   LAMBDA0, and H0 must be scalars.  ELLIPSOID is a row vector with the
%   form [semimajor axis, eccentricity].  X, Y, Z, and H0 must have the
%   same length units as the semimajor axis.  PHI0 and LAMBDA must be in
%   radians. The output coordinate arrays, XL, YL, and ZL are the local
%   vertical coordinates of the input points.  They have the same size as
%   X, Y, and Z and have the same length units as the semimajor axis.
%
%   In the local vertical Cartesian system defined by PHI0, LAMBD0, H0, and
%   ELLIPSOID, the XL axis is parallel to the plane tangent to the
%   ellipsoid at (PHI0, LAMBDA0) and points due east.  The YL axis is
%   parallel to the same plane and points due north.  The ZL axis is normal
%   to the ellipsoid at (PHI0, LAMBDA0) and points outward into space. The
%   local vertical system is sometimes referred to as east-north-up or ENU.
%
%   For a definition of the geocentric system, also known as
%   Earth-Centered, Earth-Fixed (ECEF), see the help for GEODETIC2ECEF.
%
% See also ECEF2GEODETIC, ELEVATION, GEODETIC2ECEF, LV2ECEF.

% Copyright 2004-2006 The MathWorks, Inc.
% $Revision: 1.1.6.2 $  $Date: 2006/05/24 04:06:47 $

% Reference
% ---------
% Paul R. Wolf and Bon A. Dewitt, "Elements of Photogrammetry with
% Applications in GIS," 3rd Ed., McGraw-Hill, 2000 (Appendix F-4).

% Locate the origin of the local vertical system in geocentric coordinates.
*/

void
ecef2lv (double phi0, double lambda0, double h0, double *ellipsoid, double *x,
	 double *y, double *z)
{
  double x0;
  double y0;
  double z0;
  double P[3];
  double PP[3];
  double M[9];
  double m1[9];
  double m2[9];
  geodetic2ecef (phi0, lambda0, h0, ellipsoid, &x0, &y0, &z0);

/*
 * % Construct the matrix that rotates Cartesian vectors from geocentric to
% local vertical.
gsl_Matrix M = ecef2lvRotationMatrix(phi0, lambda0);

% Construct a work array, p, to hold the offset vectors from the local
% vertical origin to the point locations defined by x, y, z.  Each column
% of p is a 3-vector oriented relative to the geocentric system.
*/
  /*n = numel(x); */
  P[0] = *x - x0;
  P[1] = *y - y0;
  P[2] = *z - z0;
/*P(1,:) = reshape(x - x0, [1 n]);
P(2,:) = reshape(y - y0, [1 n]);
P(3,:) = reshape(z - z0, [1 n]);*/

/*% Rotate each column of P into the local vertical system, overwriting P
% itself to save storage.*/
  double sinphi0 = sin (phi0);
  double cosphi0 = cos (phi0);
  double sinlambda0 = sin (lambda0);
  double coslambda0 = cos (lambda0);


  m1[0] = 1.0;
  m1[1] = 0.0;
  m1[2] = 0.0;
  m1[3] = 0;
  m1[4] = sinphi0;
  m1[5] = cosphi0;
  m1[6] = 0.0;
  m1[7] = -cosphi0;
  m1[8] = sinphi0;

  m2[0] = -sinlambda0;
  m2[1] = coslambda0;
  m2[2] = 0.0;
  m2[3] = -coslambda0;
  m2[4] = -sinlambda0;
  m2[5] = 0.;
  m2[6] = 0.0;
  m2[7] = 0.0;
  m2[8] = 1.0;


  cblas_dgemm (CblasRowMajor, CblasNoTrans, CblasNoTrans, 3, 3, 3, 1.0, m1, 3,
	       m2, 3, 0.0, M, 3);
  cblas_dgemm (CblasRowMajor, CblasNoTrans, CblasNoTrans, 3, 1, 3, 1.0, M, 3,
	       P, 1, 0.0, PP, 1);
/*return  [    1      0        0   ; ...
 *             0   sinphi0  cosphi0; ...
 *             0  -cosphi0  sinphi0] ...
 *           
 *      * [-sinlambda0   coslambda0  0; ...
 *         -coslambda0  -sinlambda0  0; ...
 *               0          0        1];


P== M * P;

*/
  *x = PP[0];
  *y = PP[1];
  *z = PP[2];
}


void
elevation (double lat1, double lon1, double h1, double lat2, double lon2,
	   double h2, double *ellipsoid, double *azimuthAngle,
	   double *elevationAngle, double *slantRange)
{
/*
function [elevationAngle, slantRange, azimuthAngle] = elevation(varargin)
%ELEVATION Local vertical elevation angle, range, and azimuth
%
%   [ELEVATIONANGLE, SLANTRANGE, AZIMUTHANGLE] = ELEVATION(LAT1, LON1, ...
%   ALT1, LAT2, LON2, ALT2) computes the elevation angle, slant range, and
%   azimuth angle of point 2 (with geodetic coordinates LAT2, LON2, and
%   ALT2) as viewed from point 1 (with geodetic coordinates LAT1, LON1, and
%   ALT1).  ALT1 and ALT2 are ellipsoidal heights.  The elevation angle is
%   the angle of the line of sight above the local horizontal at point 1.
%   The slant range is the three-dimensional Cartesian distance between
%   point 1 and point 2.  The azimuth is the angle from north to the
%   projection of the line of sight on the local horizontal. Angles are in
%   units of degrees, altitudes and distances are in meters. The figure of
%   the earth is the default ellipsoid (GRS 80) as defined by ALMANAC.
%
%   Inputs can be vectors of points, or arrays of any shape, but must match
%   in size, with the following exception:  Elevation, range, and azimuth
%   from a single point to a set of points can be computed very efficiently
%   by providing scalar coordinate inputs for point 1 and vectors or arrays
%   for point 2.
%
%   [...] = ELEVATION(LAT1,LON1, ALT1, LAT2, LON2, ALT2, ANGLEUNITS) uses
%   the string ANGLEUNITS to specify the units of the input and output
%   angles.  If omitted, 'degrees' is assumed.
%
%   [...] = ELEVATION(LAT1, LON1, ALT1, LAT2, LON2, ALT2, ANGLEUNITS,...
%   DISTANCEUNITS) uses the string DISTANCEUNITS to specify the altitude
%   and slant-range units.  If omitted, 'meters' is assumed.  Any units
%   string recognized by UNITSRATIO may be used.
%
%   [...] = ELEVATION(LAT1, LON1, ALT1, LAT2, LON2, ALT2, ANGLEUNITS,...
%   ELLIPSOID) uses the vector ELLIPSOID, with form [semimajor axis,
%   eccentricity], to specify the ellipsoid.  If ELLIPSOID is supplied, the
%   altitudes must be in the same units as the semimajor axis and the slant
%   range will be returned in these units.  If ELLIPSOID is omitted, the
%   default earth ellipsoid defined by AZIMUTH is used and distances are in
%   meters unless otherwise specified.
%
%   Note
%   ----
%   The line-of-sight azimuth angles returned by ELEVATION will generally
%   differ slightly from the corresponding outputs of AZIMUTH and DISTANCE,
%   except for great-circle azimuths on a spherical earth.
%
%   See also ALMANAC, AZIMUTH, DISTANCE.

% Copyright 1999-2007 The MathWorks, Inc.
% $Revision: 1.6.4.8 $  $Date: 2007/11/09 20:23:33 $


% Perform coordinate transformations.*/

  double x, y, z;
  double phi1 = lat1 / RAD2DEG;
  double phi2 = lat2 / RAD2DEG;
  double lambda1 = lon1 / RAD2DEG;
  double lambda2 = lon2 / RAD2DEG;

  geodetic2ecef (phi2, lambda2, h2, ellipsoid, &x, &y, &z);
/*if numel(phi1) == 1
    % vectorized computation for scalar point 1
    [x,y,z] = ecef2lv(x,y,z,phi1,lambda1,h1,ellipsoid);*/
  ecef2lv (phi1, lambda1, h1, ellipsoid, &x, &y, &z);

/*% Convert to spherical coordinates but with azimuth defined clockwise
% from north (y) rather than counter clockwise from east (x) and in the
% interval [0 2*pi).  This the same as the computation in CART2SPH,
% except for the azimuth convention.*/
  double r = hypot (x, y);
  *slantRange = hypot (r, z);
  *elevationAngle = atan2 (z, r) * RAD2DEG;

  *azimuthAngle = fmod (atan2 (x, y), 2 * M_PI) * RAD2DEG;
  if (*azimuthAngle < 0.0)
      *azimuthAngle += 360.0;
/*% The azimuth is undefined when point 1 is at a pole, so we choose a
% convention: zero at the north pole and pi at the south pole.*/

  if (phi1 <= -M_PI / 2.0)
    *azimuthAngle = 0;
  if (phi1 >= M_PI / 2.0)
    *azimuthAngle = 180.;
}


float EstimateDopplerAt1G(double az,double el,double speed,double TrackAngle)
{


	float freq = 1e9;// 1GHz
	float lambda = 3e8/freq;

	/*int gsl_blas_ddot (const gsl vector * x , const gsl vector * y , double *result )*/
	//	These functions compute the scalar product x T y for the vectors x and y, returning
	//	the result in result.
	
	
	
	// In Radar coodinates, construct the directional vector for the LOS and the airplane's speed
	gsl_vector *LOS = gsl_vector_alloc(3);
	// Cordinates system : NORTH-EAST-SKY
	gsl_vector_set (LOS, 0, cos( el/RAD2DEG ) * sin(az / RAD2DEG)  );
	gsl_vector_set (LOS, 1, cos( el/RAD2DEG ) * cos(az / RAD2DEG)  );
	gsl_vector_set (LOS, 2, sin( el/RAD2DEG )   );
	
	// For airplane, Assume that the el angle is zero for simplicity.
	// for higher precision, the earth curvature should be considered.
	gsl_vector *velocity = gsl_vector_alloc(3);
	gsl_vector_set (velocity, 0, cos( -el/RAD2DEG ) * sin(TrackAngle / RAD2DEG)  );
        gsl_vector_set (velocity, 1, cos( -el/RAD2DEG ) * cos(TrackAngle / RAD2DEG)  );
	gsl_vector_set (velocity, 2, sin( -el/RAD2DEG )   );

	double RadialVelocity,Temp;
	gsl_blas_ddot (LOS ,velocity , &Temp );
	RadialVelocity = speed * Temp;
	gsl_vector_free(LOS);
	gsl_vector_free(velocity);
	return (float)(-2*RadialVelocity/lambda );

}

int IsAzInRange(double az, double start,double stop)
{
	// the Azimuth angle is wraped at north form 360 to 0;
	if( stop > start && az > start && az< stop  ) 
	  return 1;   
	else if(stop < start && (az > start  || az < stop ) )
	{
		return 1;
	}
	else
	  return 0;
}
	

