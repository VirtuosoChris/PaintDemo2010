varying vec3 norm;
uniform mat4 cameraMatrix;
varying vec3 ref;
uniform vec3 cameraPosition;


void main(){
  


//vec2 uvCoord = vec2(theta,phi);	

  gl_TexCoord[0] = gl_MultiTexCoord0;

  vec4 wsp = gl_ModelViewMatrix*gl_Vertex;

  norm = //wsp.xyz;//
-(gl_NormalMatrix*gl_Normal);


//reflect the world space view vector across the world space normal
  ref = reflect( 
(-wsp.xyz + cameraPosition)
,
normalize(gl_NormalMatrix*gl_Normal)
);  


  gl_Position = gl_ProjectionMatrix* cameraMatrix * wsp;//gl_ModelViewMatrix*gl_Vertex;





}

