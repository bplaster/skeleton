#include "core/geometry.h"
#include "canvas.h"

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

	virtual void project(canvashdl *canvas) = 0;
	void view(canvashdl *canvas);
};

struct orthohdl : camerahdl
{
	orthohdl();
	~orthohdl();

	float left, right, bottom, top, near, far;

	void project(canvashdl *canvas);
};

struct frustumhdl : camerahdl
{
	frustumhdl();
	~frustumhdl();

	float left, right, bottom, top, near, far;

	void project(canvashdl *canvas);
};

struct perspectivehdl : camerahdl
{
	perspectivehdl();
	~perspectivehdl();

	float fovy, aspect, near, far;

	void project(canvashdl *canvas);
};

#endif
