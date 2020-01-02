/**********************************************************************
 Copyright (c), 1991-2007, T&W ELECTRONICS(SHENTHEN) Co., Ltd.
 文件名称 : cli_comm.c
 文件描述 : 命令行进程的通信模块处理


 函数列表 :


 修订记录 :
          1 创建 : liuzequn
            日期 : 2007-11-6
            描述 :

**********************************************************************/

#include <crypt.h>
#include "../cli_comm.h"
#include "../include/tbsutil.h"

/******************************************************************************
 *                                 MACRO                                      *
 ******************************************************************************/
/*
 *该宏针对MSG_CMM_GET_VAL_ACK类型的消息，用于
 *从该类型消息中的一个item中提取出值.
 *MSG_CMM_GET_VAL_ACK类型的消息定义请查看WIKI
 *中的<TBS R2 消息格式>
 */
#define CLI_GET_LEAF_VALUE(pcPointer, item) do \
{ \
    (pcPointer) = strchr((item), '='); \
    if ((pcPointer) == NULL) \
    { \
        IO_Print("Message format error!!"); \
        CLI_MSG_RELEASE(g_pstCliMsg); \
        return TBS_FAILED; \
    } \
    (pcPointer)++; \
}while(0);

/*
 *本宏也是针对MSG_CMM_GET_VAL_ACK消息
 *指所以定义本宏，是增加可读性
 */
#define CLI_MOVE2_NEXT_ITEM(curItemStart) do \
{ \
    (curItemStart) += STB_StrLen((curItemStart)) + 1; \
}while(0);

/******************************************************************************
 *                                STRUCT                                      *
 ******************************************************************************/



/******************************************************************************
 *                                GLOBAL                                      *
 ******************************************************************************/

static char     g_szCliFeedbackError[1024] = "";    // 反馈信息
static ST_MSG  *g_pstCliMsg = NULL;
//用户列表
static struct stUserInfoList g_stUserList;


/******************************************************************************
 *                               FUNCTION                                     *
 ******************************************************************************/
static int CLI_StoreUserInfo(void);
static ULONG CLI_ConfigLever2CliLevel(ULONG ulConfigLevel);


/*************************************************************************
功能:从配置树中读取用户信息
参数:无
返回:
     TBS_SUCCESS:获取信息成功
     TBS_FAILED :获取信息失败
备注:使用了全局变量g_pstCliMsg
 *************************************************************************/
int CLI_GetUserInfo(void)
{
    ST_MSG *pstMsg;      /* 发送的消息指针 */
    char   szPath[] = "InternetGatewayDevice.X_TWSZ-COM_Authentication.UserList.";
    int    iRet = TBS_FAILED; 
    USHORT usLen = strlen(szPath);

    //分配消息体空间
    // 准确应该是 sizeof(ULONG) + usLen + '\0'
    pstMsg = MSG_CreateMessage(usLen + 8);
    if (pstMsg == NULL)
    {
        IO_Print("Out of Memory");
        return TBS_FAILED;
    }

    //构造消息
    pstMsg->stMsgHead.ulBodyLength = strlen(szPath) + 1 + sizeof(ULONG);
    *(ULONG *)pstMsg->szMsgBody = 1;
    strcpy(pstMsg->szMsgBody+ sizeof(ULONG), szPath);
    pstMsg->stMsgHead.ulMsgID   = 0;
    pstMsg->stMsgHead.usSrcMID  = MID_CLI;
    pstMsg->stMsgHead.usDstMID  = MID_CMM;
    pstMsg->stMsgHead.usMsgType = MSG_CMM_GET_VAL;

    //发送消息
    if (OM_FAILED(MSG_SendMessage(pstMsg)))
    {
        IO_Print("Send message to CMM failed");
        CLI_MSG_RELEASE(pstMsg);
        return TBS_FAILED;
    }

    //释放申请的内存
    CLI_MSG_RELEASE(pstMsg);

    //每次调用MSG_ReceiveMessage前，释放一次
    CLI_MSG_RELEASE(g_pstCliMsg);

    //接收消息
    if (OM_FAILED(MSG_ReceiveMessage(&g_pstCliMsg, CLI_COMM_TIMEOUT)))
    {
        IO_Print("Wait message failed");
        return TBS_FAILED;
    }
    if (g_pstCliMsg->stMsgHead.usMsgType != MSG_CMM_GET_VAL_ACK)
    {
        IO_Print("Received unrespected message");
        return TBS_FAILED;
    }

    //处理消息
    iRet = CLI_StoreUserInfo();
    
    return iRet;
}

