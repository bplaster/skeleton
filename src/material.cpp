#include "material.h"
#include "light.h"
#include "lodepng.h"

GLuint whitehdl::vertex = 0;
GLuint whitehdl::fragment = 0;
GLuint whitehdl::program = 0;

GLuint solidhdl::vertex = 0;
GLuint solidhdl::fragment = 0;
GLuint solidhdl::program = 0;

GLuint brickhdl::vertex = 0;
GLuint brickhdl::fragment = 0;
GLuint brickhdl::program = 0;

GLuint texturehdl::vertex = 0;
GLuint texturehdl::fragment = 0;
GLuint texturehdl::program = 0;
GLuint texturehdl::texture = 0;

extern string working_directory;

materialhdl::materialhdl()
{
	type = "material";
}

materialhdl::~materialhdl()
{
}

solidhdl::solidhdl()
{
	type = "solid";
	emission = vec3f(0.0, 0.0, 0.0);
	ambient = vec3f(0.2, 0.2, 0.2);
    diffuse = vec3f(0.1, 0.1, 1.0);
    specular = vec3f(1.0, 1.0, 1.0);
	shininess = 3.0;
    
    if (vertex == 0 && fragment == 0 && program == 0)
    {
        /* TODO Assignment 3: Load and link the shaders. Keep in mind that vertex, fragment,
         * and program are static variables meaning they are *shared across all instances of
         * this class. So you only have to initialize them once when the first instance of
         * the class is created.
         */
    }
}


solidhdl::~solidhdl()
{
}

//vec3f uniformhdl::shade_vertex(canvashdl *canvas, vec3f vertex, vec3f normal, vector<float> &varying) const
//{
//    vec4f eye_space_vertex = canvas->matrices[canvashdl::modelview_matrix]*homogenize(vertex);
//    vec4f eye_space_normal = canvas->matrices[canvashdl::normal_matrix]*homogenize(normal);
//    
//    // Store varying
//    if(canvas->shade_model == canvashdl::phong) {
//        for (int i = 0; i < 3; i++) {
//            varying.push_back(vertex[i]);
//        }
//        for (int i = 0; i < 3; i++) {
//            varying.push_back(normal[i]);
//        }
//    } else {
//        
//        vec3f light_ambient(0.,0.,0.), light_diffuse(0.,0.,0.), light_specular(0.,0.,0.);
//        
//        const vector<lighthdl*> *lights;
//        canvas->get_uniform("lights", lights);
//        
//        for (vector<lighthdl*>::const_iterator iter = lights->begin(); iter != lights->end(); ++iter) {
//            if (*iter) {
//                (*iter)->shade(light_ambient, light_diffuse, light_specular, eye_space_vertex, eye_space_normal, this->shininess);
//            }
//        }
//        
//        vec3f color = this->emission + light_ambient*this->ambient + light_diffuse*this->diffuse + light_specular*this->specular;
//        for (int i = 0; i < 3; i++) {
//            varying.push_back(color[i]);
//        }
//    }
//    
//    /* TODO Assignment 2: Implement flat and gouraud shading: Get the lights from the canvas using get_uniform.
//     * Add up the results of the shade functions for the lights. Transform the vertex and normal from world
//     * coordinates to eye coordinates before passing them into the shade functions. Calculate the final color
//     * and pass that to the fragment shader through the varying array. Pass the necessary data for phong shading
//     * through the varying array.
//     */
//    
//    eye_space_vertex = canvas->matrices[canvashdl::projection_matrix]*eye_space_vertex;
//    eye_space_vertex /= eye_space_vertex[3];
//    return eye_space_vertex;
//}


