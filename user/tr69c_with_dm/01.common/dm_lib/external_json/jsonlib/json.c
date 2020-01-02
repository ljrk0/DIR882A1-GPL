/**
 * JSON common lib
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>

#include "json.h"

/*--------------------------------------------------------------------------------------------------------------------------------
 * local object/function
 *--------------------------------------------------------------------------------------------------------------------------------*/
#define JSON_MAX_ITEM_NAME   10
#define JSON_RenderCrlf(ret, ctx) if(ret == JSON_SUCCESS && (ctx)->needformat && (ctx)->callback) (ctx)->callback("\n", (ctx)->userData)

/**
 * local object
 */
typedef struct _JSON_NUM_VAL {
	double dval;
	long long lval;
} JSON_NUM_VAL;

typedef struct _JSON_REFER_OBJECT {
	JSON_OBJECT object;
	JSON_OBJECT *child;
	JSON_OBJECT *lastChild;
} JSON_REFER_OBJECT;

typedef struct _JSON_POD_ITEM {
	JSON_OBJECT object;
	char* name;
	JSON_OBJECT *item;
} JSON_POD_ITEM;

typedef struct _JSON_BASIC_OBJECT {
	JSON_OBJECT object;
	union {
		char *strVal;
		int  boolVal;
		JSON_NUM_VAL numVal;
	};
} JSON_BASIC_OBJECT;

typedef struct _JSON_RENDER_CONTEXT {
	int   depth;

	JSON_BOOL needformat;
	int preFmtLen;
	JSON_RENDER_CALLBACK callback;
	void *userData;
} JSON_RENDER_CONTEXT;

typedef struct _JSON_CONTROL {
	int errno;
	void *(*alloc)(size_t sz);
	void (*free)(void *ptr);

	char parseError[100];
} JSON_CONTROL;

static JSON_CONTROL g_jsonCtrl = {  .alloc = malloc, .free = free };

/**
 * local fucntion
 */
static int JSON_newObject(int type, JSON_OBJECT** object);
static int JSON_AddObjectToReferObject(JSON_OBJECT *object, JSON_OBJECT *childItem);

static int JSON_RenderCore(JSON_OBJECT *object, JSON_RENDER_CONTEXT* context);
static int JSON_RenderPlainObject(JSON_REFER_OBJECT *podObject, JSON_RENDER_CONTEXT* context);
static int JSON_RenderArray(JSON_REFER_OBJECT *podObject, JSON_RENDER_CONTEXT* context);
static int JSON_RenderString(JSON_BASIC_OBJECT *basicObject, JSON_RENDER_CONTEXT* context);
static int JSON_RenderNumber(JSON_BASIC_OBJECT *basicObject, JSON_RENDER_CONTEXT* context);

static int JSON_RenderBuffer(JSON_RENDER_CONTEXT* context, JSON_BOOL needIdent, char *lineFmt, ...);

static int JSON_ParseCore(const char* text, JSON_OBJECT** object, const char** nextText);
static int JSON_ParsePlainObject(const char* text, JSON_OBJECT** object, const char** nextText);
static int JSON_ParsePlainItem(const char* text, JSON_OBJECT *podObject, const char** nextText);
static int JSON_ParseArrayObject(const char* text, JSON_OBJECT** object, const char** nextText);
static int JSON_ParseStringObject(const char* text, JSON_OBJECT** object, const char** nextText);
static int JSON_ParseNumberObject(const char* text, JSON_OBJECT** object, const char** nextText);
static int JSON_ParseBooleanObject(const char* text, JSON_OBJECT** object, const char** nextText);
static int JSON_ParseNULLObject(const char* text, JSON_OBJECT** object, const char** nextText);
static const char *JSON_getToken(const char *input);
static int JSON_getTokenLen(const char *input, const char tail);
static int JSON_getString(const char* text, const char tail, char** str, int* strLen);

/*--------------------------------------------------------------------------------------------------------------------------------
 * JSON utility
 *--------------------------------------------------------------------------------------------------------------------------------*/
/* JSON allocate memory */
static inline int JSON_alloc(size_t sz, void** ptr)
{
	int ret = JSON_ERR_NOMEM;
	*ptr = g_jsonCtrl.alloc(sz);
	if(*ptr) {
		memset(*ptr, 0, sz);
		ret = JSON_SUCCESS;
	}
	return ret;
}

/* JSON free memory */
static inline void JSON_free(void** ptr)
{
	if(ptr != NULL) {
		if(*ptr != NULL) g_jsonCtrl.free(*ptr);
		*ptr = NULL;
	}
}

/* JSON strdup */
static inline char* JOSN_strdup(const char* pValue)
{
	if(pValue != NULL) {
		char* pNewVal = strdup(pValue);
		g_jsonCtrl.errno = (pNewVal) ? JSON_SUCCESS : JSON_ERR_NOMEM;
		return pNewVal;
	}
	g_jsonCtrl.errno = JSON_ERR_ARGUMENT;
	return NULL;
}