/*************************************************************************
功能:处理MSG_CMM_GET_VAL_ACK消息，把从配置树获取的配置信息存储到
     g_stUserList中
参数:无
返回:
     TBS_SUCCESS:消息处理成功
     TBS_FAILED :消息处理失败
备注:使用了全局变量g_pstCliMsg,要理解本函数，请参考MSG_CMM_GET_VAL_ACK
     消息定义
 *************************************************************************/
static int CLI_StoreUserInfo(void)
{
    ST_MSG *pstMsg = g_pstCliMsg;
    ULONG  ulRet = 0;
    ULONG  ulCount = 0;
    ULONG  ulTemp  = 0;
    int    iIndex  = 0;
    char   *pcCurItem = NULL;
    char   *pcTempPos1= NULL;

    //判断GET操作是否成功
    ulRet = *(ULONG *)pstMsg->szMsgBody;
    if (ulRet != 0)
    {
        IO_Print("Get User info failed");
        return TBS_FAILED;
    }

    //判断获取的项数是不是合法，本处只判断是否为0，0则不合法
    ulCount = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    if (ulCount == 0)
    {
        IO_Print("No item getted!");
        return TBS_FAILED;
    }
    /*因为一个用户信息实例的叶节点是3项:username,password,level
     *所以ulCount必须是3的倍数
     */
    else if ((ulCount%3) != 0)
    {
        //IO_Print("Error item count!");
        //return TBS_FAILED;
    }
    
    /*如果因为用户数太多而无法全部保存，给出提示,但不用返回
     *因为每个用户对应3项，所以要乘以3
     */
    if ((ulCount > (MAX_USER_COUNT*3)))
    {
        IO_Print("Too many items getted, some user info may lost!");
    }

    //跳过Result和ResItemCount
    pcCurItem = pstMsg->szMsgBody + sizeof(ULONG)*2;

    //解析并存储用户信息
    g_stUserList.iUserCount = 0;
    for (ulTemp = 0; ulTemp < ulCount; )
    {
        iIndex = g_stUserList.iUserCount;
       
        if (strstr(pcCurItem, ".UserName"))
        {
            //存储用户名
            CLI_GET_LEAF_VALUE(pcTempPos1, pcCurItem);
            STB_StrNCpy(g_stUserList.arrayUserInfo[iIndex].szUsername, pcTempPos1, 31);
            CLI_MOVE2_NEXT_ITEM(pcCurItem);
            ulTemp++;
        }

        if (strstr(pcCurItem, ".Password"))
        {
            //存储密码
            CLI_GET_LEAF_VALUE(pcTempPos1, pcCurItem);
            STB_StrNCpy(g_stUserList.arrayUserInfo[iIndex].szPassword, pcTempPos1, 31);
            CLI_MOVE2_NEXT_ITEM(pcCurItem);
            ulTemp++;
        }

        if (strstr(pcCurItem, ".Level"))
        {
            //存储用户等级
            CLI_GET_LEAF_VALUE(pcTempPos1, pcCurItem);
            g_stUserList.arrayUserInfo[iIndex].ulLevel= strtoul(pcTempPos1, 0, 0);
            CLI_MOVE2_NEXT_ITEM(pcCurItem);
            ulTemp++;
        }

        //用户数增1
        g_stUserList.iUserCount++; 

        //判断是否超过最多允许用户数
        if (g_stUserList.iUserCount >= MAX_USER_COUNT)
        {
            break;
        }
    }

    return TBS_SUCCESS;
}
    

/*************************************************************************
功能:判断用户名和密码是否正确，如果正确，设置相应信息
参数:szUsername:用户名
     szPassword:密码
返回:TBS_FAILED:消息处理失败
     大于等于0 :该用户命令执行等级
备注:
 *************************************************************************/
