uniform vec3 cameraPosition;
uniform mat4 cameraMatrix;
varying float dist;

void main(){
  

  gl_TexCoord[0] = gl_MultiTexCoord0;

  
 
 vec4 wsp =  gl_ModelViewMatrix*gl_Vertex;

  gl_Position = gl_ProjectionMatrix*cameraMatrix*wsp;



//vec3 temp = (cameraMatrix*wsp
//);

dist = (gl_Position).z;

}