#include "all.h"
#include "map-parser.h"
#include "calc.h"

typedef vector< pair< point<LD>,point<LD> > > vec_ldld;

const LD INF=1e18,eps=0.08181333387657540019556205937723;

point<LD> v,st;
LD zone;

QImage *inImage,*outImage;

int main(int argc, char *argv[]){
	freopen("output.txt","w",stdout);
	if(argc==1){
		printf("USAGE: project.exe name-of-map-file.map");
		return 0;
	}
	
	pair<const char*,vec_ldld> parsed=parse(argv[1]);
	const char *ImageName=parsed.first;
	vec_ldld input=parsed.second;
//	LL left=INF, right=-INF, top=INF, bottom=-INF;
	point<LD> c1[2],c2[2];

	for(int i=0;i<2;++i){
		c1[i]=input[i].first;
		if(!i)
			zone=lonToZone(input[i].second.y);
//		left=min(left,(LL)c1[i].x), right=max(right,(LL)c1[i].x), top=min(top,(LL)c1[i].y), bottom=max(bottom,(LL)c1[i].y);
		point<LD> t=geographicalToRectangular(input[i].second,zone);
		c2[i].x=t.y, c2[i].y=-t.x; //http://upload.wikimedia.org/wikipedia/commons/4/44/Descartes_system_3D.png?uselang=ru
	}
	point<LD> c=c1[0]-c1[1], d=c2[0]-c2[1];
	v=point<LD>(c^d,c*d)/c.length2();
	st=point<LD>(c2[0].x-(c1[0]^v),c2[0].y-(c1[0]*v));

	inImage=new QImage(ImageName);
	int W=inImage->width(), H=inImage->height();
//	int W=right-left+1, H=bottom-top+1;
	point<LD> *B=new point<LD>[W*H];
	outImage=new QImage(W,H,QImage::Format_RGB32);

	LD MinX=INF, MinY=INF, MaxX=-INF, MaxY=-INF;
//	forab(y,top,bottom+1)
//		forab(x,left,right+1){	
	int pos=0;
	point<LD> t1,t2;
	forn(y,H)
		forn(x,W){
			t1=point<LD>(x,y);
			t2=st+point<LD>(t1^v,t1*v);
			t2=rectangularToGeographical(point<LD>(-t2.y,t2.x),zone);
			t2.x=degToRad(t2.x), t2.y=degToRad(t2.y);
			B[pos]=point<LD>(t2.y,arctanh(sin(t2.x))-eps*arctanh(eps*sin(t2.x)));

			MinX=min(MinX,B[pos].x), MaxX=max(MaxX,B[pos].x);
			MinY=min(MinY,B[pos].y), MaxY=max(MaxY,B[pos].y);
			++pos;
		}
	LD Scale=min((W-1)/(MaxX-MinX),(H-1)/(MaxY-MinY));
	LL a,b;
//	forab(y,top,bottom+1)
//		forab(x,left,right+1){	
	pos=0;
	forn(y,H)
		forn(x,W){
			a=(B[pos].x-MinX)*Scale, b=(MaxY-B[pos].y)*Scale;
			outImage->setPixel(a,b,inImage->pixel(x,y));
			++pos;
		}

	strcpy(strrchr(ImageName,'.'),"-out.jpg");
	outImage->save(ImageName);

	return 0;
}