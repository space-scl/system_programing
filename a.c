#
#  Copyright (C) 2023-2025 Advanced Micro Devices, Inc. All rights reserved.
#

### Variables that come from TopLevel makefile or cmd line:
#		- SOC
#		- FWDIR
#		- DDK_DIR
#		- PSP_TOOLS_DIR
#		- SOC_BASE_DIR
#		- MEM_TECH
#   - BINARY_THRESHOLD_SIZE

### If an usigned binary is desired, the make command has to be invoked with
#   NOSIGN=1
#   make NOSIGN=1

SHELL = /bin/bash

#is this neccesary? We are not using the signing of dr_generic.mk
#keeping it for now
SIGNING_TA = Sign AMD Trusted Application

UESID_CFG_FILE_PATH := $(FWDIR_SOC)/Tools/Config/$(SOC)
UESID_CFG_BASE_INI  := $(UESID_CFG_FILE_PATH)/UesidConfig_base.ini
UESID_META_INF_FILE := $(UESID_CFG_FILE_PATH)/Notes/META-INF.JSON
SIGN_CONFIGURE_PATH := $(FWDIR)/Build/$(SOC)/$(MEM_TECH)/Config
UESID_CFG_GENERATOR := $(FWDIR_SOC)/Tools/Release/Scripts/generate_cfg_file.py

#Here need to check to see which ini files to use?
# nosign, sign, encrypt, multiple can be specified/built?
UESID_CFG_FILE_BASE = UesidConfig

### NOSIGN must be defined when calling make (CLI var passed to make)
ifeq ($(NOSIGN),1)
  UESID_CFG_FILE_SIGN_SUFFIX = _nosign
else
  UESID_CFG_FILE_SIGN_SUFFIX = _sign
endif

### ENCRYPT must be defined when calling make (CLI var passed to make)
ifeq ($(ENCRYPT),1)
  UESID_CFG_FILE_ENCRYPT_SUFFIX = enc
endif

### Possible UESID_CFG_FILE results:
# 1) UesidConfig_nosign
# 2) UesidConfig_sign
# 3) UesidConfig_signenc
UESID_CFG_FILE = $(UESID_CFG_FILE_BASE)$(UESID_CFG_FILE_SIGN_SUFFIX)$(UESID_CFG_FILE_ENCRYPT_SUFFIX)

COMMON_CODE_DIR 	:= $(FWDIR)/Lib/Common
EXT_DEP_CODE_DIR 	:= $(FWDIR)/Lib/ExternalInterfaces
NBIO_CODE_DIR   	:= $(FWDIR)/NBIO
DF_CODE_DIR     	:= $(FWDIR)/DF/$(DF_ARCH)
CCX_CODE_DIR      	:= $(FWDIR)/CCX
MEM_SOC_CODE_DIR 	:= $(FWDIR_SOC)/Memory/Soc/$(SOC)
MEM_PHY_CODE_DIR	:= $(FWDIR)/Memory/Phy/$(PHY)/
MEM_UMC_CODE_DIR	:= $(FWDIR)/Memory/Umc/$(UMC)/
MEM_PLAT_CODE_DIR	:= $(FWDIR)/Memory/Platform/
MEM_TECH_CODE_DIR	:= $(FWDIR)/Memory/Tech/$(MEM_TECH)/
MEM_LIB_CODE_DIR	:= $(FWDIR)/Memory/Lib/
FCH_CODE_DIR    	:= $(FWDIR)/FCH
ESID_PATHS 				:= $(dir $(wildcard $(SOC_BASE_DIR)/Esid/esid*/))
DRV_ESID_API_DIR			:= $(FWDIR)/Drivers/DrvEsid_api


### Specific includes. This needs to expand to a full path because it will be
#   passed to the submake processes
INCLUDES_SOC := -I$(SOC_BASE_DIR)/inc
INCLUDES_SOC += -I$(SOC_BASE_DIR)/inc/$(MEM_TECH)
INCLUDES_SOC += -I$(SOC_BASE_DIR)/inc/ictfw/gml/inc
INCLUDES_SOC += -I$(SOC_BASE_DIR)/inc/ictfw/pal/common/inc
INCLUDES_SOC += -I$(FWDIR)/APCB-APOB/inc/$(SOC)
INCLUDES_SOC += -I$(FWDIR)/APCB-APOB/inc/$(SOC)/$(MEM_TECH)
INCLUDES_SOC += -I$(MEM_PHY_CODE_DIR)/$(MEM_TECH)/inc
INCLUDES_SOC += -I$(MEM_PHY_CODE_DIR)/inc
export INCLUDES_SOC

