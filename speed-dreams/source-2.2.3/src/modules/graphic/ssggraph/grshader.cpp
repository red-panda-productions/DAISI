/***************************************************************************

file                 : grshader.cpp
created              : Fri Oct 10 23:16:44 CET 2015
copyright            : (C) 2015 by Xavier Bertaux
version              : $Id: grshader.cpp 4374 2015-10-10 23:20:37Z torcs-ng $

***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <tgf.h>
#include <glfeatures.h>

#include "grshader.h"

#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
PFNGLVERTEXATTRIB1DARBPROC glVertexAttrib1dARB = NULL;
PFNGLVERTEXATTRIB1DVARBPROC glVertexAttrib1dvARB = NULL;
PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB = NULL;
PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB = NULL;
PFNGLVERTEXATTRIB1SARBPROC glVertexAttrib1sARB = NULL;
PFNGLVERTEXATTRIB1SVARBPROC glVertexAttrib1svARB = NULL;
PFNGLVERTEXATTRIB2DARBPROC glVertexAttrib2dARB = NULL;
PFNGLVERTEXATTRIB2DVARBPROC glVertexAttrib2dvARB = NULL;
PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB = NULL;
PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB = NULL;
PFNGLVERTEXATTRIB2SARBPROC glVertexAttrib2sARB = NULL;
PFNGLVERTEXATTRIB2SVARBPROC glVertexAttrib2svARB = NULL;
PFNGLVERTEXATTRIB3DARBPROC glVertexAttrib3dARB = NULL;
PFNGLVERTEXATTRIB3DVARBPROC glVertexAttrib3dvARB = NULL;
PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB = NULL;
PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB = NULL;
PFNGLVERTEXATTRIB3SARBPROC glVertexAttrib3sARB = NULL;
PFNGLVERTEXATTRIB3SVARBPROC glVertexAttrib3svARB = NULL;
PFNGLVERTEXATTRIB4NBVARBPROC glVertexAttrib4NbvARB = NULL;
PFNGLVERTEXATTRIB4NIVARBPROC glVertexAttrib4NivARB= NULL;
PFNGLVERTEXATTRIB4NSVARBPROC glVertexAttrib4NsvARB= NULL;
PFNGLVERTEXATTRIB4NUBARBPROC glVertexAttrib4NubARB= NULL;
PFNGLVERTEXATTRIB4NUBVARBPROC glVertexAttrib4NubvARB = NULL;
PFNGLVERTEXATTRIB4NUIVARBPROC glVertexAttrib4NuivARB = NULL;
PFNGLVERTEXATTRIB4NUSVARBPROC glVertexAttrib4NusvARB = NULL;
PFNGLVERTEXATTRIB4BVARBPROC glVertexAttrib4bvARB = NULL;
PFNGLVERTEXATTRIB4DARBPROC glVertexAttrib4dARB = NULL;
PFNGLVERTEXATTRIB4DVARBPROC glVertexAttrib4dvARB = NULL;
PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB = NULL;
PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB = NULL;
PFNGLVERTEXATTRIB4IVARBPROC glVertexAttrib4ivARB = NULL;
PFNGLVERTEXATTRIB4SARBPROC glVertexAttrib4sARB = NULL;
PFNGLVERTEXATTRIB4SVARBPROC glVertexAttrib4svARB = NULL;
PFNGLVERTEXATTRIB4UBVARBPROC glVertexAttrib4ubvARB = NULL;
PFNGLVERTEXATTRIB4UIVARBPROC glVertexAttrib4uivARB = NULL;
PFNGLVERTEXATTRIB4USVARBPROC glVertexAttrib4usvARB = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB = NULL;
PFNGLPROGRAMSTRINGARBPROC glProgramStringARB = NULL;
PFNGLBINDPROGRAMARBPROC glBindProgramARB = NULL;
PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB = NULL;
PFNGLGENPROGRAMSARBPROC glGenProgramsARB = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLPROGRAMENVPARAMETER4DARBPROC glProgramEnvParameter4d = NULL;
PFNGLPROGRAMENVPARAMETER4DVARBPROC glProgramEnvParameter4dv = NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC glProgramEnvParameter4f = NULL;
PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fv = NULL;
PFNGLPROGRAMLOCALPARAMETER4DARBPROC glProgramLocalParameter4d = NULL;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC glProgramLocalParameter4dv = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4f = NULL;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fv = NULL;
PFNGLGETPROGRAMENVPARAMETERDVARBPROC glGetProgramEnvParameterdv = NULL;
PFNGLGETPROGRAMENVPARAMETERFVARBPROC glGetProgramEnvParameterfv = NULL;
PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glGetProgramLocalParameterdv = NULL;
PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glGetProgramLocalParameterfv = NULL;
PFNGLGETPROGRAMSTRINGARBPROC glGetProgramString = NULL;
PFNGLGETVERTEXATTRIBDVARBPROC glGetVertexAttribdv = NULL;
PFNGLGETVERTEXATTRIBFVARBPROC glGetVertexAttribfv = NULL;
PFNGLGETVERTEXATTRIBIVARBPROC glGetVertexAttribiv = NULL;
PFNGLGETVERTEXATTRIBPOINTERVARBPROC glGetVertexAttribPointerv = NULL;
PFNGLISPROGRAMARBPROC glIsProgram = NULL;

PFNGLDELETEOBJECTARBPROC glDeleteObject= NULL;
PFNGLGETHANDLEARBPROC glGetHandle= NULL;
PFNGLDETACHOBJECTARBPROC glDetachObject= NULL;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB= NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB= NULL;
PFNGLATTACHOBJECTARBPROC glAttachObject= NULL;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObject= NULL;
PFNGLVALIDATEPROGRAMARBPROC glValidateProgram= NULL;
PFNGLUNIFORM1FARBPROC glUniform1f= NULL;
PFNGLUNIFORM3FARBPROC glUniform3f= NULL;
PFNGLUNIFORM4FARBPROC glUniform4f= NULL;
PFNGLUNIFORM1IARBPROC glUniform1i= NULL;
PFNGLUNIFORM2IARBPROC glUniform2i= NULL;
PFNGLUNIFORM3IARBPROC glUniform3i= NULL;
PFNGLUNIFORM4IARBPROC glUniform4i= NULL;
PFNGLUNIFORM1FVARBPROC glUniform1fv= NULL;
PFNGLUNIFORM2FVARBPROC glUniform2fv= NULL;
PFNGLUNIFORM3FVARBPROC glUniform3fv= NULL;
PFNGLUNIFORM4FVARBPROC glUniform4fv= NULL;
PFNGLUNIFORM1IVARBPROC glUniform1iv= NULL;
PFNGLUNIFORM2IVARBPROC glUniform2iv= NULL;
PFNGLUNIFORM3IVARBPROC glUniform3iv= NULL;
PFNGLUNIFORM4IVARBPROC glUniform4iv= NULL;
PFNGLUNIFORMMATRIX2FVARBPROC glUniformMatrix2fv= NULL;
PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fv= NULL;
PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fv= NULL;
PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfv= NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameteriv= NULL;
PFNGLGETINFOLOGARBPROC glGetInfoLog= NULL;
PFNGLGETATTACHEDOBJECTSARBPROC glGetAttachedObjects= NULL;
PFNGLGETACTIVEUNIFORMARBPROC glGetActiveUniform= NULL;
PFNGLGETUNIFORMFVARBPROC glGetUniformfv= NULL;
PFNGLGETUNIFORMIVARBPROC glGetUniformiv= NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLGETSHADERSOURCEARBPROC glGetShaderSource= NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;

PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocation= NULL;
PFNGLGETACTIVEATTRIBARBPROC glGetActiveAttrib= NULL;

PFNGLBINDPROGRAMNVPROC glBindProgramNV= NULL;
PFNGLDELETEPROGRAMSNVPROC glDeleteProgramsNV= NULL;
PFNGLGENPROGRAMSNVPROC glGenProgramsNV= NULL;
PFNGLLOADPROGRAMNVPROC glLoadProgramNV= NULL;
PFNGLPROGRAMPARAMETER4FVNVPROC glProgramParameter4fvNV= NULL;
#endif

bool cgrShader::VP_supported = false;
bool cgrShader::FP_supported = false;
bool cgrShader::GLSL_supported = false;
bool cgrShader::NVFP_supported = false;
GLint cgrShader::nb_texture_unit = 0;

static bool gfglIsOpenGLExtensionSupported(const char* extension)
{
	const char *extensions, *start;
	const int len = strlen(extension);

	// TODO: Make sure there is a current window, and thus a current context available

	if (strchr(extension, ' '))
		return false;

	start = extensions = (const char *)glGetString(GL_EXTENSIONS);

	if (!extensions)
		return false;

	while (true)
	{
		const char *p = strstr(extensions, extension);
		if (!p)
			return 0;  // Not found

		// Check that the match isn't a super string
		if ((p == start || p[-1] == ' ') && (p[len] == ' ' || p[len] == 0))
			return true;

		// Skip the false match and continue
		extensions = p + len;
	}

	return false;
}

cgrShader::cgrShader(const char *name, const char *vertex, const char *fragment)
{
	program = 0;
	vertex_target = 0;
	vertex_id = 0;
	fragment_target = 0;
	fragment_id = 0;

	char *data;
	FILE *file = fopen(name,"rb");
	if(!file)
	{
		GfLogDebug("cgrShader::cgrShader(): can't open %s file\n", name);
		return;
	}

	fseek(file,0,SEEK_END);
	int size = ftell(file);
	data = new char[size + 1];
	data[size] = '\0';
	fseek(file,0,SEEK_SET);
	fread(data,1,size,file);
	fclose(file);

	// skip comments
	char *s = data;
	char *d = data;
	while(*s)
	{
		if(*s == '/' && *(s + 1) == '/')
		{
			while(*s && *s != '\n') s++;
			while(*s && *s == '\n') s++;
			*d++ = '\n';
		}
		else if(*s == '/' && *(s + 1) == '*')
		{
			while(*s && (*s != '*' || *(s + 1) != '/')) s++;
			s += 2;
			while(*s && *s == '\n') s++;
			*d++ = '\n';
		}
		else *d++ = *s++;
	}

	*d = '\0';

	// find shaders
	char *vertex_src = NULL;
	char *fragment_src = NULL;
	s = data;
	while(*s)
	{
		if(*s == '<')
		{
			char *name = s;
			while(*s)
			{
				if(strchr("> \t\n\r",*s)) break;
				s++;
			}

			if(*s == '>')
			{		// it`s shader
				*name++ = '\0';
				*s++ = '\0';
				while(*s && strchr(" \t\n\r",*s)) s++;
				if(vertex == NULL && !strcmp(name,"vertex")) vertex_src = s;
				if(vertex && !strcmp(name,vertex)) vertex_src = s;
				if(fragment == NULL && !strcmp(name,"fragment")) fragment_src = s;
				if(fragment && !strcmp(name,fragment)) fragment_src = s;
			}
		}

		s++;
	}

	if(vertex_src)
	{
		// ARB vertex program
		if(VP_supported && !strncmp(vertex_src,"!!ARBvp1.0", 10))
		{
			vertex_target = GL_VERTEX_PROGRAM_ARB;
			glGenProgramsARB(1, &vertex_id);
			glBindProgramARB(GL_VERTEX_PROGRAM_ARB,vertex_id);
			glProgramStringARB(GL_VERTEX_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,(GLsizei)strlen(vertex_src),vertex_src);
			GLint pos = -1;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB,&pos);

			if(pos != -1)
			{
				GfLogDebug("cgrShader::cgrShader(): vertex program error in %s file\n", name);
				return;
			}

			char *var = strstr(vertex_src, "#var ");
			while( var )
			{
				char *eol = strchr( var + 1, '#');
				char *c2, *c3, *c4;
				c2 = strchr( var + 6, ' ');
				if( c2 )
				{
					c3 = strchr( c2 + 1, ':');
					if( c3 )
					{
						c4 = strchr( c3 + 1, ':');

						if( c4 )
							c4 = strchr( c4 + 1, '[');

						if( c4 && c4 < eol)
						{
							char type[10], name[30];
							strncpy( type, var + 5, c2-var-5 );
							type[c2-var-5] = 0;
							strncpy( name, c2 + 1, c3-c2-2 );
							name[c3-c2-2] = 0;
							struct Parameter p;
							p.location = atoi( c4 + 1);
							p.length = 4;
							if( ! strcmp(type, "float3") )
								p.length = 3;
							else if( ! strcmp(type, "float") )
								p.length = 1;
							arb_parameters[ name ] = p;
						}
					}
				}

				var = strstr(var + 1, "#var ");
			}
		}
		// ARB vertex shader
		else
		{
			program = glCreateProgramObjectARB();

			GLint length = (GLint)strlen(vertex_src);
			GLhandleARB vertex = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
			glShaderSource(vertex,1,(const GLcharARB**)&vertex_src, &length);
			glCompileShader(vertex);
			glAttachShader(program, vertex);
			glDeleteShader(vertex);

			glBindAttribLocation(program, 0, "s_attribute_0");
			glBindAttribLocation(program, 1, "s_attribute_1");
			glBindAttribLocation(program, 2, "s_attribute_2");
			glBindAttribLocation(program, 3, "s_attribute_3");
			glBindAttribLocation(program, 4, "s_attribute_4");
			glBindAttribLocation(program, 5, "s_attribute_5");
			glBindAttribLocation(program, 6, "s_attribute_6");

			glBindAttribLocation(program, 0, "s_xyz");
			glBindAttribLocation(program, 1, "s_normal");
			glBindAttribLocation(program, 2, "s_tangent");
			glBindAttribLocation(program, 3, "s_binormal");
			glBindAttribLocation(program, 4, "s_texcoord");
		}
	}

	if(fragment_src)
	{
		// ARB fragment program
		if(FP_supported && !strncmp(fragment_src, "!!ARBfp1.0", 10))
		{
			fragment_target = GL_FRAGMENT_PROGRAM_ARB;
			glGenProgramsARB(1,&fragment_id);
			glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,fragment_id);
			glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei)strlen(fragment_src), fragment_src);
			GLint pos = -1;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &pos);

			if(pos != -1)
			{
				GfLogDebug("cgrShader::cgrShader(): fragment program error in %s file\n", name);
				return;
			}
		}

		// NV fragment program
		else if(!strncmp(fragment_src, "!!FP1.0", 7))
		{
			fragment_target = GL_FRAGMENT_PROGRAM_NV;
			glGenProgramsNV(1, &fragment_id);
			glBindProgramNV(GL_FRAGMENT_PROGRAM_NV, fragment_id);
			glLoadProgramNV(GL_FRAGMENT_PROGRAM_NV, fragment_id, (GLsizei)strlen(fragment_src), (GLubyte*)fragment_src);
			GLint pos = -1;
			glGetIntegerv(GL_PROGRAM_ERROR_POSITION_NV, &pos);
			if(pos != -1)
			{
				GfLogDebug("cgrShader::cgrShader(): fragment program error in %s file\n", name);
				return;
			}
		}

		// ARB fragment shader
		else
		{
			if(!program) program = glCreateProgramObjectARB();

			GLint length = (GLint)strlen(fragment_src);
			GLhandleARB fragment = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
			glShaderSource(fragment, 1, (const GLcharARB**)&fragment_src, &length);
			glCompileShader(fragment);
			glAttachShader(program, fragment);
			glDeleteShader(fragment);
		}
	}

	if(program)
	{
		glLinkProgram(program);
		GLint linked;
		glGetUniformiv(program,GL_OBJECT_LINK_STATUS_ARB, &linked);
		if(!linked)
		{
			GfLogDebug("cgrShader::cgrShader(): GLSL error in %s file\n", name);
			return;
		}

		glUseProgram(program);

		for(int i = 0; i < 8; i++)
		{
			char texture[32];
			snprintf(texture, 32, "s_texture_%d", i);
			GLint location = glGetUniformLocation(program, texture);
			if(location >= 0) glUniform1i(location, i);
		}

		glUseProgram(0);

		glValidateProgram(program);
		GLint validated;
		glGetUniformiv(program, GL_OBJECT_VALIDATE_STATUS_ARB, &validated);
		if(!validated)
		{
			GfLogDebug("cgrShader::cgrShader(): GLSL error in %s file\n", name);
			return;
		}
	}

	delete [] data;
}

cgrShader::~cgrShader()
{
	if(program) glDeleteShader(program);
	if(vertex_target == GL_VERTEX_PROGRAM_ARB) glDeleteProgramsARB(1, &vertex_id);
	if(fragment_target == GL_FRAGMENT_PROGRAM_ARB) glDeleteProgramsARB(1, &fragment_id);
	else if(fragment_target == GL_FRAGMENT_PROGRAM_NV) glDeleteProgramsNV(1, &fragment_id);
	parameters.clear();
}

const char *cgrShader::get_error(char *data,int pos)
{
	char *s = data;
	while(*s && pos--) s++;
	while(s >= data && *s != '\n') s--;
	char *e = ++s;
	while(*e != '\0' && *e != '\n') e++;
	*e = '\0';

	return s;
}

const char *cgrShader::get_glsl_error()
{
	GLint length;
	static char error[4096];
	glGetProgramInfoLog(program, sizeof(error), &length,error);
	return error;
}

void cgrShader::getParameter(const char *name, Parameter *parameter)
{
	if( program )
	{
		char buf[1024];
		strcpy(buf,name);
		char *s = strchr(buf,':');
		if(s)
		{
			*s++ = '\0';
			parameter->length = atoi(s);
		}
		else
		{
			parameter->length = 4;
		}

		parameter->location = glGetUniformLocation(program, buf);
	}
	else if( vertex_id )
	{
		arb_parameter_list::iterator iParam = arb_parameters.find(name);
		if( iParam != arb_parameters.end() )
			parameter->location = iParam->second.location;
		else
			parameter->location = 90;

		parameter->length = 4;
	}
}

void cgrShader::bindNames(const char *name,...)
{
	Parameter parameter;
	getParameter(name, &parameter);
	parameters.push_back(parameter);
	va_list args;
	va_start(args,name);
	while(1)
	{
		const char *name = va_arg(args, const char*);
		if(name == NULL) break;
		getParameter(name, &parameter);
		parameters.push_back(parameter);
	}

	va_end(args);
}

/*****************************************************************************/
/*                                                                           */
/* enable/disable/bind                                                       */
/*                                                                           */
/*****************************************************************************/
void cgrShader::enable()
{
	if(vertex_id) glEnable(vertex_target);
	if(fragment_id) glEnable(fragment_target);
}

