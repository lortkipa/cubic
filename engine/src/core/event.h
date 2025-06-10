#pragma once

#include "defines.h"

#define EVENT_TYPE_WINDOW_EXIT_REQUEST "Window_Exit_Request"
#define EVENT_TYPE_WINDOW_RESIZE "Window_Resize"
#define EVENT_TYPE_KEY_PRESS "Key_Press"
#define EVENT_TYPE_KEY_RELEASE "Key_Release"

#define MAX_EVENT_COUNT 100
#define MAX_EVENT_SUB_COUNT 100
#define MAX_EVENT_ARG_COUNT 5
#define MAX_EVENT_TYPE_LENGTH 20
#define MAX_EVENT_ARG_KEY_LENGTH 10

typedef union EventArgValue {
    i32 asI32;
    u32 asu32;
    f32 asF32;
    char asChar;
} EventArgValue;

typedef struct EventArg {
    char* key;
    EventArgValue value;
} EventArg;

typedef struct Event {
    char* type;
    EventArg args[MAX_EVENT_ARG_COUNT];
} Event;

typedef void (*EventCallback)(const Event* p_event);

typedef struct EventSub {
    char* type;
    EventCallback callback;
} EventSub;

EXPORT b8 StartupEventSystem(void);

EXPORT void ShutdownEventSystem(void);

EXPORT void ProcessEvents(void);

EXPORT u16 GetEventCount(void);

EXPORT u16 GetEventSubCount(void);

EXPORT void SubToEvent(const char* type, EventCallback callback);

EXPORT void UnsubToEvent(const char* type, EventCallback callback);

EXPORT void FireEvent(const char* type);

EXPORT void SetEventArgI32(const char* type, const u32 index, const char* key, const i32 value);

EXPORT void SetEventArgF32(const char* type, const u32 index, const char* key, const f32 value);

EXPORT void SetEventArg(const char* type, const u32 index, const char* key, const EventArgValue value);

EXPORT EventArgValue GetEventArg(const Event* p_event, const char* key);
