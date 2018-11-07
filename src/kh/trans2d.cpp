#include "trans2d.hpp"

trans2d::trans2d(): trans2d({0,0},0,1){}
trans2d::trans2d(vec2 t, float theta, float scale): 
	t(t), theta(theta), scale(1){
	matrixUpdate();
}


void trans2d::matrixUpdate(){
	//z= cos + isin
	float zx= cosf(theta);
	float zy= sinf(theta);
	// z*n= (zx+izy)*(nx+iny)
	//    = zx*nx + zx*iny + izy*nx + izy*iny
	//    = (zx*nx - zy*ny) + i*(zx*ny + zy*nx)
	// x= zx*nx - zy*ny
	// y= zy*nx + zx*ny
	mxx= zx; mxy=-zy;
	myx= zy; myy= zx;

	//inverse
	zx= cosf(-theta);
	zy= sinf(-theta);
	mixx= zx; mixy=-zy;
	miyx= zy; miyy= zx;

	mxx*=scale;
	mxy*=scale;
	myx*=scale;
	myy*=scale;
	mixx*=scale;
	mixy*=scale;
	miyx*=scale;
	miyy*=scale;
}

trans2d trans2d::conv(trans2d const& that) const{
	//translate to global, translate to that
	trans2d ret;
	vec2 dp= t-that.t;
	ret.t= that.rotinv(dp);

	ret.theta= theta-that.theta;
	ret.matrixUpdate();

	return ret;
}
