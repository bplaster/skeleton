#include "canvas.h"
#include "core/geometry.h"

canvashdl::canvashdl(int w, int h)
{
	width = w;
	height = h;
	last_reshape_time = -1.0;
	reshape_width = w;
	reshape_height = h;
	initialized = false;

	color_buffer = new unsigned char[width*height*3];
	// TODO Assignment 2: Initialize the depth buffer

	screen_texture = 0;
	screen_geometry = 0;
	screen_shader = 0;

	active_matrix = modelview_matrix;
    
    for (int i = 0; i < 3; i++)
        matrices[i] = identity<float, 4, 4>();
    
	polygon_mode = line;
	culling = backface;
}

canvashdl::~canvashdl()
{
	if (color_buffer != NULL)
	{
		delete [] color_buffer;
		color_buffer = NULL;
	}

	// TODO Assignment 2: Clean up the depth buffer
}

void canvashdl::clear_color_buffer()
{
	memset(color_buffer, 0, width*height*3*sizeof(unsigned char));
}

void canvashdl::clear_depth_buffer()
{
	// TODO Assignment 2: Clear the depth buffer
}

void canvashdl::resize(int w, int h)
{
	// TODO Assignment 2: resize the depth buffer

	last_reshape_time = -1.0;

	if (color_buffer != NULL)
	{
		delete [] color_buffer;
		color_buffer = NULL;
	}

	width = w;
	height = h;

	color_buffer = new unsigned char[w*h*3];

	glActiveTexture(GL_TEXTURE0);
	check_error(__FILE__, __LINE__);
	glBindTexture(GL_TEXTURE_2D, screen_texture);
	check_error(__FILE__, __LINE__);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);
	check_error(__FILE__, __LINE__);
}

/* set_matrix
 *
 * Change which matrix is active.
 */
void canvashdl::set_matrix(matrix_id matid)
{
	// TODO Assignment 1: Change which matrix is active.
    active_matrix = matid;
}

/* load_identity
 *
 * Set the active matrix to the identity matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glLoadIdentity.xml
 */
void canvashdl::load_identity()
{
	// TODO Assignment 1: Set the active matrix to the identity matrix.
    matrices[active_matrix] = identity<float, 4, 4>();
}

/* rotate
 *
 * Multiply the active matrix by a rotation matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glRotate.xml
 */
void canvashdl::rotate(float angle, vec3f axis)
{
	// TODO Assignment 1: Multiply the active matrix by a rotation matrix.
    if (angle != 0.0) {
        float c = cosf(angle);
        float cp = 1. - c;
        float s = sinf(angle);
        axis = norm(axis);
        mat4f projection(c + pow(axis[0],2)*cp, axis[0]*axis[1]*cp - axis[2]*s, axis[0]*axis[2]*cp + axis[1]*s, 0.,
                         axis[0]*axis[1]*cp + axis[2]*s, c + pow(axis[1],2)*cp, axis[1]*axis[2]*cp - axis[0]*s, 0.,
                         axis[0]*axis[2]*cp - axis[1]*s, axis[1]*axis[2]*cp + axis[0]*s, c + pow(axis[2],2)*cp, 0.,
                         0., 0., 0., 1.);
        matrices[active_matrix] *= projection;
    }
}

/* translate
 *
 * Multiply the active matrix by a translation matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glTranslate.xml
 */
void canvashdl::translate(vec3f direction)
{
	// TODO Assignment 1: Multiply the active matrix by a translation matrix.
    mat4f projection(1., 0., 0., direction[0],
                     0., 1., 0., direction[1],
                     0., 0., 1., direction[2],
                     0., 0., 0., 1.);
    
    matrices[active_matrix] *= projection;
}

/* scale
 *
 * Multiply the active matrix by a scaling matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glScale.xml
 */
void canvashdl::scale(vec3f size)
{
	// TODO Assignment 1: Multiply the active matrix by a scaling matrix.
    mat4f projection(size[0], 0., 0., 0.,
                     0., size[1], 0., 0.,
                     0., 0., size[2], 0.,
                     0., 0., 0., 1.);
    
    matrices[active_matrix] *= projection;
}

/* perspective
 *
 * Multiply the active matrix by a perspective projection matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml
 */
