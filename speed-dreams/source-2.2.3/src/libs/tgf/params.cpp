/***************************************************************************
                 params.cpp -- configuration parameters management
                             -------------------                                         
    created              : Fri Aug 13 22:27:57 CEST 1999
    copyright            : (C) 1999 by Eric Espie
    email                : torcs@free.fr   
    version              : $Id: params.cpp 7162 2020-07-31 20:50:12Z iobyte $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** @file
    		This is the parameters manipulation API.
    @author	<a href=mailto:torcs@free.fr>Eric Espie</a>
    @version	$Id: params.cpp 7162 2020-07-31 20:50:12Z iobyte $
    @ingroup	params
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include <cmath>
#include <ctime>
#include <sys/stat.h>
#include <float.h>

#ifdef WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifdef THIRD_PARTY_EXPAT
#include <expat.h>
#else
#include <xmlparse.h>
#endif

#include <portability.h>
#include "tgf.h"


#define LINE_SZ		1024

#define PARAM_CREATE	0x01

struct within
{
    char				*val;
    GF_TAILQ_ENTRY (struct within)	linkWithin;
};

GF_TAILQ_HEAD (withinHead, struct within);

#define P_NUM 0
#define P_STR 1
#define P_VAR 2  /* Variable */
#define P_FORM 3 /* Formula */

/** Parameter header structure */
struct param
{
    char				*name;		/**< Name of the parameter  */
    char				*fullName;	/**< Name of the parameter including the full section name ('/' separated) */
    char				*value;		/**< Value of the parameter */
    tdble				valnum;
    void				*formula;
    int					type;
    char				*unit;		/* for output only */
    tdble				min;
    tdble				max;
    struct withinHead			withinList;
    GF_TAILQ_ENTRY (struct param)	linkParam;	/**< Next parameter in the same section */
};

GF_TAILQ_HEAD (paramHead, struct param);
struct section;
GF_TAILQ_HEAD (sectionHead, struct section);


/** Section header structure */
struct section
{
    char *fullName;	/**< Name of the section including full path ('/' separated) */
    struct paramHead			paramList;	/**< List of the parameters of this section */
    GF_TAILQ_ENTRY (struct section)	linkSection;	/**< Next section at the same level */
    struct sectionHead			subSectionList;	/**< List of sub-sections (linked by linkSection)*/
    struct section			*curSubSection;
    struct section			*parent;	/**< Upper section */
};


#define PARM_MAGIC	0x20030815

/** Configuration header structure */
struct parmHeader
{
    char				*filename;	/**< Name of the configuration file */
    char				*name;		/**< Name of the data */
    char				*dtd;		/**< Optional DTD location */
    char				*header;	/**< Optional header (comment, xsl...) */
    int					refcount;	/**< Use counter (number of conf handle) */
    struct section			*rootSection;	/**< List of sections at the first level */
    void				*paramHash;	/**< Hash table for parameter access */
    void				*sectionHash;	/**< Hash table for section access */
    int					major;          /**< Major version number of the file */
    int					minor;          /**< Minor version number of the file */
    void                                *variableHash;  /**< Hash table for variables used in functions */
};

#define PARM_HANDLE_FLAG_PRIVATE	0x01
#define PARM_HANDLE_FLAG_PARSE_ERROR	0x02


/** Ouput control structure */
struct parmOutput
{
    int			state;
    struct section	*curSection;
    struct param	*curParam;
    char		*filename;	/**< Name of the output configuration file */
    char		*indent;
};


/** Configuration handle structure */
struct parmHandle
{
	int magic;
	struct parmHeader *conf;
	char *val;
	int flag;
	XML_Parser parser;
	struct section *curSection;
	struct parmOutput outCtrl;
	GF_TAILQ_ENTRY (struct parmHandle)	linkHandle;	/**< Next configuration handle */
};

/** Trace == true: Switches loggers on **/
static bool TraceLoggersAvailable = true;

GF_TAILQ_HEAD (parmHead, struct parmHandle);

static struct parmHead	parmHandleList;

static char *getFullName(const char *sectionName, const char *paramName);
static struct param *getParamByName (struct parmHeader *conf, const char *sectionName, const char *paramName, int flag);
static void removeParamByName (struct parmHeader *conf, const char *sectionName, const char *paramName);
static void removeParam (struct parmHeader *conf, struct section *section, struct param *param);
static struct param *addParam (struct parmHeader *conf, struct section *section, const char *paramName, const char *value);
static void removeSection (struct parmHeader *conf, struct section *section);
static struct section *addSection (struct parmHeader *conf, const char *sectionName);
static void parmClean (struct parmHeader *conf);
static void parmReleaseHandle (struct parmHandle *parmHandle);
static void parmReleaseHeader (struct parmHeader *conf);
static struct section *getParent (struct parmHeader *conf, const char *sectionName);
static void cleanUnusedSection (struct parmHeader *conf, struct section *section);
static FILE *safeFOpen(const char *fileName, const char *mode);


/** Configuration initialization.
    @ingroup	conf
    @return	none.
*/
void
gfParamInit (void)
{
    GF_TAILQ_INIT (&parmHandleList);
}

/** Configuration shutdown.
    @ingroup	conf
    @return	none
*/
void
GfParmShutdown (void)
{
    struct parmHandle	*parmHandle;

    while ((parmHandle = GF_TAILQ_FIRST (&parmHandleList)) != GF_TAILQ_END (&parmHandleList)) {
        GfLogError("GfParmShutdown: %s not released\n",
            parmHandle->conf->filename ? parmHandle->conf->filename : parmHandle->conf->name);
        parmReleaseHandle (parmHandle);
    }
}

/* Compute parameter full name           */
/* Caller must release the returned name */
static char *
getFullName (const char *sectionName, const char *paramName)
{
	char *fullName;
	
	fullName = (char *) malloc (strlen (sectionName) + strlen (paramName) + 2);
	if (!fullName) {
#if defined(_MSC_VER) && _MSC_VER < 1800
		GfLogError ("getFullName: malloc (%03Iu) failed", strlen (sectionName) + strlen (paramName) + 2);
#else //_MSC_VER
		GfLogError ("getFullName: malloc (%zu) failed", strlen (sectionName) + strlen (paramName) + 2);
#endif //_MSC_VER
		return NULL;
	}
	sprintf (fullName, "%s/%s", sectionName, paramName);
	
	return fullName;
}

/* Get a parameter by section/param names */
static struct param *
getParamByName (struct parmHeader *conf, const char *sectionName, const char *paramName, int flag)
{
	char *fullName;
	struct param *param;
	struct section *section;

	fullName = getFullName (sectionName, paramName);
	if (!fullName) {
		GfLogError ("getParamByName: getFullName failed\n");
		return NULL;
	}

	param = (struct param *)GfHashGetStr (conf->paramHash, fullName);
	free (fullName);
	if (param || ((flag & PARAM_CREATE) == 0)) {
		return param;
	}

	/* Parameter not found CREATE it */
	section = (struct section *)GfHashGetStr (conf->sectionHash, sectionName);
	if (!section) {
		section = addSection (conf, sectionName);
		if (!section) {
			GfLogError ("getParamByName: addSection failed\n");
			return NULL;
		}
	}
	param = addParam (conf, section, paramName, "");

	return param;
}

/* Remove a parameter */
static void
removeParamByName (struct parmHeader *conf, const char *sectionName, const char *paramName)
{
    char		*fullName;
    struct param	*param;
    struct section	*section;

    section = (struct section *)GfHashGetStr (conf->sectionHash, sectionName);
    if (!section) {
	return;
    }

    fullName = getFullName (sectionName, paramName);
    if (!fullName) {
	GfLogError ("removeParamByName: getFullName failed\n");
	return;
    }

    param = (struct param *)GfHashGetStr (conf->paramHash, fullName);
    freez (fullName);
    if (param) {
	removeParam (conf, section, param);
    }

    cleanUnusedSection (conf, section);
}

/* Clean up unused sections and parents */
static void
cleanUnusedSection (struct parmHeader *conf, struct section *section)
{
    struct section	*parent;

    if (!section->fullName ||
	(!GF_TAILQ_FIRST (&(section->paramList)) &&
	 !GF_TAILQ_FIRST (&(section->subSectionList)))) {
	parent = section->parent;
	removeSection (conf, section);
	if (parent) {
	    /* check if the parent is unused */
	    cleanUnusedSection (conf, parent);
	}
    }
}

/* Remove a parameter */
static void
removeParam (struct parmHeader *conf, struct section *section, struct param *param)
{
	GfHashRemStr (conf->paramHash, param->fullName);
	GF_TAILQ_REMOVE (&(section->paramList), param, linkParam);

	struct within *within;
	while ((within = GF_TAILQ_FIRST (&param->withinList)) != GF_TAILQ_END (&param->withinList)) {
		GF_TAILQ_REMOVE (&param->withinList, within, linkWithin);
		freez(within->val);
		free(within);
	}
        if (param->type == P_FORM)
		GfFormFreeCommandNew(param->formula);
	param->formula = NULL;

	freez (param->name);
    freez (param->fullName);
    freez (param->value);
	freez (param->unit);
    freez (param);
}

/* Add a parameter anywhere, does not check for duplicate. */
static struct param *
addParam (struct parmHeader *conf, struct section *section, const char *paramName, const char *value)
{
	char		*fullName;
	struct param	*param = NULL;
	char		*tmpVal = NULL;
	
	tmpVal = strdup (value);
	if (!tmpVal) {
		GfLogError ("addParam: strdup (%s) failed\n", value);
		goto bailout;
	}
	
	param = (struct param *) calloc (1, sizeof (struct param));
	if (!param) {
#if defined(_MSC_VER) && _MSC_VER < 1800
		GfLogError ("addParam: calloc (1, %03Iu) failed\n", sizeof (struct param));
#else //_MSC_VER
		GfLogError ("addParam: calloc (1, %zu) failed\n", sizeof (struct param));
#endif //_MSC_VER
		goto bailout;
	}

	param->name = strdup (paramName);
	if (!param->name) {
		GfLogError ("addParam: strdup (%s) failed\n", paramName);
		goto bailout;
	}
	
	fullName = getFullName (section->fullName, paramName);
	if (!fullName) {
		GfLogError ("addParam: getFullName failed\n");
		goto bailout;
	}

	param->fullName = fullName;
	if (GfHashAddStr (conf->paramHash, param->fullName, param)) {
		goto bailout;
	}
	
	GF_TAILQ_INIT (&(param->withinList));
	
	/* Attach to section */
	GF_TAILQ_INSERT_TAIL (&(section->paramList), param, linkParam);
	
	freez (param->value);
	param->value = tmpVal;
	
	return param;
	
 bailout:
	if (param) {
		freez (param->name);
		freez (param->fullName);
		freez (param->value);
		free  (param);
	}
	freez (tmpVal);
	
	return NULL;
}


/* Remove a section */
static void
removeSection (struct parmHeader *conf, struct section *section)
{
	struct param *param;
	struct section *subSection;

	while ((subSection = GF_TAILQ_FIRST (&(section->subSectionList))) != NULL) {
		removeSection (conf, subSection);
	}

	if (section->fullName) {
		/* not the root section */
		GfHashRemStr (conf->sectionHash, section->fullName);
		GF_TAILQ_REMOVE (&(section->parent->subSectionList), section, linkSection);
		while ((param = GF_TAILQ_FIRST (&(section->paramList))) != GF_TAILQ_END (&(section->paramList))) {
			removeParam (conf, section, param);
		}
		freez (section->fullName);
	}
	freez (section);
}

/* Get or create parent section */
static struct section *
getParent (struct parmHeader *conf, const char *sectionName)
{
	struct section	*section;
	char		*tmpName;
	char		*s;

	tmpName = strdup (sectionName);
	if (!tmpName) {
		GfLogError ("getParent: strdup (\"%s\") failed\n", sectionName);
		return NULL;
	}

	s = strrchr (tmpName, '/');
	if (s) {
		*s = '\0';
		section = (struct section *)GfHashGetStr (conf->sectionHash, tmpName);
		if (section) {
			goto end;
		}
		section = addSection (conf, tmpName);
		goto end;
	} else {
		section = conf->rootSection;
		goto end;
	}

end:
	free (tmpName);
	return section;
}

/* Add a new section */
static struct section *
addSection (struct parmHeader *conf, const char *sectionName)
{
	struct section	*section;
	struct section	*parent;

	if (GfHashGetStr (conf->sectionHash, sectionName)) {
		GfLogError ("addSection: duplicate section [%s]\n", sectionName);
		return NULL;
	}

	parent = getParent(conf, sectionName);
	if (!parent) {
		GfLogError ("addSection: Problem with getParent for section [%s]\n", sectionName);
		return NULL;
	}

	section = (struct section *) calloc (1, sizeof (struct section));
	if (!section) {
#if defined(_MSC_VER) && _MSC_VER < 1800
		GfLogError ("addSection: calloc (1, 03Iu) failed\n", sizeof (struct section));
#else //_MSC_VER
		GfLogError ("addSection: calloc (1, %zu) failed\n", sizeof (struct section));
#endif //_MSC_VER
		return NULL;
	}

	section->fullName = strdup(sectionName);
	if (!section->fullName) {
		GfLogError ("addSection: strdup (%s) failed\n", sectionName);
		goto bailout;
	}

	if (GfHashAddStr (conf->sectionHash, sectionName, section)) {
		GfLogError ("addSection: GfHashAddStr failed\n");
		goto bailout;
	}

	/* no more bailout call */
	section->parent = parent;
	GF_TAILQ_INIT (&(section->paramList));
	GF_TAILQ_INIT (&(section->subSectionList));
	GF_TAILQ_INSERT_TAIL (&(parent->subSectionList), section, linkSection);

	return section;

bailout:
	freez (section->fullName);
	freez (section);
	return NULL;
}

static struct parmHeader *
getSharedHeader (const char *file, int mode)
{
    struct parmHeader	*conf = NULL;
    struct parmHandle	*parmHandle;

    /* Search for existing conf */
    if ((mode & GFPARM_RMODE_PRIVATE) == 0) {

	for (parmHandle = GF_TAILQ_FIRST (&parmHandleList);
	     parmHandle != GF_TAILQ_END (&parmHandleList);
	     parmHandle = GF_TAILQ_NEXT (parmHandle, linkHandle)) {

	    if ((parmHandle->flag & PARM_HANDLE_FLAG_PRIVATE) == 0) {
		conf = parmHandle->conf;
		if (!strcmp(conf->filename, file)) {
		    if (mode & GFPARM_RMODE_REREAD) {
			parmClean (conf);
		    }
		    conf->refcount++;
		    return conf;
		}
	    }
	}
    }

    return NULL;
}

/* Conf header creation */
static struct parmHeader *
createParmHeader (const char *file)
{
    struct parmHeader	*conf = NULL;

    conf = (struct parmHeader *) calloc (1, sizeof (struct parmHeader));
    if (!conf) {
#if defined(_MSC_VER) && _MSC_VER < 1800
	GfLogError ("createParmHeader: calloc (1, %03Iu) failed\n", sizeof (struct parmHeader));
#else //_MSC_VER
	GfLogError ("createParmHeader: calloc (1, %zu) failed\n", sizeof (struct parmHeader));
#endif //_MSC_VER
	return NULL;
    }

    conf->refcount = 1;

    conf->rootSection = (struct section *) calloc (1, sizeof (struct section));
    if (!conf->rootSection) {
#if defined(_MSC_VER) && _MSC_VER < 1800
	GfLogError ("createParmHeader: calloc (1, %03Iu) failed\n", sizeof (struct section));
#else //_MSC_VER
	GfLogError ("createParmHeader: calloc (1, %zu) failed\n", sizeof (struct section));
#endif //_MSC_VER
	goto bailout;
    }
    GF_TAILQ_INIT (&(conf->rootSection->paramList));
    GF_TAILQ_INIT (&(conf->rootSection->subSectionList));

    conf->paramHash = GfHashCreate (GF_HASH_TYPE_STR);
    if (!conf->paramHash) {
	GfLogError ("createParmHeader: GfHashCreate (paramHash) failed\n");
	goto bailout;
    }

    conf->sectionHash = GfHashCreate (GF_HASH_TYPE_STR);
    if (!conf->sectionHash) {
	GfLogError ("createParmHeader: GfHashCreate (sectionHash) failed\n");
	goto bailout;
    }

    conf->filename = strdup (file);
    if (!conf->filename) {
	GfLogError ("createParmHeader: strdup (%s) failed\n", file);
	goto bailout;
    }
    conf->variableHash = GfHashCreate(GF_HASH_TYPE_STR);
    if (!conf->variableHash) 
    {
	GfLogError ("createParmHeader: GfHashCreate (variableHash) failed\n");
	goto bailout;
    }

    return conf;

 bailout:
    freez (conf->rootSection);
    if (conf->paramHash) {
	GfHashRelease (conf->paramHash, NULL);
    }
    if (conf->sectionHash) {
	GfHashRelease (conf->sectionHash, NULL);
    }
    if (conf->variableHash) {
    	GfHashRelease (conf->variableHash, free);
    }
    freez (conf->filename);
    freez (conf);

    return NULL;
}