/* JSON set string value */
static inline int JSON_setStringValue(JSON_BASIC_OBJECT* basicObject, const char* value)
{
	basicObject->strVal = value[0] != '\0' ? JOSN_strdup(value) : NULL;
	return g_jsonCtrl.errno;
}

/* JSON set number value */
static inline void JSON_setNumberValue(JSON_BASIC_OBJECT* basicObject, double value)
{
	basicObject->numVal.dval = value;
	basicObject->numVal.lval = (long long)value;
}

/* JSON set boolean value */
static inline void JSON_setBoolValue(JSON_BASIC_OBJECT* basicObject, JSON_BOOL value)
{
	basicObject->boolVal = value;
}

/*--------------------------------------------------------------------------------------------------------------------------------
 * JSON object model
 *--------------------------------------------------------------------------------------------------------------------------------*/
/**
 * get json error no
 */
int JSON_getErrno() { return g_jsonCtrl.errno; }

/**
 * create JSON plain object
 */
JSON_OBJECT *JSON_CreatePlainObject(void)
{
	JSON_OBJECT* object = NULL;

	int ret = JSON_newObject(JSON_TYPE_POD, &object);
	g_jsonCtrl.errno = ret;

	return object;
}

/**
 * create JSON array object
 */
JSON_OBJECT *JSON_CreateArrayObject(void)
{
	JSON_OBJECT* object = NULL;

	int ret = JSON_newObject(JSON_TYPE_ARRAY, &object);
	g_jsonCtrl.errno = ret;

	return object;
}

/**
 * create JSON string object
 */
JSON_OBJECT *JSON_CreateStringObject(const char* value)
{
	JSON_OBJECT* object = NULL;

	int ret = JSON_newObject(JSON_TYPE_STRING, &object);
	if(ret == JSON_SUCCESS) {
		ret = JSON_setStringValue((JSON_BASIC_OBJECT *)object, value);
	}
	g_jsonCtrl.errno = ret;

	return object;
}

/**
 * create JSON number object
 */
JSON_OBJECT *JSON_CreateNumberObject(double value)
{
	JSON_OBJECT* object = NULL;

	int ret = JSON_newObject(JSON_TYPE_NUMBER, &object);
	if(ret == JSON_SUCCESS) {
		JSON_setNumberValue((JSON_BASIC_OBJECT *)object, value);
	}
	g_jsonCtrl.errno = ret;

	return object;
}

/**
 * create JSON boolean object
 */
JSON_OBJECT *JSON_CreateBoolObject(JSON_BOOL value)
{
	JSON_OBJECT* object = NULL;

	int ret = JSON_newObject(JSON_TYPE_BOOLEAN, &object);
	if(ret == JSON_SUCCESS) {
		JSON_setBoolValue((JSON_BASIC_OBJECT *)object, value);
	}
	g_jsonCtrl.errno = ret;

	return object;
}

/**
 * create JSON null object
 */
JSON_OBJECT *JSON_CreateNULLObject(void)
{
	JSON_OBJECT* object = NULL;

	int ret = JSON_newObject(JSON_TYPE_NULL, &object);
	g_jsonCtrl.errno = ret;

	return object;
}

/**
 * free JSON object
 */
void JSON_DeleteObject(JSON_OBJECT *object)
{
	while (object) {
		JSON_OBJECT *next = object->next;

		if((object->type & JSON_TYPE_REFER) == JSON_TYPE_REFER) {
			JSON_REFER_OBJECT* referObj = (JSON_REFER_OBJECT *)object;
			if(referObj->child) {
				JSON_DeleteObject(referObj->child);
			}
		} else if(object->type == JSON_TYPE_POD_ITEM) {
			JSON_POD_ITEM* podItem = (JSON_POD_ITEM *)object;
			JSON_free((void **)&podItem->name);
			if(podItem->item) {
				JSON_DeleteObject(podItem->item);
			}
		} else if(object->type == JSON_TYPE_STRING) {
			JSON_BASIC_OBJECT* strObj = (JSON_BASIC_OBJECT *)object;
			if (strObj->strVal) JSON_free((void**)&strObj->strVal);
		} else {

		}

		JSON_free((void**)&object);
		object = next;
	}

	g_jsonCtrl.errno = JSON_SUCCESS;
}

/**
 * Add object to plain object
 */
int JSON_AddObjectToPlainObject(JSON_OBJECT *object, const char* name, JSON_OBJECT *childObject)
{
	int ret = JSON_ERR_NOT_POD;

	if(object->type == JSON_TYPE_POD) {
		JSON_POD_ITEM* pPodItem = NULL;
		ret = JSON_newObject(JSON_TYPE_POD_ITEM, (JSON_OBJECT **)&pPodItem);
		if(ret == JSON_SUCCESS) {
			pPodItem->name = JOSN_strdup(name);
			if(pPodItem->name == NULL) {
				ret = g_jsonCtrl.errno;
			} else {
				pPodItem->item = childObject;
				ret = JSON_AddObjectToReferObject(object, (JSON_OBJECT *)pPodItem);
			}
		}
	}
	g_jsonCtrl.errno = ret;

	return ret;
}