ULONG CLI_Authenticate(char *szUsername, char *szPassword, ULONG *ulIndex)
{
#if 0
    int iUserCount = g_stUserList.iUserCount;
    int iTemp      = 0;

    //用户名不能为空，但密码允许空
    if (szUsername == NULL)
    {
        IO_Print("CLI_Authenticate:Error input!");
        return TBS_FAILED;
    }

    for (iTemp = 0; iTemp < iUserCount; iTemp++)
    {
        //比较用户名
        if (STB_StrCmp(szUsername, g_stUserList.arrayUserInfo[iTemp].szUsername) == 0)
        {
            //密码为空的情况
            if ((szPassword == NULL || STB_StrLen(szPassword) == 0 )
                && (STB_StrLen(g_stUserList.arrayUserInfo[iTemp].szPassword) == 0))
            {
                //验证通过
                return CLI_ConfigLever2CliLevel(g_stUserList.arrayUserInfo[iTemp].ulLevel);
            }
            //密码不为空的情况
            else if(STB_StrCmp(szPassword, crypt(g_stUserList.arrayUserInfo[iTemp].szPassword, AUTH_KEY)) == 0)
            {
                //验证通过
                return CLI_ConfigLever2CliLevel(g_stUserList.arrayUserInfo[iTemp].ulLevel);
            }
        }
    }
#else
    //发送MSG_AUTH消息，让AUTH模块认证
    
    ST_MSG *pstMsg;                 /* 发送的消息指针 */
    int    iRet = TBS_SUCCESS;
    char    aszAuthInfo[4][128];
    char    *pPos = NULL;
    USHORT usLen = 0;
    ULONG  ulResult = 1;
    ULONG  ulLevel = 0;

    //用户名不能为空，但密码允许空
    if (szUsername == NULL)
    {
        IO_Print("CLI_Authenticate:Error input!");
        return TBS_FAILED;
    }

    //后面的1表示结束符占一字节
    usLen += sprintf(aszAuthInfo[0], "username=%s", szUsername) + 1;
    usLen += sprintf(aszAuthInfo[1], "password=%s", szPassword) + 1;
    usLen += sprintf(aszAuthInfo[2], "action=%s", "login") + 1;
    //这个session id是随便写的，跟别的session冲突的概率是百万分之一
    usLen += sprintf(aszAuthInfo[3], "sessionid=%s", "11111111") + 1;
    
    /*------------分配消息体空间--------*/
    // 准确应该是 sizeof(ULONG) + usLen
    pstMsg = MSG_CreateMessage(usLen + sizeof(ULONG));
    if (pstMsg == NULL)
    {
        IO_Print("Out of Memory");
        return TBS_FAILED;
    }

    /*------------构造消息-----------*/
    /*
     *在看消息构造前，请参考MSG_AUTH的消息结构
     */
    pstMsg->stMsgHead.ulBodyLength = usLen + sizeof(ULONG);
    //4项
    *(ULONG *)pstMsg->szMsgBody = (ULONG)4;
    pPos = pstMsg->szMsgBody + sizeof(ULONG);

    //添加第一项
    STB_StrCpy(pPos, aszAuthInfo[0]);
    pPos += (STB_StrLen(aszAuthInfo[0]) + 1);

    //添加第二项
    STB_StrCpy(pPos, aszAuthInfo[1]);
    pPos += (STB_StrLen(aszAuthInfo[1]) + 1);

    //添加第三项
    STB_StrCpy(pPos, aszAuthInfo[2]);
    pPos += (STB_StrLen(aszAuthInfo[2]) + 1);

    //添加第四项
    STB_StrCpy(pPos, aszAuthInfo[3]);
    pPos += (STB_StrLen(aszAuthInfo[3]) + 1);
    
    pstMsg->stMsgHead.ulMsgID   = 0;
    pstMsg->stMsgHead.usSrcMID  = MID_CLI;
    pstMsg->stMsgHead.usDstMID  = MID_AUTH;
    pstMsg->stMsgHead.usMsgType = MSG_AUTH;

    /*-----------发送消息-----*/
    if (OM_FAILED(MSG_SendMessage(pstMsg)))
    {
        IO_Print("Send message to CMM failed");
        iRet = TBS_FAILED;
        goto end_release;
    }

    //释放申请的内存
    CLI_MSG_RELEASE(pstMsg);


    /*----------------接收消息---------------*/
    if (OM_FAILED(MSG_ReceiveMessage(&pstMsg, CLI_COMM_TIMEOUT)))
    {
        IO_Print("Wait message failed");
        return TBS_FAILED;
    }
    if (pstMsg->stMsgHead.usMsgType != MSG_AUTH_ACK)
    {
        IO_Print("Received unrespected message");
        iRet = TBS_FAILED;
        goto end_release;
    }

    /*-------------------处理消息-------------*/
    /*跳过count项，反正看过AUTH模块的实现，count接下来的
     *的是result值，再下来就是level值,在看以下解析前，请
     *参考WIKI中MSG_AUTH_ACK的构造方式
     */

    /*这个判断我是根据AUTH模块的MSG_AUTH_ACK构造而写的
     *不理解的话可以去看看MSG_AUTH_ACK是怎么构造的
     *详见apps/logic/auth/auth.c的AUTH_RespAuthMsg函数
     */
    //if (*(ULONG*)(pstMsg->szMsgBody) != 0 || *(ULONG*)(pstMsg->szMsgBody+sizeof(ULONG)) != 3)
    if (*(ULONG*)(pstMsg->szMsgBody) != 0)
    {
        iRet = TBS_FAILED;
        goto end_release;
    }
    
    pPos = pstMsg->szMsgBody + sizeof(ULONG)*2;
    sscanf(pPos, "result=%lu", &ulResult);
    if (ulResult != CLI_AUTH_SUCCESS)
    {
        iRet = ulResult;
        goto end_release;
    }

    pPos += (STB_StrLen(pPos) + 1);
    sscanf(pPos, "level=%lu", &ulLevel);
     
    *ulIndex = CLI_ConfigLever2CliLevel(ulLevel);
end_release:
    //释放消息
    CLI_MSG_RELEASE(pstMsg);
    return iRet;
#endif

    //验证失败
    return TBS_FAILED;
}


