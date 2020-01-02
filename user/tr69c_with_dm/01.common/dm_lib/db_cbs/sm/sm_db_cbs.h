

#ifndef DM_SM_REGISTER_DB_CBS_HANDLER_H
#define DM_SM_REGISTER_DB_CBS_HANDLER_H


/* declaration of reigster functions */
extern BOOL8 SmInternalNotifyRegisterHandler(void);
extern BOOL8 SmDeviceNotifyRegisterHandler(void);
extern BOOL8 SmDeviceGetRegisterHandler(void);


/* inline fuction */
inline BOOL8 SmRegisterDbCbsHandler(void)
{
    /* call reigster functions here*/
    if (DM_FALSE==SmInternalNotifyRegisterHandler()) { return DM_FALSE; }
    if (DM_FALSE==SmDeviceNotifyRegisterHandler()) { return DM_FALSE; }
    if (DM_FALSE==SmDeviceGetRegisterHandler()) { return DM_FALSE; }

    return DM_TRUE;
}


#endif /* DM_SM_REGISTER_DB_CBS_HANDLER_H */