#### LIBRARY DEPENDENCIES #####
# Add here the dependency targets for future libraries
LIB_DEPENDENCIES := LibCommon
LIB_DEPENDENCIES += LibExternalDep
LIB_DEPENDENCIES += LibNbio
LIB_DEPENDENCIES += LibDF
LIB_DEPENDENCIES += LibCCX
LIB_DEPENDENCIES += LibMemSoc
LIB_DEPENDENCIES += LibMemPhy
LIB_DEPENDENCIES += LibMemUmc
LIB_DEPENDENCIES += LibMemPlat
LIB_DEPENDENCIES += LibMemTech
LIB_DEPENDENCIES += LibMemLib
LIB_DEPENDENCIES += LibFch

#### CLEAN DEPENDENCIES #####
# Add here the dependency targets for future libraries
CLEAN_DEPENDENCIES := CleanLibCommon
CLEAN_DEPENDENCIES += CleanExternalDep
CLEAN_DEPENDENCIES += CleanEsids
CLEAN_DEPENDENCIES += CleanLibNbio
CLEAN_DEPENDENCIES += CleanLibDF
CLEAN_DEPENDENCIES += CleanLibCCX
CLEAN_DEPENDENCIES += CleanLibMemSoc
CLEAN_DEPENDENCIES += CleanLibMemPhy
CLEAN_DEPENDENCIES += CleanLibMemUmc
CLEAN_DEPENDENCIES += CleanLibMemPlat
CLEAN_DEPENDENCIES += CleanLibMemTech
CLEAN_DEPENDENCIES += CleanLibMemLib
CLEAN_DEPENDENCIES += CleanLibFch
CLEAN_DEPENDENCIES += CleanDrvEsidApi
ifneq (,$(HARNESS_DIR))
  CLEAN_DEPENDENCIES += CleanHarness
endif

.SILENT:
.PHONY: clean configure all harness
.DEFAULT: all
.NOTPARALLEL: all

#Dependencies for a full build
all: $(LIB_DEPENDENCIES) Esids DrvEsidApi DuplicateSymbols CombineAndSignEsids

harness: $(LIB_DEPENDENCIES) HarnessApp

#Clean depends on the sub make calls to remove object files and binaries
#After the individual removals, it removes the Release folder
clean: $(CLEAN_DEPENDENCIES)
	rm -rf $(FWDIR)/Build/$(SOC)

###### BUILD COMMON LIBRARIES ######
LibCommon:
	echo "${yellow}Building Common services library...${reset}"
	$(MAKE) -e --directory=$(COMMON_CODE_DIR) || exit 1

###### BUILD EXTERNAL DEPENDENCIES LIBRARIES ######
LibExternalDep:
	echo "${yellow}Building External Dependencies library...${reset}"
	$(MAKE) -e --directory=$(EXT_DEP_CODE_DIR) || exit 1

###### BUILD NBIO LIBRARY ######
LibNbio:
	echo "${yellow}Building NBIO library...${reset}"
	$(MAKE) -e --directory=$(NBIO_CODE_DIR) || exit 1

###### BUILD DF LIBRARY ######
LibDF:
	echo "${yellow}Building DF library...${reset}"
	$(MAKE) -e --directory=$(DF_CODE_DIR) || exit 1

###### BUILD CCX LIBRARY ######
LibCCX:
	echo "${yellow}Building CCX library...${reset}"
	$(MAKE) -e --directory=$(CCX_CODE_DIR) || exit 1

###### BUILD Memory SOC LIBRARY ######
LibMemSoc:
	echo "${yellow}Building Memory SOC library...${reset}"
	$(MAKE) -e --directory=$(MEM_SOC_CODE_DIR) || exit 1

###### BUILD Memory PHY LIBRARY ######
LibMemPhy:
	echo "${yellow}Building Memory PHY library...${reset}"
	$(MAKE) -e --directory=$(MEM_PHY_CODE_DIR) || exit 1

###### BUILD Memory UMC LIBRARY ######
LibMemUmc:
	echo "${yellow}Building Memory UMC library...${reset}"
	$(MAKE) -e --directory=$(MEM_UMC_CODE_DIR) || exit 1

###### BUILD Memory Platform LIBRARY ######
LibMemPlat:
	echo "${yellow}Building Memory Platform library...${reset}"
	$(MAKE) -e --directory=$(MEM_PLAT_CODE_DIR) || exit 1

###### BUILD Memory Tech LIBRARY ######
LibMemTech:
	echo "${yellow}Building Memory Tech library...${reset}"
	$(MAKE) -e --directory=$(MEM_TECH_CODE_DIR) || exit 1

