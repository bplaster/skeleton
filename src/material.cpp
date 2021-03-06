#include "material.h"
#include "light.h"
#include "lodepng.h"

GLuint whitehdl::vertex = 0;
GLuint whitehdl::fragment = 0;
GLuint whitehdl::program = 0;

GLuint brickhdl::vertex = 0;
GLuint brickhdl::fragment = 0;
GLuint brickhdl::program = 0;

GLuint texturehdl::vertex = 0;
GLuint texturehdl::fragment = 0;
GLuint texturehdl::program = 0;
GLuint texturehdl::texture = 0;

GLuint gouraudhdl::vertex = 0;
GLuint gouraudhdl::fragment = 0;
GLuint gouraudhdl::program = 0;

GLuint phonghdl::vertex = 0;
GLuint phonghdl::fragment = 0;
GLuint phonghdl::program = 0;

GLuint nonuniformhdl::vertex = 0;
GLuint nonuniformhdl::fragment = 0;
GLuint nonuniformhdl::program = 0;

extern string working_directory;

materialhdl::materialhdl()
{
	type = "material";
}

materialhdl::~materialhdl()
{
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
        
        program = glCreateProgram();
        vertex = load_shader_file("res/white.vx", GL_VERTEX_SHADER);
        fragment = load_shader_file("res/white.ft", GL_FRAGMENT_SHADER);
        
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        
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
        
        program = glCreateProgram();
        vertex = load_shader_file("res/brick.vx", GL_VERTEX_SHADER);
        fragment = load_shader_file("res/brick.ft", GL_FRAGMENT_SHADER);
        
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
	}
}

brickhdl::~brickhdl()
{

}

void brickhdl::apply(const vector<lighthdl*> &lights)
{
	// TODO Assignment 3: Apply the shader program and pass it the necessary uniform values
    glUseProgram(program);
    
    int pCount = 0, sCount = 0, dCount = 0;
    for (int i = 0; i < lights.size(); i++) {
        if (lights[i]) {
            if (lights[i]->type == "point") {
                lights[i]->apply(to_string(pCount), program);
                pCount++;
            } else if (lights[i]->type == "spot"){
                lights[i]->apply(to_string(sCount), program);
                sCount++;
            } else if (lights[i]->type == "directional") {
                lights[i]->apply(to_string(dCount), program);
                dCount++;
            }
        }
    }
    
    check (pCount, "point", program);
    check (sCount, "spot", program);
    check (dCount, "directional", program);

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
        program = glCreateProgram();
        vertex = load_shader_file("res/texture.vx", GL_VERTEX_SHADER);
        fragment = load_shader_file("res/texture.ft", GL_FRAGMENT_SHADER);
        
        //load and decode
        vector<unsigned char> image;
        unsigned width, height;
        unsigned error = lodepng::decode(image, width, height, "res/texture.png");
        
        glGenTextures(1, &texture);
        
        // Set to texture slot 0
        // TODO: Add enum for texture if and when there are multiple textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
	}
}

texturehdl::~texturehdl()
{
}

void texturehdl::apply(const vector<lighthdl*> &lights)
{
	// TODO Assignment 3: Apply the shader program and pass it the necessary uniform values
    glUseProgram(program);
    GLint shLoc = glGetUniformLocation(program, "shininess");
    GLint txLoc = glGetUniformLocation(program, "tex");

    glUniform1f(shLoc, this->shininess);
    glUniform1f(txLoc, 0);

    int pCount = 0, sCount = 0, dCount = 0;
    for (int i = 0; i < lights.size(); i++) {
        if (lights[i]) {
            if (lights[i]->type == "point") {
                lights[i]->apply(to_string(pCount), program);
                pCount++;
            } else if (lights[i]->type == "spot"){
                lights[i]->apply(to_string(sCount), program);
                sCount++;
            } else if (lights[i]->type == "directional") {
                lights[i]->apply(to_string(dCount), program);
                dCount++;
            }
        }
    }
    check (pCount, "point", program);
    check (sCount, "spot", program);
    check (dCount, "directional", program);

}

