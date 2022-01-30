JSON-BUILD
==========

json-build is a zero-allocation JSON serializer compatible with C89. It is
inspired by [jsmn](https://github.com/zserge/jsmn), a minimalistic JSON tokenizer.

Features
--------

* compatible with C89
* no dependencies
* no dynamic memory allocation

Usage
-----

Download `json-build.h`, include it, done.

```c
#include "json-build.h"

...
jsonb b;
char buf[1024] = { 0 };

jsonb_init(&b);
jsonb_push_object(&b, buf, sizeof(buf));
{
    jsonb_push_key(&b, buf, sizeof(buf), "foo", strlen("foo"));
    jsonb_push_array(&b, buf, sizeof(buf));
    {
        jsonb_push_number(&b, buf, sizeof(buf), 1);
        jsonb_push_string(&b, buf, sizeof(buf), "hi", 2);
        jsonb_push_bool(&b, buf, sizeof(buf), 0);
        jsonb_push_null(&b, buf, sizeof(buf));
        jsonb_pop_array(&b, buf, sizeof(buf));
    }
    jsonb_pop_object(&b, buf, sizeof(buf));
}
printf("JSON: %s", buf); // JSON: {"foo":[1,"hi",false,null]}
```

Since json-build is a single-header, header-only library, for more complex use
cases you might need to define additional macros. `#define JSONB_STATIC`hides all
json-build API symbols by making them static. Also, if you want to include `json-build.h`
for multiple C files, to avoid duplication of symbols you may define `JSONB_HEADER` macro.

```c
/* In every .c file that uses json-build include only declarations: */
#define JSONB_HEADER
#include "json-build.h"

/* Additionally, create one json-build.c file for json-build implementation: */
#include "json-build.h"
```

API
---

* `jsonb_init()` - initialize a jsonb handle
* `jsonb_push_object()` - push an object to the builder stack
* `jsonb_pop_object()` - pop an object from the builder stack
* `jsonb_push_key()` - push an object key field to the builder stack
* `jsonb_push_array()` - push an array to the builder stack
* `jsonb_pop_array()` - pop an array from the builder stack
* `jsonb_push_token()` - push a raw token to the builder stack
* `jsonb_push_bool()` - push a boolean token to the builder stack
* `jsonb_push_null()` - push a null token to the builder stack
* `jsonb_push_string()` - push a string token to the builder stack
* `jsonb_push_number()` - push a number token to the builder stack

The following are the possible return codes for the builder functions:
* `JSONB_OK` - operation was a success, user can proceed with next operation
* `JSONB_DONE` - operation was a success, JSON is complete and expects no more operations
* `JSONB_ERROR_NOMEM` - buffer is not large enough
* `JSONB_ERROR_INPUT` - user action don't match expected next token
* `JSONB_ERROR_STACK` - user action would lead to out of boundaries access, increase `JSONB_MAX_DEPTH`!

Its worth mentioning that all `JSONB_ERROR_` prefixed codes are negative.

If you get `JSONB_ERROR_NOMEM` you can re-allocate a larger buffer and call
the builder function once more.

Other info
----------

This software is distributed under [MIT license](www.opensource.org/licenses/mit-license.php),
so feel free to integrate it in your commercial products.
