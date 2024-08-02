#define SHOWCOLOR
#define MSENS2 .5



 unsigned int resWidth=1024;
 unsigned int resHeight =768;
/*************************************************************************************************

Epic Mona Lisa (TM)
User Interface Test

By Chris Pugh, team Chuck Norris
Copyright 2010

This user interface test is to demonstrate the basic mechanics of the painting interface
Will not necessarily demonstrate other aspects (eg, platforming, camera work, navigation)
that are already proven in other games.  Not a representation of visual style, level design, etc.

This code is rough prototype code (eg, done as quickly, with as few external libraries as possible)
and does not represent a suggested or even a decent software architecture

todo:: adjust movement speed while painting(?)
painting on surfaces
text output of properties of object cursor is actually on
make (some?)properties actually work?
**************************************************************************************************/
#define FREEGLUT_STATIC			//enable freeglut as a static lib
#define FREEGLUT_LIB_PRAGMAS 0	//turn off autolinking



#define MSENS .5

#include <iostream>
#include <se/graphics/gl/FreeGLUTWindow.h>
#include <sstream>
#include "ScreenSpaceQuad.h"

//todo::fix my uniform initialization methods for vector types and structs
#include <se/graphics/cpGL/GlslShaderProgram.h>
#include <se/graphics/cpGL/GlslVertexShader.h>
#include <se/graphics/cpGL/GlslFragmentShader.h>
#include <se/graphics/cpGL/OpenGLFramebufferObject.h>
#include <se/graphics/cpGL/FboTextureAttachment.h>
//#include <cpGL/OpenGLTextureObject.h>
#include <se/graphics/cpGL/FboDepthAttachment.h>
#include <se/graphics/cpGL/FboDepthTexture.h>
#include <fstream>
#include <se/graphics/image/TgaImage.h>
#include <se/graphics/image/RgbeImage.h>
#include "radialWheel.h"
#include "paintableObject.h"
#include <GL/freeglut.h>


bool operator==(const se::math::Vector3D& a, const se::math::Vector3D& b){
    return (a[0] ==b[0] && a[1] ==b[1] && a[2]==b[2] && a[3] ==b[3]);
}

/*
extern "C"{void glutSolidSphere(GLdouble, GLint, GLint);
void glutSetCursor(int);
void glutWarpPointer  (	int x  , int y  );
void glutFullScreen();
glutGameModeString(
}*/

#define CURSOR 101 //what the fuck?



class UI_IT:public se::sim::ui::PCInputTranslator{


protected:
    double forward;
    double right;
    double turn;
    double pitch;
    bool down;
    double rads;

public:

bool selectedOnce;
bool leftDown;

    const double& getForwardMotion(){return forward;}
    const double& getSidewaysMotion(){return right;}
    const bool& wheelDown(){return down;}

     double getTurn(){
        static double last = turn;

        double avg = .5*(turn + last);

        last = avg;
        turn = 0;

        return avg;


    }

    double getRads(){return rads;}


     double getPitch(){

        static double last = pitch;

        double avg = (pitch + last)/2;

        last = avg;
        pitch = 0;

        return avg;}



     void onMouseMove(const se::sim::ui::Pointer& p){

                static bool warp = false;
        static se::math::Vector3D prev = p;

        if(warp){warp=false;return;}

        se::math::Vector3D val = p;//; - prev;

        turn = -(val[0] - 640/2);
        pitch = -(val[1] - 480/2);


        /*
        if(!(fabs(pitch) < .5 && fabs(turn) < .5)){
        rads = atan2(pitch , -turn);
        if(turn < 0.0) rads -= 3.14159;
        }*/

        if(fabs(turn) > .25 && turn > 0){
            rads += .2;
        }

        if(fabs(turn) > .25 && turn < 0){
            rads -= .2;
        }

        if(rads > 2*3.14159){rads -= 2*3.14159;}
        if(rads < 0){rads = 2*3.14159 +rads;}

        prev = p;


                glutWarpPointer(640/2,480/2);
                warp = true;


         }
	 void onButtonDown(const unsigned int& k){

	    switch(k){

        case 'c':
        case 'C':down =true; selectedOnce = true;break;
	    case 'w':
	    case 'W':forward = 1.0;
                break;

        case 'a':
        case 'A':right = -.50;

            break;

        case 's':
        case 'S':forward = -.50;

            break;

        case 'D':
        case 'd': right= .50;

            break;

            case MOUSE_LEFT: leftDown = true;
            break;

            case ESCAPE: exit(0);

        default: break;
        }

	     }
	 void onButtonUp(const unsigned int& k){


	    switch(k){
	         case 'c':
        case 'C':down =false;break;
	    case 'w':
	    case 'W': if(forward >0)forward = 0;//1.0;
                break;

        case 'a':
        case 'A': if(right <0)right =0;// -1.0;

            break;

        case 's':
        case 'S':
            if(forward < 0)
            forward =0;// -1.0;

            break;

        case 'D':
        case 'd':
        if(right >0)
        right=0;// 1.0;
 break;
            case MOUSE_LEFT: leftDown = false;


            break;

        default: break;
        }



        }
	 void onJoystickMove(const se::sim::ui::Stick&){

     }
};