materialhdl *texturehdl::clone() const
{
	texturehdl *result = new texturehdl();
	result->type = type;
	result->shininess = shininess;
	return result;
}


gouraudhdl::gouraudhdl()
{
    type = "gouraud";
    emission = vec3f(0.0, 0.0, 0.0);
    ambient = vec3f(0.1, 0.1, 0.1);
    diffuse = vec3f(1.0, 1.0, 1.0);
    specular = vec3f(1.0, 1.0, 1.0);
    shininess = 1.0;
    
    if (vertex == 0 && fragment == 0 && program == 0)
    {
        /* TODO Assignment 3: Load and link the shaders. Keep in mind that vertex, fragment,
         * and program are static variables meaning they are *shared across all instances of
         * this class. So you only have to initialize them once when the first instance of
         * the class is created.
         */
        program = glCreateProgram();
        vertex = load_shader_file("res/gourauds.vx", GL_VERTEX_SHADER);
        fragment = load_shader_file("res/gourauds.ft", GL_FRAGMENT_SHADER);
        
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
    }
}

gouraudhdl::~gouraudhdl()
{
    
}

void gouraudhdl::apply(const vector<lighthdl*> &lights)
{
    // TODO Assignment 3: Apply the shader program and pass it the necessary uniform values
    glUseProgram(program);
    GLint emLoc = glGetUniformLocation(program, "emission");
    GLint amLoc = glGetUniformLocation(program, "ambient");
    GLint diLoc = glGetUniformLocation(program, "diffuse");
    GLint spLoc = glGetUniformLocation(program, "specular");
    GLint shLoc = glGetUniformLocation(program, "shininess");
    
    glUniform3fv(emLoc, 1, &this->emission[0]);
    glUniform3fv(amLoc, 1, &this->ambient[0]);
    glUniform3fv(diLoc, 1, &this->diffuse[0]);
    glUniform3fv(spLoc, 1, &this->specular[0]);
    glUniform1f(shLoc, this->shininess);
    
    int pCount = 0, sCount = 0, dCount = 0;
    for (int i = 0; i < lights.size(); i++) {
        if (lights[i]) {
            if (lights[i]->type == "point") {
                lights[i]->apply(to_string(pCount), program);
                pCount++;
            } else if (lights[i]->type == "spot"){
                lights[i]->apply(to_string(sCount), program);
                sCount++;
            } else if (lights[i]->type == "directional") {
                lights[i]->apply(to_string(dCount), program);
                dCount++;
            }
        }
    }
    
    check (pCount, "point", program);
    check (sCount, "spot", program);
    check (dCount, "directional", program);
}

materialhdl *gouraudhdl::clone() const
{
    gouraudhdl *result = new gouraudhdl();
    result->type = type;
    result->emission = emission;
    result->ambient = ambient;
    result->diffuse = diffuse;
    result->specular = specular;
    result->shininess = shininess;
    return result;
}

phonghdl::phonghdl()
{
    type = "phong";
    emission = vec3f(0.0, 0.0, 0.0);
    ambient = vec3f(0.1, 0.1, 0.1);
    diffuse = vec3f(1.0, 1.0, 1.0);
    specular = vec3f(1.0, 1.0, 1.0);
    shininess = 1.0;
    
    if (vertex == 0 && fragment == 0 && program == 0)
    {
        /* TODO Assignment 3: Load and link the shaders. Keep in mind that vertex, fragment,
         * and program are static variables meaning they are *shared across all instances of
         * this class. So you only have to initialize them once when the first instance of
         * the class is created.
         */
        program = glCreateProgram();
        vertex = load_shader_file("res/phongs.vx", GL_VERTEX_SHADER);
        fragment = load_shader_file("res/phongs.ft", GL_FRAGMENT_SHADER);

        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
    }
}

phonghdl::~phonghdl()
{
    
}

