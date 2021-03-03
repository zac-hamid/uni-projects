STUFF = "Hi"

from libc.math cimport sin
from libc.math cimport cos
from libc.math cimport acos
from libc.math cimport asin
from libc.math cimport atan2
from libc.math cimport sqrt
from libc.math cimport fmin
from libc.math cimport fabs

cimport numpy as np
import numpy as np

cdef float pi = 3.14159265
cdef float rad = pi/180.0
cdef int R = 6378137

cdef np.ndarray[np.float64_t,ndim=1] _spherical2Cart(double lon, double lat):

    cdef double clat,x,y,z

    clat=(90-lat)*rad
    lon=lon*rad
    x=cos(lon)*sin(clat)
    y=sin(lon)*sin(clat)
    z=cos(clat)

    return np.array([x,y,z])

def c_spherical2Cart(double lon, double lat):

    cdef double clat,x,y,z

    clat=(90-lat)*rad
    lon=lon*rad
    x=cos(lon)*sin(clat)
    y=sin(lon)*sin(clat)
    z=cos(clat)

    return np.array([x,y,z])

cdef np.ndarray[np.float64_t,ndim=1] _cart2Spherical(double x, double y, double  z):

    cdef r, clat, lat, lon

    r=sqrt(x**2+y**2+z**2)
    clat=acos(z/r)/pi*180
    lat=90.-clat
    lon=atan2(y,x)/pi*180
    lon=(lon+360)%360

    return np.array([lon,lat])

def c_cart2Spherical(double  x, double  y, double  z):

    cdef r, clat, lat, lon

    r=sqrt(x**2+y**2+z**2)
    clat=acos(z/r)/pi*180
    lat=90.-clat
    lon=atan2(y,x)/pi*180
    lon=(lon+360)%360

    return np.array([lon,lat])


cdef double _great_circle_distance(double lon1,double lat1,double lon2,double lat2):
    """
    Usage
    -----
    Compute the great circle distance, in meter, between (lon1,lat1) and (lon2,lat2)

    Parameters
    ----------
    param lat1: float, latitude of the first point
    param lon1: float, longitude of the first point
    param lat2: float, latitude of the second point
    param lon2: float, longitude of the second point

    Returns
    -------
    d: float
       Great circle distance between (lon1,lat1) and (lon2,lat2)
    """
    cdef double dLat,dLon,a,c,d

    dLat = (lat2 - lat1)*rad
    dLon = rad*(lon2 - lon1)
    a = (sin(dLat / 2) * sin(dLat / 2) +
    cos(rad*(lat1)) * cos(rad*(lat2)) *
    sin(dLon / 2) * sin(dLon / 2))
    c = 2 * atan2(sqrt(a), sqrt(1 - a))
    d = R * c
    return d


def c_great_circle_distance(double lon1,double lat1,double lon2,double lat2):
    """
    Usage
    -----
    Compute the great circle distance, in meter, between (lon1,lat1) and (lon2,lat2)

    Parameters
    ----------
    param lon1: float, longitude of the first point
    param lat1: float, latitude of the first point
    param lon2: float, longitude of the second point
    param lat2: float, latitude of the second point

    Returns
    -------
    d: float
       Great circle distance between (lon1,lat1) and (lon2,lat2)
    """
    cdef double dLat,dLon,a,c,d

    dLat = rad*(lat2 - lat1)
    dLon = rad*(lon2 - lon1)
    a = (sin(dLat / 2) * sin(dLat / 2) +
    cos(rad*(lat1)) * cos(rad*(lat2)) *
    sin(dLon / 2) * sin(dLon / 2))
    c = 2 * atan2(sqrt(a), sqrt(1 - a))
    d = R * c
    return d


