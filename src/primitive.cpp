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
    rigid[0].indices.reserve(36);
    
    rigid[0].geometry.push_back(vec8f(-width/2, -height/2, -depth/2, 0.0, 0.0, -1.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f( width/2, -height/2, -depth/2, 0.0, 0.0, -1.0, 1.0, 0.0));
    rigid[0].geometry.push_back(vec8f( width/2,  height/2, -depth/2, 0.0, 0.0, -1.0, 1.0, 1.0));
    rigid[0].geometry.push_back(vec8f(-width/2,  height/2, -depth/2, 0.0, 0.0, -1.0, 0.0, 1.0));
    rigid[0].indices.push_back(0);
    rigid[0].indices.push_back(1);
    rigid[0].indices.push_back(2);
    rigid[0].indices.push_back(0);
    rigid[0].indices.push_back(2);
    rigid[0].indices.push_back(3);
    
    rigid[0].geometry.push_back(vec8f(-width/2, -height/2,  depth/2, -1.0, 0.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-width/2, -height/2, -depth/2, -1.0, 0.0, 0.0, 1.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-width/2,  height/2, -depth/2, -1.0, 0.0, 0.0, 1.0, 1.0));
    rigid[0].geometry.push_back(vec8f(-width/2,  height/2,  depth/2, -1.0, 0.0, 0.0, 0.0, 1.0));
    rigid[0].indices.push_back(4);
    rigid[0].indices.push_back(5);
    rigid[0].indices.push_back(6);
    rigid[0].indices.push_back(4);
    rigid[0].indices.push_back(6);
    rigid[0].indices.push_back(7);
    
    rigid[0].geometry.push_back(vec8f(-width/2, -height/2, -depth/2, 0.0, -1.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f( width/2, -height/2, -depth/2, 0.0, -1.0, 0.0, 1.0, 0.0));
    rigid[0].geometry.push_back(vec8f( width/2, -height/2,  depth/2, 0.0, -1.0, 0.0, 1.0, 1.0));
    rigid[0].geometry.push_back(vec8f(-width/2, -height/2,  depth/2, 0.0, -1.0, 0.0, 0.0, 1.0));
    rigid[0].indices.push_back(10);
    rigid[0].indices.push_back(9);
    rigid[0].indices.push_back(8);
    rigid[0].indices.push_back(11);
    rigid[0].indices.push_back(10);
    rigid[0].indices.push_back(8);
    
    rigid[0].geometry.push_back(vec8f( width/2,  height/2,  depth/2, 0.0, 0.0, 1.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f( width/2, -height/2,  depth/2, 0.0, 0.0, 1.0, 1.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-width/2, -height/2,  depth/2, 0.0, 0.0, 1.0, 1.0, 1.0));
    rigid[0].geometry.push_back(vec8f(-width/2,  height/2,  depth/2, 0.0, 0.0, 1.0, 0.0, 1.0));
    rigid[0].indices.push_back(12);
    rigid[0].indices.push_back(13);
    rigid[0].indices.push_back(14);
    rigid[0].indices.push_back(12);
    rigid[0].indices.push_back(14);
    rigid[0].indices.push_back(15);
    
    rigid[0].geometry.push_back(vec8f( width/2,  height/2,  depth/2, 0.0, 1.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-width/2,  height/2,  depth/2, 0.0, 1.0, 0.0, 1.0, 0.0));
    rigid[0].geometry.push_back(vec8f(-width/2,  height/2, -depth/2, 0.0, 1.0, 0.0, 1.0, 1.0));
    rigid[0].geometry.push_back(vec8f( width/2,  height/2, -depth/2, 0.0, 1.0, 0.0, 0.0, 1.0));
    rigid[0].indices.push_back(16);
    rigid[0].indices.push_back(17);
    rigid[0].indices.push_back(18);
    rigid[0].indices.push_back(16);
    rigid[0].indices.push_back(18);
    rigid[0].indices.push_back(19);
    
    rigid[0].geometry.push_back(vec8f( width/2,  height/2,  depth/2, 1.0, 0.0, 0.0, 0.0, 0.0));
    rigid[0].geometry.push_back(vec8f( width/2,  height/2, -depth/2, 1.0, 0.0, 0.0, 1.0, 0.0));
    rigid[0].geometry.push_back(vec8f( width/2, -height/2, -depth/2, 1.0, 0.0, 0.0, 1.0, 1.0));
    rigid[0].geometry.push_back(vec8f( width/2, -height/2,  depth/2, 1.0, 0.0, 0.0, 0.0, 1.0));
    rigid[0].indices.push_back(20);
    rigid[0].indices.push_back(21);
    rigid[0].indices.push_back(22);
    rigid[0].indices.push_back(20);
    rigid[0].indices.push_back(22);
    rigid[0].indices.push_back(23);
    
    bound = vec6f(-width/2.0 - 0.1, width/2.0 + 0.1, -height/2.0 - 0.1, height/2.0 + 0.1, -depth/2.0 - 0.1, depth/2.0 + 0.1);

	// TODO Assignment 2: Set up the material properties for this object
    rigid[0].material = "material";
    whitehdl *material = new whitehdl();
    this->material[rigid[0].material] = material;
    
}

boxhdl::~boxhdl()
{

}

float spherehdl::textureS(float x, float z){
    return (atan2(x, z) + M_PI)/(2.*M_PI);
}

float spherehdl::textureT(float y){
    return acos(y)/M_PI;
}

/* spherehdl
 *
 * Generate the geometry and indices required to make a sphere.
 */
spherehdl::spherehdl(float radius, int levels, int slices)
{
	rigid.push_back(rigidhdl());

	rigid[0].geometry.reserve(2 + (levels-1)*slices);
	rigid[0].geometry.push_back(vec8f(0.0, 0.0, radius, 0.0, 0.0, 1.0, textureS(0.0, 1.0), textureT(0.0)));
	for (int i = 1; i < levels; i++)
		for (int j = 0; j <= slices; j++)
		{
			vec3f dir(sin(m_pi*(float)i/(float)levels)*cos(2.0*m_pi*(float)j/(float)slices),
					  sin(m_pi*(float)i/(float)levels)*sin(2.0*m_pi*(float)j/(float)slices),
					  cos(m_pi*(float)i/(float)levels));
			rigid[0].geometry.push_back(vec8f(radius*dir[0], radius*dir[1], radius*dir[2],
									 dir[0], dir[1], dir[2], textureS(dir[0], dir[2]), textureT(dir[1])));
		}
	rigid[0].geometry.push_back(vec8f(0.0, 0.0, -radius, 0.0, 0.0, -1.0, textureS(0.0, -1.0), textureT(0.0)));

	for (int i = 0; i < slices; i++)
	{
		rigid[0].indices.push_back(1 + (i+1));
		rigid[0].indices.push_back(1 + i);
		rigid[0].indices.push_back(0);
	}

	for (int i = 0; i < levels-2; i++)
		for (int j = 0; j < slices; j++)
		{
			rigid[0].indices.push_back(1 + i*(slices+1) + j);
			rigid[0].indices.push_back(1 + i*(slices+1) + (j+1));
			rigid[0].indices.push_back(1 + (i+1)*(slices+1) + j);

			rigid[0].indices.push_back(1 + (i+1)*(slices+1) + j);
			rigid[0].indices.push_back(1 + i*(slices+1) + (j+1));
			rigid[0].indices.push_back(1 + (i+1)*(slices+1) + (j+1));
		}

	for (int i = 0; i < slices; i++)
	{
		rigid[0].indices.push_back(1 + (levels-1)*(slices+1));
		rigid[0].indices.push_back(1 + (levels-2)*(slices+1) + i);
		rigid[0].indices.push_back(1 + (levels-2)*(slices+1) + (i+1));
	}

	bound = vec6f(-radius, radius, -radius, radius, -radius, radius);

	// TODO Assignment 2: Set up the material properties for this object
    rigid[0].material = "material";
    whitehdl *material = new whitehdl();
    this->material[rigid[0].material] = material;
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
    
    rigid[0].geometry.push_back(vec8f(0.0, -height/2.0, 0.0, 0.0, -1.0, 0.0, (0.0 + radius/2.0)/radius, (0.0 + radius)/2.0*radius));
    for (int i = 0; i <= slices; i++)
        rigid[0].geometry.push_back(vec8f(radius*cos(2*m_pi*(float)i/(float)slices),
                                          -height/2.0,
                                          radius*sin(2*m_pi*(float)i/(float)slices),
                                          0.0, -1.0, 0.0, (cos(2*m_pi*(float)i/(float)slices) + 1.0)/2.0, (sin(2*m_pi*(float)i/(float)slices) + 1.0)/2.0));
    
    for (int i = 0; i <= slices; i++)
        rigid[0].geometry.push_back(vec8f(radius*cos(2*m_pi*(float)i/(float)slices),
                                          -height/2.0,
                                          radius*sin(2*m_pi*(float)i/(float)slices),
                                          cos(2*m_pi*(float)i/(float)slices),
                                          0.0,
                                          sin(2*m_pi*(float)i/(float)slices), (float)i/(float)slices, 0.0));
    
    
    for (int i = 0; i <= slices; i++)
        rigid[0].geometry.push_back(vec8f(radius*cos(2*m_pi*(float)i/(float)slices),
                                          height/2.0,
                                          radius*sin(2*m_pi*(float)i/(float)slices),
                                          cos(2*m_pi*(float)i/(float)slices),
                                          0.0,
                                          sin(2*m_pi*(float)i/(float)slices), (float)i/(float)slices, 1.0));
    
    for (int i = 0; i <= slices; i++)
        rigid[0].geometry.push_back(vec8f(radius*cos(2*m_pi*(float)i/(float)slices),
                                          height/2.0,
                                          radius*sin(2*m_pi*(float)i/(float)slices),
                                          0.0, 1.0, 0.0, (cos(2*m_pi*(float)i/(float)slices) + 1.0)/2.0, (sin(2*m_pi*(float)i/(float)slices) + 1.0)/2.0 ));
    
    rigid[0].geometry.push_back(vec8f(0.0, height/2.0, 0.0, 0.0, 1.0, 0.0, (0.0 + radius/2.0)/radius, (0.0 + radius)/2*radius));
    
    
    for (int i = 0; i < slices; i++)
    {
        rigid[0].indices.push_back(1 + (i+1));
        rigid[0].indices.push_back(1 + i);
        rigid[0].indices.push_back(0);
    }
    
    for (int i = 0; i < slices; i++)
    {
        rigid[0].indices.push_back(1 + (slices + 1) + i);
        rigid[0].indices.push_back(1 + (slices + 1) + (i+1));
        rigid[0].indices.push_back(1 + 2*(slices + 1) + i);
        
        rigid[0].indices.push_back(1 + 2*(slices + 1) + i);
        rigid[0].indices.push_back(1 + (slices + 1) + (i+1));
        rigid[0].indices.push_back(1 + 2*(slices + 1) + (i+1));
    }
    
    for (int i = 0; i < slices; i++)
    {
        rigid[0].indices.push_back(1 + 3*(slices + 1) + i);
        rigid[0].indices.push_back(1 + 3*(slices + 1) + (i+1));
        rigid[0].indices.push_back(1 + 4*(slices + 1));
    }
    
    bound = vec6f(-radius, radius, -height/2.0, height/2.0, -radius, radius);
    
	// TODO Assignment 2: Set up the material properties for this object
    rigid[0].material = "material";
    whitehdl *material = new whitehdl();
    this->material[rigid[0].material] = material;
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
    
    float nheight = sqrt(1.0f/(1.0f + (height*height)/(radius*radius)));
    float nlength = height*nheight/radius;
    
    rigid[0].geometry.push_back(vec8f(0.0, -height/2.0, 0.0, 0.0, -1.0, 0.0, (0.0 + radius/2.0)/radius, (0.0 + radius)/2.0*radius));
    for (int i = 0; i < slices; i++)
        rigid[0].geometry.push_back(vec8f(radius*cos(2*m_pi*(float)i/(float)slices),
                                          -height/2.0,
                                          radius*sin(2*m_pi*(float)i/(float)slices),
                                          0.0, -1.0, 0.0, (cos(2*m_pi*(float)i/(float)slices) + 1.0)/2.0, (sin(2*m_pi*(float)i/(float)slices) + 1.0)/2.0));
    
    for (int i = 0; i < slices; i++)
        rigid[0].geometry.push_back(vec8f(radius*cos(2*m_pi*(float)i/(float)slices),
                                          -height/2.0,
                                          radius*sin(2*m_pi*(float)i/(float)slices),
                                          nlength*cos(2*m_pi*(float)i/(float)slices),
                                          nheight,
                                          nlength*sin(2*m_pi*(float)i/(float)slices), (cos(2*m_pi*(float)i/(float)slices) + 1.0)/2.0, 1.0));
    
    for (int i = 0; i < slices; i++)
        rigid[0].geometry.push_back(vec8f(0.0, height/2.0, 0.0,
                                          nlength*cos(2*m_pi*((float)i + 0.5)/(float)slices),
                                          nheight,
                                          nlength*sin(2*m_pi*((float)i + 0.5)/(float)slices), (cos(2*m_pi*(float)i/(float)slices) + 1.0)/2.0, 0.0));
    
    for (int i = 0; i < slices; i++)
    {
        rigid[0].indices.push_back(1 + (i+1)%slices);
        rigid[0].indices.push_back(1 + i);
        rigid[0].indices.push_back(0);
    }
    
    for (int i = 0; i < slices; i++)
    {
        rigid[0].indices.push_back(1 + slices + i);
        rigid[0].indices.push_back(1 + slices + (i+1)%slices);
        rigid[0].indices.push_back(1 + 2*slices + i);
    }
    
    bound = vec6f(-radius, radius, -height/2.0, height/2.0, -radius, radius);
    
    // TODO Assignment 2: Set up the material properties for this object
    rigid[0].material = "material";
    whitehdl *material = new whitehdl();
    this->material[rigid[0].material] = material;
    
}

pyramidhdl::~pyramidhdl()
{

}
