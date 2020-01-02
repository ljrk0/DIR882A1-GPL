#
# Makefile for the Linux Wireless network device drivers.
#

obj-$(CONFIG_IPW2100) += ipw2x00/
obj-$(CONFIG_IPW2200) += ipw2x00/

obj-$(CONFIG_HERMES)		+= orinoco/

obj-$(CONFIG_AIRO)		+= airo.o
obj-$(CONFIG_AIRO_CS)		+= airo_cs.o airo.o

obj-$(CONFIG_ATMEL)             += atmel.o
obj-$(CONFIG_PCI_ATMEL)         += atmel_pci.o 
obj-$(CONFIG_PCMCIA_ATMEL)      += atmel_cs.o

obj-$(CONFIG_AT76C50X_USB)      += at76c50x-usb.o

obj-$(CONFIG_PRISM54)		+= prism54/

obj-$(CONFIG_HOSTAP)		+= hostap/
obj-$(CONFIG_B43)		+= b43/
obj-$(CONFIG_B43LEGACY)		+= b43legacy/
obj-$(CONFIG_ZD1211RW)		+= zd1211rw/
obj-$(CONFIG_RTL8180)		+= rtl818x/
obj-$(CONFIG_RTL8187)		+= rtl818x/
obj-$(CONFIG_RTLWIFI)		+= rtlwifi/

# 16-bit wireless PCMCIA client drivers
obj-$(CONFIG_PCMCIA_RAYCS)	+= ray_cs.o
obj-$(CONFIG_PCMCIA_WL3501)	+= wl3501_cs.o

obj-$(CONFIG_USB_NET_RNDIS_WLAN)	+= rndis_wlan.o

obj-$(CONFIG_USB_ZD1201)	+= zd1201.o
obj-$(CONFIG_LIBERTAS)		+= libertas/

obj-$(CONFIG_LIBERTAS_THINFIRM)	+= libertas_tf/

obj-$(CONFIG_ADM8211)	+= adm8211.o

obj-$(CONFIG_MWL8K)	+= mwl8k.o

obj-$(CONFIG_IWLWIFI)	+= iwlwifi/
obj-$(CONFIG_IWLEGACY)	+= iwlegacy/
obj-$(CONFIG_RT2X00)	+= rt2x00/

obj-$(CONFIG_P54_COMMON)	+= p54/

obj-$(CONFIG_ATH_CARDS)		+= ath/

obj-$(CONFIG_MAC80211_HWSIM)	+= mac80211_hwsim.o

obj-$(CONFIG_WL_TI)	+= ti/

obj-$(CONFIG_MWIFIEX)	+= mwifiex/

obj-$(CONFIG_BRCMFMAC)	+= brcm80211/
obj-$(CONFIG_BRCMSMAC)	+= brcm80211/

ifneq ($(CONFIG_MT_DRIVER_4411),)
ifneq ($(CONFIG_WIFI_PKT_FWD),)
ifneq ($(CONFIG_WIFI_PKT_FWD_V1),)
obj-y                   += wifi_forward/wifi_hook_v1/
else
obj-y                   += wifi_forward/wifi_hook/
endif
endif
else ifneq ($(CONFIG_MT_DRIVER_4421),)
ifneq ($(CONFIG_WIFI_PKT_FWD),)
ifneq ($(CONFIG_WIFI_PKT_FWD_V1),)
obj-y                   += wifi_forward_4421/wifi_hook_v1/
else
obj-y                   += wifi_forward_4421/wifi_hook/
endif
endif
endif

ifneq ($(CONFIG_MT_DRIVER_4411),)
ifneq ($(CONFIG_WIFI_PKT_FWD_V1),)
obj-$(CONFIG_WIFI_PKT_FWD)       += wifi_forward/wifi_fwd_v1/
else
obj-$(CONFIG_WIFI_PKT_FWD)       += wifi_forward/wifi_fwd/
endif
else ifneq ($(CONFIG_MT_DRIVER_4421),)
ifneq ($(CONFIG_WIFI_PKT_FWD_V1),)
obj-$(CONFIG_WIFI_PKT_FWD)       += wifi_forward_4421/wifi_fwd_v1/
else
obj-$(CONFIG_WIFI_PKT_FWD)       += wifi_forward_4421/wifi_fwd/
endif
endif

ifneq ($(CONFIG_RLT_AP_SUPPORT),)
obj-$(CONFIG_RLT_AP_SUPPORT)       += rlt_wifi_ap/
endif
ifneq ($(CONFIG_RLT_STA_SUPPORT),)
obj-$(CONFIG_RLT_STA_SUPPORT)      += rlt_wifi_sta/
endif

ifneq ($(CONFIG_MT_AP_SUPPORT),)
ifneq ($(CONFIG_MT_DRIVER_4411),)
ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A2")
obj-$(CONFIG_MT_AP_SUPPORT)       += MT7615D/mt_wifi_ap/
else ifeq ("$(CONFIG_PRODUCT)", "DIR_853_A1")
obj-$(CONFIG_MT_AP_SUPPORT)       += MT7615D/mt_wifi_ap/
else
obj-$(CONFIG_MT_AP_SUPPORT)       += MTK7615/mt_wifi_ap/
endif
else ifneq ($(CONFIG_MT_DRIVER_4421),)
obj-$(CONFIG_MT_AP_SUPPORT)       += MT7615_LinuxAP_V4.4.2.1_20170829/mt_wifi_ap/
endif
endif

ifneq ($(CONFIG_MT_STA_SUPPORT),)
obj-$(CONFIG_MT_STA_SUPPORT)      += mt_wifi_sta/
endif
#ifneq ($(CONFIG_WIFI_PKT_FWD),)
#obj-y                         += wifi_forward/wifi_hook/
#endif
#obj-$(CONFIG_WIFI_PKT_FWD)       += wifi_forward/wifi_fwd/