void canvashdl::perspective(float fovy, float aspect, float n, float f)
{
	// TODO Assignment 1: Multiply the active matrix by a perspective projection matrix.
    float fov = tanf(M_PI_2 - fovy/2.);
    
    mat4f projection(fov/aspect, 0., 0., 0.,
                     0., fov, 0., 0.,
                     0., 0., f/(f- n), 0.,
                     0., 0., 0., -(f*n/(f-n)));
    
    matrices[active_matrix] = projection * matrices[active_matrix];
}

/* frustum
 *
 * Multiply the active matrix by a frustum projection matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
 */
void canvashdl::frustum(float l, float r, float b, float t, float n, float f)
{
	// TODO Assignment 1: Multiply the active matrix by a frustum projection matrix.
    mat4f projection(2.*n/(r-l), 0., (r+l)/(r-l), 0.,
                     0., 2.*n/(t-b), (t+b)/(t-b), 0.,
                     0., 0., -(f+n)/(f-n), -2.*f*n/(f-n),
                     0., 0., -1., 0.);
    
    matrices[active_matrix] *= projection;
}

/* ortho
 *
 * Multiply the active matrix by an orthographic projection matrix.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml
 */
void canvashdl::ortho(float l, float r, float b, float t, float n, float f)
{
	// TODO Assignment 1: Multiply the active matrix by an orthographic projection matrix.
    mat4f projection(2./(r-l), 0., 0., -(r+l)/(r-l),
                     0., 2./(t-b), 0., -(t+b)/(t-b),
                     0., 0., -2./(f-n), (f+n)/(f-n),
                     0., 0., 0., 1.);
    
    matrices[active_matrix] *= projection;
}

/* look_at
 *
 * Move and orient the modelview so the camera is at the 'at' position focused on the 'eye'
 * position and rotated so the 'up' vector is up
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
 */
void canvashdl::look_at(vec3f eye, vec3f at, vec3f up)
{
	// TODO Assignment 1: Emulate the functionality of gluLookAt
    vec3f f = norm(at - eye);
    vec3f s = cross(f, norm(up));
    vec3f u = cross(norm(s), f);
    
    mat4f projection(s[0], s[1], s[2], 0.,
                     u[0], u[1], u[2], 0.,
                     -f[0], -f[1], -f[2], 0.,
                     0., 0., 0., 1.);
    
    matrices[active_matrix] *= projection;
    translate(-eye);
    
}

/* to_window
 *
 * Pixel coordinates to window coordinates.
 */
vec3f canvashdl::to_window(vec2i pixel)
{
	/* TODO Assignment 1: Given a pixel coordinate (x from 0 to width and y from 0 to height),
	 * convert it into window coordinates (x from -1 to 1 and y from -1 to 1).
	 */
    float x = 2.*pixel[0]/width - 1.;
    float y = 1. - 2.*pixel[1]/height;
    float z = 0.;
    
	return vec3f(x,y,z);
}

/* to_pixel
 *
 * Window coordinates to pixel coordinates.
 */
vec2i canvashdl::to_pixel(vec3f window)
{
    int x = roundf(width*(window[0]+1)/2);
    int y = roundf(height*(window[1]+1)/2);
    
    return vec2i(x,y);
}

/* unproject
 *
 * Unproject a window coordinate into world coordinates.
 * This implements: https://www.opengl.org/sdk/docs/man2/xhtml/gluUnProject.xml
 */
vec3f canvashdl::unproject(vec3f window)
{
	// TODO Assignment 1: Unproject a window coordinate into world coordinates.
    vec4f vt  = vec4f(window[0], window[1], window[2], 1);
    
    //vt = inverse(matrices[active_matrix])*vt;
    vt = inverse(matrices[modelview_matrix])*inverse(matrices[projection_matrix])*vt;
    
	return vec3f(vt[0],vt[1],vt[2]);
}

/* shade_vertex
 *
 * This is the vertex shader.
 * v[0] to v[2] is position
 * v[3] to v[5] is normal
 * v[7] to v[8] is texture coordinates
 * The result from this function is interpolated and passed on to the fragment shader
 * (its also used to draw the geometry during rasterization)
 * Note that the only requirements for the returned values are that the first 3 components
 * be a projected position. The rest are yours to decide what to do with.
 */
