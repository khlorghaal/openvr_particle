#include "doot/global.hpp"
#include "gl3w.h"
#include <glfw3.h>
#include <openvr.h>

#include "glprog.hpp"
#include "doot/timer.hpp"

volatile bool running= true;
Timer screentimer(100);

GLFWwindow* wind;

void closeGame(){
	running= false;
}
void closeWindCall(GLFWwindow*){
	closeGame();
}

void errCall(int error, char const* desc){
	cnsl<<"GLFW error: "<<desc<<endl;
}

int screen_w=0, screen_h=0;
void resizedCall(GLFWwindow* wind, int w, int h){
	screen_w= w;
	screen_h= h;
}

void initglfw(){
	if(!glfwInit()){
		cnsl<<"GLFW init failure"<<endl;
		throw;
	}
	glfwSetErrorCallback(&errCall);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	#ifdef DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	#else
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, false);
	#endif
	glfwWindowHint(GLFW_DOUBLEBUFFER, true);
	glfwWindowHint(GLFW_RESIZABLE, true);
	glfwWindowHint(GLFW_REFRESH_RATE, 60);
	wind= glfwCreateWindow(1024,512,"",0,0);
	if(!wind){
		error("glfw window creation failure");
		throw;
	}
	glfwSetWindowCloseCallback(wind, &closeWindCall);
	
	glfwMakeContextCurrent(wind);
	glfwGetWindowSize(wind, &screen_w,&screen_h);
	glfwSetFramebufferSizeCallback(wind, &resizedCall);

	if(gl3wInit())
		cnsl<<"gl3w init failure"<<endl;
	cnsl<<(char*)glGetString(GL_RENDERER)<<endl;

	//vsync
	glfwSwapInterval(0);

	glfwSetTime(0);
}
void termglfw(){
	glfwDestroyWindow(wind);
	glfwTerminate();
}







//column major
struct mat4{
	float a[16];

	mat4()= default;
	mat4(vr::HmdMatrix34_t const& m){
		a[ 0]= m.m[0][0]; a[ 1]= m.m[1][0]; a[ 2]= m.m[2][0]; a[ 3]= 0.0f;
		a[ 4]= m.m[0][1]; a[ 5]= m.m[1][1]; a[ 6]= m.m[2][1]; a[ 7]= 0.0f;
		a[ 8]= m.m[0][2]; a[ 9]= m.m[1][2]; a[10]= m.m[2][2]; a[11]= 0.0f;
		a[12]= m.m[0][3]; a[13]= m.m[1][3]; a[14]= m.m[2][3]; a[15]= 1.0f;
	};
	mat4(vr::HmdMatrix44_t const& m){
		a[ 0]= m.m[0][0]; a[ 1]= m.m[1][0]; a[ 2]= m.m[2][0]; a[ 3]= m.m[3][0];
		a[ 4]= m.m[0][1]; a[ 5]= m.m[1][1]; a[ 6]= m.m[2][1]; a[ 7]= m.m[3][1];
		a[ 8]= m.m[0][2]; a[ 9]= m.m[1][2]; a[10]= m.m[2][2]; a[11]= m.m[3][2];
		a[12]= m.m[0][3]; a[13]= m.m[1][3]; a[14]= m.m[2][3]; a[15]= m.m[3][3];
	};

	struct vec{
		float x,y,z,w;
		inline float dot(vec const& t){
			return x*t.x + y*t.y + z*t.z + w*t.w;
		}
	};
	mat4 operator*(mat4 const& b){
		mat4 p;
		vec rows[4]= {
			{a[ 0],a[ 4],a[ 8],a[12]},
			{a[ 1],a[ 5],a[ 9],a[13]},
			{a[ 2],a[ 6],a[10],a[14]},
			{a[ 3],a[ 7],a[11],a[15]}
		};
		vec cols[4]= {
			{b[ 0],b[ 1],b[ 2],b[ 3]},
			{b[ 4],b[ 5],b[ 6],b[ 7]},
			{b[ 8],b[ 9],b[10],b[11]},
			{b[12],b[13],b[14],b[15]}
		};
		for(int c=0; c!=4; c++){
			size_t coffs= c*4;
			for(int r=0; r!=4; r++){
				p[r+coffs]= rows[r].dot(cols[c]);
			}
		}
		return p;
	}
	inline float& operator[](size_t i){ return a[i]; }
	inline float operator[](size_t i) const{ return a[i]; }

	mat4 invertEuclid(){
		mat4 r= *this;
		float t;
		t= r[1];  r[1]= r[4];  r[4]= t;
		t= r[2];  r[2]= r[8];  r[8]= t;
		t= r[6];  r[6]= r[9];  r[9]= t;

		float x= r[12];
		float y= r[13];
		float z= r[14];
		r[12]= -(r[0]*x + r[4]*y + r[ 8]*z);
		r[13]= -(r[1]*x + r[5]*y + r[ 9]*z);
		r[14]= -(r[2]*x + r[6]*y + r[10]*z);

		return r;
	}
};




