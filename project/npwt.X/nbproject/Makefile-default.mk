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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/npwt.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/npwt.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
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
SOURCEFILES_QUOTED_IF_SPACED=../../source/ziku/6X12.c ../../source/ziku/8X16.c ../../source/ziku/BIOS_ZK_DAW.c ../../source/ziku/BIOS_ZK_X1.c ../../source/BIOS_JLX1864G_139.c ../../source/Flash.c ../../source/adc.c ../../source/sys_cpu.c ../../source/npwt_con_ifile_adc.c ../../source/npwt_con_main.c ../../source/npwt_con_ofile_load_00.c ../../source/npwt_con_over.c ../../source/npwt_dis_ifile_key_00.c ../../source/npwt_dis_main.c ../../source/npwt_dis_ofile_lcd_02.c ../../source/npwt_dis_sys_ini_00.c ../../source/npwt_dis_sys_uart_00.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1499059373/6X12.p1 ${OBJECTDIR}/_ext/1499059373/8X16.p1 ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1 ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1 ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1 ${OBJECTDIR}/_ext/870914629/Flash.p1 ${OBJECTDIR}/_ext/870914629/adc.p1 ${OBJECTDIR}/_ext/870914629/sys_cpu.p1 ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1 ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1 ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1 ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1 ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1 ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1 ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1 ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1 ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1499059373/6X12.p1.d ${OBJECTDIR}/_ext/1499059373/8X16.p1.d ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1.d ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1.d ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1.d ${OBJECTDIR}/_ext/870914629/Flash.p1.d ${OBJECTDIR}/_ext/870914629/adc.p1.d ${OBJECTDIR}/_ext/870914629/sys_cpu.p1.d ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1.d ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1.d ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1.d ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1.d ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1.d ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1.d ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1.d ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1.d ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1499059373/6X12.p1 ${OBJECTDIR}/_ext/1499059373/8X16.p1 ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1 ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1 ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1 ${OBJECTDIR}/_ext/870914629/Flash.p1 ${OBJECTDIR}/_ext/870914629/adc.p1 ${OBJECTDIR}/_ext/870914629/sys_cpu.p1 ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1 ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1 ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1 ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1 ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1 ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1 ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1 ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1 ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1

