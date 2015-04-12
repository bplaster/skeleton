#include <OpenGL/OpenGL.h>
#include <GLUI/GLUI.h>
#include "standard.h"
#include "canvas.h"
#include "scene.h"
#include "camera.h"
#include "model.h"
#include "primitive.h"
#include "tinyfiledialogs.h"
#include "core/geometry.h"
#include "light.h"


canvashdl canvas(750, 750);
scenehdl scene;

int mousex = 0, mousey = 0;
bool bound = false;
bool menu = false;

namespace manipulate
{
	enum type
	{
		none = -1,
		translate = 0,
		rotate = 1,
		scale = 2,
		fovy = 3,
		aspect = 4,
		width = 5,
		height = 6,
		near = 7,
		far = 8
	};
}

enum Object{
    Box,
    Cylinder,
    Sphere,
    Pyramid,
    Model
};

enum Camera{
    Ortho,
    Frustum,
    Perspective
};

enum Light{
    Ambient,
    Point,
    Spot,
    Directional
};

enum LightColor {
    White,
    Red,
    Green,
    Blue,
    Orange,
    Yellow,
    Indigo,
    Violet,
    Black,
    Brown
};

enum Material {
    Uniform,
    NonUniform
};

bool keys[256];

// GLUI Variables
int current_manipulation = manipulate::translate;
int current_polygon = canvashdl::Polygon::line;
int current_culling = canvashdl::Culling::backface;
int current_normal = scenehdl::Normal::none;
int current_shading = canvashdl::Shading::none;
int current_model;
float fovy, aspect, width, height, near, far;
float attenuation0;
float attenuation1;
float attenuation2;
int current_light_color;
int current_material;

int object_option_menu_id;
int canvas_menu_id;
int camera_menu_id;
int window_id;

string models[] = {"banana.obj", "beethoven.obj", "bishop.obj", "blob.obj", "Bunny.obj", "chain.obj", "cow.obj", "cowUV.obj", "demo.obj", "dolphin.obj", "feline.obj", "helmet.obj", "pawn.obj", "shirt.obj", "statue.obj", "teapot.obj"};
GLUI *glui;
GLUI_FileBrowser *file_browser;
GLUI_Listbox *current_objects;
GLUI_Listbox *current_cameras;
GLUI_Listbox *current_lights;
GLUI_Listbox *list_shading;
GLUI_Listbox *list_normal;
GLUI_Listbox *list_culling;
GLUI_Listbox *list_polygon;
GLUI_Listbox *list_manipulation;
GLUI_Listbox *list_material;
GLUI_Panel *camera_panel;
GLUI_Panel *light_panel;
GLUI_Panel *shading_panel;
GLUI_Listbox *list_light_colors;
GLUI_Panel *object_panel;
GLUI_EditText *fovy_text;
GLUI_EditText *aspect_text;
GLUI_EditText *near_text;
GLUI_EditText *far_text;
GLUI_EditText *light_color_text;
GLUI_EditText *attenuation_text_x;
GLUI_EditText *attenuation_text_y;
GLUI_EditText *attenuation_text_z;

GLUI_Listbox *list_model;
GLUI_Checkbox *focus_checkbox;
GLUI_Panel *cam_prop_panel;
GLUI_Panel *light_prop_panel;

// Helper functions
void create_camera(int val);
void create_object(int val);
void create_light (int val);

void handle_culling(int val);
void handle_polygon(int val);
void handle_normal(int val);
void handle_manip(int val);

void setup_glui();
void set_camera_info(int obj_ind);
void set_light_info(int obj_ind);


void init(string working_directory)
{
	for (int i = 0; i < 256; i++)
		keys[i] = false;

	canvas.working_directory = working_directory;
	scene.canvas = &canvas;
	// TODO Assignment 1: Initialize the Scene as necessary.
    
    // Setup GLUI
    setup_glui();
    
    // Create camera
    create_camera(Camera::Frustum);
    
    // Setup settings
    handle_culling(current_culling);
    handle_polygon(current_polygon);
    handle_normal(current_normal);
    
    // Create object
    create_object(Object::Sphere);
    
    // Create light
    create_light(Light::Point);
}

void displayfunc()
{
	canvas.clear_color_buffer();
	canvas.clear_depth_buffer();

	scene.draw();

	canvas.swap_buffers();
}

void reshapefunc(int w, int h)
{
	canvas.viewport(0, 0, w, h);
	glutPostRedisplay();
}

