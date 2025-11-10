#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/NWPT_VR.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/NWPT_VR.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../Application/Src/app_beep.c ../Application/Src/app_button.c ../Application/Src/app_ui.c ../Application/Src/app_battery.c ../Application/Src/app_pressure.c ../Core/Src/main.c ../HAL/Src/hal_adc.c ../HAL/Src/hal_gpio.c ../HAL/Src/hal_timer.c ../HAL/Src/hal_lcd.c ../Library/lcd_font_data_8x16.c ../Library/lcd_font_data_7x14.c ../Library/lcd_font_data_6x12.c ../Library/lcd_icon_data.c ../Library/lcd_font_data_16x32.c ../Middleware/Src/pushpull.c ../Middleware/Src/soft_timer.c ../Middleware/Src/display.c ../Middleware/Src/key_machine.c ../Middleware/Src/queue.c ../Middleware/Src/fsm.c ../Middleware/Src/pwm.c ../Middleware/Src/pid.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1263633898/app_beep.p1 ${OBJECTDIR}/_ext/1263633898/app_button.p1 ${OBJECTDIR}/_ext/1263633898/app_ui.p1 ${OBJECTDIR}/_ext/1263633898/app_battery.p1 ${OBJECTDIR}/_ext/1263633898/app_pressure.p1 ${OBJECTDIR}/_ext/959733021/main.p1 ${OBJECTDIR}/_ext/272184615/hal_adc.p1 ${OBJECTDIR}/_ext/272184615/hal_gpio.p1 ${OBJECTDIR}/_ext/272184615/hal_timer.p1 ${OBJECTDIR}/_ext/272184615/hal_lcd.p1 ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1 ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1 ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1 ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1 ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1 ${OBJECTDIR}/_ext/524563478/pushpull.p1 ${OBJECTDIR}/_ext/524563478/soft_timer.p1 ${OBJECTDIR}/_ext/524563478/display.p1 ${OBJECTDIR}/_ext/524563478/key_machine.p1 ${OBJECTDIR}/_ext/524563478/queue.p1 ${OBJECTDIR}/_ext/524563478/fsm.p1 ${OBJECTDIR}/_ext/524563478/pwm.p1 ${OBJECTDIR}/_ext/524563478/pid.p1
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1263633898/app_beep.p1.d ${OBJECTDIR}/_ext/1263633898/app_button.p1.d ${OBJECTDIR}/_ext/1263633898/app_ui.p1.d ${OBJECTDIR}/_ext/1263633898/app_battery.p1.d ${OBJECTDIR}/_ext/1263633898/app_pressure.p1.d ${OBJECTDIR}/_ext/959733021/main.p1.d ${OBJECTDIR}/_ext/272184615/hal_adc.p1.d ${OBJECTDIR}/_ext/272184615/hal_gpio.p1.d ${OBJECTDIR}/_ext/272184615/hal_timer.p1.d ${OBJECTDIR}/_ext/272184615/hal_lcd.p1.d ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1.d ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1.d ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1.d ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1.d ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1.d ${OBJECTDIR}/_ext/524563478/pushpull.p1.d ${OBJECTDIR}/_ext/524563478/soft_timer.p1.d ${OBJECTDIR}/_ext/524563478/display.p1.d ${OBJECTDIR}/_ext/524563478/key_machine.p1.d ${OBJECTDIR}/_ext/524563478/queue.p1.d ${OBJECTDIR}/_ext/524563478/fsm.p1.d ${OBJECTDIR}/_ext/524563478/pwm.p1.d ${OBJECTDIR}/_ext/524563478/pid.p1.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1263633898/app_beep.p1 ${OBJECTDIR}/_ext/1263633898/app_button.p1 ${OBJECTDIR}/_ext/1263633898/app_ui.p1 ${OBJECTDIR}/_ext/1263633898/app_battery.p1 ${OBJECTDIR}/_ext/1263633898/app_pressure.p1 ${OBJECTDIR}/_ext/959733021/main.p1 ${OBJECTDIR}/_ext/272184615/hal_adc.p1 ${OBJECTDIR}/_ext/272184615/hal_gpio.p1 ${OBJECTDIR}/_ext/272184615/hal_timer.p1 ${OBJECTDIR}/_ext/272184615/hal_lcd.p1 ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1 ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1 ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1 ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1 ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1 ${OBJECTDIR}/_ext/524563478/pushpull.p1 ${OBJECTDIR}/_ext/524563478/soft_timer.p1 ${OBJECTDIR}/_ext/524563478/display.p1 ${OBJECTDIR}/_ext/524563478/key_machine.p1 ${OBJECTDIR}/_ext/524563478/queue.p1 ${OBJECTDIR}/_ext/524563478/fsm.p1 ${OBJECTDIR}/_ext/524563478/pwm.p1 ${OBJECTDIR}/_ext/524563478/pid.p1

