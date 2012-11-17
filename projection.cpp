//10.10.12

#include "all.h"

extern point geographicalToRectangular(LD,LD);

point c1[2],c2[2];
const LD INF=1e18;

int main(int argc, char *argv[]) {
	freopen("input.txt","r",stdin);
	freopen("output.txt","w",stdout);
	LL left=INF, right=-INF, top=INF, bottom=-INF;	
	for(int i=0;i<2;++i){
		c1[i].read();
		left=min(left,(LL)c1[i].x), right=max(right,(LL)c1[i].x), top=min(top,(LL)c1[i].y), bottom=max(bottom,(LL)c1[i].y);
		LD a,b,c,d;
		cin>>a>>b>>c>>d;
		point t=geographicalToRectangular(a+b/60,c+d/60);
		c2[i].x=t.y, c2[i].y=-t.x; //http://upload.wikimedia.org/wikipedia/commons/4/44/Descartes_system_3D.png?uselang=ru
	}
	printf("%I64d %I64d %I64d %I64d ",left,right,top,bottom);
	point c=c1[0]-c1[1], d=c2[0]-c2[1];
	point v=point(c^d,c*d)/c.length2();
	point st(c2[0].x-(c1[0]^v),c2[0].y-(c1[0]*v));

	QImage in("input.jpg");
	int W=in.width(), H=in.height();
	QImage out(W,H,QImage::Format_RGB32);

	LD MinX=INF, MinY=INF, MaxX=-INF, MaxY=-INF;
	forab(y,top,bottom+1)
		forab(x,left,right+1){
			point A(x,y);
			point B=st+point(A^v,A*v);
			MinX=min(MinX,B.x), MaxX=max(MaxX,B.x), MinY=min(MinY,B.y), MaxY=max(MaxY,B.y);
		}
	LD Scale=min((right-left+1)/(MaxX-MinX),(bottom-top+1)/(MaxY-MinY));
	forab(y,top,bottom+1)
		forab(x,left,right+1){
			point A(x,y);
			point B=st+point(A^v,A*v);
			LL a=floor((B.x-MinX)*Scale), b=floor((B.y-MinY)*Scale);
			out.setPixel(a,b,in.pixel(x,y));
		}		
	
	out.save("output.jpg");
	return 0;
}