void pmotionfunc(int x, int y)
{
    if (bound)
    {
//        glutSetMenu(canvas_menu_id);
        
        int deltax = x - mousex;
        int deltay = y - mousey;
        
        mousex = x;
        mousey = y;
        
        bool warp = false;
        if (mousex > 3*canvas.get_width()/4 || mousex < canvas.get_width()/4)
        {
            mousex = canvas.get_width()/2;
            warp = true;
        }
        
        if (mousey > 3*canvas.get_height()/4 || mousey < canvas.get_height()/4)
        {
            mousey = canvas.get_height()/2;
            warp = true;
        }
        
        if (warp)
            glutWarpPointer(mousex, mousey);
        
        if (scene.active_camera_valid())
        {
            scene.cameras[scene.active_camera]->orientation[1] -= (float)deltax/500.0;
            scene.cameras[scene.active_camera]->orientation[0] -= (float)deltay/500.0;
        }
        
        glutPostRedisplay();
    }
    else if (scene.active_camera_valid())
    {
        vec3f direction;
        vec3f position;
        
        if (scene.active_camera_valid())
        {
            if (scene.cameras[scene.active_camera]->type == "ortho")
            {
                position = canvas.unproject(canvas.to_window(vec2i(x, y)));
                direction = ror3(vec3f(0.0f, 0.0f, 1.0f), scene.cameras[scene.active_camera]->orientation);
            }
            else
            {
                position = scene.cameras[scene.active_camera]->position;
                direction = norm(canvas.unproject(canvas.to_window(vec2i(x, y))));
            }
        }
        
        int old_active_object = scene.active_object;
        scene.active_object = -1;
        scene.active_light = -1;
        for (int i = 0; i < scene.objects.size(); i++)
        {
            if (scene.objects[i] != NULL && scene.cameras[scene.active_camera]->model != scene.objects[i])
            {
                bool is_camera = false;
                bool is_light = false;
                
                for (int j = 0; j < scene.cameras.size() && !is_camera; j++)
                    if (scene.cameras[j] != NULL && scene.cameras[j]->model == scene.objects[i])
                        is_camera = true;
                
//                for (int j = 0; j < scene.lights.size() && !is_light; j++) {
//                    if (scene.lights[j] != NULL && scene.lights[j]->model == scene.objects[i]){
//                        is_light = true;
//                    }
//                }
                
                if (!is_camera || (is_camera && scene.render_cameras))
                {
                    vec3f invdir = 1.0f/direction;
                    vec3i sign((int)(invdir[0] < 0), (int)(invdir[1] < 0), (int)(invdir[2] < 0));
                    vec3f origin = position - scene.objects[i]->position;
                    float tmin, tmax, tymin, tymax, tzmin, tzmax;
                    tmin = (scene.objects[i]->bound[0 + sign[0]]*scene.objects[i]->scale - origin[0])*invdir[0];
                    tmax = (scene.objects[i]->bound[0 + 1-sign[0]]*scene.objects[i]->scale - origin[0])*invdir[0];
                    tymin = (scene.objects[i]->bound[2 + sign[1]]*scene.objects[i]->scale - origin[1])*invdir[1];
                    tymax = (scene.objects[i]->bound[2 + 1-sign[1]]*scene.objects[i]->scale - origin[1])*invdir[1];
                    if ((tmin <= tymax) && (tymin <= tmax))
                    {
                        if (tymin > tmin)
                            tmin = tymin;
                        if (tymax < tmax)
                            tmax = tymax;
                        
                        tzmin = (scene.objects[i]->bound[4 + sign[2]]*scene.objects[i]->scale - origin[2])*invdir[2];
                        tzmax = (scene.objects[i]->bound[4 + 1-sign[2]]*scene.objects[i]->scale - origin[2])*invdir[2];
                        
                        if ((tmin <= tzmax) && (tzmin <= tmax))
                        {
                            scene.active_object = i;
                            i = scene.objects.size();
                        }
                    }
                }
            }
        }
        
        // Draw bounding box for light
        if (scene.render_lights) {
            for (int i = 0; i < scene.lights.size(); i++) {
                if (scene.lights[i]) {
                    vec3f invdir = 1.0f/direction;
                    vec3i sign((int)(invdir[0] < 0), (int)(invdir[1] < 0), (int)(invdir[2] < 0));
                    vec3f origin = position - scene.lights[i]->model->position;
                    float tmin, tmax, tymin, tymax, tzmin, tzmax;
                    tmin = (scene.lights[i]->model->bound[0 + sign[0]]*scene.lights[i]->model->scale - origin[0])*invdir[0];
                    tmax = (scene.lights[i]->model->bound[0 + 1-sign[0]]*scene.lights[i]->model->scale - origin[0])*invdir[0];
                    tymin = (scene.lights[i]->model->bound[2 + sign[1]]*scene.lights[i]->model->scale - origin[1])*invdir[1];
                    tymax = (scene.lights[i]->model->bound[2 + 1-sign[1]]*scene.lights[i]->model->scale - origin[1])*invdir[1];
                    if ((tmin <= tymax) && (tymin <= tmax))
                    {
                        if (tymin > tmin)
                            tmin = tymin;
                        if (tymax < tmax)
                            tmax = tymax;
                        
                        tzmin = (scene.lights[i]->model->bound[4 + sign[2]]*scene.lights[i]->model->scale - origin[2])*invdir[2];
                        tzmax = (scene.lights[i]->model->bound[4 + 1-sign[2]]*scene.lights[i]->model->scale - origin[2])*invdir[2];
                        
                        if ((tmin <= tzmax) && (tzmin <= tmax))
                        {
                            scene.active_light = i;
                            i = scene.lights.size();
                        }
                    }
                }
            }
            glutPostRedisplay();
        }
        
        if (scene.active_object != old_active_object)
        {
            bool is_camera = false;
            
            for (int i = 0; i < scene.cameras.size() && !is_camera; i++)
                if (scene.cameras[i] != NULL && scene.active_object_valid() && scene.cameras[i]->model == scene.objects[scene.active_object])
                    is_camera = true;
            
//            glutDetachMenu(GLUT_RIGHT_BUTTON);
//            if (scene.active_object == -1)
//                glutSetMenu(canvas_menu_id);
//            else if (is_camera)
//                glutSetMenu(camera_menu_id);
//            else
//                glutSetMenu(object_menu_id);
//            glutAttachMenu(GLUT_RIGHT_BUTTON);
            glutPostRedisplay();
        }
    }
}

void mousefunc(int button, int state, int x, int y)
{
	mousex = x;
	mousey = y;
}

void motionfunc(int x, int y)
{
    if (!bound && !menu)
    {
        // TODO: make so it's not setting the center, but instead, naturally dragging object
        int deltax = x - mousex;
        int deltay = mousey - y;
        
        mousex = x;
        mousey = y;
        
        vec3f direction;
        vec3f position;
        vec3f delta;
        
        if (scene.active_camera_valid())
        {
            if (scene.cameras[scene.active_camera]->type == "ortho")
            {
                position = canvas.unproject(canvas.to_window(vec2i(x, y)));
                direction = ror3(vec3f(0.0f, 0.0f, 1.0f), scene.cameras[scene.active_camera]->orientation);
            }
            else
            {
                position = scene.cameras[scene.active_camera]->position;
                direction = norm(canvas.unproject(canvas.to_window(vec2i(x, y))));
            }
        }
        
        // Manipulation Logic
        if (scene.active_object_valid() && scene.active_camera_valid())
        {
            if (current_manipulation == manipulate::translate)
            {
                float d = mag(scene.objects[scene.active_object]->position - position);
                scene.objects[scene.active_object]->position = d*direction + position;
            }
            else if (current_manipulation == manipulate::rotate)
                scene.objects[scene.active_object]->orientation += vec3f(-(float)deltay/100.0, (float)deltax/100.0, 0.0);
            else if (current_manipulation == manipulate::scale)
                scene.objects[scene.active_object]->scale += (float)deltay/100.0;
            
            for (int i = 0; i < scene.cameras.size(); i++)
                if (scene.cameras[i]->model == scene.objects[scene.active_object])
                {
                    scene.cameras[i]->position = scene.objects[scene.active_object]->position;
                    scene.cameras[i]->orientation = scene.objects[scene.active_object]->orientation;
                }
        }
        
        if (scene.active_light_valid()){
            if (current_manipulation == manipulate::translate)
            {
                float d = mag(scene.lights[scene.active_light]->model->position - position);
                scene.lights[scene.active_light]->model->position = d*direction + position;
            }
            else if (current_manipulation == manipulate::rotate)
                scene.lights[scene.active_light]->model->orientation += vec3f(-(float)deltay/100.0, (float)deltax/100.0, 0.0);
        }
        
        if (scene.active_camera_valid())
        {
            if (current_manipulation == manipulate::fovy && scene.cameras[scene.active_camera]->type == "perspective")
                ((perspectivehdl*)scene.cameras[scene.active_camera])->fovy += (float)deltay/100.0;
            else if (current_manipulation == manipulate::aspect && scene.cameras[scene.active_camera]->type == "perspective")
                ((perspectivehdl*)scene.cameras[scene.active_camera])->aspect += (float)deltay/100.0;
            else if (current_manipulation == manipulate::width && scene.cameras[scene.active_camera]->type == "ortho")
            {
                ((orthohdl*)scene.cameras[scene.active_camera])->right += (float)deltay/200.0;
                ((orthohdl*)scene.cameras[scene.active_camera])->left -= (float)deltay/200.0;
            }
            else if (current_manipulation == manipulate::width && scene.cameras[scene.active_camera]->type == "frustum")
            {
                ((frustumhdl*)scene.cameras[scene.active_camera])->right += (float)deltay/200.0;
                ((frustumhdl*)scene.cameras[scene.active_camera])->left -= (float)deltay/200.0;
            }
            else if (current_manipulation == manipulate::height && scene.cameras[scene.active_camera]->type == "ortho")
            {
                ((orthohdl*)scene.cameras[scene.active_camera])->top += (float)deltay/200.0;
                ((orthohdl*)scene.cameras[scene.active_camera])->bottom -= (float)deltay/200.0;
            }
            else if (current_manipulation == manipulate::height && scene.cameras[scene.active_camera]->type == "frustum")
            {
                ((frustumhdl*)scene.cameras[scene.active_camera])->top += (float)deltay/200.0;
                ((frustumhdl*)scene.cameras[scene.active_camera])->bottom -= (float)deltay/200.0;
            }
            else if (current_manipulation == manipulate::near && scene.cameras[scene.active_camera]->type == "ortho")
                ((orthohdl*)scene.cameras[scene.active_camera])->near += (float)deltay/100.0;
            else if (current_manipulation == manipulate::near && scene.cameras[scene.active_camera]->type == "frustum")
                ((frustumhdl*)scene.cameras[scene.active_camera])->near += (float)deltay/100.0;
            else if (current_manipulation == manipulate::near && scene.cameras[scene.active_camera]->type == "perspective")
                ((perspectivehdl*)scene.cameras[scene.active_camera])->near += (float)deltay/100.0;
            else if (current_manipulation == manipulate::far && scene.cameras[scene.active_camera]->type == "ortho")
                ((orthohdl*)scene.cameras[scene.active_camera])->far += (float)deltay/100.0;
            else if (current_manipulation == manipulate::far && scene.cameras[scene.active_camera]->type == "frustum")
                ((frustumhdl*)scene.cameras[scene.active_camera])->far += (float)deltay/100.0;
            else if (current_manipulation == manipulate::far && scene.cameras[scene.active_camera]->type == "perspective")
                ((perspectivehdl*)scene.cameras[scene.active_camera])->far += (float)deltay/100.0;
            
            if (current_manipulation == manipulate::fovy ||
                current_manipulation == manipulate::aspect ||
                current_manipulation == manipulate::width ||
                current_manipulation == manipulate::height ||
                current_manipulation == manipulate::near ||
                current_manipulation == manipulate::far)
                scene.cameras[scene.active_camera]->project(&canvas);
        }
        
        glutPostRedisplay();
    }
    else if (!bound)
    {
        menu = false;
        pmotionfunc(x, y);
    }
}