static void
addWithin (struct param *curParam, const char *s1)
{
	struct within *curWithin;

	if (!s1 || ! strlen (s1)) {
		return;
	}

	curWithin = (struct within *) calloc (1, sizeof (struct within));
	curWithin->val = strdup (s1);
	GF_TAILQ_INSERT_TAIL (&(curParam->withinList), curWithin, linkWithin);
}


/* XML Processing */
static int
myStrcmp(const void *s1, const void * s2)
{
    return strcmp((const char *)s1, (const char *)s2);
}

static tdble
getValNumFromStr (const char *str)
{
    tdble val = 0.0;

    if (!str || !strlen (str)) {
	return 0.0;
    }

    if (strncmp (str, "0x", 2) == 0) {
	return (tdble)strtol(str, NULL, 0);
    }
    
    sscanf (str, "%g", &val);
    return val;
}

/* XML Processing */
static void xmlStartElement (void *userData , const char *name, const char **atts)
{
	struct parmHandle *parmHandle = (struct parmHandle *)userData;
	struct parmHeader *conf = parmHandle->conf;
	struct param *curParam;

	int	nAtts;
	int	len;
	int	index;
	const char **p;
	const char *s1, *s2;
	char *fullName;
	const char *shortName;
	const char *val;
	const char *min;
	const char *max;
	const char *unit;
	char *within;
	char *sa, *sb;

	if (parmHandle->flag & PARM_HANDLE_FLAG_PARSE_ERROR) 
	{
		// parse error occured, ignore.
		if (TraceLoggersAvailable)
			GfLogWarning("xmlStartElement: parsing error ; ignoring\n");
		else
			fprintf(stderr,"xmlStartElement: parsing error ; ignoring\n");
		return;
	}

	p = atts;
	while (*p) 
	{
		++p;
	}

	nAtts = (p - atts) >> 1;
	if (nAtts > 1) 
	{
		qsort ((void *)atts, nAtts, sizeof(char *) * 2, myStrcmp);
	}

	if (!strcmp(name, "params")) 
	{
		parmHandle->curSection = conf->rootSection;
		if (parmHandle->curSection->fullName)
			freez(parmHandle->curSection->fullName);
		parmHandle->curSection->fullName = strdup ("");

		if (!parmHandle->curSection->fullName) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: strdup (\"\") failed\n");
			else
				fprintf(stderr,"xmlStartElement: strdup (\"\") failed\n");
			goto bailout;
		}

		while (*atts) 
		{
			s1 = *atts++;
			s2 = *atts++;
			if (!strcmp(s1, "name")) 
			{
				FREEZ (conf->name);
				conf->name = strdup(s2);
				if (!conf->name) 
				{
					if (TraceLoggersAvailable)
						GfLogError ("xmlStartElement: strdup (\"%s\") failed\n", s2);
					else
						fprintf(stderr,"xmlStartElement: strdup (\"%s\") failed\n", s2);
					goto bailout;
				}
				//break;
			}
			if (strcmp(s1, "version") == 0)
			{
				conf->major = 0;
				conf->minor = 0;
				index = 0;
				while (s2[index] != '\0' && s2[index] != '.')
				{
					if (s2[index] >= '0' && s2[index] <= '9')
					{
						conf->major *= 10;
						conf->major += s2[index] - '0';
					}
					++index;
				}
				if (s2[index] != '\0')
					++index;
				while (s2[index] != '\0')
				{
					if (s2[index] >= '0' && s2[index] <= '9')
					{
						conf->minor *= 10;
						conf->minor += s2[index] - '0';
					}
					++index;
				}
			}
		}

		if (!conf->name) 
		{
			//GfLogTrace ("xmlStartElement: Syntax error, missing \"name\" field in params definition\n");
			goto bailout;
		}

    	} else if (!strcmp(name, "section")) 
    	{

		if (!parmHandle->curSection) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: Syntax error, missing \"params\" tag\n");
			else
				fprintf(stderr,"xmlStartElement: Syntax error, missing \"params\" tag\n");
			goto bailout;
		}

		shortName = NULL;

		while (*atts) 
		{
			s1 = *atts++;
			s2 = *atts++;
			if (!strcmp(s1, "name")) 
			{
				shortName = s2;
				break;
			}
		}

		if (!shortName) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: Syntax error, missing \"name\" field in section definition\n");
			else
				fprintf(stderr,"xmlStartElement: Syntax error, missing \"name\" field in section definition\n");
			goto bailout;
		}

		if (strlen(parmHandle->curSection->fullName)) 
		{
			len = strlen (shortName) + strlen (parmHandle->curSection->fullName) + 2;
			fullName = (char *) malloc (len);
			if (!fullName) 
			{
				if (TraceLoggersAvailable)
					GfLogError ("xmlStartElement: malloc (%d) failed\n", len);
				else
					fprintf(stderr,"xmlStartElement: malloc (%d) failed\n", len);
				goto bailout;
			}
		    sprintf (fullName, "%s/%s", parmHandle->curSection->fullName, shortName);
		} else 
		{
			fullName = strdup (shortName);
		}

		parmHandle->curSection = addSection(conf, fullName);
		free(fullName);

		if (!parmHandle->curSection) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: addSection failed\n");
			else
				fprintf(stderr,"xmlStartElement: addSection failed\n");	
		}

	} else if (!strcmp(name, "attnum")) 
	{

		if ((!parmHandle->curSection) || (!strlen (parmHandle->curSection->fullName))) 
		{
			if (TraceLoggersAvailable)
	    		GfLogError ("xmlStartElement: Syntax error, missing \"section\" tag\n");
			else
				fprintf(stderr,"xmlStartElement: Syntax error, missing \"section\" tag\n");
			goto bailout;
		}

		shortName = NULL;
		val = NULL;
		min = max = unit = NULL;

		while (*atts) 
		{
			s1 = *atts++;
			s2 = *atts++;
			if (!strcmp(s1, "name")) 
			{
				shortName = s2;
			} else if (!strcmp(s1, "val")) 
			{
				val = s2;
			} else if (!strcmp(s1, "min"))
			{
				min = s2;
			} else if (!strcmp(s1, "max")) 
			{
				max = s2;
			} else if (!strcmp(s1, "unit")) 
			{
				unit = s2;
			}
		}

		if (!shortName) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: Syntax error, missing \"name\" field in %s definition\n", name);
			else
				fprintf(stderr,"xmlStartElement: Syntax error, missing \"name\" field in %s definition\n", name);
			goto bailout;
		}

		if (!val) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: Syntax error, missing \"val\" field in %s definition\n", name);
			else
				fprintf(stderr,"xmlStartElement: Syntax error, missing \"val\" field in %s definition\n", name);
			goto bailout;
		}

		curParam = addParam (conf, parmHandle->curSection, shortName, val);
		if (!curParam) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: addParam failed\n");
			else
				fprintf(stderr,"xmlStartElement: addParam failed\n");
			goto bailout;
		}

		curParam->type = P_NUM;
		curParam->valnum = getValNumFromStr (val);

		if (min)
			curParam->min    = getValNumFromStr (min);
		else 
			curParam->min = getValNumFromStr (val);

		if (max) {
			curParam->max    = getValNumFromStr (max);
			if (curParam->min > curParam->max) {
				curParam->max = curParam->min;
				curParam->min = getValNumFromStr (max);
			}
		} else 
			curParam->max = getValNumFromStr (val);

		if (curParam->min > curParam->valnum) 
		{
			if (TraceLoggersAvailable)
				GfLogWarning("Param '%s' : Loaded value (%f) < min (%f); fixing the min\n",
						 shortName, curParam->valnum, curParam->min);
			else
				fprintf(stderr,"Param '%s' : Loaded value (%f) < min (%f); fixing the min\n",
						 shortName, curParam->valnum, curParam->min);

			curParam->min = curParam->valnum;
		}

		if (curParam->max < curParam->valnum) 
		{
			if (TraceLoggersAvailable)
				GfLogWarning("Param '%s' : Loaded value (%f) > max (%f); fixing the max\n",
						 shortName, curParam->valnum, curParam->max);
			else
				fprintf(stderr,"Param '%s' : Loaded value (%f) > max (%f); fixing the max\n",
						 shortName, curParam->valnum, curParam->max);

			curParam->max = curParam->valnum;
		}

		if (unit) 
		{
			curParam->unit = strdup (unit);
			curParam->valnum = GfParmUnit2SI ((char*)unit, curParam->valnum);
			curParam->min = GfParmUnit2SI ((char*)unit, curParam->min);
			curParam->max = GfParmUnit2SI ((char*)unit, curParam->max);
		}

    	} else if (!strcmp(name, "attstr")) 
    	{

		if ((!parmHandle->curSection) || (!strlen (parmHandle->curSection->fullName))) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: Syntax error, missing \"section\" tag\n");
			else
				fprintf(stderr,"xmlStartElement: Syntax error, missing \"section\" tag\n");

			goto bailout;
		}

		shortName = NULL;
		val = NULL;
		within = NULL;

		while (*atts) 
		{
			s1 = *atts++;
			s2 = *atts++;
			if (!strcmp(s1, "name")) 
			{
				shortName = s2;
			} else if (!strcmp(s1, "val")) 
			{
				val = s2;
			} else if (!strcmp(s1, "in")) 
			{
				within = (char *)s2;
			}
		}

		if (!shortName) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: Syntax error, missing \"name\" field in %s definition\n", name);
			else
				fprintf(stderr,"xmlStartElement: Syntax error, missing \"name\" field in %s definition\n", name);
			goto bailout;
		}

		if (!val) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: Syntax error, missing \"val\" field in %s definition\n", name);
			else
				fprintf(stderr,"xmlStartElement: Syntax error, missing \"val\" field in %s definition\n", name);
			goto bailout;
		}

		curParam = addParam (conf, parmHandle->curSection, shortName, val);
		if (!curParam) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: addParam failed\n");
			else
				fprintf(stderr,"xmlStartElement: addParam failed\n");
			goto bailout;
		}

		curParam->type = P_STR;
		if (within) 
		{
			sa = within;
			sb = strchr (sa, ',');
			while (sb) 
			{
				*sb = 0;
				addWithin (curParam, sa);
				sa = sb + 1;
				sb = strchr (sa, ',');
			}
			addWithin (curParam, sa);
		}
    	} else if (strcmp(name, "attform") == 0) 
    	{
    	
		if ((!parmHandle->curSection) || (!strlen (parmHandle->curSection->fullName))) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: Syntax error, missing \"section\" tag\n");
			else
				fprintf(stderr,"xmlStartElement: Syntax error, missing \"section\" tag\n");
			goto bailout;
		}

		shortName = NULL;
		val = NULL;
		within = NULL;

		while (*atts) 
		{
			s1 = *atts++;
			s2 = *atts++;
			if (!strcmp(s1, "name")) 
			{
				shortName = s2;
			} else if (!strcmp(s1, "val")) 
			{
				val = s2;
			}
		}

		if (!shortName) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: Syntax error, missing \"name\" field in %s definition\n", name);
			else
				fprintf(stderr,"xmlStartElement: Syntax error, missing \"name\" field in %s definition\n", name);
			goto bailout;
		}

		if (!val) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: Syntax error, missing \"val\" field in %s definition\n", name);
			else
				fprintf(stderr,"xmlStartElement: Syntax error, missing \"val\" field in %s definition\n", name);
			goto bailout;
		}

		curParam = addParam (conf, parmHandle->curSection, shortName, val);
		if (!curParam) 
		{
			if (TraceLoggersAvailable)
				GfLogError ("xmlStartElement: addParam failed\n");
			else
				fprintf(stderr,"xmlStartElement: addParam failed\n");
			goto bailout;
		}

		curParam->type = P_FORM;
		curParam->formula = GfFormParseFormulaStringNew(val);
		//curParam->formula = GfFormParseFormulaString(val);
     	}
 
    return;

 bailout:
    parmHandle->flag |= PARM_HANDLE_FLAG_PARSE_ERROR;
    return;
}

static void
xmlEndElement (void *userData, const XML_Char *name)
{
    struct parmHandle	*parmHandle = (struct parmHandle *)userData;

    if (parmHandle->flag & PARM_HANDLE_FLAG_PARSE_ERROR) {
	/* parse error occured, ignore */
	return;
    }

    if (!strcmp(name, "section")) {
	if ((!parmHandle->curSection) || (!parmHandle->curSection->parent)) {
	    GfLogError ("xmlEndElement: Syntax error in \"%s\"\n", name);
	    return;
	}
	parmHandle->curSection = parmHandle->curSection->parent;
    }
}

static int
xmlExternalEntityRefHandler (XML_Parser mainparser,
			     const XML_Char *openEntityNames,
			     const XML_Char * /* base */,
			     const XML_Char *systemId,
			     const XML_Char * /* publicId */)
{
    FILE 		*in;
    char		buf[BUFSIZ];
    XML_Parser 		parser;
    int			done;
    char		fin[LINE_SZ];
    char		*s;
    struct parmHandle	*parmHandle;
    struct parmHeader	*conf;

    parmHandle = (struct parmHandle *)XML_GetUserData (mainparser);
    conf = parmHandle->conf;

    parser = XML_ExternalEntityParserCreate (mainparser,
					     openEntityNames,
					     (const XML_Char *)NULL);

    if (systemId[0] == '/') {
	strncpy (fin, systemId, sizeof (fin));
	fin[LINE_SZ - 1] = 0;
    } else {
	/* relative path */
	strncpy (fin, conf->filename, sizeof (fin));
	fin[LINE_SZ - 1] = 0;
	s = strrchr (fin, '/');
	if (s) {
	    s++;
	} else {
	    s = fin;
	}
	strncpy (s, systemId, sizeof (fin) - (s - fin));
	fin[LINE_SZ - 1] = 0;
    }

	in = fopen (fin, "r");
    if (in == NULL) {
		perror (fin);
		GfLogError ("xmlExternalEntityRefHandler: file %s has pb\n", systemId);
		return 0;
    }

	XML_SetElementHandler (parser, xmlStartElement, xmlEndElement);
	do {
		size_t len = fread (buf, 1, sizeof(buf), in);
		done = len < sizeof (buf);
		if (!XML_Parse (parser, buf, len, done)) {
			GfLogError ("xmlExternalEntityRefHandler: file %s -> %s at line %lu\n",
				systemId,
				XML_ErrorString(XML_GetErrorCode(parser)),
				XML_GetCurrentLineNumber(parser));
	    	fclose (in);
			return 0;
		}
	} while (!done);
	XML_ParserFree (parser);
	fclose(in);

	return 1; /* ok (0 for failure) */
}


/* xml type parameters line parser */
static int
parseXml (struct parmHandle *parmHandle, char *buf, int len, int done)
{
    if (!XML_Parse(parmHandle->parser, buf, len, done))
    {
		if (TraceLoggersAvailable)
			GfLogError ("parseXml: %s at line %lu\n",
				(char*)XML_ErrorString (XML_GetErrorCode (parmHandle->parser)),
				XML_GetCurrentLineNumber (parmHandle->parser));
		else
            fprintf (stderr,"parseXml: %s at line %lu\n",
				(char*)XML_ErrorString (XML_GetErrorCode (parmHandle->parser)),
				XML_GetCurrentLineNumber (parmHandle->parser));
		return 1;
	}

    if (done)
    {
		XML_ParserFree(parmHandle->parser);
		parmHandle->parser = 0;
	}

	return 0;
}


