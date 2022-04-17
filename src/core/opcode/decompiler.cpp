#pragma once
#include <string>
#include "decompiler.hpp"

const std::string opcode_assembly[0xFF] = {
	// 0x00-0x0F
	"NOP"        ,"LD BC,d16","LD (BC),A","INC BC","INC BC","DEC B","LD B,d8","RLCA",
	"LD (a16),SP","ADD HL,BC","LD A,(BC)","DEC BC","INC C" ,"DEC C","LD C,d8","RRCA",
	
	// 0x10-0x1F
	"STOP" ,"LD DE,d16","LD (DE),A","INC DE","INC D","DEC D","LD D,d8","RLA",
	"JR r8","ADD HL,DE","LD A,(DE)","DEC DE","INC E","DEC E","LD E,d8","RRA",
	
	// 0x20-0x2F
	"JR NZ,r8","LD HL,d16", "LD (HL+),A", "INC HL","INC H","DEC H","LD H,d8","DAA",
	"JR Z,r8" ,"ADD HL,HL", "LD A,(HL+)", "DEC HL","INC L","DEC L","LD L,d8","CPL",

	// 0x30-0x3F
	"JR NC,r8","LD SP,d16","LD (HL-),A","INC SP","INC (HL)", "DEC (HL)", "LD (HL),d8", "SCF",
	"JR C,r8" ,"ADD HL,SP","LD A,(HL-)","DEC SP","INC A"   , "DEC A"   , "LD A,d8"   , "CCF"

	// 0x40-0x4F
	"LD B,B","LD B,C","LD B,D","LD B,E","LD B,H", "LD B,L", "LD B,(HL)", "LD B,A",
	"LD C,B","LD C,C","LD C,D","LD C,E","LD C,H", "LD C,L", "LD C,(HL)", "LD C,A",

	// 0x50-0x5F
	"LD D,B","LD D,C","LD D,D","LD D,E","LD D,H", "LD D,L", "LD D,(HL)", "LD D,A",
	"LD E,B","LD E,C","LD E,D","LD E,E","LD E,H", "LD E,L", "LD E,(HL)", "LD E,A",

	// 0x60-0x6F
	"LD H,B","LD H,C","LD H,D","LD H,E","LD H,H", "LD H,L", "LD H,(HL)", "LD H,A",
	"LD L,B","LD L,C","LD L,D","LD L,E","LD L,H", "LD L,L", "LD L,(HL)", "LD L,A",

	// 0x70-0x7F
	"LD (HL),B","LD (HL),C","LD (HL),D","LD (HL),E","LD (HL),H", "LD (HL),L", "HALT"     , "LD (HL),A",
	"LD A,B"   ,"LD A,C"   ,"LD A,D"   ,"LD A,E"   ,"LD A,H"   , "LD A,L"   , "LD A,(HL)", "LD A,A",

	// 0x80-0x8F
	"ADD A,B","ADD A,C","ADD A,D","ADD A,E","ADD A,H", "ADD A,L", "ADD A,(HL)", "ADD A,A",
	"ADC A,B","ADC A,C","ADC A,D","ADC A,E","ADC A,H", "ADC A,L", "ADC A,(HL)", "ADC A,A",

	// 0x90-0x9F
	"SUB B","SUB C","SUB D","SUB E","SUB H", "SUB L", "SUB (HL)", "SUB A",
	"SBC B","SBC C","SBC D","SBC E","SBC H", "SBC L", "SBC (HL)", "SBC A",

	// 0xA0-0xAF
	"AND B","AND C","AND D","AND E","AND H", "AND L", "AND (HL)", "AND A",
	"XOR B","XOR C","XOR D","XOR E","XOR H", "XOR L", "XOR (HL)", "XOR A",

	// 0xB0-0xBF
	"OR B","OR C","OR D","OR E","OR H", "OR L", "OR (HL)", "OR A",
	"CP B","CP C","CP D","CP E","CP H", "CP L", "CP (HL)", "CP A",

	// 0xC0-0xCF
	"RET NZ","POP BC","JP NZ,a16","JP a16"   ,"CALL NZ,a16", "PUSH BC" , "ADD A,d8", "RST 00H",
	"RET Z" ,"RET"   ,"JP Z,a16" ,"prefix CB","CALL Z,a16" , "CALL a16", "ADC A,d8", "RST 08H",

	// 0xD0-0xDF
	"RET NC","POP DE","JP NC,a16","NULL (0xD3)","CALL NC,a16", "PUSH DE"    , "SUB d8", "RST 10H",
	"RET C" ,"RETI"  ,"JP C,a16" ,"NULL (0xDB)","CALL C,a16" , "NULL (0xDD)", "SDC d8", "RST 18H",

	// 0xE0-0xEF
	"LDH (a8),A","POP HL" ,"LD (C),A"  ,"NULL", "NULL","PUSH HL","AND d8","RST 20H",
	"ADD SP,r8" ,"JP (HL)","LD (a16),A","NULL" ,"NULL","NULL"   ,"XOR d8","RST 28H",

	// 0xF0-0xFF
	"LDH A,(a8)"  ,"POP AF"  ,"LD A,(C)"  ,"DI", "NULL","PUSH AF","OR d8","RST 30H",
	"LD HL,SP+r8" ,"LD SP,HL","LD A,(a16)","EI" ,"NULL","NULL"   ,"CP d8","RST 38H",
};

const uint8_t opcode_length[0x100] = {
	1,3,1,1,1,1,2,1,3,1,1,1,1,1,2,1,
	2,3,1,1,1,1,2,1,2,1,1,1,1,1,2,1,
	2,3,1,1,1,1,2,1,2,1,1,1,1,1,2,1,
	2,3,1,1,1,1,2,1,2,1,1,1,1,1,2,1,

	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,

	1,1,3,3,3,1,2,1,1,1,3,1,3,3,2,1,
	1,1,3,1,3,1,2,1,1,1,3,1,3,1,2,1,
	2,1,2,1,1,1,2,1,2,1,3,1,1,1,2,1,
	2,1,2,1,1,1,2,1,2,1,3,1,1,1,2,1,
};


std::string GetAssembly(uint8_t opcode) {
	return opcode_assembly[opcode];
}

uint8_t GetLength(uint8_t opcode) {
	return opcode_length[opcode];
}

