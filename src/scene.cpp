#include "scene.h"
#include "camera.h"
#include "object.h"
#include "light.h"

#include "primitive.h"
#include "model.h"

scenehdl::scenehdl()
{
	canvas = NULL;
	active_camera = -1;
	active_object = -1;
    active_light = -1;
	render_normals = none;
	render_lights = false;
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
    // TODO: doesn't need to happen every frame
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
            if (*iter) {
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
    
    

	/* TODO Assignment 2: Clear the uniform variables and pass the vector of
	 * lights into the renderer as a uniform variable.
	 * TODO Assignment 2: Update the light positions and directions
	 * TODO Assignment 2: Render the lights
	 */
    
    // Draw lights
    if (render_lights) {
        for (vector<lighthdl*>::iterator iter = lights.begin(); iter != lights.end(); ++iter) {
            if (*iter) {
                (*iter)->model->draw(canvas);
            }
        }
    }
    
    // Bounding box for light
    if (active_light_valid()) {
        lights[active_light]->model->draw_bound(canvas);
    }
}


bool scenehdl::active_camera_valid()
{
	return (active_camera >= 0 && active_camera < cameras.size() && cameras[active_camera] != NULL);
}

bool scenehdl::active_object_valid()
{
	return (active_object >= 0 && active_object < objects.size() && objects[active_object] != NULL);
}

bool scenehdl::active_light_valid()
{
    return (active_light >= 0 && active_light < lights.size() && lights[active_light] != NULL);
}
