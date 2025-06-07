#pragma once

#include "defines.h"

#define EVENT_TYPE_WINDOW_RESIZE "Window_Resize"
#define EVENT_TYPE_WINDOW_EXIT_REQUEST "Window_Exit_Request"

#define MAX_EVENT_COUNT 100
#define MAX_EVENT_SUBSCRIPTION_COUNT 100
#define MAX_EVENT_ARG_COUNT 8

typedef struct EventArg {
    char* key;
    union {
        i32 asI32;
        u32 asU32;
        f32 asF32;
        char asChar;
    } value;
} EventArg;

typedef struct Event {
    char* type;
    EventArg args[MAX_EVENT_ARG_COUNT];
} Event;

typedef void (*EventCallback)(Event* p_event);

typedef struct EventSubscription {
    char* type;
    EventCallback callback;
} EventSubscription;

bool StartupEventSystem(void);

void ShutdownEventSystem(void);

void ProcessEvents(void);

void SubscribeToEvent(const char* type, EventCallback callback);

void UnsubscribeToEvent(const char* type, EventCallback callback);

void FireEvent(const char* type);

bool SetEventArgI32(const char* type, const u8 index, const char* key, const i32 value);

i32 GetEventArgI32(const char* type, const char* key);
