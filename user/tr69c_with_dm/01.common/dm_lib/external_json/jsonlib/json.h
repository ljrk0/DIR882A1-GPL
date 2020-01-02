/**
 * JSON common lib
 */
#ifndef JSON_H_
#define JSON_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* JSON error */
#define JSON_TRUE                1
#define JSON_FALSE               0

#define JSON_SUCCESS             0
#define JSON_ERR_ARGUMENT        1
#define JSON_ERR_NOMEM           2
#define JSON_ERR_NOT_ARRAY       3
#define JSON_ERR_NOT_POD         4
#define JSON_ERR_RENDER          9
#define JSON_ERR_JSON_TYPE       10
#define JSON_ERR_PARSE_POD       21
#define JSON_ERR_PARSE_ARRAY     22
#define JSON_ERR_PARSE_NAME      23
#define JSON_ERR_PARSE_STR       24
#define JSON_ERR_PARSE_BOOL      25
#define JSON_ERR_PARSE_NULL      26
#define JSON_ERR_PARSE_KEYWORD   30

/* JSON Type */
#define JSON_TYPE_INVALID  0x00
#define JSON_TYPE_POD_ITEM 0x01

#define JSON_TYPE_BASIC    0x10
#define JSON_TYPE_NULL     0x11
#define JSON_TYPE_STRING   0x12
#define JSON_TYPE_NUMBER   0x13
#define JSON_TYPE_BOOLEAN  0x14

#define JSON_TYPE_REFER    0x20
#define JSON_TYPE_ARRAY    0x21
#define JSON_TYPE_POD      0x22

/**
 * callback & typedef
 */
typedef int JSON_BOOL;
typedef JSON_BOOL (*JSON_RENDER_CALLBACK)(const char* out, void* userdata);

/* JSON Object */
typedef struct _JSON_OBJECT {
	int type;				     /* Type */
	struct _JSON_OBJECT *next;   /* next to brother */
} JSON_OBJECT;

/* JSON Object */
typedef struct _JSON_ITERATOR {
	JSON_OBJECT *next;           /* next to brother */

	int          type;			 /* Type */
	const char*  name;           /* only used by plain object, if other, it is null */
	JSON_OBJECT* current;        /* current item */
} JSON_ITERATOR;

/**
 * Interface
 */
extern int JSON_getErrno();

extern JSON_OBJECT *JSON_CreatePlainObject(void);
extern JSON_OBJECT *JSON_CreateArrayObject(void);
extern JSON_OBJECT *JSON_CreateStringObject(const char* value);
extern JSON_OBJECT *JSON_CreateNumberObject(double value);
extern JSON_OBJECT *JSON_CreateBoolObject(JSON_BOOL value);
extern JSON_OBJECT *JSON_CreateNULLObject(void);
extern void JSON_DeleteObject(JSON_OBJECT *object);

extern int JSON_AddObjectToPlainObject(JSON_OBJECT *object, const char* name, JSON_OBJECT *childObject);
extern int JSON_AddStringToPlainObject(JSON_OBJECT *object, const char* name, const char* strVal);
extern int JSON_AddNumberToPlainObject(JSON_OBJECT *object, const char* name, double number);
extern int JSON_AddBoolToPlainObject(JSON_OBJECT *object, const char* name, JSON_BOOL boolVal);
extern int JSON_AddNULLToPlainObject(JSON_OBJECT *object, const char* name);

extern int JSON_AddObjectToArray(JSON_OBJECT *object, JSON_OBJECT *childObject);
extern int JSON_AddStringToArray(JSON_OBJECT *object, const char* strVal);
extern int JSON_AddNumberToArray(JSON_OBJECT *object, double number);
extern int JSON_AddBoolToArray(JSON_OBJECT *object, JSON_BOOL boolVal);
extern int JSON_AddNULLToArray(JSON_OBJECT *object);

extern int JSON_InitIterator(JSON_OBJECT *object, JSON_ITERATOR* iterator);
extern JSON_BOOL JSON_IterNextChild(JSON_ITERATOR* iterator);
extern const char* JSON_GetValueOfStringObject(JSON_OBJECT *object);
extern double JSON_GetValueOfNumberObject(JSON_OBJECT *object);
extern JSON_BOOL JSON_GetValueOfBoolObject(JSON_OBJECT *object);

extern int JSON_Render(JSON_OBJECT *object, JSON_BOOL fmt, JSON_RENDER_CALLBACK callback, void* userData);

extern JSON_OBJECT * JSON_Parse(const char* text);
extern const char *JSON_GetParseError(void);

#ifdef __cplusplus
}
#endif

#endif
