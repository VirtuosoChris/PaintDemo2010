
enum Paints{RED, BLUE, YELLOW, PURPLE, GREEN, ORANGE, BROWN, BLACK,GRAY};


class colorWheel{

Paints selected;

public:

Paints getSelected(){return selected;}

//double paint[8];


se::math::Vector3D getColor(const Paints& p){

        switch(p){

            case RED:
                return se::math::Vector3D(1.0,0.0,0.0);
                break;
            case BLUE:
                return se::math::Vector3D(0.0,0.0,1.0);
                break;
            case YELLOW:
                return se::math::Vector3D(1.0,1.0,0.0);
                break;
            case PURPLE:
                return se::math::Vector3D(.750,0.0,1.0);
                break;
            case GREEN:
                return se::math::Vector3D(0.0,1.0,0.0);
                break;
            case ORANGE: return se::math::Vector3D(1.0,.5,0.0,1.0);break;
            case BLACK: return se::math::Vector3D(0.0,0.0,0.0);break;
            case BROWN: return se::math::Vector3D(.6,.4,.3);break;
               default:
               return se::math::Vector3D(.0,.0,.0);break;


        }
}



se::math::Vector3D getInterpolatedColor(const Paints& p){
//double ammo = paint[p];

 return .75*getColor(p) ;
}


void render(const double& radians = 0.0){

glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

const double sliceSize = (2*3.14159) / double(8);
selected = Paints(int(radians / sliceSize)%8);


const double r1 = .1;
const double r2 = .15;
const double r3 = .2;
glDisable(GL_CULL_FACE);
    for(int i = 0; i < 8; i++){

    se::math::Vector3D intCol = getInterpolatedColor(Paints(i));
    se::math::Vector3D col = intCol*intCol*intCol;


if(selected == i){
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glLineWidth(.0051);
glBegin(GL_QUADS);



    glColor3f(1.0,1.0,1.0);
        glVertex2f(r1 * cos(i * sliceSize), r1 * sin(i*sliceSize));
        glVertex2f(r3 * cos(i * sliceSize), r3 * sin(i*sliceSize));
        glVertex2f(r3 * cos((i+1) * sliceSize), r3 * sin((i+1)*sliceSize));
        glVertex2f(r1 * cos((i+1) * sliceSize), r1 * sin((i+1)*sliceSize));



    glEnd();

    ///



    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glEnable(GL_CULL_FACE);

}
    glBegin(GL_QUADS);
const double BLENDCOL = .6;
    glColor4f(col[0], col[1],col[2],BLENDCOL);
        //glColor3f(0,.25,0);
            glVertex2f(r1 * cos(i * sliceSize), r1 * sin(i*sliceSize));

        //glColor3f(0,1.0,0);
        glColor4f(intCol[0], intCol[1],intCol[2],BLENDCOL);
            glVertex2f(r2 * cos(i * sliceSize), r2 * sin(i*sliceSize));

        //glColor3f(0,1.0,0);
        glColor4f(intCol[0], intCol[1],intCol[2],BLENDCOL);
            glVertex2f(r2 * cos((i+1) * sliceSize), r2 * sin((i+1)*sliceSize));

        //glColor3f(0,.25,0);
        glColor4f(col[0], col[1],col[2],BLENDCOL);
            glVertex2f(r1 * cos((i+1) * sliceSize), r1 * sin((i+1)*sliceSize));

            //glColor3f(0,1.0,0);

            glColor4f(intCol[0], intCol[1],intCol[2],BLENDCOL);
        glVertex2f(r2 * cos(i * sliceSize), r2 * sin(i*sliceSize));
            //glColor3f(0,1.0,0);
            glColor4f(col[0], col[1],col[2],BLENDCOL);
        glVertex2f(r3 * cos(i * sliceSize), r3 * sin(i*sliceSize));

            //glColor3f(0,.25,0);
            glColor4f(col[0], col[1],col[2],BLENDCOL);
        glVertex2f(r3 * cos((i+1) * sliceSize), r3 * sin((i+1)*sliceSize));

            //glColor3f(0,.25,0);
            glColor4f(intCol[0], intCol[1],intCol[2],BLENDCOL);
        glVertex2f(r2 * cos((i+1) * sliceSize), r2 * sin((i+1)*sliceSize));
glEnd();
    }


glDisable(GL_BLEND);
}


colorWheel(){
//for(int i = 0; i < 8; i++){paint[i] = 1.0;}
}



};
