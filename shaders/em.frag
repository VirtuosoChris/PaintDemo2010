uniform sampler2D texture;
varying vec3 norm;
uniform vec3 cameraPosition;

void main(){

vec3 lNormal = normalize(norm);

 //compute theta and phi for the vertex based on the inverse of its //normal, or the outward facing direction of the sphere at this point

	float phi = acos(lNormal.y); 

	float theta;
	theta = atan(-lNormal.z, lNormal.x);//z was neg
	

	//atan returns values from neg pi to pi.  
	//acos returns values from 0 to pi

	//we now make these into uv's



	vec2 uvCoord = vec2( .5+.5*(theta / 3.14),
 ( 1.0-(phi) /3.14) );

gl_FragColor =  //vec4(normalize(-cameraPosition),1.0);
texture2D(texture, uvCoord.st);

//gl_FragColor =vec4( gl_FragColor.xyz / (gl_FragColor.xyz + vec3(1.0)) //, 1.0);

gl_FragColor = vec4(norm,1.0);

}