/**
 * Add string to plain object
 */
int JSON_AddStringToPlainObject(JSON_OBJECT *object, const char* name, const char* strVal)
{
	int ret = JSON_SUCCESS;

	JSON_BASIC_OBJECT* childObject = NULL;
	ret = JSON_newObject(JSON_TYPE_STRING, (JSON_OBJECT **)&childObject);
	if(ret == JSON_SUCCESS) {
		ret = JSON_AddObjectToPlainObject(object, name, (JSON_OBJECT *)childObject);
		if(ret == JSON_SUCCESS) {
			ret = JSON_setStringValue((JSON_BASIC_OBJECT *)childObject, strVal);
		}
	}
	g_jsonCtrl.errno = ret;

	return ret;
}

/**
 * Add number to plain object
 */
int JSON_AddNumberToPlainObject(JSON_OBJECT *object, const char* name, double number)
{
	int ret = JSON_SUCCESS;

	JSON_OBJECT* childObject = NULL;
	ret = JSON_newObject(JSON_TYPE_NUMBER, &childObject);
	if(ret == JSON_SUCCESS) {
		ret = JSON_AddObjectToPlainObject(object, name, childObject);
		if(ret == JSON_SUCCESS) {
			JSON_setNumberValue((JSON_BASIC_OBJECT *)childObject, number);
		}
	}
	g_jsonCtrl.errno = ret;

	return ret;
}

/**
 * Add boolean to plain object
 */
int JSON_AddBoolToPlainObject(JSON_OBJECT *object, const char* name, JSON_BOOL boolVal)
{
	int ret = JSON_SUCCESS;

	JSON_OBJECT* childObject = NULL;
	ret = JSON_newObject(JSON_TYPE_BOOLEAN, &childObject);
	if(ret == JSON_SUCCESS) {
		ret = JSON_AddObjectToPlainObject(object, name, childObject);
		if(ret == JSON_SUCCESS) {
			JSON_setBoolValue((JSON_BASIC_OBJECT *)childObject, boolVal);
		}
	}
	g_jsonCtrl.errno = ret;

	return ret;
}

/**
 * Add null to plain object
 */
int JSON_AddNULLToPlainObject(JSON_OBJECT *object, const char* name)
{
	int ret = JSON_SUCCESS;

	JSON_OBJECT* childObject = NULL;
	ret = JSON_newObject(JSON_TYPE_NULL, &childObject);
	if(ret == JSON_SUCCESS) {
		ret = JSON_AddObjectToPlainObject(object, name, childObject);
	}
	g_jsonCtrl.errno = ret;

	return ret;
}

/**
 * Add object to Array
 */
int JSON_AddObjectToArray(JSON_OBJECT *object, JSON_OBJECT *childObject)
{
	int ret = (object->type != JSON_TYPE_ARRAY) ? JSON_ERR_NOT_ARRAY : JSON_AddObjectToReferObject(object, childObject);
	g_jsonCtrl.errno = ret;

	return ret;
}

/**
 * Add string to Array
 */
int JSON_AddStringToArray(JSON_OBJECT *object, const char* strVal)
{
	int ret = JSON_SUCCESS;

	JSON_OBJECT* childObject = NULL;
	ret = JSON_newObject(JSON_TYPE_STRING, &childObject);
	if(ret == JSON_SUCCESS) {
		ret = JSON_AddObjectToArray(object, childObject);
		if(ret == JSON_SUCCESS) {
			ret = JSON_setStringValue((JSON_BASIC_OBJECT *)childObject, strVal);
		}
	}
	g_jsonCtrl.errno = ret;

	return ret;
}

/**
 * Add number to plain object
 */
int JSON_AddNumberToArray(JSON_OBJECT *object, double number)
{
	int ret = JSON_SUCCESS;

	JSON_OBJECT* childObject = NULL;
	ret = JSON_newObject(JSON_TYPE_NUMBER, &childObject);
	if(ret == JSON_SUCCESS) {
		ret = JSON_AddObjectToArray(object, childObject);
		if(ret == JSON_SUCCESS) {
			JSON_setNumberValue((JSON_BASIC_OBJECT *)childObject, number);
		}
	}
	g_jsonCtrl.errno = ret;

	return ret;
}

/**
 * Add boolean to plain object
 */
int JSON_AddBoolToArray(JSON_OBJECT *object, JSON_BOOL boolVal)
{
	int ret = JSON_SUCCESS;

	JSON_OBJECT* childObject = NULL;
	ret = JSON_newObject(JSON_TYPE_BOOLEAN, &childObject);
	if(ret == JSON_SUCCESS) {
		ret = JSON_AddObjectToArray(object, childObject);
		if(ret == JSON_SUCCESS) {
			JSON_setBoolValue((JSON_BASIC_OBJECT *)childObject, boolVal);
		}
	}
	g_jsonCtrl.errno = ret;

	return ret;
}

/**
 * Add null to plain object
 */
