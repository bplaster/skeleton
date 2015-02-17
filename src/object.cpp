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
    canvas->set_matrix(canvashdl::modelview_matrix);
    canvas->load_identity();
    canvas->rotate(orientation[0], vec3f(1.,0.,0.));
    canvas->rotate(orientation[1], vec3f(0.,1.,0.));
    canvas->rotate(orientation[2], vec3f(0.,0.,1.));
    canvas->scale(vec3f(scale, scale, scale));
    canvas->translate(position);
    
    for (vector<rigidhdl>::iterator iter = rigid.begin(); iter != rigid.end(); ++iter) {
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
    canvas->set_matrix(canvashdl::modelview_matrix);
    canvas->load_identity();
    canvas->rotate(orientation[0], vec3f(1.,0.,0.));
    canvas->rotate(orientation[1], vec3f(0.,1.,0.));
    canvas->rotate(orientation[2], vec3f(0.,0.,1.));
    canvas->scale(vec3f(scale, scale, scale));
    canvas->translate(position);
    
    vector<vec8f> bound_geometry;
    vector<int> bound_indices;

    bound_geometry.reserve(8);
    bound_indices.reserve(24);
    
    // Box geometry
    // bound(left, right, bottom, top, front, back)
    bound_geometry.push_back(vec8f(bound[0], bound[3], bound[4], 0.0, 0.0, 0.0, 0.0, 0.0)); // LTF 0
    bound_geometry.push_back(vec8f(bound[0], bound[2], bound[4], 0.0, 0.0, 0.0, 0.0, 0.0)); // LBF 1
    bound_geometry.push_back(vec8f(bound[0], bound[3], bound[5], 0.0, 0.0, 0.0, 0.0, 0.0)); // LTB 2
    bound_geometry.push_back(vec8f(bound[0], bound[2], bound[5], 0.0, 0.0, 0.0, 0.0, 0.0)); // LBB 3
    bound_geometry.push_back(vec8f(bound[1], bound[3], bound[4], 0.0, 0.0, 0.0, 0.0, 0.0)); // RTF 4
    bound_geometry.push_back(vec8f(bound[1], bound[2], bound[4], 0.0, 0.0, 0.0, 0.0, 0.0)); // RBF 5
    bound_geometry.push_back(vec8f(bound[1], bound[3], bound[5], 0.0, 0.0, 0.0, 0.0, 0.0)); // RTB 6
    bound_geometry.push_back(vec8f(bound[1], bound[2], bound[5], 0.0, 0.0, 0.0, 0.0, 0.0)); // RBB 7

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

// bound(left, right, bottom, top, front, back)
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

//vec4f objecthdl::translate_point(vec4f point, vec3f direction){
//    mat4f projection(1., 0., 0., direction[0],
//                     0., 1., 0., direction[1],
//                     0., 0., 1., direction[2],
//                     0., 0., 0., 1.);
//    return projection * point;
//}
//
//vec4f objecthdl::scale_point(vec4f point, vec3f size){
//    mat4f projection(size[0], 0., 0., 0.,
//                     0., size[1], 0., 0.,
//                     0., 0., size[2], 0.,
//                     0., 0., 0., 1.);
//    return projection * point;
//}
//
//// http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
//vec4f objecthdl::rotate_point(vec4f point, float angle, vec3f axis){
//    if (angle == 0.0) {
//        return point;
//    } else {
//        float c = cosf(angle);
//        float cp = 1. - c;
//        float s = sinf(angle);
//        axis = norm(axis);
//        mat4f projection(c + pow(axis[0],2)*cp, axis[0]*axis[1]*cp - axis[2]*s, axis[0]*axis[2]*cp + axis[1]*s, 0.,
//                         axis[0]*axis[1]*cp + axis[2]*s, c + pow(axis[1],2)*cp, axis[1]*axis[2]*cp - axis[0]*s, 0.,
//                         axis[0]*axis[2]*cp - axis[1]*s, axis[1]*axis[2]*cp + axis[0]*s, c + pow(axis[2],2)*cp, 0.,
//                         0., 0., 0., 1.);
//        return projection * point;
//    }
//}
//
//// http://en.wikipedia.org/wiki/Euler_angles#Intrinsic_rotations
//vec4f objecthdl::euler_project(vec4f point){
//    vec4f axis = vec4f(0.,0.,1.,1.);
//    
//    // Rotate about Z
//    point = rotate_point(point, orientation[0], axis(0,3));
//    
//    // Rotate about rotated X
//    axis = rotate_point(vec4f(1.,0.,0.,1.), orientation[0], axis(0,3));
//    point = rotate_point(point, orientation[1], axis);
//    
//    // Rotate about rotated Z
//    axis = rotate_point(vec4f(0.,0.,1.,1.), orientation[1], axis(0,3));
//    point = rotate_point(point, orientation[2], axis);
//    
//    return point;
//};
//
//vec8f objecthdl::transform_point(vec8f point){
//    
//    vec4f new_point = vec4f(0.,0.,0.,1.);
//    new_point.set(0, 3, point(0,3));
//    
//    // Euler rotation
//    new_point = euler_project(new_point);
//    
//    // Scale
//    new_point = scale_point(new_point, vec3f(scale,scale,scale));
//    
//    // Translate
//    new_point = translate_point(new_point, position);
//    
//    // Create point
//    vec8f new_vec;
//    new_vec.set(0, 3, new_point(0,3));
//    new_vec.set(3, 8, point(3,8));
//    
//    return new_vec;
//};



