
	; Microchip MPLAB XC8 C Compiler V1.33
	; Copyright (C) 1984-2014 HI-TECH Software

	; Auto-generated runtime startup code for final link stage.

	;
	; Compiler options:
	;
	; -onpwt.cof -mnpwt.map --summary=default --output=default adc.p1 \
	; BIOS_JLX1864G_139.p1 Flash.p1 npwt_con_ifile_adc.p1 npwt_con_main.p1 \
	; npwt_con_ofile_load_00.p1 npwt_con_over.p1 npwt_dis_ifile_key_00.p1 \
	; npwt_dis_main.p1 npwt_dis_ofile_lcd_02.p1 npwt_dis_sys_ini_00.p1 \
	; npwt_dis_sys_uart_00.p1 sys_cpu.p1 6X12.p1 8X16.p1 BIOS_ZK_DAW.p1 \
	; BIOS_ZK_X1.p1 --chip=18F46J11 \
	; -ID:\2016\npwt_iii_source_code\relese\vr-npwt_s_dis_big_new13_rui_for_eu _false_sin\source\h \
	; -P --runtime=default --opt=default,+asm,-debug,-speed,+space,9 \
	; --warn=0 -N255 --addrqual=ignore -g --asmlist \
	; --errformat=Error   [%n] %f; %l.%c %s --msgformat=Advisory[%n] %s \
	; --warnformat=Warning [%n] %f; %l.%c %s
	;


	processor	18F46J11

	GLOBAL	_main,start
	FNROOT	_main

	pic18cxx	equ	1

	psect	jflash_buffer,class=CODE,space=0,noexec
	psect	config,class=CONFIG,delta=1,noexec
	psect	idloc,class=IDLOC,delta=1,noexec
	psect	const,class=CODE,delta=1,reloc=2,noexec
	psect	smallconst,class=SMALLCONST,delta=1,reloc=2,noexec
	psect	mediumconst,class=MEDIUMCONST,delta=1,reloc=2,noexec
	psect	rbss,class=COMRAM,space=1,noexec
	psect	bss,class=RAM,space=1,noexec
	psect	rdata,class=COMRAM,space=1,noexec
	psect	irdata,class=CODE,space=0,reloc=2,noexec
	psect	bss,class=RAM,space=1,noexec
	psect	data,class=RAM,space=1,noexec
	psect	idata,class=CODE,space=0,reloc=2,noexec
	psect	nvrram,class=COMRAM,space=1,noexec
	psect	nvbit,class=COMRAM,bit,space=1,noexec
	psect	temp,ovrld,class=COMRAM,space=1,noexec
	psect	struct,ovrld,class=COMRAM,space=1,noexec
	psect	rbit,class=COMRAM,bit,space=1,noexec
	psect	bigbss,class=BIGRAM,space=1,noexec
	psect	bigdata,class=BIGRAM,space=1,noexec
	psect	ibigdata,class=CODE,space=0,reloc=2,noexec
	psect	farbss,class=FARRAM,space=0,reloc=2,delta=1,noexec
	psect	nvFARRAM,class=FARRAM,space=0,reloc=2,delta=1,noexec
	psect	fardata,class=FARRAM,space=0,reloc=2,delta=1,noexec
	psect	ifardata,class=CODE,space=0,reloc=2,delta=1,noexec

	psect	reset_vec,class=CODE,delta=1,reloc=2
	psect	powerup,class=CODE,delta=1,reloc=2
	psect	intcode,class=CODE,delta=1,reloc=2
	psect	intcode_body,class=CODE,delta=1,reloc=2
	psect	intcodelo,class=CODE,delta=1,reloc=2
	psect	intret,class=CODE,delta=1,reloc=2
	psect	intentry,class=CODE,delta=1,reloc=2

	psect	intsave_regs,class=BIGRAM,space=1
	psect	init,class=CODE,delta=1,reloc=2
	psect	text,class=CODE,delta=1,reloc=2
GLOBAL	intlevel0,intlevel1,intlevel2
intlevel0:
intlevel1:
intlevel2:
GLOBAL	intlevel3
intlevel3:
	psect	end_init,class=CODE,delta=1,reloc=2
	psect	clrtext,class=CODE,delta=1,reloc=2

	psect	smallconst
	GLOBAL	__smallconst
__smallconst:
	psect	mediumconst
	GLOBAL	__mediumconst
__mediumconst:
wreg	EQU	0FE8h
fsr0l	EQU	0FE9h
fsr0h	EQU	0FEAh
fsr1l	EQU	0FE1h
fsr1h	EQU	0FE2h
fsr2l	EQU	0FD9h
fsr2h	EQU	0FDAh
postinc0	EQU	0FEEh
postdec0	EQU	0FEDh
postinc1	EQU	0FE6h
postdec1	EQU	0FE5h
postinc2	EQU	0FDEh
postdec2	EQU	0FDDh
tblptrl	EQU	0FF6h
tblptrh	EQU	0FF7h
tblptru	EQU	0FF8h
tablat		EQU	0FF5h

	PSECT	ramtop,class=RAM,noexec
	GLOBAL	__S1			; top of RAM usage
	GLOBAL	__ramtop
	GLOBAL	__LRAM,__HRAM
