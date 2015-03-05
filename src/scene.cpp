#include "scene.h"
#include "camera.h"
#include "object.h"

#include "primitive.h"
#include "model.h"

scenehdl::scenehdl()
{
	canvas = NULL;
	active_camera = -1;
	active_object = -1;
	render_normals = none;
	render_cameras = false;
}

scenehdl::~scenehdl()
{

}

/* draw
 *
 * Update the locations of all of the lights, draw all of the objects, and
 * if enabled, draw the normals, the lights, the cameras, etc.
 */
void scenehdl::draw()
{
	/* TODO Assignment 1: Draw all of the objects, and
	 * if enabled, draw the normals and the cameras.
	 */
    
    // Set projection matrix
    canvas->set_matrix(canvashdl::projection_matrix);
    canvas->load_identity();
    if (active_camera_valid()) {
        cameras[active_camera]->project(canvas);
    }
    
    // Set modelview matrix
    canvas->set_matrix(canvashdl::modelview_matrix);
    canvas->load_identity();
    if (active_camera_valid()) {
        cameras[active_camera]->view(canvas);
    }
    
    // Draw cameras
    if (render_cameras) {
        for (vector<camerahdl*>::iterator iter = cameras.begin(); iter != cameras.end(); ++iter) {
            if ((*iter)->model) {
                (*iter)->model->draw(canvas);
            }
        }
    }
    
    // Draw objects
    for (vector<objecthdl*>::iterator iter = objects.begin(); iter != objects.end(); ++iter) {
        if (*iter) {
            (*iter)->draw(canvas);
            switch (render_normals) {
                case face:
                    (*iter)->draw_normals(canvas, true);
                    break;
                case vertex:
                    (*iter)->draw_normals(canvas, false);
                default:
                    break;
            }
        }
    }
    
    // Draw bound
    if(active_object_valid()){
        objects[active_object]->draw_bound(canvas);
    }
    
    // Draw normal
    if (render_normals != none){
        if (render_normals == face){
            
        }
    }

	/* TODO Assignment 2: Pass the lights to the shaders through canvashdl::uniform.
	 * If enabled, draw the lights.
	 */
}

int scenehdl::object_index_at_point(vec3f point)
{
    int index = -1;
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i]) {
            if (objects[i]->contains_point(point)) {
                index = i;
            }
        }
    }
    return index;
}


bool scenehdl::active_camera_valid()
{
	return (active_camera >= 0 && active_camera < cameras.size() && cameras[active_camera] != NULL);
}

bool scenehdl::active_object_valid()
{
	return (active_object >= 0 && active_object < objects.size() && objects[active_object] != NULL);
}
