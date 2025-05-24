#pragma once

#include <stddef.h>
#include <stdint.h>

/*--------------------booleans--------------------*/
typedef int8_t bool;
#define true (1)
#define false (0)

/*--------------------floats--------------------*/
typedef float f32;
typedef double f64;

/*--------------------signed intigers--------------------*/
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef int_fast8_t fi8;
typedef int_fast16_t fi16;
typedef int_fast32_t fi32;
typedef int_fast64_t fi64;

/*--------------------usigned intigers--------------------*/
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint_fast8_t fu8;
typedef uint_fast16_t fu16;
typedef uint_fast32_t fu32;
typedef uint_fast64_t fu64;

/*--------------------platform--------------------*/
#if defined(__linux__)
    #define PLATFORM_LINUX
#elif defined(_WIN32)
    #define PLATFORM_WINDOWS
#else 
    #error Engine Running On Unsupported Platform
#endif
