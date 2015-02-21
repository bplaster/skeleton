#include "camera.h"
#include "object.h"

camerahdl::camerahdl()
{
	position = vec3f(0., 0., -5.0);
	orientation = vec3f(0.0, 0.0, 0.0);
	model = NULL;
	type = "camera";
	focus = NULL;
	radius = 10.0f;
}

camerahdl::~camerahdl()
{

}

void camerahdl::view(canvashdl *canvas)
{
	/* TODO Assignment 1: Do the necessary modelview transformations to move
	 * the camera into place.
	 */
    canvas->rotate(-orientation[2], vec3f(1.,0.,0.));
    canvas->rotate(-orientation[1], vec3f(0.,1.,0.));
    canvas->rotate(-orientation[0], vec3f(0.,0.,1.));
    canvas->translate(-position);

}

orthohdl::orthohdl()
{
	left = -5.0;
	right = 5.0;
	bottom = -5.0;
	top = 5.0;
	front = 2.0;
	back = 101.0;
	type = "ortho";
}

orthohdl::~orthohdl()
{
}

void orthohdl::project(canvashdl *canvas)
{
	// TODO Assignment 1: Use the canvashdl::ortho function to set up an orthographic projection
    canvas->set_matrix(canvashdl::projection_matrix);
    canvas->load_identity();
    canvas->ortho(left, right, bottom, top, front, back);
}

frustumhdl::frustumhdl()
{
	left = -5.0;
	right = 5.0;
	bottom = -5.0;
	top = 5.0;
	front = 2.0;
	back = 101.0;
	type = "frustum";
}

frustumhdl::~frustumhdl()
{

}

void frustumhdl::project(canvashdl *canvas)
{
	// TODO Assignment 1: Use the canvashdl::frustum function to set up a perspective projection
    canvas->set_matrix(canvashdl::projection_matrix);
    canvas->load_identity();
    canvas->frustum(left, right, bottom, top, front, back);
}

perspectivehdl::perspectivehdl()
{
	fovy = m_pi/4.0;
	aspect = 1.0;
	front = 2.0;
	back = 101.0;
	type = "perspective";
}

perspectivehdl::~perspectivehdl()
{

}

void perspectivehdl::project(canvashdl *canvas)
{
	// TODO Assignment 1: Use the canvashdl::perspective function to set up a perspective projection
    canvas->set_matrix(canvashdl::projection_matrix);
    canvas->load_identity();
    canvas->perspective(fovy, aspect, front, back);
}