static int
parserXmlInit (struct parmHandle *parmHandle)
{
    parmHandle->parser = XML_ParserCreate((XML_Char*)NULL);
    XML_SetElementHandler(parmHandle->parser, xmlStartElement, xmlEndElement);
    XML_SetExternalEntityRefHandler(parmHandle->parser, xmlExternalEntityRefHandler);
    XML_SetUserData(parmHandle->parser, parmHandle);

    return 0;
}

/** Read a configuration buffer.
    @ingroup	conf
    @param	logHandle	log handle
    @param	buf		input buffer.
    @return	handle on the configuration data
    <br>0 if Error
*/
void *
GfParmReadBuf (char *buffer)
{
    struct parmHeader	*conf;
    struct parmHandle	*parmHandle = NULL;

    /* Conf Header creation */
    conf = createParmHeader ("");
    if (!conf) {
	GfLogError ("GfParmReadBuf: conf header creation failed\n");
	goto bailout;
    }

    /* Handle creation */
    parmHandle = (struct parmHandle *) calloc (1, sizeof (struct parmHandle));
    if (!parmHandle) {
#if defined(_MSC_VER) && _MSC_VER < 1800
	GfLogError ("GfParmReadBuf: calloc (1, %03Iu) failed\n", sizeof (struct parmHandle));
#else //_MSC_VER
	GfLogError ("GfParmReadBuf: calloc (1, %zu) failed\n", sizeof (struct parmHandle));
#endif //_MSC_VER
	goto bailout;
    }

    parmHandle->magic = PARM_MAGIC;
    parmHandle->conf = conf;
    parmHandle->val = NULL;
    parmHandle->flag = PARM_HANDLE_FLAG_PRIVATE;

    /* Parsers Initialization */
    if (parserXmlInit (parmHandle)) {
	GfLogError ("GfParmReadBuf: parserInit failed\n");
	goto bailout;
    }

    /* Parameters reading in buffer */
    if (parseXml (parmHandle, buffer, strlen (buffer), 1)) {
	GfLogError ("GfParmReadBuf: Parsing failed for buffer\n");
	goto bailout;
    }

    GF_TAILQ_INSERT_HEAD (&parmHandleList, parmHandle, linkHandle);

    return parmHandle;

 bailout:
    freez (parmHandle);
    if (conf) {
	parmReleaseHeader (conf);
    }

    return NULL;
}




void * 
GfParmReadFileLocal(const char *file, int mode, bool neededFile)
{
	//use local dir
	char buf[255];
	sprintf(buf, "%s%s", GfLocalDir(),file);
	
	void *pResult = GfParmReadFile(buf,mode,neededFile);

	return pResult;
}

/** Read a configuration file.
    @ingroup	conf
    @param	logHandle	log handle
    @param	file		name of the file to read or content if input is a buffer.
    @param	mode		opening mode is a mask of:
    				#GF_PARM_RMODE_STD
    				#GF_PARM_RMODE_REREAD
    				#GF_PARM_RMODE_CREAT
    				#GF_PARM_RMODE_PRIVATE
    @return	handle on the configuration data
    <br>0 if Error
*/
void *
GfParmReadFile (const char *file, int mode, bool neededFile, bool trace)
{
    FILE		*in = NULL;
    struct parmHeader	*conf;
    struct parmHandle	*parmHandle = NULL;
    char		buf[LINE_SZ];
    int			len;
    int			done;

    /* switch on/off loggers */
	TraceLoggersAvailable = trace;

    /* search for an already open header & clean the conf if necessary */
    conf = getSharedHeader (file, mode);

    /* Conf Header creation */
    if (conf == NULL) {
	conf = createParmHeader (file);
	if (!conf) {
		if (TraceLoggersAvailable)
		    GfLogError ("GfParmReadFile: conf header creation failed\n");
		else
		    fprintf(stderr,"GfParmReadFile: conf header creation failed\n");
	    goto bailout;
	}
	mode |= GFPARM_RMODE_REREAD;
    }

    /* Handle creation */
    parmHandle = (struct parmHandle *) calloc (1, sizeof (struct parmHandle));
    if (!parmHandle) {
#if defined(_MSC_VER) && _MSC_VER < 1800
		if (TraceLoggersAvailable)
			GfLogError ("GfParmReadFile: calloc (1, %03Iu) failed\n", sizeof (struct parmHandle));
		else
			fprintf(stderr,"GfParmReadFile: calloc (1, %03Iu) failed\n", sizeof (struct parmHandle));
#else //_MSC_VER
		if (TraceLoggersAvailable)
			GfLogError ("GfParmReadFile: calloc (1, %zu) failed\n", sizeof (struct parmHandle));
		else
			fprintf(stderr,"GfParmReadFile: calloc (1, %zu) failed\n", sizeof (struct parmHandle));
#endif //_MSC_VER
	goto bailout;
    }

    parmHandle->magic = PARM_MAGIC;
    parmHandle->conf = conf;
    parmHandle->val = NULL;
    if (mode & GFPARM_RMODE_PRIVATE) {
	parmHandle->flag = PARM_HANDLE_FLAG_PRIVATE;
    }

    /* File opening */
    if (mode & GFPARM_RMODE_REREAD) {
	in = fopen (file, "r");
	if (!in && ((mode & GFPARM_RMODE_CREAT) == 0)) {
		if (neededFile){
			if (TraceLoggersAvailable)
				GfLogTrace ("Failed to load \"%s\" (fopen failed)\n", file);
			else
				fprintf(stderr,"Failed to load \"%s\" (fopen failed)\n", file);
		}
	    goto bailout;
	}

	if (in) {
	    /* Parsers Initialization */
	    if (parserXmlInit (parmHandle)) {
			if (TraceLoggersAvailable)
				GfLogError ("GfParmReadFile: parserInit failed for file \"%s\"\n", file);
			else
				fprintf(stderr,"GfParmReadFile: parserInit failed for file \"%s\"\n", file);
			goto bailout;
	    }
	    /* Parameters reading */
	    do {
		len = fread (buf, 1, sizeof(buf), in);
		done = len < (int)sizeof(buf);
		if (parseXml (parmHandle, buf, len, done)) {
			if (TraceLoggersAvailable)
			    GfLogError ("GfParmReadFile: Parsing failed in file \"%s\"\n", file);
			else
				fprintf(stderr,"GfParmReadFile: Parsing failed in file \"%s\"\n", file);
		    goto bailout;
		}
		if (parmHandle->flag & PARM_HANDLE_FLAG_PARSE_ERROR) {
		    /* parse error occured, ignore */
			if (TraceLoggersAvailable) 
			    GfLogError ("GfParmReadFile: Parsing failed in file \"%s\"\n", file);
			else
				fprintf(stderr,"GfParmReadFile: Parsing failed in file \"%s\"\n", file);
		    goto bailout;
		}
	    } while (!done);

	    fclose (in);
	    in = NULL;
	}

	if (TraceLoggersAvailable)
		GfLogTrace("Loaded %s (%p)\n", file, parmHandle);
	else
		fprintf(stderr,"Loaded %s (%p)\n", file, parmHandle);
    }

    GF_TAILQ_INSERT_HEAD (&parmHandleList, parmHandle, linkHandle);

    return parmHandle;

 bailout:
    if (in) {
	fclose (in);
    }
    freez (parmHandle);
    if (conf) {
	parmReleaseHeader (conf);
    }

    return NULL;
}



static char* handleEntities(char *s, const char* val)
{
	int i = 0;
	int len = strlen(val);
	for (i = 0; i < len; i++) {
		switch (val[i]) {
			case '<':
				s += sprintf(s, "&lt;"); break;
			case '>':
				s += sprintf(s, "&gt;"); break;
			case '&':
				s += sprintf(s, "&amp;"); break;
			case '\'':
				s += sprintf(s, "&apos;"); break;
			case '"':
				s += sprintf(s, "&quot;"); break;	
			default:
				*(s++) = val[i]; 
				break;
		}
	}

	return s;	
}


/**
 *	@image html output-state.png
 */
static int
xmlGetOuputLine (struct parmHandle *parmHandle, char *buffer, int /* size */, bool forceMinMax = false)
{
    struct parmOutput	*outCtrl = &(parmHandle->outCtrl);
   	struct parmHeader	*conf = parmHandle->conf;
    struct section		*curSection;
   	struct param		*curParam;
   	struct within		*curWithin;
   	char			*s, *t;

    while (1) 
	{
	switch (outCtrl->state) 
	{
	case 0:
	    sprintf (buffer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	    outCtrl->indent = (char *) malloc (LINE_SZ);
	    if (!outCtrl->indent) {
		GfLogError ("xmlGetOutputLine: malloc (%d) failed\n", LINE_SZ);
		return 0;
	    }
	    outCtrl->state = 1;
	    return 1;

	case 1:
	    if (conf->dtd == NULL) {
		conf->dtd = strdup("params.dtd");
	    }
	    if (conf->header == NULL) {
		conf->header = strdup("");
	    }
	    sprintf (buffer, "<!DOCTYPE params SYSTEM \"%s\">\n%s\n", conf->dtd, conf->header);
	    *outCtrl->indent = 0;
	    outCtrl->state = 2;
	    return 1;

	case 2:			/* Start Params */
	    outCtrl->curSection = parmHandle->conf->rootSection;
	    if (conf->major > 0 || conf->minor > 0)
	    	sprintf (buffer, "\n<params name=\"%s\" version=\"%d.%d\">\n", parmHandle->conf->name, conf->major, conf->minor);
	    else
	    	sprintf (buffer, "\n<params name=\"%s\">\n", parmHandle->conf->name);
	    curSection = GF_TAILQ_FIRST (&(outCtrl->curSection->subSectionList));
	    if (curSection) {
		outCtrl->curSection = curSection;
		sprintf (outCtrl->indent + strlen (outCtrl->indent), "  ");
		outCtrl->state = 4;
	    } else {     
		outCtrl->state = 3;
	    }
	    return 1;

	case 3:			/* End Params */
	    sprintf (buffer, "</params>\n");
	    free (outCtrl->indent);
	    outCtrl->state = 9;
	    return 1;

	case 4:			/* Parse section attributes list */
		outCtrl->curParam = GF_TAILQ_FIRST (&(outCtrl->curSection->paramList));
		s = strrchr (outCtrl->curSection->fullName, '/');
		if (!s) 
		{
			s = outCtrl->curSection->fullName;
		} else 
		{
			s++;
		}
		
		t = buffer;
		t += sprintf(t, "%s<section name=\"", outCtrl->indent);
		t = handleEntities(t, s);
		t += sprintf (t, "\">\n");

		sprintf (outCtrl->indent + strlen (outCtrl->indent), "  ");
		outCtrl->state = 5;
		return 1;

	case 5:			/* Parse one attribute */
	    if (!outCtrl->curParam) 
		{
			outCtrl->state = 6;
			break;
		}
            else if (outCtrl->curParam->type == P_FORM) 
	    {
	    	curParam = outCtrl->curParam;
		s = buffer;
		s += sprintf (s, "%s<attform name=\"%s\"", outCtrl->indent, curParam->name);

		s += sprintf(s, " val=\"");
		s = handleEntities(s, curParam->value);
		s += sprintf (s, "\"/>\n");
		outCtrl->curParam = GF_TAILQ_NEXT (curParam, linkParam);

		return 1;
	    } else if (outCtrl->curParam->type == P_STR) 
	    {
		curParam = outCtrl->curParam;
		s = buffer;
		s += sprintf (s, "%s<attstr name=\"%s\"", outCtrl->indent, curParam->name);
		curWithin = GF_TAILQ_FIRST (&(curParam->withinList));
		if (curWithin) 
		{
		    s += sprintf (s, " in=\"%s", curWithin->val);
		    while ((curWithin = GF_TAILQ_NEXT (curWithin, linkWithin)) != NULL) 
		    {
			s += sprintf (s, ",%s", curWithin->val);
		    }
		    s += sprintf (s, "\"");
		}

		s += sprintf(s, " val=\"");
		s = handleEntities(s, curParam->value);
		s += sprintf (s, "\"/>\n");

		outCtrl->curParam = GF_TAILQ_NEXT (curParam, linkParam);
		return 1;			
	    } else //outCtrl->curParam->type == P_NUM
		{
			curParam = outCtrl->curParam;

			s = buffer;
			s += sprintf (s, "%s<attnum name=\"%s\"", outCtrl->indent, curParam->name);

			if (curParam->unit)
			{
			    s += sprintf (s, " unit=\"%s\"",curParam->unit);
			}

            // DAISI: Use %f instead of %g to avoid rounding errors
			if (((forceMinMax) || (curParam->min != curParam->valnum)) && (curParam->min != -FLT_MAX)) 
		    {
				s += sprintf (s, " min=\"%f\"",
				GfParmSI2Unit (curParam->unit, curParam->min));
		    }

		    if (((forceMinMax) || (curParam->max != curParam->valnum)) && (curParam->max != FLT_MAX)) 
		    {
				s += sprintf (s, " max=\"%f\"",
				GfParmSI2Unit (curParam->unit, curParam->max));
		    }

			if (curParam->unit)
			{
			    s += sprintf (s, " val=\"%f\"/>\n",	GfParmSI2Unit (curParam->unit, curParam->valnum));
			} else 
			{
			    s += sprintf (s, " val=\"%f\"/>\n", curParam->valnum);
			}
			outCtrl->curParam = GF_TAILQ_NEXT (curParam, linkParam);
			return 1;
		}

	case 6:			/* Parse sub-section list */
	    	curSection = GF_TAILQ_FIRST (&(outCtrl->curSection->subSectionList));
	   	if (curSection) 
		{
			outCtrl->curSection = curSection;
			outCtrl->state = 4;
			break;
	   	}
	    	outCtrl->state = 7;
	    	break;

	case 7:			/* End Section */
	    *(outCtrl->indent + strlen (outCtrl->indent) - 2) = 0;
	    sprintf (buffer, "%s</section>\n\n", outCtrl->indent);
	    outCtrl->state = 8;
	    return 1;

	case 8:			/* Parse next section at the same level */
	    	curSection = GF_TAILQ_NEXT (outCtrl->curSection, linkSection);
	    	if (curSection) 
		{
		outCtrl->curSection = curSection;
		outCtrl->state = 4;
		break;
	    	}
	   	 curSection = outCtrl->curSection->parent;
	    	*(outCtrl->indent + strlen (outCtrl->indent) - 2) = 0;
	   	 if (curSection->parent) 
		{
			outCtrl->curSection = curSection;
			sprintf (buffer, "%s</section>\n\n", outCtrl->indent);
			return 1;
	    	}
	   	 outCtrl->state = 3;
	   	 break;
	case 9:
	    return 0;
	
	}
    }
}

/** Write a configuration buffer.
    @ingroup	conf
    @param	handle	Configuration handle
    @param	buf		buffer to write the configuration to
    @param	size	buffer size (has to be > 0)
    @return	0 if OK
			<br>-1 if data was truncated
    		<br>1 if other error
*/

int
GfParmWriteBuf (void *handle, char *buf, int size) /* Never used in current codebase: to be removed? */
{
    struct parmHandle	*parmHandle = (struct parmHandle *)handle;
    char		line[LINE_SZ];
    int			len;
    int			curSize;
    char		*s;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogFatal ("GfParmWriteBuf: bad handle (%p)\n", parmHandle);
		return 1; // Error
    }

    // Check buf for NULL before memcpy()ing to it later
    if((buf == NULL) || (size <= 0)){
       GfLogFatal ("GfParmWriteBuf: bad buf or size (%p) (%d) \n", buf,size);
       return 1; // Error
    }

	// Clear buf to contain 0 for all chars
	memset(buf,0,size);

    parmHandle->outCtrl.state = 0;
    parmHandle->outCtrl.curSection = NULL;
    parmHandle->outCtrl.curParam = NULL;
    curSize = size;
    s = buf;

    while (curSize && xmlGetOuputLine (parmHandle, line, sizeof (line))) {
	len = strlen (line);
	// We need space for the terminating 0, len has to be < curSize! 
	if (len >= curSize) {
	    len = curSize;
		memcpy (s, line, len - 1);
		// Don't fall through and return 0;
		return -1; // This is an error: data has been truncated
	}
	memcpy (s, line, len);
	s += len;
	curSize -= len;
    }
    // buf [size - 1] = 0; redundant: memset(buf,0,size); and if(len >= curSize){...
    
    return 0; // Success
}

