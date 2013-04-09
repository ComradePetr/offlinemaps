#include "all.h"
#include "map-parser.h"
#include "calc.h"

#include "interpolation.h"
using namespace alglib;

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

const LD
	INF=1e18,
	eps=0.08181333387657540019556205937723;

point<LD> v,st;

real_1d_array X,Y;
real_2d_array Z;

const int
	CFF[2][4]={{1,0,-1,1},{0,1,1,1}},
	ITERS_CNT=20,
	T_SIZE=256;
const double
	ScaleCoeff=5e4;

inline void Error(const char *S="Error"){
	fputs(S,stderr);
	exit(1);
}
inline void Print(const char *S="Test"){
	fputs(S,stderr);
	fflush(stderr);
}

QImage *inImage;

int main(int argc, char *argv[]){
	freopen("error.log","w",stderr);

	if(argc==1)
		Error("USAGE: project.exe name-of-map-file.map");

	pair<const char*,parseOut> parsed=parse(argv[1]);

	const char *ImageName=parsed.first;
	parseOut input=parsed.second;
	int L=input.size();
	if(L<=1)
		Error("Not enough points");

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
	if(info!=1)
		Error("Can't solve matrix");

	st=point<LD>(X[0],X[1]), v=point<LD>(X[2],X[3]);

	struct stat imstat;
	stat(ImageName,&imstat);
	int sz=imstat.st_size;

	FILE *imageFile=fopen(ImageName,"rb");
	if(!imageFile)
		Error("Can't open file with image");
	uchar *buff=new uchar[sz];
	fread(buff,sizeof(uchar),sz,imageFile);
	fclose(imageFile);

	Print("stamp0");
	inImage=new QImage();
	Print("stamp1");
	inImage->loadFromData(buff,sz);
	Print("stamp2");
	if(inImage->isNull())
		Error("Can't open image");
	Print("stamp3");
	int W=inImage->width(), H=inImage->height();
	Print("stamp4");

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
	int W1=int(Scale*(MaxX-MinX))+1,H1=int(Scale*(MaxY-MinY))+1;
	QImage *outImage=new QImage(W1,H1,QImage::Format_RGB32);
	int ix,iy;
	forn(y,H1/3)
		forn(x,W1/3){
			yt=MaxY-y*3/Scale, theta=atan(sinh(abs(yt)));
			forn(i,ITERS_CNT)
				theta=asin(1-(1+sin(theta))*pow(1-eps*sin(theta),eps)/(exp(2*abs(yt))*pow(1+eps*sin(theta),eps)));
			theta*=sign(yt);
			t=point<LD>(theta,MinX+x*3/Scale);
			forn(i,2)
				t[i]=radToDeg(t[i]);
			t=geographicalToRectangular(t,zone);
			t.geoToCart();
			t=t-st;
			t=point<LD>(t^v_rev,t*v_rev);
			ix=(int)t.x, iy=(int)t.y;
			for(int ix1=-1;ix1<2;ix1++)
				for(int iy1=-1;iy1<2;iy1++)
					if(inImage->valid(ix+ix1,iy+iy1))
						outImage->setPixel(x*3+ix1,y*3+iy1,inImage->pixel(ix+ix1,iy+iy1));
		}
	double dx=(MaxX-MinX),dy=(MaxY-MinY);
	int W2=(int)(dx*ScaleCoeff),H2=(int)(dy*ScaleCoeff);
	QImage *outImageScaled=new QImage(W2,H2,QImage::Format_RGB32);
	forn(y,H2)
		forn(x,W2){
			int x1=(int)((x/ScaleCoeff)*Scale);
			int y1=(int)((y/ScaleCoeff)*Scale);
			if(outImage->valid(x1,y1))
				outImageScaled->setPixel(x,y,outImage->pixel(x1,y1));
		}
	int nx=(int)W2/T_SIZE+1,ny=(int)H2/T_SIZE+1;
	QImage *Tile[nx][ny];
	forn(i,nx)
		forn(j,ny)
			Tile[i][j]=new QImage(T_SIZE,T_SIZE,QImage::Format_RGB32);
	forn(j,ny)
		forn(i,nx)
			forn(y,T_SIZE)
				forn(x,T_SIZE){
					ix=x+i*T_SIZE;
					iy=y+j*T_SIZE;
					if(outImageScaled->valid(ix,iy))
						Tile[i][j]->setPixel(x,y,outImageScaled->pixel(ix,iy));
				}

	srand(time(NULL));
	char TempName[30];
	sprintf(TempName,"temp%d.jpg",rand());
	strcpy(strrchr(ImageName,'.'),"-out.jpg");
//	outImage->save(TempName);
//	remove(ImageName);
//	rename(TempName,ImageName);
	outImage->save(ImageName);
	strcpy(strrchr(ImageName,'.'),"-scaled.jpg");
	outImageScaled->save(ImageName);
	strcpy(strrchr(ImageName,'.'),"-");
	char ImageName1[200];
	forn(i,nx)
		forn(j,ny){
			sprintf(ImageName1,"%s%d-%d.jpg",ImageName,i+1,j+1);
			Tile[i][j]->save(ImageName1);
		}


	strcpy(strrchr(argv[1],'.'),"-out.map");
	freopen(argv[1],"w",stdout);

	printf("OziExplorer Map Data File Version 2.2\n");
	printf("%s\n%s\n",ImageName,ImageName);
	puts("1 ,Map Code,\n\
Pulkovo 1942 (2),WGS 84,   0.0000,   0.0000,WGS 84\n\
Reserved 1\n\
Reserved 2\n\
Magnetic Variation,,,E\n\
Map Projection,Mercator,PolyCal,No,AutoCalOnly,No,BSBUseWPX,No\n");

	forn(i,L){
		t=point<LD>(input[i].fst.x,input[i].fst.y);
		t=st+point<LD>(t^v,t*v);
		t.cartToGeo();
		t=rectangularToGeographical(t,zone);
		forn(j,2)
			t[j]=degToRad(t[j]);
		t=point<LD>(t.y,arctanh(sin(t.x))-eps*arctanh(eps*sin(t.x)));
		t=point<LD>((t.x-MinX)*Scale,(MaxY-t.y)*Scale);
		ix=(int)t.x,iy=(int)t.y;
		ix=(int)(ix/Scale*ScaleCoeff);
		iy=(int)(iy/Scale*ScaleCoeff);
		printf("Point%02d,xy,   %d,   %d,in, deg,  %d, %.10lf,%c, %d, %.10lf,%c, grid,   ,           ,           ,N\n",i+1,ix,iy,
			(int)abs(input[i].snd.x),(double)(abs(input[i].snd.x)-(int)abs(input[i].snd.x)),input[i].snd.x>0?'N':'S',
			(int)abs(input[i].snd.y),(double)(abs(input[i].snd.y)-(int)abs(input[i].snd.y)),input[i].snd.y>0?'W':'E');
	}
	forab(i,L,30)
		printf("Point%02d,xy,     ,     ,in, deg,    ,        ,,    ,        ,, grid,   ,           ,           ,",i+1);

	fclose(stdout);

	return 0;
}