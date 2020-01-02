

#ifndef DM_DMM_REGISTER_DB_CBS_HANDLER_H
#define DM_DMM_REGISTER_DB_CBS_HANDLER_H


/* declaration of reigster functions */
extern BOOL8 DmmDeviceNotifyRegisterHandler(void);
extern BOOL8 DmmDeviceGetRegisterHandler(void);


/* inline fuction */
inline BOOL8 DmmRegisterDbCbsHandler(void)
{
    /* call reigster functions here*/
    if (DM_FALSE==DmmDeviceNotifyRegisterHandler()) { return DM_FALSE; }
    if (DM_FALSE==DmmDeviceGetRegisterHandler()) { return DM_FALSE; }

    return DM_TRUE;
}


#endif /* DM_DMM_REGISTER_DB_CBS_HANDLER_H */
