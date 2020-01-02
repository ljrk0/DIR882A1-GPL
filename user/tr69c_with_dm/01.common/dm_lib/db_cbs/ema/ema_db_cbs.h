

#ifndef DM_EMA_REGISTER_DB_CBS_HANDLER_H
#define DM_EMA_REGISTER_DB_CBS_HANDLER_H


/* declaration of reigster functions */
extern BOOL8 EmaDeviceNotifyRegisterHandler(void);


/* inline fuction */
inline BOOL8 EmaRegisterDbCbsHandler(void)
{
    /* call reigster functions here*/
    if (DM_FALSE==EmaDeviceNotifyRegisterHandler()) { return DM_FALSE; }

    return DM_TRUE;
}


#endif /* DM_EMA_REGISTER_DB_CBS_HANDLER_H */