void cgrShader::disable()
{
	if(program) glUseProgram(0);
	if(vertex_id) glDisable(vertex_target);
	if(fragment_id) glDisable(fragment_target);
}

void cgrShader::bind()
{
	if(program) glUseProgram(program);
	if(vertex_id)
	{
		if(vertex_target == GL_VERTEX_PROGRAM_ARB)
			glBindProgramARB(vertex_target,vertex_id);
	}

	if(fragment_id)
	{
		if(fragment_target == GL_FRAGMENT_PROGRAM_ARB)
			glBindProgramARB(fragment_target,fragment_id);
		else if(fragment_target == GL_FRAGMENT_PROGRAM_NV)
			glBindProgramNV(fragment_target,fragment_id);
	}
}

void cgrShader::bind(const float *v,...)
{
	if(fragment_id)
	{
		if(fragment_target == GL_FRAGMENT_PROGRAM_ARB)
			glBindProgramARB(fragment_target,fragment_id);
		else if(fragment_target == GL_FRAGMENT_PROGRAM_NV)
			glBindProgramNV(fragment_target,fragment_id);
	}
	else
	{
		if(program == 0)
		{
			GfLogDebug("cgrShader::bind(): error GLSL shader isn't loaded\n");
			return;
		}

		glUseProgram(program);
	}

	const float *value = v;
	va_list args;
	va_start(args,v);

	for(int i = 0; i < (int)parameters.size(); i++)
	{
		if( vertex_target )
		{
			glUniform4fv( vertex_target, parameters[i].location, value);
		}
		else if( program )
		{
			if(parameters[i].length == 1) glUniform1fv(parameters[i].location,1,value);
			else if(parameters[i].length == 2) glUniform2fv(parameters[i].location,1,value);
			else if(parameters[i].length == 3) glUniform3fv(parameters[i].location,1,value);
			else if(parameters[i].length == 4) glUniform4fv(parameters[i].location,1,value);
			else if(parameters[i].length == 9) glUniformMatrix3fv(parameters[i].location, 1, false, value);
			else if(parameters[i].length == 16) glUniformMatrix4fv(parameters[i].location, 1, false, value);
		}
		value = va_arg(args,const float*);
		if(!value) break;
	}

	va_end(args);
}

