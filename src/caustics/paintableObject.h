#define REZ 512


#include <sstream>

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


 //const unsigned int RSR= cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION;
using namespace se::graphics::cpGL;
using namespace se::graphics;

namespace CO = cpGL::constants;

struct PaintableObject{

public:

std::string name;
double temperature;
double mass;
int id;
double size;
//luminosity is a per pixel effect rather than a per object one, so we can paint on specular paths rather than making the whole object shiny


cpGL::FboTextureAttachment<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION, cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION> *psFBO;
cpGL::OpenGLFramebufferObject<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION, cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION> *fb;
cpGL::FboDepthAttachment<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION,cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION>* depth ;


static int ctr;

void initializeFBO(const unsigned int& w, const unsigned int& h){




    fb = new cpGL::OpenGLFramebufferObject<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION, cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION>(w/2, h/2);
    depth = new cpGL::FboDepthAttachment<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION, cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION>(w/2, h/2);
    psFBO = new FboTextureAttachment<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION, cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION>(w/2,h/2);

     cpGL::OpenGLTextureObject::Settings sets ;
    sets.colors_per_pixel = 4;
    sets.bits_per_color = 8;
    sets.texture_filter = OpenGLTextureObject::FILTER_BILINEAR;


    //psFBO = new cpGL::FboTextureAttachment<REZ, REZ>();
    fb->bind();
    //fb->attachRenderTarget(*psFBO);

    fb->attachRenderTarget(*psFBO);
    fb->attachRenderTarget(*depth);

    std::vector<cpGL::FboTextureAttachment<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION, cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION>* > rts;
rts.push_back(psFBO);

    fb->setRenderTargets(rts.begin(), rts.end());

//glClearColor(1,1,1,1);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); //todo: replace with steve's version



    const unsigned int resWidth=1600;
    const unsigned int resHeight =900;
    glViewport(0,0,resWidth,resHeight);


}


PaintableObject(const std::string& n, double m):name(n),temperature(0.0), mass(m), size(1.0),id(ctr++)

{



}


~PaintableObject(){
    delete psFBO;
    delete fb;
}

virtual void splatPointSprite(const double& u, const double&v, const se::math::Vector3D& col ){

glEnable(GL_POINT_SMOOTH);
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//glBlendFunc(GL_SRC_ALPHA,GL_ONE );
glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

glDisable(GL_DEPTH_TEST);
glDepthMask(false);
//std::cout<<"\n\n\n\n\n\n==============================="<<std::endl;

//cpGL::GlslShaderProgram::disableShaders();

//glEnable(GL_TEXTURE_2D);
//OpenGLTextureUnit::getUnit(2).bind();
//psFBO->bind();

//psFBO->setTextureFilter(cpGL::OpenGLTextureObject::FILTER_BILINEAR);

//glBindTexture(GL_TEXTURE_2D, 0);
    fb->bind();


    //fb->attachRenderTarget(*psFBO);
   // fb->attachRenderTarget(*depth);
//std::vector<cpGL::FboTextureAttachment<REZ, REZ>* > rts;
//rts.push_back(psFBO);

   // fb->setRenderTargets(rts.begin(), rts.end());

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

//glClearColor(0,0,0,1);
//glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
glColor4f(col[0],col[1],col[2], col[3]);

    glBegin(GL_POINTS);

    glVertex3f( (u - .5)*2 ,(v - .5)*2 ,0);

    glEnd();
//glColor3f(0,1,0);
//ScreenSpaceQuad::getInstance().render();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); //todo: replace with steve's version

    psFBO->bind();
    psFBO->generateMipMaps();

const unsigned int resWidth=1600;
const unsigned int resHeight =900;
    glViewport(0,0,resWidth,resHeight);


glEnable(GL_DEPTH_TEST);
glDepthMask(true);

glDisable(GL_BLEND);
}




};

