

#ifndef DM_TM_REGISTER_DB_CBS_HANDLER_H
#define DM_TM_REGISTER_DB_CBS_HANDLER_H


/* declaration of reigster functions */
extern BOOL8 TmDeviceNotifyRegisterHandler(void);
extern BOOL8 TmDeviceGetRegisterHandler(void);


/* inline fuction */
inline BOOL8 TmRegisterDbCbsHandler(void)
{
    /* call reigster functions here*/
    if (DM_FALSE==TmDeviceNotifyRegisterHandler()) { return DM_FALSE; }
    if (DM_FALSE==TmDeviceGetRegisterHandler()) { return DM_FALSE; }

    return DM_TRUE;
}


#endif /* DM_TM_REGISTER_DB_CBS_HANDLER_H */
