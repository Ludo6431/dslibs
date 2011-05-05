#ifndef _LIBDSTK_H
#define _LIBDSTK_H

// general definitions
#include "dstk/common.h"

// object system
#include <dstk/obj.h>                       // basic
#   include <dstk/obj_AObj.h>               // with properties and signals
#       include <dstk/obj_Data.h>           // store data using properties
#           include <dstk/obj_String.h>
#       include <dstk/obj_OList.h>          // may have children
#           include <dstk/obj_Object.h>
#include <dstk/properties.h>                // definition of standard properties
#include <dstk/signals.h>                   // definition of standard signals

// utf8
#include <dstk/utf8.h>

// abstract data
#include <dstk/adata.h>

// ...

#endif