int JSON_AddNULLToArray(JSON_OBJECT *object)
{
	int ret = JSON_SUCCESS;

	JSON_OBJECT* childObject = NULL;
	ret = JSON_newObject(JSON_TYPE_NULL, &childObject);
	if(ret == JSON_SUCCESS) {
		ret = JSON_AddObjectToArray(object, childObject);
	}
	g_jsonCtrl.errno = ret;

	return ret;
}

/* new a JSON object */
int JSON_newObject(int type, JSON_OBJECT** object)
{
	int ret = JSON_ERR_ARGUMENT;
	size_t objectSize = 0;

	if(object != NULL) {
		*object = NULL;

		if(type == JSON_TYPE_ARRAY || type == JSON_TYPE_POD) {
			objectSize = sizeof(JSON_REFER_OBJECT);
		} else if(type == JSON_TYPE_STRING || type == JSON_TYPE_NUMBER ||
			      type == JSON_TYPE_BOOLEAN || type == JSON_TYPE_NULL) {
			objectSize = sizeof(JSON_BASIC_OBJECT);
		} else if(type == JSON_TYPE_POD_ITEM) {
			objectSize = sizeof(JSON_POD_ITEM);
		}

		if(objectSize > 0) {
			ret = JSON_alloc(objectSize, (void **)object);
			if(ret == JSON_SUCCESS) {
				(*object)->type = type;
			}
		}
	}

	return ret;
}

/* Add object to reference object */
int JSON_AddObjectToReferObject(JSON_OBJECT *object, JSON_OBJECT *childItem)
{
	int ret = JSON_ERR_ARGUMENT;

	if((object->type & JSON_TYPE_REFER) == JSON_TYPE_REFER && childItem != NULL) {
		JSON_REFER_OBJECT * pRefer = (JSON_REFER_OBJECT *)object;
		if(pRefer->lastChild) {
			pRefer->lastChild->next = childItem;
		} else {
			pRefer->child = childItem;
		}
		pRefer->lastChild = childItem;
		ret = JSON_SUCCESS;
	}

	return ret;
}

/*--------------------------------------------------------------------------------------------------------------------------------
 * JSON Iterator
 *--------------------------------------------------------------------------------------------------------------------------------*/
/**
 * Initialize iterator
 */
int JSON_InitIterator(JSON_OBJECT *object, JSON_ITERATOR* iterator)
{
	int ret = JSON_ERR_ARGUMENT;

	if (object && iterator) {
		memset(iterator, 0, sizeof(JSON_ITERATOR));
		iterator->type = object->type;

		if((object->type & JSON_TYPE_REFER) == JSON_TYPE_REFER) {
			JSON_REFER_OBJECT* referObject = ((JSON_REFER_OBJECT *)object);
			iterator->next = (JSON_OBJECT *)referObject->child;
		} else {
			iterator->next = object;
		}

		ret = JSON_SUCCESS;
	}
	g_jsonCtrl.errno = ret;

	return ret;
}

/**
 * Iterate next object (Plain Object)
 */
JSON_BOOL JSON_IterNextChild(JSON_ITERATOR* iterator)
{
	JSON_BOOL hasItem = JSON_FALSE;

	if(iterator->next != NULL) {
		hasItem = JSON_TRUE;

		if(iterator->type == JSON_TYPE_ARRAY) {
			iterator->current = iterator->next;
			iterator->next = iterator->next->next;
		} else if(iterator->type == JSON_TYPE_POD) {
			iterator->name = ((JSON_POD_ITEM *)iterator->next)->name;
			iterator->current = ((JSON_POD_ITEM *)iterator->next)->item;
			iterator->next = iterator->next->next;
		} else {
			iterator->current = iterator->next;
			iterator->next = NULL;
		}
	}

	return hasItem;
}

/**
 * Get value (String)
 */
const char* JSON_GetValueOfStringObject(JSON_OBJECT *object)
{
	return (object->type != JSON_TYPE_STRING) ? "" : ((JSON_BASIC_OBJECT *)object)->strVal;
}

/**
 * Get value (Number)
 */
double JSON_GetValueOfNumberObject(JSON_OBJECT *object)
{
	return (object->type != JSON_TYPE_NUMBER) ? 0 : ((JSON_BASIC_OBJECT *)object)->numVal.dval;
}

/**
 * Get value (Boolean)
 */
JSON_BOOL JSON_GetValueOfBoolObject(JSON_OBJECT *object)
{
	return (object->type != JSON_TYPE_BOOLEAN) ? JSON_FALSE : ((JSON_BASIC_OBJECT *)object)->boolVal;
}

/*--------------------------------------------------------------------------------------------------------------------------------
 * JSON render
 *--------------------------------------------------------------------------------------------------------------------------------*/
/**
 * Render JSON object
 */