void keydownfunc(unsigned char key, int x, int y)
{
	keys[key] = true;
    
    switch (key) {
        case 27: // Escape Key Pressed
            glutDestroyWindow(window_id);
            exit(0);
            break;
        case 'm':
            if (bound) {
                bound = false;
                glutSetCursor(GLUT_CURSOR_INHERIT);
//                glutAttachMenu(GLUT_RIGHT_BUTTON);
            } else {
                bound = true;
                glutSetCursor(GLUT_CURSOR_NONE);
//                glutDetachMenu(GLUT_RIGHT_BUTTON);
                mousex = x;
                mousey = y;
            }
            break;
        case GLUT_KEY_UP: // Up arrow
            if (scene.active_camera_valid()) {
                vec3f forward = ror3(vec3f(0.,0.,-0.1), scene.cameras[scene.active_camera]->orientation);
                scene.cameras[scene.active_camera]->position += forward;
            }
            break;
        case GLUT_KEY_DOWN: // Down arrow
            if (scene.active_camera_valid()) {
                vec3f forward = ror3(vec3f(0.,0.,0.1), scene.cameras[scene.active_camera]->orientation);
                scene.cameras[scene.active_camera]->position += forward;
            }
            break;
        default:
            break;
    }
    
}

void keyupfunc(unsigned char key, int x, int y)
{
	keys[key] = false;
}

void idlefunc()
{
    bool change = false;
    if (scene.active_camera_valid() && scene.cameras[scene.active_camera]->focus == NULL)
    {
        if (keys['w'])
        {
            scene.cameras[scene.active_camera]->position += -0.25f*ror3(vec3f(0.0, 0.0, 1.0), scene.cameras[scene.active_camera]->orientation);
            change = true;
        }
        if (keys['s'])
        {
            scene.cameras[scene.active_camera]->position += 0.25f*ror3(vec3f(0.0, 0.0, 1.0), scene.cameras[scene.active_camera]->orientation);
            change = true;
        }
        if (keys['a'])
        {
            scene.cameras[scene.active_camera]->position += -0.25f*ror3(vec3f(1.0, 0.0, 0.0), scene.cameras[scene.active_camera]->orientation);
            change = true;
        }
        if (keys['d'])
        {
            scene.cameras[scene.active_camera]->position += 0.25f*ror3(vec3f(1.0, 0.0, 0.0), scene.cameras[scene.active_camera]->orientation);
            change = true;
        }
        if (keys['q'])
        {
            scene.cameras[scene.active_camera]->position += -0.25f*ror3(vec3f(0.0, 1.0, 0.0), scene.cameras[scene.active_camera]->orientation);
            change = true;
        }
        if (keys['e'])
        {
            scene.cameras[scene.active_camera]->position += 0.25f*ror3(vec3f(0.0, 1.0, 0.0), scene.cameras[scene.active_camera]->orientation);
            change = true;
        }
    }
    else if (scene.active_camera_valid() && scene.cameras[scene.active_camera]->focus != NULL)
    {
        if (keys['w'])
        {
            vec3f dir_of_movement = scene.cameras[scene.active_camera]->focus->position - scene.cameras[scene.active_camera]->position;
            dir_of_movement /= 10.;
            scene.cameras[scene.active_camera]->position += dir_of_movement;
            change = true;
        }
        if (keys['s'])
        {
            vec3f dir_of_movement = scene.cameras[scene.active_camera]->focus->position - scene.cameras[scene.active_camera]->position;
            dir_of_movement /= 10.;
            scene.cameras[scene.active_camera]->position -= dir_of_movement;
            change = true;
        }
        if (keys['a'])
        {
            vec3f up = ror3(vec3f(0.,1.,0.), scene.cameras[scene.active_camera]->orientation);
            vec3f dir = scene.cameras[scene.active_camera]->focus->position - scene.cameras[scene.active_camera]->position;
            vec3f dir_of_movement = cross(up, dir);
            dir_of_movement = norm(dir_of_movement);
            dir_of_movement /= 10.;
            scene.cameras[scene.active_camera]->position -= dir_of_movement;
            change = true;
        }
        if (keys['d'])
        {
            vec3f up = ror3(vec3f(0.,1.,0.), scene.cameras[scene.active_camera]->orientation);
            vec3f dir = scene.cameras[scene.active_camera]->focus->position - scene.cameras[scene.active_camera]->position;
            vec3f dir_of_movement = cross(up, dir);
            dir_of_movement = norm(dir_of_movement);
            dir_of_movement /= 10.;
            scene.cameras[scene.active_camera]->position += dir_of_movement;
            change = true;
        }
        if (keys['q'])
        {
            // TODO
            change = true;
        }
        if (keys['e'])
        {
            // TODO
            change = true;
        }
    }
    
    if (change)
        glutPostRedisplay();
}

