#include <OpenGL/OpenGL.h>
#include <GLUI/GLUI.h>

#include "standard.h"
#include "canvas.h"
#include "scene.h"
#include "camera.h"
#include "model.h"
#include "primitive.h"
#include "tinyfiledialogs.h"

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
		front = 7,
		back = 8
	};
}

enum class Object{
    Box,
    Cylinder,
    Sphere,
    Pyramid,
    Model
};

enum class Camera{
    Fovy,
    Aspect,
    Width,
    Height,
    Near,
    Far,
    ToggleDraw,
    ClearFocus,
    Ortho,
    Frustum,
    Perspective
};

enum class Polygon{
    Point,
    Line
};

enum class Culling{
    None,
    Back,
    Front
};

enum class Normal{
    None,
    Face,
    Vertex
};

manipulate::type manipulator;

bool keys[256];

// GLUI Variables
int current_camera = 0;
int current_manipulation = 0;
int window_id;

void init(string working_directory)
{
	for (int i = 0; i < 256; i++)
		keys[i] = false;

    canvas.working_directory = working_directory;
	scene.canvas = &canvas;
	// TODO Assignment 1: Initialize the Scene as necessary.
    
    // Create camera
    camerahdl *camera = new orthohdl;
    scene.cameras.push_back(camera);
    scene.active_camera = (int)scene.cameras.size() - 1;
    camera->view(scene.canvas);

}

void displayfunc()
{
	canvas.clear_color_buffer();

	scene.draw();

	canvas.swap_buffers();
}

void reshapefunc(int w, int h)
{
	canvas.viewport(w, h);
	glutPostRedisplay();
}