int PaintableObject::ctr = 1;
//cpGL::OpenGLFramebufferObject<REZ, REZ> PaintableObject::fb;

class Cone:public PaintableObject{
cpGL::OpenGLTextureObject* graniteDiff ;
//cpGL::OpenGLTextureObject* graniteDiff2;
GLUquadricObj *quadricObject;
cpGL::FboDepthAttachment<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION,cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION>* depth2 ;

cpGL::FboTextureAttachment<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION, cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION> *psFBO2;
cpGL::OpenGLFramebufferObject<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION, cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION> *fb2;


public:

virtual void splatPointSprite(const double& u, const double&v, const se::math::Vector3D& col, const unsigned int&  zz){

glEnable(GL_POINT_SMOOTH);
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//glBlendFunc(GL_SRC_ALPHA,GL_ONE );
glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

glDisable(GL_DEPTH_TEST);
glDepthMask(false);
//std::cout<<"\n\n\n\n\n\n==============================="<<std::endl;
//exit(0);
//cpGL::GlslShaderProgram::disableShaders();

//glEnable(GL_TEXTURE_2D);
//OpenGLTextureUnit::getUnit(2).bind();
//psFBO->bind();

//psFBO->setTextureFilter(cpGL::OpenGLTextureObject::FILTER_BILINEAR);

//glBindTexture(GL_TEXTURE_2D, 0);
if(zz == id){
fb->bind();
}else{
fb2->bind();
}
    //fb->attachRenderTarget(*psFBO);
   // fb->attachRenderTarget(*depth);
//std::vector<cpGL::FboTextureAttachment<REZ, REZ>* > rts;
//rts.push_back(psFBO);

   // fb->setRenderTargets(rts.begin(), rts.end());

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

//glClearColor(0,0,0,1);
//glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
glColor4f(col[0],col[1],col[2], col[3]);

    glBegin(GL_POINTS);

    glVertex3f( (u - .5)*2 ,(v - .5)*2 ,0);

    glEnd();
//glColor3f(0,1,0);
//ScreenSpaceQuad::getInstance().render();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); //todo: replace with steve's version

   if(zz == id){
   psFBO->bind();
    psFBO->generateMipMaps();
}else{
   psFBO2->bind();
    psFBO2->generateMipMaps();
}



const unsigned int resWidth=1600;
const unsigned int resHeight =900;
    glViewport(0,0,resWidth,resHeight);


glEnable(GL_DEPTH_TEST);
glDepthMask(true);
glDisable(GL_BLEND);

}


 Cone():PaintableObject("Cone",10.0){
ctr++;
 quadricObject = gluNewQuadric();
  gluQuadricDrawStyle(quadricObject, GLU_FILL);
  gluQuadricNormals(quadricObject, GLU_SMOOTH);
  gluQuadricOrientation(quadricObject, GLU_OUTSIDE);//dsfsdfdf
  gluQuadricTexture(quadricObject, GL_TRUE);


std::ifstream fileGranite("textures/ceiling.tga",std::fstream::binary);
image::TgaImage imgWall(fileGranite);

        cpGL::OpenGLTextureObject::Settings sets5;
        sets5.width = imgWall.width;
		sets5.height = imgWall.height;
		sets5.colors_per_pixel = imgWall.colors_per_pixel;
		sets5.bits_per_color = imgWall.bits_per_color;
		sets5.is_floating_point = true;


    graniteDiff = new OpenGLTextureObject(imgWall,sets5);
    graniteDiff->bind();
	graniteDiff->setTextureRepeat(false);
	graniteDiff->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);
    graniteDiff->bind();
    graniteDiff->generateMipMaps();

//    graniteDiff2 = new OpenGLTextureObject(imgWall,sets5);
//	graniteDiff2->bind();
//	graniteDiff2->setTextureRepeat(false);
//	graniteDiff2->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);
//	graniteDiff2->bind();
//	graniteDiff2->generateMipMaps();

unsigned int w = graniteDiff->getWidth();
unsigned int h = graniteDiff->getHeight();