using namespace se::graphics::cpGL;
using namespace se::graphics;

UI_IT inputrec;

cpGL::OpenGLTextureObject* emTexture = NULL;
cpGL::OpenGLTextureObject* emDiffuse = NULL;
cpGL::OpenGLTextureObject* shiny = NULL;
cpGL::GlslShaderProgram* lightingProg = NULL;
cpGL::GlslShaderProgram* fbProg = NULL;

Cone* cone = NULL;
Walls* walls = NULL;
Floor* gfloor = NULL;
GLUquadricObj *quadricObject;

float frameCount = 0.0f;

colorWheel x;

se::graphics::gl::FreeGLUTWindow* wp = NULL;



void rs(int w, int h){
        resWidth = w;
        resHeight = h;
        glViewport(0,0,w,h);
}



se::sim::UpdateTimer gameTimer;


void initialize(){
glutSetCursor(CURSOR);
//cpGL::GlslShaderProgram* lightingProg = NULL;
cpGL::GlslFragmentShader* lightingFragShader = NULL;
cpGL::GlslVertexShader* lightingVertShader = NULL;

cpGL::GlslFragmentShader* fbFrag = NULL;
cpGL::GlslVertexShader* fbVert = NULL;

//cpGL::GlslFragmentShader* emFragShader = NULL;
cpGL::GlslVertexShader* emVertShader = NULL;




	glViewport(0,0,resWidth, resHeight);//todo:: does steve's call to bind in renderTarget/window/whatever set the viewport?
    OpenGLTextureUnit::initialize();


    cone= new Cone();
    walls = new Walls();
    gfloor = new Floor();

//load textures
std::ifstream file("textures/grace.tga",std::fstream::binary);
cpGL::OpenGLTextureObject::Settings sets1;
cpGL::OpenGLTextureObject::Settings sets2;
cpGL::OpenGLTextureObject::Settings setsSh;


image::TgaImage img(file);


//todo: revise the texture generation api to be less redundant and confusing
	sets1.width = img.width;
	sets1.height = img.height;
	sets1.colors_per_pixel = img.colors_per_pixel;
	sets1.bits_per_color = img.bits_per_color;
	sets1.is_floating_point = true;



	emTexture = new OpenGLTextureObject(img,sets1);
	emTexture->setTextureRepeat(true);

	//emTexture->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);




std::ifstream file2("textures/gracediff.tga",std::fstream::binary);
image::TgaImage img2(file2);


//todo: revise the texture generation api to be less redundant and confusing
	sets2.width = img2.width;
	sets2.height = img2.height;
	sets2.colors_per_pixel = img2.colors_per_pixel;
	sets2.bits_per_color = img2.bits_per_color;
	sets2.is_floating_point = true;


    emDiffuse = new OpenGLTextureObject(img2,sets2);
	emDiffuse->setTextureRepeat(true);
//	emDiffuse->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);




std::ifstream fileSh("textures/grace.tga",std::fstream::binary);
image::TgaImage imgSh(fileSh);


//todo: revise the texture generation api to be less redundant and confusing
	setsSh.width = img2.width;
	setsSh.height = img2.height;
	setsSh.colors_per_pixel = img2.colors_per_pixel;
	setsSh.bits_per_color = img2.bits_per_color;
	setsSh.is_floating_point = true;

shiny = new OpenGLTextureObject(imgSh, setsSh);
shiny->bind();
shiny->setTextureRepeat(true);




//todo: revise the texture generation api to be less redundant and confusing


  glClearColor(0.0f,0.0f,0.0f,1.0f);
  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();


// ***Create shaders***
  std::vector<std::string> svec;
  svec.push_back("shaders/Lighting.vert");
  lightingVertShader = new GlslVertexShader(svec);
  svec.clear();
  svec.push_back("shaders/Lighting.frag");
  lightingFragShader = new GlslFragmentShader(svec);

    svec.clear();


  svec.push_back("shaders/texCoords.vert");
  fbVert = new GlslVertexShader(svec);
  svec.clear();
  svec.push_back("shaders/texCoords.frag");
  fbFrag = new GlslFragmentShader(svec);
  svec.clear();

  svec.push_back("shaders/em.vert");
  emVertShader = new GlslVertexShader(svec);
  svec.clear();
  svec.push_back("shaders/Lighting.frag");
//  emFragShader = new GlslFragmentShader(svec);
  svec.clear();

  std::vector<int> pvec;
  pvec.push_back(lightingVertShader->getId());
  pvec.push_back(lightingFragShader->getId());
  lightingProg = new GlslShaderProgram(pvec);
    pvec.clear();
//  std::vector<int> pvec;
  pvec.push_back(emVertShader->getId());
  //pvec.push_back(emFragShader->getId());
  pvec.push_back(lightingFragShader->getId());
  //emProg = new GlslShaderProgram(pvec);

pvec.clear();
pvec.push_back(fbVert->getId());
  pvec.push_back(fbFrag->getId());
  fbProg = new GlslShaderProgram(pvec);
    pvec.clear();


    cpGL::OpenGLTextureObject::Settings sets ;
    sets.colors_per_pixel = 4;
    sets.bits_per_color = 8;
    sets.texture_filter = OpenGLTextureObject::FILTER_BILINEAR;




glEnable(GL_DEPTH_TEST);
//delete emFragShader; // we can delete shader objects once we're created the programs we want with them, and they won't be deleted until the program object is deleted too
delete emVertShader;
delete lightingFragShader;
delete lightingVertShader;
delete fbVert;
delete fbFrag;
std::cout<<"init"<<std::endl;

quadricObject = gluNewQuadric();
  gluQuadricDrawStyle(quadricObject, GLU_FILL);
  gluQuadricNormals(quadricObject, GLU_SMOOTH);
  gluQuadricOrientation(quadricObject, GLU_OUTSIDE);//dsfsdfdf
  gluQuadricTexture(quadricObject, GL_TRUE);


  emDiffuse->bind();
  emDiffuse->generateMipMaps();
  emTexture->bind();
  emTexture->generateMipMaps();


}