vec8f canvashdl::shade_vertex(vec8f v)
{
	// TODO Assignment 1: Do all of the necessary transformations (normal, projection, modelview, etc)
    vec4f vt = vec4f(v[0],v[1],v[2],1.);
    vt = matrices[normal_matrix]*matrices[projection_matrix]*matrices[modelview_matrix]*vt;
    v.set(0, 3, vt(0,3));

	// TODO Assignment 2: Implement Flat and Gouraud shading.
	return v;
}

/* shade_fragment
 *
 * This is the fragment shader. The pixel color is determined here.
 * the values for v are the interpolated result of whatever you returned from the vertex shader
 */
vec3f canvashdl::shade_fragment(vec8f v)
{
	// TODO Assignment 1: Pick a color, any color (as long as it is distinguishable from the background color).
    vec3f color;
    color[red] = 100.;
    color[green] = 100.;
    color[blue] = 100.;

	/* TODO Assignment 2: Figure out the pixel color due to lighting and materials
	 * and implement phong shading.
	 */
	return color;
}

/* plot
 *
 * Plot a pixel.
 */
void canvashdl::plot(vec2i xy, vec8f v)
{
	// TODO Assignment 1: Plot a pixel, calling the fragment shader.
    vec3f color = shade_fragment(v);
    if ((xy[0] >= 0 && xy[0] < width) && (xy[1] >=0 && xy[1] < height)) {
        color_buffer[3*(width*xy[1]+xy[0])+0] = color[red];
        color_buffer[3*(width*xy[1]+xy[0])+1] = color[green];
        color_buffer[3*(width*xy[1]+xy[0])+2] = color[blue];
    }
	// TODO Assignment 2: Check the pixel depth against the depth buffer.
}

/* plot_point
 *
 * Plot a point given in window coordinates.
 */
void canvashdl::plot_point(vec8f v)
{
	// TODO Assignment 1: Plot a point given in window coordinates.
    vec2i vp = to_pixel(vec3f(v[0],v[1],v[2]));
    plot(vp,v);
}

/* plot_line
 *
 * Plot a line defined by two points in window coordinates.
 */
void canvashdl::plot_line(vec8f v1, vec8f v2)
{
	// TODO Assignment 1: Implement Bresenham's Algorithm.
    // Convert to Pixel coordinates here
    vec2i vp1 = to_pixel(vec3f(v1[0],v1[1],v1[2]));
    vec2i vp2 = to_pixel(vec3f(v2[0],v2[1],v2[2]));
    
    // Variables
    vec2i xy, xy_max;
    vec8f v;
    int dy = abs(vp2[1] - vp1[1]);
    int dx = abs(vp2[0] - vp1[0]);
    
    // Check cases
    // abs(slope) < 1
    if(dy<=dx){
        // orient points for E/NE/SE movement
        if(vp2[0]>=vp1[0]){
            xy = vp1;
            xy_max = vp2;
            v = v1;
        } else {
            xy = vp2;
            xy_max = vp1;
            v = v2;
        }
        plot(xy, v);
        int d = 2*dy-dx;
        // step through each pixel
        while (xy[0] <= xy_max[0]){
            xy[0]++;
            // E
            if(d < 0){
                d+=2*dy;
            }
            // NE/SE
            else {
                if(xy_max[1]>xy[1]){
                    xy[1]++;
                } else {
                    xy[1]--;
                }
                d+=2*(dy-dx);
            }
            plot(xy, v);
        }
    }
    // abs(slope) > 1
    else {
        // orient points for E/NE/SE movement
        if(vp2[1]>=vp1[1]) {
            xy = vp1;
            xy_max = vp2;
            v = v1;
        } else {
            xy = vp2;
            xy_max = vp1;
            v = v2;
        }
        plot(xy, v);
        int d = 2*dx-dy;
        // step through each pixel
        while (xy[1] <= xy_max[1]){
            xy[1]++;
            // E
            if(d<=0){
                d+=2*dx;
            }
            // NE/SE
            else {
                if(xy_max[0]>xy[0]){
                    xy[0]++;
                } else {
                    xy[0]--;
                }
                d+=2*(dx-dy);
            }
            plot(xy, v);
        }
    }
	// TODO Assignment 2: Add interpolation for the normals and texture coordinates as well.
}