void specialfunc(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP: // Up arrow
            if (scene.active_camera_valid()) {
                if (scene.cameras[scene.active_camera]->focus){
                    vec3f dir_of_movement = scene.cameras[scene.active_camera]->focus->position - scene.cameras[scene.active_camera]->position;
                    dir_of_movement /= 10.;
                    scene.cameras[scene.active_camera]->position += dir_of_movement;
                } else {
                    vec3f forward = ror3(vec3f(0.,0.,-0.1), scene.cameras[scene.active_camera]->orientation);
                    scene.cameras[scene.active_camera]->position += forward;
                }
            }
            break;
        case GLUT_KEY_DOWN: // Down arrow
            if (scene.active_camera_valid()) {
                if (scene.cameras[scene.active_camera]->focus){
                    vec3f dir_of_movement = scene.cameras[scene.active_camera]->focus->position - scene.cameras[scene.active_camera]->position;
                    dir_of_movement /= 10.;
                    scene.cameras[scene.active_camera]->position -= dir_of_movement;
                }
                else {
                    vec3f backward = ror3(vec3f(0.,0.,0.1), scene.cameras[scene.active_camera]->orientation);
                    scene.cameras[scene.active_camera]->position += backward;
                }
            }
            break;
        case GLUT_KEY_RIGHT: // Right arrow
            if (scene.active_camera_valid()) {
                if (scene.cameras[scene.active_camera]->focus){
                    vec3f up = ror3(vec3f(0.,1.,0.), scene.cameras[scene.active_camera]->orientation);
                    vec3f dir = scene.cameras[scene.active_camera]->focus->position - scene.cameras[scene.active_camera]->position;
                    vec3f dir_of_movement = cross(up, dir);
                    dir_of_movement = norm(dir_of_movement);
                    dir_of_movement /= 10.;
                    scene.cameras[scene.active_camera]->position += dir_of_movement;
                }
                else{
                    vec3f right = ror3(vec3f(0.1,0.,0.), scene.cameras[scene.active_camera]->orientation);
                    scene.cameras[scene.active_camera]->position += right;
                }
            }
            break;
        case GLUT_KEY_LEFT: // Left arrow
            if (scene.active_camera_valid()) {
                if (scene.cameras[scene.active_camera]->focus){
                    vec3f up = ror3(vec3f(0.,1.,0.), scene.cameras[scene.active_camera]->orientation);
                    vec3f dir = scene.cameras[scene.active_camera]->focus->position - scene.cameras[scene.active_camera]->position;
                    vec3f dir_of_movement = cross(up, dir);
                    dir_of_movement = norm(dir_of_movement);
                    dir_of_movement /= 10.;
                    scene.cameras[scene.active_camera]->position -= dir_of_movement;
                }
                else{
                    vec3f left = ror3(vec3f(-0.1,0.,0.), scene.cameras[scene.active_camera]->orientation);
                    scene.cameras[scene.active_camera]->position += left;
                }
            }
            break;
        default:
            break;
    }
    glutPostRedisplay();
    
}

void menustatusfunc(int status, int x, int y)
{
	if (status == GLUT_MENU_IN_USE)
		menu = true;
}

void toggle_cameras (int val){
    if (val)
        scene.render_cameras = !scene.render_cameras;
    glutPostRedisplay();
}

void toggle_lights (int val){
    if (val){
        scene.render_lights = !scene.render_lights;
    }
    glutPostRedisplay();
}

void focus_object (int val) {
    if (val) {
        if (scene.active_camera_valid()){
            if (!scene.cameras[scene.active_camera]->focus){
                int obj_ind = current_objects->get_int_val();
                scene.cameras[scene.active_camera]->focus = scene.objects[obj_ind];
            }
            else {
                scene.cameras[scene.active_camera]->focus = NULL;
                
            }
            glutPostRedisplay();
        }
    }
}

void create_object (int val){
    int index = -1;
    switch (val){
        case Object::Box : {
            
            // Create dynamic box object
            boxhdl *box = new boxhdl(1.0, 1.0, 1.0);
            scene.objects.push_back(box);
            index = (int)scene.objects.size()-1;
            current_objects->add_item(index, "Box");
            break;
        }
        case Object::Cylinder : {
            
            // Create dynamic sphere object
            cylinderhdl *cylinder = new cylinderhdl(1.0, 4.0, 10.0);
            scene.objects.push_back(cylinder);
            index = (int)scene.objects.size()-1;
            current_objects->add_item(index, "Cylinder");
            break;
        }
        case Object::Sphere : {
            
            // Create dynamic sphere object
            spherehdl *sphere = new spherehdl(1.0, 16.0, 16.0);
            scene.objects.push_back(sphere);
            index = (int)scene.objects.size()-1;
            current_objects->add_item(index, "Sphere");
            break;
        }
        case Object::Pyramid : {
            
            // Create dynamic sphere object
            pyramidhdl *pyramid = new pyramidhdl(1.0, 4.0, 12.0);
            scene.objects.push_back(pyramid);
            index = (int)scene.objects.size()-1;
            current_objects->add_item(index, "Pyramid");
            break;
        }
        case Object::Model : {
            
            modelhdl *model = new modelhdl("res/models/"+models[current_model]);
            scene.objects.push_back(model);
            index = (int)scene.objects.size()-1;
            current_objects->add_item(index, "Model");
            break;
        }
            
        default:
            
            break;
    }
    current_objects->set_int_val(index);
    glutPostRedisplay();
}

void object_menu(int num)
{
    if (num == 0)
    {
        if (scene.active_object >= 0 && scene.active_object < scene.objects.size())
        {
            if (scene.objects[scene.active_object] != NULL)
            {
                /* TODO Assignment 2: clean up the lights as well when the associated object
                 * is deleted.
                 */
                
                for (int i = 0; i < scene.cameras.size(); )
                {
                    if (scene.cameras[i] != NULL && scene.cameras[i]->model == scene.objects[scene.active_object])
                    {
                        delete scene.cameras[i];
                        scene.cameras.erase(scene.cameras.begin() + i);
                    }
                    else
                        i++;
                }
                delete scene.objects[scene.active_object];
            }
            scene.objects.erase(scene.objects.begin() + scene.active_object);
            glutPostRedisplay();
        }
    }
    else if (num == 4)
    {
        scene.active_camera = -1;
        for (int i = 0; i < scene.cameras.size(); i++)
            if (scene.cameras[i] != NULL && scene.active_object_valid() && scene.cameras[i]->model == scene.objects[scene.active_object])
                scene.active_camera = i;
        
        if (scene.active_camera_valid())
            scene.cameras[scene.active_camera]->project(&canvas);
        
        glutPostRedisplay();
    }
    else if (num == 1)
        current_manipulation = manipulate::translate;
    else if (num == 2)
        current_manipulation = manipulate::rotate;
    else if (num == 3)
        current_manipulation = manipulate::scale;
    else if (num == 5 && scene.active_object_valid() && scene.active_camera_valid())
    {
        scene.cameras[scene.active_camera]->focus = scene.objects[scene.active_object];
        scene.cameras[scene.active_camera]->radius = dist(scene.objects[scene.active_object]->position, scene.cameras[scene.active_camera]->position);
    }
    
}