/*************************************************************************
功能:由于配置树中的用户等级跟CLI的用户等级划分不一致，所以这个函数用来
     转换
参数:ulConfigLevel:配置树中的用户等级
返回:命令的执行等级
备注:权限:CLI_AL_ADMIN > CLI_AL_OPERATOR > CLI_AL_QUERY
 *************************************************************************/
static ULONG CLI_ConfigLever2CliLevel(ULONG ulConfigLevel)
{
    switch(ulConfigLevel)
    {
    //这是配置树中的admin用户
    case 1:
        return CLI_AL_ADMIN;
        
    //这是配置树中的user用户    
    case 2:
        return CLI_AL_OPERATOR;

    //这是配置树中的guest用户    
    case 3:
        return CLI_AL_QUERY;
    default:
        break;
    }

    return CLI_AL_QUERY;
}

/*=========================================================================*/
/*  函数名称: CLI_ComStatusStart                                           */
/*  函数功能: 通讯环境状态初始化                                           */
/*  函数说明: 该函数应在命令行调用回调函数之前使用                         */
/*  创  建  : liuzequn / 2007-11-6                                         */
/*=========================================================================*/
void CLI_ComStatusStart()
{
    g_szCliFeedbackError[0] = '\0';
    CLI_MSG_RELEASE(g_pstCliMsg); 
}

void CLI_ComStatusEnd()
{
    CLI_MSG_RELEASE(g_pstCliMsg); 
}

void CLI_ComErrorAdd(PCSTR szErrorInfo)
{
    DBG_ASSERT(szErrorInfo != NULL);

    strcat(g_szCliFeedbackError, "\n  ");
    strcat(g_szCliFeedbackError, szErrorInfo);
}

