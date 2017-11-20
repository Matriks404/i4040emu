#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define printf(fmt, ...) (0) // Uncomment this to disable printf's.

unsigned char cycles[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // NOP, HLT, BBS, LCR, OR4, OR5, AN6, AN7, DB0, DB1, IR0, IR1, EIN, DIN, RPM, NOP (probably)
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // JCN
	2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, // FIM (Even), SRC (Odd)
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // FIN (Even), JIN (Odd)
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // JUN
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // JMS
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // INC
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

unsigned int steps = 20; // Change if you like.
unsigned int current_steps = 0;

void clearRAM() {
	for(int i = 0; i< 4096; i++) {
		memory[i] = 0;
	}
}

void resetRAM() { // This function is temporary used to randomize data in RAM to test CPU.
	for(int i = 0; i < 4096; i++) {
		memory[i] = rand() % 112;
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

void src(unsigned char rpair) { // TODO: Send src to ROM and RAM at x2 and x3 time in instr cycle.
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
	pc_stack[0] = address - 0x2;
	
	// TODO: test bit?
}

void jms(unsigned char address) {
	printf("\tWARNING: JMS instruction is not yet implemented!!!");
}

void inc(unsigned char reg) {
	if(ir_bank == 0) {
		ir0[reg]++;
		
		if(ir0[reg] & 0xF0) ir0[reg] = 0;
	} else { // TODO: What happens after wrting to register > 7 in register bank 1?
		ir1[reg]++;
		
		if(ir1[reg] & 0xF0) ir1[reg] = 0;
	}
}

void runInstr(int opcode) {
	switch(opcode) {
		case 0x00:
			printf("NOP\n");
			break;
		case 0x01:
			printf("HLT\n");
			
			printf("\tHalting (and interrupting) CPU is not yet implemented!\n");
			unimplemented();
			
			break;
		case 0x02:
			printf("BBS\n");
			
			printf("\tReturn from subroutines and restoring SRC is not yet implemented!\n");
			unimplemented();
		
			break;
		case 0x03:
			printf("LCR\n");
			
			acc = 0x8; //TODO: This is temporary unless instr is implemented correctly.
			printf("\tData RAM and ROM bank status loading into ACC is not yet fully implemented!\n");
			unimplemented();
		
			break;
		case 0x04:
			printf("OR4\n");
			
			if(ir_bank == 0) {
				acc = (acc | ir0[4]) & 0xF;
			} else {
				acc = (acc | ir1[4]) & 0xF;
			}
			
			break;
		case 0x05:
			printf("OR5\n");
			
			if(ir_bank == 0) {
				acc = (acc | ir0[5]) & 0xF;
			} else {
				acc = (acc | ir1[5]) & 0xF;
			}
			
			break;
		case 0x06:
			printf("AN6\n");
			
			if(ir_bank == 0) {
				acc = (acc & ir0[6]) & 0xF;
			} else {
				acc = (acc & ir1[6]) & 0xF;
			}
			
			break;
		case 0x07:
			printf("AN7\n");
			
			if(ir_bank == 0) {
				acc = (acc & ir0[7]) & 0xF;
			} else {
				acc = (acc & ir1[7]) & 0xF;
			}
			
			break;
		case 0x08:
			printf("DB0\n");
			
			rom_bank = 0;
			
			break;
		case 0x09:
			printf("DB1\n");
			
			rom_bank = 1;
			
			break;
		case 0x0A:
			printf("IR0\n");
			
			ir_bank = 0;
			
			break;
		case 0x0B:
			printf("IR1\n");
			
			ir_bank = 1;
			
			break;
		case 0x0C:
			printf("EIN\n");
			
			printf("\tEnabling interrupt detection is not yet implemented!\n");
			unimplemented();
			
			break;
		case 0x0D:
			printf("DIN\n");
			
			printf("\tDisabling interrupt detection is not yet implemented!\n");
			unimplemented();
			
			break;
		case 0x0E:
			printf("RPM\n");
			
			printf("\tLoading accumulator from 4289-controlled program RAM is not yet implemented!\n");
			unimplemented();
			
			break;
		case 0x0F:
			printf("NOP (probably)\n");
			
			break;
		case 0x10:
			printf("JCN, CN=0\n");
			
			jcn(0);
			
			break;
		case 0x11:
			printf("JCN, CN=1\n");
			
			jcn(1);
			
			break;
		case 0x12:
			printf("JCN, CN=2\n");
			
			jcn(2);
			
			break;
		case 0x13:
			printf("JCN, CN=3\n");
			
			jcn(3);
			
			break;
		case 0x14:
			printf("JCN, CN=4\n");
			
			jcn(4);
			
			break;
		case 0x15:
			printf("JCN, CN=5\n");
			
			jcn(5);
			
			break;
		case 0x16:
			printf("JCN, CN=6\n");
			
			jcn(6);
			
			break;
		case 0x17:
			printf("JCN, CN=7\n");
			
			jcn(7);
			
			break;
		case 0x18:
			printf("JCN, CN=8\n");
			
			jcn(8);
			
			break;
		case 0x19:
			printf("JCN, CN=9\n");
			
			jcn(9);
			
			break;
		case 0x1A:
			printf("JCN, CN=10\n");
			
			jcn(10);
			
			break;
		case 0x1B:
			printf("JCN, CN=11\n");
			
			jcn(11);
			
			break;
		case 0x1C:
			printf("JCN, CN=12\n");
			
			jcn(12);
			
			break;
		case 0x1D:
			printf("JCN, CN=13\n");
			
			jcn(13);
			
			break;
		case 0x1E:
			printf("JCN, CN=14\n");
			
			jcn(14);
			
			break;
		case 0x1F:
			printf("JCN, CN=15\n");
			
			jcn(15);
			
			break;
		case 0x20:
			printf("FIM 0\n");
			
			fim(0, memory[pc_stack[0] + 1]);
			
			break;
		case 0x21:
			printf("SRC 0\n");
			
			src(0);
			
			break;
		case 0x22:
			printf("FIM 2\n");
			
			fim(1, memory[pc_stack[0] + 1]);
			
			break;
		case 0x23:
			printf("SRC 2\n");
			
			src(1);
			
			break;
		case 0x24:
			printf("FIM 4\n");
			
			fim(2, memory[pc_stack[0] + 1]);
			
			break;
		case 0x25:
			printf("SRC 4\n");
			
			src(2);
			
			break;
		case 0x26:
			printf("FIM 6\n");
			
			fim(3, memory[pc_stack[0] + 1]);
			
			break;
		case 0x27:
			printf("SRC 6\n");
			
			src(3);
			
			break;
		case 0x28:
			printf("FIM 8\n");
			
			fim(4, memory[pc_stack[0] + 1]);
			
			break;
		case 0x29:
			printf("SRC 8\n");
			
			src(4);
			
			break;
		case 0x2A:
			printf("FIM 10\n");
			
			fim(5, memory[pc_stack[0] + 1]);
			
			break;
		case 0x2B: // SRC 10
			printf("SRC 10\n");
			
			src(5);
			
			break;
		case 0x2C:
			printf("FIM 12");
			
			fim(6, memory[pc_stack[0] + 1]);
			
			break;
		case 0x2D: // SRC 12
			printf("SRC 12\n");
			
			src(6);
			
			break;
		case 0x2E:
			printf("FIM 14\n");
			
			fim(7, memory[pc_stack[0] + 1]);
			
			break;
		case 0x2F:
			printf("SRC 14\n");
			
			src(7);
			
			break;
		case 0x30:
			printf("FIN 0\n");
			
			fin(0);
			
			break;
		case 0x31:
			printf("JIN 0\n");
			
			jin(0);
			
			break;
		case 0x32:
			printf("FIN 2\n");
			
			fin(1);
			
			break;
		case 0x33:
			printf("JIN 2\n");
			
			jin(1);
			
			break;
		case 0x34:
			printf("FIN 4\n");
			
			fin(2);
			
			break;
		case 0x35:
			printf("JIN 4\n");
			
			jin(2);
			
			break;
		case 0x36:
			printf("FIN 6\n");
			
			fin(3);
			
			break;
		case 0x37:
			printf("JIN 6\n");
			
			jin(3);
			
			break;
		case 0x38:
			printf("FIN 8\n");
			
			fin(4);
			
			break;
		case 0x39:
			printf("JIN 8\n");
			
			jin(4);
			
			break;
		case 0x3A:
			printf("FIN 10\n");
			
			fin(5);
			
			break;
		case 0x3B:
			printf("JIN 10\n");
			
			jin(5);
			
			break;
		case 0x3C:
			printf("FIN 12\n");
			
			fin(6);
			
			break;
		case 0x3D:
			printf("JIN 12\n");
			
			jin(6);
			
			break;
		case 0x3E:
			printf("FIN 14\n");
			
			fin(7);
			
			break;
		case 0x3F:
			printf("JIN 14\n");
			
			jin(7);
			
			break;
		case 0x40:
			printf("JUN 0\n");
			
			jun((0x0 << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x41:
			printf("JUN 1\n");
			
			jun((0x1 << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x42:
			printf("JUN 2\n");
			
			jun((0x2 << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x43:
			printf("JUN 3\n");
			
			jun((0x3 << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x44:
			printf("JUN 4\n");
			
			jun((0x4 << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x45:
			printf("JUN 5\n");
			
			jun((0x5 << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x46:
			printf("JUN 6\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0x6 << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x47:
			printf("JUN 7\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0x7 << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x48:
			printf("JUN 8\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0x8 << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x49:
			printf("JUN 9\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0x9 << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x4A:
			printf("JUN 10\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0xA << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x4B:
			printf("JUN 11\n");
			
			jun((0xB << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x4C:
			printf("JUN 12\n");
			
			jun((0xC << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			break;
		case 0x4D:
			printf("JUN 13\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0xD << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x4E:
			printf("JUN 14\n");
			printf("data: %08x", memory[pc_stack[0] + 1]);
			jun((0xE << 8) + memory[pc_stack[0] + 1]);
			
			break;
		case 0x4F:
			printf("JUN 15\n");
			
			jun((0xF << 8) + memory[pc_stack[0] + 1]);
			printf("data: %08x", memory[pc_stack[0] + 1]);
			
			break;
		case 0x50:
			printf("JMS 0\n");
			
			jms(0);
			
			break;
		case 0x51:
			printf("JMS 1\n");
			
			jms(1);
			
			break;
		case 0x52:
			printf("JMS 2\n");
			
			jms(2);
			
			break;
		case 0x53:
			printf("JMS 3\n");
			
			jms(3);
			
			break;
		case 0x54:
			printf("JMS 4\n");
			
			jms(4);
			
			break;
		case 0x55:
			printf("JMS 5\n");
			
			jms(5);
			
			break;
		case 0x56:
			printf("JMS 6\n");
			
			jms(6);
			
			break;
		case 0x57:
			printf("JMS 7\n");
			
			jms(7);
			
			break;
		case 0x58:
			printf("JMS 8\n");
			
			jms(8);
			
			break;
		case 0x59:
			printf("JMS 9\n");
			
			jms(9);
			
			break;
		case 0x5A:
			printf("JMS 10\n");
			
			jms(10);
			
			break;
		case 0x5B:
			printf("JMS 11\n");
			
			jms(11);
			
			break;
		case 0x5C:
			printf("JMS 12\n");
			
			jms(12);
			
			break;
		case 0x5D:
			printf("JMS 13\n");
			
			jms(13);
			
			break;
		case 0x5E:
			printf("JMS 14\n");
			
			jms(14);
			
			break;
		case 0x5F:
			printf("JMS 15\n");
			
			jms(15);
			
			break;
		case 0x60:
			printf("INC 0\n");
			
			inc(0);
			
			break;
		case 0x61:
			printf("INC 1\n");
			
			inc(1);
			
			break;
		case 0x62:
			printf("INC 2\n");
			
			inc(2);
			
			break;
		case 0x63:
			printf("INC 3\n");
			
			inc(3);
			
			break;
		case 0x64:
			printf("INC 4\n");
			
			inc(4);
			
			break;
		case 0x65:
			printf("INC 5\n");
			
			inc(5);
			
			break;
		case 0x66:
			printf("INC 6\n");
			
			inc(6);
			
			break;
		case 0x67:
			printf("INC 7\n");
			
			inc(7);
			
			break;
		case 0x68:
			printf("INC 8\n");
			
			inc(8);
			
			break;
		case 0x69:
			printf("INC 9\n");
			
			inc(9);
			
			break;
		case 0x6A:
			printf("INC 10\n");
			
			inc(10);
			
			break;
		case 0x6B:
			printf("INC 11\n");
			
			inc(11);
			
			break;
		case 0x6C:
			printf("INC 12\n");
			
			inc(12);
			
			break;
		case 0x6D:
			printf("INC 13\n");
			
			inc(13);
			
			break;
		case 0x6E:
			printf("INC 14\n");
			
			inc(14);
			
			break;
		case 0x6F:
			printf("INC 15\n");
			
			inc(15);
			
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
