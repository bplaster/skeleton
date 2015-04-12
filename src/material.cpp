#include "material.h"
#include "canvas.h"
#include "light.h"

materialhdl::materialhdl()
{
	type = "material";
}

materialhdl::~materialhdl()
{
}

uniformhdl::uniformhdl()
{
	type = "uniform";
	emission = vec3f(0.0, 0.0, 0.0);
	ambient = vec3f(0.2, 0.2, 0.2);
    diffuse = vec3f(1.0, 1.0, 1.0);
    specular = vec3f(1.0, 1.0, 1.0);
	shininess = 1.0;
}

uniformhdl::~uniformhdl()
{

}

/* This is the vertex shader for a uniform material. The vertex and normal are in world coordinates, and
 * you have write access to the varying array which is used to pass data to the fragment shader. Don't
 * forget that this data is interpolated before it reaches the fragment shader.
 */
vec3f uniformhdl::shade_vertex(canvashdl *canvas, vec3f vertex, vec3f normal, vector<float> &varying) const
{
	vec4f eye_space_vertex = canvas->matrices[canvashdl::modelview_matrix]*homogenize(vertex);
    vec4f eye_space_normal = canvas->matrices[canvashdl::normal_matrix]*homogenize(normal);

    // Store varying
    if(canvas->shade_model == canvashdl::phong) {
        for (int i = 0; i < 3; i++) {
            varying.push_back(vertex[i]);
        }
        for (int i = 0; i < 3; i++) {
            varying.push_back(normal[i]);
        }
    } else {
        
        vec3f light_ambient(0.,0.,0.), light_diffuse(0.,0.,0.), light_specular(0.,0.,0.);
        
        const vector<lighthdl*> *lights;
        canvas->get_uniform("lights", lights);
        
        for (vector<lighthdl*>::const_iterator iter = lights->begin(); iter != lights->end(); ++iter) {
            if (*iter) {
                (*iter)->shade(light_ambient, light_diffuse, light_specular, eye_space_vertex, eye_space_normal, this->shininess);
            }
        }
        
        vec3f color = this->emission + light_ambient*this->ambient + light_diffuse*this->diffuse + light_specular*this->specular;
        for (int i = 0; i < 3; i++) {
            varying.push_back(color[i]);
        }
    }

	/* TODO Assignment 2: Implement flat and gouraud shading: Get the lights from the canvas using get_uniform.
	 * Add up the results of the shade functions for the lights. Transform the vertex and normal from world
	 * coordinates to eye coordinates before passing them into the shade functions. Calculate the final color
	 * and pass that to the fragment shader through the varying array. Pass the necessary data for phong shading
	 * through the varying array.
	 */

	eye_space_vertex = canvas->matrices[canvashdl::projection_matrix]*eye_space_vertex;
	eye_space_vertex /= eye_space_vertex[3];
	return eye_space_vertex;
}


vec3f uniformhdl::shade_fragment(canvashdl *canvas, vector<float> &varying) const
{
	/* TODO Assignment 2: For flat and gouraud shading, just return the color you passed through the varying array.
	 * Implement phong shading, doing the same thing that you did to implement the gouraud and flat. The difference
	 * is that the normals have been interpolated. Implement the none shading model, this just returns the
	 * color of the material without lighting.
	 */
    vec3f color = vec3f(1.0, 1.0, 1.0);
    
    switch (canvas->shade_model) {
        case canvashdl::none: {
            color = this->emission + this->ambient + this->diffuse + this->specular;
            break;
        }
        case canvashdl::flat: {
            color = vec3f(varying[0], varying[1], varying[2]);
            break;
        }
        case canvashdl::gouraud: {
            color = vec3f(varying[0], varying[1], varying[2]);
            break;
        }
        case canvashdl::phong: {
            vec4f eye_space_vertex = canvas->matrices[canvashdl::modelview_matrix]*homogenize(vec3f(varying[0],varying[1],varying[2]));
            vec4f eye_space_normal = canvas->matrices[canvashdl::normal_matrix]*homogenize(vec3f(varying[3],varying[4],varying[5]));
            
            vec3f light_ambient(0.,0.,0.), light_diffuse(0.,0.,0.), light_specular(0.,0.,0.);
            
            const vector<lighthdl*> *lights;
            canvas->get_uniform("lights", lights);
            
            for (vector<lighthdl*>::const_iterator iter = lights->begin(); iter != lights->end(); ++iter) {
                if (*iter) {
                    (*iter)->shade(light_ambient, light_diffuse, light_specular, eye_space_vertex, eye_space_normal, this->shininess);
                }
            }
            
            color = this->emission + light_ambient*this->ambient + light_diffuse*this->diffuse + light_specular*this->specular;
            
            break;
        }
            
        default:
            break;
    }
    
    // Overflow
    for (int i = 0; i < 3; i++) {
        if (color[i] > 1.0) {
            color[i] = 1.0;
        }
    }

    return color;
}

materialhdl *uniformhdl::clone() const
{
	uniformhdl *result = new uniformhdl();
	result->type = type;
	result->emission = emission;
	result->ambient = ambient;
	result->diffuse = diffuse;
	result->specular = specular;
	result->shininess = shininess;
	return result;
}

