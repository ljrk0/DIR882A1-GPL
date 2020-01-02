/**
 * JSON common lib
 */
#include <stdio.h>
#include <stdlib.h>

#include "json.h"


JSON_BOOL jsonOutput(const char* out, void* userData) {
	FILE* fp = (FILE*)userData;

	if(fp) {
		fprintf(fp, "%s", out);
	} else {
		printf("%s", out);
	}
	return JSON_TRUE;
}

void createJsonTree(const char* pTestFile)
{
	JSON_OBJECT *leafObject = JSON_CreatePlainObject();
	JSON_AddStringToPlainObject(leafObject, "name", "leaf");
	JSON_AddNumberToPlainObject(leafObject, "value", 0);

	JSON_OBJECT *childArray = JSON_CreateArrayObject();

	JSON_AddStringToArray(childArray, "array item 1");
	JSON_AddBoolToArray(childArray,   1);
	JSON_AddNumberToArray(childArray, 2345);
	JSON_AddObjectToArray(childArray, JSON_CreatePlainObject());
	JSON_AddObjectToArray(childArray, leafObject);
	JSON_AddNULLToArray(childArray);

	JSON_OBJECT *root = JSON_CreatePlainObject();

	JSON_AddStringToPlainObject(root, "name", "Jack (\"Bee\") Nimble");
	JSON_AddBoolToPlainObject(root, "isNNN", 1);
	JSON_AddNumberToPlainObject(root, "value1", 2345.66);
	JSON_AddNULLToPlainObject(root, "value3");
	JSON_AddObjectToPlainObject(root, "array", childArray);

	FILE* fp = fopen(pTestFile, "w");
	if(fp != NULL) {
		JSON_Render(root, JSON_TRUE, jsonOutput, (void *)fp);
		JSON_DeleteObject(root);
		fclose(fp);
	}

	printf("\n");
}

void parseJsonTree(const char* pTestFile)
{
	FILE* fp = fopen(pTestFile, "r");
	if(fp != NULL) {
		fseek(fp, 0, SEEK_END);
		int fileSize = ftell(fp);
		char *buff = calloc(1, fileSize + 1);

		fseek(fp, 0, SEEK_SET);
		fread(buff, fileSize, 1, fp);

		JSON_OBJECT *root = JSON_Parse(buff);
		if(root == NULL) {
			printf("ERROR : %s\n", JSON_GetParseError());
		} else {
			JSON_Render(root, JSON_TRUE, jsonOutput, (void *)NULL);
		}

		free(buff);
		fclose(fp);
	}
}

int main (int argc, const char * argv[])
{
	createJsonTree("json.txt");
	parseJsonTree("json.txt");

	return 0;
}
