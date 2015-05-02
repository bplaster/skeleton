#include "camera.h"
#include "object.h"
#include "primitive.h"

camerahdl::camerahdl()
{
	position = vec3f(0., 0., 3.0);
	orientation = vec3f(0.0, 0.0, 0.0);
    model = new pyramidhdl(0.2, 0.5, 4.0);
	type = "camera";
	focus = NULL;
	radius = 10.0f;
}

camerahdl::~camerahdl()
{

}

void camerahdl::view()
{
	/* TODO Assignment 1: Do the necessary modelview transformations to move
	 * the camera into place.
	 */
    vec3f up = ror3(vec3f(0.,1.,0.), orientation);
    vec3f center = position + ror3(vec3f(0.,0.,-1.), orientation);
    
    if (focus) {
        center = focus->position;
    }

//    canvas->look_at(position, center, up);
    gluLookAt(position[0], position[1], position[2], center[0], center[1], center[2], up[0], up[1], up[2]);

}

orthohdl::orthohdl()
{
	left = -2.0;
	right = 2.0;
	bottom = -2.0;
	top = 2.0;
	near = 2.0;
	far = 101.0;
	type = "ortho";
}

orthohdl::~orthohdl()
{
}

void orthohdl::project()
{
	// TODO Assignment 1: Use the canvashdl::ortho function to set up an orthographic projection
//    canvas->ortho(left, right, bottom, top, near, far);
    glOrtho(left, right, bottom, top, near, far);
}

frustumhdl::frustumhdl()
{
	left = -2.0;
	right = 2.0;
	bottom = -2.0;
	top = 2.0;
	near = 2.0;
	far = 101.0;
	type = "frustum";
}

frustumhdl::~frustumhdl()
{

}

void frustumhdl::project()
{
	// TODO Assignment 1: Use the canvashdl::frustum function to set up a perspective projection
//    canvas->frustum(left, right, bottom, top, near, far);
    glFrustum(left, right, bottom, top, near, far);

}

perspectivehdl::perspectivehdl()
{
	fovy = m_pi/4.0;
	aspect = 1.0;
	near = 2.0;
	far = 101.0;
	type = "perspective";
}

perspectivehdl::~perspectivehdl()
{

}

void perspectivehdl::project()
{
	// TODO Assignment 1: Use the canvashdl::perspective function to set up a perspective projection
//    canvas->perspective(fovy, aspect, near, far);
    gluPerspective(fovy, aspect, near, far);
}