void pmotionfunc(int x, int y)
{
	if (bound)
	{
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

		// TODO Assignment 1: Use the mouse delta to change the orientation of the active camera

		glutPostRedisplay();
	}
	else if (scene.active_camera_valid())
	{
		/* TODO Assignment 1: Figure out which object the mouse pointer is hovering over and make
		 * that the active object.
		 */
        vec3f mouse_window = canvas.to_window(vec2i(x,y));
        vec3f mouse_world = canvas.unproject(mouse_window);
        
        int object_index = scene.object_index_at_point(mouse_world);
        scene.active_object = object_index;
        glutPostRedisplay();
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
        vec3f old_mouse_window = canvas.to_window(vec2i(mousex,mousey));
        vec3f old_mouse_world = canvas.unproject(old_mouse_window);
        
		mousex = x;
		mousey = y;
        
        vec3f mouse_window = canvas.to_window(vec2i(x,y));
        vec3f mouse_world = canvas.unproject(mouse_window);
        vec3f delta_world = mouse_world - old_mouse_world;
        
        if (scene.active_object_valid() && scene.objects[scene.active_object]->contains_point(mouse_world)) {
            switch (current_manipulation) {
                case manipulate::translate:
                    scene.objects[scene.active_object]->position += delta_world;
                    break;
                case manipulate::rotate:
                    break;
                case manipulate::scale: {
                    vec3f old_diff = old_mouse_world - scene.objects[scene.active_object]->position;
                    vec3f new_diff = mouse_world - scene.objects[scene.active_object]->position;
                    float delta_scale = mag(new_diff)/mag(old_diff) - 1.;
                    scene.objects[scene.active_object]->scale += delta_scale;
                    break;
                }
                default:
                    break;
            }
            
        }

		/* TODO Assignment 1: Implement the functionality for the following operations here:
		 * translation, rotation, and scaling of an object
		 * changing the fovy, aspect, width, height, near, or far values of the active camera
		 *
		 * This uses the mouse, so you'll have to determine the world coordinate position of the
		 * mouse pointer.
		 */
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

	if (key == 27) // Escape Key Pressed
	{
		glutDestroyWindow(window_id);
		exit(0);
	}
	else if (key == 'm' && bound)
	{
		bound = false;
		glutSetCursor(GLUT_CURSOR_INHERIT);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
	}
	else if (key == 'm' && !bound)
	{
		bound = true;
		glutSetCursor(GLUT_CURSOR_NONE);
		glutDetachMenu(GLUT_RIGHT_BUTTON);
		mousex = x;
		mousey = y;
	}
}

void keyupfunc(unsigned char key, int x, int y)
{
	keys[key] = false;
}

void idlefunc()
{
	bool change = false;
    
    if ( glutGetWindow() != window_id )
        glutSetWindow(window_id);

	// TODO Assignment 1: handle continuous keyboard inputs

	if (change)
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
}

void handle_objects (int val){

    switch ((Object)val){
        case Object::Box : {
            
            // Create dynamic box object
            boxhdl *box = new boxhdl(1.0, 1.0, 1.0);
            scene.objects.push_back(box);
            break;
        }
        case Object::Cylinder : {
            
            // Create dynamic sphere object
            cylinderhdl *cylinder = new cylinderhdl(1.0, 4.0, 4.0);
            scene.objects.push_back(cylinder);
            break;
        }
        case Object::Sphere : {
            
            // Create dynamic sphere object
            spherehdl *sphere = new spherehdl(1.0, 8.0, 16.0);
            scene.objects.push_back(sphere);
            break;
        }
        case Object::Pyramid :
            
            break;
        case Object::Model :
            
            break;
        default:
            
            break;
    }
    glutPostRedisplay();
}

void handle_cameras (int val){
    
    switch ((Camera)val){
        case Camera::Fovy :
            
            break;
        case Camera::Aspect :
            
            break;
        case Camera::Width :
            
            break;
        case Camera::Height :
            
            break;
        case Camera::Near :
            
            break;
        case Camera::Far :
            
            break;
        case Camera::ToggleDraw :
            
            break;
        case Camera::ClearFocus :
            
            break;
        case Camera::Ortho : {
            
            camerahdl *camera = new orthohdl;
            scene.cameras.push_back(camera);
            scene.active_camera = (int)scene.cameras.size() - 1;
            camera->view(scene.canvas);

            
            break;
        }
        case Camera::Frustum : {
            camerahdl *camera = new frustumhdl;
            scene.cameras.push_back(camera);
            scene.active_camera = (int)scene.cameras.size() - 1;
            camera->view(scene.canvas);

            break;
        }
        case Camera::Perspective :
            
            break;
        default:
            
            break;
    }
    glutPostRedisplay();
}

void handle_polygons (int val){
    
    switch ((Polygon)val){
        case Polygon::Point :
            
            break;
        case Polygon::Line :
            
            break;
        default:
            
            break;
    }
    glutPostRedisplay();
}

void handle_culling (int val){
    
    switch ((Culling)val){
        case Culling::None :
            
            break;
        case Culling::Back :
            
            break;
        case Culling::Front :
            
            break;
        default:
            
            break;
    }
    glutPostRedisplay();
}

void handle_normal (int val){
    
    switch ((Normal)val){
        case Normal::None :
            
            break;
        case Normal::Face :
            
            break;
        case Normal::Vertex :
            
            break;
        default:
            
            break;
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
    int object_menu_id = glutCreateMenu(handle_objects);
    glutAddMenuEntry("Box", 0);
    glutAddMenuEntry("Cylinder", 1);
    glutAddMenuEntry("Sphere", 2);
    glutAddMenuEntry("Pyramid", 3);
    glutAddMenuEntry("Model", 4);
    
    // Add camera menu items
    int camera_menu_id = glutCreateMenu(handle_cameras);
    glutAddMenuEntry("Fovy", 0);
    glutAddMenuEntry("Aspect", 1);
    glutAddMenuEntry("Width", 2);
    glutAddMenuEntry("Height", 3);
    glutAddMenuEntry("Near", 4);
    glutAddMenuEntry("Far", 5);
    glutAddMenuEntry("Toggle Draw", 6);
    glutAddMenuEntry("Clear Focus", 7);
    glutAddMenuEntry("Ortho", 8);
    glutAddMenuEntry("Frustum", 9);
    glutAddMenuEntry("Perspective", 10);

    // Add polygon menu items
    int polygon_menu_id = glutCreateMenu(handle_polygons);
    glutAddMenuEntry("Point", 0);
    glutAddMenuEntry("Line", 1);

    // Add culling menu items
    int culling_menu_id = glutCreateMenu(handle_culling);
    glutAddMenuEntry("None", 0);
    glutAddMenuEntry("Back", 1);
    glutAddMenuEntry("Front", 2);
    
    // Add normals menu items
    int normals_menu_id = glutCreateMenu(handle_normal);
    glutAddMenuEntry("None", 0);
    glutAddMenuEntry("Face", 1);
    glutAddMenuEntry("Vertex", 2);
    
    // Add submenus to menu
    int menu_id = glutCreateMenu(handle_menu);
    glutAddSubMenu("Objects", object_menu_id);
    glutAddSubMenu("Cameras", camera_menu_id);
    glutAddSubMenu("Polygon", polygon_menu_id);
    glutAddSubMenu("Culling", culling_menu_id);
    glutAddSubMenu("Normals", normals_menu_id);
    glutAddMenuEntry("Quit", 5);


	/* TODO Assignment 2: Add to the menu the following operations:
	 * - create directional, point, or spot lights
	 * - enable/disable rendering of lights
	 * - set the polygon mode to fill
	 * - change the ambient, diffuse, or specular color of a light
	 * - change the attenuation parameters of a point or spot light
	 * - set the shading mode to none, flat, gouraud, or phong
	 */

	glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMenuStatusFunc(menustatusfunc);
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
    
    // Setup GLUI
    /* TODO Assignment 1: Implement a menu that allows the following operations:
     * - change the fovy, aspect, width, height, near or far values of the active camera
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
    GLUI *glui = GLUI_Master.create_glui("GLUI",0,800,0);
    glui->add_statictext("Create Object");
    glui->add_button("Box",         (int)Object::Box,       handle_objects);
    glui->add_button("Cylinder",    (int)Object::Cylinder,  handle_objects);
    glui->add_button("Sphere",      (int)Object::Sphere,    handle_objects);
    glui->add_button("Pyramid",     (int)Object::Pyramid,   handle_objects);
    glui->add_button("Model",       (int)Object::Model,     handle_objects);
    GLUI_Listbox *list_manipulate = glui->add_listbox("Manipulation", &current_manipulation);
    list_manipulate->add_item(manipulate::translate,"Translate");
    list_manipulate->add_item(manipulate::rotate,   "Rotate");
    list_manipulate->add_item(manipulate::scale,    "Scale");

    glui->add_separator();
    GLUI_Listbox *list_camera = glui->add_listbox("Camera");
    list_camera->add_item(1,"Ortho");
    list_camera->add_item(2,"Frustum");
    list_camera->add_item(3,"Perspective");
    GLUI_Listbox *list_polygon = glui->add_listbox("Polygon");
    list_polygon->add_item(1,"Line");
    list_polygon->add_item(2,"Point");
    glui->add_checkbox("Draw Cameras", NULL, 1, toggle_cameras);
    glui->add_separator();
    glui->add_button("Quit", 0, exit);
    
    glui->set_main_gfx_window(window_id);
    GLUI_Master.set_glutIdleFunc(idlefunc);
    GLUI_Master.set_glutMouseFunc(mousefunc);
    
    // Start GLUT loop
	glutMainLoop();
}