void render(){


	glViewport(0,0,resWidth, resHeight);
 static double rot = .01f;
static bool movedOnce = false;
static unsigned int walkTicks = 0;
/***gluPerspective(45.0, float(resWidth)/resHeight, 0.1, 15.0); *///

se::sim::Quantity<double,se::sim::physics::time> tim = gameTimer.getElapsedSeconds();
	gameTimer.update();//get the amount of time that's elapsed since the last frame


static double frames = 0.0f;
static double time = 0.0f;
time += double(tim);
double fps = frames/time;
frames +=1.0f;

static float lightMV[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//static  float lightProj[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);



emTexture->bind();
emTexture->setTextureFilter(OpenGLTextureObject::FILTER_BILINEAR);
emTexture->setTextureRepeat(true);
lightingProg->bind();

OpenGLTextureUnit::getUnit(1).bind();
//woodDiff->bind();
//woodDiff->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);
lightingProg->setTexture("diffMap", cpGL::OpenGLTextureUnit::getActiveUnit());


OpenGLTextureUnit::getUnit(2).bind();
lightingProg->setTexture("decals", cpGL::OpenGLTextureUnit::getActiveUnit());



OpenGLTextureUnit::getUnit(3).bind();
lightingProg->setTexture("shiny", cpGL::OpenGLTextureUnit::getActiveUnit());



OpenGLTextureUnit::getUnit(0).bind();

lightingProg->setTexture("texture", cpGL::OpenGLTextureUnit::getActiveUnit());


//emProg->bind();

//GlslShaderProgram::disableShaders();

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
gluPerspective(45.0f, float(resWidth)/resHeight, .10, 50.0);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

glDepthMask(GL_FALSE);
//static double look = 0.0;
//look -=.01;

static float cpos[3] = {-1.0,.15,2.50};
static float camMat[16];
//look cross product up is the right vector relative to wherever the camera is oriented



//cpos[0] = -2+ 6*cos(frameCount/100);
//cpos[2] = -3+ 6*sin(frameCount/100);
//cpos[0] = -1.0 + sin(frameCount/100.0);
//cpos[1] =  cos(frameCount/100.0);
//static double theta = 0.0;
//theta += .01;


static float theta = 0.0;
frameCount++;
glPushMatrix();


glPushMatrix();
glLoadIdentity();


static double pitch= 0.0f;
//pitch = 0;//21.1;
if(!inputrec.wheelDown()){
pitch
-= MSENS*(inputrec.leftDown?MSENS2:1.0)*inputrec.getPitch();
}
if(pitch > 90) pitch = 90;
if(pitch < -90)pitch = -90;

static double bob = 0.0;
//if(inputrec.getForwardMotion() != 0.0){




glRotatef(pitch,1,0,0);
if(!inputrec.wheelDown()){
    theta -= inputrec.getTurn()*MSENS*(inputrec.leftDown?MSENS2:1.0);
}

//theta +=.5;
glRotatef(theta ,0,1,0);//rotates the matrix left and right




//glTranslatef(-(cpos[0] += inputrec.getForwardMotion() * .01*cos( 0.0174532925*theta)),-cpos[1],-(cpos[2] += inputrec.getForwardMotion() * .01*sin( 0.0174532925*theta)));

if(fabs(inputrec.getSidewaysMotion()) > 0 || fabs(inputrec.getForwardMotion())>0){movedOnce = true;

bob += .075;
//glRotatef(.5*fmax(fabs(inputrec.getSidewaysMotion()), fabs(inputrec.getForwardMotion()))*sin(bob), 0,0,1);


}else{bob = 0;}

cpos[0] += inputrec.getSidewaysMotion() * .0251*cos( 0.0174532925*theta);

cpos[2] += inputrec.getSidewaysMotion() * .0251*sin( 0.0174532925*theta);


cpos[0] += inputrec.getForwardMotion() * .0251*cos( 0.0174532925*(theta - 90));

cpos[2] += inputrec.getForwardMotion() * .0251*sin( 0.0174532925*(theta-90));

glTranslatef(-cpos[0],-(cpos[1] + .01*fmax(fabs(inputrec.getSidewaysMotion()), fabs(inputrec.getForwardMotion()) )*sin(bob*2)*sin(bob*2)),-cpos[2]);




glGetFloatv(GL_MODELVIEW_MATRIX, camMat);




glPopMatrix();

glLoadMatrixf(camMat);



fbProg->bind();
glGetFloatv(GL_MODELVIEW_MATRIX, lightMV);
glUniformMatrix4fv(glGetUniformLocation(lightingProg->getID(), "cameraMatrix"), 1, false, lightMV);//***
glUniform3f(glGetUniformLocation(lightingProg->getID(), "cameraPosition"), cpos[0], cpos[1], cpos[2]);
lightingProg->bind();
//gluLookAt(-1.0,0.0,0.0,0,0,-1.0,0,-1,0);

glGetFloatv(GL_MODELVIEW_MATRIX, lightMV);
glUniformMatrix4fv(glGetUniformLocation(lightingProg->getID(), "cameraMatrix"), 1, false, lightMV);//***

glUniform3f(glGetUniformLocation(lightingProg->getID(), "cameraPosition"), cpos[0], cpos[1], cpos[2]);


//emProg->bind();

//emProg->setUniformValue("cameraPosition", cpos);
//glUniform3f(glGetUniformLocation(emProg->getID(),"cameraPosition") ,cpos[0],cpos[1],cpos[2]);
//glUniformMatrix4fv(glGetUniformLocation(emProg->getID(), "cameraMatrix"), 1, false, lightMV);//***
glPopMatrix();


/************************
*************************/
glClear(GL_DEPTH_BUFFER_BIT);
glDepthMask(GL_TRUE);
glEnable(GL_DEPTH_TEST);

glPushMatrix();
fbProg->bind();

glUniform1i(glGetUniformLocation(fbProg->getID(), "id"),  walls->id);//***


walls->render();

//glUniform1f(glGetUniformLocation(fbProg->getID(), "temperature"),  gfloor->temperature);//***
glUniform1i(glGetUniformLocation(fbProg->getID(), "id"),  gfloor->id);//***
gfloor->render();


//glUniform1f(glGetUniformLocation(fbProg->getID(), "temperature"),  cone->temperature);//***
glUniform1i(glGetUniformLocation(fbProg->getID(), "id"),  cone->id);//***
cone->render();

glUniform1i(glGetUniformLocation(fbProg->getID(), "id"),  (cone->id+1));//***
cone->render2();

static GLfloat pix[4];
glReadPixels(resWidth/2, resHeight/2, 1, 1, GL_RGBA, GL_FLOAT, pix);
glPopMatrix();



//return;
glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

/*********************
*********************/

#ifdef SHOWCOLOR
// ****render skydome*** /



//OpenGLTextureUnit::getUnit(1).bind();
//graniteDiff->bind();
//OpenGLTextureUnit::getUnit(0).bind();

glDisable(GL_CULL_FACE);
lightingProg->bind();

glUniform1i(glGetUniformLocation(lightingProg->getID(), "world"),  false);//***


//gluLookAt(lightPosition[0],lightPosition[1],lightPosition[2],0,0,-5,0,-1,0);
glPushMatrix();
glTranslatef(cpos[0],cpos[1],cpos[2]);
glColor3f(0,0,1);
glTranslatef(0.0,-.5,-5.0);
		glScalef(2.0,2.0,2.0);
		glRotatef(180.0,0.0,0.0,1.0);
		//glRotatef(rot,0.0,1.0,0.0);
		rot+=.1;


gluSphere(quadricObject,10,24,24);
glPopMatrix();
glEnable(GL_CULL_FACE);

glDepthMask(GL_TRUE);

glColor3f(1,1,1);


//*********draw box*** /

//emProg->bind();
emDiffuse->bind();
emDiffuse->setTextureFilter(OpenGLTextureObject::FILTER_BILINEAR);
emDiffuse->setTextureRepeat(true);
//emProg->setTexture("texture", cpGL::OpenGLTextureUnit::getActiveUnit());
OpenGLTextureUnit::getUnit(1).bind();
//wallDiff->bind();

OpenGLTextureUnit::getUnit(3).bind();
shiny->bind();
shiny->setTextureFilter(cpGL::OpenGLTextureObject::FILTER_BILINEAR);
OpenGLTextureUnit::getUnit(0).bind();


glUniform1i(glGetUniformLocation(lightingProg->getID(), "world"),  true);//***


glPushMatrix();
		glTranslatef(0.0,-.5,-5.0);
		glScalef(2.0,2.0,2.0);
		glRotatef(180.0,0.0,0.0,1.0);
		glRotatef(rot,0.0,1.0,0.0);
		rot+=.1;

	  //gluSphere(quadricObject, 1.0, 100,100);
glPopMatrix();

emTexture->bind();

emDiffuse->bind();

glPushMatrix();




//static float cpos[3] = {-1.0,0.0,2.50};

//fbProg->bind();
//lightingProg->bind();

glUniform1f(glGetUniformLocation(lightingProg->getID(), "temperature"),  walls->temperature);//***
walls->render();

glUniform1f(glGetUniformLocation(lightingProg->getID(), "temperature"),  gfloor->temperature);//***
lightingProg->bind();
//lightingProg->bind();
gfloor->render();


glUniform1f(glGetUniformLocation(lightingProg->getID(), "temperature"),  cone->temperature);//***
cone->render();
cone->render2();
glPopMatrix();

//** todo: this needs to be a patch for Soaper* * /


#endif

glColor3f(0,1,0);
GlslShaderProgram::disableShaders();

glMatrixMode(GL_PROJECTION);
glLoadIdentity();


glClear(GL_DEPTH_BUFFER_BIT);



std::stringstream ss2;
ss2<<"FPS:"<<fps<<"\n";
ss2<<"pix:"<<pix[0]<<" "<<pix[1]<<" "<<(pix[2]*255)<<" "<<pix[3];//<<" "<<pix[3];//<<" "<<pix[4];
 wp->printRasterText(se::math::Vector3D(-.95, .950, 0),ss2.str(),se::graphics::gl::FreeGLUTWindow::BITMAP_8_BY_13);

glPushMatrix();

if(inputrec.wheelDown()){

//glScalef( float(resHeight)/float(resWidth),1.0 ,1.0 );

wp->printRasterText(se::math::Vector3D(-.20, -.25, 0),"Move mouse left and right to choose color\n",se::graphics::gl::FreeGLUTWindow::BITMAP_8_BY_13);


glScalef( float(resHeight)/float(resWidth),1.0 ,1.0 );


x.render(inputrec.getRads());
}

glPopMatrix();

se::math::Vector3D cc = x.getColor(x.getSelected());
glColor3f(cc[0],cc[1],cc[2]);

/*
glEnable(GL_POINT_SMOOTH);
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA,GL_SRC_ALPHA );
glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);*/
glPointSize(10);
glBegin(GL_POINTS);

glVertex3f(0,0,0);
glEnd();
glPopMatrix();

glPushMatrix();
//glScalef(5,5,5);
glColor3f(0,1,0);

if(!movedOnce){
wp->printRasterText(se::math::Vector3D(-.095, -.120, 0),"Use the mouse to look.\nPress WASD to walk.",se::graphics::gl::FreeGLUTWindow::BITMAP_8_BY_13);
}else{

if(walkTicks < 100){walkTicks++;}
else{

if(!inputrec.selectedOnce){
wp->printRasterText(se::math::Vector3D(-.17, -.120, 0),"Press C to select a paint color",se::graphics::gl::FreeGLUTWindow::BITMAP_8_BY_13);
}else{

if(!inputrec.wheelDown()){
    if(walkTicks < 300){
    wp->printRasterText(se::math::Vector3D(-.12, -.120, 0),"Hold left click to paint.",se::graphics::gl::FreeGLUTWindow::BITMAP_8_BY_13);
    walkTicks++;
    }else{
;
    }

}

}

}

}



PaintableObject* abc = NULL;
if(inputrec.leftDown){
//const double rate = .001;
//    x.paint[x.getSelected()] -= rate;
  //  if(x.paint[x.getSelected()] <0){
    //   x.paint[x.getSelected()]=0; }



int zz = int(pix[2]*255);



    if(zz == gfloor->id){

        glPointSize(10);
        gfloor->splatPointSprite(pix[0],pix[1],x.getColor(x.getSelected()));

            abc = gfloor;


    }

    else if(walls->id == zz){
        glPointSize(7);
        walls->splatPointSprite(pix[0],pix[1],x.getColor(x.getSelected()));

        abc = walls;


    }

    else if (cone->id == zz || zz == cone->id + 1){
       // if( zz == cone->id + 1)exit(0);
        glPointSize(30);
        cone->splatPointSprite(pix[0], pix[1],x.getColor(x.getSelected()), zz);
        abc = cone;

    }


}/*else if(inputrec.rightdown){


    }
*/

if(abc){
        if(x.getColor(x.getSelected()) == x.getColor(RED)){
                abc->temperature += .001;
        }else if(x.getColor(x.getSelected()) == x.getColor(BLUE)){
                abc->temperature -= .001;
        }else if( x.getColor(x.getSelected()) == x.getColor(GREEN)){
                abc->size += .01;
        }else if(x.getColor(x.getSelected()) == x.getColor(BROWN)){
                if(abc->size > .25){
                abc->size -= .01;
                }
        }else if(x.getColor(x.getSelected()) == x.getColor(PURPLE)){
            abc->mass+= .01;
        }
        else if(x.getColor(x.getSelected()) == x.getColor(YELLOW)){
            abc->mass-= .01;
        }
}


//}

return;


}





int main(void){

std::cout<<"main begins\n";
//create the window
se::graphics::gl::FreeGLUTWindow::Settings windowSettings;
windowSettings.window_name = "Caustics Mapping Demo";
windowSettings.width = resWidth;
windowSettings.height = resHeight;
//010s0e::sim::ui::PCInputTranslator

se::graphics::gl::FreeGLUTWindow win(windowSettings, inputrec);
wp = &win;
//glutEnterGameMode();

glutFullScreen();
try{

/*

glutGameModeString("640x480");
if(!glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)){exit(0);}
glutEnterGameMode();

//if(glutGameModeGet(GLUT_GAME_MODE_ACTIVE)){exit(0);}
*/initialize();
glutReshapeFunc(rs);
while(true){

  render();
  win.update(se::sim::Quantity<double,se::sim::physics::seconds>(0.0));
  win.swap(win.getGLRuntime());

}
}catch(std::string s){
		std::cerr<<s;
}

delete lightingProg;
delete emTexture;
//delete emProg;
delete emDiffuse;
delete shiny;

gluDeleteQuadric(quadricObject);
quadricObject = NULL;
delete cone;
delete walls;
delete gfloor;
delete fbProg;

/*
delete depthFbo;
delete depth;
delete causticsMap;
delete receiverPositionsWorld;
delete casterNormalsWorld;
delete casterPositionsWorld;
delete causticsProg;
*/

return 0;
}