// This is so that there are only four variables displayed
// TODO: rename variables
void handle_camera_manip (int val){
    switch (val) {
        case manipulate::fovy :
            height = fovy;
            break;
        case manipulate::aspect :
            width = aspect;
            break;
        case manipulate::width :
            
            break;
        case manipulate::height :
            
            break;
        case manipulate::near:
            
            break;
        case manipulate::far:
            
            break;
        default:
            break;
    }
    
    if (scene.active_camera_valid()){
        
        string type = scene.cameras[scene.active_camera]->type;
        if (type == "ortho") {
            orthohdl *cam = (orthohdl*)scene.cameras[scene.active_camera];
            cam->near = near;
            cam->far = far;
            cam->left = -width/2.;
            cam->right = width/2.;
            cam->top = height/2.;
            cam->bottom = -height/2.;
        } else if (	type == "frustum") {
            frustumhdl *cam = (frustumhdl*)scene.cameras[scene.active_camera];
            cam->near = near;
            cam->far = far;
            cam->left = -width/2.;
            cam->right = width/2.;
            cam->top = height/2.;
            cam->bottom = -height/2.;
            
        } else if (type == "perspective"){
            perspectivehdl *cam = (perspectivehdl*)scene.cameras[scene.active_camera];
            cam->near = near;
            cam->far = far;
            cam->fovy = fovy;
            cam->aspect = aspect;
        }
    }
}
void fb_callback (int value){
    //const char* file = file_browser->get_file();
    if (value == 0) {
        printf("Get file: %s\n\n",file_browser->get_file());
    }
    cout << "testing" << endl;
}

void create_camera (int val){
    int index = -1;
    
    if (scene.active_camera_valid()){
        scene.cameras[scene.active_camera]->focus = NULL;
        focus_checkbox->set_int_val(0);
    }

    switch (val){
        case Camera::Ortho: {
            camerahdl *camera = new orthohdl;
            scene.cameras.push_back(camera);
            index = (int)scene.cameras.size() - 1;
            scene.active_camera = index;
            current_cameras->add_item(index, "Ortho");
            break;
        }
        case Camera::Frustum: {
            camerahdl *camera = new frustumhdl;
            scene.cameras.push_back(camera);
            index = (int)scene.cameras.size() - 1;
            scene.active_camera = index;
            current_cameras->add_item(index, "Frustum");
            break;
        }
        case Camera::Perspective: {
            camerahdl *camera = new perspectivehdl;
            scene.cameras.push_back(camera);
            index = (int)scene.cameras.size() - 1;
            scene.active_camera = index;
            current_cameras->add_item(index, "Perspective");

            break;
        }
        default:
            break;
    }
    current_cameras->set_int_val(index);
    set_camera_info(index);
    glutPostRedisplay();
}

void create_light (int val){
    int index = -1;
    
    switch (val){
        case Light::Ambient: {
            lighthdl *light = new ambienthdl;
            scene.lights.push_back(light);
            index = (int)scene.lights.size() - 1;
            current_lights->add_item(index, "Ambient");
            break;
        }
        case Light::Point: {
            lighthdl *light = new pointhdl;
            scene.lights.push_back(light);
            index = (int)scene.lights.size() - 1;
            current_lights->add_item(index, "Point");
            break;
        }
        case Light::Spot: {
            lighthdl *light = new spothdl;
            scene.lights.push_back(light);
            index = (int)scene.lights.size() - 1;
            current_lights->add_item(index, "Spot");
            break;
        }
        case Light::Directional: {
            lighthdl *light = new directionalhdl;
            scene.lights.push_back(light);
            index = (int)scene.lights.size() - 1;
            current_lights->add_item(index, "Directional");
            break;
        }
        default: break;
    }
    current_lights->set_int_val(index);
    set_light_info(index);
    glutPostRedisplay();
}

void handle_polygon (int val){
    switch (val){
        case canvashdl::point:
            current_polygon = canvashdl::point;
            break;
        case canvashdl::line:
            current_polygon = canvashdl::line;
            break;
        case canvashdl::fill:
            current_polygon = canvashdl::fill;
            break;
        default:
            break;
    }
    canvas.polygon_mode = (canvashdl::Polygon)current_polygon;
    glutPostRedisplay();
}

void handle_shading (int val){
    switch (val){
        case canvashdl::Shading::none:
            current_shading = canvashdl::Shading::none;
            break;
        case canvashdl::Shading::flat:
            current_shading = canvashdl::Shading::flat;
            break;
        case canvashdl::Shading::gouraud:
            current_shading = canvashdl::Shading::gouraud;
            break;
        case canvashdl::Shading::phong:
            current_shading = canvashdl::Shading::phong;
            break;
        default:
            break;
    }
    canvas.shade_model = (canvashdl::Shading)current_shading;
    glutPostRedisplay();
}

void handle_culling (int val){
    switch (val){
        case canvashdl::Culling::disable:
            current_culling = canvashdl::Culling::disable;
            break;
        case canvashdl::Culling::backface:
            current_culling = canvashdl::Culling::backface;
            break;
        case canvashdl::Culling::frontface:
            current_culling = canvashdl::Culling::frontface;
            break;
        default:
            break;
    }
    canvas.culling_mode = (canvashdl::Culling)current_culling;
    glutPostRedisplay();
}

void handle_normal (int val){
    switch (val){
        case scenehdl::Normal::none:
            current_normal = scenehdl::Normal::none;
            break;
        case scenehdl::Normal::face:
            current_normal = scenehdl::Normal::face;
            break;
        case scenehdl::Normal::vertex:
            current_normal = scenehdl::Normal::vertex;
            break;
        default:
            break;
    }
    scene.render_normals = (scenehdl::Normal)current_normal;
    glutPostRedisplay();
}

void handle_material (int val) {
    string name = "material";
    materialhdl *material;
    switch (val) {
        case Material::Uniform:
            current_material = Material::Uniform;
            material = new uniformhdl();
            break;
        case Material::NonUniform:
            current_material = Material::NonUniform;
            material = new nonuniformhdl();
            break;
        default:
            material = new uniformhdl();
            break;
    }
    int obj_ind = current_objects->get_int_val();
    
    if (scene.objects[obj_ind] != NULL && scene.objects.size() > obj_ind) {
        scene.objects[obj_ind]->material.clear();
        scene.objects[obj_ind]->material[name] = material;
        for (vector<rigidhdl>::iterator iter = scene.objects[obj_ind]->rigid.begin(); iter != scene.objects[obj_ind]->rigid.end(); ++iter) {
            iter->material = name;
        }
        
    }
    glutPostRedisplay();
}