#ifdef WEBSERVER
/** Write a configuration string.
    @ingroup	conf
    @param	handle	Configuration handle
    @param	str		a std:string to vrite the configuration to
    @return	0 if OK
    		<br>1 bad handle
*/

int
GfParmWriteString (void *handle, std::string& str)
{
    struct parmHandle	*parmHandle = (struct parmHandle *)handle;
    char		line[LINE_SZ];

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogFatal ("GfParmWriteString: bad handle (%p)\n", parmHandle);
		return 1; // Error
    }
    
    parmHandle->outCtrl.state = 0;
    parmHandle->outCtrl.curSection = NULL;
    parmHandle->outCtrl.curParam = NULL;

    while (xmlGetOuputLine (parmHandle, line, sizeof (line))) {
		str.append(line);
    }
   
    return 0; // Success
}
#endif //WEBSERVER

/** Set the dtd path and header if necessary
    @ingroup	conf
    @param	parmHandle	Configuration handle
    @param	dtd		Optional dtd path
    @param	header		Optional header
    @return	none
*/
void
GfParmSetDTD (void *parmHandle, char *dtd, char*header)
{
    struct parmHandle	*handle = (struct parmHandle *)parmHandle;
    struct parmHeader	*conf = handle->conf;

    if (dtd) {
	FREEZ(conf->dtd);
	conf->dtd = strdup(dtd);
    }
    
    if (header) {
	FREEZ(conf->header);
	conf->header = strdup(header);
    }
}


int
GfParmWriteFileLocal(const char *file, void *parmHandle, const char *name)
{
	//use local dir
	char buf[255];
	sprintf(buf, "%s%s", GfLocalDir(), file);

	return GfParmWriteFile(buf,parmHandle,name);
}

/** Write a configuration file.
    @ingroup	conf
    @param	parmHandle	Configuration handle
    @param	file		Name of the file to write (NULL if previously read file)
    @param	name	Name of the parameters
    @return	0 if OK
    <br>1 if Error
*/
int
GfParmWriteFile (const char *file, void *parmHandle, const char *name)
{
    struct parmHandle *handle = (struct parmHandle *)parmHandle;
    struct parmHeader *conf;
    char line[LINE_SZ];
    FILE *fout;
 
    if ((handle == NULL) || (handle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmWriteFile: bad handle (%p)\n", handle);
		return 1;
    }

    conf = handle->conf;

    if (!file) {
	file = conf->filename;
	if (!file) {
	    GfLogError ("GfParmWriteFile: bad file name\n");
	    return 1;
	}
    }
    
    fout = safeFOpen(file, "wb");
    if (!fout) {
	GfLogError ("GfParmWriteFile: fopen (%s, \"wb\") failed\n", file);
	return 1;
    }

    if (name) {
	FREEZ (conf->name);
	conf->name = strdup (name);
    }

    handle->outCtrl.state = 0;
    handle->outCtrl.curSection = NULL;
    handle->outCtrl.curParam = NULL;

    while (xmlGetOuputLine (handle, line, sizeof (line))) {
	fputs (line, fout);
    }    

    fclose (fout);
  
    GfLogTrace ("Wrote %s (%p)\n", file, parmHandle);
    
    return 0;
}

/** Write a configuration file with SD Header as comment.
    @ingroup	conf
    @param	parmHandle	Configuration handle
    @param	file		Name of the file to write (NULL if previously read file)
    @param	name	Name of the parameters
    @param	author	Name for copyright
    @return	0 if OK
    <br>1 if Error
*/
int
GfParmWriteFileSDHeader (const char *file, void *parmHandle, const char *name, const char *author, bool trace)
{
    struct parmHandle *handle = (struct parmHandle *)parmHandle;
    struct parmHeader *conf;
    char line[LINE_SZ];
    FILE *fout;

    TraceLoggersAvailable = trace;

    if ((handle == NULL) || (handle->magic != PARM_MAGIC)) {
		if (TraceLoggersAvailable)
			GfLogError ("GfParmWriteFileSDHeader: bad handle (%p)\n", handle);
		else
			fprintf(stderr,"GfParmWriteFileSDHeader: bad handle (%p)\n", handle);
		return 1;
    }

	conf = handle->conf;
	char buf[255];

/*
	//use local dir
	char buf[255];
	sprintf(buf, "%s%s", GfLocalDir(),file);
    char* filepath = &buf[0];

    if (!filepath) {
	filepath = conf->filename;
	if (!filepath) {
	    GfLogError ("GfParmWriteFileSDHeader: bad file name\n");
	    return 1;
	}
    }
    
    fout = safeFOpen(filepath, "wb");
	if (!fout) {
	GfLogError ("gfParmWriteFileSDHeader: fopen (%s, \"wb\") failed\n", filepath);
	return 1;
    }
*/
	if (!file) {
	file = conf->filename;
	if (!file) {
		if (TraceLoggersAvailable)
		    GfLogError ("GfParmWriteFileSDHeader: bad file name\n");
		else
			fprintf(stderr,"GfParmWriteFileSDHeader: bad file name\n");

	    return 1;
	}
    }
    fout = safeFOpen(file, "wb");
	if (!fout) {
		if (TraceLoggersAvailable)
			GfLogError ("GfParmWriteFileSDHeader: fopen (%s, \"wb\") failed\n", file);
		else
			fprintf(stderr,"GfParmWriteFileSDHeader: fopen (%s, \"wb\") failed\n", file);
	return 1;
    }

    if (name) {
	FREEZ (conf->name);
	conf->name = strdup (name);
    }

    handle->outCtrl.state = 0;
    handle->outCtrl.curSection = NULL;
    handle->outCtrl.curParam = NULL;

	bool First = true;
	// Set forceMinMax default parameter true to get always the min and max attribute!
    while (xmlGetOuputLine (handle, line, sizeof (line), true)) 
	{
	  fputs (line, fout);
      if (First)
	  {
		  First = false;

		  char time_buf[255];
#ifdef _MSC_VER
		  _strdate_s(time_buf, sizeof(time_buf));
#else //_MSC_VER
		  time_t rawtime;
		  struct tm *timeinfo;
		  time( &rawtime );
		  timeinfo = localtime( &rawtime );
		  strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %X", timeinfo);
#endif //_MSC_VER

  	      fputs ("<!--\n", fout);
  	      fputs ("    file          : ", fout);
		  if (file)
		  {
			const char * ld = GfLocalDir();
			if (ld != NULL)
			{
				int n = strlen(ld);
				if (strncmp(ld,file,n) == 0)
				{
					strncpy(buf,&file[n],strlen(file)-n-4);
					buf[strlen(file)-n-4] = 0;
				}
				else
				{
					strncpy(buf, file, sizeof(buf) - 1);
					size_t clampSize = strlen(file) - 4 < sizeof(buf) ? strlen(file) - 4 : sizeof(buf) - 1;
					buf[clampSize] = 0;
				}
			}
			else
			{
				strncpy(buf, file, 254);
				buf[254] = '\0';
			}
			fputs (buf, fout);
		  }
  	      fputs ("\n    created       : ", fout);
		  fputs (time_buf, fout);
  	      fputs ("\n    last modified : ", fout);
		  fputs (time_buf, fout);
		  snprintf(buf,sizeof(buf),"\n    copyright     : (C) 2010-2014 %s\n",author);
  	      fputs (buf, fout);
  	      fputs ("\n", fout);
		  snprintf(buf,sizeof(buf),"    SVN version   : $%s$\n","Id:"); // Written in a way that is not replaced here
  	      fputs (buf, fout);
  	      fputs ("-->\n", fout);
  	      fputs ("<!--    This program is free software; you can redistribute it and/or modify  -->\n", fout);
  	      fputs ("<!--    it under the terms of the GNU General Public License as published by  -->\n", fout);
  	      fputs ("<!--    the Free Software Foundation; either version 2 of the License, or     -->\n", fout);
  	      fputs ("<!--    (at your option) any later version.                                   -->\n", fout);
	  }
    }

    fclose (fout);
  
	if (TraceLoggersAvailable)
	    GfLogTrace ("Wrote %s (%p)\n", file, parmHandle);
	else
		fprintf(stderr,"Wrote %s (%p)\n", file, parmHandle);
    
    return 0;
}

/** Remove a parameter.
    @ingroup	conf
    @param	parmHandle	Configuration handle
    @param	sectionName	Parameter section name
    @param	paramName	Parameter name
    @return	none
*/
void
GfParmRemove (void *parmHandle, const char *sectionName, const char *paramName)
{
    struct parmHandle *handle = (struct parmHandle *)parmHandle;
    struct parmHeader *conf;

	if ((handle == NULL) || (handle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmRemove: bad handle (%p)\n", handle);
		return;
    }

    conf = handle->conf;

    removeParamByName (conf, sectionName, paramName);
}


/** Remove a section with given path.
    @ingroup	conf
    @param	handle	handle of parameters
    @param	path	path of section
    @return	0 Ok
		<br>-1 Error
 */
int
GfParmRemoveSection (void *handle, const char *path)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section *section;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmRemoveSection: bad handle (%p)\n", parmHandle);
		return -1;
    }

	conf = parmHandle->conf;

    section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if (!section) {
      GfLogError ("GfParmRemoveSection: Section \"%s\" not found\n", path);
	return -1;
    }
    removeSection (conf, section);
    return 0;
}


static void parmClean (struct parmHeader *conf)
{
	struct section	*section;

	while ((section = GF_TAILQ_FIRST (&(conf->rootSection->subSectionList))) !=
		    GF_TAILQ_END (&(conf->rootSection->subSectionList)))
	{
		removeSection (conf, section);
	}
}


/** Clean all the parameters of a set.
    @ingroup	conf
    @param	parmHandle	Configuration handle
    @return	0 if OK
    		<br>-1 if Error
*/
void
GfParmClean (void *parmHandle)
{
    struct parmHandle	*handle = (struct parmHandle *)parmHandle;
    struct parmHeader	*conf;

	if ((handle == NULL) || (handle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmClean: bad handle (%p)\n", handle);
		return;
    }

    conf = handle->conf;

    parmClean (conf);
}


static void parmReleaseHeader(struct parmHeader *conf)
{
	conf->refcount--;
	if (conf->refcount > 0) {
		return;
	}

	//GfLogTrace ("parmReleaseHeader: refcount null free \"%s\"\n", conf->filename);

	parmClean (conf);

	freez (conf->filename);
	if (conf->paramHash) {
		GfHashRelease (conf->paramHash, NULL);
	}

	if (conf->sectionHash) {
		GfHashRelease (conf->sectionHash, NULL);
	}
	if (conf->variableHash) 
	{
		GfHashRelease (conf->variableHash, free);
	}

	freez (conf->rootSection->fullName);
	freez (conf->rootSection);
	freez (conf->dtd);
	freez (conf->name);
	freez (conf->header);
	freez (conf);
}


static void parmReleaseHandle (struct parmHandle *parmHandle)
{
	struct parmHeader *conf = parmHandle->conf;

	//GfLogTrace ("parmReleaseHandle: release \"%s\" (%p)\n", conf->filename, parmHandle);

	GF_TAILQ_REMOVE (&parmHandleList, parmHandle, linkHandle);
	parmHandle->magic = 0;
	freez (parmHandle->val);
	freez (parmHandle);

	parmReleaseHeader(conf);
}


/** Clean the parms and release the handle without updating the file
    @note	Called by #gfShutdownThread
    @ingroup	conf
    @param	logHandle	log handle
    @param	parmHandle	Configuration handle
    @return	none
*/
void GfParmReleaseHandle (void *parmHandle)
{
	struct parmHandle *handle = (struct parmHandle *)parmHandle;

	if ((handle == NULL) || (handle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmReleaseHandle: bad handle (%p)\n", handle);
		return;
	}

	parmReleaseHandle(handle);
}


static void
evalUnit (char *unit, tdble *dest, int invert)
{
	// TODO: Use a static std::map<const char*, tdble> to make this code faster ?
    tdble coeff = 1.0;
	
    // SI units.
    if (strcmp(unit, "m") == 0) return;
    if (strcmp(unit, "N") == 0) return;
    if (strcmp(unit, "kg") == 0) return;
    if (strcmp(unit, "s") == 0) return;
    if (strcmp(unit, "rad") == 0) return;
    if (strcmp(unit, "Pa") == 0) return;

    // Other non-SI units that are considered as SI ones (backward compatibility with TORCS).
    if ((strcmp(unit, "l") == 0) || (strcmp(unit, "litre") == 0)) return;

    // Non-SI units conversion
	// (please keep the order of the following tests : it is statistically optimized).
    if (strcmp(unit, "deg") == 0) {
	coeff = (float) (M_PI/180.0); /* rad */
    } else if ((strcmp(unit, "lbs") == 0)  || (strcmp(unit, "lb") == 0)) {
	coeff = 0.45359237f; /* kg */
    } else if ((strcmp(unit, "%") == 0) || (strcmp(unit, "percent") == 0)) {
	coeff = 0.01f;
    } else if (strcmp(unit, "mm") == 0) {
	coeff = 0.001f; /* m */
    } else if ((strcmp(unit, "rpm") == 0) || (strcmp(unit, "RPM") == 0)) {
	coeff = 0.104719755f; /* rad/s */
    } else if (strcmp(unit, "kPa") == 0) {
	coeff = 1000.0; /* Pa */
    } else if ((strcmp(unit, "feet") == 0) || (strcmp(unit, "ft") == 0)) {
	coeff = 0.304801f; /* m */
    } else if (strcmp(unit, "km") == 0) {
	coeff = 1000.0; /* m */
    } else if (strcmp(unit, "cm") == 0) {
	coeff = 0.01f; /* m */
    } else if ((strcmp(unit, "in") == 0) || (strcmp(unit, "inch") == 0) || (strcmp(unit, "inches") == 0)) {
	coeff = 0.0254f; /* m */
    } else if ((strcmp(unit, "psi") == 0) || (strcmp(unit, "PSI") == 0)){
	coeff = 6894.76f; /* Pa */
    } else if ((strcmp(unit, "mph") == 0) || (strcmp(unit, "MPH") == 0)) {
	coeff = 0.44704f; /* m/s */
    } else if (strcmp(unit, "MPa") == 0) {
	coeff = 1000000.0; /* Pa */
    } else if ((strcmp(unit, "h") == 0) || (strcmp(unit, "hour") == 0) || (strcmp(unit, "hours") == 0)) {
	coeff = 3600.0; /* s */
    } else if ((strcmp(unit, "day") == 0) || (strcmp(unit, "days") == 0)) {
	coeff = 24*3600.0; /* s */
    } else if ((strcmp(unit, "slug") == 0) || (strcmp(unit, "slugs") == 0)) {
	coeff = 14.59484546f; /* kg */
    } else if (strcmp(unit, "kN") == 0) {
	coeff = 1000.0f; /* N */
    }

    if (invert) {
	*dest /= coeff;
    } else {
	*dest *= coeff;
    }
    
    return;
}

/** Convert a value in "units" into SI.
    @ingroup	paramsdata
    @param	unit	unit name
    @param	val	value in units
    @return	the value in corresponding SI unit
    @warning	The supported units are:
    <br><ul><li><b>feet</b> or <b>ft</b>  converted to <b>m</b></li>
			<li><b>inches</b> or <b>in</b> converted to <b>m</b></li>
			<li><b>lbs</b> converted to <b>kg</b></li>
			<li><b>slug</b> or <b>slugs</b> converted to <b>kg</b></li>
			<li><b>h</b> or <b>hours</b> converted to <b>s</b></li>
			<li><b>day</b> or <b>days</b> converted to <b>s</b></li>
			<li><b>km</b> converted to <b>m</b></li>
			<li><b>cm</b> converted to <b>m</b></li>
			<li><b>mm</b> converted to <b>m</b></li>
			<li><b>kPa</b> converted to <b>Pa</b></li>
			<li><b>deg</b> converted to <b>rad</b></li>
			<li><b>rpm</b> or <b>RPM</b> converted to <b>rad/s</b></li>
			<li><b>percent</b> or <b>%</b> divided by <b>100</b></li>
	     </ul>
	     <br>All other units are considered SI, and thus not converted (coef=1)
    @see	GfParmSI2Unit
 */
tdble
GfParmUnit2SI (const char *unit, tdble val)
{
	char buf[256];
	int  idx;
	const char *s;
	int  inv;
	tdble dest = val;
	
	if ((unit == NULL) || (strlen(unit) == 0)) return dest;
	
	s = unit;
	buf[0] = 0;
	inv = 0;
	idx = 0;
	
	while (*s != 0) {
		switch (*s) {
			case '.':
				evalUnit(buf, &dest, inv);
				buf[0] = 0;
				idx = 0;
				break;
			case '/':
				evalUnit(buf, &dest, inv);
				buf[0] = 0;
				idx = 0;
				inv = 1;
				break;
			case '2':
				evalUnit(buf, &dest, inv);
				evalUnit(buf, &dest, inv);
				buf[0] = 0;
				idx = 0;
				break;	    
			default:
				buf[idx++] = *s;
				buf[idx] = 0;
				break;
		}
		s++;
	}

	evalUnit(buf, &dest, inv);
	return dest;
}

/** Convert a value in SI to "units".
    @ingroup	paramsdata
    @param	unit	unit name to convert to
    @param	val	value in SI units to be converted to units
    @return	converted value to units
    @see	GfParmUnit2SI
 */
tdble
GfParmSI2Unit (const char *unit, tdble val)
{
	char buf[256];
	int  idx;
	const char *s;
	int  inv;
	tdble dest = val;
	
	if ((unit == NULL) || (strlen(unit) == 0)) return dest;
	
	s = unit;
	buf[0] = 0;
	inv = 1;
	idx = 0;
	
	while (*s != 0) {
		switch (*s) {
			case '.':
				evalUnit(buf, &dest, inv);
				buf[0] = 0;
				idx = 0;
				break;
			case '/':
				evalUnit(buf, &dest, inv);
				buf[0] = 0;
				idx = 0;
				inv = 0;
				break;
			case '2':
				evalUnit(buf, &dest, inv);
				evalUnit(buf, &dest, inv);
				buf[0] = 0;
				idx = 0;
				break;	    
			default:
				buf[idx++] = *s;
				buf[idx] = 0;
				break;
		}
		s++;
	}

	evalUnit(buf, &dest, inv);
	return dest;
}



/** Get the name of the parameters (value of the "name" attribute of the root element).
    @ingroup	paramsdata
    @param	handle	Handle on the parameters
    @return	Name
*/
char *
GfParmGetName (void *handle)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetName: bad handle (%p)\n", parmHandle);
		return NULL;
    }

    return parmHandle->conf->name;
}

