#include "light.h"
#include "object.h"
#include "canvas.h"
#include "primitive.h"

lighthdl::lighthdl()
{
	model = NULL;
	type = "light";
}

lighthdl::lighthdl(const vec3f &ambient, const vec3f &diffuse, const vec3f &specular)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	//model = new modelhdl("res/models/banana.obj");
    model = new pyramidhdl(0.2, 1.0, 4.0);
	type = "light";
}

lighthdl::~lighthdl()
{

}

directionalhdl::directionalhdl() : lighthdl(white*0.1f, white*0.5f, white)
{
	type = "directional";
}

directionalhdl::directionalhdl(const vec3f &direction, const vec3f &ambient, const vec3f &diffuse, const vec3f &specular) : lighthdl(ambient, diffuse, specular)
{
	type = "directional";
}

directionalhdl::~directionalhdl()
{

}

void directionalhdl::update(canvashdl *canvas)
{
	/* TODO Assignment 2: Update the direction of the light using the orientation of the attached model.
	 * The easiest thing is to do translations and rotations like you were going to render the object, and
	 * then just multiply some initial direction vector by the normal matrix.
	 */
}

void directionalhdl::shade(vec3f &ambient, vec3f &diffuse, vec3f &specular, vec3f vertex, vec3f normal, float shininess) const
{
	/* TODO Assignment 2: Implement a directional light. See the OpenGL Orange Book in the references section
	 * of the course website. Its under the section about emulating the fixed function pipeline.
	 */
}

pointhdl::pointhdl() : lighthdl(white*0.1f, white*0.5f, white)
{
	this->attenuation = vec3f(1.0, 0.14, 0.7);
	type = "point";
}

pointhdl::pointhdl(const vec3f &position, const vec3f &attenuation, const vec3f &ambient, const vec3f &diffuse, const vec3f &specular) : lighthdl(ambient, diffuse, specular)
{
	this->attenuation = attenuation;
	type = "point";
}

pointhdl::~pointhdl()
{

}

void pointhdl::update(canvashdl *canvas)
{
	/* TODO Assignment 2: Update the position of the light using the position of the attached model.
	 * The easiest thing is to do translations and rotations like you were going to render the object, and
	 * then just multiply the origin by the modelview matrix.
	 */
}

void pointhdl::shade(vec3f &ambient, vec3f &diffuse, vec3f &specular, vec3f vertex, vec3f normal, float shininess) const
{
	/* TODO Assignment 2: Implement a point light. See the OpenGL Orange Book in the references section
	 * of the course website. Its under the section about emulating the fixed function pipeline.
	 */
    
    float nDotVP; // normal . light direction
    float nDotHV; // normal . light half vector
    float pf; // power factor
    float attenuation; // computed attenuation factor
    float d; // distance from surface to light source
    vec3f VP; // direction from surface to light position
    vec3f halfVector; // direction of maximum highlights
    // Compute vector from surface to light position
    VP = this->position - vertex;
    // Compute distance between surface and light position
    d = mag(VP);
    // Normalize the vector from surface to light position
    VP = norm(VP);
    // Compute attenuation
    attenuation = 1.0 / (this->attenuation[0] +
                         this->attenuation[1] * d +
                         this->attenuation[2] * d * d);

    halfVector = norm(VP);
    nDotVP = fmax(0.0, dot(normal, VP));
    nDotHV = fmax(0.0, dot(normal, halfVector));
    if (nDotVP == 0.0)
        pf = 0.0;
    else
        pf = pow(nDotHV, shininess);
    ambient += this->ambient * attenuation;
    diffuse += this->diffuse * nDotVP * attenuation;
    specular += this->specular * pf * attenuation;
    
}

spothdl::spothdl() : lighthdl(white*0.1f, white*0.5f, white)
{
	this->attenuation = vec3f(1.0, 0.14, 0.7);
	this->cutoff = 0.5;
	this->exponent = 1.0;
	type = "spot";
}

spothdl::spothdl(const vec3f &attenuation, const float &cutoff, const float &exponent, const vec3f &ambient, const vec3f &diffuse, const vec3f &specular) : lighthdl(ambient, diffuse, specular)
{
	this->attenuation = attenuation;
	this->cutoff = cutoff;
	this->exponent = exponent;
	type = "spot";
}

spothdl::~spothdl()
{

}

void spothdl::update(canvashdl *canvas)
{
	/* TODO Assignment 2: Update both the direction and position of the light using the position and orientation
	 * of the attached model. See above.
	 */
}

void spothdl::shade(vec3f &ambient, vec3f &diffuse, vec3f &specular, vec3f vertex, vec3f normal, float shininess) const
{
	/* TODO Assignment 2: Implement a spot light. See the OpenGL Orange Book in the references section
	 * of the course website. Its under the section about emulating the fixed function pipeline.
	 */
    
    float nDotVP; // normal . light direction
    float nDotHV; // normal . light half vector
    float pf; // power factor
    float spotDot; // cosine of angle between spotlight
    float spotAttenuation; // spotlight attenuation factor
    float attenuation; // computed attenuation factor
    float d; // distance from surface to light source
    vec3f VP; // direction from surface to light position
    vec3f halfVector; // direction of maximum highlights
    // Compute vector from surface to light position
    VP = this->position - vertex;
    // Compute distance between surface and light position
    d = mag(VP);
    
    // Normalize the vector from surface to light position
    VP = norm(VP);
    // Compute attenuation
    attenuation = 1.0 / (this->attenuation[0] +
                         this->attenuation[1] * d +
                         this->attenuation[2] * d * d);
    
    // See if point on surface is inside cone of illumination
    spotDot = dot(-VP, norm(this->direction));
    if (spotDot < this->cutoff)
        spotAttenuation = 0.0; // light adds no contribution
    else
        spotAttenuation = pow(spotDot, this->exponent);
    // Combine the spotlight and distance attenuation.
    attenuation *= spotAttenuation;
    halfVector = norm(VP);
    nDotVP = fmax(0.0, dot(normal, VP));
    nDotHV = fmax(0.0, dot(normal, halfVector));
    if (nDotVP == 0.0)
        pf = 0.0;
    else
        pf = pow(nDotHV, shininess);
    ambient += this->ambient * attenuation;
    diffuse += this->diffuse * nDotVP * attenuation;
    specular += this->specular * pf * attenuation;

}

ambienthdl::ambienthdl() : lighthdl(white*0.1f, white*0.5f, white)
{
    type = "ambient";
}

void ambienthdl::update(canvashdl *canvas)
{

}

ambienthdl::~ambienthdl()
{
    
}

void ambienthdl::shade(vec3f &ambient, vec3f &diffuse, vec3f &specular, vec3f vertex, vec3f normal, float shininess) const
{
    
}
