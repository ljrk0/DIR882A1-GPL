#include <stdio.h>
#include <stdarg.h>

#include <dlfcn.h>

#include "dm_srv.h"
#include "dm_internal.h"
#include "device_external_type.h"
#include "dmJson_addon.h"

#include "prc_ps_log.h"
#include "prc_perview.h"

static int g_dmLogOutEnable = DM_FALSE;

/**
 *  Trace output log
 */
void  DmTest_Log(const INT8 *file, INT32 line, UINT16 traceLevel, UINT32 errType, INT8 *pTraceContext)
{
	switch(traceLevel)
	{
	case DM_TRACE_LEVEL_LOW :
		//ps_board_trace(GROUP_OAM_DM,TRACE_LEVEL_LOW, "TEST : %s", pTraceContext);
		break;
	case DM_TRACE_LEVEL_MIDDLE :
		//ps_board_trace(GROUP_OAM_DM,TRACE_LEVEL_MED, "TEST : %s", pTraceContext);
		break;
	case DM_TRACE_LEVEL_HIGH:
		//ps_board_trace(GROUP_OAM_DM,TRACE_LEVEL_HIGH, "TEST : %s", pTraceContext);
		if (g_dmLogOutEnable) printf("INFO : %s \n", pTraceContext);
		break;
	case DM_TRACE_LEVEL_WARNING :
		//ps_board_trace(GROUP_OAM_DM, TRACE_LEVEL_WARNING, "TEST : %s", pTraceContext);
		printf("WARN : %s \n", pTraceContext);
		break;
	case DM_TRACE_LEVEL_ERROR :
		//ps_board_trace(GROUP_OAM_DM,TRACE_LEVEL_ERROR, "TEST : %s", pTraceContext);
		printf("ERROR : %s \n", pTraceContext);
		break;
	default :
		//ps_board_trace(GROUP_OAM_DM, TRACE_LEVEL_ERROR, "TEST : %s", pTraceContext);
		printf("ERROR : %s \n", pTraceContext);
		break;
	}

	return;
}

static void dm_test_loadDmJson(INT8* pucPath);
static void dm_test_updDmJson(void);

/**
 *  main
 */
int main(int argc, char **argv)
{
	INT32 lRet = DM_SUCCESS;

	//prc_trace_service_start(PRC_TRACE_SERVICE_AS_CLIENT,0);
	DmTraceHookerRegister(DmTest_Log);

#if 1
	if (DM_SUCCESS != DmServiceStartReq(NULL))
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "start DM failed.");
		return -1;
	}
#else
	DM_START_REQ_T startReq;	
	osa_mem_set(&startReq, 0, sizeof(DM_START_REQ_T));
	strncpy(startReq.entity, "Oam", sizeof(startReq.entity) - 1);
	if(DM_SUCCESS != DmStartReq(&startReq))
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR, "start DM failed.");
		return -1;
	}
#endif

	DM_LOAD_DB_HANDLER_REQ_T LoadDbHandlerReq;
	osa_mem_set(&LoadDbHandlerReq, 0, sizeof(DM_LOAD_DB_HANDLER_REQ_T));
	LoadDbHandlerReq.bDbReset = DM_FALSE;
	LoadDbHandlerReq.bXmlWithComment = DM_FALSE;

	snprintf(LoadDbHandlerReq.dbLibPath,   sizeof(LoadDbHandlerReq.dbLibPath),   "/oam/libs/");
    	snprintf(LoadDbHandlerReq.workingPath, sizeof(LoadDbHandlerReq.workingPath), "/tmp/");
    	snprintf(LoadDbHandlerReq.loadPath,    sizeof(LoadDbHandlerReq.loadPath),    "/data/");
    	snprintf(LoadDbHandlerReq.backupPath,  sizeof(LoadDbHandlerReq.backupPath),  "/store/");
	/* If LoadDbHandlerReq.dbName is null, all db will be loade in /oam/libs */

	lRet = DmLoadGlobalDbHandlerReq(&LoadDbHandlerReq);
	if (lRet != DM_SUCCESS)
	{
		DM_TRACE(DM_TRACE_LEVEL_ERROR,"Failed to load db.");
	}

	if(argc > 1 && strcmp(argv[1], "upd") ==0)
	{
		dm_test_updDmJson();
	} 
	else 
	{
		dm_test_loadDmJson(argc > 1  ? argv[1] : "Device.");
	}

	DmServiceStopReq();
	//prc_trace_service_stop();

	return 0;
}

/**
 * DON't delete this function
 * unused function, but it can load libDbConv in compile
 */