# Source Files
SOURCEFILES=../Application/Src/app_beep.c ../Application/Src/app_button.c ../Application/Src/app_ui.c ../Application/Src/app_battery.c ../Application/Src/app_pressure.c ../Core/Src/main.c ../HAL/Src/hal_adc.c ../HAL/Src/hal_gpio.c ../HAL/Src/hal_timer.c ../HAL/Src/hal_lcd.c ../Library/lcd_font_data_8x16.c ../Library/lcd_font_data_7x14.c ../Library/lcd_font_data_6x12.c ../Library/lcd_icon_data.c ../Library/lcd_font_data_16x32.c ../Middleware/Src/pushpull.c ../Middleware/Src/soft_timer.c ../Middleware/Src/display.c ../Middleware/Src/key_machine.c ../Middleware/Src/queue.c ../Middleware/Src/fsm.c ../Middleware/Src/pwm.c ../Middleware/Src/pid.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/NWPT_VR.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F46J11
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1263633898/app_beep.p1: ../Application/Src/app_beep.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1263633898" 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_beep.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_beep.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1263633898/app_beep.p1 ../Application/Src/app_beep.c 
	@-${MV} ${OBJECTDIR}/_ext/1263633898/app_beep.d ${OBJECTDIR}/_ext/1263633898/app_beep.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1263633898/app_beep.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1263633898/app_button.p1: ../Application/Src/app_button.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1263633898" 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_button.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_button.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1263633898/app_button.p1 ../Application/Src/app_button.c 
	@-${MV} ${OBJECTDIR}/_ext/1263633898/app_button.d ${OBJECTDIR}/_ext/1263633898/app_button.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1263633898/app_button.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1263633898/app_ui.p1: ../Application/Src/app_ui.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1263633898" 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_ui.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_ui.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1263633898/app_ui.p1 ../Application/Src/app_ui.c 
	@-${MV} ${OBJECTDIR}/_ext/1263633898/app_ui.d ${OBJECTDIR}/_ext/1263633898/app_ui.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1263633898/app_ui.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1263633898/app_battery.p1: ../Application/Src/app_battery.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1263633898" 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_battery.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_battery.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1263633898/app_battery.p1 ../Application/Src/app_battery.c 
	@-${MV} ${OBJECTDIR}/_ext/1263633898/app_battery.d ${OBJECTDIR}/_ext/1263633898/app_battery.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1263633898/app_battery.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1263633898/app_pressure.p1: ../Application/Src/app_pressure.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1263633898" 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_pressure.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_pressure.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1263633898/app_pressure.p1 ../Application/Src/app_pressure.c 
	@-${MV} ${OBJECTDIR}/_ext/1263633898/app_pressure.d ${OBJECTDIR}/_ext/1263633898/app_pressure.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1263633898/app_pressure.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/959733021/main.p1: ../Core/Src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/959733021" 
	@${RM} ${OBJECTDIR}/_ext/959733021/main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/959733021/main.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/959733021/main.p1 ../Core/Src/main.c 
	@-${MV} ${OBJECTDIR}/_ext/959733021/main.d ${OBJECTDIR}/_ext/959733021/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/959733021/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/272184615/hal_adc.p1: ../HAL/Src/hal_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/272184615" 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_adc.p1.d 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_adc.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/272184615/hal_adc.p1 ../HAL/Src/hal_adc.c 
	@-${MV} ${OBJECTDIR}/_ext/272184615/hal_adc.d ${OBJECTDIR}/_ext/272184615/hal_adc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/272184615/hal_adc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/272184615/hal_gpio.p1: ../HAL/Src/hal_gpio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/272184615" 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_gpio.p1.d 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_gpio.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/272184615/hal_gpio.p1 ../HAL/Src/hal_gpio.c 
	@-${MV} ${OBJECTDIR}/_ext/272184615/hal_gpio.d ${OBJECTDIR}/_ext/272184615/hal_gpio.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/272184615/hal_gpio.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/272184615/hal_timer.p1: ../HAL/Src/hal_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/272184615" 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_timer.p1.d 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_timer.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/272184615/hal_timer.p1 ../HAL/Src/hal_timer.c 
	@-${MV} ${OBJECTDIR}/_ext/272184615/hal_timer.d ${OBJECTDIR}/_ext/272184615/hal_timer.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/272184615/hal_timer.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/272184615/hal_lcd.p1: ../HAL/Src/hal_lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/272184615" 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_lcd.p1.d 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_lcd.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/272184615/hal_lcd.p1 ../HAL/Src/hal_lcd.c 
	@-${MV} ${OBJECTDIR}/_ext/272184615/hal_lcd.d ${OBJECTDIR}/_ext/272184615/hal_lcd.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/272184615/hal_lcd.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1: ../Library/lcd_font_data_8x16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/150355788" 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1.d 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1 ../Library/lcd_font_data_8x16.c 
	@-${MV} ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.d ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1: ../Library/lcd_font_data_7x14.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/150355788" 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1.d 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1 ../Library/lcd_font_data_7x14.c 
	@-${MV} ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.d ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1: ../Library/lcd_font_data_6x12.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/150355788" 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1.d 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1 ../Library/lcd_font_data_6x12.c 
	@-${MV} ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.d ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1: ../Library/lcd_icon_data.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/150355788" 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1.d 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1 ../Library/lcd_icon_data.c 
	@-${MV} ${OBJECTDIR}/_ext/150355788/lcd_icon_data.d ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1: ../Library/lcd_font_data_16x32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/150355788" 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1.d 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1 ../Library/lcd_font_data_16x32.c 
	@-${MV} ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.d ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/pushpull.p1: ../Middleware/Src/pushpull.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/pushpull.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/pushpull.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/pushpull.p1 ../Middleware/Src/pushpull.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/pushpull.d ${OBJECTDIR}/_ext/524563478/pushpull.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/pushpull.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/soft_timer.p1: ../Middleware/Src/soft_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/soft_timer.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/soft_timer.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/soft_timer.p1 ../Middleware/Src/soft_timer.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/soft_timer.d ${OBJECTDIR}/_ext/524563478/soft_timer.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/soft_timer.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/display.p1: ../Middleware/Src/display.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/display.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/display.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/display.p1 ../Middleware/Src/display.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/display.d ${OBJECTDIR}/_ext/524563478/display.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/display.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/key_machine.p1: ../Middleware/Src/key_machine.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/key_machine.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/key_machine.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/key_machine.p1 ../Middleware/Src/key_machine.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/key_machine.d ${OBJECTDIR}/_ext/524563478/key_machine.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/key_machine.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/queue.p1: ../Middleware/Src/queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/queue.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/queue.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/queue.p1 ../Middleware/Src/queue.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/queue.d ${OBJECTDIR}/_ext/524563478/queue.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/queue.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/fsm.p1: ../Middleware/Src/fsm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/fsm.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/fsm.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/fsm.p1 ../Middleware/Src/fsm.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/fsm.d ${OBJECTDIR}/_ext/524563478/fsm.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/fsm.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/pwm.p1: ../Middleware/Src/pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/pwm.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/pwm.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/pwm.p1 ../Middleware/Src/pwm.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/pwm.d ${OBJECTDIR}/_ext/524563478/pwm.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/pwm.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/pid.p1: ../Middleware/Src/pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/pid.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/pid.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/pid.p1 ../Middleware/Src/pid.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/pid.d ${OBJECTDIR}/_ext/524563478/pid.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/pid.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1263633898/app_beep.p1: ../Application/Src/app_beep.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1263633898" 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_beep.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_beep.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1263633898/app_beep.p1 ../Application/Src/app_beep.c 
	@-${MV} ${OBJECTDIR}/_ext/1263633898/app_beep.d ${OBJECTDIR}/_ext/1263633898/app_beep.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1263633898/app_beep.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1263633898/app_button.p1: ../Application/Src/app_button.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1263633898" 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_button.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_button.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1263633898/app_button.p1 ../Application/Src/app_button.c 
	@-${MV} ${OBJECTDIR}/_ext/1263633898/app_button.d ${OBJECTDIR}/_ext/1263633898/app_button.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1263633898/app_button.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1263633898/app_ui.p1: ../Application/Src/app_ui.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1263633898" 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_ui.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_ui.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1263633898/app_ui.p1 ../Application/Src/app_ui.c 
	@-${MV} ${OBJECTDIR}/_ext/1263633898/app_ui.d ${OBJECTDIR}/_ext/1263633898/app_ui.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1263633898/app_ui.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1263633898/app_battery.p1: ../Application/Src/app_battery.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1263633898" 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_battery.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_battery.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1263633898/app_battery.p1 ../Application/Src/app_battery.c 
	@-${MV} ${OBJECTDIR}/_ext/1263633898/app_battery.d ${OBJECTDIR}/_ext/1263633898/app_battery.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1263633898/app_battery.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1263633898/app_pressure.p1: ../Application/Src/app_pressure.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1263633898" 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_pressure.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1263633898/app_pressure.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1263633898/app_pressure.p1 ../Application/Src/app_pressure.c 
	@-${MV} ${OBJECTDIR}/_ext/1263633898/app_pressure.d ${OBJECTDIR}/_ext/1263633898/app_pressure.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1263633898/app_pressure.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/959733021/main.p1: ../Core/Src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/959733021" 
	@${RM} ${OBJECTDIR}/_ext/959733021/main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/959733021/main.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/959733021/main.p1 ../Core/Src/main.c 
	@-${MV} ${OBJECTDIR}/_ext/959733021/main.d ${OBJECTDIR}/_ext/959733021/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/959733021/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/272184615/hal_adc.p1: ../HAL/Src/hal_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/272184615" 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_adc.p1.d 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_adc.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/272184615/hal_adc.p1 ../HAL/Src/hal_adc.c 
	@-${MV} ${OBJECTDIR}/_ext/272184615/hal_adc.d ${OBJECTDIR}/_ext/272184615/hal_adc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/272184615/hal_adc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/272184615/hal_gpio.p1: ../HAL/Src/hal_gpio.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/272184615" 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_gpio.p1.d 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_gpio.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/272184615/hal_gpio.p1 ../HAL/Src/hal_gpio.c 
	@-${MV} ${OBJECTDIR}/_ext/272184615/hal_gpio.d ${OBJECTDIR}/_ext/272184615/hal_gpio.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/272184615/hal_gpio.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/272184615/hal_timer.p1: ../HAL/Src/hal_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/272184615" 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_timer.p1.d 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_timer.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/272184615/hal_timer.p1 ../HAL/Src/hal_timer.c 
	@-${MV} ${OBJECTDIR}/_ext/272184615/hal_timer.d ${OBJECTDIR}/_ext/272184615/hal_timer.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/272184615/hal_timer.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/272184615/hal_lcd.p1: ../HAL/Src/hal_lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/272184615" 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_lcd.p1.d 
	@${RM} ${OBJECTDIR}/_ext/272184615/hal_lcd.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/272184615/hal_lcd.p1 ../HAL/Src/hal_lcd.c 
	@-${MV} ${OBJECTDIR}/_ext/272184615/hal_lcd.d ${OBJECTDIR}/_ext/272184615/hal_lcd.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/272184615/hal_lcd.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1: ../Library/lcd_font_data_8x16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/150355788" 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1.d 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1 ../Library/lcd_font_data_8x16.c 
	@-${MV} ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.d ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/150355788/lcd_font_data_8x16.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1: ../Library/lcd_font_data_7x14.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/150355788" 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1.d 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1 ../Library/lcd_font_data_7x14.c 
	@-${MV} ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.d ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/150355788/lcd_font_data_7x14.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1: ../Library/lcd_font_data_6x12.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/150355788" 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1.d 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1 ../Library/lcd_font_data_6x12.c 
	@-${MV} ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.d ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/150355788/lcd_font_data_6x12.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1: ../Library/lcd_icon_data.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/150355788" 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1.d 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1 ../Library/lcd_icon_data.c 
	@-${MV} ${OBJECTDIR}/_ext/150355788/lcd_icon_data.d ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/150355788/lcd_icon_data.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1: ../Library/lcd_font_data_16x32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/150355788" 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1.d 
	@${RM} ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1 ../Library/lcd_font_data_16x32.c 
	@-${MV} ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.d ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/150355788/lcd_font_data_16x32.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/pushpull.p1: ../Middleware/Src/pushpull.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/pushpull.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/pushpull.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/pushpull.p1 ../Middleware/Src/pushpull.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/pushpull.d ${OBJECTDIR}/_ext/524563478/pushpull.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/pushpull.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/soft_timer.p1: ../Middleware/Src/soft_timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/soft_timer.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/soft_timer.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/soft_timer.p1 ../Middleware/Src/soft_timer.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/soft_timer.d ${OBJECTDIR}/_ext/524563478/soft_timer.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/soft_timer.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/display.p1: ../Middleware/Src/display.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/display.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/display.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/display.p1 ../Middleware/Src/display.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/display.d ${OBJECTDIR}/_ext/524563478/display.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/display.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/key_machine.p1: ../Middleware/Src/key_machine.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/key_machine.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/key_machine.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/key_machine.p1 ../Middleware/Src/key_machine.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/key_machine.d ${OBJECTDIR}/_ext/524563478/key_machine.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/key_machine.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/queue.p1: ../Middleware/Src/queue.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/queue.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/queue.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/queue.p1 ../Middleware/Src/queue.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/queue.d ${OBJECTDIR}/_ext/524563478/queue.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/queue.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/fsm.p1: ../Middleware/Src/fsm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/fsm.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/fsm.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/fsm.p1 ../Middleware/Src/fsm.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/fsm.d ${OBJECTDIR}/_ext/524563478/fsm.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/fsm.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/pwm.p1: ../Middleware/Src/pwm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/pwm.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/pwm.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/pwm.p1 ../Middleware/Src/pwm.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/pwm.d ${OBJECTDIR}/_ext/524563478/pwm.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/pwm.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/524563478/pid.p1: ../Middleware/Src/pid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/524563478" 
	@${RM} ${OBJECTDIR}/_ext/524563478/pid.p1.d 
	@${RM} ${OBJECTDIR}/_ext/524563478/pid.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/524563478/pid.p1 ../Middleware/Src/pid.c 
	@-${MV} ${OBJECTDIR}/_ext/524563478/pid.d ${OBJECTDIR}/_ext/524563478/pid.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/524563478/pid.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/NWPT_VR.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -Wl,-Map=dist/${CND_CONF}/${IMAGE_TYPE}/NWPT_VR.X.${IMAGE_TYPE}.map  -D__DEBUG=1  -DXPRJ_default=$(CND_CONF)  -Wl,--defsym=__MPLAB_BUILD=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto        $(COMPARISON_BUILD) -Wl,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -o dist/${CND_CONF}/${IMAGE_TYPE}/NWPT_VR.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/NWPT_VR.X.${IMAGE_TYPE}.hex 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/NWPT_VR.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -Wl,-Map=dist/${CND_CONF}/${IMAGE_TYPE}/NWPT_VR.X.${IMAGE_TYPE}.map  -DXPRJ_default=$(CND_CONF)  -Wl,--defsym=__MPLAB_BUILD=1    -fno-short-double -fno-short-float -memi=wordwrite -O0 -fasmfile -maddrqual=ignore -xassembler-with-cpp -I"../Application/Inc" -I"../Core/Inc" -I"../Drivers/Inc" -I"../Middleware/Inc" -I"../HAL/Inc" -mwarn=-3 -Wa,-a -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     $(COMPARISON_BUILD) -Wl,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -o dist/${CND_CONF}/${IMAGE_TYPE}/NWPT_VR.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
