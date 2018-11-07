#pragma once

#include "vec2.hpp"


struct trans2d{
	float mxx, mxy;
	float myx, myy;
	vec2 t;

	float mixx, mixy;
	float miyx, miyy;

	float theta;
	float scale;
	
	trans2d();
	trans2d(vec2 trans, float theta, float scale);


	inline vec2 operator*(vec2 p) const{
		return vec2(
			p.x*mxx + p.y*mxy + t.x,
			p.x*myx + p.y*myy + t.y
		);
	}
	inline vec2 inv(vec2 p) const{
		p-= t;
		return vec2(
			p.x*mixx + p.y*mixy,
			p.x*miyx + p.y*miyy
		);
	}

	inline vec2 rot(vec2 p) const{
		return vec2(
			p.x*mxx + p.y*mxy,
			p.x*myx + p.y*myy
		);
	}
	inline vec2 rotinv(vec2 p) const{
		return vec2(
			p.x*mixx + p.y*mixy,
			p.x*miyx + p.y*miyy
		);
	}

	void matrixUpdate();

	trans2d conv(trans2d const&) const;
};