void handle_menu(int val)
{
    if (val == 5){
        glutDestroyWindow(window_id);
        exit(0);
    }
}

vec3f get_color()
{
    vec3f color;
    switch (current_light_color){
        case LightColor::White:
            color = white;
            break;
        case LightColor::Red:
            color = red;
            break;
        case LightColor::Green:
            color = green;
            break;
        case LightColor::Blue:
            color = blue;
            break;
        case LightColor::Black:
            color = black;
            break;
        case LightColor::Brown :
            color = brown;
            break;
        case LightColor::Indigo:
            color = indigo;
            break;
        case LightColor::Orange:
            color = orange;
            break;
        case LightColor::Violet:
            color = violet;
            break;
        case LightColor::Yellow:
            color = yellow;
            break;
        default: break;
    }
    return color;
}

void handle_update(int val)
{
    if (val == 1) {
        
        // Change the current light color and attenuation, if applicable
        int light_ind = current_lights->get_int_val();
        if (scene.lights.size() > light_ind && scene.lights[light_ind] != NULL) {
            
            string type = scene.lights[light_ind]->type;
            vec3f color = get_color();
            
            if (type.compare("ambient") == 0){
                scene.lights[light_ind]->ambient = color;
            }
            else if (type.compare("directional") == 0){
                scene.lights[light_ind]->ambient = color*0.1f;
                scene.lights[light_ind]->diffuse = color*0.5f;
                scene.lights[light_ind]->specular = color;
            }
            else if (type.compare("point") == 0){
                vec3f attenuation_vector = {attenuation0,attenuation1,attenuation2};
                ((pointhdl*)scene.lights[light_ind])->attenuation = attenuation_vector;
                scene.lights[light_ind]->ambient = color*0.1f;
                scene.lights[light_ind]->diffuse = color*0.5f;
                scene.lights[light_ind]->specular = color;
                
            }
            else if (type.compare("spot") == 0){
                vec3f attenuation_vector = {attenuation0,attenuation1,attenuation2};
                ((spothdl*)scene.lights[light_ind])->attenuation = attenuation_vector;
                scene.lights[light_ind]->ambient = color*0.1f;
                scene.lights[light_ind]->diffuse = color*0.5f;
                scene.lights[light_ind]->specular = color;
            }
        }
    }

    glutPostRedisplay();
}

void set_light_info(int obj_ind){
    attenuation_text_x->set_float_val(0.0);
    attenuation_text_y->set_float_val(0.0);
    attenuation_text_z->set_float_val(0.0);
    
    if (scene.lights[obj_ind]) {
        string type = scene.lights[obj_ind]->type;
        if (type == "spot") {
            spothdl *light = (spothdl*)scene.lights[obj_ind];
            attenuation_text_x->set_float_val(light->attenuation[0]);
            attenuation_text_y->set_float_val(light->attenuation[1]);
            attenuation_text_z->set_float_val(light->attenuation[2]);
            
        } else if (	type == "point") {
            pointhdl *light = (pointhdl*)scene.lights[obj_ind];
            attenuation_text_x->set_float_val(light->attenuation[0]);
            attenuation_text_y->set_float_val(light->attenuation[1]);
            attenuation_text_z->set_float_val(light->attenuation[2]);
        }
    }
}

void handle_delete(int val)
{
    if (val == 0) {
        int obj_ind = current_objects->get_int_val();
        if (obj_ind < scene.objects.size() && obj_ind >= 0) {
            scene.objects[obj_ind] = NULL; // TODO: list will keep getting bigger if you only set pointer to null
            current_objects->delete_item(obj_ind);
            current_objects->set_int_val(-1);
        }
    } else if (val == 1) {
        int obj_ind = current_cameras->get_int_val();
        if (obj_ind < scene.cameras.size() && obj_ind >= 0) {
            scene.cameras[obj_ind] = NULL; // TODO: list will keep getting bigger if you only set pointer to null
            current_cameras->delete_item(obj_ind);
            current_cameras->set_int_val(-1);
        }
    } else if (val == 2) {
        int light_ind = current_lights->get_int_val();
        if (light_ind < scene.lights.size() && light_ind >= 0) {
            scene.lights[light_ind] = NULL;
            current_lights->delete_item(light_ind);
            current_lights->set_int_val(-1);
        }
    }
    glutPostRedisplay();
}

void create_menu()
{
	/* TODO Assignment 1: Implement a menu that allows the following operations:
	 * - create a box, cylinder, sphere, pyramid, or load a model
	 * - change the fovy, aspect, width, height, near or far values of the active camera
	 * - enable/disable drawing of cameras
	 * - create an orthographic, frustum, or perspective camera
	 * - set the polygon mode to point or line
	 * - enable culling for front or back face or disable culling
	 * - enable rendering of vertex or face normals, or disabe both
	 * - Set an object or camera as the focus of the active camera (using canvashdl::look_at)
	 * - Unset the focus of the active camera
	 * - Translate, rotate, or scale an object or camera
	 * - delete an object or camera
	 * - Set the active camera
	 * - quit
	 */
    
    // Add object menu items
    int object_menu_id = glutCreateMenu(create_object);
    glutAddMenuEntry("Box",         Object::Box);
    glutAddMenuEntry("Cylinder",    Object::Cylinder);
    glutAddMenuEntry("Sphere",      Object::Sphere);
    glutAddMenuEntry("Pyramid",     Object::Pyramid);
    glutAddMenuEntry("Model",       Object::Model);
    
    // Add camera menu items
    camera_menu_id = glutCreateMenu(create_camera);
//    glutAddMenuEntry("Fovy", 0);
//    glutAddMenuEntry("Aspect", 1);
//    glutAddMenuEntry("Width", 2);
//    glutAddMenuEntry("Height", 3);
//    glutAddMenuEntry("Near", 4);
//    glutAddMenuEntry("Far", 5);
//    glutAddMenuEntry("Toggle Draw", 6);
//    glutAddMenuEntry("Clear Focus", 7);
    glutAddMenuEntry("Ortho",       Camera::Ortho);
    glutAddMenuEntry("Frustum",     Camera::Frustum);
    glutAddMenuEntry("Perspective", Camera::Perspective);

    // Add polygon menu items
    int polygon_menu_id = glutCreateMenu(handle_polygon);
    glutAddMenuEntry("Point",   canvashdl::Polygon::point);
    glutAddMenuEntry("Line",    canvashdl::Polygon::line);
    glutAddMenuEntry("Fill",    canvashdl::Polygon::fill);

    // Add culling menu items
    int culling_menu_id = glutCreateMenu(handle_culling);
    glutAddMenuEntry("None",    canvashdl::Culling::disable);
    glutAddMenuEntry("Back",    canvashdl::Culling::backface);
    glutAddMenuEntry("Front",   canvashdl::Culling::frontface);
    
    // Add normals menu items
    int normals_menu_id = glutCreateMenu(handle_normal);
    glutAddMenuEntry("None",    scenehdl::Normal::none);
    glutAddMenuEntry("Face",    scenehdl::Normal::face);
    glutAddMenuEntry("Vertex",  scenehdl::Normal::vertex);
    
    // Add submenus to menu
    int menu_id = glutCreateMenu(handle_menu);
    glutAddSubMenu("Objects", object_menu_id);
    glutAddSubMenu("Cameras", camera_menu_id);
    glutAddSubMenu("Polygon", polygon_menu_id);
    glutAddSubMenu("Culling", culling_menu_id);
    glutAddSubMenu("Normals", normals_menu_id);
    glutAddMenuEntry("Quit", 5);


	/* TODO Assignment 2: Add menus for handling the lights. You should
	 * be able to
	 * - enable/disable the drawing of the lights
	 * - create directional, point, or spot lights sources
	 * - change the emissive, ambient, diffuse, and specular colors, and the attenuation of the lights
	 * - rotate and translate the lights just like you would an object and have it affect the lighting of the scene
	 * - set the polygon mode to fill
	 * - set the shading mode to none, flat, gouraud, or phong
	 */

	glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMenuStatusFunc(menustatusfunc);
}