int JSON_Render(JSON_OBJECT *object, JSON_BOOL fmt, JSON_RENDER_CALLBACK callback, void* userData)
{
	int ret = JSON_ERR_ARGUMENT;

	if (object) {
		JSON_RENDER_CONTEXT context = { .depth = 0, .needformat = fmt, .preFmtLen = 0, .callback = callback, .userData = userData};
		ret = JSON_RenderCore(object, &context);
		JSON_RenderCrlf(ret, &context);
	}
	g_jsonCtrl.errno = ret;

	return ret;
}

/* Render JSON core */
int JSON_RenderCore(JSON_OBJECT *object, JSON_RENDER_CONTEXT* context)
{
	int ret = JSON_ERR_ARGUMENT;

	if (object) {
		switch(object->type) {
		case JSON_TYPE_POD:
			ret = JSON_RenderPlainObject((JSON_REFER_OBJECT*)object, context);
			break;
		case JSON_TYPE_ARRAY :
			ret = JSON_RenderArray((JSON_REFER_OBJECT*)object, context);
			break;
		case JSON_TYPE_STRING :
			ret = JSON_RenderString((JSON_BASIC_OBJECT *)object, context);
			break;
		case JSON_TYPE_NUMBER :
			ret = JSON_RenderNumber((JSON_BASIC_OBJECT *)object, context);
			break;
		case JSON_TYPE_BOOLEAN :
			ret = JSON_RenderBuffer(context, JSON_FALSE, ((JSON_BASIC_OBJECT *)object)->boolVal ? "true" : "false");
			break;
		case JSON_TYPE_NULL :
			ret = JSON_RenderBuffer(context, JSON_FALSE, "null");
			break;
		default :
			ret = JSON_ERR_JSON_TYPE;
			break;
		}
	}

	return ret;
}

/* Render JSON POD */
int JSON_RenderPlainObject(JSON_REFER_OBJECT *podObject, JSON_RENDER_CONTEXT* context)
{
	int ret = JSON_ERR_RENDER;
	int preFmtLen = context->preFmtLen;

	if (!podObject->child) {
		ret = JSON_RenderBuffer(context, JSON_FALSE, "{}");
	} else {
		ret = JSON_RenderBuffer(context, JSON_FALSE, "{");
		JSON_RenderCrlf(ret, context);

		context->depth ++;

		JSON_POD_ITEM* podItem = (JSON_POD_ITEM*)podObject->child;
		while(ret == JSON_SUCCESS && podItem != NULL) {
			context->preFmtLen = preFmtLen;
			ret = JSON_RenderBuffer(context, JSON_TRUE, context->needformat ?  "\"%s\" : " : "\"%s\":", podItem->name);
			if(ret == JSON_SUCCESS) {
				context->preFmtLen = preFmtLen + strlen(podItem->name) + 2;
				ret = JSON_RenderCore(podItem->item, context);
				if(podItem != (JSON_POD_ITEM*)podObject->lastChild) {
					ret = JSON_RenderBuffer(context, JSON_FALSE, ",");
				}
				JSON_RenderCrlf(ret, context);
			}
			podItem = (JSON_POD_ITEM*)podItem->object.next;
		}

		context->depth --;
		context->preFmtLen = preFmtLen;

		ret = JSON_RenderBuffer(context, JSON_TRUE, "}");
	}

	return ret;
}

/* Render JSON array */
int JSON_RenderArray(JSON_REFER_OBJECT *arrayObject, JSON_RENDER_CONTEXT* context)
{
	int ret = JSON_ERR_RENDER;

	if (!arrayObject->child) {
		ret = JSON_RenderBuffer(context, JSON_FALSE, "[]");
	} else {
		ret = JSON_RenderBuffer(context, JSON_FALSE, "[");
		JSON_RenderCrlf(ret, context);

		context->depth ++;

		JSON_OBJECT* childItem = arrayObject->child;
		while(ret == JSON_SUCCESS && childItem != NULL) {
			ret = JSON_RenderBuffer(context, JSON_TRUE, "");
			if(ret == JSON_SUCCESS) {
				ret = JSON_RenderCore(childItem, context);
				if(childItem != arrayObject->lastChild) {
					ret = JSON_RenderBuffer(context, JSON_FALSE, ",");
				}
				JSON_RenderCrlf(ret, context);
			}
			childItem = childItem->next;
		}

		context->depth --;

		ret = JSON_RenderBuffer(context, JSON_TRUE, "]");
	}

	return ret;
}

/* Render JSON number */
int JSON_RenderNumber(JSON_BASIC_OBJECT *basicObject, JSON_RENDER_CONTEXT* context)
{
	int ret = JSON_ERR_RENDER;

	if(basicObject->object.type == JSON_TYPE_NUMBER) {
		double dval = basicObject->numVal.dval;
		long long lval = basicObject->numVal.lval;
		if(dval == 0) {
			ret = JSON_RenderBuffer(context, JSON_FALSE, "0");
		} else if(fabs((double)lval - dval) <= DBL_EPSILON && dval >= INT_MIN && dval <= UINT_MAX) {
			ret = JSON_RenderBuffer(context, JSON_FALSE, "%lld", lval);
		} else if(fabs(floor(dval) - dval) <= DBL_EPSILON && fabs(dval) < 1.0e60) {
			ret = JSON_RenderBuffer(context, JSON_FALSE, "%.6g", dval);
		} else if(fabs(dval) <= 1.0e-6 || fabs(dval) >= 1.0e9) {
			ret = JSON_RenderBuffer(context, JSON_FALSE, "%e", dval);
		} else {
			ret = JSON_RenderBuffer(context, JSON_FALSE, "%f", dval);
		}
	}

	return ret;
}

