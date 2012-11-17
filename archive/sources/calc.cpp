#include "all.h"

static LD A = 6378245.0;
static LD E2 = 0.0066934216;
static LD E12 = E2 / (1 - E2);
static LD E1 = (1 - sqrt(1 - E2)) / (1 + sqrt(1 - E2));
static LD FALSENORTHING = 0;
static LD FALSEEASTING = 500000;
static LD SCALEFACTOR = 1;

LD degToRad(LD x){
	return x*M_PI/180;
}

LD radToDeg(LD x){
	return x*180/M_PI;
}

static inline LD foo(LD f0){
	LD z=(1-E2/4-3*pow(E2,2)/64-5*pow(E2,3)/256)*f0;
	z-=(3*E2/8+3*pow(E2,2)/32+45*pow(E2,3)/1024)*sin(2*f0);
	z+=(15*pow(E2,2)/256+45*pow(E2,3)/1024)*sin(4*f0);
	z-=(35*pow(E2,3)/3072)*sin(6*f0);
	return z;
}

LD lonToZone(LD lon){
	return ceil(lon/6);
}

point<LD> geographicalToRectangular(point<LD> P,LD zone){
	LD lat=P.x, lon=P.y;
	LD lon0=degToRad(zone*6-3);	
	lat=degToRad(lat), lon=degToRad(lon);	
	LD z=1-E2*pow(sin(lat),2);
	LD v1=A/sqrt(z);
	LD a=(lon-lon0)*cos(lat);
	LD T=pow(tan(lat),2);
	LD C=E12*pow(cos(lat),2);
	LD M0=A*foo(0);
	LD M=A*foo(lat);
	z=a+(1-T+C)*pow(a,3)/6+(5-18*T+pow(T,2)+72*C-58*E12)*pow(a,5)/120;
	LD y=FALSEEASTING+SCALEFACTOR*v1*z+zone*1000000;
	z=pow(a, 2)/2+(5-T+9*C+4*pow(C,2))*pow(a,4)/24+(61-58*T+pow(T,2)+600*C-330*E12)*pow(a,6)/720;
	LD x=FALSENORTHING+SCALEFACTOR*(M-M0+v1*tan(lat)*z);
	return point<LD>(x,y);
}

point<LD> rectangularToGeographical(point<LD> P,LD zone){
	LD x=P.x,y=P.y;
	y-=zone*1000000;
	LD lon0=zone*6-3;
	LD M0=A*foo(0);
	LD M1=M0+(x-FALSENORTHING)/SCALEFACTOR;
	LD m11=M1/(A*(1-E2/4-3*pow(E2,2)/64-5*pow(E2,3)/256));
	LD z=m11+(3*E1/2-27*pow(E1,3)/32)*sin(2*m11);
	z+=(21*pow(E1,2)/16-55*pow(E1,4)/32)*sin(4*m11);
	z+=(151*pow(E1,3)/96)*sin(6*m11);
	LD f1=z+(1097*pow(E1,4)/512)*sin(8*m11);
	z=pow(sin(f1),2);
	z=1-E2*z;
	LD p1=(A*(1-E2))/(sqrt(pow(z,3)));
	LD v1=A/sqrt(z);
	LD T1=pow(tan(f1),2);
	LD C1=E12*pow(cos(f1),2);
	LD D=(y-FALSEEASTING)/(v1*SCALEFACTOR);
	z=pow(D,2)/2-(5+3*T1+10*C1-4*pow(C1,2)-9*E12)*pow(D,4)/24;
	z=z+(61+90*T1+298*C1+45*pow(T1,2)-252*E12-3*pow(C1,2))*pow(D,6)/720;
	LD f=f1-(v1*tan(f1)/p1)*z;
	z=D-(1+2*T1+C1)*pow(D,3)/6+(5-2*C1+28*T1-3*pow(C1,2)+8*E12+24*pow(T1,2))*pow(D,5)/120;
	return point<LD>(radToDeg(f),radToDeg(z/cos(f1))+lon0);
}