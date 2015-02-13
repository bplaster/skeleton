#include "opengl.h"
#include "standard.h"
#include "canvas.h"
#include "scene.h"
#include "camera.h"
#include "model.h"
#include "primitive.h"
#include "tinyfiledialogs.h"

int window_id;

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

void init(string working_directory)
{
	for (int i = 0; i < 256; i++)
		keys[i] = false;

    canvas.working_directory = working_directory;
	scene.canvas = &canvas;
	// TODO Assignment 1: Initialize the Scene as necessary.
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
		int deltax = x - mousex;
		int deltay = mousey - y;

		mousex = x;
		mousey = y;

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

	// TODO Assignment 1: handle continuous keyboard inputs

	if (change)
		glutPostRedisplay();
}

void menustatusfunc(int status, int x, int y)
{
	if (status == GLUT_MENU_IN_USE)
		menu = true;
}


void handle_objects (int val){

    switch ((Object)val){
        case Object::Box :
            
            break;
        case Object::Cylinder :
            
            break;
        case Object::Sphere : {
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
        case Camera::Ortho :
            canvas.ortho(-10, 10, -10, 10, -10, 10);
            break;
        case Camera::Frustum :
            
            break;
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
	glutIdleFunc(idlefunc);

	glutPassiveMotionFunc(pmotionfunc);
	glutMotionFunc(motionfunc);
	glutMouseFunc(mousefunc);

	glutKeyboardFunc(keydownfunc);
	glutKeyboardUpFunc(keyupfunc);

	glutMainLoop();
}