//vec3f uniformhdl::shade_fragment(canvashdl *canvas, vector<float> &varying) const
//{
//	/* TODO Assignment 2: For flat and gouraud shading, just return the color you passed through the varying array.
//	 * Implement phong shading, doing the same thing that you did to implement the gouraud and flat. The difference
//	 * is that the normals have been interpolated. Implement the none shading model, this just returns the
//	 * color of the material without lighting.
//	 */
//    vec3f color = vec3f(1.0, 1.0, 1.0);
//    
//    switch (canvas->shade_model) {
//        case canvashdl::none: {
//            color = this->emission + this->ambient + this->diffuse + this->specular;
//            break;
//        }
//        case canvashdl::flat: {
//            color = vec3f(varying[0], varying[1], varying[2]);
//            break;
//        }
//        case canvashdl::gouraud: {
//            color = vec3f(varying[0], varying[1], varying[2]);
//            break;
//        }
//        case canvashdl::phong: {
//            vec4f eye_space_vertex = canvas->matrices[canvashdl::modelview_matrix]*homogenize(vec3f(varying[0],varying[1],varying[2]));
//            vec4f eye_space_normal = canvas->matrices[canvashdl::normal_matrix]*homogenize(vec3f(varying[3],varying[4],varying[5]));
//            
//            vec3f light_ambient(0.,0.,0.), light_diffuse(0.,0.,0.), light_specular(0.,0.,0.);
//            
//            const vector<lighthdl*> *lights;
//            canvas->get_uniform("lights", lights);
//            
//            for (vector<lighthdl*>::const_iterator iter = lights->begin(); iter != lights->end(); ++iter) {
//                if (*iter) {
//                    (*iter)->shade(light_ambient, light_diffuse, light_specular, eye_space_vertex, eye_space_normal, this->shininess);
//                }
//            }
//            
//            color = this->emission + light_ambient*this->ambient + light_diffuse*this->diffuse + light_specular*this->specular;
//            
//            break;
//        }
//            
//        default:
//            break;
//    }
//    
//    // Overflow
//    for (int i = 0; i < 3; i++) {
//        if (color[i] > 1.0) {
//            color[i] = 1.0;
//        }
//    }
//
//    return color;
//}

void solidhdl::apply(const vector<lighthdl*> &lights)
{
	// TODO Assignment 3: Apply the shader program and pass it the necessary uniform values
}

materialhdl *solidhdl::clone() const
{
	solidhdl *result = new solidhdl();
	result->type = type;
	result->emission = emission;
	result->ambient = ambient;
	result->diffuse = diffuse;
	result->specular = specular;
	result->shininess = shininess;
	return result;
}

whitehdl::whitehdl()
{
	type = "white";

	if (vertex == 0 && fragment == 0 && program == 0)
	{
		/* TODO Assignment 3: Load and link the shaders. Keep in mind that vertex, fragment, 
		 * and program are static variables meaning they are *shared across all instances of
		 * this class. So you only have to initialize them once when the first instance of
		 * the class is created.
		 */
	}
}

whitehdl::~whitehdl()
{

}

void whitehdl::apply(const vector<lighthdl*> &lights)
{
	glUseProgram(program);
}

materialhdl *whitehdl::clone() const
{
	whitehdl *result = new whitehdl();
	result->type = type;
	return result;
}

brickhdl::brickhdl()
{
	type = "brick";

	if (vertex == 0 && fragment == 0 && program == 0)
	{
		/* TODO Assignment 3: Load and link the shaders. Keep in mind that vertex, fragment, 
		 * and program are static variables meaning they are *shared across all instances of
		 * this class. So you only have to initialize them once when the first instance of
		 * the class is created.
		 */
	}
}

brickhdl::~brickhdl()
{

}

void brickhdl::apply(const vector<lighthdl*> &lights)
{
	// TODO Assignment 3: Apply the shader program and pass it the necessary uniform values
}

materialhdl *brickhdl::clone() const
{
	brickhdl *result = new brickhdl();
	result->type = type;
	return result;
}

texturehdl::texturehdl()
{
	type = "texture";

	shininess = 1.0;

	if (vertex == 0 && fragment == 0 && program == 0)
	{
		/* TODO Assignment 3: Load and link the shaders and load the texture Keep in mind that vertex, fragment,
		 * and program are static variables meaning they are *shared across all instances of
		 * this class. So you only have to initialize them once when the first instance of
		 * the class is created.
		 */
	}
}

texturehdl::~texturehdl()
{
}

void texturehdl::apply(const vector<lighthdl*> &lights)
{
	// TODO Assignment 3: Apply the shader program and pass it the necessary uniform values
}

materialhdl *texturehdl::clone() const
{
	texturehdl *result = new texturehdl();
	result->type = type;
	result->shininess = shininess;
	return result;
}

