#include "object.h"
#include "canvas.h"
#include "core/geometry.h"

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
        
        rigidhdl new_rigid;
        new_rigid.indices = (*iter).indices;
        new_rigid.geometry.reserve(iter->geometry.size());
        vec4f loc = vec4f(0.,0.,0.,1.);
        vec4f axis = vec3f(0.,0.,1.,1.);

        
        for (vector<vec8f>::iterator iter2 = iter->geometry.begin(); iter2 != iter->geometry.end(); iter2++) {
            
            loc.set(0, 3, (*iter2)(0,3));
            
            // Rotate about Z
            loc = rotate_point(loc, orientation[0], axis(0,3));
            
            // Rotate about rotated X
            axis = rotate_point(vec4f(1.,0.,0.,1.), orientation[0], axis(0,3));
            loc = rotate_point(loc, orientation[1], axis);
            
            // Rotate about rotated Z
            axis = rotate_point(vec4f(0.,0.,1.,1.), orientation[1], axis(0,3));
            loc = rotate_point(loc, orientation[2], axis);

            // Scale
            loc = scale_point(loc, vec3f(scale,scale,scale));
            
            // Translate
            loc = translate_point(loc, position);
            
            // Add point
            vec8f new_vec;
            new_vec.set(0, 3, loc(0,3));
            new_vec.set(3, 8, (*iter2)(3,8));
            new_rigid.geometry.push_back(new_vec);

        }
        new_rigid.draw(canvas);
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
    vector<vec8f> bound_geometry;
    vector<int> bound_indices;

    bound_geometry.reserve(8);
    bound_indices.reserve(24);
    
    // Box geometry
    // bound(left, right, bottom, top, front, back)
    bound *= scale;
    bound_geometry.push_back(vec8f(position[0]+bound[0], position[1]+bound[3], position[2]+bound[4], 0.0, 0.0, 0.0, 0.0, 0.0)); // LTF 0
    bound_geometry.push_back(vec8f(position[0]+bound[0], position[1]+bound[2], position[2]+bound[4], 0.0, 0.0, 0.0, 0.0, 0.0)); // LBF 1
    bound_geometry.push_back(vec8f(position[0]+bound[0], position[1]+bound[3], position[2]+bound[5], 0.0, 0.0, 0.0, 0.0, 0.0)); // LTB 2
    bound_geometry.push_back(vec8f(position[0]+bound[0], position[1]+bound[2], position[2]+bound[5], 0.0, 0.0, 0.0, 0.0, 0.0)); // LBB 3
    bound_geometry.push_back(vec8f(position[0]+bound[1], position[1]+bound[3], position[2]+bound[4], 0.0, 0.0, 0.0, 0.0, 0.0)); // RTF 4
    bound_geometry.push_back(vec8f(position[0]+bound[1], position[1]+bound[2], position[2]+bound[4], 0.0, 0.0, 0.0, 0.0, 0.0)); // RBF 5
    bound_geometry.push_back(vec8f(position[0]+bound[1], position[1]+bound[3], position[2]+bound[5], 0.0, 0.0, 0.0, 0.0, 0.0)); // RTB 6
    bound_geometry.push_back(vec8f(position[0]+bound[1], position[1]+bound[2], position[2]+bound[5], 0.0, 0.0, 0.0, 0.0, 0.0)); // RBB 7
    bound /= scale;

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

bool objecthdl::contains_point(vec3f point)
{
    if ((point[0] > position[0] + scale*bound[0]) &&
        (point[0] < position[0] + scale*bound[1]) &&
        (point[1] > position[1] + scale*bound[2]) &&
        (point[1] < position[1] + scale*bound[3])) {
        return true;
    }
    return false;
}

vec4f objecthdl::translate_point(vec4f point, vec3f direction){
    mat4f projection(1., 0., 0., direction[0],
                     0., 1., 0., direction[1],
                     0., 0., 1., direction[2],
                     0., 0., 0., 1.);
    return projection * point;
}

vec4f objecthdl::scale_point(vec4f point, vec3f size){
    mat4f projection(size[0], 0., 0., 0.,
                     0., size[1], 0., 0.,
                     0., 0., size[2], 0.,
                     0., 0., 0., 1.);
    return projection * point;
}

vec4f objecthdl::rotate_point(vec4f point, float angle, vec3f axis){
    if (angle == 0.0) {
        return point;
    } else {
        float c = cosf(angle);
        float cp = 1. - c;
        float s = sinf(angle);
        mat4f projection(c + pow(axis[0],2)*cp, axis[0]*axis[1]*cp - axis[2]*s, axis[0]*axis[2]*cp + axis[1]*s, 0.,
                         axis[0]*axis[1]*cp + axis[2]*s, c + pow(axis[1],2)*cp, axis[1]*axis[2]*cp - axis[0]*s, 0.,
                         axis[0]*axis[2]*cp - axis[1]*s, axis[1]*axis[2]*cp + axis[0]*s, c + pow(axis[2],2)*cp, 0.,
                         0., 0., 0., 1.);
        return projection * point;
    }
}