/* plot_half_triangle
 *
 * Plot half of a triangle defined by three points in window coordinates (v1, v2, v3).
 * The remaining inputs are as follows (s1, s2, s3) are the pixel coordinates of (v1, v2, v3),
 * and (ave) is the average value of the normal and texture coordinates for flat shading.
 */
void canvashdl::plot_half_triangle(vec2i s1, vec2i s2, vec2i s3, vec8f v1, vec8f v2, vec8f v3, vec5f ave)
{
	/* TODO Assignment 2: Implement Bresenham's algorithm. You may plot the horizontal
	 * half or the vertical half. Add interpolation for the normals and texture coordinates as well.
	 */
}

/* plot_triangle
 *
 * Plot a triangle. (v1, v2, v3) are given in window coordinates.
 */
void canvashdl::plot_triangle(vec8f v1, vec8f v2, vec8f v3)
{
	/* TODO Assignment 1: Use the above functions to plot a whole triangle. Don't forget to
	 * take into account the polygon mode. You should be able to render the
	 * triangle as 3 points or 3 lines.
	 */
    
    // 3 Line implementation
    plot_line(v1, v2);
    plot_line(v2, v3);
    plot_line(v3, v1);
    
    // 3 Point implementation

	// TODO Assignment 2: Add in the fill polygon mode.
}

/* draw_points
 *
 * Draw a set of 3D points on the canvas. Each point in geometry is
 * formatted (vx, vy, vz, nx, ny, nz, s, t).
 */
void canvashdl::draw_points(const vector<vec8f> &geometry)
{
	// TODO Assignment 1: Clip the points against the frustum, call the vertex shader, and then draw them.
    vector<vec8f> new_geometry = geometry;
    
    for (vector<vec8f>::iterator iter = new_geometry.begin(); iter != new_geometry.end(); ++iter) {
        
        *iter = shade_vertex(*iter);
        plot_point(*iter);
    }
}

/* Draw a set of 3D lines on the canvas. Each point in geometry
 * is formatted (vx, vy, vz, nx, ny, nz, s, t).
 */
void canvashdl::draw_lines(const vector<vec8f> &geometry, const vector<int> &indices)
{
	// TODO Assignment 1: Clip the lines against the frustum, call the vertex shader, and then draw them.
    vector<vec8f> new_geometry = geometry;
    
    for (vector<vec8f>::iterator iter = new_geometry.begin(); iter != new_geometry.end(); ++iter) {
        
        *iter = shade_vertex(*iter);
    }
    
    for (int i = 0; i < indices.size()-1; i+=2) {
        plot_line(new_geometry[indices[i]], new_geometry[indices[i+1]]);
    }
    
}

/* Draw a set of 3D triangles on the canvas. Each point in geometry is
 * formatted (vx, vy, vz, nx, ny, nz, s, t). Don't forget to clip the
 * triangles against the clipping planes of the projection. You can't
 * just not render them because you'll get some weird popping at the
 * edge of the view. Also, this is where font/back face culling is implemented.
 */
void canvashdl::draw_triangles(const vector<vec8f> &geometry, const vector<int> &indices)
{
	/* TODO Assignment 1: Clip the triangles against the frustum, call the vertex shader,
	 * break the resulting polygons back into triangles, implement front and back face
	 * culling, and then draw the remaining triangles.
	 */
    
    vector<vec8f> new_geometry = geometry;
    
    for (vector<vec8f>::iterator iter = new_geometry.begin(); iter != new_geometry.end(); ++iter) {
        
        *iter = shade_vertex(*iter);
    }
    
    for (int i = 0; i < indices.size()-2; i+=3) {
        plot_triangle(new_geometry[indices[i]], new_geometry[indices[i+1]], new_geometry[indices[i+2]]);
    }

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Do not edit anything below here, that code just sets up OpenGL to render a single
 * quad that covers the whole screen, applies the color_buffer as a texture to it, and
 * keeps the buffer size and texture up to date.
 */
void canvashdl::load_texture()
{
	glGenTextures(1, &screen_texture);
	check_error(__FILE__, __LINE__);
	glActiveTexture(GL_TEXTURE0);
	check_error(__FILE__, __LINE__);
	glBindTexture(GL_TEXTURE_2D, screen_texture);
	check_error(__FILE__, __LINE__);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	check_error(__FILE__, __LINE__);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	check_error(__FILE__, __LINE__);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	check_error(__FILE__, __LINE__);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	check_error(__FILE__, __LINE__);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	check_error(__FILE__, __LINE__);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);
	check_error(__FILE__, __LINE__);
}

