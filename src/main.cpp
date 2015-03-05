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

manipulate::type manipulator;

bool keys[256];

// GLUI Variables
int current_manipulation = manipulate::translate;
int current_polygon = canvashdl::Polygon::line;
int current_culling = canvashdl::Culling::disable;
int current_normal = scenehdl::Normal::none;
int window_id;
GLUI *glui;
GLUI_Listbox *current_objects;
GLUI_Listbox *current_cameras;
GLUI_Listbox *list_normal;
GLUI_Listbox *list_culling;
GLUI_Listbox *list_polygon;
GLUI_Listbox *list_manipulation;

// Helper functions
void create_camera(int val);
void create_object(int val);
void handle_culling(int val);
void handle_polygon(int val);
void handle_normal(int val);
void handle_manip(int val);
void setup_glui();

void init(string working_directory)
{
	for (int i = 0; i < 256; i++)
		keys[i] = false;

    canvas.working_directory = working_directory;
	scene.canvas = &canvas;
	// TODO Assignment 1: Initialize the Scene as necessary.
    
    // Setup GLUI
    setup_glui();
    
//    mat4f test (11, 12, 13, 14,
//                21, 22, 23, 24,
//                31, 32, 33, 34,
//                41, 42, 43, 44);
//    vec<float,4> a = -test[3];
//    cout << "Test matrix " << a[3] << endl;
    
    // Create camera
    create_camera(Camera::Ortho);
    
    // Setup settings
    handle_culling(current_culling);
    handle_polygon(current_polygon);
    handle_normal(current_normal);
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
        if (scene.active_camera_valid()) {
            scene.cameras[scene.active_camera]->orientation[1] += (float)deltax/100.;
            scene.cameras[scene.active_camera]->orientation[0] += (float)deltay/100.;
        }

		glutPostRedisplay();
	}
	else if (scene.active_camera_valid())
	{
		/* TODO Assignment 1: Figure out which object the mouse pointer is hovering over and make
		 * that the active object.
		 */
        canvas.set_matrix(canvashdl::modelview_matrix);
        //canvas.load_identity();
        
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
        // TODO: Do we need to set the perspective matrix to Ortho before object selection
        // canvas.set_matrix(canvashdl::modelview_matrix);
        
        vec3f old_mouse_window = canvas.to_window(vec2i(mousex,mousey));
        vec3f old_mouse_world = canvas.unproject(old_mouse_window);
        
        int deltax = x - mousex;
        int deltay = y - mousey;
        
		mousex = x;
		mousey = y;
        
        vec3f mouse_window = canvas.to_window(vec2i(x,y));
        vec3f mouse_world = canvas.unproject(mouse_window);
        vec3f delta_world = mouse_world - old_mouse_world;

        
        if (scene.active_object_valid()) {
            switch (current_manipulation) {
                case manipulate::translate: {
                    scene.objects[scene.active_object]->position += delta_world;
                    break;
                }
                case manipulate::rotate: {
                    delta_world *= vec3f(-1.0, 1.0, 1.0);
                    scene.objects[scene.active_object]->orientation += delta_world.swap(0, 1);
                    break;
                }
                case manipulate::scale: { // TODO: make better
                    vec3f old_diff = old_mouse_world - scene.objects[scene.active_object]->position;
                    vec3f new_diff = mouse_world - scene.objects[scene.active_object]->position;
                    float delta_scale = mag(new_diff)/mag(old_diff) - 1.;
                    scene.objects[scene.active_object]->scale += delta_scale/2.;
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

void specialfunc(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP: // Up arrow
            if (scene.active_camera_valid()) {
                vec3f forward = ror3(vec3f(0.,0.,-0.1), scene.cameras[scene.active_camera]->orientation);
                scene.cameras[scene.active_camera]->position += forward;
            }
            break;
        case GLUT_KEY_DOWN: // Down arrow
            if (scene.active_camera_valid()) {
                vec3f backward = ror3(vec3f(0.,0.,0.1), scene.cameras[scene.active_camera]->orientation);
                scene.cameras[scene.active_camera]->position += backward;
            }
            break;
        case GLUT_KEY_RIGHT: // Right arrow
            if (scene.active_camera_valid()) {
                vec3f right = ror3(vec3f(0.1,0.,0.), scene.cameras[scene.active_camera]->orientation);
                scene.cameras[scene.active_camera]->position += right;
            }
            break;
        case GLUT_KEY_LEFT: // Left arrow
            if (scene.active_camera_valid()) {
                vec3f left = ror3(vec3f(-0.1,0.,0.), scene.cameras[scene.active_camera]->orientation);
                scene.cameras[scene.active_camera]->position += left;
            }
            break;
        default:
            break;
    }
    glutPostRedisplay();

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
                glutAttachMenu(GLUT_RIGHT_BUTTON);
            } else {
                bound = true;
                glutSetCursor(GLUT_CURSOR_NONE);
                glutDetachMenu(GLUT_RIGHT_BUTTON);
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
    
    
//	if (key == 27) // Escape Key Pressed
//	{
//		glutDestroyWindow(window_id);
//		exit(0);
//	}
//	else if (key == 'm' && bound)
//	{
//		bound = false;
//		glutSetCursor(GLUT_CURSOR_INHERIT);
//		glutAttachMenu(GLUT_RIGHT_BUTTON);
//	}
//	else if (key == 'm' && !bound)
//	{
//		bound = true;
//		glutSetCursor(GLUT_CURSOR_NONE);
//		glutDetachMenu(GLUT_RIGHT_BUTTON);
//		mousex = x;
//		mousey = y;
//	}
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
            spherehdl *sphere = new spherehdl(1.0, 8.0, 16.0);
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
        case Object::Model :
            
            break;
        default:
            
            break;
    }
    current_objects->set_int_val(index);
    glutPostRedisplay();
}

void create_camera (int val){
    int index = -1;

    switch (val){
//        case Camera::Fovy :
//            
//            break;
//        case Camera::Aspect :
//            
//            break;
//        case Camera::Width :
//            
//            break;
//        case Camera::Height :
//            
//            break;
//        case Camera::Near :
//            
//            break;
//        case Camera::Far :
//            
//            break;
//        case Camera::ToggleDraw :
//            
//            break;
//        case Camera::ClearFocus :
//            
//            break;
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
        default:
            break;
    }
    canvas.polygon_mode = (canvashdl::Polygon)current_polygon;
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

void handle_menu(int val)
{
    if (val == 5){
        glutDestroyWindow(window_id);
        exit(0);
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
    int camera_menu_id = glutCreateMenu(create_camera);
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

/* TODO Assignment 1: Implement a menu that allows the following operations:
 * - change the fovy, aspect, width, height, near or far values of the active camera
 * - Set an object or camera as the focus of the active camera (using canvashdl::look_at)
 * - Unset the focus of the active camera
 * - Translate, rotate, or scale an object or camera
 */
void setup_glui() {
    glui = GLUI_Master.create_glui("Controls",0,800,0);
    GLUI_Panel *scene_panel = glui->add_panel("Current Scene");
    
    current_objects = glui->add_listbox_to_panel(scene_panel, "Objects");
    current_objects->add_item(-1, "");
    glui->add_button_to_panel(scene_panel, "Delete", 0, handle_delete);

    glui->add_separator_to_panel(scene_panel);
    current_cameras = glui->add_listbox_to_panel(scene_panel, "Cameras");
    current_cameras->add_item(-1, "");
    glui->add_button_to_panel(scene_panel, "Delete", 1, handle_delete);
    
    glui->add_separator_to_panel(scene_panel);

    list_polygon = glui->add_listbox_to_panel(scene_panel, "Polygon", &current_polygon, -1, handle_polygon);
    list_polygon->add_item(canvashdl::Polygon::line,    "Line");
    list_polygon->add_item(canvashdl::Polygon::point,   "Point");
    list_normal = glui->add_listbox_to_panel(scene_panel, "Normal", &current_normal, -1, handle_normal);
    list_normal->add_item(scenehdl::Normal::none,       "None");
    list_normal->add_item(scenehdl::Normal::face,       "Face");
    list_normal->add_item(scenehdl::Normal::vertex,     "Vertex");
    list_culling = glui->add_listbox_to_panel(scene_panel, "Culling", &current_culling, -1, handle_culling);
    list_culling->add_item(canvashdl::Culling::disable,     "None");
    list_culling->add_item(canvashdl::Culling::backface,    "Back");
    list_culling->add_item(canvashdl::Culling::frontface,   "Front");
    list_manipulation = glui->add_listbox_to_panel(scene_panel, "Manipulation", &current_manipulation);
    list_manipulation->add_item(manipulate::translate,"Translate");
    list_manipulation->add_item(manipulate::rotate,   "Rotate");
    list_manipulation->add_item(manipulate::scale,    "Scale");
    glui->add_separator_to_panel(scene_panel);
    glui->add_checkbox_to_panel(scene_panel, "Draw Cameras", NULL, 1, toggle_cameras);

    glui->add_column(true);
    GLUI_Panel *obj_panel = glui->add_panel("Create Object");
    glui->add_button_to_panel(obj_panel,    "Box",         Object::Box,       create_object);
    glui->add_button_to_panel(obj_panel,    "Cylinder",    Object::Cylinder,  create_object);
    glui->add_button_to_panel(obj_panel,    "Sphere",      Object::Sphere,    create_object);
    glui->add_button_to_panel(obj_panel,    "Pyramid",     Object::Pyramid,   create_object);
    glui->add_button_to_panel(obj_panel,    "Model",       Object::Model,     create_object);
    
    GLUI_Panel *cam_panel = glui->add_panel("Create Camera");
    glui->add_button_to_panel(cam_panel,    "Ortho",       Camera::Ortho,         create_camera);
    glui->add_button_to_panel(cam_panel,    "Frustum",     Camera::Frustum,       create_camera);
    glui->add_button_to_panel(cam_panel,    "Perspective", Camera::Perspective,   create_camera);
    
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
    glutSpecialFunc(specialfunc);
	//glutMouseFunc(mousefunc);

	glutKeyboardFunc(keydownfunc);
	glutKeyboardUpFunc(keyupfunc);
    
    // Start GLUT loop
	glutMainLoop();
}
