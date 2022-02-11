/***************************************************************************

    file                 : grloadac.cpp
    copyright            : (C) 2000 by Eric Espie
    web                  : www.speed-dreams.org
    version              : $Id: grloadac.cpp 6856 2020-03-21 21:02:42Z torcs-ng $

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* A adapted version of the PLib AC3D loader */

/*
     PLIB - A Suite of Portable Game Libraries
     Copyright (C) 2001  Steve Baker

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.

     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the Free
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA

     For further information visit http://plib.sourceforge.net
*/

#include <zlib.h> // gz*
#include <plib/ssg.h>

#include "grutil.h"
#include "grloadac.h"
#include "grssgext.h"
#include "grvtxtable.h"
#include "grmain.h"
#include "grtexture.h"	//doMipMap, cgrSimpleState, cgrMultiTexState


// API for transparently loading .ac/.acc files, gzip-compressed or not.
#define FGETS(buf, len, file) gzgets(file, buf, len)
#define FGETC(file) gzgetc(file)
#define FOPEN(path, mode) gzopen(path, mode)
#define FCLOSE(fd) gzclose(fd)

// Car to track X,Y bounding boxes ratios (see grcar.cpp).
// Warning: Computed in grssgCarLoadAC3D from t_* of the loaded car and of the last loaded track
//          (so, it works only if the track is loaded before all the cars).
// TODO: Do the computation elsewhere, without such assumptions.
double carTrackRatioX = 0;
double carTrackRatioY = 0;

// X,Y bounding box of the last loaded track model.
// TODO: Store this elsewhere (in the car data if a car, track data is a track, ...).
double shad_xmax;
double shad_ymax;
double shad_xmin;
double shad_ymin;

// AC3D loader own state ==============================================================
// File descriptor.
static gzFile loader_fd;

// X, Y bounding box of the loaded model, whichever it is (car, track, ...).
double t_xmax;
double t_ymax;
double t_xmin;
double t_ymin;


struct _ssgMaterial
{
  sgVec4 spec;
  sgVec4 emis;
  sgVec4 amb;
  sgVec4 rgb;
  float  shi;
} ;

static int num_materials = 0;

static sgVec3 *vtab  = 0;
static sgVec3 *ntab  = 0;

static sgVec2 *t0tab = 0;
static sgVec2 *t1tab = 0;
static sgVec2 *t2tab = 0;
static sgVec2 *t3tab = 0;

static ssgIndexArray *vertlist = 0;
static ssgIndexArray *striplist = 0;

static int totalnv = 0;
static int totalstripe = 0;
static int usenormal = FALSE;
static int nv = 0;

static int isacar = FALSE;
static int isawheel = FALSE;
static int isawindow = FALSE;
static int usestrip = FALSE;
static int usegroup = FALSE;
static int inGroup = FALSE;

static int numTexMaps;  // Number of active texture maps.
static unsigned bfTexMaps; // 1 bit for each active texture map.

static int indexCar;

static grssgLoaderOptions	*current_options = 0 ;
static _ssgMaterial	*current_material = 0 ;
static sgVec4		*current_colour = 0 ;
static ssgBranch	*current_branch = 0 ;
static char		*current_tfname = 0 ;
static char		*current_tbase = 0 ;
static char		*current_ttiled = 0 ;
static char		*current_tskids = 0 ;
static char		*current_tshad = 0 ;
static char		*current_data = 0 ;

static int last_num_kids = -1 ;
static int current_flags = -1 ;

#define MAX_MATERIALS 1000    /* This *ought* to be enough! */
static _ssgMaterial   *mlist    [ MAX_MATERIALS ] ;
static sgVec4         *clist    [ MAX_MATERIALS ] ;

static sgMat4 current_matrix ;
static sgVec2 texrep ;
static sgVec2 texoff ;

// Parser state automaton "call-backs" ===================================================
static int do_material ( char *s ) ;
static int do_object   ( char *s ) ;
static int do_name     ( char *s ) ;
static int do_data     ( char *s ) ;
static int do_texture  ( char *s ) ;
static int do_texrep   ( char *s ) ;
static int do_texoff   ( char *s ) ;
static int do_rot      ( char *s ) ;
static int do_loc      ( char *s ) ;
static int do_url      ( char *s ) ;
static int do_numvert  ( char *s ) ;
static int do_numsurf  ( char *s ) ;
static int do_surf     ( char *s ) ;
static int do_mat      ( char *s ) ;
static int do_refs     ( char *s ) ;
static int do_kids     ( char *s ) ;
static int do_ignore   ( char *s ) ;

static int do_obj_world ( char *s ) ;
static int do_obj_poly  ( char *s ) ;
static int do_obj_group ( char *s ) ;
static int do_obj_light ( char *s ) ;

// Parser internal utility functions ======================================================

static void skip_spaces ( char **s )
{
  while ( **s == ' ' || **s == '\t' || **s == '\r' )
    (*s)++ ;
}


static void skip_quotes ( char **s )
{
  skip_spaces ( s ) ;

  if ( **s == '\"' )
  {
    (*s)++ ;

    char *t = *s ;

    while ( *t != '\0' && *t != '\"' )
      t++ ;

    if ( *t != '\"' )
      ulSetError ( UL_WARNING, "ac_to_gl: Mismatched double-quote ('\"') in '%s'", *s ) ;

    *t = '\0' ;
  }
  else
    ulSetError ( UL_WARNING, "ac_to_gl: Expected double-quote ('\"') in '%s'", *s ) ;
}

#define PARSE_CONT   0
#define PARSE_POP    1

