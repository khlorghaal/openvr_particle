flat in vec4 v_color;
flat in float v_psiz;
flat in vec2 v_subpixel_trans;
out vec4 color;
void main(){
	vec4 c= v_color;
	vec2 pc= gl_PointCoord.xy*2-1 - v_subpixel_trans;
	
	//sphere
	c.a= sqrt(1-length(pc));
	
	//diamond
	//c.a= 1-abs(pc.x)-abs(pc.y);
	//c.a= pow(c.a, .2);

	//star
	//c.a= .5-(  abs(pc.x*pc.y)  + length(pc)*.3 );
	//c.a= pow(c.a, .7);


	if( c.a!=c.a || c.a<=0 )
		discard;

	color= c;

}