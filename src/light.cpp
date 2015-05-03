#include "light.h"
#include "object.h"
#include "primitive.h"
#include "material.h"
#include "opengl.h"

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
	type = "light";
}

lighthdl::~lighthdl()
{

}

directionalhdl::directionalhdl() : lighthdl(white*0.1f, white*0.5f, white)
{
    model = new cylinderhdl(0.2, 1.0, 4.0);
//    ((uniformhdl *)model->material["material"])->emission = vec3f(1.0,1.0,1.0);
	type = "directional";
}

directionalhdl::directionalhdl(const vec3f &direction, const vec3f &ambient, const vec3f &diffuse, const vec3f &specular) : lighthdl(ambient, diffuse, specular)
{
    model = new cylinderhdl(0.2, 1.0, 4.0);
//    ((uniformhdl *)model->material["material"])->emission = vec3f(1.0,1.0,1.0);
	type = "directional";
}

directionalhdl::~directionalhdl()
{

}

void directionalhdl::update()
{
	/* TODO Assignment 2: Update the direction of the light using the orientation of the attached model.
	 * The easiest thing is to do translations and rotations like you were going to render the object, and
	 * then just multiply some initial direction vector by the normal matrix.
	 */
    glRotatef(this->model->orientation[2], 0., 0., 1.);
    glRotatef(this->model->orientation[1], 0., 1., 0.);
    glRotatef(this->model->orientation[0], 1., 0., 0.);
    
    GLfloat modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    mat4f modelviewmatrix = convert_to_matrix(modelview);
    mat4f normalmatrix = transpose(inverse(modelviewmatrix));
    this->direction = normalmatrix*vec4f(0.,-1.,0.,1.);
    
    glRotatef(-this->model->orientation[0], 1., 0., 0.);
    glRotatef(-this->model->orientation[1], 0., 1., 0.);
    glRotatef(-this->model->orientation[2], 0., 0., 1.);
}

void directionalhdl::apply(string name, GLuint program)
{
	/* TODO Assignment 3: Pass all necessary uniforms to the shaders for the directional light.
	 */
    
    string lName = "dlights[" + name + "].";
    
    GLint nLoc = glGetUniformLocation(program, "num_dlights");
    
    GLint drLoc = glGetUniformLocation(program, (lName+"direction").c_str());
    GLint amLoc = glGetUniformLocation(program, (lName+"ambient").c_str());
    GLint diLoc = glGetUniformLocation(program, (lName+"diffuse").c_str());
    GLint spLoc = glGetUniformLocation(program, (lName+"specular").c_str());
    
    glUniform1i(nLoc, stoi(name)+1);
    glUniform3fv(drLoc, 1, &this->direction[0]);
    glUniform3fv(amLoc, 1, &this->ambient[0]);
    glUniform3fv(diLoc, 1, &this->diffuse[0]);
    glUniform3fv(spLoc, 1, &this->specular[0]);
}

pointhdl::pointhdl() : lighthdl(white*0.1f, white*0.5f, white)
{
	this->attenuation = vec3f(1.0, 0.14, 0.7);
    model = new spherehdl(0.2, 4.0, 8.0);
//    ((uniformhdl *)model->material["material"])->emission = vec3f(1.0,1.0,1.0);
	type = "point";
}

pointhdl::pointhdl(const vec3f &position, const vec3f &attenuation, const vec3f &ambient, const vec3f &diffuse, const vec3f &specular) : lighthdl(ambient, diffuse, specular)
{
	this->attenuation = attenuation;
    model = new spherehdl(0.2, 4.0, 8.0);
//    ((uniformhdl *)model->material["material"])->emission = vec3f(1.0,1.0,1.0);
	type = "point";
}

pointhdl::~pointhdl()
{

}

void pointhdl::update()
{
	/* TODO Assignment 2: Update the position of the light using the position of the attached model.
	 * The easiest thing is to do translations and rotations like you were going to render the object, and
	 * then just multiply the origin by the modelview matrix.
	 */
    glTranslatef(this->model->position[0], this->model->position[1], this->model->position[2]);
    
    GLfloat modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    mat4f modelviewmatrix = convert_to_matrix(modelview);
    this->position = modelviewmatrix*vec4f(0.,0.,0.,1.);
    
    glTranslatef(-this->model->position[0], -this->model->position[1], -this->model->position[2]);
}

