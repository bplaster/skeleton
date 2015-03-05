#include "primitive.h"

/* boxhdl
 *
 * Generate the geometry and indices required to make a box.
 */
boxhdl::boxhdl(float width, float height, float depth)
{
	/* TODO Assignment 1: Generate the geometry and indices required to make a box.
	 * Calculate its bounding box.
	 */
    rigid.push_back(rigidhdl());
    
    rigid[0].geometry.reserve(24);
    
    // Near Face
    rigid[0].geometry.push_back(vec8f((width/2), (height/2), (depth/2), 0.0, 0.0, 1.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f((width/2), -(height/2), (depth/2), 0.0, 0.0, 1.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-(width/2), (height/2), (depth/2), 0.0, 0.0, 1.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-(width/2), -(height/2), (depth/2), 0.0, 0.0, 1.0, 0.0, 0.0));
    
    // Far face
    rigid[0].geometry.push_back(vec8f((width/2), (height/2), -(depth/2), 0.0, 0.0, -1.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f((width/2), -(height/2), -(depth/2), 0.0, 0.0, -1.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-(width/2), (height/2), -(depth/2), 0.0, 0.0, -1.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-(width/2), -(height/2), -(depth/2), 0.0, 0.0, -1.0, 0.0, 0.0));
    
    // Left face
    rigid[0].geometry.push_back(vec8f(-(width/2), (height/2), (depth/2), -1.0, 0.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-(width/2), (height/2), -(depth/2), -1.0, 0.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-(width/2), -(height/2), (depth/2), -1.0, 0.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-(width/2), -(height/2), -(depth/2), -1.0, 0.0, 0.0, 0.0, 0.0));
    
    // Right face
    rigid[0].geometry.push_back(vec8f((width/2), (height/2), (depth/2), 1.0, 0.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f((width/2), (height/2), -(depth/2), 1.0, 0.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f((width/2), -(height/2), (depth/2), 1.0, 0.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f((width/2), -(height/2), -(depth/2), 1.0, 0.0, 0.0, 0.0, 0.0));
    
    // Top face
    rigid[0].geometry.push_back(vec8f((width/2), (height/2), (depth/2), 0.0, 1.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f((width/2), (height/2), -(depth/2), 0.0, 1.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-(width/2), (height/2), (depth/2), 0.0, 1.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-(width/2), (height/2), -(depth/2), 0.0, 1.0, 0.0, 0.0, 0.0));
    
    // Bottom face
    rigid[0].geometry.push_back(vec8f((width/2), -(height/2), (depth/2), 0.0, -1.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f((width/2), -(height/2), -(depth/2), 0.0, -1.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-(width/2), -(height/2), (depth/2), 0.0, -1.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-(width/2), -(height/2), -(depth/2), 0.0, -1.0, 0.0, 0.0, 0.0));
    
    // Indices
    for (int i = 0; i < 24; i += 4) {
        rigid[0].indices.push_back(i);
        rigid[0].indices.push_back(i + 1);
        rigid[0].indices.push_back(i + 2);
        
        rigid[0].indices.push_back(i + 1);
        rigid[0].indices.push_back(i + 2);
        rigid[0].indices.push_back(i + 3);
    }
    
    // Bounding box (same as box in this case)
    bound = vec6f(-(width/2) - 0.1, (width/2) + 0.1, -(height/2) - 0.1, (height/2) + 0.1, -(depth/2) - 0.1, (depth/2) + 0.1);
}

boxhdl::~boxhdl()
{

}

/* spherehdl
 *
 * Generate the geometry and indices required to make a sphere.
 */
spherehdl::spherehdl(float radius, int levels, int slices)
{
	rigid.push_back(rigidhdl());

	rigid[0].geometry.reserve(2 + (levels-1)*slices);
	rigid[0].geometry.push_back(vec8f(0.0, 0.0, radius, 0.0, 0.0, 1.0, 0.0, 0.0));
	for (int i = 1; i < levels; i++)
		for (int j = 0; j < slices; j++)
		{
			vec3f dir(sin(m_pi*(float)i/(float)levels)*cos(2.0*m_pi*(float)j/(float)slices),
					  sin(m_pi*(float)i/(float)levels)*sin(2.0*m_pi*(float)j/(float)slices),
					  cos(m_pi*(float)i/(float)levels));
			rigid[0].geometry.push_back(vec8f(radius*dir[0], radius*dir[1], radius*dir[2],
									 dir[0], dir[1], dir[2], 0.0, 0.0));
		}
	rigid[0].geometry.push_back(vec8f(0.0, 0.0, -radius, 0.0, 0.0, -1.0, 0.0, 0.0));

	for (int i = 0; i < slices; i++)
	{
		rigid[0].indices.push_back(1 + (i+1)%slices);
		rigid[0].indices.push_back(1 + i);
		rigid[0].indices.push_back(0);
	}

	for (int i = 0; i < levels-2; i++)
		for (int j = 0; j < slices; j++)
		{
			rigid[0].indices.push_back(1 + i*slices + j);
			rigid[0].indices.push_back(1 + i*slices + (j+1)%slices);
			rigid[0].indices.push_back(1 + (i+1)*slices + j);

			rigid[0].indices.push_back(1 + (i+1)*slices + j);
			rigid[0].indices.push_back(1 + i*slices + (j+1)%slices);
			rigid[0].indices.push_back(1 + (i+1)*slices + (j+1)%slices);
		}

	for (int i = 0; i < slices; i++)
	{
		rigid[0].indices.push_back(1 + (levels-1)*slices);
		rigid[0].indices.push_back(1 + (levels-2)*slices + i);
		rigid[0].indices.push_back(1 + (levels-2)*slices + (i+1)%slices);
	}

	bound = vec6f(-radius, radius, -radius, radius, -radius, radius);
}

spherehdl::~spherehdl()
{

}

/* cylinderhdl
 *
 * Generate the geometry and indices required to make a cylinder.
 */
cylinderhdl::cylinderhdl(float radius, float height, int slices)
{
	/* TODO Assignment 1: Generate the geometry and indices required to make a cylinder.
	 * Calculate its bounding box.
	 */
    rigid.push_back(rigidhdl());
    rigid[0].geometry.reserve(2 + 4*slices);
    
    vector<vec2f> dir (slices);
    
    // Calculate directions along X and Z axes
    for (int i = 0; i < slices; i++){
        vec2f temp(sin(2.0*m_pi*((float)i/(float)slices)), cos(2.0*m_pi*((float)i/(float)slices)));
        dir[i] = temp;
    };
    
    // Enter geometry for all faces of cylinder
    for (int i = 0; i < 3; i++){
        
        for (int j = 0; j < slices; j++){
            
            if (i == 0)
            {
                rigid[0].geometry.push_back(vec8f(radius*dir[j][0], -(height/2), radius*dir[j][1], dir[j][0], 0.0, dir[j][1], 0.0, 0.0));
                rigid[0].geometry.push_back(vec8f(radius*dir[j][0], (height/2), radius*dir[j][1], dir[j][0], 0.0, dir[j][1], 0.0, 0.0));
            }
            else if (i == 1)
            {
                rigid[0].geometry.push_back(vec8f(radius*dir[j][0], (height/2), radius*dir[j][1], 0.0, 1.0, 0.0, 0.0, 0.0));
            }
            else if (i == 2)
            {
                rigid[0].geometry.push_back(vec8f(radius*dir[j][0], -(height/2), radius*dir[j][1], 0.0, -1.0, 0.0, 0.0, 0.0));
            }
        }
    }
    
    // Add points representing centre of top and bottom surfaces
    rigid[0].geometry.push_back(vec8f(0.0, (height/2), 0.0, 0.0, 1.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(0.0, -(height/2), 0.0, 0.0, -1.0, 0.0, 0.0, 0.0));
    
    // Indices
    
    for (int i = 0; i < 2*slices - 2; i++){
        
        // Lateral surface area
        rigid[0].indices.push_back(i);
        rigid[0].indices.push_back(i + 1);
        rigid[0].indices.push_back(i + 2);
    }
    
    // Wrap around Lateral surface area
    rigid[0].indices.push_back(0);
    rigid[0].indices.push_back(2*slices - 2);
    rigid[0].indices.push_back(2*slices - 1);
    
    rigid[0].indices.push_back(0);
    rigid[0].indices.push_back(1);
    rigid[0].indices.push_back(2*slices - 1);
    
    // Top and Bottom surfaces
    for (int i = 0; i < slices; i++){
        
        if (i < slices - 1){
            
            // Top surface
            rigid[0].indices.push_back(2*slices + i);
            rigid[0].indices.push_back(2*slices + i + 1);
            rigid[0].indices.push_back(4*slices);
            
            // Bottom surface
            rigid[0].indices.push_back(3*slices + i);
            rigid[0].indices.push_back(3*slices + i + 1);
            rigid[0].indices.push_back(4*slices + 1);
        }
        else if (i == slices - 1) {
            // Wrap around top surface
            rigid[0].indices.push_back(2*slices + i);
            rigid[0].indices.push_back(2*slices);
            rigid[0].indices.push_back(4*slices);
            
            // Wrap around bottom surface
            rigid[0].indices.push_back(3*slices + i);
            rigid[0].indices.push_back(3*slices);
            rigid[0].indices.push_back(4*slices + 1);
        }
                
    }
    
    // Bounding box
    bound = vec6f(-radius, radius, -(height/2), (height/2), -radius, radius);
    
}

cylinderhdl::~cylinderhdl()
{

}

/* pyramidhdl
 *
 * Generate the geometry and indices required to make a pyramid.
 */
pyramidhdl::pyramidhdl(float radius, float height, int slices)
{
	/* TODO Assignment 1: Generate the geometry and indices required to make a pyramid.
	 * Calculate its bounding box.
	 */
    rigid.push_back(rigidhdl());
    rigid[0].geometry.reserve(1 + 3*slices);
    
    vector<vec3f> dir (slices);
    float cone_angle = atan2f(radius, height);
    
    // Calculate directions along X, Y and Z axes
    for (int i = 0; i < slices; i ++){
        vec3f temp(cos(cone_angle)*sin(2.0*m_pi*((float)i/(float)slices)), sin(cone_angle),
                   cos(cone_angle)*cos(2.0*m_pi*((float)i/(float)slices)));
        dir[i] = temp;
    };
    
    // Enter geometry for all faces of pyramid
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < slices; j++){
            
            if (i == 0)
            {
                rigid[0].geometry.push_back(vec8f(radius*dir[j][0], -(height/3), radius*dir[j][2], dir[j][0], dir[j][1], dir[j][2], 0.0, 0.0));
                rigid[0].geometry.push_back(vec8f(0.0, 2*(height/3), 0.0, dir[j][0], dir[j][1], dir[j][2], 0.0, 0.0));
            }
            else if (i == 1)
            {
                rigid[0].geometry.push_back(vec8f(radius*dir[j][0], -(height/3), radius*dir[j][2], 0.0, -1.0, 0.0, 0.0, 0.0));
            }
        }
    }
    
    // Add point representing centre of bottom of pyramid
    rigid[0].geometry.push_back(vec8f(0.0, -(height/3), 0.0, 0.0, -1.0, 0.0, 0.0, 0.0));
    
    // Indices
    
    for (int i = 0; i < 2*slices - 2; i += 2){
        
        // Curved surface area
        rigid[0].indices.push_back(i);
        rigid[0].indices.push_back(i + 1);
        rigid[0].indices.push_back(i + 2);
    }
    
    // Wrap around curved surface area
    rigid[0].indices.push_back(2*slices - 2);
    rigid[0].indices.push_back(2*slices - 1);
    rigid[0].indices.push_back(0);
    
    for (int i = 0; i < slices; i++){
        if (i < slices - 1){
            
            // Bottom surface
            rigid[0].indices.push_back(2*slices + i);
            rigid[0].indices.push_back(2*slices + i + 1);
            rigid[0].indices.push_back(3*slices);
        }
        else if (i == slices - 1) {
            
            // Wrap around bottom surface
            rigid[0].indices.push_back(2*slices + i);
            rigid[0].indices.push_back(2*slices);
            rigid[0].indices.push_back(3*slices);
        }
        
    }
    
    // Bounding box
    bound = vec6f(-radius, radius, -(height/3), 2*(height/3), -radius, radius);
}

pyramidhdl::~pyramidhdl()
{

}