__ramtop:

	psect	reset_vec
reset_vec:
	; No powerup routine
	global start

; jump to start
	goto start
	GLOBAL __accesstop
__accesstop EQU 96


	psect	init
start:

;Initialize the stack pointer (FSR1)
	global stacklo, stackhi
	stacklo	equ	014Bh
	stackhi	equ	09FFh


	psect	stack,class=STACK,space=2,noexec
	global ___sp,___inthi_sp,___intlo_sp
___sp:
___inthi_sp:
___intlo_sp:

	psect	end_init
	global start_initialization
	goto start_initialization	;jump to C runtime clear & initialization

; Config register CONFIG1L @ 0xFFF8
;	Stack Overflow/Underflow Reset
;	STVREN = 0x1, unprogrammed default
;	Background Debug
;	DEBUG = 0x1, unprogrammed default
;	Watchdog Timer
;	WDTEN = ON, Enabled
;	Extended Instruction Set
;	XINST = OFF, Disabled

	psect	config,class=CONFIG,delta=1,noexec
		org 0x0
		db 0xA1

; Config register CONFIG1H @ 0xFFF9
;	Code Protect
;	CP0 = OFF, Program memory is not code-protected

	psect	config,class=CONFIG,delta=1,noexec
		org 0x1
		db 0xF4

; Config register CONFIG2L @ 0xFFFA
;	T1OSCEN Enforcement
;	T1DIG = 0x1, unprogrammed default
;	Oscillator
;	OSC = INTOSCPLL, INTOSCPLL
;	Fail-Safe Clock Monitor
;	FCMEN = 0x1, unprogrammed default
;	Internal External Oscillator Switch Over Mode
;	IESO = 0x1, unprogrammed default
;	Low-Power Timer1 Oscillator
;	LPT1OSC = 0x1, unprogrammed default

	psect	config,class=CONFIG,delta=1,noexec
		org 0x2
		db 0xDA

; Config register CONFIG2H @ 0xFFFB
;	Watchdog Postscaler
;	WDTPS = 16384, 1:16384

	psect	config,class=CONFIG,delta=1,noexec
		org 0x3
		db 0xFE

; Config register CONFIG3L @ 0xFFFC
;	Deep Sleep BOR
;	DSBOREN = 0x1, unprogrammed default
;	Deep Sleep Watchdog Postscaler
;	DSWDTPS = 0xF, unprogrammed default
;	DSWDT Clock Select
;	DSWDTOSC = 0x1, unprogrammed default
;	Deep Sleep Watchdog Timer
;	DSWDTEN = OFF, Disabled
;	RTCC Clock Select
;	RTCOSC = 0x1, unprogrammed default

	psect	config,class=CONFIG,delta=1,noexec
		org 0x4
		db 0xF7

; Config register CONFIG3H @ 0xFFFD
;	IOLOCK One-Way Set Enable bit
;	IOL1WAY = OFF, The IOLOCK bit (PPSCON<0>) can be set and cleared as needed
;	MSSP address masking
;	MSSP7B_EN = 0x1, unprogrammed default

	psect	config,class=CONFIG,delta=1,noexec
		org 0x5
		db 0xF8

; Config register CONFIG4L @ 0xFFFE
;	Write/Erase Protect Page Start/End Location
;	WPFP = 0x3F, unprogrammed default
;	Write/Erase Protect Configuration Region
;	WPCFG = OFF, Configuration Words page not erase/write-protected
;	Write/Erase Protect Region Select (valid when WPDIS = 0)
;	WPEND = 0x1, unprogrammed default

	psect	config,class=CONFIG,delta=1,noexec
		org 0x6
		db 0xFF

; Config register CONFIG4H @ 0xFFFF
;	Write Protect Disable bit
;	WPDIS = OFF, WPFP<5:0>/WPEND region ignored

	psect	config,class=CONFIG,delta=1,noexec
		org 0x7
		db 0xF1


psect comram,class=COMRAM,space=1
psect abs1,class=ABS1,space=1
psect bigram,class=BIGRAM,space=1
psect ram,class=RAM,space=1
psect bank0,class=BANK0,space=1
psect bank1,class=BANK1,space=1
psect bank2,class=BANK2,space=1
psect bank3,class=BANK3,space=1
psect bank4,class=BANK4,space=1
psect bank5,class=BANK5,space=1
psect bank6,class=BANK6,space=1
psect bank7,class=BANK7,space=1
psect bank8,class=BANK8,space=1
psect bank9,class=BANK9,space=1
psect bank10,class=BANK10,space=1
psect bank11,class=BANK11,space=1
psect bank12,class=BANK12,space=1
psect bank13,class=BANK13,space=1
psect bank14,class=BANK14,space=1
psect sfr,class=SFR,space=1


	end	start
