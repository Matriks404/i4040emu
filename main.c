#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned char cycles[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // NOP, HLT, BBS, LCR, OR4, OR5, AN6, AN7, DB0, DB1, IR0, IR1, EIN, DIN, RPM, NOP (probably)
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // JCN
};

unsigned int pc = 0;
unsigned char acc = 0;

unsigned int rombank = 0;
unsigned int irbank = 0;

unsigned char ir0[16] = { // These are temporary values for registers.
	0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0 // IR0-15 from IR0 bank
};

unsigned char ir1[8] = { // These are temporary values for registers.
	0x0F, 0x1F, 0x2F, 0x3F, 0x4F, 0x5F, 0x6F, 0x7F // IR0-7 from IR1 bank
};

unsigned char memory[4096];

void clearRAM() {
	for(int i = 0; i< 4096; i++) {
		memory[i] = 0;
	}
}

void resetRAM() { // This function is temporary used to randomize data in RAM to test CPU.
	for(int i = 0; i < 4096; i++) {
		memory[i] = rand() % 32;
	}
}

void unimplemented() {
	printf("\tWARNING: This instruction is not fully implemented!!!\n");
}

void jcnUnimplemented() {
	printf("\tWARNING: JCN instructions are not yet implemented!!!\n");
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
			
			acc = 0x66; //TODO: This is temporary unless instr is implemented correctly.
			printf("\tData RAM and ROM bank status loading into ACC is not yet fully implemented!\n");
			unimplemented();
		
			break;
		case 0x04: // OR4
			printf("OR4\n");
			
			if(irbank == 0) {
				acc = acc | ir0[4];
			} else {
				acc = acc | ir1[4];
			}
			
			break;
		case 0x05: // OR5
			printf("OR5\n");
			
			if(irbank == 0) {
				acc = acc | ir0[5];
			} else {
				acc = acc | ir1[5];
			}
			
			break;
		case 0x06: // AN6
			printf("AN6\n");
			
			if(irbank == 0) {
				acc = acc & ir0[6];
			} else {
				acc = acc & ir1[6];
			}
			
			break;
		case 0x07: // AN7
			printf("AN7\n");
			
			if(irbank == 0) {
				acc = acc & ir0[7];
			} else {
				acc = acc & ir1[7];
			}
			
			break;
		case 0x08: // DB0
			printf("DB0\n");
			
			rombank = 0;
			
			break;
		case 0x09: // DB1
			printf("DB1\n");
			
			rombank = 1;
			
			break;
		case 0x0A: // IR0
			printf("IR0\n");
			
			irbank = 0;
			
			break;
		case 0x0B: // IR1
			printf("IR1\n");
			
			irbank = 1;
			
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
	}
}

void runCPU() {
	while(pc < 50) {
		printf("PC: %d\tROM bank: %d\tIR bank: %d\tACC: %-8x\n\n", pc, rombank, irbank, acc);
		
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
		
		int opcode = memory[pc];
		
		printf("\nExecuted: ");
		
		runInstr(opcode);
		
		pc += cycles[opcode];
		
		printf("\n--------------------\n");
	}
}

int main(int argc, char **argv) {
	srand(time(NULL));
	
	resetRAM();
	runCPU();
	
	return 0;
}