void DmTest_LoadDbConvLib()
{
	INT8* pData = DmConvertIntToStr(2);
	printf("%s \n", pData);
	free(pData);
}


void dm_test_loadDmJson(INT8* pucPath)
{
	g_dmLogOutEnable = DM_TRUE;

	DM_ADDON_CONTROL_REQ_T jsonAddonCtrl;
	osa_mem_set(&jsonAddonCtrl, 0, sizeof(DM_ADDON_CONTROL_REQ_T));
	jsonAddonCtrl.action = DM_ADDON_CTRL_ACTION_LOAD;
	snprintf(jsonAddonCtrl.addonPath, sizeof(jsonAddonCtrl.addonPath), "../lib");
	snprintf(jsonAddonCtrl.addonName, sizeof(jsonAddonCtrl.addonName), "Json");
	
	INT32 lRet = DmAddOnControlReq(&jsonAddonCtrl);
	if(lRet == DM_SUCCESS)
	{
		DM_JSON_GET_OBJECT_T jsonGetObject;
		jsonGetObject.msg.reqId = DM_JSON_REQ_MSG_GET_OBJECT;
		snprintf(jsonGetObject.accessEntity,  sizeof(jsonGetObject.accessEntity), "%s", "GUI");
		snprintf(jsonGetObject.pucPath, sizeof(jsonGetObject.pucPath), "%s",  pucPath);
		snprintf(jsonGetObject.pucJsonFile, sizeof(jsonGetObject.pucJsonFile), "%s", "./test.json");
		jsonGetObject.isFormat = DM_TRUE;
		jsonGetObject.depth = 2;

		DM_ADDON_REQ_MESSAGE_T jsonReqMsg = { "Json", (void *)&jsonGetObject, sizeof(DM_JSON_GET_OBJECT_T) };
		lRet = DmAddOnRouteMsgReq(&jsonReqMsg);
	}
	jsonAddonCtrl.action = DM_ADDON_CTRL_ACTION_UNLOAD;
	lRet = DmAddOnControlReq(&jsonAddonCtrl);

	g_dmLogOutEnable = DM_FALSE;
}

void dm_test_updDmJson(void)
{
	g_dmLogOutEnable = DM_TRUE;

	char* pJson = "{\"path\":\"Internal.\",\"action\":\"M\",\"list\":{\"VendorDataModelVersion\":\"2\"} }";
	//char* pJson = "{\"path\":\"Device.Ethernet.Interface.\",\"action\":\"A\"}";
	//char* pJson = "{\"path\":\"Device.Ethernet.Interface.3.\",\"action\":\"D\"}";
	FILE* fp = fopen("./test.json", "w");
	if(fp == NULL)
	{
		printf("failed to write test file");
	}
	fwrite(pJson, 1, strlen(pJson), fp);
	fclose(fp);

	DM_ADDON_CONTROL_REQ_T jsonAddonCtrl;
	osa_mem_set(&jsonAddonCtrl, 0, sizeof(DM_ADDON_CONTROL_REQ_T));
	jsonAddonCtrl.action = DM_ADDON_CTRL_ACTION_LOAD;
	snprintf(jsonAddonCtrl.addonPath, sizeof(jsonAddonCtrl.addonPath), "../lib");
	snprintf(jsonAddonCtrl.addonName, sizeof(jsonAddonCtrl.addonName), "Json");

	INT32 lRet = DmAddOnControlReq(&jsonAddonCtrl);
	if(lRet == DM_SUCCESS)
	{
		DM_JSON_UPD_OBJECT_T jsonUpdObject;
		jsonUpdObject.msg.reqId = DM_JSON_REQ_MSG_UPD_OBJECT;
		snprintf(jsonUpdObject.accessEntity,  sizeof(jsonUpdObject.accessEntity), "%s", "GUI");
		snprintf(jsonUpdObject.pucJsonFile,   sizeof(jsonUpdObject.pucJsonFile),  "%s", "./test.json");
		jsonUpdObject.isFormatResult = DM_TRUE;

		DM_ADDON_REQ_MESSAGE_T jsonReqMsg = { "Json", (void *)&jsonUpdObject, sizeof(DM_JSON_UPD_OBJECT_T) };
		lRet = DmAddOnRouteMsgReq(&jsonReqMsg);
	}
	jsonAddonCtrl.action = DM_ADDON_CTRL_ACTION_UNLOAD;
	lRet = DmAddOnControlReq(&jsonAddonCtrl);

	g_dmLogOutEnable = DM_FALSE;
}