bool vrOn= false;
vr::IVRSystem* vrsys;
vr::IVRCompositor* compositor;
uint32_t vr_w, vr_h;
GLuint vr_tex;
GLuint vr_fbo;
//ssaa_fbo_t vr_msaa_fbo;//not used by desktop
vr::Texture_t* compostex;
void initvr(){
	bool hmd= vr::VR_IsHmdPresent();

	vrOn= hmd;
	if(!vrOn)
		return;

	bool rti= vr::VR_IsRuntimeInstalled();
	vr::EVRInitError initerr;
	vrsys= vr::VR_Init(&initerr, vr::EVRApplicationType::VRApplication_Scene);
	if(initerr){
		auto msg= vr::VR_GetVRInitErrorAsEnglishDescription(initerr);
		vr::VR_Shutdown();
		cnsl<<msg<<endl;
		return;
	}

	compositor= vr::VRCompositor();
	
	vrsys->SetDisplayVisibility(false);

	vrsys->GetRecommendedRenderTargetSize(&vr_w,&vr_h);

	glGenTextures(1,&vr_tex);
	glBindTexture(GL_TEXTURE_2D, vr_tex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, vr_w*2, vr_h);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenFramebuffers(1,&vr_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, vr_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vr_tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void termvr(){
	return;
	vr::VR_Shutdown();
}

const size_t MAX_DEVICES= vr::k_unMaxTrackedDeviceCount;
vr::TrackedDevicePose_t vr_renderposes[MAX_DEVICES];
vr::ETrackedDeviceClass vr_device_classes[MAX_DEVICES];
mat4 vr_matrices[MAX_DEVICES];
mat4 vpL;
mat4 vpR;
void updatePoses(){
	if(vrOn){
		compositor->WaitGetPoses(vr_renderposes, MAX_DEVICES, NULL, 0 );

		for(int i=0; i!=vr::k_unMaxTrackedDeviceCount; i++){
			auto& pose= vr_renderposes[i];
			if(pose.bPoseIsValid){
				vr_matrices[i]= mat4(pose.mDeviceToAbsoluteTracking);
				vr_device_classes[i]= vrsys->GetTrackedDeviceClass(i);
			}
		}

		if(vr_renderposes[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid){
			mat4 pL= vrsys->GetProjectionMatrix(vr::EVREye::Eye_Left,  .008f, 1000.f, vr::EGraphicsAPIConvention::API_OpenGL);
			mat4 pR= vrsys->GetProjectionMatrix(vr::EVREye::Eye_Right, .008f, 1000.f, vr::EGraphicsAPIConvention::API_OpenGL);

			mat4 m= vr_matrices[vr::k_unTrackedDeviceIndex_Hmd];
			mat4 v= m.invertEuclid();

			mat4 tL= vrsys->GetEyeToHeadTransform(vr::Eye_Left);
			mat4 tR= vrsys->GetEyeToHeadTransform(vr::Eye_Right);
			tL= tL.invertEuclid();
			tR= tR.invertEuclid();

			vpL= pL*tL*v;
			vpR= pR*tR*v;
		}
	}
}


void APIENTRY glerrcall(GLenum source, GLenum type, GLuint id,
						GLenum severity, GLsizei length,
						const GLchar* message, const void* userParam){
	if(severity!=GL_DEBUG_SEVERITY_NOTIFICATION)
		error(message);
}

const int pw= 64;
const int pointcount= pw*pw*pw;
struct particle{
	float  x, y, z, w;
	float vx,vy,vz,vw;
	unsigned int abgr;
	float size;
	float pad[2];
};

GLProg partprog;
GLProg transprog;
GLuint ssbo;
GLuint vao;
void initrender(){
	glDebugMessageCallback(&glerrcall, 0);

	partprog.init("part.vert", "part.frag");
	transprog.init("trans.comp");

	//vbo
	size_t sizparticles= pointcount*sizeof(particle);
	particle* particles= (particle*)malloc(sizparticles);
	const int pw2= pw*pw;
	const float fw= pw;
	for(size_t i=0; i!=pointcount; i++){
		particle& p= particles[i];
		float x=  (i%pw    )/fw*2-1;
		float y= ((i/pw)%pw)/fw*2-.6;
		float z=  (i/pw2   )/fw*2-1;
		p.x=  x*10; p.y=  y*10; p.z=  z*10; p.w=  1;
		p.vx= 0; p.vy= .2; p.vz= 0; p.vw= 0;
		p.abgr= 0x22DD3308;
		p.size=3;
	}
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_ARRAY_BUFFER, ssbo);
	glBufferData(GL_ARRAY_BUFFER, sizparticles,particles,GL_DYNAMIC_DRAW);
	free(particles);

	//vao
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	{
		glBindBuffer(GL_ARRAY_BUFFER, ssbo);
		size_t STRIDE= sizeof(particle);
		glVertexAttribPointer(0, 3,GL_FLOAT,0,         STRIDE, (void*)offsetof(particle,x));
		glVertexAttribPointer(1, 3,GL_FLOAT,0,         STRIDE, (void*)offsetof(particle,vx));
		glVertexAttribPointer(2, 4,GL_UNSIGNED_BYTE,1, STRIDE, (void*)offsetof(particle,abgr));
		glVertexAttribPointer(3, 1,GL_FLOAT,0,         STRIDE, (void*)offsetof(particle,size));
	}

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

int64 lasttime= 0;
int64 curtime= 0;
void render(){
	lasttime= curtime;
	curtime= current_time_us();
	float dtime= (curtime-lasttime)/1e6f;
	if(dtime>.1)//paused or blocked
		dtime= 0;//don't explode

	//compute
	transprog.reload();
	transprog.bind();
	glUniform1d(0, curtime/1e6);
	glUniform1f(1, dtime);
	if(vrOn){
		int dev= 0;
		for(int i=0; i!=MAX_DEVICES; i++){
			if(vr_device_classes[i]!=vr::TrackedDeviceClass_Controller)
				continue;
			vr::VRControllerState_t state;
			vrsys->GetControllerState(i, &state);
			float trigger_pull;
			/*for(auto& axis : ){
				if(vr::Prop_Axis0Type_Int32)
				
			}*/
			trigger_pull= state.rAxis[1].x;
			bool trigger_click= 1&(state.ulButtonPressed>>(vr::EVRButtonId::k_EButton_SteamVR_Trigger));
			float magnitude= 50*trigger_pull;
			if(trigger_pull>.999)
				magnitude*= -1;
			float lx= vr_matrices[i][12];
			float ly= vr_matrices[i][13];
			float lz= vr_matrices[i][14];
			glUniform4f(dev+2, lx,ly,lz, magnitude);
			if(++dev==2)
				break;
		}
	}
	else{
		glUniform4f(2, -.8,0,.8, -20);
		glUniform4f(3,  .8,0,.8, 32);
	}

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glDispatchCompute(pointcount,1,1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glClearColor(0,0,0,0);

	partprog.reload();
	partprog.bind();
	glBindVertexArray(vao);

	//desktop
	#undef far /*windows pls*/
	float far= 1000;
	float mvp[]= {
		float(screen_h)/screen_w,0,0,0,
		0,1,0,0,
		0,0,1/far,0,
		0,0,1,1.5
	};
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0,0,screen_w,screen_h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniform2i(1, screen_w, screen_h);
	glUniformMatrix4fv(0,1,true,mvp);
	glDrawArrays(GL_POINTS,0,pointcount);

	//vr
	if(vrOn){
		updatePoses();

		glBindFramebuffer(GL_FRAMEBUFFER, vr_tex);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		vr::Texture_t submittex= {(void*)vr_tex, vr::EGraphicsAPIConvention::API_OpenGL, vr::ColorSpace_Gamma };
		glUniform2i(1, vr_w, vr_h);

		if(!!compositor){
			//left eye
			glViewport(0,   0, vr_w,   vr_h);
			glUniformMatrix4fv(0,1,false,vpL.a);
			glDrawArrays(GL_POINTS,0,pointcount);

			//right eye
			glViewport(vr_w,0, vr_w, vr_h);
			glUniformMatrix4fv(0,1,false,vpR.a);
			glDrawArrays(GL_POINTS,0,pointcount);

			vr::VRTextureBounds_t const leye_bound= { 0,0,.5,1};
			vr::VRTextureBounds_t const reye_bound= {.5,0, 1,1};
			compositor->Submit(vr::EVREye::Eye_Left,  &submittex, &leye_bound, vr::EVRSubmitFlags::Submit_Default);
			compositor->Submit(vr::EVREye::Eye_Right, &submittex, &reye_bound, vr::EVRSubmitFlags::Submit_Default);
		}

		//desktop
		glBindFramebuffer(GL_READ_FRAMEBUFFER, vr_fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0,0,vr_w,vr_h,
						  0,0,vr_w,vr_h,//dont stretch
						  GL_COLOR_BUFFER_BIT,GL_NEAREST);

	}

	glFlush();
	glFinish();
	glfwSwapBuffers(wind);
}

int main_(int argc, char** argv){
	initglfw();
	initvr();
	initrender();

	while(running){
		glfwPollEvents();
		render();
		screentimer.invoke();
	}

	termvr();
	termglfw();
	return 0;
}