###### BUILD Memory Lib LIBRARY ######
LibMemLib:
	echo "${yellow}Building Memory Lib library...${reset}"
	$(MAKE) -e --directory=$(MEM_LIB_CODE_DIR) || exit 1

###### BUILD FCH LIBRARY ######
LibFch:
	echo "${yellow}Building FCH library...${reset}"
	$(MAKE) -e --directory=$(FCH_CODE_DIR) || exit 1

###### BUILD ESIDS ######
## If a subdirectory starts with "esid" it will be marked to be built.
## The full path for each esid is stored in ESID_PATHS var. All submakes are
## called using -e to override local sets of vars exported (just a fail-safe
## mechanism)
## Note: Variable SOC comes from cmd line
## NOte: After each eSID binary is built, the script will check their size

Esids:
	for Esid in $(ESID_PATHS) ; do \
		echo "${yellow}Building $$(basename $$Esid)...${reset}" ;\
		DR_NAME=$$(basename $$Esid);\
		$(MAKE) -e BUILD=$(SOC) PROGRAM=$(SOC) DR_NAME=$${DR_NAME}\
						--directory=$$Esid || exit 1 ;\
		$(PIPENV) python3 $(ELF_SIZE_CHECKER) $(SOC) $(SOC_REDIRECT_DIR) $(MEM_TECH) $(ARCH) $${DR_NAME}\
		                  $(FWDIR) $(BINARY_THRESHOLD_SIZE) $(BINARY_THRESHOLD_SIZE_FOR_ORCHESTRATOR) || exit 1;\
	done

###### BUILD DRV ESID API ######
DrvEsidApi:
	echo "${yellow}Building DrvEsid API...${reset}"
	$(MAKE) arch -e --directory=$(DRV_ESID_API_DIR) || exit 1


###### BUILD Harness Tool ######
HarnessApp:
	echo "${yellow}Building Harness...${reset}"
	$(MAKE) -e --directory=$(HARNESS_DIR) IMAGE_VERSION=$(IMAGE_VERSION) || exit 1

DuplicateSymbols:
	echo TODO Check for duplicate symbols

Release:
	mkdir -p $(BUILD_BASE_DIR)/Release/$(ARCH)

# Check for nosign, sign, encrypt signing variants and copy output to the right extension, cbin, csbin, ecsbin
# in release dir
define CHANGE_BIN_NAME
	@BIN_SUFFIX="";\
	for i in 'encryptbinary' 'compressbinary' 'signbinary';do\
	  SIGN_SELECTOR=`grep -E "^$${i}" $(SIGN_CONFIGURE_PATH)/$(UESID_CFG_FILE).ini |  tail -1`; \
	  SIGN_SUFFIX=`echo $${SIGN_SELECTOR} | cut -d" " -f3`; \
	  if [ "$${SIGN_SUFFIX:0:4}" == "true" ]; then\
	    BIN_SUFFIX+=$${i:0:1}; \
	  fi; \
	done;\
	if [ -n "$${BIN_SUFFIX}" ]; then\
	  BIN_BASE_NAME_PATH=`grep -E "^combinedpath" $(SIGN_CONFIGURE_PATH)/$(UESID_CFG_FILE).ini`; \
	  BIN_BASE_PATH=`echo $${BIN_BASE_NAME_PATH} | rev | cut -d" " -f1 | rev`; \
	  BIN_BASE_PATH_1=$$(echo $${BIN_BASE_PATH} | tr -d '\r');\
  	  BIN_NAME=$${BIN_BASE_PATH_1}.$${BIN_SUFFIX}bin; \
	  echo mv $${BIN_BASE_PATH_1}.sbin $${BIN_NAME}; \
	  mv $${BIN_BASE_PATH_1}.sbin $${BIN_NAME}; \
	fi
endef

