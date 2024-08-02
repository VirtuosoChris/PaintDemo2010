uniform int id;
varying float dist;

void main(){

//float result;
 //result = 
//if(dist < 1.0)else result = 0.0
//float result = id/255.0; //what the fuck?

gl_FragColor = vec4(gl_TexCoord[0].st, id/255.0,1);



//gl_FragColor = vec4(dist);

if(dist >1.0) {
	gl_FragColor.b = 0.0;
}

//if(dist!= 1.0) gl_FragColor.a = .1;
//gl_FragColor = vec4(1.0);
}