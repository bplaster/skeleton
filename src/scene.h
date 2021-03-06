#include "opengl.h"

#ifndef scene_h
#define scene_h

struct objecthdl;
struct lighthdl;
struct camerahdl;

struct scenehdl
{
	scenehdl();
	~scenehdl();

	vector<objecthdl*> objects;
	vector<lighthdl*> lights;
	vector<camerahdl*> cameras;

	int active_camera;
	int active_object;
    int active_light;

	// Whether and how to render the normals
	enum Normal
	{
		none = 0,
		face = 1,
		vertex = 2
	} render_normals;

	// Whether or not to render the lights or the cameras or both
	bool render_lights;
	bool render_cameras;

	void draw();

	bool active_camera_valid();
	bool active_object_valid();
    bool active_light_valid();

	void create_camera();
    void clear_glsl();
};

#endif
