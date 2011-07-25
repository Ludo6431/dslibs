#ifndef _LIBDSTK_H
#define _LIBDSTK_H

// general definitions
#include "dstk/common.h"

// object system
#include <dstk/obj.h>                       // basic (allocation)
#   include <dstk/obj_Object.h>             // with properties and signals
#       include <dstk/obj_Data.h>           // store data using properties
#           include <dstk/obj_String.h>
#include <dstk/properties.h>                // definition of standard properties
#include <dstk/signals.h>                   // definition of standard signals

// utf8
#include <dstk/utf8.h>

// data management
#include <dstk/adata.h> // abstract data
#include <dstk/slice.h> // slice
#include <dstk/dlist.h> // doubly-linked list
#include <dstk/slist.h> // singly-linked list

// ...

#endif