nonuniformhdl::nonuniformhdl()
{
	type = "non_uniform";
    emission = vec3f(0.0, 0.0, 0.0);
    ambient = vec3f(0.1, 0.0, 0.1);
    diffuse = vec3f(1.0, 0.0, 1.0);
    specular = vec3f(1.0, 0.0, 1.0);
    shininess = 1.0;
}

nonuniformhdl::~nonuniformhdl()
{

}

vec3f nonuniformhdl::shade_vertex(canvashdl *canvas, vec3f vertex, vec3f normal, vector<float> &varying) const
{
	vec4f eye_space_vertex = canvas->matrices[canvashdl::modelview_matrix]*homogenize(vertex);

	/* TODO Assignment 2: Implement the vertex shader for some non-uniform material here. You can actually
	 * implement as many as you want, just make sure to make the correct changes in model.cpp when you load
	 * the material library. Same thing goes if you decide to rename this class.
	 */

    vec4f eye_space_normal = canvas->matrices[canvashdl::normal_matrix]*homogenize(normal);
    

    // Store varying
    if(canvas->shade_model == canvashdl::phong) {
        for (int i = 0; i < 3; i++) {
            varying.push_back(vertex[i]);
        }
        for (int i = 0; i < 3; i++) {
            varying.push_back(normal[i]);
        }
    } else {
        
        vec3f light_ambient(0.,0.,0.), light_diffuse(0.,0.,0.), light_specular(0.,0.,0.);
        
        const vector<lighthdl*> *lights;
        canvas->get_uniform("lights", lights);
        
        for (vector<lighthdl*>::const_iterator iter = lights->begin(); iter != lights->end(); ++iter) {
            if (*iter) {
                (*iter)->shade(light_ambient, light_diffuse, light_specular, eye_space_vertex, eye_space_normal, this->shininess);
            }
        }
        
        vec3f ambient = this->ambient, diffuse = this->diffuse, specular = this->specular;
        ambient *= (pow(cosf(vertex[0]*M_PI*4),2) + pow(sinf(vertex[1]*M_PI*4),2));
        diffuse *= (pow(cosf(vertex[0]*M_PI*4),2) + pow(sinf(vertex[1]*M_PI*4),2));
        specular *= (pow(cosf(vertex[0]*M_PI*4),2) + pow(sinf(vertex[1]*M_PI*4),2));
        
        vec3f color = this->emission + light_ambient*ambient + light_diffuse*diffuse + light_specular*specular;
        for (int i = 0; i < 3; i++) {
            varying.push_back(color[i]);
        }
    }
    
    eye_space_vertex = canvas->matrices[canvashdl::projection_matrix]*eye_space_vertex;
    eye_space_vertex /= eye_space_vertex[3];
    return eye_space_vertex;
}

vec3f nonuniformhdl::shade_fragment(canvashdl *canvas, vector<float> &varying) const
{
	/* TODO Assignment 2: Implement the fragment shader for some non-uniform material here. You can actually
	 * implement as many as you want, just make sure to make the correct changes in model.cpp when you load
	 * the material library. Same thing goes if you decide to rename this class.
	 */
    
    vec3f color = vec3f(1.0, 1.0, 1.0);
    
    switch (canvas->shade_model) {
        case canvashdl::none: {
            color = this->emission + this->ambient + this->diffuse + this->specular;
            break;
        }
        case canvashdl::flat: {
            color = vec3f(varying[0], varying[1], varying[2]);
            break;
        }
        case canvashdl::gouraud: {
            color = vec3f(varying[0], varying[1], varying[2]);
            break;
        }
        case canvashdl::phong: {
            vec4f eye_space_vertex = canvas->matrices[canvashdl::modelview_matrix]*homogenize(vec3f(varying[0],varying[1],varying[2]));
            vec4f eye_space_normal = canvas->matrices[canvashdl::normal_matrix]*homogenize(vec3f(varying[3],varying[4],varying[5]));
            
            vec3f light_ambient(0.,0.,0.), light_diffuse(0.,0.,0.), light_specular(0.,0.,0.);
            
            const vector<lighthdl*> *lights;
            canvas->get_uniform("lights", lights);
            
            for (vector<lighthdl*>::const_iterator iter = lights->begin(); iter != lights->end(); ++iter) {
                if (*iter) {
                    (*iter)->shade(light_ambient, light_diffuse, light_specular, eye_space_vertex, eye_space_normal, this->shininess);
                }
            }
            
            
            color = this->emission + light_ambient*this->ambient + light_diffuse*this->diffuse + light_specular*this->specular;
            
            
            break;
        }
            
        default:
            break;
    }
    
    // Overflow
    for (int i = 0; i < 3; i++) {
        if (color[i] > 1.0) {
            color[i] = 1.0;
        }
    }
    
    return color;
}

materialhdl *nonuniformhdl::clone() const
{
	nonuniformhdl *result = new nonuniformhdl();
	result->type = type;
	return result;
}
