#pragma once

typedef unsigned int INTERVENTION_TYPE;

#define INTERVENTION_TYPE_NO_INTERVENTION       0
#define INTERVENTION_TYPE_INDICATION            1
#define INTERVENTION_TYPE_ASK_FOR               2
#define INTERVENTION_TYPE_PERFORM_WHEN_NEEDED   3
#define INTERVENTION_TYPE_ALWAYS_INTERVENE      4

typedef unsigned int BLACK_BOX_TYPE;

#define BLACK_BOX_TYPE_SOCKET   0

typedef unsigned int DATA_STORAGE_TYPE;

#define DATA_STORAGE_TYPE_SOCKET    0
#define DATA_STORAGE_TYPE_SQL       1

//#define RECORD_SESSION 1
#ifndef RECORD_SESSION
#define RECORD_SESSION 0
#endif