#include "object.h"
#include "canvas.h"

rigidhdl::rigidhdl()
{

}

rigidhdl::~rigidhdl()
{

}

/* draw
 *
 * Draw a rigid body.
 */
void rigidhdl::draw(canvashdl *canvas)
{
	// TODO Assignment 1: Send the rigid body geometry to the renderer
    canvas->draw_triangles(geometry, indices);
}

objecthdl::objecthdl()
{
	position = vec3f(0.0, 0.0, 0.0);
	orientation = vec3f(0.0, 0.0, 0.0);
	bound = vec6f(1.0e6, -1.0e6, 1.0e6, -1.0e6, 1.0e6, -1.0e6);
	scale = 1.0;
}

objecthdl::~objecthdl()
{

}

/* draw
 *
 * Draw the model. Don't forget to apply the transformations necessary
 * for position, orientation, and scale.
 */
void objecthdl::draw(canvashdl *canvas)
{
	// TODO Assignment 1: Send transformations and geometry to the renderer to draw the object
    for (vector<rigidhdl>::iterator iter = rigid.begin(); iter != rigid.end(); iter++) {
        (*iter).draw(canvas);
    }
}

/* draw_bound
 *
 * Create a representation for the bounding box and
 * render it.
 */
void objecthdl::draw_bound(canvashdl *canvas)
{
	/* TODO Assignment 1: Generate the geometry for the bounding box and send the necessary
	 * transformations and geometry to the renderer
	 */
    bound_geometry.reserve(8);
    bound_indices.reserve(24);
    
    // Box geometry
    // bound(left, right, bottom, top, front, back)
    bound_geometry.push_back(vec8f(position[0]+bound[0], position[1]+bound[3], position[2]+bound[4], 0.0, 0.0, 0.0, 0.0, 0.0)); // LTF 0
    bound_geometry.push_back(vec8f(position[0]+bound[0], position[1]+bound[2], position[2]+bound[4], 0.0, 0.0, 0.0, 0.0, 0.0)); // LBF 1
    bound_geometry.push_back(vec8f(position[0]+bound[0], position[1]+bound[3], position[2]+bound[5], 0.0, 0.0, 0.0, 0.0, 0.0)); // LTB 2
    bound_geometry.push_back(vec8f(position[0]+bound[0], position[1]+bound[2], position[2]+bound[5], 0.0, 0.0, 0.0, 0.0, 0.0)); // LBB 3
    bound_geometry.push_back(vec8f(position[0]+bound[1], position[1]+bound[3], position[2]+bound[4], 0.0, 0.0, 0.0, 0.0, 0.0)); // RTF 4
    bound_geometry.push_back(vec8f(position[0]+bound[1], position[1]+bound[2], position[2]+bound[4], 0.0, 0.0, 0.0, 0.0, 0.0)); // RBF 5
    bound_geometry.push_back(vec8f(position[0]+bound[1], position[1]+bound[3], position[2]+bound[5], 0.0, 0.0, 0.0, 0.0, 0.0)); // RTB 6
    bound_geometry.push_back(vec8f(position[0]+bound[1], position[1]+bound[2], position[2]+bound[5], 0.0, 0.0, 0.0, 0.0, 0.0)); // RBB 7

    
    // Indices
    bound_indices.push_back(0);
    bound_indices.push_back(1);
    bound_indices.push_back(0);
    bound_indices.push_back(2);
    bound_indices.push_back(0);
    bound_indices.push_back(4);
    
    bound_indices.push_back(1);
    bound_indices.push_back(3);
    bound_indices.push_back(1);
    bound_indices.push_back(5);
    
    bound_indices.push_back(2);
    bound_indices.push_back(3);
    bound_indices.push_back(2);
    bound_indices.push_back(6);
    
    bound_indices.push_back(3);
    bound_indices.push_back(7);
    
    bound_indices.push_back(4);
    bound_indices.push_back(5);
    bound_indices.push_back(4);
    bound_indices.push_back(6);
    
    bound_indices.push_back(5);
    bound_indices.push_back(7);
    
    bound_indices.push_back(6);
    bound_indices.push_back(7);

    // Draw bounding lines
    canvas->draw_lines(bound_geometry, bound_indices);
}

/* draw_normals
 *
 * create a representation of the normals for this object.
 * If face is false, render the vertex normals. Otherwise,
 * calculate the normals for each face and render those.
 */
void objecthdl::draw_normals(canvashdl *canvas, bool face)
{
	/* TODO Assignment 1: Generate the geometry to display the normals and send the necessary
	 * transformations and geometry to the renderer
	 */
}