cdef double _initial_bearing(double lon1,double lat1,double lon2,double lat2):
    """
    Usage
    -----
    Bearing between (lon1,lat1) and (lon2,lat2), in degree.

    Parameters
    ----------
    param lat1: float, latitude of the first point
    param lon1: float, longitude of the first point
    param lat2: float, latitude of the second point
    param lon2: float, longitude of the second point

    Returns
    -------
    brng: float
           Bearing between (lon1,lat1) and (lon2,lat2), in degree.

    """
    cdef double dLat,dLon,x,y,ibrng

    dLon = rad*(lon2 - lon1)
    y = sin(dLon) * cos(rad*(lat2))
    x = cos(rad*(lat1))*sin(rad*(lat2)) - sin(rad*(lat1))*cos(rad*(lat2))*cos(dLon)
    ibrng = atan2(y, x)

    return ibrng

def c_initial_bearing(double lon1,double lat1,double lon2,double lat2):
    """
    Usage
    -----
    Bearing between (lon1,lat1) and (lon2,lat2), in degree.

    Parameters
    ----------
    param lat1: float, latitude of the first point
    param lon1: float, longitude of the first point
    param lat2: float, latitude of the second point
    param lon2: float, longitude of the second point

    Returns
    -------
    brng: float
           Bearing between (lon1,lat1) and (lon2,lat2), in degree.

    """
    cdef double dLat,dLon,x,y,ibrng

    dLon = rad*(lon2 - lon1)
    y = sin(dLon) * cos(rad*(lat2))
    x = cos(rad*(lat1))*sin(rad*(lat2)) - sin(rad*(lat1))*cos(rad*(lat2))*cos(dLon)
    ibrng = atan2(y, x)

    return ibrng

cdef np.ndarray[np.float64_t,ndim=1] _cross_track_point(double  lon1, double  lat1, double  lon2, double  lat2, double  lon3, double  lat3):
    '''Get the closest point on great circle path to the 3rd point

    <lat1>, <lon1>: scalar float or nd-array, latitudes and longitudes in
                    degree, start point of the great circle.
    <lat2>, <lon2>: scalar float or nd-array, latitudes and longitudes in
                    degree, end point of the great circle.
    <lat3>, <lon3>: scalar float or nd-array, latitudes and longitudes in
                    degree, a point away from the great circle.

    Return <latp>, <lonp>: latitude and longitude of point P on the great
                           circle that connects P1, P2, and is closest
                           to point P3.
    '''
    cdef double x1,y1,z1,x2,y2,z2,x3,y3,z3,D,E,F,a,b,c,f,g,h,tt,xp,yp,zp,d1,d2

    x1,y1,z1=_spherical2Cart(lon1,lat1)
    x2,y2,z2=_spherical2Cart(lon2,lat2)
    x3,y3,z3=_spherical2Cart(lon3,lat3)

    D,E,F=np.cross([x1,y1,z1],[x2,y2,z2])

    a=E*z3-F*y3
    b=F*x3-D*z3
    c=D*y3-E*x3

    f=c*E-b*F
    g=a*F-c*D
    h=b*D-a*E

    tt=sqrt(f**2+g**2+h**2)
    xp=f/tt
    yp=g/tt
    zp=h/tt

    lon1, lat1 =_cart2Spherical(xp,yp,zp)
    lon2, lat2 =_cart2Spherical(-xp,-yp,-zp)
    d1=_great_circle_distance(lon1, lat1, lon3, lat3)
    d2=_great_circle_distance(lon2, lat2, lon3, lat3)

    if d1>d2:
        return np.array([lon2, lat2])
    else:
        return np.array([lon1, lat1])