struct Tag
{
  const char *token ;
  int (*func) ( char *s ) ;
} ;

static int search ( Tag *tags, char *s )
{
  skip_spaces ( & s ) ;

  if ( *s == '\0' )
    return PARSE_CONT; /* skip empty lines */

  for ( int i = 0 ; tags[i].token ; i++ )
    if ( ulStrNEqual ( tags[i].token, s, strlen(tags[i].token) ) )
    {
      s += strlen ( tags[i].token ) ;

      skip_spaces ( & s ) ;

      return (*(tags[i].func))( s ) ;
    }

  ulSetError ( UL_FATAL, "ac_to_gl: Unrecognised token '%s' (%d)", s , strlen(s)) ;

  return 0 ;  /* Should never get here */
}

static ssgState *get_simple_state ( _ssgMaterial *mat )
{
  // GfLogDebug("get_simple_state(%s) : Object '%s'\n",
  // 			 (current_tfname ? current_tfname : "<nul>"), current_branch->getName());

  cgrSimpleState *st = grStateFactory->getSimpleState();

  st->setMaterial ( GL_SPECULAR, mat->spec ) ;
  st->setMaterial ( GL_EMISSION, mat->emis ) ;
  st->setMaterial ( GL_AMBIENT_AND_DIFFUSE, mat->amb ) ;
  st->setShininess ( mat->shi ) ;

  st->enable ( GL_COLOR_MATERIAL ) ;
  st->setColourMaterial ( GL_AMBIENT_AND_DIFFUSE ) ;

  st->enable  ( GL_LIGHTING ) ;

  st->setShadeModel ( GL_SMOOTH ) ;

  st->setAlphaClamp(0);

  if (isawindow)
  {
      st->enable  ( GL_BLEND );
      st->setTranslucent () ;
  }
  else if (isacar || isawheel)
  {
      st->enable  ( GL_BLEND );
      st->setOpaque () ;
  }
  else if ( mat->rgb[3] < 0.99 )
  {
      st->enable ( GL_ALPHA_TEST ) ;
      st->enable  ( GL_BLEND ) ;
      st->setTranslucent () ;
  }
  else
  {
      st->disable ( GL_BLEND ) ;
      st->setOpaque () ;
  }

  if (current_tfname)
  {
      st->setTexture( current_options->createTexture(current_tfname) ) ;
      st->enable( GL_TEXTURE_2D ) ;

      if (strstr(current_tfname,"tree")
          || strstr(current_tfname,"trans-")
          || strstr(current_tfname,"arbor"))
      {
          st->setAlphaClamp(0.65f);
          st->enable ( GL_ALPHA_TEST ) ;
          st->enable  ( GL_BLEND );
      }
  }
  else
  {
      st->disable ( GL_BLEND ) ;
      st->disable( GL_TEXTURE_2D ) ;
  }

  return st ;
}

static cgrMultiTexState *get_multi_texture_state
                          (char* name,
                           cgrMultiTexState::tfnTexScheme fnTexScheme = cgrMultiTexState::modulate)
{
    // GfLogDebug("get_multi_texture_state(%s) : Object '%s'\n",
    // 		   (name ? name : "<nul>"), current_branch->getName());

    if (!name)
        return 0;

    cgrMultiTexState *st = grStateFactory->getMultiTexState(fnTexScheme);

    st->disable(GL_BLEND);
    st->setOpaque();

    if (name)
    {
        st->setTexture(current_options->createTexture(name));
        st->enable(GL_TEXTURE_2D) ;

        // Special case of tree and ?transparent? textures.
        if (strstr(current_tfname, "tree")
            || strstr(current_tfname, "trans-")
            || strstr(current_tfname, "arbor"))
        {
            st->enable(GL_BLEND);
            st->setAlphaClamp(0.7f);
            st->enable(GL_ALPHA_TEST);
        }
    }
    else
    {
        st->disable(GL_BLEND);
        st->disable(GL_TEXTURE_2D);
    }

    return st ;
}

#define TEXMAP_BASE    (1<<0)
#define TEXMAP_TILED   (1<<1)
#define TEXMAP_SKIDS   (1<<2) // Actually the "raceline" layer.
#define TEXMAP_SHADOWS (1<<3)
#define TEXMAP_ALL     (TEXMAP_BASE | TEXMAP_TILED | TEXMAP_SKIDS | TEXMAP_SHADOWS)

static void setup_vertex_table_states(cgrVtxTable* vtab)
{
    vtab->setState ( get_simple_state ( current_material ) ) ;
    vtab->setCullFace ( ! ( (current_flags>>4) & 0x02 ) ) ;

    // Don't create multi-texture states for cars (done differently).
    // TODO: Move this car-specific multi-texture states setup here, for consistency ?
    if (!isacar && !isawheel)
    {
        if (bfTexMaps & TEXMAP_TILED)
            vtab->setMultiTexState (0, get_multi_texture_state (current_ttiled ));
        if (bfTexMaps & TEXMAP_SKIDS)
            vtab->setMultiTexState (1, get_multi_texture_state (current_tskids ));
        if (bfTexMaps & TEXMAP_SHADOWS)
            vtab->setMultiTexState (2, get_multi_texture_state (current_tshad ));
    }
}

// Parser state automaton state/transition data ============================================
static Tag top_tags [] =
{
  { "MATERIAL", do_material },
  { "OBJECT"  , do_object   },
} ;


