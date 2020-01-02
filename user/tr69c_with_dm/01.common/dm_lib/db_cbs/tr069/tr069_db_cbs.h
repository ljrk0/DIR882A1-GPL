

#ifndef DM_TR069_REGISTER_DB_CBS_HANDLER_H
#define DM_TR069_REGISTER_DB_CBS_HANDLER_H


/* declaration of reigster functions */
extern BOOL8 Tr069DeviceNotifyRegisterHandler(void);
extern BOOL8 Tr069DeviceGetRegisterHandler(void);


/* inline fuction */
inline BOOL8 Tr069RegisterDbCbsHandler(void)
{
    /* call reigster functions here*/
    if (DM_FALSE==Tr069DeviceNotifyRegisterHandler()) { return DM_FALSE; }
    if (DM_FALSE==Tr069DeviceGetRegisterHandler()) { return DM_FALSE; }

    return DM_TRUE;
}


#endif /* DM_TR069_REGISTER_DB_CBS_HANDLER_H */
