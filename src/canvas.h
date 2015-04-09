#include "opengl.h"
#include "standard.h"
#include "core/geometry.h"

using namespace std;
using namespace core;

#ifndef canvas_h
#define canvas_h

/* This is the renderer. Here you will implement
 * all of the basic OpenGL functionality.
 */
struct canvashdl
{
private:
	unsigned char *color_buffer;
	unsigned short *depth_buffer;
	int width, height;
	static const int depth = 65535;
	int reshape_width, reshape_height;
	double last_reshape_time;
	bool initialized;

	// Interfacing with OpenGL
	GLuint screen_texture;
	GLuint screen_geometry;
	GLuint screen_shader;


	void load_texture();
	void load_geometry();
	void load_shader();
	void init_opengl();
	void check_error(const char *file, int line);
	double get_time();

	// Buffer Management
	void reallocate(int w, int h);

	// Bresenham's Algorithms
    void plot(vec3i xyz, vector<float> varying);
    void plot_point(vec3f v, vector<float> varying);
    void plot_line(vec3f v1, vector<float> v1_varying, vec3f v2, vector<float> v2_varying);
    void plot_half_triangle(vec3i s1, vector<float> v1_varying, vec3i s2, vector<float> v2_varying, vec3i s3, vector<float> v3_varying, vector<float> ave_varying);
    void plot_triangle(vec3f v1, vector<float> v1_varying, vec3f v2, vector<float> v2_varying, vec3f v3, vector<float> v3_varying);
    
    // Clipping
    struct plane
    {
        vec3f  normal;
        float distance;
    };
    
    std::vector<plane> planes;
    vector<vec8f> clip_line(vec8f point1, vec8f point2);
    vector<vec8f> clip_triangle(vec8f v1, vec8f v2, vec8f v3);
    void construct_planes ();
    plane get_plane (vec<float, 4> row1, vec<float, 4> row2);
    vec3f get_intersection_point (vec3f inside_point, vec3f outside_point, float intersection_factor);
    vector<vec8f> clip_triangle_against_plane (vector<vec8f> triangle_points, plane clipping_plane);
    void clip_line_against_plane (vec8f v1, vec8f v2, plane clipping_plane, vector<vec8f> &new_clipping_points);


public:
	canvashdl(int w, int h);
	~canvashdl();

	string working_directory;

	// Buffer Manipulation
	void clear_color_buffer();
	void clear_depth_buffer();
	void resize(int w, int h);
	void swap_buffers();

	int get_width();
	int get_height();

	// Matrix Manipulation
	mat4f matrices[4];

	enum matrix_id
	{
		modelview_matrix = 0,
		projection_matrix = 1,
		normal_matrix = 2,
		viewport_matrix = 3
	} active_matrix;

	void set_matrix(matrix_id matid);
	void load_identity();
	void rotate(float angle, vec3f axis);
	void translate(vec3f direction);
	void scale(vec3f size);
	void perspective(float fovy, float aspect, float n, float f);
	void frustum(float l, float r, float b, float t, float n, float f);
	void ortho(float l, float r, float b, float t, float n, float f);
	void look_at(vec3f eye, vec3f at, vec3f up);
	void viewport(int left, int bottom, int right, int top);
	void update_normal_matrix();
	vec3f to_window(vec2i pixel);
	vec3f unproject(vec3f window);
    vec3i to_pixel(vec3f window);

	// Drawing and Shading
	enum Polygon
	{
		point = 0,
		line = 1,
		fill = 2
	} polygon_mode;

	enum Shading
	{
		none = 0,
		flat = 1,
		gouraud = 2,
		phong = 3
	} shade_model;

	enum Culling
	{
		disable = 0,
		backface = 1,
		frontface = 2
	} culling_mode;
    
    enum 
    {
        red = 0,
        green = 1,
        blue = 2
    } color;

	// Values to pass into the shaders that are uniform across all vertices and fragments
	map<string, const void*> uniform;

	template <class type>
	void get_uniform(string name, const type* &result)
	{
		result = (const type*)uniform[name];
	}

	vec3f shade_vertex(vec8f v, vector<float> &varying);
	vec3f shade_fragment(vector<float> varying);

	void draw_points(const vector<vec8f> &geometry);
	void draw_lines(const vector<vec8f> &geometry, const vector<int> &indices);
	void draw_triangles(const vector<vec8f> &geometry, const vector<int> &indices);
};

#endif