static Tag object_tags [] =
{
  { "name"    , do_name     },
  { "data"    , do_data     },
  { "texture" , do_texture  },
  { "texrep"  , do_texrep   },
  { "texoff"  , do_texoff   },
  { "rot"     , do_rot      },
  { "loc"     , do_loc      },
  { "url"     , do_url      },
  { "numvert" , do_numvert  },
  { "numsurf" , do_numsurf  },
  { "kids"    , do_kids     },
  { "crease"  , do_ignore   },
  { "shader"  , do_ignore   },
  { 0, 0 }
} ;

static Tag surf_tag [] =
{
  { "SURF"    , do_surf     },
  { 0, 0 }
} ;

static Tag surface_tags [] =
{
  { "mat"     , do_mat      },
  { "refs"    , do_refs     },
  { 0, 0 }
} ;

static Tag obj_type_tags [] =
 {
   { "world", do_obj_world },
   { "poly" , do_obj_poly  },
   { "group", do_obj_group },
   { "light", do_obj_light },
   { 0, 0 }
 } ;

// Parser call-backs and internal functions ===============================================
#define OBJ_WORLD  0
#define OBJ_POLY   1
#define OBJ_GROUP  2
#define OBJ_LIGHT  3

static int do_obj_world ( char * ) { return OBJ_WORLD ; }
static int do_obj_poly  ( char * ) { return OBJ_POLY  ; }
static int do_obj_group ( char * ) { return OBJ_GROUP ; }
static int do_obj_light ( char * ) { return OBJ_LIGHT ; }

static int do_material ( char *s )
{
  char name [ 1024 ] ;
  sgVec4 rgb  ;
  sgVec4 amb  ;
  sgVec4 emis ;
  sgVec4 spec ;
  int   shi ;
  float trans ;

  //#537: Limiting parsing 1023 bytes to fit in "name", as we have
  // no info how long a string we received as param "s"
  if ( sscanf ( s,
  "%1023s rgb %f %f %f amb %f %f %f emis %f %f %f spec %f %f %f shi %d trans %f",
    name,
    &rgb [0], &rgb [1], &rgb [2],
    &amb [0], &amb [1], &amb [2],
    &emis[0], &emis[1], &emis[2],
    &spec[0], &spec[1], &spec[2],
    &shi,
    &trans ) != 15 )
  {
    //#537: Limiting error message length, as PLIB error handler
    // can store only 1024 bytes
    ulSetError ( UL_WARNING, "grloadac:do_material: Can't parse this MATERIAL: %512s", s ) ;
  }
  else
  {
    char *nm = name ;

    skip_quotes ( &nm ) ;

    amb [ 3 ] = emis [ 3 ] = spec [ 3 ] = 1.0f ;
    rgb [ 3 ] = 1.0f - trans ;

    mlist [ num_materials ] = new _ssgMaterial ;
    clist [ num_materials ] = new sgVec4 [ 1 ] ;

    sgCopyVec4 ( clist [ num_materials ][ 0 ], rgb ) ;

    current_material = mlist [ num_materials ] ;
    sgCopyVec4 ( current_material->spec, spec ) ;
    sgCopyVec4 ( current_material->emis, emis ) ;
    sgCopyVec4 ( current_material->rgb , rgb  ) ;
    sgCopyVec4 ( current_material->amb , amb  ) ;
    current_material->shi = (float) shi ;
  }

  num_materials++ ;
  return PARSE_CONT ;
}


static int do_object   ( char * s  )
{
    int obj_type = search(obj_type_tags, s);

    delete [] current_tfname;
    current_tfname = 0;

    char buffer[1024];

    sgSetVec2(texrep, 1.0f, 1.0f);
    sgSetVec2(texoff, 0.0f, 0.0f);

    sgMakeIdentMat4 ( current_matrix ) ;

    ssgEntity *old_cb = current_branch ;

    if (obj_type == OBJ_GROUP) {
        ssgBranch *current_branch_g = 0;
        inGroup = TRUE;
        current_branch_g = new ssgBranchCb();
        current_branch->addKid(current_branch_g);
        current_branch = (ssgTransform*) current_branch_g;

        extern int preScene(ssgEntity *e);
        current_branch_g->setCallback(SSG_CALLBACK_PREDRAW, preScene);
    } else {
        inGroup=FALSE;
    }

    ssgTransform *tr = new ssgTransform () ;

    tr->setTransform ( current_matrix ) ;

    current_branch->addKid ( tr ) ;
    current_branch = tr ;

    while ( FGETS ( buffer, 1024, loader_fd ) )
        if ( search ( object_tags, buffer ) == PARSE_POP )
            break ;

    int num_kids = last_num_kids ;

    for ( int i = 0 ; i < num_kids ; i++ ) {
        /* EE: bad hack for buggy .acc format... */
        if (FGETS ( buffer, 1024, loader_fd ) )
            search ( top_tags, buffer ) ;
        else
            break;
    }

    current_branch = (ssgBranch *) old_cb ;
    return PARSE_CONT ;
}


static int do_name ( char *s )
{
  char *q=0;
  skip_quotes ( &s ) ;

  // Detect special objects
  isawindow = strncmp(s, "WI", 2) ? FALSE : TRUE;

  if (strstr(s,"__TKMN"))
    usegroup=TRUE;

  if (!strncmp(s, "TKMN",4))
  {
    q=strstr(s,"_g");
    if (q)
      *q='\0';
  }

  if (!strncmp(s, "DR", 2)) {
      current_branch->setName ( "DRIVER" );
  } else {
      current_branch->setName ( s ) ;
  }

  return PARSE_CONT ;
}


