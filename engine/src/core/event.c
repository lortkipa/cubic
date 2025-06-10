#include "core/event.h"
#include "core/assert.h"
#include "core/logger.h"
#include "core/stack_allocator.h"
#include <inttypes.h>
#include <string.h>

#define CHANNEL "Event System"

static StackAllocator allocator;
static Event* events;
static u16 eventCount = 0;
static EventSub* eventSubs;
static u16 eventSubCount = 0;

b8 StartupEventSystem(void)
{
    // create stack allocator and check for errors
    if (
            !CreateStackAllocator
            (
             &allocator,
             MAX_EVENT_COUNT * sizeof(Event) +                            // events array
             MAX_EVENT_SUB_COUNT * sizeof(EventSub) +                           // eventsubs array
             MAX_EVENT_TYPE_LENGTH * (MAX_EVENT_COUNT + MAX_EVENT_SUB_COUNT) +  // event and eventsub types
             MAX_EVENT_ARG_KEY_LENGTH * MAX_EVENT_COUNT * MAX_EVENT_ARG_COUNT   // event arg keys
            )
       )
    {
        LogError(CHANNEL, "Stack Allocator Not Created");
        return false;
    }

    // request heap memory for events array 
    events = RequestStackAllocatorMemory(&allocator, MAX_EVENT_COUNT * sizeof(Event));

    // request heap memory for event subs array 
    eventSubs = RequestStackAllocatorMemory(&allocator, MAX_EVENT_SUB_COUNT * sizeof(EventSub));

    // loop thro events
    for (fu16 i = 0; i < MAX_EVENT_COUNT; i++)
    {
        // request memory for event types
        events[i].type = RequestStackAllocatorMemory(&allocator, MAX_EVENT_TYPE_LENGTH);

        // loop thro event args
        for (fu8 j = 0; j < MAX_EVENT_ARG_COUNT; j++)
        {
            // request memory for event args
            events[i].args[j].key = RequestStackAllocatorMemory(&allocator, MAX_EVENT_ARG_KEY_LENGTH);
        }
    }

    // loop thro event subs
    for (fu16 i = 0; i < MAX_EVENT_SUB_COUNT; i++)
    {
        // request memory for event sub types
        eventSubs[i].type = RequestStackAllocatorMemory(&allocator, MAX_EVENT_TYPE_LENGTH);
    }

    // zero out array lengths
    eventCount = 0;
    eventSubCount = 0;

    // return success
    LogSuccess(CHANNEL, SYSTEM_INITIALIZED_MESSAGE);
    return true;
}

void ShutdownEventSystem(void)
{
    // zero out array lengths
    eventCount = 0;
    eventSubCount = 0;

    // destroy stack allocator
    DestroyStackAllocator(&allocator);

    // destroy stack allocator
    LogSuccess(CHANNEL, SYSTEM_TERMINATED_MESSAGE);
}

void ProcessEvents(void)
{
    // loop thro event subs
    for (fu16 i = 0; i < eventSubCount; i++)
    {
        // loop thro events
        for (fu16 j = 0; j < eventCount; j++)
        {
            // if event sub and event type much, call its callback function
            if (strcmp(eventSubs[i].type, events[j].type) == 0)
            {
                eventSubs[i].callback(&events[j]);
                break;
            }
        }
    }

    // clear events array
    eventCount = 0;
}

void SubToEvent(const char* type, EventCallback callback)
{
    // check params for invalid pointers
    Assert(CHANNEL, type != null, "Invalid Pointer Provided");
    Assert(CHANNEL, callback != null, "Invalid Pointer Provided");

    // make sure type is not longer than normal
    Assert(CHANNEL, strlen(type) < MAX_EVENT_TYPE_LENGTH, "Too Large String Provieded");

    // store type
    strcpy(eventSubs[eventSubCount].type, type);

    // store callback
    eventSubs[eventSubCount].callback = callback;

    // resize array
    eventSubCount++;
}

void UnsubToEvent(const char* type, EventCallback callback)
{
    // check params for invalid pointers
    Assert(CHANNEL, type != null, "Invalid Pointer Provided");
    Assert(CHANNEL, callback != null, "Invalid Pointer Provided");

    // make sure type is not longer than normal
    Assert(CHANNEL, strlen(type) < MAX_EVENT_TYPE_LENGTH, "Too Large String Provieded");

    // loop thro event subs
    for (fu16 i = 0; i < eventSubCount; i++)
    {
        // check if types muches
        if (strcmp(eventSubs[i].type, type) == 0)
        {
            // remove sub from array
            for (fu16 j = i; j < eventSubCount - 1; j++)
            {
                eventSubs[j] = eventSubs[j + 1];
            }

            // resize array
            eventSubCount--;

            // exit function
            return;
        }
    }
}

void FireEvent(const char* type)
{
    // check params for invalid pointesr
    Assert(CHANNEL, type != null, "Invalid Pointer Provided");

    // make sure type is not longer than normal
    Assert(CHANNEL, strlen(type) < MAX_EVENT_TYPE_LENGTH, "Too Large String Provieded");

    // store type
    strcpy(events[eventCount].type, type);

    // resize array
    eventCount++;
}

void SetEventArgI32(const char* type, const u32 index, const char* key, const i32 value)
{
    // check params for invalid pointers
    Assert(CHANNEL, type != null, "Invalid String Provided");
    Assert(CHANNEL, key != null, "Invalid String Provided");

    // make sure correct index is provieded
    Assert(CHANNEL, index < MAX_EVENT_ARG_COUNT, "Invalid Event Arg Index Provided");

    // loop thro events
    for (fu16 i = 0; i < eventCount; i++)
    {
        // check if types much
        if (strcmp(events[i].type, type) == 0)
        {
            // set key
            strcpy(events[i].args[index].key, key);

            // set value
            events[i].args[index].value.asI32 = value;

            // exit function
            return;
        }
    }
}

void SetEventArg(const char* type, const u32 index, const char* key, const EventArgValue value)
{
    // check params for invalid pointers
    Assert(CHANNEL, type != null, "Invalid String Provided");
    Assert(CHANNEL, key != null, "Invalid String Provided");

    // make sure correct index is provieded
    Assert(CHANNEL, index < MAX_EVENT_ARG_COUNT, "Invalid Event Arg Index Provided");

    // loop thro events
    for (fu16 i = 0; i < eventCount; i++)
    {
        // check if types much
        if (strcmp(events[i].type, type) == 0)
        {
            // set key
            strcpy(events[i].args[index].key, key);

            // set value
            events[i].args[index].value = value;

            // exit function
            return;
        }
    }

    // if code comes here, event was not found
    LogWarning(CHANNEL, "Event \"%s\" Not Found To Set Value", type);
}

EventArgValue GetEventArg(const Event* p_event, const char* key)
{
    // check params for invalid pointers
    Assert(CHANNEL, p_event != null, "Invalid Pointer Provided");
    Assert(CHANNEL, key != null, "Invalid String Provided");

    // loop thro args
    for (fu8 i = 0; i < MAX_EVENT_ARG_COUNT; i++)
    {
        // if keys much, return value
        if (strcmp(p_event->args[i].key, key) == 0)
        {
            return p_event->args[i].value;
        }
    }

    // return 0 as default
    LogWarning(CHANNEL, "Either Event Type Or Argument Is Not Found");
    return (EventArgValue){0};
}