/*****************************************************************************/
/*                                                                           */
/* set parameters                                                            */
/*                                                                           */
/*****************************************************************************/

void cgrShader::setLocalParameter(int location,const float *value)
{
	if(vertex_target == 0)
	{
		GfLogDebug("cgrShader::setLocalParameter(): error vertex program isn't loaded\n");
		return;
	}

	glUniform4fv(vertex_target, location, value);
}

void cgrShader::setEnvParameter(int location,const float *value)
{
	if(vertex_target == 0)
	{
		GfLogDebug("cgrShader::setEnvParameter(): error vertex program isn't loaded\n");
		return;
	}

	glUniform4fv(vertex_target,location,value);
}

void cgrShader::setParameter(const char *name,const float *value)
{
	Parameter parameter;
	getParameter(name, &parameter);

	if( vertex_target )
	{
		glUniform4fv( vertex_target, parameter.location, value);
		return;
	}

	if(program == 0)
	{
		GfLogDebug("cgrShader::setLocalParameter(): error GLSL shader isn't loaded\n");
		return;
	}

	if(parameter.length == 1) glUniform1fv(parameter.location,1,value);
	else if(parameter.length == 2) glUniform2fv(parameter.location,1,value);
	else if(parameter.length == 3) glUniform3fv(parameter.location,1,value);
	else if(parameter.length == 4) glUniform4fv(parameter.location,1,value);
	else if(parameter.length == 9) glUniformMatrix3fv(parameter.location, 1, false, value);
	else if(parameter.length == 16) glUniformMatrix4fv(parameter.location, 1, false, value);
}

