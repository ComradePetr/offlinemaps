#include "all.h"
#include "map-parser.h"
#include "calc.h"

#include "interpolation.h"
using namespace alglib;

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

const LD INF=1e18,eps=0.08181333387657540019556205937723;

point<LD> v,st;

QImage *inImage,*outImage;

real_1d_array X,Y;
real_2d_array Z;

const int
	CFF[2][4]={{1,0,-1,1},{0,1,1,1}},
	ITERS_CNT=2;

int main(int argc, char *argv[]){
	freopen("output.txt","w",stdout);
	if(argc==1){
		puts("USAGE: project.exe name-of-map-file.map");
		return 0;
	}

	pair<const char*,parseOut> parsed=parse(argv[1]);
	const char *ImageName=parsed.first;
	parseOut input=parsed.second;
	int L=input.size();
	if(L<=1){
		puts("Not enough points");
		return 0;
	}

	Y.setlength(2*L), Z.setlength(2*L,4);
	LD zone=lonToZone(input[0].snd.y);
	forn(i,L){
		forn(j,2)
			forn(k,4)
				Z[2*i+j][k]=LD(CFF[j][k]*(k<2?1:input[i].fst[!(j^(k-2))]));
		point<LD> t=geographicalToRectangular(input[i].snd,zone);
		t.geoToCart();
		forn(j,2)
			Y[2*i+j]=t[j];
	}

	ae_int_t info;
	lsfitreport rep;
	lsfitlinear(Y,Z,info,X,rep);
	if(info!=1){
		puts("Can't solve matrix");
		return 0;
	}
	st=point<LD>(X[0],X[1]), v=point<LD>(X[2],X[3]);

	struct stat imstat;
	stat(ImageName,&imstat);
	int sz=imstat.st_size;
	FILE *imageFile=fopen(ImageName,"rb");
	uchar *buff=new uchar[sz];
	fread(buff,sizeof(uchar),sz,imageFile);
	fclose(imageFile);

	inImage=new QImage();
	inImage->loadFromData(buff,sz);
	if(inImage->isNull()){
		puts("Can't open image");
		return 0;
	}

	int W=inImage->width(), H=inImage->height();
	outImage=new QImage(W,H,QImage::Format_RGB32);

	LD MinX=INF, MinY=INF, MaxX=-INF, MaxY=-INF;
	point<LD> t;
	forn(y,H)
		forn(x,W){
			t=point<LD>(x,y);
			t=st+point<LD>(t^v,t*v);
			t.cartToGeo();
			t=rectangularToGeographical(t,zone);
			forn(i,2)
				t[i]=degToRad(t[i]);
			t=point<LD>(t.y,arctanh(sin(t.x))-eps*arctanh(eps*sin(t.x)));
			MinX=min(MinX,t.x), MaxX=max(MaxX,t.x);
			MinY=min(MinY,t.y), MaxY=max(MaxY,t.y);
		}


	point<LD> v_rev=point<LD>(-v.x,v.y)/v.length2();
	LD Scale=min((W-1)/(MaxX-MinX),(H-1)/(MaxY-MinY)),yt,theta;
	int ix,iy;
	forn(y,H)
		forn(x,W){
			yt=MaxY-y/Scale, theta=atan(sinh(yt));
			forn(i,ITERS_CNT)
				theta=asin(1-(1+sin(theta))*pow(1-eps*sin(theta),eps)/(exp(2*yt)*pow(1+eps*sin(theta),eps)));
			t=point<LD>(theta,MinX+x/Scale);
			forn(i,2)
				t[i]=radToDeg(t[i]);
			t=geographicalToRectangular(t,zone);
			t.geoToCart();
			t=t-st;
			t=point<LD>(t^v_rev,t*v_rev);
			ix=(int)t.x, iy=(int)t.y;
			if(inImage->valid(ix,iy))
				outImage->setPixel(x,y,inImage->pixel(ix,iy));
		}

	srand(time(NULL));
	char TempName[30];
	sprintf(TempName,"temp%d.jpg",rand());
	strcpy(strrchr(ImageName,'.'),"-out.jpg");
	outImage->save(TempName);
	remove(ImageName);
	rename(TempName,ImageName);

	return 0;
}