# Source Files
SOURCEFILES=../../source/ziku/6X12.c ../../source/ziku/8X16.c ../../source/ziku/BIOS_ZK_DAW.c ../../source/ziku/BIOS_ZK_X1.c ../../source/BIOS_JLX1864G_139.c ../../source/Flash.c ../../source/adc.c ../../source/sys_cpu.c ../../source/npwt_con_ifile_adc.c ../../source/npwt_con_main.c ../../source/npwt_con_ofile_load_00.c ../../source/npwt_con_over.c ../../source/npwt_dis_ifile_key_00.c ../../source/npwt_dis_main.c ../../source/npwt_dis_ofile_lcd_02.c ../../source/npwt_dis_sys_ini_00.c ../../source/npwt_dis_sys_uart_00.c



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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/npwt.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F46J11
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1499059373/6X12.p1: ../../source/ziku/6X12.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1499059373" 
	@${RM} ${OBJECTDIR}/_ext/1499059373/6X12.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1499059373/6X12.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1499059373/6X12.p1 ../../source/ziku/6X12.c 
	@-${MV} ${OBJECTDIR}/_ext/1499059373/6X12.d ${OBJECTDIR}/_ext/1499059373/6X12.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1499059373/6X12.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1499059373/8X16.p1: ../../source/ziku/8X16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1499059373" 
	@${RM} ${OBJECTDIR}/_ext/1499059373/8X16.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1499059373/8X16.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1499059373/8X16.p1 ../../source/ziku/8X16.c 
	@-${MV} ${OBJECTDIR}/_ext/1499059373/8X16.d ${OBJECTDIR}/_ext/1499059373/8X16.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1499059373/8X16.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1: ../../source/ziku/BIOS_ZK_DAW.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1499059373" 
	@${RM} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1 ../../source/ziku/BIOS_ZK_DAW.c 
	@-${MV} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.d ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1: ../../source/ziku/BIOS_ZK_X1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1499059373" 
	@${RM} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1 ../../source/ziku/BIOS_ZK_X1.c 
	@-${MV} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.d ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1: ../../source/BIOS_JLX1864G_139.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1 ../../source/BIOS_JLX1864G_139.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.d ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/Flash.p1: ../../source/Flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/Flash.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/Flash.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/Flash.p1 ../../source/Flash.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/Flash.d ${OBJECTDIR}/_ext/870914629/Flash.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/Flash.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/adc.p1: ../../source/adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/adc.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/adc.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/adc.p1 ../../source/adc.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/adc.d ${OBJECTDIR}/_ext/870914629/adc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/adc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/sys_cpu.p1: ../../source/sys_cpu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/sys_cpu.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/sys_cpu.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/sys_cpu.p1 ../../source/sys_cpu.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/sys_cpu.d ${OBJECTDIR}/_ext/870914629/sys_cpu.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/sys_cpu.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1: ../../source/npwt_con_ifile_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1 ../../source/npwt_con_ifile_adc.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.d ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_con_main.p1: ../../source/npwt_con_main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1 ../../source/npwt_con_main.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_con_main.d ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1: ../../source/npwt_con_ofile_load_00.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1 ../../source/npwt_con_ofile_load_00.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.d ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_con_over.p1: ../../source/npwt_con_over.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1 ../../source/npwt_con_over.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_con_over.d ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1: ../../source/npwt_dis_ifile_key_00.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1 ../../source/npwt_dis_ifile_key_00.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.d ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1: ../../source/npwt_dis_main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1 ../../source/npwt_dis_main.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_dis_main.d ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1: ../../source/npwt_dis_ofile_lcd_02.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1 ../../source/npwt_dis_ofile_lcd_02.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.d ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1: ../../source/npwt_dis_sys_ini_00.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1 ../../source/npwt_dis_sys_ini_00.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.d ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1: ../../source/npwt_dis_sys_uart_00.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c  -D__DEBUG=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1 ../../source/npwt_dis_sys_uart_00.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.d ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1499059373/6X12.p1: ../../source/ziku/6X12.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1499059373" 
	@${RM} ${OBJECTDIR}/_ext/1499059373/6X12.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1499059373/6X12.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1499059373/6X12.p1 ../../source/ziku/6X12.c 
	@-${MV} ${OBJECTDIR}/_ext/1499059373/6X12.d ${OBJECTDIR}/_ext/1499059373/6X12.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1499059373/6X12.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1499059373/8X16.p1: ../../source/ziku/8X16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1499059373" 
	@${RM} ${OBJECTDIR}/_ext/1499059373/8X16.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1499059373/8X16.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1499059373/8X16.p1 ../../source/ziku/8X16.c 
	@-${MV} ${OBJECTDIR}/_ext/1499059373/8X16.d ${OBJECTDIR}/_ext/1499059373/8X16.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1499059373/8X16.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1: ../../source/ziku/BIOS_ZK_DAW.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1499059373" 
	@${RM} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1 ../../source/ziku/BIOS_ZK_DAW.c 
	@-${MV} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.d ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_DAW.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1: ../../source/ziku/BIOS_ZK_X1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1499059373" 
	@${RM} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1 ../../source/ziku/BIOS_ZK_X1.c 
	@-${MV} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.d ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1499059373/BIOS_ZK_X1.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1: ../../source/BIOS_JLX1864G_139.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1 ../../source/BIOS_JLX1864G_139.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.d ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/BIOS_JLX1864G_139.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/Flash.p1: ../../source/Flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/Flash.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/Flash.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/Flash.p1 ../../source/Flash.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/Flash.d ${OBJECTDIR}/_ext/870914629/Flash.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/Flash.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/adc.p1: ../../source/adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/adc.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/adc.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/adc.p1 ../../source/adc.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/adc.d ${OBJECTDIR}/_ext/870914629/adc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/adc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/sys_cpu.p1: ../../source/sys_cpu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/sys_cpu.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/sys_cpu.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/sys_cpu.p1 ../../source/sys_cpu.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/sys_cpu.d ${OBJECTDIR}/_ext/870914629/sys_cpu.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/sys_cpu.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1: ../../source/npwt_con_ifile_adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1 ../../source/npwt_con_ifile_adc.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.d ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_con_ifile_adc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_con_main.p1: ../../source/npwt_con_main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1 ../../source/npwt_con_main.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_con_main.d ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_con_main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1: ../../source/npwt_con_ofile_load_00.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1 ../../source/npwt_con_ofile_load_00.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.d ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_con_ofile_load_00.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_con_over.p1: ../../source/npwt_con_over.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1 ../../source/npwt_con_over.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_con_over.d ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_con_over.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1: ../../source/npwt_dis_ifile_key_00.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1 ../../source/npwt_dis_ifile_key_00.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.d ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_dis_ifile_key_00.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1: ../../source/npwt_dis_main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1 ../../source/npwt_dis_main.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_dis_main.d ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_dis_main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1: ../../source/npwt_dis_ofile_lcd_02.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1 ../../source/npwt_dis_ofile_lcd_02.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.d ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_dis_ofile_lcd_02.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1: ../../source/npwt_dis_sys_ini_00.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1 ../../source/npwt_dis_sys_ini_00.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.d ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_ini_00.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1: ../../source/npwt_dis_sys_uart_00.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/870914629" 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1.d 
	@${RM} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1 
	${MP_CC} $(MP_EXTRA_CC_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -c    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -DXPRJ_default=$(CND_CONF)  -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits $(COMPARISON_BUILD)  -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     -o ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1 ../../source/npwt_dis_sys_uart_00.c 
	@-${MV} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.d ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/870914629/npwt_dis_sys_uart_00.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
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
dist/${CND_CONF}/${IMAGE_TYPE}/npwt.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -Wl,-Map=dist/${CND_CONF}/${IMAGE_TYPE}/npwt.X.${IMAGE_TYPE}.map  -D__DEBUG=1  -DXPRJ_default=$(CND_CONF)  -Wl,--defsym=__MPLAB_BUILD=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto        $(COMPARISON_BUILD) -Wl,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -o dist/${CND_CONF}/${IMAGE_TYPE}/npwt.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/npwt.X.${IMAGE_TYPE}.hex 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/npwt.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -mcpu=$(MP_PROCESSOR_OPTION) -Wl,-Map=dist/${CND_CONF}/${IMAGE_TYPE}/npwt.X.${IMAGE_TYPE}.map  -DXPRJ_default=$(CND_CONF)  -Wl,--defsym=__MPLAB_BUILD=1    -fno-short-double -fno-short-float -memi=wordwrite -O3 -maddrqual=ignore -xassembler-with-cpp -I"../../../vr-npwt-s-dis-big-new13_ri_for_eu _chalarm/source/ziku" -I"../../../vr-npwt-s-dis-big-new13_rui_for_eu _chalarm/source/" -I"../../source/h" -I"../../source/ziku" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include" -I"C:/Program Files (x86)/Microchip/xc8/v2.00/include/plib" -mwarn=-3 -Wa,-a -msummary=-psect,-class,+mem,-hex,-file  -ginhx032 -Wl,--data-init -mkeep-startup -mdownload -mdefault-config-bits -std=c99 -gdwarf-3 -mstack=compiled:auto:auto:auto     $(COMPARISON_BUILD) -Wl,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -o dist/${CND_CONF}/${IMAGE_TYPE}/npwt.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	
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
