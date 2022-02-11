/*
 * SOLID - Software Library for Interference Detection
 * Copyright (c) 2001 Dtecta <gino@dtecta.com>
 *
 * All rights reserved.
 */

#ifndef SOLID_BROAD_H
#define SOLID_BROAD_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif
    
DT_DECLARE_HANDLE(BP_SceneHandle);
DT_DECLARE_HANDLE(BP_ProxyHandle);

typedef void (*BP_Callback)(void *client_data,
                            void *object1,
                            void *object2);

BP_SceneHandle BP_CreateScene(void *client_data,
								 			 BP_Callback beginOverlap,
											 BP_Callback endOverlap);
 
void           BP_DeleteScene(BP_SceneHandle scene);
	
BP_ProxyHandle BP_CreateProxy(BP_SceneHandle scene, void *object,
									 const DT_Vector3 min, const DT_Vector3 max);

void           BP_DeleteProxy(BP_SceneHandle scene, 
									 BP_ProxyHandle proxy);

void BP_SetBBox(BP_ProxyHandle proxy, const DT_Vector3 min, const DT_Vector3 max);

#ifdef __cplusplus
}
#endif

#endif