void cgrShader::setParameters(const float *v,...)
{
	const float *value = v;
	va_list args;
	va_start(args,v);

	for(int i = 0; i < (int)parameters.size(); i++)
	{
		if( vertex_target )
		{
			glUniform4fv( vertex_target, parameters[i].location, value);
		}
		else if( program )
		{
			if(parameters[i].length == 1) glUniform1fv(parameters[i].location, 1, value);
			else if(parameters[i].length == 2) glUniform2fv(parameters[i].location, 1, value);
			else if(parameters[i].length == 3) glUniform3fv(parameters[i].location, 1, value);
			else if(parameters[i].length == 4) glUniform4fv(parameters[i].location, 1, value);
			else if(parameters[i].length == 9) glUniformMatrix3fv(parameters[i].location, 1, false, value);
			else if(parameters[i].length == 16) glUniformMatrix4fv(parameters[i].location, 1, false, value);
		}
		value = va_arg(args, const float*);
		if(!value) break;
	}
	va_end(args);
}

#ifndef CONCAT
#define CONCAT(a,b) a##b
#endif
#define mystringify(a) CONCAT(ST,R)(a)
#define STR(x) #x
#define LOAD_EXT(fn) CONCAT(fn,Ptr) = (CONCAT(fn,Proc)) SDLookupFunction( mystringify(CONCAT(fn, ARB)) )