void phonghdl::apply(const vector<lighthdl*> &lights)
{
    // TODO Assignment 3: Apply the shader program and pass it the necessary uniform values
    glUseProgram(program);
    GLint emLoc = glGetUniformLocation(program, "emission");
    GLint amLoc = glGetUniformLocation(program, "ambient");
    GLint diLoc = glGetUniformLocation(program, "diffuse");
    GLint spLoc = glGetUniformLocation(program, "specular");
    GLint shLoc = glGetUniformLocation(program, "shininess");
    
    glUniform3fv(emLoc, 1, &this->emission[0]);
    glUniform3fv(amLoc, 1, &this->ambient[0]);
    glUniform3fv(diLoc, 1, &this->diffuse[0]);
    glUniform3fv(spLoc, 1, &this->specular[0]);
    glUniform1f(shLoc, this->shininess);
    
    int pCount = 0, sCount = 0, dCount = 0;
    for (int i = 0; i < lights.size(); i++) {
        if (lights[i]) {
            if (lights[i]->type == "point") {
                lights[i]->apply(to_string(pCount), program);
                pCount++;
            } else if (lights[i]->type == "spot"){
                lights[i]->apply(to_string(sCount), program);
                sCount++;
            } else if (lights[i]->type == "directional") {
                lights[i]->apply(to_string(dCount), program);
                dCount++;
            }
        }
    }
    
    check (pCount, "point", program);
    check (sCount, "spot", program);
    check (dCount, "directional", program);
}

materialhdl *phonghdl::clone() const
{
    phonghdl *result = new phonghdl();
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
    type = "nonuniform";
    emission = vec3f(0.0, 0.0, 0.0);
    ambient = vec3f(0.1, 0.1, 0.1);
    diffuse = vec3f(1.0, 1.0, 1.0);
    specular = vec3f(1.0, 1.0, 1.0);
    shininess = 1.0;
    
    if (vertex == 0 && fragment == 0 && program == 0)
    {
        /* TODO Assignment 3: Load and link the shaders. Keep in mind that vertex, fragment,
         * and program are static variables meaning they are *shared across all instances of
         * this class. So you only have to initialize them once when the first instance of
         * the class is created.
         */
        program = glCreateProgram();
        vertex = load_shader_file("res/nonuniform.vx", GL_VERTEX_SHADER);
        fragment = load_shader_file("res/nonuniform.ft", GL_FRAGMENT_SHADER);
        
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
    }
}

nonuniformhdl::~nonuniformhdl()
{
    
}

void nonuniformhdl::apply(const vector<lighthdl*> &lights)
{
    // TODO Assignment 3: Apply the shader program and pass it the necessary uniform values
    glUseProgram(program);
    GLint emLoc = glGetUniformLocation(program, "emission");
    GLint amLoc = glGetUniformLocation(program, "ambient");
    GLint diLoc = glGetUniformLocation(program, "diffuse");
    GLint spLoc = glGetUniformLocation(program, "specular");
    GLint shLoc = glGetUniformLocation(program, "shininess");
    
    glUniform3fv(emLoc, 1, &this->emission[0]);
    glUniform3fv(amLoc, 1, &this->ambient[0]);
    glUniform3fv(diLoc, 1, &this->diffuse[0]);
    glUniform3fv(spLoc, 1, &this->specular[0]);
    glUniform1f(shLoc, this->shininess);
    
    int pCount = 0, sCount = 0, dCount = 0;
    for (int i = 0; i < lights.size(); i++) {
        if (lights[i]) {
            if (lights[i]->type == "point") {
                lights[i]->apply(to_string(pCount), program);
                pCount++;
            } else if (lights[i]->type == "spot"){
                lights[i]->apply(to_string(sCount), program);
                sCount++;
            } else if (lights[i]->type == "directional") {
                lights[i]->apply(to_string(dCount), program);
                dCount++;
            }
        }
    }
    
    check (pCount, "point", program);
    check (sCount, "spot", program);
    check (dCount, "directional", program);
}

