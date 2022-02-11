/*
 * SOLID - Software Library for Interference Detection
 * Copyright (c) 2001 Dtecta <gino@dtecta.com>
 *
 * All rights reserved.
 */

#ifndef SOLID_TYPES_H
#define SOLID_TYPES_H

#if defined(_WIN32)
#   if defined(SOLID_STATIC)
#      define DECLSPEC extern
#   elif defined(DLL_EXPORT)
#      define DECLSPEC __declspec(dllexport)
#   else 
#      define DECLSPEC __declspec(dllimport)
#   endif
#else
#   define DECLSPEC extern
#endif

#define DT_DECLARE_HANDLE(name) typedef struct name##__ { int unused; } *name
    
typedef unsigned int  DT_Index;
typedef unsigned int  DT_Count;
typedef unsigned int  DT_Size;
typedef float         DT_Scalar; 
typedef int           DT_Bool;

#define DT_FALSE 0
#define DT_TRUE  1


typedef DT_Scalar DT_Vector3[3]; 
typedef DT_Scalar DT_Quaternion[4]; 

#endif
