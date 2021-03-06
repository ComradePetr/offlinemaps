#ifndef ALL_H
#define ALL_H

#include<cstdio>
#include<cmath>
#include<cstring>
#include<cctype>
#include<cassert>
#include<ctime>

#include<algorithm>
#include<vector>
#include<iostream>
using namespace std;

#include<QtCore>
#include<QImage>
#include<QPoint>
#include<QColor>

#define pb push_back
#define mp make_pair
#define fst first
#define snd second
#define forn(i,N) for(int i=0;i<(N);++i)
#define forab(i,a,b) for(int i=(a);i<(b);++i)

typedef long double LD;
typedef long long LL;

inline LD arctanh(LD x){
	return log((1+x)/(1-x))/2;
}

template<class T=LD>
struct point{
	T x,y;
	point(){}
	point(T x,T y):x(x),y(y){}
	
	void read(){
		cin>>x>>y;
	}
	LD length2(){
		return x*x+y*y;
	}
	LD length(){
		return sqrt(x*x+y*y);
	}
	
	T& operator[](const int &k){
		return k?y:x;
	}

	inline void cartToGeo(){
		swap(x,y), x*=-1;
	}
	inline void geoToCart(){
		swap(x,y), y*=-1;
	}	
};

template<class T>
inline point<T> operator +(const point<T> &A,const point<T> &B){
	return point<T>(A.x+B.x,A.y+B.y);
}

template<class T>
inline point<T> operator -(const point<T> &A,const point<T> &B){
	return point<T>(A.x-B.x,A.y-B.y);
}

template<class T>
inline point<T> operator *(const point<T> &A,const LD B){
	return point<T>(A.x*B,A.y*B);
}

template<class T>
inline point<T> operator /(const point<T> &A,const LD B){
	return point<T>(A.x/B,A.y/B);
}

template<class T>
inline LD operator *(const point<T> &A,const point<T> &B){
	return A.x*B.x+A.y*B.y;
}

template<class T>
inline LD operator ^(const point<T> &A,const point<T> &B){
	return A.x*B.y-A.y*B.x;
}
#endif