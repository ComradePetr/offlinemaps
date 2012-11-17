#include "all.h"

extern point<LD> geographicalToRectangular(point<LD>,LD);
extern point<LD> rectangularToGeographical(point<LD>,LD);
extern LD degToRad(LD);

typedef vector< pair< point<LD>,point<LD> > > vec_ldld;
extern pair<const char *,vec_ldld> parse(const char*);
extern LD lonToZone(LD lon);

const LD INF=1e18,eps=0.08181333387657540019556205937723;

point<LD> v,st;
LD zone;

inline point<LD> Calculate(int x,int y){
	static point<LD> A,B;
	A=point<LD>(x,y);
	B=st+point<LD>(A^v,A*v);
	B=rectangularToGeographical(point<LD>(-B.y,B.x),zone);
	B.x=degToRad(B.x), B.y=degToRad(B.y);
	return point<LD>(B.y,arctanh(sin(B.x))-eps*arctanh(eps*sin(B.x)));
}

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
	outImage=new QImage(W,H,QImage::Format_RGB32);

	LD MinX=INF, MinY=INF, MaxX=-INF, MaxY=-INF;
	point<LD> B;
//	forab(y,top,bottom+1)
//		forab(x,left,right+1){	
	forn(y,H)
		forn(x,W){
			B=Calculate(x,y);
			MinX=min(MinX,B.x), MaxX=max(MaxX,B.x), MinY=min(MinY,B.y), MaxY=max(MaxY,B.y);
		}
	LD Scale=min((W-1)/(MaxX-MinX),(H-1)/(MaxY-MinY));
	LL a,b;
//	forab(y,top,bottom+1)
//		forab(x,left,right+1){	
	forn(y,H)
		forn(x,W){
			B=Calculate(x,y);
			a=(B.x-MinX)*Scale, b=(MaxY-B.y)*Scale;
			outImage->setPixel(a,b,inImage->pixel(x,y));
		}

	strcpy(strrchr(ImageName,'.'),"-out.jpg");
	outImage->save(ImageName);



	QApplication *Application=new QApplication(argc,argv);

	QMainWindow *MainForm = new QMainWindow(0, Qt::Window);
	MainForm->resize(640, 480);
	MainForm->setWindowTitle("Finished");
	MainForm->show();

	Application->exec();

	return 0;
}