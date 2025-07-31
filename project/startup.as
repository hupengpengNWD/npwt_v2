
	; HI-TECH C PRO for the PIC18 MCU Family V9.66
	; Copyright (C) 1984-2011 HI-TECH Software
	;Serial no. HCPIC18P-111111

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
	; -ID:\1Software_development_document_20121015\npwt_iii_source_code\relese\vr-npwt_s_dis_big_new13_rui_for_droye_end_tom\source\h \
	; -ID:\1Software_development_document_20121015\npwt_iii_source_code\relese\vr-npwt_s_dis_big_new13_rui_for_droye_end_tom\source\ziku \
	; -ID:\1Software_development_document_20121015\npwt_iii_source_code\relese\vr-npwt_s_dis_big_new13_rui_for_droye_end_tom\source \
	; -ID:\1Software_development_document_20121015\npwt_iii_source_code\relese\vr-npwt_s_dis_big_new13_rui_for_droye_end_tom\source\ziku \
	; -P --runtime=default --opt=default -g --asmlist \
	; --errformat=Error   [%n] %f; %l.%c %s --msgformat=Advisory[%n] %s \
	; --warnformat=Warning [%n] %f; %l.%c %s
	;


	GLOBAL	_main,start,_exit
	FNROOT	_main

	pic18cxx	equ	1

	psect	jflash_buffer,class=CODE,reloc=0x400,space=0
	psect	config,class=CONFIG,delta=1
	psect	idloc,class=IDLOC,delta=1
	psect	eeprom_data,class=EEDATA,delta=1
	psect	const,class=CODE,delta=1,reloc=2
	psect	smallconst,class=SMALLCONST,delta=1,reloc=2
	psect	mediumconst,class=MEDIUMCONST,delta=1,reloc=2
	psect	rbss,class=COMRAM,space=1
	psect	bss,class=RAM,space=1
	psect	rdata,class=COMRAM,space=1
	psect	irdata,class=CODE,space=0,reloc=2
	psect	bss,class=RAM,space=1
	psect	data,class=RAM,space=1
	psect	idata,class=CODE,space=0,reloc=2
	psect	nvram,class=NVRAM,space=1
	psect	nvrram,class=COMRAM,space=1
	psect	nvbit,class=COMRAM,bit,space=1
	psect	temp,ovrld,class=COMRAM,space=1
	psect	struct,ovrld,class=COMRAM,space=1
	psect	rbit,class=COMRAM,bit,space=1
	psect	bigbss,class=BIGRAM,space=1
	psect	bigdata,class=BIGRAM,space=1
	psect	ibigdata,class=CODE,space=0,reloc=2
	psect	farbss,class=FARRAM,space=0,reloc=2,delta=1
	psect	fardata,class=FARRAM,space=0,reloc=2,delta=1
	psect	ifardata,class=CODE,space=0,reloc=2,delta=1

	psect	reset_vec,class=CODE,delta=1,reloc=2
	psect	powerup,class=CODE,delta=1,reloc=2
	psect	intcode,class=CODE,delta=1,reloc=2
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


; User config settings for word 1
;
	psect config
	org 0
	dw 0xF4A1

; User config settings for word 2
;
	psect config
	org 2
	dw 0xFFDA

; User config settings for word 3
;
	psect config
	org 4
	dw 0xF9F7

; User config settings for word 4
;
	psect config
	org 6
	dw 0xF1FF
	psect	rparam,class=COMRAM,space=1
	psect	param0,class=BANK0,space=1
	psect	param1,class=BANK1,space=1
	psect	param2,class=BANK2,space=1
	psect	param3,class=BANK3,space=1
	psect	param4,class=BANK4,space=1
	psect	param5,class=BANK5,space=1
	psect	param6,class=BANK6,space=1
	psect	param7,class=BANK7,space=1
	psect	param8,class=BANK8,space=1
	psect	param9,class=BANK9,space=1
	psect	param10,class=BANK10,space=1
	psect	param11,class=BANK11,space=1
	psect	param12,class=BANK12,space=1
	psect	param13,class=BANK13,space=1
	psect	param14,class=BANK14,space=1
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

	GLOBAL __accesstop
__accesstop EQU 96
	PSECT	ramtop,class=RAM
	GLOBAL	__S1			; top of RAM usage
	GLOBAL	__ramtop
	GLOBAL	__LRAM,__HRAM
__ramtop:

	PSECT	reset_vec
reset_pos:
	goto	start

	PSECT	init
start:
_exit:
; fardata psect  - 0 bytes to load

; farbss psect  - 0 bytes to clear

; bigdata psect - 0 bytes to load

; data0 psect - 26 bytes to load
GLOBAL	__Ldata0,__Lidata
	lfsr	0,__Ldata0
	; load TBLPTR registers with __Lidata
	movlw	low (__Lidata)
	movwf	tblptrl
	movlw	high(__Lidata)
	movwf	tblptrh
	lfsr	1,26	; loop variable
	call	copy_data

	lfsr	0,0
	movlw	33	; loop counter
clear_1:
	clrf	postinc0,c	; clear, increment FSR0
	decf	wreg,c	; decrement loop counter
	bnz	clear_1
	lfsr	0,96
	movlw	208	; loop counter
clear_2:
	clrf	postinc0,c	; clear, increment FSR0
	decf	wreg,c	; decrement loop counter
	bnz	clear_2
	PSECT	end_init
	GLOBAL	__Lmediumconst
	movlw	low highword(__Lmediumconst)
	movwf	tblptru
	goto	_main		;go do the main stuff
; Copy the ROM data image to destination in RAM
copy_data:
	tblrd	*+
	movff	tablat,postinc0
	movf	postdec1,w	;decrement loop variable
	movf	fsr1l,w
	bnz	copy_data
	movf	fsr1h,w
	bnz	copy_data
	return

	END	reset_pos