ULONG CLI_ComMsgInteract(ST_MSG *pstMsg, USHORT usType)
{
    DBG_ASSERT(pstMsg != NULL);

    pstMsg->stMsgHead.ulMsgID   = 0;
    pstMsg->stMsgHead.usSrcMID  = MID_CLI;
    pstMsg->stMsgHead.usDstMID  = MID_CMM;
    pstMsg->stMsgHead.usMsgType = usType;

    if (OM_FAILED(MSG_SendMessage(pstMsg)))
    {
        CLI_ComErrorAdd("Send message to CMM failed");
        return TBS_FAILED;
    }

    //调用MSG_ReceiveMessage前要先释放
    CLI_MSG_RELEASE(g_pstCliMsg); 

    // 以下的pstMsg将指向接收到的信息
    if (OM_FAILED(MSG_ReceiveMessage(&g_pstCliMsg, CLI_COMM_TIMEOUT)))
    {
        CLI_ComErrorAdd("Wait message failed");
        return TBS_FAILED;
    }
    if (g_pstCliMsg->stMsgHead.usMsgType != (RESPONSE_FLAG |usType))
    {
        CLI_ComErrorAdd("Received unrespected message");
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

int CLI_AddStrToLink(ST_CLISTR_LINK **ppHead, char *szStr)
{
    ST_CLISTR_LINK *pObjAdd, *pObjTmp;

    DBG_ASSERT(ppHead != 0 && szStr != 0);

    pObjAdd = malloc(sizeof(ST_CLISTR_LINK));
    if (pObjAdd == NULL)
   {
       return TBS_FAILED;   //KW check, wjf
   }
    
    if (pObjAdd != NULL)
    {
        pObjAdd->pstrValue = malloc(strlen(szStr) + 1);

        //KW check,wjf
        if( pObjAdd->pstrValue == NULL)
        {
             free(pObjAdd);
             return TBS_FAILED;   
        }
        
        pObjAdd->pNext = NULL;
        strcpy(pObjAdd->pstrValue, szStr);
    }

    /*if (pObjAdd == NULL || pObjAdd->pstrValue == NULL)
        return TBS_FAILED; */

    if (*ppHead == NULL)
        *ppHead = pObjAdd;
    else
    {
        pObjTmp = *ppHead;
        while (pObjTmp->pNext != NULL) pObjTmp = pObjTmp->pNext;
        pObjTmp->pNext = pObjAdd;
    }
    return TBS_SUCCESS;
}

void CLI_DestroyStrLink(ST_CLISTR_LINK *pHead)
{

    while (pHead != 0)
    {
        ST_CLISTR_LINK *pTmp = pHead;
        pHead = pHead->pNext;
        free(pTmp->pstrValue);
        free(pTmp);
    }
}

/*=========================================================================*/
/*  函数名称: CLI_CommInit                                                 */
/*  函数功能: 命令行通信初始化                                             */
/*  输入参数:                                                              */
/*  返  回  : 成功、失败                                                   */
/*  创  建  : liuzequn / 2007-11-6                                         */
/*=========================================================================*/
ULONG CLI_CommInit()
{
    /* 消息模块的注册 */
    if(OM_FAILED(MSG_RegModule(MID_CLI, NULL)))  /* 通讯初始化失败 */
    {
        /* kill old cmd process  杀掉CMD进程，使TELNET可以重新登陆*/
		char szCmd[256] = {0};	
    	FILE *fp;            
        pid_t pid1;
        FILE *fd = NULL;

	    if((fp = fopen("/var/cmd_pid","r")) == NULL){
			printf("err on opening file");
			return 0;
		}
		fscanf(fp, "%d", &pid1); 
		fclose(fp);
        sprintf(szCmd, "ps -aux | grep cmd");

        fd = (FILE *)popen(szCmd, "r");   
        if(NULL != fd)   
        {
            fgets(szCmd, sizeof(szCmd), fd);
            pclose(fd);            
        }

		if( (strstr(szCmd, "-cmd") || strstr(szCmd, "/usr/bin/cmd")) && (pid1 > 0)){
            sprintf(szCmd, "kill -9 %d", pid1);
            tbsSystem(szCmd, 0);
    		tbsSystem("echo -1 > /var/cmd_pid", 0); 
        }        

//		sprintf(szDefaultRule,"rm -f /var/cmd_pid");
//		tbsSystem(szDefaultRule, 0);
		if (OM_FAILED(MSG_RegModule(MID_CLI, NULL)))
		{
            DBG_Out("Module register error. Exiting.\n");
            return TBS_FAILED;
		}
    }
    MSG_AllModStartOK();

    return TBS_SUCCESS;
}

// 单个叶子节点信息的获取
ULONG CLI_GetLeaf(char *szLeafName, char *szLeafVal)
{
    ST_MSG *pstMsg;      /* 发送的消息指针 */
    USHORT  usLen = strlen(szLeafName);
    int     nRet;
    char    *pValPos = NULL;

    DBG_ASSERT(szLeafName != NULL);
    pstMsg = MSG_CreateMessage(usLen + 8);  // 准确应该是 sizeof(ULONG) + usLen + '\0'
    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    }

    pstMsg->stMsgHead.ulBodyLength = strlen(szLeafName) + 1 + sizeof(ULONG);
    *(ULONG *)pstMsg->szMsgBody = 1;
    strcpy(pstMsg->szMsgBody+ sizeof(ULONG), szLeafName);

    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_GET_VAL)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //在离开前，先释放已经申请的内存
        CLI_MSG_RELEASE(pstMsg); 

        return TBS_FAILED;
    }

    //在重新给指针赋值前，先把之前指向的内存释放掉
    CLI_MSG_RELEASE(pstMsg);

    pstMsg = g_pstCliMsg;
    nRet = (int)*(ULONG *)pstMsg->szMsgBody;
    if (nRet != 0)
    {
        CLI_ComErrorAdd("Get leaf info failed");
        return TBS_FAILED;
    }

    pValPos = strchr(pstMsg->szMsgBody + sizeof(ULONG) * 2, '=');

    if (pValPos == NULL)
        return TBS_FAILED;

    if (szLeafVal != NULL)
        strcpy(szLeafVal, pValPos + 1);
    else
        strcpy(szLeafName, pValPos + 1);
    return TBS_SUCCESS;

}

