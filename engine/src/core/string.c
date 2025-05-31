#include "core/string.h"
#include "core/assert.h"
#include <stdlib.h>
#include <string.h>

String createString(void)
{
    // create string structure
    String string;

    // zero out stuff
    string.data = null;
    string.length = 0;

    // return created string
    return string;
}

void asignString(String* p_string, const char* newData)
{
    // check params
    assert("String", p_string != null, "invalid pointer provided");
    assert("String", newData != null, "invalid pointer provided");

    // store new length
    p_string->length = strlen(newData);

    // allocate string data on heap
    if (p_string->data == null)
    {
        p_string->data = malloc(p_string->length);
    }
    else 
    {
        p_string->data = realloc(p_string->data, p_string->length);
    }

    // check for allocation errors
    assert("string", p_string->data != NULL, "allocation failed");

    // store new datra into string
    strcpy(p_string->data, newData);
}

void destroyString(String* p_string)
{
    // check params
    assert("String", p_string != null, "invalid pointer provided");

    // free string data
    free(p_string->data);
    
    // zero out stuff
    p_string->data = null;
    p_string->length = 0;
}