/** Get the major version of the parameters (value of the "version" attribute of the root element).
    @ingroup	paramsdata
    @param	handle	Handle on the parameters
    @return	major version number
*/
int
GfParmGetMajorVersion (void *handle)
{
    struct parmHandle	*parmHandle = (struct parmHandle *)handle;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetMajorVersion: bad handle (%p)\n", parmHandle);
		return 0;
    }

    return parmHandle->conf->major;
}

/** Get the minor version of the parameters (value of the "version" attribute of the root element).
    @ingroup	paramsdata
    @param	handle	Handle on the parameters
    @return	Name
*/
int
GfParmGetMinorVersion (void *handle)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetMinorVersion: bad handle (%p)\n", parmHandle);
		return 0;
    }

    return parmHandle->conf->minor;
}


/** Get the parameters file name
    @ingroup	paramsfile
    @param	handle	Handle on the parameters
    @return	File Name
*/
char *
GfParmGetFileName (void *handle)
{
    struct parmHandle	*parmHandle = (struct parmHandle *)handle;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetFileName: bad handle (%p)\n", parmHandle);
		return NULL;
    }

    return parmHandle->conf->filename;
}


/** Count the number of section elements of a list.
    @ingroup	paramslist
    @param	handle	handle of parameters
    @param	path	path of list
    @return	element count
 */
int
GfParmGetEltNb (void *handle, const char *path)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section	*section;
    int	count;
    
    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetEltNb: bad handle (%p)\n", parmHandle);
		return 0;
    }

	conf = parmHandle->conf;

    section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if (!section) {
	return 0;
    }

    count = 0;
    section = GF_TAILQ_FIRST (&(section->subSectionList));
    while (section) {
	count++;
	section = GF_TAILQ_NEXT (section, linkSection);
    }

    return count;
}

/** Check if a section exists.
    @ingroup	paramsdata
    @param	handle	handle of parameters
    @param	path	path of section
    @return	0 if doesn't exist, 1 otherwise.
 */
int
GfParmExistsSection (void *handle, const char *path)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section	*section;
    
    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmExistsSection: bad handle (%p)\n", parmHandle);
		return 0;
    }

	conf = parmHandle->conf;

    section = (struct section *)GfHashGetStr (conf->sectionHash, path);

	return section != 0 ? 1 : 0;
}

/** Check if a parameter exists.
    @ingroup    paramsdata
    @param  handle  handle of parameters
    @param  path    path of param
    @param  key key name
    @return 1   exists
            0   doesn't exist
 */
int
GfParmExistsParam(void *handle, const char *path, const char *key)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct param      *param;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
        GfLogError ("GfParmExistsParam: bad handle (%p)\n", parmHandle);
        return 0;
    }

    conf = parmHandle->conf;

    param = getParamByName (conf, path, key, 0);
    if (!param) 
    {
        return 0;
    }

    return 1;
}

/** Seek the first sub-section element of a section.
    @ingroup	paramslist
    @param	handle	handle of parameters
    @param	path	section path
    @return	0 Ok
    		<br>-1 Failed
    @see	GfParmListSeekNext
    @see	GfParmListGetCurEltName
 */
int
GfParmListSeekFirst (void *handle, const char *path)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section *section;
    
    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmListSeekFirst: bad handle (%p)\n", parmHandle);
		return -1;
    }

	conf = parmHandle->conf;

    section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if (!section) {
	return -1;
    }

    section->curSubSection = GF_TAILQ_FIRST (&(section->subSectionList));
    return 0;
}

/** Go to the next sub-section element in the current section.
    @ingroup	paramslist
    @param	handle	handle of parameters
    @param	path	path of the current section
    @return	0 Ok
    		<br>1 End of list reached
    		<br>-1 Failed
    @see	GfParmListSeekFirst	
    @see	GfParmListGetCurEltName
 */
int
GfParmListSeekNext (void *handle, const char *path)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section *section;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmListSeekNext: bad handle (%p)\n", parmHandle);
		return -1;
    }

	conf = parmHandle->conf;

    section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if ((!section) || (!section->curSubSection)) {
	return -1;
    }

    section->curSubSection = GF_TAILQ_NEXT (section->curSubSection, linkSection);
    
    if (section->curSubSection) {
	return 0;
    }
    return 1;			/* EOL reached */
}


/** Remove a section element with given name of a list.
    @ingroup	paramslist
    @param	handle	handle of parameters
    @param	path	path of list
    @param	key	name of the element to remove
    @return	0 Ok
		<br>-1 Error
 */
int
GfParmListRemoveElt (void *handle, const char *path, const char *key)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section *listSection;
    struct section *section;
    char *fullName;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmListRemoveElt: bad handle (%p)\n", parmHandle);
		return -1;
    }

	conf = parmHandle->conf;

    listSection = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if (!listSection) {
	//GfLogTrace ("GfParmListRemoveElt: \"%s\" not found\n", path);
	return -1;
    }
    fullName = (char *) malloc (strlen (path) + strlen (key) + 2);
    if (!fullName) {
#if defined(_MSC_VER) && _MSC_VER < 1800
	GfLogError ("GfParmListRemoveElt: malloc (%lu) failed\n", strlen (path) + strlen (key) + 2);
#else
	GfLogError ("GfParmListRemoveElt: malloc (%zu) failed\n", strlen (path) + strlen (key) + 2);
#endif
	return -1;
    }
    sprintf (fullName, "%s/%s", path, key);
    section = (struct section *)GfHashGetStr (conf->sectionHash, fullName);
    freez(fullName);
    if (!section) {
      GfLogError ("GfParmListRemoveElt: Element \"%s\" not found in \"%s\"\n", key, path);
	return -1;
    }
    removeSection (conf, section);
    return 0;
}


/** Rename a section element to a given name.
    @ingroup	paramslist
    @param	handle	handle of parameters
    @param	path	path of list
    @param	oldKey	name of the element to rename
    @param	newKey	new name to give to the element
    @return	0 Ok
		<br>-1 Error
 */
int
GfParmListRenameElt (void *handle, const char *path, const char *oldKey, const char *newKey)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section *section;
    struct param *param;
    char *oldFullName;
    char *newFullName;

    if ((parmHandle == NULL) ||(parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmListRenameElt: bad handle (%p)\n", parmHandle);
		return -1;
    }

	conf = parmHandle->conf;

	// Build new element full name.
    newFullName = (char *) malloc (strlen (path) + strlen (newKey) + 2);
    if (!newFullName) {
#if defined(_MSC_VER) && _MSC_VER < 1800
	GfLogError ("GfParmListRenameElt: malloc (%lu) failed\n", strlen (path) + strlen (newKey) + 2);
#else
	GfLogError ("GfParmListRenameElt: malloc (%zu) failed\n", strlen (path) + strlen (newKey) + 2);
#endif
	return -1;
    }
    sprintf (newFullName, "%s/%s", path, newKey);

	// Check if no other element has same fullname in the list.
    section = (struct section *)GfHashGetStr (conf->sectionHash, newFullName);
    if (section) {
      GfLogError ("GfParmListRenameElt: Element \"%s\" already in list \"%s\"\n", newKey, path);
	return -1;
    }

	// Check if no other element has same fullname in the list.
    oldFullName = (char *) malloc (strlen (path) + strlen (oldKey) + 2);
    if (!oldFullName) {
#if defined(_MSC_VER) && _MSC_VER < 1800
	GfLogError ("GfParmListRenameElt: malloc (%lu) failed", strlen (path) + strlen (oldKey) + 2);
#else
	GfLogError ("GfParmListRenameElt: malloc (%zu) failed", strlen (path) + strlen (oldKey) + 2);
#endif
	return -1;
    }
    sprintf (oldFullName, "%s/%s", path, oldKey);
    section = (struct section *)GfHashGetStr (conf->sectionHash, oldFullName);
    if (!section) {
      GfLogError ("GfParmListRenameElt: Element \"%s\" not found in list \"%s\"\n", newKey, path);
	return -1;
    }

	// Modify element fullname in section hash list.
    GfHashRemStr(conf->sectionHash, oldFullName);
    freez(oldFullName);
    section->fullName = newFullName;
    GfHashAddStr(conf->sectionHash, newFullName, section);

	// Modify the full name of each param of the list in the section param hash list.
    for (param = GF_TAILQ_FIRST (&(section->paramList));
	 param != GF_TAILQ_END (&(section->paramList));
	 param = GF_TAILQ_NEXT (param, linkParam)) 
	{
        GfHashRemStr(conf->paramHash, param->fullName);
        freez(param->fullName);
		param->fullName = getFullName(section->fullName, param->name);
		GfHashAddStr(conf->paramHash, param->fullName, param);
    }

    return 0;
}


/** Remove all the section elements of a list.
    @ingroup	paramslist
    @param	handle	handle of parameters
    @param	path	path of list
    @return	0 Ok
		<br>-1 Error
 */
int
GfParmListClean (void *handle, const char *path)
{
    struct parmHandle	*parmHandle = (struct parmHandle *)handle;
    struct parmHeader	*conf;
    struct section	*listSection;
    struct section	*section;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmListClean: bad handle (%p)\n", parmHandle);
		return -1;
    }

	conf = parmHandle->conf;

    listSection = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if (!listSection) {
	//GfLogTrace ("GfParmListClean: \"%s\" not found\n", path);
	return -1;
    }
    while ((section = GF_TAILQ_FIRST (&(listSection->subSectionList))) != NULL) {
	removeSection (conf, section);
    }
    return 0;
}


/** Get the name of the current sub-section of a section .
    @ingroup	paramslist
    @param	handle	handle of parameters
    @param	path	path of the section
    @return	Name of the current sub-section in the section
		<br>NULL if failed
    @see	GfParmListSeekFirst	
    @see	GfParmListSeekNext
 */
char *
GfParmListGetCurEltName (void *handle, const char *path)
{
	struct parmHandle *parmHandle = (struct parmHandle *)handle;
	struct parmHeader *conf;
	struct section *section;
	char *s;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmListGetCurEltName: bad handle (%p)\n", parmHandle);
		return NULL;
    }

	conf = parmHandle->conf;

	section = (struct section *)GfHashGetStr (conf->sectionHash, path);
	if ((!section) || (!section->curSubSection)) {
		return NULL;
	}

	s = strrchr (section->curSubSection->fullName, '/');
	if (s) {
		s++;
		return s;
	}

	return section->curSubSection->fullName;
}
/** Get the names of the params in a given section
    @ingroup	paramslist
    @param	handle	handle of parameters
    @param	path	path of list
    @return	a vector of strings containing the names of the params in the list
 */
std::vector<std::string> 
GfParmListGetParamsNamesList (void *handle, const char *path)
{
	struct parmHandle *parmHandle = (struct parmHandle *)handle;
	struct parmHeader *conf;
	struct section *section;
    struct param	*param;
	 std::vector<std::string> paramsNamesList;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmListGetParamsNamesList: bad handle (%p)\n", parmHandle);
		return paramsNamesList;
    }

	conf = parmHandle->conf;
	section = (struct section *)GfHashGetStr (conf->sectionHash, path);
	param = GF_TAILQ_FIRST (&(section->paramList));
	
	while (param) {

		paramsNamesList.push_back(param->name);
	    param = GF_TAILQ_NEXT (param, linkParam);

	}	
	
	return paramsNamesList;
}

/** Get the names of the sections 
    @ingroup	paramslist
    @param	handle	handle of parameters
    @param	path	path of list
    @return	a vector of strings containing the names of the sections in the list
 */
std::vector<std::string>
GfParmListGetSectionNamesList(void *handle)
{
	struct parmHandle *parmHandle = (struct parmHandle *)handle;
	struct parmHeader *conf;
	struct section *section;
	std::vector<std::string> sectionNamesList;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmListGetSectionNamesList: bad handle (%p)\n", parmHandle);
		return sectionNamesList;
    }

	conf = parmHandle->conf;
	section = GF_TAILQ_FIRST (&(conf->rootSection->subSectionList));

    while (section)
    {
		sectionNamesList.push_back(section->fullName);
	    section = GF_TAILQ_NEXT (section, linkSection);
	}	
	
	return sectionNamesList;
}

