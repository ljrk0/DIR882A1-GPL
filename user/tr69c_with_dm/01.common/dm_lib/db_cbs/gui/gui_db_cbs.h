

#ifndef DM_GUI_REGISTER_DB_CBS_HANDLER_H
#define DM_GUI_REGISTER_DB_CBS_HANDLER_H


/* declaration of reigster functions */
extern BOOL8 GuiInternalNotifyRegisterHandler(void);
extern BOOL8 GuiDeviceNotifyRegisterHandler(void);


/* inline fuction */
inline BOOL8 GuiRegisterDbCbsHandler(void)
{
    /* call reigster functions here*/
    if (DM_FALSE==GuiInternalNotifyRegisterHandler()) { return DM_FALSE; }
    if (DM_FALSE==GuiDeviceNotifyRegisterHandler()) { return DM_FALSE; }

    return DM_TRUE;
}


#endif /* DM_GUI_REGISTER_DB_CBS_HANDLER_H */
