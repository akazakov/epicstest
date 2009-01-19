# Makefile <top>
TOP = .
include $(TOP)/configure/CONFIG_APP
DIRS += configure
DIRS += common
DIRS += accessApp
DIRS += alarmApp
DIRS += caApp
DIRS += convertApp
DIRS += dbcaPerformApp
DIRS += demoApp
DIRS += miscApp
DIRS += performApp
DIRS += putApp
DIRS += linkinfoApp
DIRS += softcallbackApp
DIRS += iocBoot
include $(TOP)/configure/RULES_TOP
