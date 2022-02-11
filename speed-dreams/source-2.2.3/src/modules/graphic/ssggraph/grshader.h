/***************************************************************************

    file                 : grshader.h
    created              : Fri Oct 10 23:16:44 CET 2015
    copyright            : (C) 2015 by Xavier Bertaux
    version              : $Id: grshader.h 4374 2015-10-10 23:20:37Z torcs-ng $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __GRSHADER_H
#define __GRSHADER_H

#include <plib/ssg.h>
#include "grmain.h"

#include <portability.h>

#ifdef WIN32
#  include <windows.h>

extern PFNGLVERTEXATTRIB1DARBPROC glVertexAttrib1dARB;
extern PFNGLVERTEXATTRIB1DVARBPROC glVertexAttrib1dvARB;
extern PFNGLVERTEXATTRIB1FARBPROC glVertexAttrib1fARB;
extern PFNGLVERTEXATTRIB1FVARBPROC glVertexAttrib1fvARB;
extern PFNGLVERTEXATTRIB1SARBPROC glVertexAttrib1sARB;
extern PFNGLVERTEXATTRIB1SVARBPROC glVertexAttrib1svARB;
extern PFNGLVERTEXATTRIB2DARBPROC glVertexAttrib2dARB;
extern PFNGLVERTEXATTRIB2DVARBPROC glVertexAttrib2dvARB;
extern PFNGLVERTEXATTRIB2FARBPROC glVertexAttrib2fARB;
extern PFNGLVERTEXATTRIB2FVARBPROC glVertexAttrib2fvARB;
extern PFNGLVERTEXATTRIB2SARBPROC glVertexAttrib2sARB;
extern PFNGLVERTEXATTRIB2SVARBPROC glVertexAttrib2svARB;
extern PFNGLVERTEXATTRIB3DARBPROC glVertexAttrib3dARB;
extern PFNGLVERTEXATTRIB3DVARBPROC glVertexAttrib3dvARB;
extern PFNGLVERTEXATTRIB3FARBPROC glVertexAttrib3fARB;
extern PFNGLVERTEXATTRIB3FVARBPROC glVertexAttrib3fvARB;
extern PFNGLVERTEXATTRIB3SARBPROC glVertexAttrib3sARB;
extern PFNGLVERTEXATTRIB3SVARBPROC glVertexAttrib3svARB;
extern PFNGLVERTEXATTRIB4NBVARBPROC glVertexAttrib4NbvARB;
extern PFNGLVERTEXATTRIB4NIVARBPROC glVertexAttrib4NivARB;
extern PFNGLVERTEXATTRIB4NSVARBPROC glVertexAttrib4NsvARB;
extern PFNGLVERTEXATTRIB4NUBARBPROC glVertexAttrib4NubARB;
extern PFNGLVERTEXATTRIB4NUBVARBPROC glVertexAttrib4NubvARB;
extern PFNGLVERTEXATTRIB4NUIVARBPROC glVertexAttrib4NuivARB;
extern PFNGLVERTEXATTRIB4NUSVARBPROC glVertexAttrib4NusvARB;
extern PFNGLVERTEXATTRIB4BVARBPROC glVertexAttrib4bvARB;
extern PFNGLVERTEXATTRIB4DARBPROC glVertexAttrib4dARB;
extern PFNGLVERTEXATTRIB4DVARBPROC glVertexAttrib4dvARB;
extern PFNGLVERTEXATTRIB4FARBPROC glVertexAttrib4fARB;
extern PFNGLVERTEXATTRIB4FVARBPROC glVertexAttrib4fvARB;
extern PFNGLVERTEXATTRIB4IVARBPROC glVertexAttrib4ivARB;
extern PFNGLVERTEXATTRIB4SARBPROC glVertexAttrib4sARB;
extern PFNGLVERTEXATTRIB4SVARBPROC glVertexAttrib4svARB;
extern PFNGLVERTEXATTRIB4UBVARBPROC glVertexAttrib4ubvARB;
extern PFNGLVERTEXATTRIB4UIVARBPROC glVertexAttrib4uivARB;
extern PFNGLVERTEXATTRIB4USVARBPROC glVertexAttrib4usvARB;
extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB;
extern PFNGLPROGRAMSTRINGARBPROC glProgramStringARB;
extern PFNGLBINDPROGRAMARBPROC glBindProgramARB;
extern PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB;
extern PFNGLGENPROGRAMSARBPROC glGenProgramsARB;
extern PFNGLPROGRAMENVPARAMETER4DARBPROC glProgramEnvParameter4d;
extern PFNGLPROGRAMENVPARAMETER4DVARBPROC glProgramEnvParameter4dv;
extern PFNGLPROGRAMENVPARAMETER4FARBPROC glProgramEnvParameter4f;
extern PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fv;
extern PFNGLPROGRAMLOCALPARAMETER4DARBPROC glProgramLocalParameter4d;
extern PFNGLPROGRAMLOCALPARAMETER4DVARBPROC glProgramLocalParameter4dv;
extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4f;
extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fv;
extern PFNGLGETPROGRAMENVPARAMETERDVARBPROC glGetProgramEnvParameterdv;
extern PFNGLGETPROGRAMENVPARAMETERFVARBPROC glGetProgramEnvParameterfv;
extern PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC glGetProgramLocalParameterdv;
extern PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC glGetProgramLocalParameterfv;
extern PFNGLGETPROGRAMSTRINGARBPROC glGetProgramString;
extern PFNGLGETVERTEXATTRIBDVARBPROC glGetVertexAttribdv;
extern PFNGLGETVERTEXATTRIBFVARBPROC glGetVertexAttribfv;
extern PFNGLGETVERTEXATTRIBIVARBPROC glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVARBPROC glGetVertexAttribPointerv;
extern PFNGLISPROGRAMARBPROC glIsProgram;

extern PFNGLDELETEOBJECTARBPROC glDeleteObject;
extern PFNGLGETHANDLEARBPROC glGetHandle;
extern PFNGLDETACHOBJECTARBPROC glDetachObject;
extern PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
extern PFNGLATTACHOBJECTARBPROC glAttachObject;
extern PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObject;
extern PFNGLVALIDATEPROGRAMARBPROC glValidateProgram;
extern PFNGLUNIFORM1FARBPROC glUniform1f;
extern PFNGLUNIFORM3FARBPROC glUniform3f;
extern PFNGLUNIFORM4FARBPROC glUniform4f;
extern PFNGLUNIFORM1IARBPROC glUniform1i;
extern PFNGLUNIFORM2IARBPROC glUniform2i;
extern PFNGLUNIFORM3IARBPROC glUniform3i;
extern PFNGLUNIFORM4IARBPROC glUniform4i;
extern PFNGLUNIFORM1FVARBPROC glUniform1fv;
extern PFNGLUNIFORM2FVARBPROC glUniform2fv;
extern PFNGLUNIFORM3FVARBPROC glUniform3fv;
extern PFNGLUNIFORM4FVARBPROC glUniform4fv;
extern PFNGLUNIFORM1IVARBPROC glUniform1iv;
extern PFNGLUNIFORM2IVARBPROC glUniform2iv;
extern PFNGLUNIFORM3IVARBPROC glUniform3iv;
extern PFNGLUNIFORM4IVARBPROC glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVARBPROC glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fv;
extern PFNGLGETOBJECTPARAMETERFVARBPROC glGetObjectParameterfv;
extern PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameteriv;
extern PFNGLGETINFOLOGARBPROC glGetInfoLog;
extern PFNGLGETATTACHEDOBJECTSARBPROC glGetAttachedObjects;
extern PFNGLGETACTIVEUNIFORMARBPROC glGetActiveUniform;
extern PFNGLGETUNIFORMFVARBPROC glGetUniformfv;
extern PFNGLGETUNIFORMIVARBPROC glGetUniformiv;
extern PFNGLGETSHADERSOURCEARBPROC glGetShaderSource;

extern PFNGLBINDATTRIBLOCATIONARBPROC glBindAttribLocation;
extern PFNGLGETACTIVEATTRIBARBPROC glGetActiveAttrib;

extern PFNGLBINDPROGRAMNVPROC glBindProgramNV;
extern PFNGLDELETEPROGRAMSNVPROC glDeleteProgramsNV;
extern PFNGLGENPROGRAMSNVPROC glGenProgramsNV;
extern PFNGLLOADPROGRAMNVPROC glLoadProgramNV;
extern PFNGLPROGRAMPARAMETER4FVNVPROC glProgramParameter4fvNV;
#endif //WIN32

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <OpenGL/glext.h>
#else //__APPLE__
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glext.h>
#endif

#include <vector>
#include <map>

class cgrShader
{
public:

	cgrShader(const char *name, const char *vertex = NULL, const char *fragment = NULL);
	~cgrShader();

	void bindNames(const char *name,...);

	void enable();
	void disable();
	void bind();
	void bind(const float *value,...);

	void setLocalParameter(int location,const float *value);
	void setEnvParameter(int location,const float *value);

	void setParameter(const char *name,const float *value);
	void setParameters(const float *value,...);

	static void Init(void);
	inline static bool is_VP_supported(void) {return VP_supported;}
	inline static bool is_FP_supported(void) {return FP_supported;}
	inline static bool is_GLSL_supported(void) {return GLSL_supported;}
	inline static bool is_NVFP_supported(void) {return NVFP_supported;}
	inline static int get_nb_texture_units(void) {return nb_texture_unit;}

protected:
	struct Parameter
	{
		GLuint location;
		int length;
	};

	const char *get_error(char *data, int pos);
	const char *get_glsl_error();

	void getParameter(const char *name, Parameter *parameter);

	GLhandleARB program;

	GLuint vertex_target;
	GLuint vertex_id;

	GLuint fragment_target;
	GLuint fragment_id;

	std::vector<Parameter> parameters;
	typedef std::map<std::string, struct Parameter> arb_parameter_list;
	arb_parameter_list arb_parameters;

	static bool VP_supported, FP_supported, GLSL_supported, NVFP_supported;
	static GLint nb_texture_unit;
};

#endif /* __SHADER_H__ */