static int do_data     ( char *s )
{
  int len = strtol ( s, 0, 0 ) ;

  current_data = new char [ len + 1 ] ;

  for ( int i = 0 ; i < len ; i++ )
    current_data [ i ] = FGETC ( loader_fd ) ;

  current_data [ len ] = '\0' ;

  FGETC ( loader_fd ) ;  /* Final RETURN */

  ssgBranch *br = current_options->createBranch ( current_data ) ;

  if ( br )
  {
    current_branch->addKid ( br ) ;
    current_branch = br ;
  }

  current_data = 0 ;

  return PARSE_CONT ;
}

#define NOTEXTURE "empty_texture_no_mapping"

static int do_texture  ( char *s )
{
  char *p ;

  if ( s == 0 || s[0] == '\0')
    current_tfname = 0 ;
  else
  {
    if ((p=strstr(s," base")))
    {
      *p='\0';
      numTexMaps = 1;
      bfTexMaps = TEXMAP_BASE;
      delete [] current_tbase ;
      delete [] current_tfname ;
      delete [] current_ttiled ;
      current_ttiled = 0;
      delete [] current_tskids ;
      current_tskids = 0;
      delete [] current_tshad ;
      current_tshad = 0;
      skip_quotes ( &s ) ;
      if (current_options->textureMapping())
      {
        const char* pszNewTex = current_options->mapTexture( s );
        current_tbase = new char [ strlen(pszNewTex)+1 ] ;
        strcpy ( current_tbase, pszNewTex ) ;
        current_tfname = new char [ strlen(pszNewTex)+1 ] ;
        strcpy ( current_tfname, pszNewTex ) ;
      }
      else
      {
        current_tbase = new char [ strlen(s)+1 ] ;
        current_tfname = new char [ strlen(s)+1 ] ;
        strcpy ( current_tbase, s ) ;
        strcpy ( current_tfname, s ) ;
      }
    }
    else  if ((p=strstr(s," tiled")))
    {
      *p='\0';
      delete [] current_ttiled ;
      current_ttiled=0;
      delete [] current_tskids ;
      current_tskids = 0;
      delete [] current_tshad ;
      current_tshad = 0;
      if (!strstr(s,NOTEXTURE) && numTexMaps < grMaxTextureUnits)
      {
        numTexMaps++;;
        bfTexMaps |= TEXMAP_TILED;
        skip_quotes ( &s ) ;
        if (current_options->textureMapping())
        {
          const char* pszNewTex = current_options->mapTexture( s );
          current_ttiled = new char [ strlen(pszNewTex)+1 ] ;
          strcpy ( current_ttiled, pszNewTex ) ;
        }
        else
        {
          current_ttiled = new char [ strlen(s)+1 ] ;
          strcpy ( current_ttiled, s ) ;
        }
      }
    }
    else  if ((p=strstr(s," skids")))
    {
      *p='\0';
      delete [] current_tskids ;
      current_tskids = 0;
      delete [] current_tshad ;
      current_tshad = 0;
      if (!strstr(s,NOTEXTURE) && numTexMaps < grMaxTextureUnits)
      {
        numTexMaps++;;
        bfTexMaps |= TEXMAP_SKIDS;
        skip_quotes ( &s ) ;
        if (current_options->textureMapping())
        {
          const char* pszNewTex = current_options->mapTexture( s );
          current_tskids = new char [ strlen(pszNewTex)+1 ] ;
          strcpy ( current_tskids, pszNewTex ) ;
        }
        else
        {
          current_tskids = new char [ strlen(s)+1 ] ;
          strcpy ( current_tskids, s ) ;
        }
      }
    }
    else  if ((p=strstr(s," shad")))
    {
      *p='\0';
      delete [] current_tshad ;
      current_tshad = 0;
      if (!strstr(s,NOTEXTURE) && numTexMaps < grMaxTextureUnits)
      {
        numTexMaps++;;
        bfTexMaps |= TEXMAP_SHADOWS;
        skip_quotes ( &s ) ;
        if (current_options->textureMapping())
        {
          const char* pszNewTex = current_options->mapTexture( s );
          current_tshad = new char [ strlen(pszNewTex)+1 ] ;
          strcpy ( current_tshad, pszNewTex ) ;
        }
        else
        {
          current_tshad = new char [ strlen(s)+1 ] ;
          strcpy ( current_tshad, s ) ;
        }
      }
    }
    else
    {
      skip_quotes ( &s ) ;
      numTexMaps = 1;
      bfTexMaps = TEXMAP_BASE;
      delete [] current_tfname ;
      delete [] current_tbase ;
      current_tbase = 0;
      delete [] current_ttiled ;
      current_ttiled = 0;
      delete [] current_tskids ;
      current_tskids = 0;
      delete [] current_tshad ;
      current_tshad = 0;
      if (current_options->textureMapping())
      {
        const char* pszNewTex = current_options->mapTexture( s );
        current_tfname = new char [ strlen(pszNewTex)+1 ] ;
        strcpy ( current_tfname, pszNewTex ) ;
      }
      else
      {
        current_tfname = new char [ strlen(s)+1 ] ;
        strcpy ( current_tfname, s ) ;
      }
    }
  }

  return PARSE_CONT ;
}


static int do_texrep ( char *s )
{
  if ( sscanf ( s, "%f %f", & texrep [ 0 ], & texrep [ 1 ] ) != 2 )
  {
    ulSetError ( UL_WARNING, "ac_to_gl: Illegal texrep record." ) ;
  }

  return PARSE_CONT ;
}


