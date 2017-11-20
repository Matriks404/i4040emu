#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define printf(fmt, ...) (0) // Uncomment this to disable printf's.

unsigned char cycles[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // NOP, HLT, BBS, LCR, OR4, OR5, AN6, AN7, DB0, DB1, IR0, IR1, EIN, DIN, RPM, NOP (probably)
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // JCN
	2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, // FIM (Even), SRC (Odd)
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // FIN
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // JIN
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // JUN
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // JMS
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

unsigned int steps = 100; // Change if you like.
unsigned int current_steps = 0;

void clearRAM() {
	for(int i = 0; i< 4096; i++) {
		memory[i] = 0;
	}
}

void resetRAM() { // This function is temporary used to randomize data in RAM to test CPU.
	for(int i = 0; i < 4096; i++) {
		memory[i] = rand() % 80; // TODO: 96
	}
}

void unimplemented() {
	printf("\tWARNING: This instruction is not fully implemented!!!\n");
}

void jcn(unsigned char condition) {
	if(condition & 0x4) {
		if((!acc) ^ (condition & 0x8)) {
			pc_stack[0] = memory[pc_stack[0] + 1];
		}
	} else if(condition & 0x2) {
		if(carry ^ (condition & 0x8)) {
			pc_stack[0] = memory[pc_stack[0] + 1];
		}
	} else if(condition & 0x1) {
		if(test ^ (condition & 0x8)) {
			pc_stack[0] = memory[pc_stack[0] + 1];
		}
	}
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

void fin(unsigned char rpair) {
	printf("\tWARNING: FIN instruction is not yet implemented!!!\n");
}

void jin(unsigned char rpair) {
	if(ir_bank == 0) {
		pc_stack[0] = (pc_stack[0] & 0xF00 | (ir0[2 * rpair] << 4 & 0xF0 | ir0[2 * rpair + 1 & 0xF])) - 1;
	} else { // TODO: What happens after reading from register > 7 in register bank 1?
		pc_stack[0] = (pc_stack[0] & 0xF00 | (ir1[2 * rpair] << 4 & 0xF0 | ir1[2 * rpair + 1 & 0xF])) - 1;
	}
	
	// TODO: test bit?
}

void jun(unsigned short address) {
	pc_stack[0] = address - 0x1;
	
	// TODO: test bit?
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
			
			acc = 0x8; //TODO: This is temporary unless instr is implemented correctly.
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
			
			jcn(0);
			
			break;
		case 0x11: // JCN, CN = 1 also JNT
			printf("JCN, CN=1\n");
			
			jcn(1);
			
			break;
		case 0x12: // JCN, CN = 2 also JC
			printf("JCN, CN=2\n");
			
			jcn(2);
			
			break;
		case 0x13: // JCN, CN = 3
			printf("JCN, CN=3\n");
			
			jcn(3);
			
			break;
		case 0x14: // JCN, CN = 4 also JZ
			printf("JCN, CN=4\n");
			
			jcn(4);
			
			break;
		case 0x15: // JCN, CN = 5
			printf("JCN, CN=5\n");
			
			jcn(5);
			
			break;
		case 0x16: // JCN, CN = 6
			printf("JCN, CN=6\n");
			
			jcn(6);
			
			break;
		case 0x17: // JCN, CN = 7
			printf("JCN, CN=7\n");
			
			jcn(7);
			
			break;
		case 0x18: // JCN, CN = 8
			printf("JCN, CN=8\n");
			
			jcn(8);
			
			break;
		case 0x19: // JCN, CN = 9 also JT
			printf("JCN, CN=9\n");
			
			jcn(9);
			
			break;
		case 0x1A: // JCN, CN = 10 also JNC
			printf("JCN, CN=10\n");
			
			jcn(10);
			
			break;
		case 0x1B: // JCN, CN = 11
			printf("JCN, CN=11\n");
			
			jcn(11);
			
			break;
		case 0x1C: // JCN, CN = 12 also JNZ
			printf("JCN, CN=12\n");
			
			jcn(12);
			
			break;
		case 0x1D: // JCN, CN = 13
			printf("JCN, CN=13\n");
			
			jcn(13);
			
			break;
		case 0x1E: // JCN, CN = 14
			printf("JCN, CN=14\n");
			
			jcn(14);
			
			break;
		case 0x1F: // JCN, CN = 15
			printf("JCN, CN=15\n");
			
			jcn(15);
			
			break;
		case 0x20: // FIM 0
			printf("FIM 0\n");
			
			fim(0, memory[pc_stack[0] + 1]);
			
			break;
		case 0x21: // SRC 0
			printf("SRC 0\n");
			
			src(0);
			
			break;
		case 0x22: // FIM 2
			printf("FIM 2\n");
			
			fim(1, memory[pc_stack[0] + 1]);
			
			break;
		case 0x23: // SRC 2
			printf("SRC 2\n");
			
			src(1);
			
			break;
		case 0x24: // FIM 4
			printf("FIM 4\n");
			
			fim(2, memory[pc_stack[0] + 1]);
			
			break;
		case 0x25: // SRC 4
			printf("SRC 4\n");
			
			src(2);
			
			break;
		case 0x26: // FIM 6
			printf("FIM 6\n");
			
			fim(3, memory[pc_stack[0] + 1]);
			
			break;
		case 0x27: // SRC 6
			printf("SRC 6\n");
			
			src(3);
			
			break;
		case 0x28: // FIM 8
			printf("FIM 8\n");
			
			fim(4, memory[pc_stack[0] + 1]);
			
			break;
		case 0x29: // SRC 8
			printf("SRC 8\n");
			
			src(4);
			
			break;
		case 0x2A: // FIM 10
			printf("FIM 10\n");
			
			fim(5, memory[pc_stack[0] + 1]);
			
			break;
		case 0x2B: // SRC 10
			printf("SRC 10\n");
			
			src(5);
			
			break;
		case 0x2C: // FIM 12
			printf("FIM 12");
			
			fim(6, memory[pc_stack[0] + 1]);
			
			break;
		case 0x2D: // SRC 12
			printf("SRC 12\n");
			
			src(6);
			
			break;
		case 0x2E: // FIM 14
			printf("FIM 14\n");
			
			fim(7, memory[pc_stack[0] + 1]);
			
			break;
		case 0x2F: // SRC 14
			printf("SRC 14\n");
			
			src(7);
			
			break;
		case 0x30: // FIN 0
			printf("FIN 0\n");
			
			fin(0);
			
			break;
		case 0x31: // JIN 0
			printf("JIN 0\n");
			
			jin(0);
			
			break;
		case 0x32: // FIN 2
			printf("FIN 2\n");
			
			fin(1);
			
			break;
		case 0x33: // JIN 2
			printf("JIN 2\n");
			
			jin(1);
			
			break;
		case 0x34: // FIN 4
			printf("FIN 4\n");
			
			fin(2);
			
			break;
		case 0x35: // JIN 4
			printf("JIN 4\n");
			
			jin(2);
			
			break;
		case 0x36: // FIN 6
			printf("FIN 6\n");
			
			fin(3);
			
			break;
		case 0x37: // JIN 6
			printf("JIN 6\n");
			
			jin(3);
			
			break;
		case 0x38: // FIN 8
			printf("FIN 8\n");
			
			fin(4);
			
			break;
		case 0x39: // JIN 8
			printf("JIN 8\n");
			
			jin(4);
			
			break;
		case 0x3A: // FIN 10
			printf("FIN 10\n");
			
			fin(5);
			
			break;
		case 0x3B: // JIN 10
			printf("JIN 10\n");
			
			jin(5);
			
			break;
		case 0x3C: // FIN 12
			printf("FIN 12\n");
			
			fin(6);
			
			break;
		case 0x3D: // JIN 12
			printf("JIN 12\n");
			
			jin(6);
			
			break;
		case 0x3E: // FIN 14
			printf("FIN 14\n");
			
			fin(7);
			
			break;
		case 0x3F: // JIN 14
			printf("JIN 14\n");
			
			jin(7);
			
			break;
		case 0x40: // JUN 0
			printf("JUN 0\n");
			
			jun((0x0 << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x41: // JUN 1
			printf("JUN 1\n");
			
			jun((0x1 << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x42: // JUN 2
			printf("JUN 2\n");
			
			jun((0x2 << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x43: // JUN 3
			printf("JUN 3\n");
			
			jun((0x3 << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x44: // JUN 4
			printf("JUN 4\n");
			
			jun((0x4 << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x45: // JUN 5
			printf("JUN 5\n");
			
			jun((0x5 << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x46: // JUN 6
			printf("JUN 6\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0x6 << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x47: // JUN 7
			printf("JUN 7\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0x7 << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x48: // JUN 8
			printf("JUN 8\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0x8 << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x49: // JUN 9
			printf("JUN 9\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0x9 << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x4A: // JUN 10
			printf("JUN 10\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0xA << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x4B: // JUN 11
			printf("JUN 11\n");
			
			jun((0xB << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x4C: // JUN 12
			printf("JUN 12\n");
			
			jun((0xC << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x4D: // JUN 13
			printf("JUN 13\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0xD << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x4E: // JUN 14
			printf("JUN 14\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0xE << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x4F: // JUN 15
			printf("JUN 15\n");
			
			jun((0xF << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			
			break;
	}
}

void step() {
	printf("PC: %08x\tROM bank: %d\tIR bank: %d\tRAM pointer: %08x\tACC: %-8x\n\n", pc_stack[0], rom_bank, ir_bank, ram_pointer, acc);
	
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
	
	unsigned int opcode = memory[pc_stack[0]];
	
	printf("\nExecuted: ");
	
	runInstr(opcode);
	
	pc_stack[0] += cycles[opcode];
	current_steps++;
	
	printf("\n--------------------\n");
}

int main(int argc, char **argv) {
	srand(time(NULL));

	resetRAM();

	while(current_steps < steps) step();
	
	return 0;
}
