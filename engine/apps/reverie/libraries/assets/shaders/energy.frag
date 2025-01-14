// http://glslsandbox.com/e#71879.0

#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 resolution;


float hash( float n ) { return fract(sin(n)*753.5453123); }

// Slight modification of iq's noise function.
float noise( in vec2 x )
{
    vec2 p = floor(x);
    vec2 f = fract(x);
    f = f*f*(3.0-2.0*f);
    
    float n = p.x + p.y*157.0;
    return mix(
                    mix( hash(n+  0.0), hash(n+  1.0),f.x),
                    mix( hash(n+157.0), hash(n+158.0),f.x),
            f.y);
}


float fbm(vec2 p, vec3 a)
{
     float v = 0.0;
     v += noise(p*a.x)*.50;
     v += noise(p*a.y)*.50;
     v += noise(p*a.z)*.125;
     return v;
}

vec3 drawLines( vec2 uv, vec3 fbmOffset,  vec3 color2 )
{
    float timeVal = time * 0.1;
    vec3 finalColor = vec3( 0.0 );
    
    for( int i=0; i < 10; ++i )
    {
        float indexAsFloat = float(i);
        float amp = 8.0 + (indexAsFloat*7.0);
        float period = 2.0 + (indexAsFloat+8.0);
        float thickness = mix( 0.7, 1.0, noise(uv*10.0) );
        float t = abs( 1.0 / (sin(uv.x + fbm( uv + timeVal * period, fbmOffset )) * amp) * thickness );
        
        finalColor +=  t * color2 * 0.6;
    }
    
    return finalColor;
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main( void ) 
{

    vec2 uv = ( gl_FragCoord.xy / resolution.xy ) * 2.0 - 1.5;
    uv.x *= resolution.x/resolution.y;
    uv.xy = uv.yx;

    //vec3 lineColor1 = vec3( 2.3, 1.5, .5 );
    vec3 lineColor2 = vec3( 1., 1., 1. );
    
    vec3 finalColor = vec3(0.0);

    
    float t = sin( time *80.) * 0.5 + 0.5;
    float pulse = mix( 0.40, 1.0, t);
    
    //finalColor += drawLines( uv, vec3( 1.0, 20.0, 30.0), lineColor1, lineColor2 ) * pulse;
    finalColor += drawLines( uv, vec3( 1.0, 2.0, 4.0),  lineColor2 );
    
    finalColor = vec3( smoothstep(.3, 1.0,(finalColor.x+finalColor.y+finalColor.z)/3.) )*finalColor*pulse;
    finalColor *= hsv2rgb(vec3(sin( time *0.3) * 0.5 + 0.5, .7, .7 ));
	
    gl_FragColor = vec4( finalColor, 1.);
