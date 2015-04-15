#include "core/geometry.h"
#include "opengl.h"

using namespace core;

#ifndef camera_h
#define camera_h

struct objecthdl;

struct camerahdl
{
	camerahdl();
	virtual ~camerahdl();

	string type;

	objecthdl *model;
	vec3f position;
	vec3f orientation;

	objecthdl *focus;
	float radius;

	virtual void project() = 0;
	void view();
};

struct orthohdl : camerahdl
{
	orthohdl();
	~orthohdl();

	float left, right, bottom, top, near, far;

	void project();
};

struct frustumhdl : camerahdl
{
	frustumhdl();
	~frustumhdl();

	float left, right, bottom, top, near, far;

	void project();
};

struct perspectivehdl : camerahdl
{
	perspectivehdl();
	~perspectivehdl();

	float fovy, aspect, near, far;

	void project();
};

#endif