static int do_texoff ( char *s )
{
  if ( sscanf ( s, "%f %f", & texoff [ 0 ], & texoff [ 1 ] ) != 2 )
    ulSetError ( UL_WARNING, "ac_to_gl: Illegal texoff record." ) ;

  return PARSE_CONT ;
}

static int do_rot ( char *s )
{
  current_matrix [ 0 ][ 3 ] = current_matrix [ 1 ][ 3 ] = current_matrix [ 2 ][ 3 ] =
    current_matrix [ 3 ][ 0 ] = current_matrix [ 3 ][ 1 ] = current_matrix [ 3 ][ 2 ] = 0.0f ;
  current_matrix [ 3 ][ 3 ] = 1.0f ;

  if ( sscanf ( s, "%f %f %f %f %f %f %f %f %f",
        & current_matrix [ 0 ] [ 0 ], & current_matrix [ 0 ] [ 1 ], & current_matrix [ 0 ] [ 2 ],
        & current_matrix [ 1 ] [ 0 ], & current_matrix [ 1 ] [ 1 ], & current_matrix [ 1 ] [ 2 ],
        & current_matrix [ 2 ] [ 0 ], & current_matrix [ 2 ] [ 1 ], & current_matrix [ 2 ] [ 2 ] ) != 9 )
    ulSetError ( UL_WARNING, "ac_to_gl: Illegal rot record." ) ;

  ((ssgTransform *)current_branch)->setTransform ( current_matrix ) ;
  return PARSE_CONT ;
}

static int do_loc      ( char *s )
{
  if ( sscanf ( s, "%f %f %f", & current_matrix [ 3 ][ 0 ], & current_matrix [ 3 ][ 2 ], & current_matrix [ 3 ][ 1 ] ) != 3 )
    ulSetError ( UL_WARNING, "ac_to_gl: Illegal loc record." ) ;

  current_matrix [ 3 ][ 1 ] = - current_matrix [ 3 ][ 1 ] ;
  current_matrix [ 3 ][ 3 ] = 1.0f ;
  ((ssgTransform *)current_branch)->setTransform ( current_matrix ) ;

  return PARSE_CONT ;
}

static int do_url      ( char *s )
{
  skip_quotes ( & s ) ;

#ifdef PRINT_URLS
  printf ( "/* URL: \"%s\" */\n", s ) ;
#endif

  return PARSE_CONT ;
}

static int do_numvert  ( char *s )
{
  char buffer [ 1024 ] ;

  nv = strtol ( s, 0, 0 ) ;

  delete [] vtab ;
  delete [] ntab ;
  delete [] t0tab ;
  delete [] t1tab ;
  delete [] t2tab ;
  delete [] t3tab ;
  totalnv=nv;
  totalstripe=0;

  vtab = new sgVec3 [ nv ] ;
  ntab = new sgVec3 [ nv ] ;
  t0tab = new sgVec2 [ nv ] ;
  t1tab = new sgVec2 [ nv ] ;
  t2tab = new sgVec2 [ nv ] ;
  t3tab = new sgVec2 [ nv ] ;
  vertlist=new ssgIndexArray ();
  striplist=new ssgIndexArray ();

  for ( int i = 0 ; i < nv ; i++ )
  {
    FGETS ( buffer, 1024, loader_fd ) ;

    if ( sscanf ( buffer, "%f %f %f %f %f %f",
                          &vtab[i][0], &vtab[i][1], &vtab[i][2],&ntab[i][0], &ntab[i][1], &ntab[i][2] ) != 6 )
      {
    usenormal = FALSE;
    if ( sscanf ( buffer, "%f %f %f",
              &vtab[i][0], &vtab[i][1], &vtab[i][2] ) != 3 )
      {
        ulSetError ( UL_FATAL, "ac_to_gl: Illegal vertex record." ) ;
      }
      }
    else
      {
    usenormal = TRUE;
    float tmp  =  ntab[i][1] ;
    ntab[i][1] = -ntab[i][2] ;
    ntab[i][2] = tmp ;
      }

    float tmp  =  vtab[i][1] ;
    vtab[i][1] = -vtab[i][2] ;
    vtab[i][2] = tmp ;

    if (vtab[i][0] >t_xmax)
      t_xmax=vtab[i][0];
    if (vtab[i][0] <t_xmin)
      t_xmin=vtab[i][0];

    if (vtab[i][1] >t_ymax)
      t_ymax=vtab[i][1];
    if (vtab[i][1] <t_ymin)
      t_ymin=vtab[i][1];

  }

  return PARSE_CONT ;
}

static int do_numsurf  ( char *s )
{
  int ns = strtol ( s, 0, 0 ) ;

  for ( int i = 0 ; i < ns ; i++ )
  {
    char buffer [ 1024 ] ;

    FGETS ( buffer, 1024, loader_fd ) ;
    search ( surf_tag, buffer ) ;
  }

  return PARSE_CONT ;
}

static int do_surf     ( char *s )
{
  current_flags = strtol ( s, 0, 0 ) ;

  char buffer [ 1024 ] ;

  while ( FGETS ( buffer, 1024, loader_fd ) )
    if ( search ( surface_tags, buffer ) == PARSE_POP )
      break ;

  return PARSE_CONT ;
}


static int do_mat ( char *s )
{
  int mat = strtol ( s, 0, 0 ) ;

  current_material = mlist [ mat ] ;
  current_colour   = clist [ mat ] ;

  return PARSE_CONT ;
}


