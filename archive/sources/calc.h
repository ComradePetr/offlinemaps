inline LD degToRad(LD x){
	return x*M_PI/180;
}

inline LD radToDeg(LD x){
	return x*180/M_PI;
}

inline LD lonToZone(LD lon){
	return ceil(lon/6);
}


point<LD> geographicalToRectangular(point<LD>,LD);
point<LD> rectangularToGeographical(point<LD>,LD);