void canvashdl::load_geometry()
{
	// x, y, s, t
	const GLfloat geometry[] = {
			-1.0, -1.0, 0.0, 0.0,
			 1.0, -1.0, 1.0, 0.0,
			-1.0,  1.0, 0.0, 1.0,
			-1.0,  1.0, 0.0, 1.0,
			 1.0, -1.0, 1.0, 0.0,
			 1.0,  1.0, 1.0, 1.0
	};

	glGenBuffers(1, &screen_geometry);
	glBindBuffer(GL_ARRAY_BUFFER, screen_geometry);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*6, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*4*6, geometry);
}

void canvashdl::load_shader()
{
	GLuint vertex = load_shader_file(working_directory + "res/canvas.vx", GL_VERTEX_SHADER);
	GLuint fragment = load_shader_file(working_directory + "res/canvas.ft", GL_FRAGMENT_SHADER);

	screen_shader = glCreateProgram();
	glAttachShader(screen_shader, vertex);
	glAttachShader(screen_shader, fragment);
	glLinkProgram(screen_shader);
}

void canvashdl::init_opengl()
{
	glEnable(GL_TEXTURE_2D);
	glViewport(0, 0, width, height);

	load_texture();
	load_geometry();
	load_shader();
	initialized = true;
}

void canvashdl::check_error(const char *file, int line)
{
	GLenum error_code;
	error_code = glGetError();
	if (error_code != GL_NO_ERROR)
		cerr << "error: " << file << ":" << line << ": " << gluErrorString(error_code) << endl;
}

double canvashdl::get_time()
{
	timeval gtime;
	gettimeofday(&gtime, NULL);
	return gtime.tv_sec + gtime.tv_usec*1.0E-6;
}

void canvashdl::viewport(int w, int h)
{
	glViewport(0, 0, w, h);
	last_reshape_time = get_time();
	reshape_width = w;
	reshape_height = h;
}

void canvashdl::swap_buffers()
{
	if (!initialized)
		init_opengl();

	if (last_reshape_time > 0.0 && get_time() - last_reshape_time > 0.125)
		resize(reshape_width, reshape_height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(screen_shader);
	check_error(__FILE__, __LINE__);

	glActiveTexture(GL_TEXTURE0);
	check_error(__FILE__, __LINE__);
	glBindTexture(GL_TEXTURE_2D, screen_texture);
	check_error(__FILE__, __LINE__);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, color_buffer);
	check_error(__FILE__, __LINE__);
	glUniform1i(glGetUniformLocation(screen_shader, "tex"), 0);
	check_error(__FILE__, __LINE__);

	glBindBuffer(GL_ARRAY_BUFFER, screen_geometry);
	check_error(__FILE__, __LINE__);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	check_error(__FILE__, __LINE__);
	glEnableClientState(GL_VERTEX_ARRAY);
	check_error(__FILE__, __LINE__);

	glTexCoordPointer(2, GL_FLOAT, sizeof(GLfloat)*4, (float*)(sizeof(GLfloat)*2));
	check_error(__FILE__, __LINE__);
	glVertexPointer(2, GL_FLOAT, sizeof(GLfloat)*4, NULL);
	check_error(__FILE__, __LINE__);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	check_error(__FILE__, __LINE__);

	glDisableClientState(GL_VERTEX_ARRAY);
	check_error(__FILE__, __LINE__);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	check_error(__FILE__, __LINE__);

	glutSwapBuffers();
	check_error(__FILE__, __LINE__);
}

int canvashdl::get_width()
{
	return width;
}

int canvashdl::get_height()
{
	return height;
}