static int do_refs( char *s )
{
    int nrefs = strtol( s, 0, 0 );
    char buffer[1024];

    if (nrefs == 0) {
        return PARSE_POP ;
    }

    // GfLogDebug("do_refs(nm=%d, m=0x%X) : ...\n", numTexMaps, bfTexMaps);

    // Force bfTexMaps for cars (texture states not read from the .ac/.acc).
    if (isacar) {
        numTexMaps = 1;
        bfTexMaps = TEXMAP_BASE;
        for (int nTUIndex = 1; nTUIndex < MIN(grMaxTextureUnits, 4); nTUIndex++) {
            numTexMaps++;
            bfTexMaps |= (1<<nTUIndex);
        }
    } else if (isawheel) {
        numTexMaps = 1;
        bfTexMaps = TEXMAP_BASE;
    }

    ssgVertexArray *vlist = new ssgVertexArray(nrefs);
    ssgTexCoordArray *tlist = new ssgTexCoordArray (nrefs);
    ssgTexCoordArray *tlist1 = (bfTexMaps & (1<<1)) ? new ssgTexCoordArray(nrefs) : 0;
    ssgTexCoordArray *tlist2 = (bfTexMaps & (1<<2)) ? new ssgTexCoordArray(nrefs) : 0;
    ssgTexCoordArray *tlist3 = (bfTexMaps & (1<<3)) ? new ssgTexCoordArray(nrefs) : 0;
    ssgNormalArray *nrm = new ssgNormalArray(nrefs);

    for (int i = 0; i < nrefs; i++)
    {
        FGETS(buffer, 1024, loader_fd);

        int vtx;
        sgVec2 tc;
        sgVec2 tc1 = {0};
        sgVec2 tc2 = {0};
        sgVec2 tc3 = {0};
        int tn =
            sscanf ( buffer, "%d %f %f %f %f %f %f %f %f", &vtx,
                     &tc[0],&tc[1], &tc1[0],&tc1[1], &tc2[0],&tc2[1], &tc3[0],&tc3[1]);

        if (tn < 3 )
        {
            ulSetError ( UL_FATAL, "ac_to_gl: Illegal ref record not enough text coord." ) ;
        }

        tc[0] *= texrep[0] ;
        tc[1] *= texrep[1] ;
        tc[0] += texoff[0] ;
        tc[1] += texoff[1] ;

        tlist->add ( tc ) ;
        t0tab[vtx][0]=tc[0];
        t0tab[vtx][1]=tc[1];

        t1tab[vtx][0]=tc1[0];
        t1tab[vtx][1]=tc1[1];

        t2tab[vtx][0]=tc2[0];
        t2tab[vtx][1]=tc2[1];

        t3tab[vtx][0]=tc3[0];
        t3tab[vtx][1]=tc3[1];

        if (tlist1)
            tlist1->add(tc1);
        if (tlist2)
            tlist2->add(tc2);
        if (tlist3)
            tlist3->add(tc3);

        vlist->add(vtab[vtx]);
        if (usenormal)
            nrm->add(ntab[vtx]);
        vertlist->add(vtx);
    }

#ifdef GUIONS
    if (usenormal)
        printf("use normal\n");
#endif /* GUIONS */

    ssgColourArray *col = new ssgColourArray(1);

    col->add(*current_colour);

    if (!usenormal)
    {
        sgVec3 nm;
        if (nrefs < 3)
            sgSetVec3 (nm, 0.0f, 0.0f, 1.0f);
        else
            sgMakeNormal (nm, vlist->get(0), vlist->get(1), vlist->get(2));
        nrm->add ( nm ) ;
    }

    int type = ( current_flags & 0x0F ) ;
    if ( type >= 0 && type <= 4 ) {
        GLenum gltype = GL_TRIANGLES ;
        switch ( type )
        {
            case 0 :
                gltype = GL_TRIANGLE_FAN ;
                break ;
            case 1 :
                gltype = GL_LINE_LOOP ;
                break ;
            case 2 :
                gltype = GL_LINE_STRIP ;
                break ;
            case 4 :
                gltype = GL_TRIANGLE_STRIP ;
                usestrip=TRUE;
                break ;
    }

#ifdef NORMAL_TEST
    /* GUIONS TEST that draw all the normals of a car */
    if(isacar || isawheel) {
        ssgVertexArray *vlinelist = new ssgVertexArray(nv*2);
        for (i = 0; i < nv; i++) {
            sgVec3 tv;
            tv[0] = ntab[i][0]*0.2 + vtab[i][0];
            tv[1] = ntab[i][1]*0.2 + vtab[i][1];
            tv[2] = ntab[i][2]*0.2 + vtab[i][2];
            vlinelist->add(vtab[i]);
            vlinelist->add(tv);
        }
        ssgVtxTable *vline = new ssgVtxTable(GL_LINES, vlinelist, 0, 0, 0);
        current_branch->addKid(current_options->createLeaf(vline, 0));
    }
#endif

    if (!usestrip)
    {
        cgrVtxTable* vtab;
        if (isacar || isawheel)
            vtab = new cgrVtxTableCarPart ( numTexMaps, indexCar, gltype, vlist,
                                            nrm, tlist, tlist1, tlist2, tlist3, col ) ;
        else
            vtab = new cgrVtxTableTrackPart ( numTexMaps, gltype, vlist,
                                              nrm, tlist, tlist1, tlist2, tlist3, col ) ;

        setup_vertex_table_states(vtab);

        ssgLeaf* leaf = current_options->createLeaf ( vtab, 0 ) ;

        if (leaf)
            current_branch->addKid(leaf);
        else
            delete vtab;
    }

    // TODO: Simply avoid setting up vlist, tlist*, nrm ... if we delete them now ?
    //       Through detecting usestrip condition earlier in this function ?
    else
    {
        /* memorize the stripe index */
        striplist-> add (nrefs);
        totalstripe++;
        delete vlist;
        vlist = 0;
        delete tlist;
        tlist = 0;
        delete tlist1;
        tlist1 = 0;
        delete tlist2;
        tlist2 = 0;
        delete tlist3;
        tlist3 = 0;
        delete nrm;
        nrm = 0;
        }
    }

    if (col->getRef() == 0)
        delete col;

    return PARSE_POP ;
}

