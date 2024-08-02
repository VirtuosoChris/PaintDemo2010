varying vec3 norm;
uniform mat4 cameraMatrix;
//uniform mat4 inverseCam;
uniform vec3 cameraPosition;

void main(){
  

  vec4 worldPos = gl_ModelViewMatrix*gl_Vertex;

  vec4 eyespacePos = cameraMatrix * worldPos;

  gl_Position = gl_ProjectionMatrix* eyespacePos;


//reflect the world space view vector across the world space normal
  norm = reflect( 
(worldPos.xyz - cameraPosition)
,
normalize(gl_NormalMatrix*gl_Normal)
);

  

}