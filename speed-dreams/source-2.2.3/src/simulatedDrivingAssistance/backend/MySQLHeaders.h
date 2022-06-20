/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once

#ifdef WIN32
#include "mysql/jdbc.h"
#else
#include "mysql_connection.h"
#include "mysql_driver.h"
#include "mysql_error.h"
#include "cppconn/build_config.h"
#include "cppconn/config.h"
#include "cppconn/connection.h"
#include "cppconn/datatype.h"
#include "cppconn/driver.h"
#include "cppconn/exception.h"
#include "cppconn/metadata.h"
#include "cppconn/parameter_metadata.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/resultset.h"
#include "cppconn/resultset_metadata.h"
#include "cppconn/statement.h"
#include "cppconn/sqlstring.h"
#include "cppconn/warning.h"
#include "cppconn/version_info.h"
#include "cppconn/variant.h"
#endif