def c_cross_track_point(double  lon1, double  lat1, double  lon2, double  lat2, double  lon3, double  lat3):
    '''Get the closest point on great circle path to the 3rd point

    <lat1>, <lon1>: scalar float or nd-array, latitudes and longitudes in
                    degree, start point of the great circle.
    <lat2>, <lon2>: scalar float or nd-array, latitudes and longitudes in
                    degree, end point of the great circle.
    <lat3>, <lon3>: scalar float or nd-array, latitudes and longitudes in
                    degree, a point away from the great circle.

    Return <latp>, <lonp>: latitude and longitude of point P on the great
                           circle that connects P1, P2, and is closest
                           to point P3.
    '''
    cdef double x1,y1,z1,x2,y2,z2,x3,y3,z3,D,E,F,a,b,c,f,g,h,tt,xp,yp,zp,d1,d2

    x1,y1,z1=_spherical2Cart(lon1,lat1)
    x2,y2,z2=_spherical2Cart(lon2,lat2)
    x3,y3,z3=_spherical2Cart(lon3,lat3)

    D,E,F=np.cross([x1,y1,z1],[x2,y2,z2])

    a=E*z3-F*y3
    b=F*x3-D*z3
    c=D*y3-E*x3

    f=c*E-b*F
    g=a*F-c*D
    h=b*D-a*E

    tt=sqrt(f**2+g**2+h**2)
    xp=f/tt
    yp=g/tt
    zp=h/tt

    lon1, lat1 =_cart2Spherical(xp,yp,zp)
    lon2, lat2 =_cart2Spherical(-xp,-yp,-zp)
    d1=_great_circle_distance(lon1, lat1, lon3, lat3)
    d2=_great_circle_distance(lon2, lat2, lon3, lat3)

    if d1>d2:
        return np.array([lon2, lat2])
    else:
        return np.array([lon1, lat1])


cdef double _cross_track_distance( double lon1, double lat1, double lon2, double lat2, double lon3, double lat3):
    """
    Usage
    -----
    Angular cross-track-distance of a point (lon3, lat3) from a great-circle path between (lon1, lat1) and (lon2, lat2)
    The sign of this distance tells which side of the path the third point is on.

    Parameters :

    param lat1: float, latitude of the first point
    param lon1: float, longitude of the first point
    param lat2: float, latitude of the second point
    param lon2: float, longitude of the second point
    param lat3: float, latitude of the third point
    param lon3: float, longitude of the third point

    Usage
    -----
    crt: float
         the (angular) c_cross_track_distance

    """
    cdef double d13,theta13,theta12,crt

    d13=_great_circle_distance(lon1,lat1,lon3,lat3) # distance from start point to third point
    theta13=_initial_bearing(lon1,lat1,lon3,lat3) # bearing from start point to third point
    theta12=_initial_bearing(lon1,lat1,lon2,lat2) # bearing from start point to end point

    crt= asin(sin(d13/R)*sin(theta13-theta12))*R

    return crt

def c_cross_track_distance( double lon1, double lat1, double lon2, double lat2, double lon3, double lat3):
    """
    Usage
    -----
    Angular cross-track-distance of a point (lon3, lat3) from a great-circle path between (lon1, lat1) and (lon2, lat2)
    The sign of this distance tells which side of the path the third point is on.

    Parameters :

    param lat1: float, latitude of the first point
    param lon1: float, longitude of the first point
    param lat2: float, latitude of the second point
    param lon2: float, longitude of the second point
    param lat3: float, latitude of the third point
    param lon3: float, longitude of the third point

    Usage
    -----
    crt: float
         the (angular) c_cross_track_distance

    """
    cdef double d13,theta13,theta12,crt


    d13=_great_circle_distance(lon1,lat1,lon3,lat3) # distance from start point to third point
    theta13=_initial_bearing(lon1,lat1,lon3,lat3) # bearing from start point to third point
    theta12=_initial_bearing(lon1,lat1,lon2,lat2) # bearing from start point to end point

    crt= asin(sin(d13/R)*sin(theta13-theta12))*R

    return crt

cdef double _along_track_distance(double crt,double lon1,double lat1,double lon3,double lat3):
    """
    Usage
    -----
    The along-track distance from the start point (lon1, lat1) to the closest point on the the path
    to the third point (lon3, lat3).

    Parameters
    ----------
    param lat1: float, latitude of the first point
    param lon1: float, longitude of the first point
    param lat3: float, latitude of the third point
    param lon3: float, longitude of the third point
    param crt : float, c_cross_track_distance

    Returns
    -------
    alt: float
         The along-track distance
    """

    cdef double d13,alt

    d13=_great_circle_distance(lon1,lat1,lon3,lat3)

    alt=acos(cos(d13/R)/cos(crt/R)) * R
    return alt