/*************************************************************************
功能: 发送MSG_CMM_GET_VAL并返回GET的结果
参数: pszNodePath :所要获取的节点的路径，
                   可以是非叶节点，也可以是
                   叶节点
返回: NULL        :GET失败
      非NULL值    :GET成功，返回的是接收到的
                   消息的指针
备注: 由于要求各异，返回指针以适应各中处理要求
*************************************************************************/
ST_MSG *CLI_GetNode(char *pszNodePath)
{
    ST_MSG *pstMsg;      /* 发送的消息指针 */
    USHORT  usLen = 0;
    
    DBG_ASSERT(pszNodePath != NULL);
    usLen = strlen(pszNodePath);

    //创建消息
    pstMsg = MSG_CreateMessage(usLen + 8);  // 准确应该是 sizeof(ULONG) + usLen + '\0'
    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return NULL;
    }

    //初始消息的一些变量
    pstMsg->stMsgHead.ulBodyLength = strlen(pszNodePath) + 1 + sizeof(ULONG);
    *(ULONG *)pstMsg->szMsgBody = 1;
    strcpy(pstMsg->szMsgBody+ sizeof(ULONG), pszNodePath);

    //发送与接收
    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_GET_VAL)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //在离开前，先释放已经申请的内存
        CLI_MSG_RELEASE(pstMsg); 

        return NULL;
    }

    //在重新给指针赋值前，先把之前指向的内存释放掉
    CLI_MSG_RELEASE(pstMsg);

    pstMsg = g_pstCliMsg;

    //判断消息类型是否正确
    if (pstMsg->stMsgHead.usMsgType != MSG_CMM_GET_VAL_ACK)
    {
        CLI_ComErrorAdd("Error Msg Type\n");
        return NULL;
    }

    //消息接收成功，返回消息指针
    return pstMsg;

}

// 对象下一层节点名称的获取
int CLI_GetNextLevelName(char *szPathName, char aszNextLevel[CLI_MAX_OBJ_NUM][CLI_LEAF__LEN])
{
    int     nLine, i;     // 获取内容行数
    char   *pGetMsgBody, *pPosCur;
    ST_MSG* pstMsg = MSG_CreateMessage(256);    /* 发送POST 信息*/

    if (szPathName == NULL)
    {
        CLI_ComErrorAdd("Parameter error");
        return TBS_FAILED;
    }


    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    }

    pstMsg->szMsgBody[0] = 1;       // 该标记表示只取下一层
    strcpy(pstMsg->szMsgBody + 1, szPathName);
    pstMsg->stMsgHead.ulBodyLength = strlen(pstMsg->szMsgBody) + 1;     //  这里把第0个字节也算进去了

    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_GET_NAME)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //在离开前，先释放已经申请的内存
        CLI_MSG_RELEASE(pstMsg); 
        return -1;
    }

    CLI_MSG_RELEASE(pstMsg);

    // 将相应得到的索引加入索引列表
    pGetMsgBody = pPosCur = g_pstCliMsg->szMsgBody;
    if (*(unsigned long*)pPosCur != 0)   // ulResult !=0 表示获取失败
    {
        CLI_ComErrorAdd("Get name failed");
        return -1;
    }
    pPosCur += sizeof(unsigned long);
    nLine = *(unsigned long*)pPosCur;   // count Num
    pPosCur += sizeof(unsigned long);

    // 如果没有内容， 不需要做任何事情， 也不算失败
    if (nLine == 0)
        return 0;

    // 将回应的消息中的叶子提取出来保存到aszNextLevel;
    for (i = 0; i < nLine; i++)
    {
        char *pEqlPos;
        pPosCur = strchrR(pPosCur, '.'); // 获取叶子名称的位置
        if (pPosCur == NULL || pPosCur + 1 >= pGetMsgBody + g_pstCliMsg->stMsgHead.ulBodyLength)
        {
            CLI_ComErrorAdd("Get name message format error");
            return -1;
        }
        ++pPosCur;
        pEqlPos = strchr(pPosCur, ' ');
        if (pEqlPos == NULL)    // 不是name attibute的格式， 错误
        {
            CLI_ComErrorAdd("Get name conntent format error");
            return -1;
        }
        *pEqlPos = '\0';    // 本模块只关心‘=’之前的内容
        strcpy(aszNextLevel[i], pPosCur);
        *pEqlPos = ' ';     // 恢复‘ ’
        pPosCur += strlen(pPosCur) + 1;
    }
    return nLine;

}

