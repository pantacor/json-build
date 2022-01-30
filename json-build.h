#ifndef JSON_BUILD_H
#define JSON_BUILD_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef JSONB_STATIC
#define JSONB_API static
#else
#define JSONB_API extern
#endif

/* if necessary should be increased to avoid segfault */
#ifndef JSONB_MAX_DEPTH
#define JSONB_MAX_DEPTH 512
#endif

/** @brief json-builder return codes */
typedef enum jsonbcode {
    /** no error, operation was a success */
    JSONB_OK = 0,
    /** string is complete, expects no more inputs */
    JSONB_END,
    /** not enough tokens were provided */
    JSONB_ERROR_NOMEM = -1,
    /** token doesn't match expected value */
    JSONB_ERROR_INPUT = -2,
    /** operation would lead to out of boundaries access */
    JSONB_ERROR_STACK = -3
} jsonbcode;

/** @brief json-builder serializing state */
enum jsonbstate {
    JSONB_INIT = 0,
    JSONB_ARRAY_OR_OBJECT_OR_VALUE = JSONB_INIT,
    JSONB_OBJECT_KEY_OR_CLOSE,
    JSONB_OBJECT_VALUE,
    JSONB_OBJECT_NEXT_KEY_OR_CLOSE,
    JSONB_ARRAY_VALUE_OR_CLOSE,
    JSONB_ARRAY_NEXT_VALUE_OR_CLOSE,
    JSONB_ERROR,
    JSONB_DONE
};

/** @brief Handle for building a JSON string */
typedef struct jsonb {
    /** expected next input */
    enum jsonbstate stack[JSONB_MAX_DEPTH + 1];
    /** pointer to stack top */
    enum jsonbstate *top;
    /** offset in the JSON buffer (current length) */
    size_t pos;
} jsonb;

/**
 * @brief Initialize a jsonb handle
 *
 * @param builder the handle to be initialized
 */
JSONB_API void jsonb_init(jsonb *builder);

/**
 * @brief Push an object to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return @ref jsonbcode value
 */
JSONB_API jsonbcode jsonb_push_object(jsonb *builder,
                                      char buf[],
                                      size_t bufsize);

/**
 * @brief Pop an object from the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return @ref jsonbcode value
 */
JSONB_API jsonbcode jsonb_pop_object(jsonb *builder,
                                     char buf[],
                                     size_t bufsize);

/**
 * @brief Push a key to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @param key the key to be inserted
 * @param len the key length
 * @return @ref jsonbcode value
 */
JSONB_API jsonbcode jsonb_push_key(
    jsonb *builder, char buf[], size_t bufsize, const char key[], size_t len);

/**
 * @brief Push an array to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return @ref jsonbcode value
 */
JSONB_API jsonbcode jsonb_push_array(jsonb *builder,
                                     char buf[],
                                     size_t bufsize);

/**
 * @brief Pop an array from the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return @ref jsonbcode value
 */
JSONB_API jsonbcode jsonb_pop_array(jsonb *builder,
                                    char buf[],
                                    size_t bufsize);

/**
 * @brief Push a raw JSON token to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @param token the token to be inserted
 * @param len the token length
 * @return @ref jsonbcode value
 */
JSONB_API jsonbcode jsonb_push_token(jsonb *builder,
                                     char buf[],
                                     size_t bufsize,
                                     const char token[],
                                     size_t len);

/**
 * @brief Push a boolean token to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @param boolean the boolean to be inserted
 * @return @ref jsonbcode value
 */
JSONB_API jsonbcode jsonb_push_bool(jsonb *builder,
                                    char buf[],
                                    size_t bufsize,
                                    int boolean);

/**
 * @brief Push a null token to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return @ref jsonbcode value
 */
JSONB_API jsonbcode jsonb_push_null(jsonb *builder,
                                    char buf[],
                                    size_t bufsize);

/**
 * @brief Push a string token to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @param str the string to be inserted
 * @param len the string length
 * @return @ref jsonbcode value
 */
JSONB_API jsonbcode jsonb_push_string(
    jsonb *builder, char buf[], size_t bufsize, const char str[], size_t len);

/**
 * @brief Push a number token to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @param number the number to be inserted
 * @return @ref jsonbcode value
 */
