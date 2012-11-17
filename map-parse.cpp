#include "all.h"

static const int
	POINTS_CNT=30,
	FIRST_STRS=9,
	LEN=100;

pair<const char*,vector<pair<point<LD>,point<LD>>>> parse(const char *FileName){
	FILE* file=fopen(FileName,"r");

	char (*S)[LEN]=new char[FIRST_STRS][LEN];
	forn(i,FIRST_STRS)
		fgets(S[i],LEN,file);
	vector<pair<point<LD>,point<LD>>> answer;		
	forn(i,POINTS_CNT){
		int x,y,wdeg,ldeg;
		double wmin,lmin;
		char cw,cl;

		if(fscanf(file,"%*[^,],%*s %d,%d,%*[^,],%*[^,],%d,%lf,%c,%d,%lf,%c%*[^\n]\n",&x,&y,&wdeg,&wmin,&cw,&ldeg,&lmin,&cl)!=8)
			break;
		
		LD w=wdeg+wmin/60, l=ldeg+lmin/60;
		w*=(cw=='N'?+1:-1), l*=(cl=='W'?+1:-1);
		answer.pb(mp(point<LD>((LD)x,(LD)y),point<LD>(w,l)));
	}
	
	int L=strlen(S[2]);
	while(L && isspace(S[2][L-1]))
		S[2][L-1]=0, --L;
	return mp(S[2],answer);
}