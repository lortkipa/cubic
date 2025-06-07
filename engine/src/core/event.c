#include "core/event.h"
#include "core/stack_allocator.h"
#include "core/logger.h"
#include "core/assert.h"
#include <string.h>

static bool initialized = false;
static StackAllocator allocator;
static StackAllocatorMarker subscriptionsMarker;

static Event* events;
static u16 eventCount;
static EventSubscription* subscriptions;
static u16 subscriptionCount;

bool StartupEventSystem(void)
{
    // create stack allocator
    allocator = CreateStackAllocator
        (
         MAX_EVENT_COUNT * sizeof(Event) +
         MAX_EVENT_SUBSCRIPTION_COUNT * sizeof(EventSubscription) + 
         MAX_EVENT_COUNT * MAX_EVENT_ARG_COUNT * 50
        );

    // check for errors
    if (allocator.size == 0)
    {
        LogError("Event System", "Failed To Allocate Memory For Events Array");
        return false;
    }

    // request memory for events array
    events = RequestStackAllocatorMemory(&allocator, MAX_EVENT_COUNT * sizeof(Event));

    // request memory for subscriptions array
    subscriptions = RequestStackAllocatorMemory(&allocator, MAX_EVENT_SUBSCRIPTION_COUNT * sizeof(EventSubscription));

    // get market to subscriptions array
    subscriptionsMarker = GetStackAllocatorMarker(&allocator);

    // return success as default
    initialized = true;
    LogSuccess("Event System", "System Initialized");
    return true;
}

void ShutdownEventSystem(void)
{
    // check if system is initialized
    Assert("Event System", initialized, "System Is Not Initialized");

    // destroy allocator
    DestroyStackAllocator(&allocator);

    initialized = false;
    LogSuccess("Event System", "System Terminated");
}

void ProcessEvents(void)
{
    // check if system is initialized
    Assert("Event System", initialized, "System Is Not Initialized");

    // loop thro subscriptions
    for (fu16 i = 0; i < subscriptionCount; i++)
    {
        // loop thro events
        for (fu16 j = 0; j < eventCount; j++)
        {
            // try to find event the subscriptions is subscribed too
            if (strcmp(subscriptions[i].type, events[j].type) == 0)
            {
                // call callback function
                subscriptions[i].callback(&events[j]);

                // break the loop
                break;
            }
        }
    }

    // zero out events arrays
    eventCount = 0;

    // clear stack memory to subscriptions (events arg stuff)
    FreeStackAllocatorMemoryToMarker(&allocator, subscriptionsMarker);
}

void SubscribeToEvent(const char* type, EventCallback callback)
{
    // create subscription
    EventSubscription subscription = 
    {
        .type = (char*)type,
        .callback = callback
    };

    // add subscription in the array
    subscriptions[subscriptionCount] = subscription;

    // resize subscriptions array
    subscriptionCount++;
}

void UnsubscribeToEvent(const char* type, EventCallback callback)
{
    // loop thro events
    for (fu16 i = 0; i < subscriptionCount; i++)
    {
        // try to find provided subscription
        if (strcmp(subscriptions[i].type, type) == 0 && subscriptions[i].callback == callback)
        {
            // remove event
            for (fu16 j = subscriptionCount - 1; j > i; j--)
            {
                subscriptions[j] = subscriptions[j + 1];
            }

            // resize events array
            subscriptionCount--;
        }
    }
}

void FireEvent(const char* type)
{
    // create structure
    Event event;

    // request memory for event type
    event.type = RequestStackAllocatorMemory(&allocator, strlen(type) + 1);

    // copy event data into structure
    strcpy(event.type, type);

    // loop thro events
    for (fu16 i = 0; i < eventCount; i++)
    {
        // if event with same type is found, override it
        if (strcmp(events[i].type, event.type) == 0)
        {
            // add event in the array
            events[i] = event;

            // stop function
            return;
        }
    }

    // add event in the array
    events[eventCount] = event;

    // resize events array
    eventCount++;
}

bool SetEventArgI32(const char* type, const u8 index, const char* key, const i32 value)
{
    // loop thro events
    for (fu16 i = 0; i < eventCount; i++)
    {
        // try to find provided event type
        if (strcmp(events[i].type, type) == 0)
        {
            // set event argument key
            events[i].args[index].key = RequestStackAllocatorMemory(&allocator, strlen(key) + 1);
            strcpy(events[i].args[index].key, key);

            // set event argument value
            events[i].args[index].value.asI32 = value;
            
            // argument key and value is set, so retunr success
            LogInfo("Event System", "Event \"%s\" Argument Is Set To: { key: \"%s\", value: %d }",
                    type, key, value);
            return true;
        }
    }

    // return failure as default
    LogWarning("Event System", "Event \"%s\" Not Found To Set Argument: { key: \"%s\", value: %d }",
            type, key, value);
    return false;
}

i32 GetEventArgI32(const char* type, const char* key)
{
    // loop thro events
    for (fu16 i = 0; i < eventCount; i++)
    {
        // try to find provided event type
        if (strcmp(events[i].type, type) == 0)
        {
            // loop thro event args
            for (fu8 j = 0; j < MAX_EVENT_ARG_COUNT; j++)
            {
                // try to find provieded event arg
                if (strcmp(events[i].args[j].key, key) == 0)
                {
                    // event and its argr is found, so return its data
                    return events[i].args[j].value.asI32;
                }
            }
        }
    }

    // return zero as defualt
    return 0;
}
