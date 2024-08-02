
uniform sampler2D texture;
varying vec3 norm;
uniform sampler2D diffMap;
uniform bool world;
uniform sampler2D decals;
uniform sampler2D shiny;
varying vec3 ref;
uniform float temperature; 

void main(){

vec3 lNormal = normalize(norm);

	float phi = acos(lNormal.y); 

	float theta;
	theta = atan(-lNormal.z, lNormal.x);
	

	//atan returns values from neg pi to pi.  
	//acos returns values from 0 to pi

	//we now make these into uv's



	vec2 uvCoord = vec2( .5+.5*(theta / 3.14),
 ( 1.0-(phi) /3.14) );

gl_FragColor =  texture2D(texture, uvCoord.st);
//gl_FragColor =vec4( (gl_FragColor.xyz) / (gl_FragColor.xyz + vec3//(1.0)) , 1.0);


//const float p = 1.7510250;
//gl_FragColor.x = 1.0 / (1.0 + pow(2.71,-gl_FragColor.x/p) );
//gl_FragColor.y = 1.0 / (1.0 + pow(2.71,-gl_FragColor.y/p) );
//gl_FragColor.z = 1.0 / (1.0 + pow(2.71,-gl_FragColor.z/p) );
//gl_FragColor = vec4(vec3(norm ),1.0);

if(world){
vec4 decalL = texture2D(decals, gl_TexCoord[0].st);

gl_FragColor *= texture2D(diffMap, gl_TexCoord[0].st) ;


float dll = length(decalL);
/*
if(temperature > 0.0){
gl_FragColor *= vec4((1.0 + temperature), (1.0 - temperature), 1.0-temperature,1.0);
}
else{
 gl_FragColor *= vec4( (1.0 + temperature), (1.0 + temperature), 1.0-temperature,1.0);
}


gl_FragColor = mix(gl_FragColor, gl_FragColor * decalL, dll);
*/

// *************************
if(dll >.2){
gl_FragColor*= decalL;
//gl_FragColor = mix(gl_FragColor, gl_FragColor * decalL, dll);
}
else{
//if(decalL.rgb == vec3(1.0,1.0,1.0)){
if(temperature > 0.0){
gl_FragColor *= vec4((1.0 + temperature), (1.0 - temperature), 1.0-temperature,1.0);
}
else{
 gl_FragColor *= vec4( (1.0 + temperature), (1.0 + temperature), 1.0-temperature,1.0);

}
//}
}


 // * /////////////
float odist = length(decalL.rgb - vec3(1.0,.5,0.0));
if(odist < .2){
	//gl_FragColor = texture2D(shiny, gl_TexCoord[0].st);

vec3 lNormal = normalize(ref);

 //compute theta and phi for the vertex based on the inverse of its //normal, or the outward facing direction of the sphere at this point

	float phi = acos(lNormal.y); 

	float theta;
	theta = atan(-lNormal.z, lNormal.x);//z was neg
	

	//atan returns values from neg pi to pi.  
	//acos returns values from 0 to pi

	//we now make these into uv's



	vec2 uvCoord = vec2( .5+.5*(theta / 3.14),
 ( 1.0-(phi) /3.14) );

	gl_FragColor = vec4(mix(texture2D(shiny, uvCoord).rgb, vec3(1.0,.5,0.0), odist/.2),1.0);

}

}
}