void set_camera_info(int obj_ind){
    string type = scene.cameras[obj_ind]->type;
    if (type == "ortho") {
        orthohdl *cam = (orthohdl*)scene.cameras[obj_ind];
        near_text->set_float_val(cam->near);
        far_text->set_float_val(cam->far);
        aspect_text->set_name("width");
        fovy_text->set_name("height");
        aspect_text->set_float_val(cam->right - cam->left);
        fovy_text->set_float_val(cam->top - cam->bottom);
        
    } else if (	type == "frustum") {
        frustumhdl *cam = (frustumhdl*)scene.cameras[obj_ind];
        near_text->set_float_val(cam->near);
        far_text->set_float_val(cam->far);
        aspect_text->set_name("width");
        fovy_text->set_name("height");
        aspect_text->set_float_val(cam->right - cam->left);
        fovy_text->set_float_val(cam->top - cam->bottom);
        
    } else if (type == "perspective"){
        perspectivehdl *cam = (perspectivehdl*)scene.cameras[obj_ind];
        near_text->set_float_val(cam->near);
        far_text->set_float_val(cam->far);
        aspect_text->set_name("aspect");
        fovy_text->set_name("fovy");
        aspect_text->set_float_val(cam->aspect);
        fovy_text->set_float_val(cam->fovy);

    }
}

void selected_object(int id) {
    switch (id) {
        case 1:{ //Light
            int obj_ind = current_lights->get_int_val();
            if (obj_ind < scene.lights.size() && obj_ind >= 0) {
                set_light_info(obj_ind);
            }

            break;
        }
        case 2:{ //Camera
            if (scene.active_camera_valid()){
                scene.cameras[scene.active_camera]->focus = NULL;
                focus_checkbox->set_int_val(0);
            }

            int obj_ind = current_cameras->get_int_val();
            if (obj_ind < scene.cameras.size() && obj_ind >= 0) {
                scene.active_camera = obj_ind;
                set_camera_info(obj_ind);
            }
            break;
        }
        default:
            break;
    }
    glutPostRedisplay();
}



/* TODO Assignment 1: Implement a menu that allows the following operations:
 * - change the fovy, aspect, width, height, near or far values of the active camera
 * - Set an object or camera as the focus of the active camera (using canvashdl::look_at)
 * - Unset the focus of the active camera
 * - Translate, rotate, or scale an object or camera
 */
