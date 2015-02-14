#include "camera.h"
#include "object.h"

camerahdl::camerahdl()
{
	position = vec3f(0.0, 0.0, 0.0);
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
    canvas->load_identity();
    project(canvas);
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
}