/** Get string parameter value.
    @ingroup	paramsdata
    @param	parmHandle	Configuration handle
    @param	path		Parameter section name
    @param	key		Parameter name
    @param	deflt		Default value if parameter not existing
    @return	Parameter value
    <br>deflt if Error or not found
    @note	The pointer returned is for immediate use, if you plan
    		to keep the value for a long time, it is necessary to
    		copy it elsewhere, because removing the attribute will
    		produce incoherent pointer.
*/
const char *
GfParmGetStr (void *parmHandle, const char *path, const char *key, const char *deflt)
{
	struct param *param;
	struct parmHandle *handle = (struct parmHandle *)parmHandle;
	struct parmHeader *conf;
	char const *val;
	char *ncval;

	if ((handle == NULL) || (handle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetStr: bad handle (%p)\n", parmHandle);
		return deflt;
	}

	conf = handle->conf;

	param = getParamByName (conf, path, key, 0);
	if (!param || !(param->value) || !strlen (param->value) || (param->type != P_STR && param->type != P_FORM)) {
		return deflt;
	}

	if (param->type == P_FORM)
	{
		ncval = NULL;
		GfFormCalcFuncNew( param->formula, handle, path, NULL, NULL, NULL, &ncval);
		val = ncval ? ncval : deflt;
	} else
	{
		val = param->value;
	}

	return val;
}

/** Get string parameter in values.
    @ingroup	paramsdata
    @param	parmHandle	Configuration handle
    @param	path		Parameter section name
    @param	key		Parameter name
    @return	vector of possible strings
*/
std::vector<std::string>
GfParmGetStrIn(void *parmHandle, const char *path, const char *key)
{
    struct param      *param;
    struct parmHandle *handle = (struct parmHandle *)parmHandle;
    struct parmHeader *conf;
    struct within     *curWithin;
    std::vector<std::string> paramsInList;

    if ((handle == NULL) || (handle->magic != PARM_MAGIC)) {
        GfLogError ("GfParmGetStrIn: bad handle (%p)\n", parmHandle);
        return paramsInList;
    }

    conf = handle->conf;

    param = getParamByName (conf, path, key, 0);
    if (!param || !(param->value) || !strlen (param->value) || (param->type != P_STR && param->type != P_FORM)) {
        return paramsInList;
    }

    curWithin = GF_TAILQ_FIRST (&(param->withinList));
    if (curWithin)
    {
        paramsInList.push_back(curWithin->val);
        while ((curWithin = GF_TAILQ_NEXT (curWithin, linkWithin)) != NULL)
        {
            paramsInList.push_back(curWithin->val);
        }
    }

    return paramsInList;
}

/** Get string parameter value.
    @ingroup	paramsdata
    @param	parmHandle	Configuration handle
    @param	path		Parameter section name
    @param	key		Parameter name
    @param	deflt		Default value if parameter not existing
    @return	Parameter value
    <br>deflt if Error or not found
    @note	The pointer returned is for immediate use, if you plan
    		to keep the value for a long time, it is necessary to
    		copy it elsewhere, because removing the attribute will
    		produce incoherent pointer.
*/
char *
GfParmGetStrNC (void *parmHandle, const char *path, const char *key, char *deflt)
{
	struct param *param;
	struct parmHandle *handle = (struct parmHandle *)parmHandle;
	struct parmHeader *conf;
	char *val;

	if ((handle == NULL) || (handle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetStrNC: bad handle (%p)\n", parmHandle);
		return deflt;
	}

	conf = handle->conf;

	param = getParamByName (conf, path, key, 0);
	if (!param || !(param->value) || !strlen (param->value) || (param->type != P_STR && param->type != P_FORM)) 
	{
		return deflt;
	}
	
	if (param->type == P_FORM) 
	{
		val = deflt;
		//GfFormCalcFuncNew( param->formula, handle, NULL, NULL, NULL, NULL, &val );	
		GfFormCalcFuncNew( param->formula, handle, path, NULL, NULL, NULL, &val );
	} else 
	{
		val = param->value;
	}

//	val = param->value;
	return val;
}

/** Get a string parameter in a config file.
    @ingroup	paramslist
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	deflt	default string	
    @return	parameter value
    @warning	the return value is allocated by the function the caller must free it.
    @see	GfParmListSeekNext
*/
const char *
GfParmGetCurStr (void *handle, const char *path, const char *key, const char *deflt)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
	struct parmHeader *conf;
    struct section	*section;
    struct param	*param;
    char const *val;
    char *ncval;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetCurStr: bad handle (%p)\n", parmHandle);
		return deflt;
	}

	conf = parmHandle->conf;

    section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if ((!section) || (!section->curSubSection)) {
	return deflt;
    }

    param = getParamByName (conf, section->curSubSection->fullName, key, 0);
    if (!param || !(param->value) || !strlen (param->value) || (param->type != P_STR && param->type != P_FORM)) {
	return deflt;
    }

    if (param->type == P_FORM)
    {
	ncval = NULL;
	GfFormCalcFuncNew( param->formula, handle, path, NULL, NULL, NULL, &ncval);
	val = ncval ? ncval : deflt;
    } else
    {
        val = param->value;
    }

    return val;
}

/** Get a string parameter in a config file.
    @ingroup	paramslist
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	deflt	default string	
    @return	parameter value
    @warning	the return value is allocated by the function the caller must free it.
    @see	GfParmListSeekNext
*/
char *
GfParmGetCurStrNC (void *handle, const char *path, const char *key, char *deflt)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section	*section;
    struct param	*param;
    char *val;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetCurStrNC: bad handle (%p)\n", parmHandle);
		return deflt;
	}
	
	conf = parmHandle->conf;

    section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if ((!section) || (!section->curSubSection)) {
	return deflt;
    }

    param = getParamByName (conf, section->curSubSection->fullName, key, 0);
    if (!param || !(param->value) || !strlen (param->value) || (param->type != P_STR && param->type != P_FORM)) 
    {
	return deflt;
    }

    if (param->type == P_FORM) 
    {
        val = deflt;
    //GfFormCalcFuncNew( param->formula, handle, NULL, NULL, NULL, NULL, &val );    
	GfFormCalcFuncNew( param->formula, handle, path, NULL, NULL, NULL, &val );
    } else 
    {
        val = param->value;
    }
    return val;
}

/** Get a numerical parameter in a config file.
    @ingroup	paramsdata
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	unit	unit to convert the result to (NULL if SI wanted)	
    @param	deflt	default string	
    @return	parameter value
 */
tdble
GfParmGetNum (void *handle, char const *path, const char *key, const char *unit, tdble deflt)
{
	struct parmHandle *parmHandle = (struct parmHandle *)handle;
	struct parmHeader *conf;
	struct param *param;
	tdble val;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetNum: bad handle (%p)\n", parmHandle);
		return deflt;
	}

	conf = parmHandle->conf;

	if (parmHandle->magic != PARM_MAGIC) 
	{
		GfLogFatal ("GfParmGetNum: bad handle (%p)\n", parmHandle);
		return deflt;
	}

	param = getParamByName (conf, path, key, 0);
    if (!param ||  (param->type != P_NUM && param->type != P_FORM)) 
    {
		return deflt;
    }
    if (param->type == P_FORM) 
    {
         val = deflt;
		 //GfFormCalcFuncNew( param->formula, parmHandle, NULL, NULL, NULL, &val, NULL );        
		 GfFormCalcFuncNew( param->formula, parmHandle, path, NULL, NULL, &val, NULL );
    }
	else 
    {
         val = param->valnum;
    }

	if (unit) 
	{
		return GfParmSI2Unit(unit, val);
	}
	
    return val;
}



/** Get min of a numerical parameter in a config file.
    @ingroup	paramsdata
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	unit	unit to convert the result to (NULL if SI wanted)	
    @param	deflt	default string	
    @return	parameter value
 */
tdble
GfParmGetNumMin (void *handle, char const *path, const char *key, const char *unit, tdble deflt)
{
	struct parmHandle *parmHandle = (struct parmHandle *)handle;

	if (parmHandle == NULL)
		return deflt;

	struct parmHeader *conf;
	struct param *param;
	tdble min;

	if (parmHandle->magic != PARM_MAGIC) {
		GfFatal ("GfParmGetNumMin: bad handle (%p)\n", parmHandle);
		return deflt;
	}

	conf = parmHandle->conf;

	if (parmHandle->magic != PARM_MAGIC) 
	{
		GfFatal ("GfParmGetNum: bad handle (%p)\n", parmHandle);
		return deflt;
	}

	param = getParamByName (conf, path, key, 0);
	if (!param ||  (param->type != P_NUM)) 
    {
		return deflt;
    }

	min = param->min;

	if (unit) 
	{
		return GfParmSI2Unit(unit, min);
	}
	
    return min;
}

/** Get max of a numerical parameter in a config file.
    @ingroup	paramsdata
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	unit	unit to convert the result to (NULL if SI wanted)	
    @param	deflt	default string	
    @return	parameter value
 */
tdble
GfParmGetNumMax (void *handle, char const *path, const char *key, const char *unit, tdble deflt)
{
	struct parmHandle *parmHandle = (struct parmHandle *)handle;

	if (parmHandle == NULL)
		return deflt;

	struct parmHeader *conf;
	struct param *param;
	tdble max;

	if (parmHandle->magic != PARM_MAGIC) {
		GfFatal ("GfParmGetNum: bad handle (%p)\n", parmHandle);
		return deflt;
	}

	conf = parmHandle->conf;

	if (parmHandle->magic != PARM_MAGIC) 
	{
		GfFatal ("GfParmGetNum: bad handle (%p)\n", parmHandle);
		return deflt;
	}

	param = getParamByName (conf, path, key, 0);
	if (!param ||  (param->type != P_NUM)) 
    {
		return deflt;
    }

	max = param->max;

	if (unit) 
	{
		return GfParmSI2Unit(unit, max);
	}
	
    return max;
}



/** Get a numerical parameter in a config file with limits.
    @ingroup	paramsdata
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	unit	unit to convert the result to (NULL if SI wanted)	
    @param	value	pointer to value
    @param	min	pointer to minimum
    @param	max	pointer to maximum
    @return	0 in success, -1 otherwise
    in success value, min and max is changed
 */
int
GfParmGetNumWithLimits (void *handle, char const *path, const char *key, const char *unit, tdble* value, tdble* min, tdble* max)
{
	struct parmHandle *parmHandle = (struct parmHandle *)handle;
	struct parmHeader *conf;
	struct param *param;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetNumWithLimits: bad handle (%p)\n", parmHandle);
		return -1;
	}

	conf = parmHandle->conf;

	if (parmHandle->magic != PARM_MAGIC) 
	{
		GfLogFatal ("GfParmGetNumWithLimits: bad handle (%p)\n", parmHandle);
		return -1;
	}

	param = getParamByName (conf, path, key, 0);
    if (!param ||  (param->type != P_NUM && param->type != P_FORM)) 
    {
		return -1;
    }
    if (param->type == P_FORM) 
    {
	 GfFormCalcFuncNew( param->formula, parmHandle, path, NULL, NULL, value, NULL );
	 (*min) = (*value);
	 (*max) = (*value);
    }
	else 
    {
	 (*value) = param->valnum;
	 (*min) = param->min;
	 (*max) = param->max;
    }

	if (unit) 
	{
		(*value) = GfParmSI2Unit(unit, *value);
		(*min) = GfParmSI2Unit(unit, *min);
		(*max) = GfParmSI2Unit(unit, *max);
	}
	
    return 0;
}



/** Get a numerical parameter in a config file.
    @ingroup	paramslist
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	unit	unit to convert the result to (NULL if SI wanted)	
    @param	deflt	default string	
    @return	parameter value
 */
tdble
GfParmGetCurNum (void *handle, const char *path, const char *key, const char *unit, tdble deflt)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section	*section;
    struct param	*param;
    tdble val;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetCurNum: bad handle (%p)\n", parmHandle);
		return deflt;
	}

	conf = parmHandle->conf;

	section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if ((!section) || (!section->curSubSection)) {
	return deflt;
    }

    param = getParamByName (conf, section->curSubSection->fullName, key, 0);
    if (!param || (param->type != P_NUM && param->type != P_FORM)) 
    {
	return deflt;
    }

    if (param->type == P_FORM) {
    	val = deflt;
    	GfFormCalcFuncNew( param->formula, parmHandle, section->curSubSection->fullName, NULL, NULL, &val, NULL );
    } else {
    	val = param->valnum;
    }

    if (unit) {
	return GfParmSI2Unit(unit, val);
    }
    return  val;
}



/** Get min of a numerical parameter in a config file.
    @ingroup	paramslist
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	unit	unit to convert the result to (NULL if SI wanted)	
    @param	deflt	default string	
    @return	parameter value
 */
tdble
GfParmGetCurNumMin (void *handle, const char *path, const char *key, const char *unit, tdble deflt)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section	*section;
    struct param	*param;
	tdble min;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetCurNumMin: bad handle (%p)\n", parmHandle);
		return deflt;
	}

	conf = parmHandle->conf;

	section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if ((!section) || (!section->curSubSection)) {
	return deflt;
    }

    param = getParamByName (conf, section->curSubSection->fullName, key, 0);
    if (!param || (param->type != P_NUM && param->type != P_FORM)) 
    {
	return deflt;
    }

	min = param->min;

	if (unit) 
	{
		return GfParmSI2Unit(unit, min);
	}
	
    return min;
}



/** Get max of a numerical parameter in a config file.
    @ingroup	paramslist
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	unit	unit to convert the result to (NULL if SI wanted)	
    @param	deflt	default string	
    @return	parameter value
 */
tdble
GfParmGetCurNumMax (void *handle, const char *path, const char *key, const char *unit, tdble deflt)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section	*section;
    struct param	*param;
    tdble max;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetCurNumMax: bad handle (%p)\n", parmHandle);
		return deflt;
	}

	conf = parmHandle->conf;

	section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if ((!section) || (!section->curSubSection)) {
	return deflt;
    }

    param = getParamByName (conf, section->curSubSection->fullName, key, 0);
    if (!param || (param->type != P_NUM && param->type != P_FORM)) 
    {
	return deflt;
    }

	max = param->max;

	if (unit) 
	{
		return GfParmSI2Unit(unit, max);
	}
	
    return max;
}

/** This function returns TRUE if the entry is a formula, and FALSE otherwise
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name
    @return     0 if the entry is a formula; 1 otherwise
 */
int
GfParmIsFormula (void *handle, char const *path, char const *key)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
	struct parmHeader *conf;
    struct section	*section;
    struct param	*param;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmIsFormula: bad handle (%p)\n", parmHandle);
		return 1;
	}

	conf = parmHandle->conf;

	section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if ((!section) || (!section->curSubSection)) {
	return -1;
    }

    param = getParamByName (conf, section->curSubSection->fullName, key, 0);
    if (!param) {
	return -1;
    }

    return param->type == P_FORM ? 0 : -1;
}

/** Get a formula parameter in a config file.
    @ingroup	paramslist
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @return	parameter value; NULL on error
 */
char*
GfParmGetFormula (void *handle, char const *path, char const *key)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
	struct parmHeader *conf;
    struct section	*section;
    struct param	*param;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetFormula: bad handle (%p)\n", parmHandle);
		return NULL;
	}

	conf = parmHandle->conf;

	section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if ((!section) || (!section->curSubSection)) {
	return NULL;
    }

    param = getParamByName (conf, section->curSubSection->fullName, key, 0);
    if (!param || param->type != P_FORM) {
	return NULL;
    }

    return param->value;
}

/** Get a formula parameter in a config file.
    @ingroup	paramslist
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @return	parameter value; NULL on error
 */
char*
GfParmGetCurFormula (void *handle, char const *path, char const *key)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
	struct parmHeader *conf;
    struct section	*section;
    struct param	*param;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetCurFormula: bad handle (%p)\n", parmHandle);
		return NULL;
	}

	conf = parmHandle->conf;

    section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if ((!section) || (!section->curSubSection)) {
	return NULL;
    }

    param = getParamByName (conf, section->curSubSection->fullName, key, 0);
    if (!param || (param->type != P_NUM && param->type != P_FORM)) {
	return NULL;
    }

    return param->value;
}

/** Set a string parameter in a config file.
    @ingroup	paramsdata
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	val	value (NULL or empty string to remove the parameter)	
    @return	0	ok
    		<br>-1	error
    @warning	The key is created is necessary	
 */
int
GfParmSetStr(void *handle, const char *path, const char *key, const char *val)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct param      *param;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
        GfLogError ("GfParmSetStr: bad handle (%p)\n", parmHandle);
        return -1;
    }

    conf = parmHandle->conf;

    if (!val || !strlen (val)) {
        /* Remove the entry */
        removeParamByName (conf, path, key);
        return 0;
    }
  
    param = getParamByName (conf, path, key, PARAM_CREATE);
    if (!param) {
        return -1;
    }
    param->type = P_STR;
    freez (param->value);
    param->value = strdup (val);
    if (!param->value) {
        GfLogError ("GfParmSetStr: strdup (%s) failed\n", val);
        removeParamByName (conf, path, key);
        return -1;
    }

    return 0;
}

/** Set a string parameter in vector in a config file.
    @ingroup    paramsdata
    @param  handle  handle of parameters
    @param  path    path of param
    @param  key     key name
    @param  in      vector of possible values
    @return 0       ok
            <br>-1  error
    @warning    The key is created is necessary
 */