// 对象节点信息的获取
// 注意本函数返回的字符串链表需要调用者删除
ST_CLISTR_LINK *CLI_GetObject(char *szPathName, int nValNum, char aszLeafName[][CLI_LEAF__LEN])
{
    int   nRow, i;     // 获取内容行数
    ST_MSG* pstMsg = NULL;    /* 发送信息*/
    char   *pGetMsgBody, *pPosCur;
    ST_CLISTR_LINK *pStrLink = NULL;

    if (szPathName == NULL && aszLeafName == NULL)
    {
        CLI_ComErrorAdd("GetObject Parameter error");
        return NULL;
    }

    pstMsg = MSG_CreateMessage(1024);
    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return NULL;
    }

    pPosCur = pstMsg->szMsgBody;

    // 个数填充
    *(unsigned long *)pPosCur = nValNum;
    pPosCur += sizeof(unsigned long);

    // 逐个填充获取内容
    for (i = 0; i < nValNum; i++)
    {
        pPosCur += sprintf(pPosCur, "%s%s", szPathName, aszLeafName[i]) + 1;
    }
    pstMsg->stMsgHead.ulBodyLength = pPosCur - pstMsg->szMsgBody;

    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_GET_VAL)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //离开前先释放内存
        CLI_MSG_RELEASE(pstMsg);
        return NULL;
    }

    CLI_MSG_RELEASE(pstMsg);

    // 将相应得到的索引加入索引列表
    pGetMsgBody = pPosCur = g_pstCliMsg->szMsgBody;
    if (*(unsigned long *)pPosCur != 0)   // ulResult !=0 表示获取失败
    {
        CLI_ComErrorAdd("Get value list failed");
        return NULL;
    }
    pPosCur += sizeof(unsigned long);

    nRow = *(unsigned long *)pPosCur;   // count Num
    pPosCur += sizeof(unsigned long);

    // 如果没有内容， 不需要做任何事情， 也不算失败
    if (nRow != nValNum)
    {
        CLI_ComErrorAdd("Get Value num is unrespected");
        return NULL;
    }

    // 将回应的消息中的值提取出来保存到szValRet;
    for (i = 0; i < nValNum; i++)
    {
        pPosCur = strchr(pPosCur, '=');
        if(pPosCur == NULL) return NULL; //KW check, wjf
        if (*pPosCur == '\0' || pPosCur >= pGetMsgBody + g_pstCliMsg->stMsgHead.ulBodyLength)
        {
            CLI_ComErrorAdd("Get too less value!");
            CLI_DestroyStrLink(pStrLink);
            return NULL;
        }
        CLI_AddStrToLink(&pStrLink, pPosCur + 1);
        pPosCur += strlen(pPosCur) + 1;
    }

    return pStrLink;
}

// 叶子节点信息的设置
int CLI_SetLeaf(int nValNum, char aszName[][CLI_NODE__LEN], char aszVal[][CLI_NODE__LEN])
{
    ST_MSG         *pstMsg;
    int i;
    USHORT          usSetLen = sizeof(ULONG);
    char           szFaultCode[10];

    if (aszName == NULL || aszVal == NULL || nValNum == 0)
        return TBS_FAILED;

    pstMsg = MSG_CreateMessage(nValNum * 2 * CLI_NODE__LEN);
    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    }

    for (i = 0; i < nValNum; i++)
    {
        usSetLen += sprintf(pstMsg->szMsgBody + usSetLen, "%s=%s",
                            aszName[i], aszVal[i]) + 1;
    }
    pstMsg->stMsgHead.ulBodyLength = usSetLen;
    *(unsigned long *)pstMsg->szMsgBody = nValNum;

    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_SET_VAL)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //离开前先释放内存
        CLI_MSG_RELEASE(pstMsg);
        return TBS_FAILED;
    }

    CLI_MSG_RELEASE(pstMsg);

    pstMsg = g_pstCliMsg;

    // skip result+status+errcount

    if (*(unsigned long*)pstMsg->szMsgBody != 0)
    {
    	snprintf(szFaultCode,sizeof(szFaultCode)-1,"%ld",*(unsigned long*)pstMsg->szMsgBody);
        CLI_ComErrorAdd(szFaultCode);
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

// 对象节点信息的添加
int CLI_AddObject(char *szPathName)
{
#if 0
    ST_MSG         *pstMsg = MSG_CreateMessage(256);

    if (szPathName == NULL)
        return TBS_FAILED;

    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    }

    pstMsg->stMsgHead.ulBodyLength = sprintf(pstMsg->szMsgBody, "%s", szPathName) + 1;

    /* 发送命令消息 */
    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_ADD_NODE)))
    {
        CLI_ComErrorAdd("Communcate failed");
        return TBS_FAILED;
    }

    pstMsg = g_pstCliMsg;

    // 如果添加不成功， 提供错误码
    if (*(unsigned long*)pstMsg->szMsgBody != 0)    // Result
    {
        CLI_ComErrorAdd("Add object failed");
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
#else
    return CLI_AddObjectEx(szPathName, NULL);
#endif
    
}


