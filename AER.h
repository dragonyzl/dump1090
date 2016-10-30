#ifndef __AER_H
#define __AER_H

#include <stdio.h>
#ifndef _WIN32
#include <termios.h>
#endif

#ifdef _WIN32
#include "winstubs.h"
#endif

#include <math.h>
#include <time.h>
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_vector.h>
#ifdef __cplusplus
extern "C" {
#endif
void
geodetic2ecef (double phi, double lambda, double h, double *ellipsoid,
	       double *x, double *y, double *z);

void
ecef2lv (double phi0, double lambda0, double h0, double *ellipsoid, double *x,
	 double *y, double *z);

void
elevation (double lat1, double lon1, double h1, double lat2, double lon2,
	   double h2, double *ellipsoid, double *azimuthAngle,
	   double *elevationAngle, double *slantRange);
int IsAzInRange( double az, double start,double stop);

float EstimateDopplerAt1G(double az,double el,double speed,double TrackAngle);
#ifdef __cplusplus
}
#endif

#endif