JSONB_API jsonbcode jsonb_push_number(jsonb *builder,
                                      char buf[],
                                      size_t bufsize,
                                      double number);

#ifndef JSONB_HEADER
#include <stdio.h>
#ifndef JSONB_DEBUG
#define TRACE(prev, next) next
#define DECORATOR(a)
#else
static const char *
_jsonb_eval_state(enum jsonbstate state)
{
    switch (state) {
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
        return "array or object or value";
    case JSONB_OBJECT_KEY_OR_CLOSE:
        return "object key or close";
    case JSONB_OBJECT_NEXT_KEY_OR_CLOSE:
        return "object next key or close";
    case JSONB_OBJECT_VALUE:
        return "object value";
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        return "array value or close";
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        return "array next value or close";
    case JSONB_ERROR:
        return "error";
    case JSONB_DONE:
        return "done";
    default:
        return "unknown";
    }
}
#define TRACE(prev, next)                                                     \
    do {                                                                      \
        enum jsonbstate _prev = prev, _next = next;                           \
        fprintf(stderr, "%s():L%d | %s -> %s\n", __func__, __LINE__,          \
                _jsonb_eval_state(_prev), _jsonb_eval_state(_next));          \
    } while (0)
#define DECORATOR(d) d
#endif /* JSONB_DEBUG */

#define STACK_PUSH(b, state) TRACE(*(b)->top, *++(b)->top = (state))
#define STACK_POP(b)         TRACE(*(b)->top, DECORATOR(*)--(b)->top)
#define STACK_HEAD(b, state) TRACE(*(b)->top, *(b)->top = (state))

#define BUFFER_COPY_CHAR(b, c, _pos, buf, bufsize)                            \
    do {                                                                      \
        if ((b)->pos + (_pos) + 1 > (bufsize)) {                              \
            return JSONB_ERROR_NOMEM;                                         \
        }                                                                     \
        (buf)[(b)->pos + (_pos)++] = (c);                                     \
    } while (0)
#define BUFFER_COPY(b, value, len, _pos, buf, bufsize)                        \
    do {                                                                      \
        size_t i;                                                             \
        if ((b)->pos + (_pos) + (len) > (bufsize)) {                          \
            return JSONB_ERROR_NOMEM;                                         \
        }                                                                     \
        for (i = 0; i < (len); ++i)                                           \
            (buf)[(b)->pos + (_pos) + i] = (value)[i];                        \
        (_pos) += (len);                                                      \
    } while (0)

void
jsonb_init(jsonb *b)
{
    static jsonb empty_builder;
    *b = empty_builder;
    b->top = b->stack;
}