//vec3f nonuniformhdl::shade_vertex(canvashdl *canvas, vec3f vertex, vec3f normal, vector<float> &varying) const
//{
//    vec4f eye_space_vertex = canvas->matrices[canvashdl::modelview_matrix]*homogenize(vertex);
//    
//    /* TODO Assignment 2: Implement the vertex shader for some non-uniform material here. You can actually
//     * implement as many as you want, just make sure to make the correct changes in model.cpp when you load
//     * the material library. Same thing goes if you decide to rename this class.
//     */
//    
//    vec4f eye_space_normal = canvas->matrices[canvashdl::normal_matrix]*homogenize(normal);
//    
//    
//    // Store varying
//    if(canvas->shade_model == canvashdl::phong) {
//        for (int i = 0; i < 3; i++) {
//            varying.push_back(vertex[i]);
//        }
//        for (int i = 0; i < 3; i++) {
//            varying.push_back(normal[i]);
//        }
//    } else {
//        
//        vec3f light_ambient(0.,0.,0.), light_diffuse(0.,0.,0.), light_specular(0.,0.,0.);
//        
//        const vector<lighthdl*> *lights;
//        canvas->get_uniform("lights", lights);
//        
//        for (vector<lighthdl*>::const_iterator iter = lights->begin(); iter != lights->end(); ++iter) {
//            if (*iter) {
//                (*iter)->shade(light_ambient, light_diffuse, light_specular, eye_space_vertex, eye_space_normal, this->shininess);
//            }
//        }
//        
//        vec3f ambient = this->ambient, diffuse = this->diffuse, specular = this->specular;
//        ambient *= (pow(cosf(vertex[0]*M_PI*4),2) + pow(sinf(vertex[1]*M_PI*4),2));
//        diffuse *= (pow(cosf(vertex[0]*M_PI*4),2) + pow(sinf(vertex[1]*M_PI*4),2));
//        specular *= (pow(cosf(vertex[0]*M_PI*4),2) + pow(sinf(vertex[1]*M_PI*4),2));
//        
//        vec3f color = this->emission + light_ambient*ambient + light_diffuse*diffuse + light_specular*specular;
//        for (int i = 0; i < 3; i++) {
//            varying.push_back(color[i]);
//        }
//    }
//    
//    eye_space_vertex = canvas->matrices[canvashdl::projection_matrix]*eye_space_vertex;
//    eye_space_vertex /= eye_space_vertex[3];
//    return eye_space_vertex;
//}
//
//vec3f nonuniformhdl::shade_fragment(canvashdl *canvas, vector<float> &varying) const
//{
//    /* TODO Assignment 2: Implement the fragment shader for some non-uniform material here. You can actually
//     * implement as many as you want, just make sure to make the correct changes in model.cpp when you load
//     * the material library. Same thing goes if you decide to rename this class.
//     */
//    
//    vec3f color = vec3f(1.0, 1.0, 1.0);
//    
//    switch (canvas->shade_model) {
//        case canvashdl::none: {
//            color = this->emission + this->ambient + this->diffuse + this->specular;
//            break;
//        }
//        case canvashdl::flat: {
//            color = vec3f(varying[0], varying[1], varying[2]);
//            break;
//        }
//        case canvashdl::gouraud: {
//            color = vec3f(varying[0], varying[1], varying[2]);
//            break;
//        }
//        case canvashdl::phong: {
//            vec4f eye_space_vertex = canvas->matrices[canvashdl::modelview_matrix]*homogenize(vec3f(varying[0],varying[1],varying[2]));
//            vec4f eye_space_normal = canvas->matrices[canvashdl::normal_matrix]*homogenize(vec3f(varying[3],varying[4],varying[5]));
//            
//            vec3f light_ambient(0.,0.,0.), light_diffuse(0.,0.,0.), light_specular(0.,0.,0.);
//            
//            const vector<lighthdl*> *lights;
//            canvas->get_uniform("lights", lights);
//            
//            for (vector<lighthdl*>::const_iterator iter = lights->begin(); iter != lights->end(); ++iter) {
//                if (*iter) {
//                    (*iter)->shade(light_ambient, light_diffuse, light_specular, eye_space_vertex, eye_space_normal, this->shininess);
//                }
//            }
//            
//            vec3f ambient = this->ambient, diffuse = this->diffuse, specular = this->specular;
//            ambient *= (pow(cosf(varying[0]*M_PI*4),2) + pow(sinf(varying[1]*M_PI*4),2));
//            diffuse *= (pow(cosf(varying[0]*M_PI*4),2) + pow(sinf(varying[1]*M_PI*4),2));
//            specular *= (pow(cosf(varying[0]*M_PI*4),2) + pow(sinf(varying[1]*M_PI*4),2));
//            
//            color = this->emission + light_ambient*ambient + light_diffuse*diffuse + light_specular*specular;
//            
//            break;
//        }
//            
//        default:
//            break;
//    }
//    
//    // Overflow
//    for (int i = 0; i < 3; i++) {
//        if (color[i] > 1.0) {
//            color[i] = 1.0;
//        }
//    }
//    
//    return color;
//}

