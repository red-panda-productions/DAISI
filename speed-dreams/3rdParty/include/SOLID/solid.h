/*
  SOLID - Software Library for Interference Detection
  Copyright (C) 1997-1998  Gino van den Bergen

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
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  Please send remarks, questions and bug reports to gino@win.tue.nl,
  or write to:
                  Gino van den Bergen
		  Department of Mathematics and Computing Science
		  Eindhoven University of Technology
		  P.O. Box 513, 5600 MB Eindhoven, The Netherlands
*/

#ifndef SOLID_SOLID_H
#define SOLID_SOLID_H

#ifdef __cplusplus
extern "C" {
#endif

#include "SOLID/types.h"

typedef void *DtObjectRef;
DT_DECLARE_HANDLE(DtShapeRef);

typedef enum DtPolyType {
  DT_SIMPLEX,
  DT_POLYGON,
  DT_POLYHEDRON
} DtPolyType;

typedef enum DtResponseType { 
  DT_NO_RESPONSE,
  DT_SIMPLE_RESPONSE,
  DT_SMART_RESPONSE,
  DT_WITNESSED_RESPONSE
} DtResponseType;

typedef struct DtCollData {
  DT_Vector3 point1;
  DT_Vector3 point2;
  DT_Vector3 normal;
} DtCollData;

typedef void (*DtResponse)(
  void *client_data,
  DtObjectRef object1,
  DtObjectRef object2,
  const DtCollData *coll_data);



/* Shape definition */

 DECLSPEC DtShapeRef dtBox(DT_Scalar x, DT_Scalar y, DT_Scalar z);
 DECLSPEC DtShapeRef dtCone(DT_Scalar radius, DT_Scalar height);
 DECLSPEC DtShapeRef dtCylinder(DT_Scalar radius, DT_Scalar height);
 DECLSPEC DtShapeRef dtSphere(DT_Scalar radius);

 DECLSPEC DtShapeRef dtNewComplexShape();
 DECLSPEC void dtEndComplexShape();

 DECLSPEC void dtBegin(DtPolyType type);
 DECLSPEC void dtEnd();

 DECLSPEC void dtVertex(DT_Scalar x, DT_Scalar y, DT_Scalar z);
 DECLSPEC void dtVertexBase(const void *base);
 DECLSPEC void dtVertexIndex(DT_Index index);
 DECLSPEC void dtVertexIndices(DtPolyType type, DT_Count count, 
			    const DT_Index *indices);
 DECLSPEC void dtVertexRange(DtPolyType type, DT_Index first, DT_Count count); 

 DECLSPEC void dtChangeVertexBase(DtShapeRef shape, const void *base);

 DECLSPEC void dtDeleteShape(DtShapeRef shape);


/* Object  */

 DECLSPEC void dtCreateObject(DtObjectRef object, DtShapeRef shape); 
 DECLSPEC void dtDeleteObject(DtObjectRef object);
 DECLSPEC void dtSelectObject(DtObjectRef object);

 DECLSPEC void dtLoadIdentity();

 DECLSPEC void dtLoadMatrixf(const float *m);
 DECLSPEC void dtLoadMatrixd(const double *m);

 DECLSPEC void dtMultMatrixf(const float *m);
 DECLSPEC void dtMultMatrixd(const double *m);

 DECLSPEC void dtTranslate(DT_Scalar x, DT_Scalar y, DT_Scalar z);
 DECLSPEC void dtRotate(DT_Scalar x, DT_Scalar y, DT_Scalar z, DT_Scalar w);
 DECLSPEC void dtScale(DT_Scalar x, DT_Scalar y, DT_Scalar z);


/* Response */

 DECLSPEC void dtSetDefaultResponse(DtResponse response, DtResponseType type, 
				 void *client_data);

 DECLSPEC void dtClearDefaultResponse();

 DECLSPEC void dtSetObjectResponse(DtObjectRef object, DtResponse response, 
				DtResponseType type, void *client_data);
 DECLSPEC void dtClearObjectResponse(DtObjectRef object);
 DECLSPEC void dtResetObjectResponse(DtObjectRef object);

 DECLSPEC void dtSetPairResponse(DtObjectRef object1, DtObjectRef object2, 
			      DtResponse response, DtResponseType type, 
			      void *client_data);
 DECLSPEC void dtClearPairResponse(DtObjectRef object1, DtObjectRef object2);
 DECLSPEC void dtResetPairResponse(DtObjectRef object1, DtObjectRef object2);


/* Global */

 DECLSPEC DT_Count dtTest();
 DECLSPEC void dtTestObjects(DtObjectRef object1, DtObjectRef object2);
 DECLSPEC void dtProceed();

 DECLSPEC void dtEnableCaching();
 DECLSPEC void dtDisableCaching();

 DECLSPEC void dtSetTolerance(DT_Scalar tol);

#ifdef __cplusplus
}
#endif

#endif
