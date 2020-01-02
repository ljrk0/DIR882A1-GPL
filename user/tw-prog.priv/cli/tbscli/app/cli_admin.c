#include "../cli_pub.h"
#include "../cli/cli_term.h"
#include "../cli/cli_io.h"
#include "../cli/cli_interpret.h"
#include "cli_cmd.inc"
#include "tbsutil.h"

#include "../cli_comm.h"
#include "common.h"



#define CLI_ADMINCONFIGURATION_PATH     "InternetGatewayDevice.X_TWSZ-COM_Authentication.UserList."


#define CLI_GET_PARAMETER_ASSERT(pPointer, name) do \
{ \
    (pPointer) = CLI_GetParamByName((name)); \
    if ((pPointer) == NULL) \
    { \
        return TBS_FAILED; \
    } \
}while(0);

//This macro is used to get the unsiged long port number
#define CLI_GET_ULONG_ASSERT(ulValue, name) do \
{ \
    if (CLI_GetUlongByName(name, &ulValue) != TBS_SUCCESS) \
    { \
        return TBS_FAILED; \
    } \
}while(0);



ULONG CLI_Admin_Change();
ULONG CLI_Admin_Show();


ULONG CLI_Admin()
{
    if (CLI_GetParamByName("change") != NULL)
    {
        return CLI_Admin_Change();
    }
   
    else if (CLI_GetParamByName("show") != NULL)
    {
        return CLI_Admin_Show();
    }
    
    return TBS_SUCCESS;
}



ULONG CLI_Admin_Change()
{
    char *pszUserLevel        = NULL;
    //char *pszOldUserName      = NULL;
    char *pszOldPassWord       = NULL;
    char *pszNewUserName      = NULL;
    char *pszNewPassWord      = NULL;
    ULONG userlevel = 0;
    char aszPath[2][CLI_NODE__LEN];
    char aszValue[2][CLI_NODE__LEN];
    //char passwordinleaf[CLI_LEAF__LEN];
    //char usernameinleaf[CLI_LEAF__LEN];
    //char szConfirmPassword[64]={0};
     //char crypassword[64]={0};

    CLI_GET_PARAMETER_ASSERT(pszUserLevel, "userlevel");
    
    //CLI_GET_PARAMETER_ASSERT(pszOldUserName, "ousername");
    
    CLI_GET_PARAMETER_ASSERT(pszOldPassWord, "opassword");

    CLI_GET_PARAMETER_ASSERT(pszNewUserName, "nusername");

    CLI_GET_PARAMETER_ASSERT(pszNewPassWord, "npassword");

    if(!strcmp(pszUserLevel, "admin"))
    {
    userlevel=1;
    }
    else if(!strcmp(pszUserLevel, "user"))
    {
    userlevel=2;
    }
    else if(!strcmp(pszUserLevel, "support"))
    {
    userlevel=3;
    }
    else
    {
    IO_Print("Userlevel is wrong.\n");
    }
    sprintf(aszPath[0], "%s%lu.%s", CLI_ADMINCONFIGURATION_PATH, userlevel,"UserName");
	sprintf(aszPath[1], "%s%lu.%s", CLI_ADMINCONFIGURATION_PATH, userlevel,"Password");

    /*if (TBS_SUCCESS!=CLI_GetLeaf(aszPath[1],passwordinleaf))
	{
	     CLI_CommErrorPrint();
	     return TBS_FAILED;
	}
    if (TBS_SUCCESS!=CLI_GetLeaf(aszPath[0],usernameinleaf))
	{
	     CLI_CommErrorPrint();
	     return TBS_FAILED;
	}*/
    /*strcpy(szConfirmPassword,crypt(pszOldPassWord,AUTH_KEY));
    strcpy(crypassword,crypt(pszNewPassWord,AUTH_KEY));
    if(0 != strcmp(pszOldUserName,passwordinleaf)||0 != strcmp(usernameinleaf,pszOldUserName))
    {
	  IO_Print("username or password is wrong.\n");
	   return TBS_FAILED;
    }*/
	strcpy(aszValue[0], pszNewUserName); 
    sprintf(aszValue[1], "%s:%s",pszOldPassWord,pszNewPassWord);

	
    if (CLI_SetLeaf(2, aszPath, aszValue)  != TBS_SUCCESS)
        {
            CLI_CommErrorPrint();
            return TBS_FAILED;
        }
        
        return TBS_SUCCESS;


}



ULONG CLI_Admin_Show()
{

return 0;


}