void cgrShader::Init(void)
{
	if( gfglIsOpenGLExtensionSupported("GL_ARB_multitexture") )
		glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &nb_texture_unit );

	VP_supported = gfglIsOpenGLExtensionSupported("GL_ARB_vertex_program");
	FP_supported = gfglIsOpenGLExtensionSupported("GL_ARB_fragment_program");
	// check that
	GLSL_supported = gfglIsOpenGLExtensionSupported("GL_ARB_shading_language_100") &&
	gfglIsOpenGLExtensionSupported("GL_ARB_fragment_shader") &&
	gfglIsOpenGLExtensionSupported("GL_ARB_vertex_shader") &&
	gfglIsOpenGLExtensionSupported("GL_ARB_shader_objects");
	NVFP_supported = gfglIsOpenGLExtensionSupported("GL_NV_fragment_program");

	if( VP_supported || FP_supported )
	{
#ifdef WIN32
		/* All ARB_fragment_program entry points are shared with ARB_vertex_program. */
		glVertexAttrib1dARB  = (PFNGLVERTEXATTRIB1DARBPROC)wglGetProcAddress("glVertexAttrib1dARB");
		glVertexAttrib1dvARB = (PFNGLVERTEXATTRIB1DVARBPROC)wglGetProcAddress("glVertexAttrib1dvARB");
		glVertexAttrib1fARB  = (PFNGLVERTEXATTRIB1FARBPROC)wglGetProcAddress("glVertexAttrib1fARB");
		glVertexAttrib1fvARB = (PFNGLVERTEXATTRIB1FVARBPROC)wglGetProcAddress("glVertexAttrib1fvARB");
		/*LOAD_EXT( glVertexAttrib1s );
		LOAD_EXT( glVertexAttrib1sv );
		LOAD_EXT( glVertexAttrib2d );
		LOAD_EXT( glVertexAttrib2dv );
		LOAD_EXT( glVertexAttrib2f );
		LOAD_EXT( glVertexAttrib2fv );
		LOAD_EXT( glVertexAttrib2s );
		LOAD_EXT( glVertexAttrib2sv );
		LOAD_EXT( glVertexAttrib3d );
		LOAD_EXT( glVertexAttrib3dv );
		LOAD_EXT( glVertexAttrib3f );
		LOAD_EXT( glVertexAttrib3fv );
		LOAD_EXT( glVertexAttrib3s );
		LOAD_EXT( glVertexAttrib3sv );
		LOAD_EXT( glVertexAttrib4Nbv );
		LOAD_EXT( glVertexAttrib4Niv );
		LOAD_EXT( glVertexAttrib4Nsv );
		LOAD_EXT( glVertexAttrib4Nub );
		LOAD_EXT( glVertexAttrib4Nubv );
		LOAD_EXT( glVertexAttrib4Nuiv );
		LOAD_EXT( glVertexAttrib4Nusv );
		LOAD_EXT( glVertexAttrib4bv );
		LOAD_EXT( glVertexAttrib4d );
		LOAD_EXT( glVertexAttrib4dv );
		LOAD_EXT( glVertexAttrib4f );
		LOAD_EXT( glVertexAttrib4fv );
		LOAD_EXT( glVertexAttrib4iv );
		LOAD_EXT( glVertexAttrib4s );
		LOAD_EXT( glVertexAttrib4sv );
		LOAD_EXT( glVertexAttrib4ubv );
		LOAD_EXT( glVertexAttrib4uiv );
		LOAD_EXT( glVertexAttrib4usv );
		LOAD_EXT( glVertexAttribPointer );
		LOAD_EXT( glEnableVertexAttribArray );
		LOAD_EXT( glDisableVertexAttribArray );*/
		//glProgramString =);
		glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)wglGetProcAddress("glBindProgramARB");
		glDeleteProgramsARB = (PFNGLDELETEPROGRAMSARBPROC)wglGetProcAddress("glDeleteProgramsARB");
		/*LOAD_EXT( glGenPrograms );
		LOAD_EXT( glProgramEnvParameter4d );
		LOAD_EXT( glProgramEnvParameter4dv );
		LOAD_EXT( glProgramEnvParameter4f );
		LOAD_EXT( glProgramEnvParameter4fv );
		LOAD_EXT( glProgramLocalParameter4d );
		LOAD_EXT( glProgramLocalParameter4dv );
		LOAD_EXT( glProgramLocalParameter4f );
		LOAD_EXT( glProgramLocalParameter4fv );
		LOAD_EXT( glGetProgramEnvParameterdv );
		LOAD_EXT( glGetProgramEnvParameterfv );
		LOAD_EXT( glGetProgramLocalParameterdv );
		LOAD_EXT( glGetProgramLocalParameterfv );
		LOAD_EXT( glGetProgramiv );
		LOAD_EXT( glGetProgramString );
		LOAD_EXT( glGetVertexAttribdv );
		LOAD_EXT( glGetVertexAttribfv );
		LOAD_EXT( glGetVertexAttribiv );
		LOAD_EXT( glGetVertexAttribPointerv );
		LOAD_EXT( glIsProgram );
		}

		if( GLSL_supported )
		{
		LOAD_EXT( glDeleteObject );
		LOAD_EXT( glGetHandle );
		LOAD_EXT( glDetachObject );
		LOAD_EXT( glCreateShaderObject );
		LOAD_EXT( glShaderSource );
		LOAD_EXT( glCompileShader );
		LOAD_EXT( glCreateProgramObject );
		LOAD_EXT( glAttachObject );
		LOAD_EXT( glLinkProgram );
		LOAD_EXT( glUseProgramObject );
		LOAD_EXT( glValidateProgram );
		LOAD_EXT( glUniform1f );
		LOAD_EXT( glUniform2f );
		LOAD_EXT( glUniform3f );
		LOAD_EXT( glUniform4f );
		LOAD_EXT( glUniform1i );
		LOAD_EXT( glUniform2i );
		LOAD_EXT( glUniform3i );
		LOAD_EXT( glUniform4i );
		LOAD_EXT( glUniform1fv );
		LOAD_EXT( glUniform2fv );
		LOAD_EXT( glUniform3fv );
		LOAD_EXT( glUniform4fv );
		LOAD_EXT( glUniform1iv );
		LOAD_EXT( glUniform2iv );
		LOAD_EXT( glUniform3iv );
		LOAD_EXT( glUniform4iv );
		LOAD_EXT( glUniformMatrix2fv );
		LOAD_EXT( glUniformMatrix3fv );
		LOAD_EXT( glUniformMatrix4fv );
		LOAD_EXT( glGetObjectParameterfv );
		LOAD_EXT( glGetObjectParameteriv );
		LOAD_EXT( glGetInfoLog );
		LOAD_EXT( glGetAttachedObjects );
		LOAD_EXT( glGetUniformLocation );
		LOAD_EXT( glGetActiveUniform );
		LOAD_EXT( glGetUniformfv );
		LOAD_EXT( glGetUniformiv );
		LOAD_EXT( glGetShaderSource );

		LOAD_EXT( glBindAttribLocation );
		LOAD_EXT( glGetActiveAttrib );
		LOAD_EXT( glGetAttribLocation );

		}

		if( NVFP_supported )
		{
		glBindProgramNVPtr = (glBindProgramNVProc) SDLookupFunction( "glBindProgramNV" );
		glDeleteProgramsNVPtr = (glDeleteProgramsNVProc) SDLookupFunction( "glDeleteProgramsNV" );
		glGenProgramsNVPtr = (glGenProgramsNVProc) SDLookupFunction( "glGenProgramsNV" );
		glLoadProgramNVPtr = (glLoadProgramNVProc) SDLookupFunction( "glLoadProgramNV" );
		glProgramParameter4fvNVPtr = (glProgramParameter4fvNVProc) SDLookupFunction( "glProgramParameter4fvNV" );
		}*/
#endif
	}
}
