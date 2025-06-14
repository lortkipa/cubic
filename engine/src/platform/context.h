#pragma once

#include "defines.h"

b8 CreateGLContext(const u32 majorV, const u32 minorV);

void BindGLContext(void);

void SwapGLBuffers(void);