/* render JSON string */
int JSON_RenderString(JSON_BASIC_OBJECT *basicObject, JSON_RENDER_CONTEXT* context)
{
	int ret = JSON_ERR_RENDER;

	if(basicObject->object.type == JSON_TYPE_STRING) {
		if(!context->callback) {
			return JSON_SUCCESS;
		}

		if(context->callback("\"", context->userData)) {
			if(context->callback(!basicObject->strVal ? "" : basicObject->strVal, context->userData)) {
				ret = context->callback("\"", context->userData) ? JSON_SUCCESS : JSON_ERR_RENDER;
			}
		}
	}

	return ret;
}

/* render buffer */
int JSON_RenderBuffer(JSON_RENDER_CONTEXT* context, JSON_BOOL needIdent, char *lineFmt, ...)
{
	int ret = JSON_ERR_RENDER;

	if(!context->callback) {
		return JSON_SUCCESS;
	}

	va_list vargs;
	char data[128];

	if(context->needformat && needIdent) {
		int ident = context->depth + (context->preFmtLen + 7) / 8;
		int i;
		for(i = 0; i < ident; i++) {
			context->callback("\t", context->userData);
		}
	}

	va_start(vargs, lineFmt);
	vsnprintf(data, sizeof(data), lineFmt, vargs);
	va_end(vargs);

	ret = context->callback(data, context->userData) ? JSON_SUCCESS : JSON_ERR_RENDER;

	return ret;
}

/*--------------------------------------------------------------------------------------------------------------------------------
 * JSON parse
 *--------------------------------------------------------------------------------------------------------------------------------*/
/**
 * Parse JSON object
 */
JSON_OBJECT* JSON_Parse(const char* text)
{
	int ret = JSON_ERR_ARGUMENT;
	const char* nextText = NULL;
	JSON_OBJECT* object = NULL;

	g_jsonCtrl.parseError[0] = '\0';
	if(text) {
		ret = JSON_ParseCore(text, &object, &nextText);
		if(ret != JSON_SUCCESS) {
			snprintf(g_jsonCtrl.parseError, sizeof(g_jsonCtrl.parseError), "%s", nextText);
		}
	}
	g_jsonCtrl.errno = ret;

	return object;
}

const char *JSON_GetParseError() {
	return g_jsonCtrl.parseError;
}

/* parse json object */
int JSON_ParseCore(const char* text, JSON_OBJECT** object, const char** nextText)
{
	int ret = JSON_ERR_PARSE_KEYWORD;

	const char *token = JSON_getToken(text);
	if (*token=='{')	{
		return JSON_ParsePlainObject(token, object, nextText);
	} else if (*token=='[')	{
		return JSON_ParseArrayObject(token, object, nextText);
	} else if (*token=='\"' || *token=='\'')	{
		return JSON_ParseStringObject(token, object, nextText);
	} else if (!strncmp(token,"true",4) || !strncmp(token,"false",5)) {
		return JSON_ParseBooleanObject(token, object, nextText);
	} else if (!strncmp(token,"null",4)) {
		return JSON_ParseNULLObject(token, object, nextText);
	} else if (*token=='-' || isdigit(*token)){
		return JSON_ParseNumberObject(token, object, nextText);
	} else {
		*nextText = text;
	}

	return ret;
}

/* parse plain object */
int JSON_ParsePlainObject(const char* text, JSON_OBJECT** object, const char** nextText)
{
	int ret = JSON_ERR_PARSE_POD;
	JSON_OBJECT *podObject = NULL;

	*object = NULL; *nextText = text;

	const char *token = JSON_getToken(text);
	if (*token != '{') {
		return ret;
	}

	if((ret = JSON_newObject(JSON_TYPE_POD, &podObject)) != JSON_SUCCESS) {
		return ret;
	}

	const char *nextToken = JSON_getToken(token + 1);
	if (*nextToken == '}') {
		*object = podObject; *nextText = token + 2;
		ret = JSON_SUCCESS;
	} else {
		do {
			ret = JSON_ParsePlainItem(token+1, podObject, nextText);
			if(ret == JSON_SUCCESS) {
				token = JSON_getToken(*nextText);
			} else {
				token = *nextText;
			}
		} while(ret == JSON_SUCCESS && *token == ',');

		if(ret == JSON_SUCCESS && *token != '}') {
			ret = JSON_ERR_PARSE_POD;
		}

		if(ret == JSON_SUCCESS) {
			*nextText = token + 1; *object = podObject;
		} else {
			JSON_DeleteObject(podObject);
		}
	}

	return ret;
}