static int do_kids ( char *s )
{
    last_num_kids = strtol(s, 0, 0);

    if (last_num_kids == 0 && usestrip && !inGroup)
    {
        ssgVertexArray *vlist = new ssgVertexArray(totalnv);
        ssgNormalArray *nrm = new ssgNormalArray(totalnv);

        // Force bfTexMaps for cars (texture states not read from the .ac/.acc).
        if (isacar) {
            numTexMaps = 1;
            bfTexMaps = TEXMAP_BASE;
            for (int nTUIndex = 1; nTUIndex < MIN(grMaxTextureUnits, 4); nTUIndex++) {
                numTexMaps++;
                bfTexMaps |= (1<<nTUIndex);
            }
        } else if (isawheel) {
            numTexMaps = 1;
            bfTexMaps = TEXMAP_BASE;
        }

        // GfLogDebug("do_kids(nm=%d, m=0x%X) : ...\n", numTexMaps, bfTexMaps);

        ssgTexCoordArray *tlist0 = new ssgTexCoordArray(totalnv);
        ssgTexCoordArray *tlist1 = (bfTexMaps & (1<<1)) ? new ssgTexCoordArray(totalnv) : 0;
        ssgTexCoordArray *tlist2 = (bfTexMaps & (1<<2)) ? new ssgTexCoordArray(totalnv) : 0;
        ssgTexCoordArray *tlist3 = (bfTexMaps & (1<<3)) ? new ssgTexCoordArray(totalnv) : 0;

        // TODO: Check if no other faster method (preventing the loop)
        for (int i = 0; i < totalnv; i++)
        {
            tlist0->add ( t0tab[i] ) ;
            if (tlist1)
                tlist1->add ( t1tab[i] ) ;
            if (tlist2)
                tlist2->add ( t2tab[i] ) ;
            if (tlist3)
                tlist3->add ( t3tab[i] ) ;
            vlist->add ( vtab[i] ) ;
            if (usenormal)
                nrm->add ( ntab[i] ) ;
        }

        ssgColourArray *col = new ssgColourArray ( 1 ) ;
        col->add ( *current_colour ) ;

        const GLenum gltype = GL_TRIANGLE_STRIP ;

        cgrVtxTable* vtab;
        if (isacar || isawheel)
            vtab = new cgrVtxTableCarPart ( numTexMaps, indexCar, gltype, vlist,
                                            nrm, tlist0, tlist1, tlist2, tlist3, col,
                                            striplist, totalstripe, vertlist ) ;
        else
            vtab = new cgrVtxTableTrackPart ( numTexMaps, gltype, vlist,
                                              nrm, tlist0, tlist1, tlist2, tlist3, col,
                                              striplist, totalstripe, vertlist ) ;

        setup_vertex_table_states(vtab);

        ssgLeaf* leaf = current_options->createLeaf ( vtab, 0 ) ;

        if (leaf)
            current_branch->addKid ( leaf ) ;
        else
            delete vtab;
    }

    numTexMaps = 1;
    bfTexMaps = TEXMAP_BASE;

    return PARSE_POP ;
}

static int do_ignore( char *s )
{
    return PARSE_CONT ;
}

//================================================================================
// Original PLib loader function for backwards compatibility ...
// (except for ssgLoaderOptions => grssgLoaderOptions)

static ssgEntity *myssgLoadAC ( const char *fname, const grssgLoaderOptions* options )
{

  char filename [ 1024 ] ;
  current_options->makeModelPath ( filename, fname ) ;

  num_materials = 0 ;
  vtab = 0 ;

  current_material = 0 ;
  current_colour   = 0 ;
  current_tfname   = 0 ;
  current_branch   = 0 ;

  sgSetVec2 ( texrep, 1.0, 1.0 ) ;
  sgSetVec2 ( texoff, 0.0, 0.0 ) ;

  loader_fd = FOPEN ( filename, "rb" ) ;

  if ( !loader_fd )
  {
    ulSetError ( UL_WARNING, "ssgLoadAC: Failed to open '%s' for reading", filename ) ;
    return 0 ;
  }

  char buffer [ 1024 ] ;
  int firsttime = TRUE ;

  current_branch = new ssgTransform () ;

  while ( FGETS ( buffer, 1024, loader_fd ) )
  {
    char *s = buffer ;


    /* Skip leading whitespace */

    skip_spaces ( & s ) ;

    /* Skip blank lines and comments */

    if ( *s < ' ' && *s != '\t' ) continue ;
    if ( *s == '#' || *s == ';' ) continue ;

    if ( firsttime )
    {
      firsttime = FALSE ;

      if ( ! ulStrNEqual ( s, "AC3D", 4 ) )
      {
        FCLOSE ( loader_fd ) ;
        ulSetError ( UL_WARNING, "ssgLoadAC: '%s' is not in AC3D format.", filename ) ;
        return 0 ;
      }
    }
    else
      search ( top_tags, s ) ;
  }

  delete [] current_tfname;
  current_tfname = 0 ;
  delete [] vtab ;
  vtab = 0;

    int i;
    for (i = 0; i < num_materials; i++) {
        delete mlist[i];
        delete [] clist[i];
    }

  FCLOSE ( loader_fd ) ;

  return current_branch ;

}