materialhdl *nonuniformhdl::clone() const
{
    nonuniformhdl *result = new nonuniformhdl();
    result->type = type;
    result->emission = emission;
    result->ambient = ambient;
    result->diffuse = diffuse;
    result->specular = specular;
    result->shininess = shininess;
    return result;
}

// Gets rid of additional light information stored in uniform
void materialhdl::check (int count, string light_type, GLuint program) {
    GLint nLoc;
    string startlName;
    vec3f replace = {0.0,0.0,0.0};
    
    if (light_type == "point"){
        nLoc = glGetUniformLocation(program, "num_plights");
        startlName = "plights[";
        int value;
        glGetUniformiv(program, nLoc, &value);
        if (value > count) {
            for (int i=count+1; i<(value+1); i++){
                string lName = startlName + to_string(i) + "].";
                GLint poLoc = glGetUniformLocation(program, (lName+"position").c_str());
                GLint amLoc = glGetUniformLocation(program, (lName+"ambient").c_str());
                GLint diLoc = glGetUniformLocation(program, (lName+"diffuse").c_str());
                GLint spLoc = glGetUniformLocation(program, (lName+"specular").c_str());
                GLint atLoc = glGetUniformLocation(program, (lName+"attenuation").c_str());
                
                glUniform3fv(poLoc, 1, &replace[0]);
                glUniform3fv(amLoc, 1, &replace[0]);
                glUniform3fv(diLoc, 1, &replace[0]);
                glUniform3fv(spLoc, 1, &replace[0]);
                glUniform3fv(atLoc, 1, &replace[0]);
            }
        }

    }
    else if (light_type == "spot") {
        nLoc = glGetUniformLocation(program, "num_slights");
        startlName = "slights[";
        int value;
        glGetUniformiv(program, nLoc, &value);
        if (value > count) {
            for (int i=count+1; i<(value+1); i++){
                string lname = startlName + to_string(i) + "].";
                GLint poLoc = glGetUniformLocation(program, (lname+"position").c_str());
                GLint drLoc = glGetUniformLocation(program, (lname+"direction").c_str());
                
                GLint amLoc = glGetUniformLocation(program, (lname+"ambient").c_str());
                GLint diLoc = glGetUniformLocation(program, (lname+"diffuse").c_str());
                GLint spLoc = glGetUniformLocation(program, (lname+"specular").c_str());
                
                GLint atLoc = glGetUniformLocation(program, (lname+"attenuation").c_str());
                GLint cuLoc = glGetUniformLocation(program, (lname+"cutoff").c_str());
                GLint exLoc = glGetUniformLocation(program, (lname+"exponent").c_str());
                
                glUniform3fv(poLoc, 1, &replace[0]);
                glUniform3fv(drLoc, 1, &replace[0]);
                glUniform3fv(amLoc, 1, &replace[0]);
                glUniform3fv(diLoc, 1, &replace[0]);
                glUniform3fv(spLoc, 1, &replace[0]);
                glUniform3fv(atLoc, 1, &replace[0]);
                glUniform1f(cuLoc, 0.0);
                glUniform1f(exLoc, 0.0);
            }
        }

    }
    else if (light_type == "directional") {
        nLoc = glGetUniformLocation(program, "num_dlights");
        startlName = "dlights[";
        int value;
        glGetUniformiv(program, nLoc, &value);
        if (value > count) {
            for (int i=count+1; i<(value+1); i++){
                string lName = startlName + to_string(i) + "].";
                GLint drLoc = glGetUniformLocation(program, (lName+"direction").c_str());
                GLint amLoc = glGetUniformLocation(program, (lName+"ambient").c_str());
                GLint diLoc = glGetUniformLocation(program, (lName+"diffuse").c_str());
                GLint spLoc = glGetUniformLocation(program, (lName+"specular").c_str());
                
                glUniform3fv(drLoc, 1, &replace[0]);
                glUniform3fv(amLoc, 1, &replace[0]);
                glUniform3fv(diLoc, 1, &replace[0]);
                glUniform3fv(spLoc, 1, &replace[0]);
            }
        }

    }
    
    glUniform1i(nLoc, count);
}
