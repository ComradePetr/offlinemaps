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

const int CFF[2][4]={{1,0,-1,1},{0,1,1,1}};

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
		t=point<LD>(t.y,-t.x);
		forn(j,2)
			Y[2*i+j]=t[j];
	}
	
	ae_int_t info;
	real_1d_array c;
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
	point<LD> *B=new point<LD>[W*H];
	outImage=new QImage(W,H,QImage::Format_RGB32);

	LD MinX=INF, MinY=INF, MaxX=-INF, MaxY=-INF;
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
	pos=0;
	forn(y,H)
		forn(x,W){
			a=(B[pos].x-MinX)*Scale, b=(MaxY-B[pos].y)*Scale;
			outImage->setPixel(a,b,inImage->pixel(x,y));
			++pos;
		}

	srand(time(NULL));
	char TempName[30];
	sprintf(TempName,"temp%d.jpg",rand());
	strcpy(strrchr(ImageName,'.'),"-out.jpg");	
	outImage->save(TempName);
	rename(TempName,ImageName);

	return 0;
}