// Never used (calls are commented out) : remove ?
/*
void myssgFlatten(ssgEntity *obj)
{

 if ( obj->isAKindOf ( ssgTypeBranch() ) )
  {
    ssgBranch *br = (ssgBranch *) obj ;
#ifdef WIN32
    if (!strnicmp(br->getKid(0)->getName(), "tkmn",4))
#else
    if (!strncasecmp(br->getKid(0)->getName(), "tkmn",4))
#endif
      {
    ssgFlatten(br->getKid(0));
      }
    else
      {
    for ( int i = 0 ; i < br->getNumKids () ; i++ )
      ssgFlatten( br->getKid ( i ) );
      }
  }
 return ;
 }
*/

// Loader public API ======================================================================

ssgEntity *grssgCarLoadAC3D ( const char *fname, const grssgLoaderOptions* options,int index )
{

  isacar=TRUE;
  isawheel=FALSE;
  usestrip=FALSE;
  indexCar=index;
  t_xmax=-999999.0;
  t_ymax=-999999.0;
  t_xmin=+999999.0;
  t_ymin=+999999.0;

  GfLogTrace("Loading car %s\n", fname);

  ssgEntity *obj = myssgLoadAC ( fname, options ) ;

  if ( obj == 0 )
    return 0 ;

  /* Do some simple optimisations */

  ssgBranch *model = new ssgBranch () ;
  model->addKid ( obj ) ;
  if(usestrip==FALSE)
  {
    /*myssgFlatten(obj);*/
    ssgFlatten    ( obj ) ;
    ssgStripify   ( model ) ;
  }

  carTrackRatioX = (t_xmax-t_xmin)/(shad_xmax-shad_xmin);
  carTrackRatioY = (t_ymax-t_ymin)/(shad_ymax-shad_ymin);

  return model ;
}

ssgEntity *grssgCarWheelLoadAC3D ( const char *fname, const grssgLoaderOptions* options, int carIndex )
{

  isacar=FALSE;
  isawheel=TRUE;
  usestrip=FALSE;
  indexCar=carIndex;
  t_xmax=-999999.0;
  t_ymax=-999999.0;
  t_xmin=+999999.0;
  t_ymin=+999999.0;

  GfLogTrace("Loading wheel %s\n", fname);

  ssgEntity *obj = myssgLoadAC ( fname, options ) ;

  if ( obj == 0 )
    return 0 ;

  /* Do some simple optimisations */

  ssgBranch *model = new ssgBranch () ;
  model->addKid ( obj ) ;
  if(!usestrip)
  {
    /*myssgFlatten(obj);*/
    ssgFlatten    ( obj ) ;
    ssgStripify   ( model ) ;
  }

  return model ;
}

ssgEntity *grssgLoadAC3D ( const char *fname, const grssgLoaderOptions* options )
{
  isacar=FALSE;
  isawheel=FALSE;
  usegroup=FALSE;
  usestrip=FALSE;

  t_xmax=-999999.0;
  t_ymax=-999999.0;
  t_xmin=+999999.0;
  t_ymin=+999999.0;

  GfOut("Loading %s\n", fname);

  ssgEntity *obj = myssgLoadAC ( fname, options ) ;

  if ( obj == 0 )
    return 0 ;

  /* Do some simple optimisations */
  ssgBranch *model = new ssgBranch () ;
  model->addKid ( obj ) ;
  if (!usegroup && !usestrip)
  {
    ssgFlatten    ( obj ) ;
    ssgStripify   ( model ) ;
  }

  return model ;
}

// grssgLoaderOptions implementation =========================================

grssgLoaderOptions::grssgLoaderOptions(bool bTextureMipMap)
: ssgLoaderOptions(), _bTextureMipMap(bTextureMipMap), _bTextureMapping(false)
{
}

void grssgLoaderOptions::makeModelPath(char* path, const char *fname) const
{
    ulFindFile(path, model_dir, fname, 0) ;
}

void grssgLoaderOptions::makeTexturePath(char* path, const char *fname) const
{
    ulFindFile(path, texture_dir, fname, 0) ;
}

ssgTexture* grssgLoaderOptions::createTexture(char* tfname, int wrapu, int wrapv, int mipmap)
{
    return ssgLoaderOptions::createTexture(tfname, wrapu, wrapv,
                                           _bTextureMipMap ? doMipMap(tfname, mipmap) : mipmap) ;
}

void grssgLoaderOptions::addTextureMapping(const char* pszSrcFileName, const char* pszTgtFileName)
{
    _mapTextures[pszSrcFileName] = pszTgtFileName;
    _bTextureMapping = true;
}

bool grssgLoaderOptions::textureMapping() const
{
    return _bTextureMapping;
}

const char* grssgLoaderOptions::mapTexture(const char* pszSrcFileName) const
{
    const std::map<std::string, std::string>::const_iterator iterTex =
        _mapTextures.find(pszSrcFileName);
    return iterTex != _mapTextures.end() ? iterTex->second.c_str() : pszSrcFileName;
}

void grssgSetCurrentOptions(grssgLoaderOptions* options)
{
    ssgSetCurrentOptions(options);
    current_options = static_cast<grssgLoaderOptions*>(ssgGetCurrentOptions());
}