/* parse each item of plain object */
int JSON_ParsePlainItem(const char* text, JSON_OBJECT *podObject, const char** nextText)
{
	int ret = JSON_SUCCESS;

	const char *token = JSON_getToken(text);
	int tokenLen = JSON_getTokenLen(token, ':');

	char tail = (*token == '\'' || *token == '\"') ? (*token) : 0;
	if(tokenLen <= 0) {
		*nextText = text;
		ret = JSON_ERR_PARSE_NAME;
	} else {
		int nameLen = tokenLen;
		if(tail != 0) {
			nameLen = JSON_getTokenLen(token + 1, tail);
			if (*JSON_getToken(token + 1 + nameLen) != tail) {
				*nextText = token;
				ret = JSON_ERR_PARSE_NAME;
			}
			token ++;
		}

		char *PodItemName = NULL;
		if(nameLen <= 0) {
			*nextText = text;
			ret = JSON_ERR_PARSE_NAME;
		} else if(ret == JSON_SUCCESS) {
			ret = JSON_alloc(nameLen + 1, (void **)&PodItemName);
			if(ret == JSON_SUCCESS) {
				memcpy(PodItemName, token, nameLen);
				PodItemName[nameLen] = 0;
			}
		}

		if(ret == JSON_SUCCESS) {
			token = JSON_getToken(token + nameLen + (tail != 0 ? 1 : 0));
			if (*token != ':') {
				*nextText = token;
				ret = JSON_ERR_PARSE_NAME;
			} else {
				JSON_OBJECT* childObject = NULL;
				ret = JSON_ParseCore(token + 1, &childObject, nextText);
				if(ret == JSON_SUCCESS) {
					ret = JSON_AddObjectToPlainObject(podObject, PodItemName, childObject);
				}
			}
		}

		JSON_free((void **)&PodItemName);
	}

	return ret;
}

/* parse Array object */
int JSON_ParseArrayObject(const char* text, JSON_OBJECT** object, const char** nextText)
{
	int ret = JSON_ERR_PARSE_ARRAY;
	JSON_OBJECT *arrayObject = NULL;

	*object = NULL; *nextText = text;

	const char *token = JSON_getToken(text);
	if (*token != '[') {
		return ret;
	}

	if((JSON_newObject(JSON_TYPE_ARRAY, &arrayObject)) != JSON_SUCCESS) {
		return ret;
	}

	const char *nextToken = JSON_getToken(token + 1);
	if (*nextToken == ']') {
		*nextText = nextToken + 1; *object = arrayObject;
		ret = JSON_SUCCESS;
	} else {
		do {
			JSON_OBJECT* childObject = NULL;
			ret = JSON_ParseCore(token+1, &childObject, nextText);
			if (ret == JSON_SUCCESS) {
				token = JSON_getToken(*nextText);
				ret = JSON_AddObjectToArray(arrayObject, childObject);
			} else {
				token = *nextText;
			}
		} while(ret == JSON_SUCCESS && *token == ',');

		if(ret == JSON_SUCCESS && *token != ']') {
			ret = JSON_ERR_PARSE_ARRAY;
		}

		if(ret == JSON_SUCCESS) {
			*object = arrayObject; *nextText = token + 1;
		} else {
			JSON_DeleteObject(arrayObject);
		}
	}

	return ret;
}

/* parse string object */
int JSON_ParseStringObject(const char* text, JSON_OBJECT** object, const char** nextText)
{
	int ret = JSON_ERR_PARSE_STR;
	JSON_BASIC_OBJECT *strObject = NULL;

	*object = NULL; *nextText = text;

	const char *token = JSON_getToken(text);
	char tail = (*token == '\'' || *token == '\"') ? (*token) : 0;
	if(tail == 0) {
		return ret;
	}

	if((JSON_newObject(JSON_TYPE_STRING, (JSON_OBJECT **)&strObject)) != JSON_SUCCESS) {
		return ret;
	}

	token = JSON_getToken(token + 1);
	if(*token != 0) {
		int tokenLen = JSON_getTokenLen(token, tail);
		if(tokenLen == 0 && *(token + 1) == tail) {
			*nextText = token;
			strObject->strVal = NULL;
			*object = (JSON_OBJECT*)strObject;
		} else {
			ret = JSON_getString(token, tail, &strObject->strVal, &tokenLen);
			if(ret == JSON_SUCCESS) {
				*object = (JSON_OBJECT*)strObject;
				*nextText = token + tokenLen + 1;
			} else {
				*object = NULL; *nextText = token;
				JSON_DeleteObject((JSON_OBJECT *)strObject);
			}
		}
	}

	return ret;
}