void pointhdl::apply(string name, GLuint program)
{
	/* TODO Assignment 3: Pass all necessary uniforms to the shaders for point lights.
	 */
    
    string lName = "plights[" + name + "].";

    GLint nLoc = glGetUniformLocation(program, "num_plights");
    
    GLint poLoc = glGetUniformLocation(program, (lName+"position").c_str());
    GLint amLoc = glGetUniformLocation(program, (lName+"ambient").c_str());
    GLint diLoc = glGetUniformLocation(program, (lName+"diffuse").c_str());
    GLint spLoc = glGetUniformLocation(program, (lName+"specular").c_str());
    GLint atLoc = glGetUniformLocation(program, (lName+"attenuation").c_str());
    
    glUniform1i(nLoc, stoi(name)+1);
    glUniform3fv(poLoc, 1, &this->position[0]);
    glUniform3fv(amLoc, 1, &this->ambient[0]);
    glUniform3fv(diLoc, 1, &this->diffuse[0]);
    glUniform3fv(spLoc, 1, &this->specular[0]);
    glUniform3fv(atLoc, 1, &this->attenuation[0]);

}

spothdl::spothdl() : lighthdl(white*0.1f, white*0.5f, white)
{
	this->attenuation = vec3f(1.0, 0.14, 0.7);
	this->cutoff = 0.5;
	this->exponent = 1.0;
    model = new pyramidhdl(0.2, 1.0, 8.0);
//    ((uniformhdl *)model->material["material"])->emission = vec3f(1.0,1.0,1.0);
	type = "spot";
}

spothdl::spothdl(const vec3f &attenuation, const float &cutoff, const float &exponent, const vec3f &ambient, const vec3f &diffuse, const vec3f &specular) : lighthdl(ambient, diffuse, specular)
{
	this->attenuation = attenuation;
	this->cutoff = cutoff;
	this->exponent = exponent;
    model = new pyramidhdl(0.2, 1.0, 8.0);
//    ((uniformhdl *)model->material["material"])->emission = vec3f(1.0,1.0,1.0);
	type = "spot";
}

spothdl::~spothdl()
{

}

void spothdl::update()
{
	/* TODO Assignment 2: Update both the direction and position of the light using the position and orientation
	 * of the attached model. See above.
	 */
    glTranslatef(this->model->position[0], this->model->position[1], this->model->position[2]);
    glRotatef(this->model->orientation[2], 0., 0., 1.);
    glRotatef(this->model->orientation[1], 0., 1., 0.);
    glRotatef(this->model->orientation[0], 1., 0., 0.);
    // Need to update normal matrix if necessary
    
    GLfloat modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    mat4f modelviewmatrix = convert_to_matrix(modelview);
    this->position = modelviewmatrix*vec4f(0.,0.,0.,1.);
    
    mat4f normalmatrix = transpose(inverse(modelviewmatrix));
    this->direction = normalmatrix*vec4f(0.,-1.,0.,1.);
    
    glRotatef(-this->model->orientation[0], 1., 0., 0.);
    glRotatef(-this->model->orientation[1], 0., 1., 0.);
    glRotatef(-this->model->orientation[2], 0., 0., 1.);
    glTranslatef(-this->model->position[0], -this->model->position[1], -this->model->position[2]);
//
//    this->position = canvas->matrices[canvashdl::modelview_matrix]*vec4f(0.,0.,0.,1.);
//    this->direction = canvas->matrices[canvashdl::normal_matrix]*vec4f(0.,-1.,0.,1.);

}

void spothdl::apply(string name, GLuint program)
{
	/* TODO Assignment 3: Pass all necessary uniforms to the shaders for spot lights.
	 */
    
}

ambienthdl::ambienthdl() : lighthdl(white*0.7f, white*0.0f, white*0.0f)
{
    type = "ambient";
    model = new boxhdl(0.5, 0.5, 0.5);
//    ((uniformhdl *)model->material["material"])->emission = vec3f(1.0,1.0,1.0);
}

void ambienthdl::update()
{
    
}

ambienthdl::~ambienthdl()
{
    
}

void ambienthdl::apply(string name, GLuint program)
{

}

mat4f lighthdl::convert_to_matrix (GLfloat arr[16]) {
    mat4f matrix;
    
    for (int i=0; i<4; i++){
        for (int j=0; j<4; j++){
            matrix[j][i] = arr[i+j];
        }
    }
    
    return matrix;
}