void setup_glui() {
    glui = GLUI_Master.create_glui("Controls",0,800,0);
    GLUI_Panel *scene_panel = glui->add_panel("Current Scene");
    
    object_panel = glui->add_panel_to_panel(scene_panel, "Object");
    current_objects = glui->add_listbox_to_panel(object_panel, "", NULL, 1, selected_object);
    current_objects->add_item(-1, "");
    focus_checkbox = glui->add_checkbox_to_panel(object_panel, "Focus on Object", NULL, 1, focus_object);
    list_material = glui->add_listbox_to_panel(object_panel, "Material", &current_material, -1, handle_material);
    list_material->add_item(Material::Uniform, "Uniform");
    list_material->add_item(Material::NonUniform, "Non-uniform");
//    GLUI_Panel *obj_pos_panel = glui->add_panel_to_panel(object_panel, "Position");
//    glui->add_edittext_to_panel(obj_pos_panel, "x:");
//    glui->add_edittext_to_panel(obj_pos_panel, "y:");
//    glui->add_edittext_to_panel(obj_pos_panel, "z:");
//    GLUI_Panel *obj_ori_panel = glui->add_panel_to_panel(object_panel, "Orientation");
//    glui->add_edittext_to_panel(obj_ori_panel, "x:");
//    glui->add_edittext_to_panel(obj_ori_panel, "y:");
//    glui->add_edittext_to_panel(obj_ori_panel, "z:");
    glui->add_button_to_panel(object_panel, "Delete", 0, handle_delete);

    glui->add_separator_to_panel(scene_panel);
    
    light_panel = glui->add_panel_to_panel(scene_panel, "Light");
    current_lights = glui->add_listbox_to_panel(light_panel, "", NULL, 1, selected_object);
    current_lights->add_item(-1, "");
    glui->add_checkbox_to_panel(light_panel, "Render Lights", NULL, 1, toggle_lights);
    light_prop_panel = glui->add_panel_to_panel(light_panel, "Properties");
    list_light_colors = glui->add_listbox_to_panel(light_prop_panel, "Color", &current_light_color, 1, handle_update);
    list_light_colors->add_item(LightColor::White, "White");
    list_light_colors->add_item(LightColor::Red, "Red");
    list_light_colors->add_item(LightColor::Green, "Green");
    list_light_colors->add_item(LightColor::Blue, "Blue");
    list_light_colors->add_item(LightColor::Orange, "Orange");
    list_light_colors->add_item(LightColor::Violet, "Violet");
    list_light_colors->add_item(LightColor::Indigo, "Indigo");
    list_light_colors->add_item(LightColor::Brown, "Brown");
    list_light_colors->add_item(LightColor::Black, "Black");
    list_light_colors->add_item(LightColor::Yellow, "Yellow");
    attenuation_text_x = glui->add_edittext_to_panel(light_prop_panel, "Attenuation (const):", GLUI_EDITTEXT_FLOAT, &attenuation0, 1, handle_update);
    attenuation_text_y = glui->add_edittext_to_panel(light_prop_panel, "Attenuation (power1):", GLUI_EDITTEXT_FLOAT, &attenuation1, 1, handle_update);
    attenuation_text_z = glui->add_edittext_to_panel(light_prop_panel, "Attenuation (power2):", GLUI_EDITTEXT_FLOAT, &attenuation2, 1, handle_update);
//    glui->add_button_to_panel(light_prop_panel, "Update", 1, handle_update);
    glui->add_button_to_panel(light_panel, "Delete", 2, handle_delete);
    
    glui->add_separator_to_panel(scene_panel);
    
    camera_panel = glui->add_panel_to_panel(scene_panel, "Camera");
    current_cameras = glui->add_listbox_to_panel(camera_panel, "", NULL, 2, selected_object);
    current_cameras->add_item(-1, "");
    glui->add_checkbox_to_panel(camera_panel, "Draw Cameras", NULL, 1, toggle_cameras);
    cam_prop_panel = glui->add_panel_to_panel(camera_panel, "Properties");
    fovy_text = glui->add_edittext_to_panel(cam_prop_panel, "fovy:", GLUI_EDITTEXT_FLOAT, &fovy, manipulate::fovy, handle_camera_manip);
    aspect_text = glui->add_edittext_to_panel(cam_prop_panel, "aspect:", GLUI_EDITTEXT_FLOAT, &aspect, manipulate::aspect, handle_camera_manip);
    near_text = glui->add_edittext_to_panel(cam_prop_panel, "near:", GLUI_EDITTEXT_FLOAT, &near, manipulate::fovy, handle_camera_manip);
    far_text = glui->add_edittext_to_panel(cam_prop_panel, "far:", GLUI_EDITTEXT_FLOAT, &far, manipulate::fovy, handle_camera_manip);
    glui->add_button_to_panel(camera_panel, "Delete", 1, handle_delete);
    
    glui->add_separator_to_panel(scene_panel);

    glui->add_column(true);
    GLUI_Panel *options_panel = glui->add_panel("Scene Options");
    list_polygon = glui->add_listbox_to_panel(options_panel, "Polygon", &current_polygon, -1, handle_polygon);
    list_polygon->add_item(canvashdl::Polygon::line,    "Line");
    list_polygon->add_item(canvashdl::Polygon::point,   "Point");
    list_polygon->add_item(canvashdl::Polygon::fill,    "Fill");
    list_normal = glui->add_listbox_to_panel(options_panel, "Normal", &current_normal, -1, handle_normal);
    list_normal->add_item(scenehdl::Normal::none,       "None");
    list_normal->add_item(scenehdl::Normal::face,       "Face");
    list_normal->add_item(scenehdl::Normal::vertex,     "Vertex");
    list_culling = glui->add_listbox_to_panel(options_panel, "Culling", &current_culling, -1, handle_culling);
    list_culling->add_item(canvashdl::Culling::backface,    "Back");
    list_culling->add_item(canvashdl::Culling::frontface,   "Front");
    list_culling->add_item(canvashdl::Culling::disable,     "None");
    list_manipulation = glui->add_listbox_to_panel(options_panel, "Manipulation", &current_manipulation);
    list_manipulation->add_item(manipulate::translate,"Translate");
    list_manipulation->add_item(manipulate::rotate,   "Rotate");
    list_manipulation->add_item(manipulate::scale,    "Scale");
    list_shading = glui->add_listbox_to_panel(options_panel, "Shading", &current_shading, -1, handle_shading);
    list_shading->add_item(canvashdl::Shading::none,"None");
    list_shading->add_item(canvashdl::Shading::flat, "Flat");
    list_shading->add_item(canvashdl::Shading::gouraud, "Gouraud");
    list_shading->add_item(canvashdl::Shading::phong, "Phong");
    
    glui->add_separator_to_panel(options_panel);

    GLUI_Panel *obj_panel = glui->add_panel("Create Object");
    glui->add_button_to_panel(obj_panel,    "Box",         Object::Box,       create_object);
    glui->add_button_to_panel(obj_panel,    "Cylinder",    Object::Cylinder,  create_object);
    glui->add_button_to_panel(obj_panel,    "Sphere",      Object::Sphere,    create_object);
    glui->add_button_to_panel(obj_panel,    "Pyramid",     Object::Pyramid,   create_object);
    glui->add_button_to_panel(obj_panel, "Model", Object::Model, create_object);
    list_model = glui->add_listbox_to_panel(obj_panel, "Choose Model", &current_model);
    list_model->add_item(0, "Banana");
    list_model->add_item(1, "Beethoven");
    list_model->add_item(2, "Bishop");
    list_model->add_item(3, "Blob");
    list_model->add_item(4, "Bunny");
    list_model->add_item(5, "Chain");
    list_model->add_item(6, "Cow");
    list_model->add_item(7, "CowUV");
    list_model->add_item(8, "Demo");
    list_model->add_item(9, "Dolphin");
    list_model->add_item(10, "Feline");
    list_model->add_item(11, "Helmet");
    list_model->add_item(12, "Pawn");
    list_model->add_item(13, "Shirt");
    list_model->add_item(14, "Statue");
    list_model->add_item(15, "Teapot");
    
    
    GLUI_Panel *cam_panel = glui->add_panel("Create Camera");
    glui->add_button_to_panel(cam_panel,    "Ortho",       Camera::Ortho,         create_camera);
    glui->add_button_to_panel(cam_panel,    "Frustum",     Camera::Frustum,       create_camera);
    glui->add_button_to_panel(cam_panel,    "Perspective", Camera::Perspective,   create_camera);
    glui->add_separator();
    
    GLUI_Panel *light_panel = glui->add_panel("Create Light");
    glui->add_button_to_panel(light_panel,    "Ambient",       Light::Ambient,         create_light);
    glui->add_button_to_panel(light_panel,    "Point",     Light::Point,       create_light);
    glui->add_button_to_panel(light_panel,    "Spot", Light::Spot,   create_light);
    glui->add_button_to_panel(light_panel,    "Directional", Light::Directional,   create_light);
    
    glui->add_separator();
    glui->add_button("Quit", 0, exit);
    
    glui->set_main_gfx_window(window_id);
    GLUI_Master.set_glutIdleFunc(idlefunc);
    GLUI_Master.set_glutMouseFunc(mousefunc);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	int display_mode = GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE;
#ifdef OSX_CORE3
	display_mode |= GLUT_3_2_CORE_PROFILE;
#endif
	glutInitDisplayMode(display_mode);

	glutInitWindowSize(750, 750);
	glutInitWindowPosition(0, 0);
	window_id = glutCreateWindow("Assignment");

#ifdef __GLEW_H__
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr << "Error: " << glewGetErrorString(err) << endl;
		exit(1);
	}
	cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;
#endif

	cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << endl;
	cout << "Status: Using GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    
    init(string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\")) + "/");

	create_menu();

	glutReshapeFunc(reshapefunc);
	glutDisplayFunc(displayfunc);
	//glutIdleFunc(idlefunc);

	glutPassiveMotionFunc(pmotionfunc);
	glutMotionFunc(motionfunc);
	//glutMouseFunc(mousefunc);

	glutKeyboardFunc(keydownfunc);
	glutKeyboardUpFunc(keyupfunc);
    
    // Start GLUT loop
	glutMainLoop();
}