int JSON_ParseNumberObject(const char* text, JSON_OBJECT** object, const char** nextText)
{
	JSON_BASIC_OBJECT *numObject = NULL;
	int ret = JSON_newObject(JSON_TYPE_NUMBER, (JSON_OBJECT **)&numObject);
	if(ret != JSON_SUCCESS) {
		return ret;
	}

	const char *word = JSON_getToken(text);

	double sign = 1, scale = 0;
	int    subscale=0, signsubscale = 1;
	double numVal = 0;

	// sign
	if (*word == '-') {
		sign = -1;
		word ++;
	} else if(*word == '+') {
		word ++;
	}

	// prefix padding 0
	while (*word == '0' ) {
		word ++;
	}

	if (*word >= '1' && *word <= '9') {
		do	{
			numVal = (numVal * 10.0) + (*word++ - '0');
		} while (*word >= '0' && *word <= '9');
	}

	if (*word == '.' && word[1] >= '0' && word[1] <= '9') {
		word++;
		do	{
			numVal = (numVal * 10.0)+(*word++ - '0');
			scale--;
		} while (*word>='0' && *word<='9');
	}

	if (*word == 'e' || *word == 'E') {
		word++;
		if (*word=='+') {
			word++;
		} else if (*word=='-') {
			signsubscale=-1;
			word++;
		}
		while (*word >= '0' && *word <= '9') {
			subscale=(subscale * 10) + (*word++ - '0');
		}
	}

	*nextText = word;

	/* number = +/- number.fraction * 10^ +/- exponent */
	numVal = sign * numVal * pow(10.0, (scale + subscale * signsubscale));

	numObject->numVal.dval = numVal;
	numObject->numVal.lval = (long long)numVal;

	*object = (JSON_OBJECT *)numObject;

	return ret;
}

/* parse boolean object */
int JSON_ParseBooleanObject(const char* text, JSON_OBJECT** object, const char** nextText)
{
	int ret = JSON_ERR_PARSE_BOOL;
	JSON_BASIC_OBJECT *boolObject = NULL;

	*object = NULL; *nextText = text;

	const char *token = JSON_getToken(text);
	const char *nextToken = NULL;
	if(strncmp(token, "true", 4) == 0) {
		nextToken = token + 4;
	} else if(strncmp(token, "false", 4) == 0) {
		nextToken = token + 5;
	}

	if (nextToken && (isspace(*nextToken) || *nextToken == '}' || *nextToken == ',')) {
		ret = JSON_newObject(JSON_TYPE_BOOLEAN, (JSON_OBJECT **)&boolObject);
		if(ret == JSON_SUCCESS) {
			boolObject->boolVal = (strncmp(token, "true", 4) == 0) ? JSON_TRUE : JSON_FALSE;
			*object = (JSON_OBJECT *)boolObject; *nextText = nextToken;
		} else {
			*object = NULL; *nextText = text;
		}
	}

	return ret;
}

/* parse null object */
int JSON_ParseNULLObject(const char* text, JSON_OBJECT** object, const char** nextText)
{
	int ret = JSON_ERR_PARSE_NULL;
	JSON_BASIC_OBJECT *nullObject = NULL;

	*object = NULL; *nextText = text;

	const char *token = JSON_getToken(text);
	const char *nextToken = (strncmp(token, "null", 4) == 0 ? (token + 4) : NULL);

	if (nextToken && (isspace(*nextToken) || *nextToken == '}' || *nextToken == ',')) {
		ret = JSON_newObject(JSON_TYPE_NULL, (JSON_OBJECT **)&nullObject);
		if(ret == JSON_SUCCESS) {
			*object = (JSON_OBJECT *)nullObject; *nextText = nextToken;
		} else {
			*object = NULL; *nextText = text;
		}
	}

	return ret;
}

/* get token (start pointer) */
const char *JSON_getToken(const char *input)
{
	while (*input && isspace(*input)) {
		input++;
	}

	return input;
}

/* get token (length) */
int JSON_getTokenLen(const char *input, const char tail)
{
	int wordLen = 0;
	while (*input && !isspace(*input) && *input != tail) {
		wordLen++;
		input++;
	}

	return wordLen;
}

/* get token(string) */
int JSON_getString(const char* text, const char tail, char** str, int* strLen)
{
	int ret = JSON_ERR_PARSE_STR;
	int nestCount = 0;
	const char *cursor = text;
	int string_len = 0;

	*str = NULL; *strLen = 0;

	do {
		cursor = JSON_getToken(cursor);
		string_len = JSON_getTokenLen(cursor, tail);
		cursor += string_len;

		if(*cursor == tail) {
			if(nestCount > 0 && *(cursor + 1) == ')') {
				nestCount --;
				cursor += 2;
			} else if(*(cursor - 1) == '(') {
				nestCount ++;
				cursor ++;
			} else {
				ret = JSON_SUCCESS;
				break;
			}
		} else if(*cursor == '\0' || *cursor == '}' || *cursor == ',') {
			*strLen = cursor - text - 1;
			break;
		}
	} while(JSON_TRUE);

	if(ret == JSON_SUCCESS) {
		string_len = cursor - text;
		ret = JSON_alloc(string_len + 1, (void **)str);
		if(ret == JSON_SUCCESS) {
			memcpy(*str, text, string_len);
			(*str)[string_len] = 0;
			*strLen = string_len;
		}
	}

	return ret;
}