    fb2 = new cpGL::OpenGLFramebufferObject<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION, cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION>(w/2, h/2);
    depth2 = new cpGL::FboDepthAttachment<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION, cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION>(w/2, h/2);
    psFBO2 = new FboTextureAttachment<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION, cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION>(w/2,h/2);

     cpGL::OpenGLTextureObject::Settings sets ;
    sets.colors_per_pixel = 4;
    sets.bits_per_color = 8;
    sets.texture_filter = OpenGLTextureObject::FILTER_BILINEAR;


    //psFBO2 = new cpGL::FboTextureAttachment<REZ, REZ>();
    fb2->bind();
    //fb2->attachRenderTarget(*psFBO2);

    fb2->attachRenderTarget(*psFBO2);
    fb2->attachRenderTarget(*depth2);

    std::vector<cpGL::FboTextureAttachment<cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION, cpGL::constants::RUNTIME_SPECIFIED_RESOLUTION>* > rts;
rts.push_back(psFBO2);

    fb2->setRenderTargets(rts.begin(), rts.end());

//glClearColor(1,1,1,1);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); //todo: replace with steve's version



   // const unsigned int resWidth=1024;
  //  const unsigned int resHeight =768;
    glViewport(0,0,resWidth,resHeight);
	initializeFBO(graniteDiff->getWidth(), graniteDiff->getHeight());
 }

 ~Cone(){
     delete graniteDiff;
//     delete graniteDiff2;
    gluDeleteQuadric(quadricObject);
    quadricObject = NULL;
  }

    void render(){// PaintableObject::render();
 static double rot = 0.0;
rot+=.01;
 OpenGLTextureUnit::getUnit(2).bind();
 psFBO->bind();

        OpenGLTextureUnit::getUnit(1).bind();
        graniteDiff->bind();
        graniteDiff->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);
        OpenGLTextureUnit::getUnit(0).bind();

        glPushMatrix();
		glTranslatef(-1.0,+.05*sin(rot/10)
		,1.250);
		//glScalef(2.0,2.0,2.0);
		//glRotatef(180.0,0.0,0.0,1.0);
		glRotatef(rot,0.0,1.0,0.0);
		rot+=.1;

        glPushMatrix();
        glRotatef(180, 0,1,0);
        OpenGLTextureUnit::getUnit(1).bind();
    graniteDiff->bind();
         OpenGLTextureUnit::getUnit(2).bind();
        psFBO->bind();
        psFBO->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);
    //    graniteDiff2->bind();
      //   graniteDiff2->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);
        //OpenGLTextureUnit::getUnit(0).bind();
        glScalef(.5, .5, .5);
        glScalef(size, size, size);
        gluDisk(quadricObject, 0,.21, 30,30);
        psFBO->setTextureFilter(OpenGLTextureObject::FILTER_BILINEAR);
        glPopMatrix();
        glScalef(.5, .5, .5);
        glScalef(size, size, size);




    }


    void render2(){
    OpenGLTextureUnit::getUnit(2).bind();
    psFBO2->bind();
    psFBO2->setTextureFilter(cpGL::OpenGLTextureObject::FILTER_TRILINEAR);
    gluCylinder(quadricObject,.21,0,.5,30,30);
    psFBO2->setTextureFilter(cpGL::OpenGLTextureObject::FILTER_TRILINEAR);
    OpenGLTextureUnit::getUnit(0).bind();
    }



};

class Walls:public PaintableObject{
cpGL::OpenGLTextureObject* wallDiff;
GLUquadricObj *quadricObject;

public:

 Walls():PaintableObject("Walls",100.0){



std::ifstream fileWall("textures/wall.tga",std::fstream::binary);
image::TgaImage imgWall(fileWall);

cpGL::OpenGLTextureObject::Settings sets4;
//todo: revise the texture generation api to be less redundant and confusing
	sets4.width = imgWall.width;
	sets4.height = imgWall.height;
	sets4.colors_per_pixel = imgWall.colors_per_pixel;
	sets4.bits_per_color = imgWall.bits_per_color;
	sets4.is_floating_point = true;


    wallDiff = new OpenGLTextureObject(imgWall,sets4);
	wallDiff->bind();
	wallDiff->setTextureRepeat(false);

	wallDiff->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);

    wallDiff->generateMipMaps();

 quadricObject = gluNewQuadric();
  gluQuadricDrawStyle(quadricObject, GLU_FILL);
  gluQuadricNormals(quadricObject, GLU_SMOOTH);
  gluQuadricOrientation(quadricObject, GLU_OUTSIDE);//dsfsdfdf
  gluQuadricTexture(quadricObject, GL_TRUE);


initializeFBO(wallDiff->getWidth(), wallDiff->getHeight());

 }

 ~Walls(){
    delete wallDiff;
      gluDeleteQuadric(quadricObject);
    quadricObject = NULL;
  }

 void render(){
 //PaintableObject::render();
 OpenGLTextureUnit::getUnit(2).bind();
 psFBO->bind();
 psFBO->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);
     OpenGLTextureUnit::getUnit(1).bind();
        wallDiff->bind();
        wallDiff->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);
        OpenGLTextureUnit::getUnit(0).bind();
        glTranslatef(0.0,+.5,2.0);

        glRotatef(90,1.0f, 0.0f, 0.0);

      //  glScalef(size, size, size);

        gluQuadricOrientation(quadricObject, GLU_INSIDE);//dsfsdfdf
        gluCylinder(quadricObject, 3, 3,2, 10, 1);
        gluQuadricOrientation(quadricObject, GLU_OUTSIDE);//dsfsdfdf

        glPopMatrix();

        OpenGLTextureUnit::getUnit(2).bind();
 psFBO->bind();
 psFBO->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);
        OpenGLTextureUnit::getUnit(0).bind();



 }

};

class Floor: public PaintableObject{
cpGL::OpenGLTextureObject* woodDiff ;
public:

    Floor():PaintableObject("Floor",50.0){


std::ifstream fileWood("textures/wood.tga",std::fstream::binary);
image::TgaImage imgWood(fileWood);
cpGL::OpenGLTextureObject::Settings sets3;

//todo: revise the texture generation api to be less redundant and confusing
	sets3.width = imgWood.width;
	sets3.height = imgWood.height;
	sets3.colors_per_pixel = imgWood.colors_per_pixel;
	sets3.bits_per_color = imgWood.bits_per_color;
	sets3.is_floating_point = true;


    woodDiff = new OpenGLTextureObject(imgWood,sets3);
    woodDiff->bind();
	woodDiff->setTextureRepeat(false);
	woodDiff->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);
    woodDiff->generateMipMaps();

    initializeFBO(woodDiff->getWidth()*2, woodDiff->getHeight()*2);

    }

    ~Floor(){
    delete woodDiff;
    }

    void render(){

   //     PaintableObject::render();

        OpenGLTextureUnit::getUnit(2).bind();
        psFBO->bind();
    psFBO->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);

        OpenGLTextureUnit::getUnit(1).bind();
        woodDiff->bind();
        woodDiff->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);
        OpenGLTextureUnit::getUnit(0).bind();

        glPushMatrix();
        glTranslatef(-.5,-.40,1.74250);
        glRotatef(-90, 1.0,0.0,0.0);
        glScalef(3.50,3.5,3.5);
        ScreenSpaceQuad::getInstance().render();

        glPopMatrix();

            OpenGLTextureUnit::getUnit(2).bind();
        psFBO->bind();
    psFBO->setTextureFilter(OpenGLTextureObject::FILTER_TRILINEAR);
    OpenGLTextureUnit::getUnit(0).bind();
    }


};
