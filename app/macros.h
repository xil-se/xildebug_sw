#pragma once

#ifndef MODULE_NAME
#error You must define MODULE_NAME before including this file
#endif

//#undef MODULE_NAME
//#define MODULE_NAME Trololo_lo

#define cat(x, y) x ## y
#define cat2(x, y) cat(x, y)
#define SELF cat2(MODULE_NAME, _self)
