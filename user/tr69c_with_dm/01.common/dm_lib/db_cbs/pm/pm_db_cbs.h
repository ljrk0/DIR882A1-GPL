

#ifndef DM_PM_REGISTER_DB_CBS_HANDLER_H
#define DM_PM_REGISTER_DB_CBS_HANDLER_H


/* declaration of reigster functions */
extern BOOL8 PmDeviceNotifyRegisterHandler(void);
extern BOOL8 PmDeviceGetRegisterHandler(void);


/* inline fuction */
inline BOOL8 PmRegisterDbCbsHandler(void)
{
    /* call reigster functions here*/
    if (DM_FALSE==PmDeviceNotifyRegisterHandler()) { return DM_FALSE; }
    if (DM_FALSE==PmDeviceGetRegisterHandler()) { return DM_FALSE; }

    return DM_TRUE;
}


#endif /* DM_PM_REGISTER_DB_CBS_HANDLER_H */