jsonbcode
jsonb_push_object(jsonb *b, char buf[], size_t bufsize)
{
    enum jsonbstate next_state;
    enum jsonbcode code;
    size_t pos = 0;
    if (b->top - b->stack >= JSONB_MAX_DEPTH)
        return JSONB_ERROR_STACK;
    switch (*b->top) {
    case JSONB_DONE:
        return JSONB_ERROR_INPUT;
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        BUFFER_COPY_CHAR(b, ',', pos, buf, bufsize);
        /* fall-through */
    case JSONB_OBJECT_VALUE:
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        if (*b->top <= JSONB_OBJECT_NEXT_KEY_OR_CLOSE)
            next_state = JSONB_OBJECT_NEXT_KEY_OR_CLOSE;
        else if (*b->top <= JSONB_ARRAY_NEXT_VALUE_OR_CLOSE)
            next_state = JSONB_ARRAY_NEXT_VALUE_OR_CLOSE;
        code = JSONB_OK;
        break;
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
        next_state = JSONB_DONE;
        code = JSONB_END;
        break;
    default:
        STACK_HEAD(b, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    BUFFER_COPY_CHAR(b, '{', pos, buf, bufsize);
    STACK_HEAD(b, next_state);
    STACK_PUSH(b, JSONB_OBJECT_KEY_OR_CLOSE);
    b->pos += pos;
    return code;
}

jsonbcode
jsonb_pop_object(jsonb *b, char buf[], size_t bufsize)
{
    enum jsonbcode code;
    size_t pos = 0;
    switch (*b->top) {
    case JSONB_DONE:
        code = JSONB_END;
        break;
    case JSONB_OBJECT_KEY_OR_CLOSE:
    case JSONB_OBJECT_NEXT_KEY_OR_CLOSE:
        code = JSONB_OK;
        break;
    default:
        STACK_HEAD(b, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    BUFFER_COPY_CHAR(b, '}', pos, buf, bufsize);
    STACK_POP(b);
    b->pos += pos;
    return code;
}

jsonbcode
jsonb_push_key(
    jsonb *b, char buf[], size_t bufsize, const char key[], size_t len)
{
    size_t pos = 0;
    switch (*b->top) {
    case JSONB_DONE:
        return JSONB_ERROR_INPUT;
    case JSONB_OBJECT_NEXT_KEY_OR_CLOSE:
        BUFFER_COPY_CHAR(b, ',', pos, buf, bufsize);
    /* fall-through */
    case JSONB_OBJECT_KEY_OR_CLOSE:
        BUFFER_COPY_CHAR(b, '"', pos, buf, bufsize);
        BUFFER_COPY(b, key, len, pos, buf, bufsize);
        BUFFER_COPY(b, "\":", 2, pos, buf, bufsize);
        STACK_HEAD(b, JSONB_OBJECT_VALUE);
        break;
    default:
        STACK_HEAD(b, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    b->pos += pos;
    return JSONB_OK;
}

jsonbcode
jsonb_push_array(jsonb *b, char buf[], size_t bufsize)
{
    enum jsonbstate next_state;
    enum jsonbcode code;
    size_t pos = 0;
    if (b->top - b->stack >= JSONB_MAX_DEPTH)
        return JSONB_ERROR_STACK;
    switch (*b->top) {
    case JSONB_DONE:
        return JSONB_ERROR_INPUT;
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        BUFFER_COPY_CHAR(b, ',', pos, buf, bufsize);
        /* fall-through */
    case JSONB_OBJECT_VALUE:
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        if (*b->top <= JSONB_OBJECT_NEXT_KEY_OR_CLOSE)
            next_state = JSONB_OBJECT_NEXT_KEY_OR_CLOSE;
        else if (*b->top <= JSONB_ARRAY_NEXT_VALUE_OR_CLOSE)
            next_state = JSONB_ARRAY_NEXT_VALUE_OR_CLOSE;
        code = JSONB_OK;
        break;
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
        next_state = JSONB_DONE;
        code = JSONB_END;
        break;
    default:
        STACK_HEAD(b, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    BUFFER_COPY_CHAR(b, '[', pos, buf, bufsize);
    STACK_HEAD(b, next_state);
    STACK_PUSH(b, JSONB_ARRAY_VALUE_OR_CLOSE);
    b->pos += pos;
    return code;
}

jsonbcode
jsonb_pop_array(jsonb *b, char buf[], size_t bufsize)
{
    enum jsonbcode code;
    size_t pos = 0;
    switch (*b->top) {
    case JSONB_DONE:
    case JSONB_ERROR:
        code = JSONB_END;
        break;
    case JSONB_ARRAY_VALUE_OR_CLOSE:
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        code = JSONB_OK;
        break;
    default:
        STACK_HEAD(b, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    BUFFER_COPY_CHAR(b, ']', pos, buf, bufsize);
    STACK_POP(b);
    b->pos += pos;
    return code;
}

jsonbcode
jsonb_push_token(
    jsonb *b, char buf[], size_t bufsize, const char token[], size_t len)
{
    enum jsonbstate next_state;
    enum jsonbcode code;
    size_t pos = 0;
    switch (*b->top) {
    case JSONB_DONE:
    case JSONB_ERROR:
        return JSONB_ERROR_INPUT;
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
        next_state = JSONB_DONE;
        code = JSONB_END;
        break;
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        BUFFER_COPY_CHAR(b, ',', pos, buf, bufsize);
        /* fall-through */
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        next_state = JSONB_ARRAY_NEXT_VALUE_OR_CLOSE;
        code = JSONB_OK;
        break;
    case JSONB_OBJECT_VALUE:
        next_state = JSONB_OBJECT_NEXT_KEY_OR_CLOSE;
        code = JSONB_OK;
        break;
    default:
        STACK_HEAD(b, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    BUFFER_COPY(b, token, len, pos, buf, bufsize);
    STACK_HEAD(b, next_state);
    b->pos += pos;
    return code;
}

jsonbcode
jsonb_push_bool(jsonb *b, char buf[], size_t bufsize, int boolean)
{
    if (boolean) return jsonb_push_token(b, buf, bufsize, "true", 4);
    return jsonb_push_token(b, buf, bufsize, "false", 5);
}

jsonbcode
jsonb_push_null(jsonb *b, char buf[], size_t bufsize)
{
    return jsonb_push_token(b, buf, bufsize, "null", 4);
}

static long
_jsonb_escape(
    size_t *pos, char buf[], size_t bufsize, const char str[], size_t len)
{
    char *esc_tok = NULL, _esc_tok[8] = "\\u00";
    char *esc_buf = NULL;
    int extra_bytes = 0;
    size_t i;
second_iter:
    /* 1st iteration, esc_buf is NULL and count extra_bytes needed for escaping
     * 2st iteration, esc_buf is not NULL, and does escaping.  */
    for (i = 0; i < len; ++i) {
        unsigned char c = str[i];
        esc_tok = NULL;
        switch (c) {
        case 0x22: esc_tok = "\\\""; break;
        case 0x5C: esc_tok = "\\\\"; break;
        case '\b': esc_tok = "\\b"; break;
        case '\f': esc_tok = "\\f"; break;
        case '\n': esc_tok = "\\n"; break;
        case '\r': esc_tok = "\\r"; break;
        case '\t': esc_tok = "\\t"; break;
        default: if (c <= 0x1F) {
                   static const char tohex[] = "0123456789abcdef";
                   _esc_tok[4] = tohex[c >> 4];
                   _esc_tok[5] = tohex[c & 0xF];
                   _esc_tok[6] = 0;
                   esc_tok = _esc_tok;
                 }
        }
        if (esc_tok) {
            int j;
            for (j = 0; esc_tok[j]; j++) {
                if (!esc_buf) /* count how many extra bytes are needed */
                    continue;
                *esc_buf++ = esc_tok[j];
            }
            extra_bytes += j - 1;
        }
        else if (esc_buf) {
            *esc_buf++ = c;
        }
    }

    if (*pos + len + extra_bytes > bufsize) return JSONB_ERROR_NOMEM;

    if (esc_buf) {
        *pos += len + extra_bytes;
        return JSONB_OK;
    }
    if (!extra_bytes) {
        size_t j;
        for (j = 0; j < len; ++j)
            buf[*pos + j] = str[j];
        *pos += len;
        return JSONB_OK;
    }
    esc_buf = buf + *pos;
    extra_bytes = 0;
    goto second_iter;
}

jsonbcode
jsonb_push_string(
    jsonb *b, char buf[], size_t bufsize, const char str[], size_t len)
{
    enum jsonbstate next_state;
    enum jsonbcode code, ret;
    size_t pos = 0;
    switch (*b->top) {
    case JSONB_DONE:
    case JSONB_ERROR:
        return JSONB_ERROR_INPUT;
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
        next_state = JSONB_DONE;
        code = JSONB_END;
        break;
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        BUFFER_COPY_CHAR(b, ',', pos, buf, bufsize);
        /* fall-through */
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        next_state = JSONB_ARRAY_NEXT_VALUE_OR_CLOSE;
        code = JSONB_OK;
        break;
    case JSONB_OBJECT_VALUE:
        next_state = JSONB_OBJECT_NEXT_KEY_OR_CLOSE;
        code = JSONB_OK;
        break;
    default:
        STACK_HEAD(b, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    BUFFER_COPY_CHAR(b, '"', pos, buf, bufsize);
    ret = _jsonb_escape(&pos, buf + b->pos, bufsize, str, len);
    if (ret != JSONB_OK) return ret;
    BUFFER_COPY_CHAR(b, '"', pos, buf, bufsize);
    STACK_HEAD(b, next_state);
    b->pos += pos;
    return code;
}

jsonbcode
jsonb_push_number(jsonb *b, char buf[], size_t bufsize, double number)
{
    char token[32];
    long len = sprintf(token, "%.17G", number);
    if (len < 0) return JSONB_ERROR_INPUT;
    return jsonb_push_token(b, buf, bufsize, token, len);
}
#endif /* JSONB_HEADER */

#ifdef __cplusplus
}
#endif

#endif /* JSON_BUILD_H */
