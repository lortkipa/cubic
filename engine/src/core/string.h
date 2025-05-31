#pragma once

#include "defines.h"

typedef struct String {
    char* data;
    u32 length;
} String;

String createString(void);

void asignString(String* p_string, const char* newData);

void destroyString(String* p_string);