int
GfParmSetStrIn(void *handle, const char *path, const char *key, const std::vector<std::string> &in)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct param      *param;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
        GfLogError ("GfParmSetStrIn: bad handle (%p)\n", parmHandle);
        return -1;
    }

    conf = parmHandle->conf;
    param = getParamByName (conf, path, key, PARAM_CREATE);
    if (!param) {
        return -1;
    }
    param->type = P_STR;

    struct within *within;
    while ((within = GF_TAILQ_FIRST (&param->withinList)) != GF_TAILQ_END (&param->withinList)) {
        GF_TAILQ_REMOVE (&param->withinList, within, linkWithin);
        freez(within->val);
        free(within);
    }

    for (size_t i = 0; i < in.size(); ++i)
        addWithin(param, in[i].c_str());

    return 0;
}

/** Set a string parameter in a config file.
    @ingroup    paramsdata
    @param  handle  handle of parameters
    @param  path    path of param
    @param  key     key name
    @param  val     value (NULL or empty string to remove the parameter)
    @param  in      vector of possible values
    @return 0   ok
            <br>-1  error
    @warning    The key is created is necessary
 */
int GfParmSetStrAndIn(void *handle, const char *path, const char *key, const char *val, const std::vector<std::string> &in)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct param      *param;

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
        GfLogError ("GfParmSetStrAndIn: bad handle (%p)\n", parmHandle);
        return -1;
    }

    conf = parmHandle->conf;

    if (!val || !strlen (val)) {
        /* Remove the entry */
        removeParamByName (conf, path, key);
        return 0;
    }
  
    param = getParamByName (conf, path, key, PARAM_CREATE);
    if (!param) {
        return -1;
    }
    param->type = P_STR;
    freez (param->value);
    param->value = strdup (val);
    if (!param->value) {
        GfLogError ("GfParmSetStrAndIn: strdup (%s) failed\n", val);
        removeParamByName (conf, path, key);
        return -1;
    }

    struct within *within;
    while ((within = GF_TAILQ_FIRST (&param->withinList)) != GF_TAILQ_END (&param->withinList)) {
        GF_TAILQ_REMOVE (&param->withinList, within, linkWithin);
        freez(within->val);
        free(within);
    }

    for (size_t i = 0; i < in.size(); ++i)
        addWithin(param, in[i].c_str());

    return 0;
}

/** Set a string parameter in a config file.
    @ingroup	paramslist
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	val	value	
    @return	0	ok
    		<br>-1	error
    @warning	The key is created is necessary	
 */
int
GfParmSetCurStr(void *handle, const char *path, const char *key, const char *val)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section	*section;
    struct param	*param;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmSetCurStr: bad handle (%p)\n", parmHandle);
		return -1;
	}

	conf = parmHandle->conf;

	section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if ((!section) || (!section->curSubSection)) {
	return -1;
    }

    param = getParamByName (conf, section->curSubSection->fullName, key, PARAM_CREATE);
    if (!param) {
	return -1;
    }
    param->type = P_STR;
    freez (param->value);
    param->value = strdup (val);
    if (!param->value) {
	GfLogError ("gfParmSetCurStr: strdup (%s) failed\n", val);
	removeParamByName (conf, path, key);
	return -1;
    }

    return 0;
}


/** Set a numerical parameter in a config file.
    @ingroup	paramsdata
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	unit	unit to convert the result to (NULL if SI wanted)	
    @param	val	value to set	
    @return	0	ok
    		<br>-1	error
    @warning	The key is created is necessary
 */
int
GfParmSetNum(void *handle, const char *path, const char *key, const char *unit, tdble val)
{
	struct parmHandle *parmHandle = (struct parmHandle *)handle;
	struct parmHeader *conf;
	struct param	*param;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmSetNum: bad handle (%p)\n", parmHandle);
		return -1;
	}

	conf = parmHandle->conf;
	
	param = getParamByName (conf, path, key, PARAM_CREATE);
	if (!param) 
	{
		return -11;
	}

	param->type = P_NUM;
	FREEZ (param->unit);
	if (unit) 
	{
		param->unit = strdup (unit);
	}
	
	val = GfParmUnit2SI (unit, val);
	param->valnum = val;
	param->min = val;
	param->max = val;
	
	return 0;
}



/** Set a numerical parameter in a config file.
    @ingroup	paramsdata
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	unit	unit to convert the result to (NULL if SI wanted)	
    @param	val	value to set
    @param	min	min value
    @param	max	max value
    @return	0	ok
    		<br>-1	error
    @warning	The key is created is necessary
 */
int
GfParmSetNum(void *handle, const char *path, const char *key, const char *unit, tdble val, tdble min, tdble max)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct param	*param;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmSetNum: bad handle (%p)\n", parmHandle);
		return -1;
	}

	conf = parmHandle->conf;

    param = getParamByName (conf, path, key, PARAM_CREATE);
    if (!param) {
	return -1;
    }
    param->type = P_NUM;
    FREEZ (param->unit);
    if (unit) {
	param->unit = strdup (unit);
    }

    param->valnum = GfParmUnit2SI (unit, val);
    param->min = GfParmUnit2SI (unit, min);
    param->max = GfParmUnit2SI (unit, max);

    return 0;
}

/** Set a numerical parameter in a config file.
    @ingroup	paramslist
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	unit	unit to convert the result to (NULL if SI wanted)	
    @param	val	value to set	
    @return	0	ok
    		<br>-1	error
    @warning	The key is created is necessary
 */
int
GfParmSetCurNum(void *handle, const char *path, const char *key, const char *unit, tdble val)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section	*section;
    struct param	*param;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmSetCurNum: bad handle (%p)\n", parmHandle);
		return -1;
	}

	conf = parmHandle->conf;

	section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if ((!section) || (!section->curSubSection)) {
	return -1;
    }

    param = getParamByName (conf, section->curSubSection->fullName, key, PARAM_CREATE);
    if (!param) {
	return -1;
    }
    param->type = P_NUM;
    FREEZ (param->unit);
    if (unit) {
	param->unit = strdup (unit);
    }

    val = GfParmUnit2SI (unit, val);
    param->valnum = val;
    param->min = val;
    param->max = val;

    return 0;
}

/** Set a formula parameter in a config file.
    @ingroup	paramslist
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	val	value to set	
    @return	0	ok
    		<br>-1	error
    @warning	The key is created is necessary
 */
int
GfParmSetFormula (void *handle, char const *path, char const *key, char const *val)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct param	*param;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmSetFormula: bad handle (%p)\n", parmHandle);
		return -1;
	}

	conf = parmHandle->conf;

    if (!val || !strlen (val)) {
	/* Remove the entry */
	removeParamByName (conf, path, key);
	return 0;
    }
  
    param = getParamByName (conf, path, key, PARAM_CREATE);
    if (!param) {
	return -1;
    }
    param->type = P_FORM;
    param->formula = GfFormParseFormulaStringNew (val);
    freez (param->value);
    param->value = strdup (val);
    if (!param->value) {
	GfLogError ("gfParmSetFormula: strdup (%s) failed\n", val);
	removeParamByName (conf, path, key);
	return -1;
    }

    return 0;
}

/** Set a formula parameter in a config file.
    @ingroup	paramslist
    @param	handle	handle of parameters	
    @param	path	path of param
    @param	key	key name	
    @param	val	value to set	
    @return	0	ok
    		<br>-1	error
    @warning	The key is created is necessary
 */
int
GfParmSetCurFormula(void *handle, char const *path, char const *key, char const *val)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct section	*section;
    struct param	*param;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmSetCurFormula: bad handle (%p)\n", parmHandle);
		return -1;
	}

	conf = parmHandle->conf;

	section = (struct section *)GfHashGetStr (conf->sectionHash, path);
    if ((!section) || (!section->curSubSection)) {
	return -1;
    }

    param = getParamByName (conf, section->curSubSection->fullName, key, PARAM_CREATE);
    if (!param) {
	return -1;
    }
    param->type = P_FORM;
    param->formula = GfFormParseFormulaStringNew (val);
    freez (param->value);
    param->value = strdup (val);
    if (!param->value) {
	GfLogError ("GfParmSetCurFormula: strdup (%s) failed\n", val);
	removeParamByName (conf, path, key);
	return -1;
    }

    return 0;
}

tdble GfParmGetVariable(void *handle, char const *path, char const *key)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    char *pathdup = (char*)malloc( strlen( path ) + strlen( key ) + 3 );
    char *str;
    tdble *val;
    strcpy( pathdup, path );
    if ( pathdup[ 0 ] == '/' )
        memmove( pathdup, pathdup + sizeof( char ), sizeof( char ) * strlen( pathdup ) );

    if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
        GfLogError ("GfParmGetVariable: bad handle (%p)\n", parmHandle);
        free(pathdup);
        return 0.0f;
    }

	conf = parmHandle->conf;

    do {
        strcat( pathdup, "/" );
	strcat( pathdup, key );
        val = (tdble*)GfHashGetStr(conf->variableHash, pathdup);
	str = strrchr(pathdup, '/');
	if (!str)
	    break;
	*str = '\0';
	str = strrchr(pathdup, '/');
	if (!str) {
	    if (pathdup[0] == '\0')
	        break;
	    else
	        str = pathdup;
	}
	*str = '\0';
    } while( val == NULL );

    free(pathdup);

    if (val)
        return *val;
    else
        return 0.0f;
}

void GfParmRemoveVariable(void *handle, char const *path, char const *key)
{
    struct parmHandle	*parmHandle = (struct parmHandle *)handle;
    struct parmHeader	*conf;
    char *pathdup = (char*)malloc( strlen( path ) + strlen( key ) + 3 );
    void *val;

    strcpy( pathdup, path );
    if( pathdup[ 0 ] == '/' )
    	memmove( pathdup, pathdup + sizeof( char ), sizeof( char ) * strlen( pathdup ) );
    if( pathdup[ strlen( path ) - 1 ] != '/' )
        strcat( pathdup, "/" );
    strcat( pathdup, key );

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmRemoveVariable: bad handle (%p)\n", parmHandle);
		free(pathdup);
   	    return;
	}

	conf = parmHandle->conf;
 
    val = GfHashGetStr(conf->variableHash, pathdup);
    GfHashRemStr(conf->variableHash, pathdup);
    free(pathdup);

    if (val)
    	free(val);
}

void GfParmSetVariable(void *handle, char const *path, char const *key, tdble val)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    char *pathdup = (char*)malloc( strlen( path ) + strlen( key ) + 3 );

    tdble *val_ptr;
    strcpy( pathdup, path );
    if( pathdup[ 0 ] == '/' )
    	memmove( pathdup, pathdup + sizeof( char ), sizeof( char ) * strlen( pathdup ) );
    if( pathdup[ strlen( path ) - 1 ] != '/' )
        strcat( pathdup, "/" );
    strcat( pathdup, key );

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmSetVariable: bad handle (%p)\n", parmHandle);
		free(pathdup);
   	    return;
	}

	conf = parmHandle->conf;
 
    val_ptr = (tdble*)malloc( sizeof(tdble) );
    *val_ptr = val;
    GfHashAddStr(conf->variableHash, pathdup, (void*)val_ptr);
    val_ptr = (tdble*)GfHashGetStr(conf->variableHash, pathdup );
    free(pathdup);
}

static char const* GfParmMakePathKey( char const* path, va_list arg, char const **key )
{
    static char buffer[1024];

    vsnprintf( buffer, 1024, path, arg );

    char *lastSlash = strrchr( buffer, '/' );

    if( lastSlash != NULL && key )
    {
        *key = lastSlash + 1;
        lastSlash = (char*)'\0';
    }
    else if( key )
    {
        *key = "";
    }
    return buffer;
}

/*static char const* GfParmMakePathFromPathKey( char const* path, va_list arg )
{
    static char buffer[1024];
	
    vsnprintf( buffer, 1024, path, arg );

    char *lastSlash = strrchr( buffer, '/' );
    if( lastSlash != NULL )
        lastSlash = '\0';
    return buffer;
}

static char const* GfParmMakePathFromPath( char const* path, va_list arg )
{
    static char buffer[1024];

    vsnprintf( buffer, 1024, path, arg );

    return buffer;
}*/

const char *GfParmGetStrf(void *handle, const char *deflt, char const *format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    char const* ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmGetStr( handle, path, key, deflt );
    va_end( arg );

    return ret;
}

char *GfParmGetStrNCf(void *handle, char *deflt, char const *format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    char *ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmGetStrNC( handle, path, key, deflt );
    va_end( arg );

    return ret;
}

const char *GfParmGetCurStrf(void *handle, const char *deflt, char const *format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    char const* ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmGetCurStr( handle, path, key, deflt );
    va_end( arg );

    return ret;
}

char *GfParmGetCurStrNCf(void *handle, char *deflt, char const *format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    char* ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmGetCurStrNC( handle, path, key, deflt );
    va_end( arg );

    return ret;
}

int GfParmSetStrf(void *handle, const char *val, char const *format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    int ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmSetStr( handle, path, key, val );
    va_end( arg );

    return ret;
}

int GfParmSetCurStrf(void *handle, const char *val, char const *format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    int ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmSetCurStr( handle, path, key, val );
    va_end( arg );

    return ret;
}

tdble GfParmGetNumf(void *handle, const char *unit, tdble deflt, char const* format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    tdble ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmGetNum( handle, path, key, unit, deflt );
    va_end( arg );

    return ret;
}

tdble GfParmGetCurNumf(void *handle, const char *unit, tdble deflt, char const* format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    tdble ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmGetCurNum( handle, path, key, unit, deflt );
    va_end( arg );

    return ret;
}

int GfParmSetNumf(void *handle, const char *unit, tdble val, char const* format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    int ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmSetNum( handle, path, key, unit, val );
    va_end( arg );

    return ret;
}

int GfParmSetCurNumf(void *handle, const char *unit, tdble val, char const* format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    int ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmSetCurNum( handle, path, key, unit, val );
    va_end( arg );

    return ret;
}

int GfParmIsFormulaf(void *handle, char const *format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    int ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmIsFormula( handle, path, key);
    va_end( arg );

    return ret;
}

char* GfParmGetFormulaf(void *handle, char const *format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    char* ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmGetFormula( handle, path, key );
    va_end( arg );

    return ret;
}

char* GfParmGetCurFormulaf(void *handle, char const *format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    char *ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmGetCurFormula( handle, path, key );
    va_end( arg );

    return ret;
}

int GfParmSetFormulaf(void* handle, char const *formula, char const* format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    int ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmSetFormula( handle, path, key, formula );
    va_end( arg );

    return ret;
}

int GfParmSetCurFormulaf(void* handle, char const *formula, char const* format, ...)
{
    va_list arg;
    char const* path;
    char const* key;
    int ret;

    va_start( arg, format );
    path = GfParmMakePathKey( format, arg, &key );
    ret = GfParmSetCurFormula( handle, path, key, formula );
    va_end( arg );

    return ret;
}

/** Check a parameter set against another.
    @ingroup	paramsfile
    @param	ref	Contains the min and max values (reference)
    @param	tgt	Contains the parameters to check.
    @return	0 Match
		<br>-1 Values are out of bounds
    @warning	Only the parameters present in tgt and in ref are tested.
    @see	GfParmMergeHandles
 */
