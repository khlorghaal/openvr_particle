layout(location=0) uniform mat4 mvp;
layout(location=1) uniform ivec2 res;

layout(location=0) in vec3 p;
layout(location=1) in vec3 v;
layout(location=2) in vec4 color;
layout(location=3) in float size;

flat out vec4 v_color;
flat out float v_psiz;
flat out vec2 v_subpixel_trans;//for antialiasing

void main(){
	vec4 pos= vec4(p,1);
	pos= mvp*pos;
	float dist= 1.*pos.w;
	float big= min(64, size/dist);
	float alpha;
	if(big>1)
		alpha= 1;
	else
		alpha= big*big;

	gl_Position= pos;
	gl_PointSize= big;
	v_color= color;
	v_psiz= big;
	v_subpixel_trans= 0.0000001*fract( (pos.xy/pos.w)*res/2. );
}