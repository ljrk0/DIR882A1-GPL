

#ifndef DM_PS_REGISTER_DB_CBS_HANDLER_H
#define DM_PS_REGISTER_DB_CBS_HANDLER_H


/* declaration of reigster functions */
extern BOOL8 PsSon_dmNotifyRegisterHandler(void);
extern BOOL8 PsDeviceNotifyRegisterHandler(void);
extern BOOL8 PsDeviceGetRegisterHandler(void);


/* inline fuction */
inline BOOL8 PsRegisterDbCbsHandler(void)
{
    /* call reigster functions here*/
    if (DM_FALSE==PsSon_dmNotifyRegisterHandler()) { return DM_FALSE; }
    if (DM_FALSE==PsDeviceNotifyRegisterHandler()) { return DM_FALSE; }
    if (DM_FALSE==PsDeviceGetRegisterHandler()) { return DM_FALSE; }

    return DM_TRUE;
}


#endif /* DM_PS_REGISTER_DB_CBS_HANDLER_H */
