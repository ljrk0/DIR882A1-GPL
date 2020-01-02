

#ifndef DM_FM_REGISTER_DB_CBS_HANDLER_H
#define DM_FM_REGISTER_DB_CBS_HANDLER_H


/* declaration of reigster functions */
extern BOOL8 FmDeviceNotifyRegisterHandler(void);


/* inline fuction */
inline BOOL8 FmRegisterDbCbsHandler(void)
{
    /* call reigster functions here*/
    if (DM_FALSE==FmDeviceNotifyRegisterHandler()) { return DM_FALSE; }

    return DM_TRUE;
}


#endif /* DM_FM_REGISTER_DB_CBS_HANDLER_H */