CombineAndSignEsids: Release | $(SIGN_CONFIGURE_PATH)
ifneq ($(NOSIGN),1)
  ifneq ($(wildcard $(HOME)/fw-sign/*.bin),)
		@echo Applying encrypted login information for $(USER) on $(shell hostname).
		$(eval LOGIN_OPTIONS := -u $(USER) -pp $(HOME)/fw-sign)
  else
		@echo  ===============================================================================================
		@echo  Encrypted Password Path is not found. To use the auto login option:
		@echo  1. Go to https://atlkdsappdev01/pspsp/epw.action
		@echo  2. Complete the form, using this computername $(shell hostname) as the Host name
		@echo  3. Create your encrypted password and save it to $(HOME)/fw-sign/
		@echo  ===============================================================================================
		@echo $(USER)
		$(eval LOGIN_OPTIONS := -u $(USER))
  endif

	@echo "*****************************************************************************"
	@echo "******* 			Signing Binaries with $(SIGN_FUNC_TYPE) Key (pre-si)"
	@echo "*****************************************************************************"
else
	@echo "*****************************************************************************"
	@echo "******* 			Binary not signed"
	@echo "*****************************************************************************"
endif

ifeq ($(CI_SIGN),true)
	@echo "*****************************************************************************"
	@echo "******* 			Signing Binaries with CI_SIGN"
	@echo "*****************************************************************************"
	$(eval LOGIN_OPTIONS := -u $(SP_USERNAME) -p $(SP_PASSWORD))
endif

	mkdir -p $(SIGN_CONFIGURE_PATH)
	$(PIPENV) python3  $(UESID_CFG_GENERATOR) $(UESID_CFG_BASE_INI) $(UESID_META_INF_FILE) $(BUILD_BASE_DIR) $(ARCH) $(MEM_TECH) $(SIGN_CONFIGURE_PATH)/UesidConfig_nosign.ini
	$(PIPENV) python3  $(UESID_CFG_GENERATOR) $(UESID_CFG_BASE_INI) $(UESID_META_INF_FILE) $(BUILD_BASE_DIR) $(ARCH) $(MEM_TECH) $(SIGN_CONFIGURE_PATH)/UesidConfig_sign.ini    --sign --compress
	$(PIPENV) python3  $(UESID_CFG_GENERATOR) $(UESID_CFG_BASE_INI) $(UESID_META_INF_FILE) $(BUILD_BASE_DIR) $(ARCH) $(MEM_TECH) $(SIGN_CONFIGURE_PATH)/UesidConfig_signenc.ini --sign --compress --encrypt
	$(PIPENV) $(FW_SIGN_TOOL) -c $(SIGN_CONFIGURE_PATH)/$(UESID_CFG_FILE).ini -ah false $(LOGIN_OPTIONS)
	$(call CHANGE_BIN_NAME)

$(SIGN_CONFIGURE_PATH) :
	mkdir -p $(SIGN_CONFIGURE_PATH)

#####################################################################################

###### CLEAN DEPENDENCIES ######
## MAKECMDGOALS var is passed in each submake, for them to know "clean" target
## is being called
## Note: Variable SOC comes from cmd line

CleanEsids:
	for Esid in $(ESID_PATHS) ; do \
		rm -rf $$Esid/obj; \
		$(MAKE) BUILD=$(SOC) --directory=$$Esid $(MAKECMDGOALS); \
	done

CleanDrvEsidApi:
	rm -rf $(DRV_ESID_API_DIR)/obj
	$(MAKE) --directory=$(DRV_ESID_API_DIR) $(MAKECMDGOALS)

CleanLibCommon:
	$(MAKE) --directory=$(COMMON_CODE_DIR) $(MAKECMDGOALS)

CleanExternalDep:
	$(MAKE) --directory=$(EXT_DEP_CODE_DIR) $(MAKECMDGOALS)

CleanLibNbio:
	$(MAKE) --directory=$(NBIO_CODE_DIR) $(MAKECMDGOALS)

CleanLibDF:
	$(MAKE) --directory=$(DF_CODE_DIR) $(MAKECMDGOALS)

CleanLibCCX:
	$(MAKE) --directory=$(CCX_CODE_DIR) $(MAKECMDGOALS)

CleanLibMemSoc:
	$(MAKE) --directory=$(MEM_SOC_CODE_DIR) $(MAKECMDGOALS)

CleanLibMemPhy:
	$(MAKE) --directory=$(MEM_PHY_CODE_DIR) $(MAKECMDGOALS)

CleanLibMemUmc:
	$(MAKE) --directory=$(MEM_UMC_CODE_DIR) $(MAKECMDGOALS)

CleanLibMemPlat:
	$(MAKE) --directory=$(MEM_PLAT_CODE_DIR) $(MAKECMDGOALS)

CleanLibMemTech:
	$(MAKE) --directory=$(MEM_TECH_CODE_DIR) $(MAKECMDGOALS)

CleanLibMemLib:
	$(MAKE) --directory=$(MEM_LIB_CODE_DIR) $(MAKECMDGOALS)

CleanLibFch:
	$(MAKE) --directory=$(FCH_CODE_DIR) $(MAKECMDGOALS)

CleanHarness:
ifneq (,$(HARNESS_DIR))
	$(MAKE) --directory=$(HARNESS_DIR) $(MAKECMDGOALS)
endif
