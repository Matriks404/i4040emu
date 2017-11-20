#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define printf(fmt, ...) (0) // Uncomment to disable printf's.

unsigned int cycles[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // NOP, HLT, BBS, LCR, OR4, OR5, AN6, AN7, DB0, DB1, IR0, IR1, EIN, DIN, RPM, NOP (probably)
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // JCN
	2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1 // FIM (Even), SRC (Odd)
};

unsigned int pc_stack[8];
unsigned int ram_pointer;
unsigned char acc = 0;

unsigned char carry = 0; // False
unsigned char test = 0; // False

unsigned int rom_bank = 0;
unsigned int ir_bank = 0;

unsigned char ir0[16]; // IR0-15 from IR0 bank
unsigned char ir1[8]; // IR0-7 from IR1 bank

unsigned char memory[4096];

void clearRAM() {
	for(int i = 0; i< 4096; i++) {
		memory[i] = 0;
	}
}

void resetRAM() { // This function is temporary used to randomize data in RAM to test CPU.
	for(int i = 0; i < 4096; i++) {
		memory[i] = rand() % 48;
	}
}

void unimplemented() {
	printf("\tWARNING: This instruction is not fully implemented!!!\n");
}

void jcnUnimplemented() {
	printf("\tWARNING: JCN instructions are not yet implemented!!!\n");
}

void fim(unsigned char rpair, unsigned int data) {
	if(ir_bank == 0) {
		ir0[2 * rpair + 1] = memory[data] & 0xF;
		ir0[2 * rpair] = memory[data] >> 0x4;
	} else { // TODO: What happens after wrting to register > 7 in register bank 1?
		ir1[2 * rpair + 1] = memory[data] & 0xF;
		ir1[2 * rpair] = memory[data] >> 0x4;
	}
}

void src(unsigned char rpair) {
	if(ir_bank == 0) {
		ram_pointer = ir0[2 * rpair] << 4 & 0xF0 | ir0[2 * rpair + 1] & 0xF;
	} else { // TODO: What happens after reading from register > 7 in register bank 1?
		ram_pointer = ir1[2 * rpair] << 4 & 0xF0 | ir1[2 * rpair + 1] & 0xF;
	}
}

