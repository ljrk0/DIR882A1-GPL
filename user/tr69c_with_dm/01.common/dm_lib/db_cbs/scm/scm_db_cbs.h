

#ifndef DM_SCM_REGISTER_DB_CBS_HANDLER_H
#define DM_SCM_REGISTER_DB_CBS_HANDLER_H


/* declaration of reigster functions */
extern BOOL8 ScmDeviceNotifyRegisterHandler(void);
extern BOOL8 ScmDeviceGetRegisterHandler(void);


/* inline fuction */
inline BOOL8 ScmRegisterDbCbsHandler(void)
{
    /* call reigster functions here*/
    if (DM_FALSE==ScmDeviceNotifyRegisterHandler()) { return DM_FALSE; }
    if (DM_FALSE==ScmDeviceGetRegisterHandler()) { return DM_FALSE; }

    return DM_TRUE;
}


#endif /* DM_SCM_REGISTER_DB_CBS_HANDLER_H */
