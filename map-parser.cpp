#include "all.h"
#include "map-parser.h"

static const int
	POINTS_CNT=30,
	FIRST_STRS=9,
	LEN=100;

pair<const char*,parseOut> parse(const char *FileName){
	FILE* file=fopen(FileName,"r");

	char (*S)[LEN]=new char[FIRST_STRS][LEN];
	forn(i,FIRST_STRS)
		fgets(S[i],LEN,file);
	parseOut answer;
	forn(i,POINTS_CNT){
		int x,y,latdeg,londeg;
		double latmin,lonmin;
		char latc,lonc;

		if(fscanf(file,"%*[^,],%*s %d,%d,%*[^,],%*[^,],%d,%lf,%c,%d,%lf,%c%*[^\n]\n",&x,&y,&latdeg,&latmin,&latc,&londeg,&lonmin,&lonc)!=8)
			break;

		LD lat=latdeg+latmin/60, lon=londeg+lonmin/60;
		lat*=(latc=='N'?+1:-1), lon*=(lonc=='E'?+1:-1);
		answer.pb(mp(point<int>(x,y),point<LD>(lat,lon)));
	}

	int L=strlen(S[2]);
	while(L && isspace(S[2][L-1]))
		S[2][L-1]=0, --L;
	fclose(file);
	return mp(S[2],answer);
}