/*************************************************************************
功能: 添加一个实例
参数: szPathName :输入参数，所要添加的实例的父路径
      iInstIndex :输出参数，添加实例成功后的实例号
返回: TBS_FAILED :添加实例失败
      TBS_SUCCESS:添加实例成功
备注:   
*************************************************************************/
int CLI_AddObjectEx(char *szPathName, ULONG *pulInstIndex)
{
    ST_MSG         *pstMsg = NULL;

    if (szPathName == NULL)
        return TBS_FAILED;

    pstMsg = MSG_CreateMessage(256);
    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    }

    pstMsg->stMsgHead.ulBodyLength = sprintf(pstMsg->szMsgBody, "%s", szPathName) + 1;

    /* 发送命令消息 */
    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_ADD_NODE)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //离开前先释放内存
        CLI_MSG_RELEASE(pstMsg);
        return TBS_FAILED;
    }

    CLI_MSG_RELEASE(pstMsg);
    pstMsg = g_pstCliMsg;

    //判断消息类型是否正确
    if (pstMsg->stMsgHead.usMsgType != MSG_CMM_ADD_NODE_ACK)
    {
        CLI_ComErrorAdd("Unexpected message type recieved");
        return TBS_FAILED;
    }

    // 如果添加不成功， 提供错误码
    if (*(unsigned long*)pstMsg->szMsgBody != 0)    // Result
    {
        CLI_ComErrorAdd("Add object failed");
        return TBS_FAILED;
    }

    //获取实例号
    if (pulInstIndex != NULL)
    {
        *pulInstIndex = *(ULONG *)(pstMsg->szMsgBody + sizeof(ULONG));
    }

    return TBS_SUCCESS;
}

// 对象节点信息的删除
int CLI_DelObject(char *szPathName)
{
    ST_MSG         *pstMsg = NULL;

    if (szPathName == NULL)
        return TBS_FAILED;

    pstMsg = MSG_CreateMessage(256);
    if (pstMsg == NULL)
    {
        CLI_ComErrorAdd("Out of Memory");
        return TBS_FAILED;
    }

    pstMsg->stMsgHead.ulBodyLength = sprintf(pstMsg->szMsgBody, "%s", szPathName) + 1;

    /* 发送命令消息 */
    if (OM_FAILED(CLI_ComMsgInteract(pstMsg, MSG_CMM_DEL_NODE)))
    {
        CLI_ComErrorAdd("Communcate failed");
        //离开前先释放内存
        CLI_MSG_RELEASE(pstMsg);
        return TBS_FAILED;
    }

    pstMsg = g_pstCliMsg;

    // 如果添加不成功， 提供错误码
    if (*(unsigned long*)pstMsg->szMsgBody != 0)    // Result
    {
        CLI_ComErrorAdd("Del object failed");
        return TBS_FAILED;
    }

    return TBS_SUCCESS;
}

void CLI_CommErrorPrint()
{
   //lan config 提示于dhcp 池的冲突
	if(!strcmp(g_szCliFeedbackError,"\n  15762308"))
		{
		   IO_Print("\n Interface address and dhcp address pool not match");	
		   return;
		}
	else if(!strcmp(g_szCliFeedbackError,"\n  15749201"))
		{
		   IO_Print("\n CPE IP conflicts with DHCP pool IP addr\n"); 
		   return;
		}

    IO_Print(g_szCliFeedbackError);
}
/******************************************************************************
 *                                 END                                        *
 ******************************************************************************/