void runInstr(int opcode) {
	switch(opcode) {
		case 0x00: // NOP
			printf("NOP\n");
			break;
		case 0x01: // HLT
			printf("HLT\n");
			
			printf("\tHalting (and interrupting) CPU is not yet implemented!\n");
			unimplemented();
			
			break;
		case 0x02: // BBS
			printf("BBS\n");
			
			printf("\tReturn from subroutines and restoring SRC is not yet implemented!\n");
			unimplemented();
		
			break;
		case 0x03: // LCR
			printf("LCR\n");
			
			acc = 0x08; //TODO: This is temporary unless instr is implemented correctly.
			printf("\tData RAM and ROM bank status loading into ACC is not yet fully implemented!\n");
			unimplemented();
		
			break;
		case 0x04: // OR4
			printf("OR4\n");
			
			if(ir_bank == 0) {
				acc = (acc | ir0[4]) & 0xF;
			} else {
				acc = (acc | ir1[4]) & 0xF;
			}
			
			break;
		case 0x05: // OR5
			printf("OR5\n");
			
			if(ir_bank == 0) {
				acc = (acc | ir0[5]) & 0xF;
			} else {
				acc = (acc | ir1[5]) & 0xF;
			}
			
			break;
		case 0x06: // AN6
			printf("AN6\n");
			
			if(ir_bank == 0) {
				acc = (acc & ir0[6]) & 0xF;
			} else {
				acc = (acc & ir1[6]) & 0xF;
			}
			
			break;
		case 0x07: // AN7
			printf("AN7\n");
			
			if(ir_bank == 0) {
				acc = (acc & ir0[7]) & 0xF;
			} else {
				acc = (acc & ir1[7]) & 0xF;
			}
			
			break;
		case 0x08: // DB0
			printf("DB0\n");
			
			rom_bank = 0;
			
			break;
		case 0x09: // DB1
			printf("DB1\n");
			
			rom_bank = 1;
			
			break;
		case 0x0A: // IR0
			printf("IR0\n");
			
			ir_bank = 0;
			
			break;
		case 0x0B: // IR1
			printf("IR1\n");
			
			ir_bank = 1;
			
			break;
		case 0x0C: // EIN
			printf("EIN\n");
			
			printf("\tEnabling interrupt detection is not yet implemented!\n");
			unimplemented();
			
			break;
		case 0x0D: // DIN
			printf("DIN\n");
			
			printf("\tDisabling interrupt detection is not yet implemented!\n");
			unimplemented();
			
			break;
		case 0x0E: // RPM
			printf("RPM\n");
			
			printf("\tLoading accumulator from 4289-controlled program RAM is not yet implemented!\n");
			unimplemented();
			
			break;
		case 0x0F: // NOP (probably)
			printf("NOP (probably)\n");
			
			break;
		case 0x10: // JCN, CN = 0
			printf("JCN, CN=0\n");
			
			jcnUnimplemented();
			
			break;
		case 0x11: // JCN, CN = 1 also JNT
			printf("JCN, CN=1\n");
			
			jcnUnimplemented();
			
			break;
		case 0x12: // JCN, CN = 2 also JC
			printf("JCN, CN=2\n");
			
			jcnUnimplemented();
			
			break;
		case 0x13: // JCN, CN = 3
			printf("JCN, CN=3\n");
			
			jcnUnimplemented();
			
			break;
		case 0x14: // JCN, CN = 4 also JZ
			printf("JCN, CN=4\n");
			
			jcnUnimplemented();
			
			break;
		case 0x15: // JCN, CN = 5
			printf("JCN, CN=5\n");
			
			jcnUnimplemented();
			
			break;
		case 0x16: // JCN, CN = 6
			printf("JCN, CN=6\n");
			
			jcnUnimplemented();
			
			break;
		case 0x17: // JCN, CN = 7
			printf("JCN, CN=7\n");
			
			jcnUnimplemented();
			
			break;
		case 0x18: // JCN, CN = 8
			printf("JCN, CN=8\n");
			
			jcnUnimplemented();
			
			break;
		case 0x19: // JCN, CN = 9 also JT
			printf("JCN, CN=9\n");
			
			jcnUnimplemented();
			
			break;
		case 0x1A: // JCN, CN = 10 also JNC
			printf("JCN, CN=10\n");
			
			jcnUnimplemented();
			
			break;
		case 0x1B: // JCN, CN = 11
			printf("JCN, CN=11\n");
			
			jcnUnimplemented();
			
			break;
		case 0x1C: // JCN, CN = 12 also JNZ
			printf("JCN, CN=12\n");
			
			jcnUnimplemented();
			
			break;
		case 0x1D: // JCN, CN = 13
			printf("JCN, CN=13\n");
			
			jcnUnimplemented();
			
			break;
		case 0x1E: // JCN, CN = 14
			printf("JCN, CN=14\n");
			
			jcnUnimplemented();
			
			break;
		case 0x1F: // JCN, CN = 15
			printf("JCN, CN=15\n");
			
			jcnUnimplemented();
			
			break;
		case 0x20: // FIM 0
			printf("FIM 0");
			
			fim(0, memory[pc_stack[0] + 1]);
			
			break;
		case 0x21: // SRC 0
			printf("SRC 0");
			
			src(0);
			
			break;
		case 0x22: // FIM 2
			printf("FIM 2");
			
			fim(1, memory[pc_stack[0] + 1]);
			
			break;
		case 0x23: // SRC 2
			printf("SRC 2");
			
			src(1);
			
			break;
		case 0x24: // FIM 4
			printf("FIM 4");
			
			fim(2, memory[pc_stack[0] + 1]);
			
			break;
		case 0x25: // SRC 4
			printf("SRC 4");
			
			src(2);
			
			break;
		case 0x26: // FIM 6
			printf("FIM 6");
			
			fim(3, memory[pc_stack[0] + 1]);
			
			break;
		case 0x27: // SRC 6
			printf("SRC 6");
			
			src(3);
			
			break;
		case 0x28: // FIM 8
			printf("FIM 8");
			
			fim(4, memory[pc_stack[0] + 1]);
			
			break;
		case 0x29: // SRC 8
			printf("SRC 8");
			
			src(4);
			
			break;
		case 0x2A: // FIM 10
			printf("FIM 10");
			
			fim(5, memory[pc_stack[0] + 1]);
			
			break;
		case 0x2B: // SRC 10
			printf("SRC 10");
			
			src(5);
			
			break;
		case 0x2C: // FIM 12
			printf("FIM 12");
			
			fim(6, memory[pc_stack[0] + 1]);
			
			break;
		case 0x2D: // SRC 12
			printf("SRC 12");
			
			src(6);
			
			break;
		case 0x2E: // FIM 14
			printf("FIM 14");
			
			fim(7, memory[pc_stack[0] + 1]);
			
			break;
		case 0x2F: // SRC 14
			printf("SRC 14");
			
			src(7);
			
			break;
	}
}

void step() {
	printf("PC: %d\tROM bank: %d\tIR bank: %d\tRAM pointer: %-8x\tACC: %-8x\n\n", pc_stack[0], rom_bank, ir_bank, ram_pointer, acc);
	
	printf("---IR0 bank---\n");
	for(int i = 0; i < 16; i++) {
		if(i < 10) {
			printf("R%d:  %-8x", i, ir0[i]);
		} else {
			printf("R%d: %-8x", i, ir0[i]);
		}
		if(i % 8 == 7) printf("\n");
	}
	
	printf("---IR1 bank---\n");
	for(int i = 0; i < 8; i++) {
		printf("R%d:  %-8x", i, ir1[i]);
		if(i % 8 == 7) printf("\n");
	}
	
	int opcode = memory[pc_stack[0]];
	
	printf("\nExecuted: ");
	
	runInstr(opcode);
	
	pc_stack[0] += cycles[opcode];
	
	printf("\n--------------------\n");
}

int main(int argc, char **argv) {
	srand(time(NULL));
	
	resetRAM();
	
	unsigned int steps = 1000; // Change if you like.
	
	while(pc_stack[0] < steps) step();
	
	return 0;
}
