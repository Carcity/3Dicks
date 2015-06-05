#ifndef SHADOWMAPFBO_H
#define	SHADOWMAPFBO_H

#include <gl/glew.h>
#include <gl/GL.h>
#include "glm\glm\glm.hpp"
#include <GL/glew.h>

class ShadowMapFBO
{
public:
	GLuint m_fbo;
	GLuint m_shadowMap;

	ShadowMapFBO();
	~ShadowMapFBO();
	bool Init(unsigned int WindowWidth, unsigned int WindowHeight);
	void BindForWriting();
	void BindForReading(GLenum TextureUnit);
};

#endif