def c_along_track_distance(double crt,double lon1,double lat1,double lon3,double lat3):
    """
    Usage
    -----
    The along-track distance from the start point (lon1, lat1) to the closest point on the the path
    to the third point (lon3, lat3).

    Parameters
    ----------
    param lat1: float, latitude of the first point
    param lon1: float, longitude of the first point
    param lat3: float, latitude of the third point
    param lon3: float, longitude of the third point
    param crt : float, c_cross_track_distance

    Returns
    -------
    alt: float
         The along-track distance
    """

    cdef double d13,alt

    d13=_great_circle_distance(lon1,lat1,lon3,lat3)

    alt=acos(cos(d13/R)/cos(crt/R)) * R
    return alt


cdef double _point_to_path(double lon1,double lat1,double lon2,double lat2,double lon3,double lat3):
    """
    Usage
    -----
    The point-to-path distance between point (lon3, lat3) and path delimited by (lon1, lat1) and (lon2, lat2).
    The point-to-path distance is the cross_track distance between the great circle path if the projection of
    the third point lies on the path. If it is not on the path, return the minimum of the
    c_great_circle_distance between the first and the third or the second and the third point.

    Parameters
    ----------
    param lat1: float, latitude of the first point
    param lon1: float, longitude of the first point
    param lat2: float, latitude of the second point
    param lon2: float, longitude of the second point
    param lat3: float, latitude of the third point
    param lon3: float, longitude of the third point

    Returns
    -------

    ptp : float
          The point-to-path distance between point (lon3, lat3) and path delimited by (lon1, lat1) and (lon2, lat2)

    """
    cdef double crt,d1p,d2p,d12
    crt=_cross_track_distance(lon1,lat1,lon2,lat2,lon3,lat3)
    d1p=_along_track_distance(crt,lon1,lat1,lon3,lat3)
    d2p=_along_track_distance(crt,lon2,lat2,lon3,lat3)
    d12=_great_circle_distance(lon1,lat1,lon2,lat2)
    if (d1p > d12) or (d2p > d12):
        crt=fmin(_great_circle_distance(lon1,lat1,lon3,lat3),_great_circle_distance(lon2,lat2,lon3,lat3))
    else:
        crt=fabs(crt)
    return crt

def c_point_to_path(double lon1,double lat1,double lon2,double lat2,double lon3,double lat3):
    """
    Usage
    -----
    The point-to-path distance between point (lon3, lat3) and path delimited by (lon1, lat1) and (lon2, lat2).
    The point-to-path distance is the cross_track distance between the great circle path if the projection of
    the third point lies on the path. If it is not on the path, return the minimum of the
    c_great_circle_distance between the first and the third or the second and the third point.

    Parameters
    ----------
    param lat1: float, latitude of the first point
    param lon1: float, longitude of the first point
    param lat2: float, latitude of the second point
    param lon2: float, longitude of the second point
    param lat3: float, latitude of the third point
    param lon3: float, longitude of the third point

    Returns
    -------

    ptp : float
          The point-to-path distance between point (lon3, lat3) and path delimited by (lon1, lat1) and (lon2, lat2)

    """
    cdef double crt,d1p,d2p,d12
    crt=_cross_track_distance(lon1,lat1,lon2,lat2,lon3,lat3)
    d1p=_along_track_distance(crt,lon1,lat1,lon3,lat3)
    d2p=_along_track_distance(crt,lon2,lat2,lon3,lat3)
    d12=_great_circle_distance(lon1,lat1,lon2,lat2)
    if (d1p > d12) or (d2p > d12):
        crt=fmin(_great_circle_distance(lon1,lat1,lon3,lat3),_great_circle_distance(lon2,lat2,lon3,lat3))
    else:
        crt=fabs(crt)
    return crt