int
GfParmCheckHandle(void *ref, void *tgt)
{
    struct parmHandle	*parmHandleRef = (struct parmHandle *)ref;
    struct parmHandle	*parmHandle = (struct parmHandle *)tgt;
    struct parmHeader	*confRef;
    struct parmHeader	*conf;
    struct section	*curSectionRef;
    struct section	*nextSectionRef;
    struct param	*curParamRef;
    struct param	*curParam;
    struct within	*curWithinRef;
    int			found;
    int			error = 0;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmCheckHandle: bad handle (%p)\n", parmHandle);
   	    return -1;
	}

	if ((parmHandleRef == NULL) || (parmHandleRef->magic != PARM_MAGIC)) {
		GfLogError ("GfParmCheckHandle: bad handle (%p)\n", parmHandleRef);
   	    return -1;
	}

	conf = parmHandle->conf;
	confRef = parmHandleRef->conf;

    /* Traverse all the reference tree */
    curSectionRef = GF_TAILQ_FIRST (&(confRef->rootSection->subSectionList));
    while (curSectionRef) {
	curParamRef = GF_TAILQ_FIRST (&(curSectionRef->paramList));
	while (curParamRef) {
	    /* compare params */
	    curParam = getParamByName (conf, curSectionRef->fullName, curParamRef->name, 0);
	    if (curParam) {
		if (curParamRef->type != curParam->type) {
		    GfLogError("GfParmCheckHandle: type mismatch for parameter \"%s\" in (\"%s\" - \"%s\")\n",
			    curParamRef->fullName, conf->name, conf->filename);
		    error = -1;
		} else if (curParamRef->type == P_NUM) {
		    if ((curParam->valnum < curParamRef->min) || (curParam->valnum > curParamRef->max)) {
			GfLogError("GfParmCheckHandle: parameter \"%s\" out of bounds: min:%g max:%g val:%g in (\"%s\" - \"%s\")\n",
				curParamRef->fullName, curParamRef->min, curParamRef->max, curParam->valnum, conf->name, conf->filename);
			//error = -1;
		    }
		} else {
		    curWithinRef = GF_TAILQ_FIRST (&(curParamRef->withinList));
		    found = 0;
		    while (!found && curWithinRef) {
			if (!strcmp (curWithinRef->val, curParam->value)) {
			    found = 1;
			} else {
			    curWithinRef = GF_TAILQ_NEXT (curWithinRef, linkWithin);
			}
		    }
		    if (!found && strcmp (curParamRef->value, curParam->value)) {
			GfLogError("GfParmCheckHandle: parameter \"%s\" value:\"%s\" not allowed in (\"%s\" - \"%s\")\n",
				curParamRef->fullName, curParam->value, conf->name, conf->filename);
			//error = -1;
		    }
		}
	    }
	    curParamRef = GF_TAILQ_NEXT (curParamRef, linkParam);
	}
	nextSectionRef = GF_TAILQ_NEXT (curSectionRef, linkSection);
	while (!nextSectionRef) {
	    nextSectionRef = curSectionRef->parent;
	    if (!nextSectionRef) {
		/* Reached the root */
		break;
	    }
	    curSectionRef = nextSectionRef;
	    nextSectionRef = GF_TAILQ_NEXT (curSectionRef, linkSection);
	}
	curSectionRef = nextSectionRef;
    }
    
    return error;
}

static void
insertParamMerge (struct parmHandle *parmHandle, char *path, struct param *paramRef, struct param *param)
{
    struct parmHeader *conf;
    struct param	*paramNew;
    struct within	*withinRef;
    struct within	*within;
    tdble		num;
    char		*str;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("insertParamMerge: bad handle (%p)\n", parmHandle);
   	    return;
	}

	if (paramRef == NULL) {
		GfLogError ("insertParamMerge: bad handle (%p)\n", paramRef);
   	    return;
	}

	if (param == NULL) {
		GfLogError ("insertParamMerge: bad handle (%p)\n", param);
   	    return;
	}

	conf = parmHandle->conf;
 
    paramNew = getParamByName (conf, path, param->name, PARAM_CREATE);
    if (!paramNew) {
	return;
    }
    if (param->type == P_NUM) {
	paramNew->type = P_NUM;
	FREEZ (paramNew->unit);
	if (param->unit) {
	    paramNew->unit = strdup (param->unit);
	}

	if (param->min <= paramRef->min && param->max >= paramRef->min) {
	    num = paramRef->min;
	} else if (paramRef->min <= param->min && paramRef->max >= param->min) {
	    num = param->min;
	} else {
	    num = paramRef->min;
	    GfLogError("insertParamMerge: Incompatible ranges \"%s\": using %f for min\n", paramNew->fullName, num);
	}
	paramNew->min = num;

	if (param->max >= paramRef->max && param->min <= paramRef->max) {
	    num = paramRef->max;
	} else if (paramRef->max >= param->max && paramRef->min <= param->max) {
	    num = param->max;
	} else {
	    num = paramRef->max;
	    GfLogError("insertParamMerge: Incompatible ranges \"%s\": using %f for max\n", paramNew->fullName, num);
	}
	paramNew->max = num;

	num = param->valnum;
	if (num < paramNew->min) {
	    GfLogError("insertParamMerge: Fixing parameter \"%s\": %f -> %f\n", paramNew->fullName, num, paramNew->min);
	    num = paramNew->min;
	}
	if (num > paramNew->max) {
	    GfLogError("insertParamMerge: Fixing parameter \"%s\": %f -> %f\n", paramNew->fullName, num, paramNew->max);
	    num = paramNew->max;
	}
	paramNew->valnum = num;
    } else {
	paramNew->type = P_STR;
	FREEZ (paramNew->value);
	within = GF_TAILQ_FIRST (&(param->withinList));
	while (within) {
	    withinRef = GF_TAILQ_FIRST (&(paramRef->withinList));
	    while (withinRef) {
		if (!strcmp (withinRef->val, within->val)) {
		    addWithin (paramNew, within->val);
		    break;
		}
		withinRef = GF_TAILQ_NEXT (withinRef, linkWithin);
	    }
	    within = GF_TAILQ_NEXT (within, linkWithin);
	}
	str = NULL;
	withinRef = GF_TAILQ_FIRST (&(paramRef->withinList));
	while (withinRef) {
	    if (!strcmp (withinRef->val, param->value)) {
		str = param->value;
		break;
	    }
	    withinRef = GF_TAILQ_NEXT (withinRef, linkWithin);
	}
	if (!str) {
	    str = paramRef->value;
	}
	paramNew->value = strdup (str);
    }
}

static void
insertParam (struct parmHandle *parmHandle, char *path, struct param *param)
{
    struct parmHeader *conf;
    struct param	*paramNew;
    struct within	*within;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("insertParam: bad handle (%p)\n", parmHandle);
   	    return;
	}

	if (param == NULL) {
		GfLogError ("insertParam: bad handle (%p)\n", param);
   	    return;
	}

	conf = parmHandle->conf;

    paramNew = getParamByName (conf, path, param->name, PARAM_CREATE);
    if (!paramNew) {
	return;
    }
    if (param->type == P_NUM) {
	paramNew->type = P_NUM;
	FREEZ (paramNew->unit);
	if (param->unit) {
	    paramNew->unit = strdup (param->unit);
	}
	paramNew->valnum = param->valnum;
	paramNew->min = param->min;
	paramNew->max = param->max;
    } else {
	paramNew->type = P_STR;
	FREEZ (paramNew->value);
	paramNew->value = strdup (param->value);
	within = GF_TAILQ_FIRST (&(param->withinList));
	while (within) {
	    addWithin (paramNew, within->val);
	    within = GF_TAILQ_NEXT (within, linkWithin);
	}
    }
}


/** Merge two parameters sets into a new one.
    @ingroup	paramsfile
    @param	ref	reference handle
    @param	tgt	target handle for merge
    @param	mode	merge mode, can be any combination of:
		<br>#GFPARM_MMODE_SRC Use ref and modify existing parameters with tgt
		<br>#GFPARM_MMODE_DST Use tgt and verify ref parameters
		<br>#GFPARM_MMODE_RELSRC Release ref after the merge
		<br>#GFPARM_MMODE_RELDST Release tgt after the merge
    @return	The new handle containing the merge.
    @see	GfParmCheckHandle
 */
void *
GfParmMergeHandles(void *ref, void *tgt, int mode)
{
    struct parmHandle	*parmHandleRef = (struct parmHandle *)ref;
    struct parmHandle	*parmHandleTgt = (struct parmHandle *)tgt;
    struct parmHandle	*parmHandleOut;
    struct parmHeader	*confRef;
    struct parmHeader	*confTgt;
    struct parmHeader	*confOut;
    struct section	*curSectionRef;
    struct section	*nextSectionRef;
    struct section	*curSectionTgt;
    struct section	*nextSectionTgt;
    struct param	*curParamRef;
    struct param	*curParamTgt;

    //GfLogTrace ("Merging \"%s\" and \"%s\" (%s - %s)\n", confRef->filename, confTgt->filename, ((mode & GFPARM_MMODE_SRC) ? "SRC" : ""), ((mode & GFPARM_MMODE_DST) ? "DST" : ""));

	if ((parmHandleRef == NULL) || (parmHandleRef->magic != PARM_MAGIC)) {
		GfLogError ("GfParmMergeHandles: bad handle (%p)\n", parmHandleRef);
   	    return NULL;
	}

	if ((parmHandleTgt == NULL) || (parmHandleTgt->magic != PARM_MAGIC)) {
		GfLogError ("GfParmMergeHandles: bad handle (%p)\n", parmHandleTgt);
   	    return NULL;
	}

    confRef = parmHandleRef->conf;
    confTgt = parmHandleTgt->conf;

        /* Conf Header creation */
    confOut = createParmHeader ("");
    if (!confOut) {
	GfLogError ("GfParmMergeHandles: conf header creation failed\n");
	return NULL;
    }

    /* Handle creation */
    parmHandleOut = (struct parmHandle *) calloc (1, sizeof (struct parmHandle));
    if (!parmHandleOut) {
#if defined(_MSC_VER) && _MSC_VER < 1800
	GfLogError ("GfParmMergeHandles: calloc (1, %03Iu) failed\n", sizeof (struct parmHandle));
#else //_MSC_VER
	GfLogError ("GfParmMergeHandles: calloc (1, %zu) failed\n", sizeof (struct parmHandle));
#endif //_MSC_VER
	parmReleaseHeader (confOut);
	return NULL;
    }

    parmHandleOut->magic = PARM_MAGIC;
    parmHandleOut->conf = confOut;
    parmHandleOut->flag = PARM_HANDLE_FLAG_PRIVATE;
    
    if (mode & GFPARM_MMODE_SRC) {
	/* Traverse all the reference tree */
	curSectionRef = GF_TAILQ_FIRST (&(confRef->rootSection->subSectionList));
	while (curSectionRef) {
	    curParamRef = GF_TAILQ_FIRST (&(curSectionRef->paramList));
	    while (curParamRef) {
		/* compare params */
		curParamTgt = getParamByName (confTgt, curSectionRef->fullName, curParamRef->name, 0);
		if (curParamTgt) {
		    insertParamMerge (parmHandleOut, curSectionRef->fullName, curParamRef, curParamTgt);
		} else {
		    insertParam (parmHandleOut, curSectionRef->fullName, curParamRef);
		}
		curParamRef = GF_TAILQ_NEXT (curParamRef, linkParam);
	    }
	    nextSectionRef = GF_TAILQ_FIRST (&(curSectionRef->subSectionList));
	    if (nextSectionRef) {
		curSectionRef = nextSectionRef;
	    } else {
		nextSectionRef = GF_TAILQ_NEXT (curSectionRef, linkSection);
		while (!nextSectionRef) {
		    nextSectionRef = curSectionRef->parent;
		    if (!nextSectionRef) {
			/* Reached the root */
			break;
		    }
		    curSectionRef = nextSectionRef;
		    nextSectionRef = GF_TAILQ_NEXT (curSectionRef, linkSection);
		}
		curSectionRef = nextSectionRef;
	    }
	}
    }
    
    if (mode & GFPARM_MMODE_DST) {
	/* Traverse all the target tree */
	curSectionTgt = GF_TAILQ_FIRST (&(confTgt->rootSection->subSectionList));
	while (curSectionTgt) {
	    curParamTgt = GF_TAILQ_FIRST (&(curSectionTgt->paramList));
	    while (curParamTgt) {
		/* compare params */
		curParamRef = getParamByName (confRef, curSectionTgt->fullName, curParamTgt->name, 0);
		if (curParamRef) {
		    insertParamMerge (parmHandleOut, curSectionTgt->fullName, curParamRef, curParamTgt);
		} else {
		    insertParam (parmHandleOut, curSectionTgt->fullName, curParamTgt);
		}
		curParamTgt = GF_TAILQ_NEXT (curParamTgt, linkParam);
	    }
	    nextSectionTgt = GF_TAILQ_FIRST (&(curSectionTgt->subSectionList));
	    if (nextSectionTgt) {
		curSectionTgt = nextSectionTgt;
	    } else {
		nextSectionTgt = GF_TAILQ_NEXT (curSectionTgt, linkSection);
		while (!nextSectionTgt) {
		    nextSectionTgt = curSectionTgt->parent;
		    if (!nextSectionTgt) {
			/* Reached the root */
			break;
		    }
		    curSectionTgt = nextSectionTgt;
		    nextSectionTgt = GF_TAILQ_NEXT (curSectionTgt, linkSection);
		}
		curSectionTgt = nextSectionTgt;
	    }
	}
    }

    if (mode & GFPARM_MMODE_RELSRC) {
	GfParmReleaseHandle(ref);
    }

    if (mode & GFPARM_MMODE_RELDST) {
	GfParmReleaseHandle(tgt);
    }

    GF_TAILQ_INSERT_HEAD (&parmHandleList, parmHandleOut, linkHandle);
 
    return (void*)parmHandleOut;
}

/** Merge two parameters sets into a new one.
    @ingroup	paramsfile
    @param		ref	reference handle
    @fileName	filename of the new parameter file
    @return	The new handle containing the merge.
    @see	GfParmCheckHandle
 */
void* GfParmMergeFiles(void * params, const char* fileName)
{
  void * newParams =							 // Open setup file
	GfParmReadFile(fileName, GFPARM_RMODE_STD);

  if(newParams == NULL)                          // Return old one,
    return params;                               //   if new one is empty

  if(params == NULL)                             // Return new one,
    return newParams;                            //   if old one is empty

  return GfParmMergeHandles(params, newParams,   // Merge setup files
    GFPARM_MMODE_SRC
	| GFPARM_MMODE_DST
	| GFPARM_MMODE_RELSRC
	| GFPARM_MMODE_RELDST);
} // GfParamMergeFile

/** Get the min and max of a numerical parameter.
    @ingroup	paramsdata
    @param	handle	handle of parameters	
    @param	path	path of the attribute
    @param	key	key name	
    @param	min	Receives the min value
    @param	max	Receives the max value
    @return	0 Ok
		<br>-1 Parameter not existing
 */
int
GfParmGetNumBoundaries(void *handle, char *path, char *key, tdble *min, tdble *max)
{
    struct parmHandle *parmHandle = (struct parmHandle *)handle;
    struct parmHeader *conf;
    struct param	*param;

	if ((parmHandle == NULL) || (parmHandle->magic != PARM_MAGIC)) {
		GfLogError ("GfParmGetNumBoundaries: bad handle (%p)\n", parmHandle);
   	    return -1;
	}

    conf = parmHandle->conf;
    
    param = getParamByName (conf, path, key, 0);
    if (!param || (param->type != P_NUM)) {
	return -1;
    }

    *min = param->min;
    *max = param->max;

    return 0;
}


/*
 * 
 * name: safeFOpen
 * Safely opens a file (useful for writing a file).
 * If the directory where the file should reside does not exists,
 * it creates each directory on the given path, step by step.
 * If the file exists, is read-only and we want to write it,
 * try and remove the read-only attribute.
 * 
 * Note: needs <errno.h> and <sys/stat.h> (+ <io.h> under Ruin'dows)
 * 
 * @param fileName: full path of the file to be opened (for writing)
 * @param mode: open mode
 * @return: FILE *, handler for the file just opened. NULL if failed.
 */

// mkdir with u+rwx access rights by default
#ifndef WIN32
# ifdef mkdir
#  undef mkdir
# endif
# define mkdir(x) mkdir((x), S_IRWXU)
#endif

FILE *
safeFOpen(const char *fileName, const char *mode)
{
    // Try first normal fopen (very likely to work well => if OK, nothing more to do)
    FILE* file = fopen(fileName, mode);
    if (file)
		return file;

	// If it failed, may be its because we want to write it, but it is read-only ?
	struct stat st;
	if ((strchr(mode, 'w') || strchr(mode, 'a')) && !stat(fileName, &st)) {
		if (chmod(fileName, 0640)) {
			const int errnum = errno; // Get errno before it is overwritten by some system call.
			GfLogWarning("Failed to set 0640 attributes to %s (%s)\n",
						 fileName, strerror(errnum));
		}
		file = fopen(fileName, mode);
		if (file)
			return file;
	}
	
    // Otherwise, try and create parent dirs in case it is the cause of the error :
    char *pszDirName = GfFileGetDirName(fileName);
	if (GfDirCreate(pszDirName) != GF_DIR_CREATED)
		GfLogWarning("Failed to create parent dir(s) of %s\n", fileName);
	free(pszDirName);
	
	// And finally try again to open the file.
    return fopen(fileName, mode);
	
}//safeFOpen
