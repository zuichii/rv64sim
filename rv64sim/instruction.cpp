#include "instruction.h"
#include "processor.h"

#include <iostream> 
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <cstdio>
#include <algorithm>
#include <cmath>
using namespace std;

decoder::decoder(memory* main_memory, processor* CPU, bool verbose){
    // if(verbose) debug_mode = true;
    debug_mode = verbose;

    this->main_memory = main_memory;
    this->CPU = CPU;

    instruction_array = new int[64];
    for(int i = 0; i < 64; i++) instruction_array[i] = 0;
}

void decoder::init_instruction_array(uint64_t mem_addr){
    uint32_t data = main_memory->read_doubleword(mem_addr) & 0xffffffff;

    for(int j = 0; j < 32; j++) instruction_array[j] = 0;

    ll i = 0, rem = 0;
    while(data != 0){
        rem = data % 2;
        instruction_array[i] = rem;
        data /= 2;
        i++;
    }

    if(debug_mode){
        cout << "Memory address: " << hex << mem_addr;
        cout << "Instruction register: " << setfill('0') << setw(8) << hex << (main_memory->read_doubleword(mem_addr) & 0xffffffff) << endl;
    }
}

uint32_t decoder::get_field_signed(int start, int end){
    ll res = 0, exp = 0;

    // '+'
    if(instruction_array[end] == 0){
        for(int i = start; i < end; i++){
            if(instruction_array[i] == 1) res += pow(2, exp);
            exp++;
        }
        return res;
    }
    // '-'
    else{
        for(int j = start; j < end; j++){
            if(instruction_array[j] == 0) res += pow(2, exp);
            exp++;
        }
        return ~res;
    }
}

uint32_t decoder::get_field_unsigned(int start, int end){
    ll res = 0, exp = 0;
    for(int i = start; i <= end; i++){
        if(instruction_array[i] == 1) res += pow(2, exp);
        exp++;
    }
    return res;
}

string decoder::get_instruction(){
    string op, funct3, funct7;

    for(int i = 0; i < 7; i++){
        op += static_cast<char>(instruction_array[i] + '0');
    }
    for(int i = 12; i < 15; i++){
        funct3 += static_cast<char>(instruction_array[i] + '0');
    }
    for(int i = 25; i < 32; i++){
        funct7 += static_cast<char>(instruction_array[i] + '0');
    }

    if(debug_mode) cout << "OP: " << op << " FUNCT3: " << funct3 << " FUNCT7: " << funct7 << endl;

    if(op == "1100000" && funct3 == "000") return "lb";
    if(op == "1100000" && funct3 == "100") return "lh";
    if(op == "1100000" && funct3 == "010") return "lw";
    if(op == "1100000" && funct3 == "110") return "ld";
    if(op == "1100000" && funct3 == "001") return "lbu";
    if(op == "1100000" && funct3 == "101") return "lhu";
    if(op == "1100000" && funct3 == "011") return "lwu";

    // no operation
    if(op == "1111000" && funct3 == "000") return "fence";
    if(op == "1111000" && funct3 == "100") return "fence.i";

    if(op == "1100100" && funct3 == "000") return "addi";
    if(op == "1100100" && funct3 == "100") return "slli";
    if(op == "1100100" && funct3 == "010") return "slti";
    if(op == "1100100" && funct3 == "110") return "sltiu";
    if(op == "1100100" && funct3 == "001") return "xori";
    if(op == "1100100" && funct3 == "101" && instruction_array[30] == 0) return "srli";
    if(op == "1100100" && funct3 == "101" && instruction_array[30] == 1) return "srai";
    if(op == "1100100" && funct3 == "011") return "ori";
    if(op == "1100100" && funct3 == "111") return "andi";
    if(op == "1110100") return "auipc";
    if(op == "1101100" && funct3 == "000") return "addiw";
    if(op == "1101100" && funct3 == "100" && funct7 == "0000000") return "slliw";
    if(op == "1101100" && funct3 == "101" && instruction_array[30] == 0) return "srliw";
    if(op == "1101100" && funct3 == "101" && instruction_array[30] == 1) return "sraiw";
    if(op == "1100010" && funct3 == "000") return "sb";
    if(op == "1100010" && funct3 == "100") return "sh";
    if(op == "1100010" && funct3 == "010") return "sw";
    if(op == "1100010" && funct3 == "110") return "sd";
    if(op == "1100110" && funct3 == "000" && funct7 == "0000000") return "add";
    if(op == "1100110" && funct3 == "000" && funct7 == "0000010") return "sub";
    if(op == "1100110" && funct3 == "100" && funct7 == "0000000") return "sll";
    if(op == "1100110" && funct3 == "010" && funct7 == "0000000") return "slt";
    if(op == "1100110" && funct3 == "110" && funct7 == "0000000") return "sltu";
    if(op == "1100110" && funct3 == "001" && funct7 == "0000000") return "xor";
    if(op == "1100110" && funct3 == "101" && funct7 == "0000000") return "srl";
    if(op == "1100110" && funct3 == "101" && funct7 == "0000010") return "sra";
    if(op == "1100110" && funct3 == "011" && funct7 == "0000000") return "or";
    if(op == "1100110" && funct3 == "111" && funct7 == "0000000") return "and";
    if(op == "1110110") return "lui";
    if(op == "1101110" && funct3 == "000" && funct7 == "0000000") return "addw";
    if(op == "1101110" && funct3 == "000" && funct7 == "0000010") return "subw";
    if(op == "1101110" && funct3 == "100" && funct7 == "0000000") return "sllw";
    if(op == "1101110" && funct3 == "101" && funct7 == "0000000") return "srlw";
    if(op == "1101110" && funct3 == "101" && funct7 == "0000010") return "sraw";
    if(op == "1100011" && funct3 == "000") return "beq";
    if(op == "1100011" && funct3 == "100") return "bne";
    if(op == "1100011" && funct3 == "001") return "blt";
    if(op == "1100011" && funct3 == "101") return "bge";
    if(op == "1100011" && funct3 == "011") return "bltu";
    if(op == "1100011" && funct3 == "111") return "bgeu";
    if(op == "1110011" && funct3 == "000") return "jalr";
    if(op == "1111011") return "jal";

    // unimplemented
    if(op=="1100111" && funct3 == "000") return "ecall";
    if(op=="1100111" && funct3 == "000") return "ebreak";

    return "";
}

void decoder::execute_instruction(uint64_t mem_addr){
    init_instruction_array(mem_addr);

    string mnemonic = get_instruction();
    if(debug_mode) cout << "Instruction mnemonic: " << mnemonic << endl;

    if(mnemonic == "lb"){
        uint64_t rd, rs1, imm, address, data;
        
        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);
        
        if(instruction_array[31] != 0) imm |= 0xfffffffffffff000;
         
        address = imm + rs1;
        data = main_memory->read_unaligned(address) & 0xff;
        
        if (data >> 7 != 0) data |= 0xFFFFFFFFFFFFFF00;
        CPU->set_reg(rd, data);
        
        if(debug_mode){
            cout << "LB" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", imm: " << setfill('0') << setw(16) << hex << imm << endl;
        }
    } 
    if(mnemonic == "lh"){
        uint64_t rd, rs1, imm, address, data;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);
        
        if(instruction_array[31] != 0) imm |= 0xfffffffffffff000;
         
        address = imm + rs1;
        data = main_memory->read_unaligned(address) & 0xffff;
        if(data >> 15 != 0) data |= 0xffffffffffff0000;
        
        // if misaligned load instruction
        if(address % 2 != 0){
            CPU->set_pc(CPU->get_pc() - 4);
            CPU->instruction_count--;
        } 
        else CPU->set_reg(rd,data);

        if(debug_mode){
            cout << "LH" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", imm: " << setfill('0') << setw(16) << hex << imm
            << endl;
        }
    } 
    if(mnemonic == "lw"){
        uint64_t rd, rs1, imm, address, data;
        
        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);
        
        if(instruction_array[31] != 0) imm |= 0xfffffffffffff000;
        
        address = imm + rs1;
        data = main_memory->read_unaligned(address) & 0xffffffff;
        if(data >> 31 == 1) data |= 0xffffffff00000000;

        // misasligned load instruction
        if(address % 4 != 0){
            CPU->set_pc(CPU->get_pc() - 4);
            CPU->instruction_count--;
        }
        else CPU->set_reg(rd, data);

        if(debug_mode){
            cout << "LW" << endl;
            
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", imm: " << setfill('0') << setw(16) << hex << imm << endl;

            cout << "mem read word: address = " << setw(16) << hex << address
            << " data: " << setfill('0') << setw(16) << hex << data << endl;
        }   
    }
    if(mnemonic == "ld"){
        uint64_t rd, rs1, imm, address, data;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);
        
        if(instruction_array[31] != 0) imm |= 0xfffffffffffff000;
    
        address = imm + rs1;
        data = main_memory->read_doubleword(address); 
        
        // misaligned load instruction
        if(address % 8 != 0){
            CPU->set_pc(CPU->get_pc() - 4);
            CPU->instruction_count--;        
        }
        else CPU->set_reg(rd, data);
        
        if(debug_mode){
            cout << "LD" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", imm:  " << setfill('0') << setw(16) << hex << imm << endl;
        }
    }
    if(mnemonic == "lbu"){
        uint64_t rd, rs1, imm, address, data;
        
        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);
        
        if(instruction_array[31] != 0) imm |= 0xfffffffffffff000;
        
        address = imm + rs1;
        data = main_memory->read_unaligned(address) & 0xff;
        CPU->set_reg(rd,data);
        
        if(debug_mode){
            cout << "LBU" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", imm: " << setfill('0') << setw(16) << hex << imm << endl;

            cout << "mem read word: address = " << setw(16) << hex << address
            << " data: " << setfill('0') << setw(16) << hex << data << endl;
        }
    }
    if(mnemonic == "lhu"){
        uint64_t rd, rs1, imm, address, data;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);
        
        if(instruction_array[31] != 0) imm |= 0xfffffffffffff000;
        
        address = imm + rs1;
        data = main_memory->read_unaligned(address) & 0xffff;
        
        // misaligned
        if(address % 2 != 0){
            CPU->set_pc(CPU->get_pc() - 4);
            CPU->instruction_count--; 
        }
        else CPU->set_reg(rd, data);

        if(debug_mode){
            cout << "LHU" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", imm: " << setfill('0') << setw(16) << hex << imm << endl;

            cout << "mem read word: address = " << setw(16) << hex << address
            << " data: " << setfill('0') << setw(16) << hex << data << endl;
        }
    }
    if(mnemonic == "lwu"){
        uint64_t rd, rs1, imm, address, data;
        
        rd = get_field_unsigned(7,11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);
        
        if(instruction_array[31] != 0) imm |= 0xfffffffffffff000;
         
        address = imm + rs1;
        data = main_memory->read_unaligned(address) & 0xFFFFFFFF;
        data &= 0xFFFFFFFF;
        
        // misaligned
        if(address % 4 != 0){
            CPU->set_pc(CPU->get_pc() - 4);
            CPU->instruction_count--;      
        }
        else CPU->set_reg(rd,data);
        
        if(debug_mode){
            cout << "LWU" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", imm: " << setfill('0') << setw(16) << hex << imm << endl;
        }
    }
    if(mnemonic == "addi"){
        uint64_t rd, rs1, imm, res;
        
        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);

        if(imm >> 11 != 0) imm |= 0xfffffffffffff000;
    
        res = rs1 + imm;
        CPU->set_reg(rd, res);
    }
    if(mnemonic == "slli"){
       uint64_t rd, rs1, imm;
       
       rd = get_field_unsigned(7, 11);
       rs1= CPU->get_reg(get_field_unsigned(15, 19));
       imm = get_field_signed(20, 31);
       
       if(imm >> 11 !=0) imm = (imm | 0xfffffffffffff000);
       imm &=0x3f;
       CPU->set_reg(rd, rs1 << imm);
       
       if(debug_mode){
        cout << "SLLI" << endl;
        cout << "rd: " << dec << rd << ", rs1: ";
        cout << dec << get_field_unsigned(15, 19) << ", shift: ";
        cout << dec << imm << endl;
       }
    }
    if(mnemonic == "slti"){
        uint64_t rd;
        int64_t rs1, imm;
        
        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);

        if((int64_t)rs1 < imm) CPU->set_reg(rd, 1);
        else CPU->set_reg(rd, 0);
        
        if(debug_mode){
            cout << "SLTI" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", imm: " << setfill('0') << setw(16) << hex << imm << endl;
        }
    }
    if(mnemonic == "sltiu"){
        uint64_t rd, rs1, imm;
        
        rd = get_field_unsigned(7, 11);
        rs1= CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);

        if(rs1 < imm) CPU->set_reg(rd, 1);
        else CPU->set_reg(rd, 0);
        
        if(debug_mode){
            cout << "SLTIU" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", imm: " << setfill('0') << setw(16) << hex << imm << endl;
        }
    }
    if(mnemonic == "xori"){
        uint64_t rd, rs1, imm;
        
        rd = get_field_unsigned(7, 11);
        rs1= CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);
       
        CPU->set_reg(rd, rs1 ^ imm);
    }
    if(mnemonic == "srli"){
        uint64_t rd, rs1, imm;
        
        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);
        
        if(imm >> 11 != 0) imm = (imm | 0xfffffffffffff000);
        imm &=0x3f;
        CPU->set_reg( rd , rs1 >> imm );
       
        if(debug_mode){
            cout << "SRLI" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", shift: " << dec << imm << endl;
        }
    }
    if(mnemonic == "srai"){
        uint64_t rd, rs1, imm;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = (get_field_unsigned(20, 31) - 0x400);

        if(imm >> 11 !=0 ) imm |=  0xfffffffffffff000;
        imm &=0x3f;
        CPU->set_reg(rd, (int64_t)rs1 >> imm);
        
        if(debug_mode){
            cout << "SRAI" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", shift: " << dec << imm << endl;
        }
    }
    if(mnemonic == "ori"){
        uint64_t rd, rs1, imm;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);
        
        CPU->set_reg(rd, rs1 | imm);
    }
    if(mnemonic == "andi"){
        uint64_t rd, rs1, imm;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);
       
        CPU->set_reg(rd, rs1 & imm);
    }
    if(mnemonic == "auipc"){
        uint64_t rd, imm;

        rd = get_field_unsigned(7, 11);
        imm = get_field_signed(12, 31) << 12;
        
        if(instruction_array[31] != 0) imm |= 0xffffffff00000000;
        CPU->set_reg(rd, imm + CPU->get_pc());
        
        if(debug_mode){
            cout << "AUIPC" << endl;
            cout << "rd: " << dec << rd
            << ", imm: " << setfill('0') << setw(16) << hex << imm << endl;  
        }
    }
    if(mnemonic == "addiw"){   
        uint64_t rd, rs1, imm;
        
        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_signed(20, 31);
        
        int32_t res = (rs1 + imm) & 0xffffffff;

        if(instruction_array[31] != 0) CPU->set_reg(rd, res | 0xffffffff00000000);
        CPU->set_reg(rd, res);
        
        if(debug_mode){
            cout << "ADDIW" << endl;
            cout << "rd: " << dec << rd << ", rs1: ";
            cout << dec << get_field_unsigned(15, 19) << ", imm: ";
            cout << setfill('0') << setw(16) << hex << imm << endl;
        }
    }
    if(mnemonic == "slliw"){
        uint64_t rd, rs1, imm, temp;
       
        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_unsigned(20, 24);
        temp = (rs1 & 0xffffffff) << imm;

        if((temp & 0xffffffff) >> 31 != 0) temp |= 0xffffffff00000000;
        else temp &=0xffffffff;
        CPU->set_reg(rd, temp);
        
        if(debug_mode){
            cout << "SLLIW" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", shift: " << setfill('0') << setw(16) << hex << imm << endl;
        }
    }
    if(mnemonic == "srliw"){
        uint64_t rd, rs1, imm, temp;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_unsigned(20, 24);
        temp = (rs1 & 0xffffffff ) >> imm;

        if((temp & 0xffffffff) >> 31 != 0) temp |= 0xffffffff00000000;
        else temp &=0xffffffff;
        CPU->set_reg(rd , temp);
        
        if(debug_mode){
            cout << "SRLIW" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", shift: " << setfill('0') << setw(16) << hex << imm << endl;
        }
    }
    if(mnemonic == "sraiw"){
        uint64_t rd, rs1, imm, temp;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        imm = get_field_unsigned(20, 24);
        temp = ((int64_t)rs1 & 0xffffffff ) >> imm;

        if(((int64_t)rs1 & 0xffffffff) >> 31 != 0){
            uint64_t temp_mask = 0xffffffffffffffff << (32 - imm);
            temp |= temp_mask;
        }
        else temp = temp & 0xffffffff;

        CPU->set_reg(rd, temp);
        
        if(debug_mode){
            cout << "SRAIW" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", shift: " << setfill('0') << setw(16) << hex << imm << endl;
        }
    }
    if(mnemonic == "sb"){
        uint64_t rs1, rs2, imm4, imm11, imm, address;
        
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24))& 0xff;
        imm4 = get_field_signed(7,11);
        imm11 = get_field_signed(25,31) << 5 ;
        imm = imm4 | imm11;
        
        if(instruction_array[31] != 0) imm |= 0xfffffffffffff000;
        
        address = imm + rs1;
        int rem = address % 8 ;
        address = address - address % 8 ;
        rs2 = rs2 << rem * 8;
        main_memory->write_doubleword(address,rs2, (0xff * pow(0x100, rem))); 
        
        if (debug_mode){
            cout << "SB" << endl;

            cout << "rs1: " << dec << get_field_unsigned(15, 19)
            << ", rs2: " << dec << get_field_unsigned(20, 24)
            << ", imm: " << setfill('0') << setw(16) << hex << imm
            << " rem : " << dec << rem << endl;

            cout << "mem write word: address = " << setw(16) << hex << address
            << " data: " << setfill('0') << setw(16) << hex << rs2
            << " mask: " << setw(16) << hex << ((uint64_t)(0xff * pow(0x100, rem))) << endl;
        }   
    }
    if(mnemonic == "sh"){
        uint64_t rs1, rs2, imm4, imm11, imm, address;

        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24)) & 0xffff;
        imm4 = get_field_signed(7, 11);
        imm11 = get_field_signed(25, 31) << 5 ;
        imm = imm4 | imm11;
        
        if(instruction_array[31] != 0) imm |= 0xfffffffffffff000;
        
        address = imm + rs1;
        int rem = address % 8 ;
        rs2 = rs2 << rem * 8;

        // misaligned storei nstruciton
        if(address % 2 != 0){
            CPU->set_pc(CPU->get_pc() - 4);
            CPU->instruction_count--;  
        }
        else{
            if(rem < 7) main_memory->write_doubleword(address,rs2, (0xffff * pow(0x100,rem)));
            else{
                main_memory->write_doubleword(address, rs2 & ((8 - rem) * 0xff), 0xffffffffffffffff - rem * 0xff);
                main_memory->write_doubleword(address + 1 , rs2 & (0xffff - (8 - rem) * 0xff) >> (8* (8 - rem)), 0x00000000000000ff);
            }
        }
        
        if(debug_mode){
            cout << "SH" << endl;
            
            cout << "rs1: " << dec << get_field_unsigned(15, 19)
            << ", rs2: " << dec << get_field_unsigned(20, 24)
            << ", imm: " << setfill('0') << setw(16) << hex << imm
            << " rem : " << dec << rem << endl;

            cout << "mem write word: address = " << setw(16) << hex << address
            << " data: " << setfill('0') << setw(16) << hex << rs2
            << " mask: " << setw(16) << hex << ((uint64_t)(0xff * pow(0x100, rem))) << endl;
        }
    }
    if(mnemonic == "sw"){
        uint64_t rs1, rs2, imm4, imm11, imm, address;

        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24)) & 0xffffffff;
        imm4 = get_field_signed(7, 11);
        imm11 = get_field_signed(25, 31) << 5 ;
        imm = imm4 | imm11;
        
        if(instruction_array[31] != 0) imm |= 0xfffffffffffff000;
        
        address = imm + rs1;
        int rem = address % 8 ;
        rs2 = rs2 << rem * 8;

        // misaligneed
        if (address%4 != 0){
            CPU->set_pc(CPU->get_pc() - 4);
            CPU->instruction_count--;  
        }
        else{
            if(rem < 5) main_memory->write_doubleword(address, rs2, (0xffffffff * pow(0x100,rem)));
            else{
                main_memory->write_doubleword(address, rs2 & ((8 - rem) * 0xff), 0xffffffffffffffff - rem * 0xff);
                main_memory->write_doubleword(address + 1, rs2 & (0xffffffff - (8 - rem) * 0xff) >> (8 * (8 - rem)), 0x00000000000000ff);
            }
        }
       
        if(debug_mode){
            cout << "SW" << endl;
            cout << "rs1: " << dec << get_field_unsigned(15, 19)
            << ", rs2: " << dec << get_field_unsigned(20, 24)
            << ", imm: " << setfill('0') << setw(16) << hex << imm
            << " rem: " << dec << rem << endl;

            cout << "memory write word: address = " << setw(16) << hex << address
            << " data: " << setfill('0') << setw(16) << hex << rs2
            << " mask: " << setw(16) << hex << ((uint64_t)(0xff * pow(0x100, rem))) << endl;
        }
    }
    if(mnemonic == "sd"){
        uint64_t rs1, rs2, imm4, imm11, imm, address;

        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        imm4 = get_field_signed(7, 11);
        imm11 = get_field_signed(25, 31) << 5 ;
        imm = imm4 | imm11;
        
        if(instruction_array[31] != 0) imm |= 0xfffffffffffff000;
        
        address = imm + rs1;
        int rem = address % 8;
        
        // misaligned
        if(rem != 0){
            CPU->set_pc(CPU->get_pc() - 4);
            CPU->instruction_count--;  
        }
        else{
            if(rem == 0) main_memory->write_doubleword(address, rs2, 0xFFFFFFFFFFFFFFFFULL);
            else{
                main_memory->write_doubleword(address, rs2 & ((8-rem) * 0xff), 0xffffffffffffffff - rem * 0xff);
                main_memory->write_doubleword(address + 1,(rs2 & (0xffffffffffffffff - (8 - rem) * 0xff)) >> (8 * (8 - rem)), 0xffffffffffffffff - (8 - rem) * 0xff00000000000000);
            }
        }   
        
        if(debug_mode){
            cout << "SD" << endl;

            cout << "rs1: " << dec << get_field_unsigned(15, 19)
            << ", rs2: " << dec << get_field_unsigned(20, 24)
            << ", imm: " << setfill('0') << setw(16) << hex << imm << endl;

            cout << "memory write word: address: " << hex << address
            << " data: " << setfill('0') << setw(16) << hex << rs2 << endl;
        }
    }
    if(mnemonic == "add"){
       uint64_t rd, rs1, rs2;

       rd = get_field_unsigned(7, 11);
       rs1 = CPU->get_reg(get_field_unsigned(15, 19));
       rs2 = CPU->get_reg(get_field_unsigned(20, 24));
       CPU->set_reg(rd, rs1 + rs2);
    }
    if(mnemonic == "sub"){
       uint64_t rd, rs1, rs2;

       rd = get_field_unsigned(7, 11);
       rs1 = CPU->get_reg(get_field_unsigned(15, 19));
       rs2 = CPU->get_reg(get_field_unsigned(20, 24));
       CPU->set_reg(rd, rs1 - rs2);
    }
    if(mnemonic == "sll"){
        uint64_t rd, rs1, rs2, temp;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        temp = rs2 & 0x3f;
        
        temp = rs1 << temp;
        CPU->set_reg(rd, temp);
    }
    if(mnemonic == "slt"){
        uint64_t rd;
        int64_t rs1, rs2;
        
        rd = get_field_unsigned(7,11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
       
        if(rs1 < rs2) CPU->set_reg(rd,1);
        else CPU->set_reg(rd, 0);
    }
    if(mnemonic == "sltu"){
        uint64_t rd, rs1, rs2;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
       
        if(rs1 < rs2) CPU->set_reg(rd, 1);
        else CPU->set_reg(rd, 0);
    }
    if(mnemonic == "xor"){
        uint64_t rd, rs1, rs2;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
       
        CPU->set_reg(rd, rs1 ^ rs2);
    }
    if(mnemonic == "srl"){
        uint64_t rd, rs1, rs2, temp;

        rd = get_field_unsigned(7, 11);
        rs1= CPU->get_reg(get_field_unsigned(15, 19));
        rs2= CPU->get_reg(get_field_unsigned(20, 24));
        
        temp = rs2 & 0x3f;
        temp = rs1 >> temp;
        CPU->set_reg(rd, temp);
        
        if(debug_mode){
            cout << "SRL" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", rs2: " << dec << get_field_unsigned(20, 24) << endl;
        }
    }
    if(mnemonic == "sra"){
        uint64_t rd, rs2, temp;
        int64_t rs1;
        
        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        
        temp = rs2 & 0x3f;
        temp = rs1 >> temp;
        CPU->set_reg(rd, temp);
        
        if(debug_mode){
            cout << "SRA" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", rs2: " << dec << get_field_unsigned(20, 24) << endl;
        }
    }
    if(mnemonic == "or"){
        uint64_t rd, rs1, rs2;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        
        CPU->set_reg(rd, rs1 | rs2);
    }
    if(mnemonic == "and"){
        uint64_t rd, rs1, rs2;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        
        CPU->set_reg(rd, rs1 & rs2);
    }
    if(mnemonic == "lui"){
        uint64_t rd, imm;
        
        rd = get_field_unsigned(7, 11);
        imm = get_field_signed(12, 31) << 12;
        
        if(instruction_array[31] != 0) imm |= 0xffffffff00000000;
        
        CPU->set_reg(rd, imm);
        
        if(debug_mode){
            cout << "LUI" << endl;
            cout << "rd: " << dec << rd << ", imm: ";
            cout << setfill('0') << setw(16) << hex << imm << endl;
        } 
    }
    if(mnemonic == "addw"){
        uint64_t rd, rs1, rs2, res;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        res = rs1 + rs2;
       
        if((res & 0xffffffff) >> 31 != 0){ 
            res |= 0xffffffff00000000;
        }      
        else res &= 0xffffffff;
        
        CPU->set_reg(rd, res);
        
        if(debug_mode){
            cout << "ADDW" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", rs2: " << dec << get_field_unsigned(20, 24) << endl;
        }
    }
    if(mnemonic == "subw"){
        uint64_t rd, rs1, rs2, res;
       
        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        res = rs1 - rs2;
       
        if((res & 0xffffffff) >> 31 != 0){
            res |= 0xffffffff00000000;
        }
        else res &= 0xffffffff;
        
        CPU->set_reg(rd, res);
    }
    if(mnemonic == "sllw"){
        uint64_t rd, rs1, rs2, temp;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        temp = rs2 & 0x1f;
        temp = (rs1 & 0xffffffff) << temp;

        if((temp & 0xffffffff) >> 31 != 0){
            temp |= 0xffffffff00000000;
        }
        else temp &= 0xffffffff;

        CPU->set_reg(rd, temp);
        
        if(debug_mode){
            cout << "SLLW" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", rs2: " << dec << get_field_unsigned(20, 24) << endl;
        }
    }
    if(mnemonic == "srlw"){
        uint64_t rd, rs1, rs2, temp;

        rd = get_field_unsigned(7, 11);
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        
        temp = rs2 & 0x1f;
        temp = (rs1 & 0xffffffff) >> temp;
        
        if((temp & 0xffffffff) >> 31 != 0){
          temp |=  0xffffffff00000000;
        }
        else temp &= 0xffffffff;

        CPU->set_reg(rd, temp);
        
        if(debug_mode){
            cout << "SRLW" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", rs2: " << dec << get_field_unsigned(20, 24) << endl;
        }
    }
    if(mnemonic == "sraw"){
        uint64_t rd, rs1, rs2, temp;
        
        rd = get_field_unsigned(7,11);
        rs1 = CPU->get_reg(get_field_unsigned(15,19));
        rs2 = CPU->get_reg(get_field_unsigned(20,24));
        temp = rs2 & 0x1f;
        
        uint64_t shift, temp_2, sign;
        shift = temp;
        temp_2 = (int64_t)rs1 & 0xffffffff;
        sign = temp_2 >> 31;
        temp = temp_2 >> shift;

        if(sign != 0){
            uint64_t mask = 0xffffffffffffffff << (32 - shift);
            temp |= mask;
        }
        else temp &= 0xffffffff;
    
        CPU->set_reg(rd, temp);
       
        if(debug_mode){
            cout << "SRAW" << endl;
            cout << "rd: " << dec << rd
            << ", rs1: " << dec << get_field_unsigned(15, 19)
            << ", rs2: " << dec << get_field_unsigned(20, 24) << endl;
        }
    }
    if(mnemonic == "beq"){
        uint64_t rs1, rs2, imm10_5, imm4_1, imm_11, imm; 
        
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        imm10_5 = get_field_unsigned(25, 30) << 5;
        imm4_1 = get_field_unsigned(8,11) << 1;
        imm_11 = instruction_array[7] << 11 ;
        imm = imm10_5 | imm4_1 | imm_11;
        
        if(instruction_array[31] != 0) imm = (imm | 0xfffffffffffff000);
        if(rs1 == rs2) CPU->set_pc(CPU->get_pc() + imm - 4);        
    }
    if(mnemonic == "bne"){
        uint64_t rs1, rs2, imm10_5, imm4_1, imm_11, imm_B;     
        
        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        imm10_5 = get_field_unsigned(25, 30) << 5;
        imm4_1 = get_field_unsigned(8, 11) << 1;
        imm_11 = instruction_array[7] << 11 ;
        imm_B = imm10_5 | imm4_1 | imm_11;
        
        if(instruction_array[31] != 0) imm_B = (imm_B | 0xfffffffffffff000);
        if(rs1 != rs2) CPU->set_pc(CPU->get_pc() + imm_B - 4);
    
        if(debug_mode){
            cout << "BNE" << endl;
            cout << "rs1: " << dec << get_field_unsigned(15, 19) << ", rs2: ";
            cout << dec << get_field_unsigned(20, 24) << ", imm_B: ";
            cout << setfill('0') << setw(16) << hex << imm_B << endl;
        }     
    }
    if(mnemonic == "blt"){
        int64_t rs1, rs2;
        uint64_t imm10_5, imm4_1, imm_11, imm;

        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        imm10_5 = get_field_unsigned(25, 30) << 5;
        imm4_1 = get_field_unsigned(8, 11) << 1;
        imm_11 = instruction_array[7] << 11 ;
        imm = imm10_5 | imm4_1 | imm_11;
        
        if(instruction_array[31] != 0) imm = (imm | 0xfffffffffffff000);
        if(rs1 < rs2) CPU->set_pc(CPU->get_pc() + imm - 4);
    }
    if(mnemonic == "bge"){
        int64_t rs1, rs2;
        uint64_t imm10_5, imm4_1, imm_11, imm;

        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        imm10_5 = get_field_unsigned(25, 30) << 5;
        imm4_1 = get_field_unsigned(8, 11) << 1;
        imm_11 = instruction_array[7] << 11 ;
        imm = imm10_5 | imm4_1 | imm_11;
        
        if(instruction_array[31] != 0) imm = (imm | 0xfffffffffffff000);
        if(rs1 >= rs2) CPU->set_pc(CPU->get_pc() + imm - 4);        
    }
    if(mnemonic == "bltu"){
        uint64_t rs1, rs2, imm10_5, imm4_1, imm_11, imm;

        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        imm10_5 = get_field_unsigned(25, 30) << 5;
        imm4_1 = get_field_unsigned(8, 11) << 1;
        imm_11 = instruction_array[7] << 11 ;
        imm = imm10_5 | imm4_1 | imm_11;
        
        if(instruction_array[31] != 0) imm = (imm | 0xfffffffffffff000);
        if(rs1 < rs2) CPU->set_pc(CPU->get_pc() + imm - 4);
    }
    if(mnemonic == "bgeu"){
        uint64_t rs1, rs2, imm10_5, imm4_1, imm_11, imm;

        rs1 = CPU->get_reg(get_field_unsigned(15, 19));
        rs2 = CPU->get_reg(get_field_unsigned(20, 24));
        imm10_5 = get_field_unsigned(25, 30) << 5;
        imm4_1 = get_field_unsigned(8, 11) << 1;
        imm_11 = instruction_array[7] << 11 ;
        imm = imm10_5 | imm4_1 | imm_11;
         
        if(instruction_array[31] != 0) imm = (imm | 0xfffffffffffff000);
        if(rs1 >= rs2) CPU->set_pc(CPU->get_pc() + imm - 4);        
    }
    if(mnemonic == "jalr"){
        uint64_t rd, rs1, imm;

        rd = get_field_unsigned(7,11);
        rs1 = CPU->get_reg(get_field_unsigned(15,19));
        imm = get_field_signed(20,31);
        
        if(instruction_array[31] != 0) imm |= 0xfffffffffffff000;
    
        CPU->set_reg(rd, CPU->get_pc() + 4);
        CPU->set_pc(((rs1 + imm) & 0xfffffffffffffffe) - 4);
        
        if(debug_mode){
            cout << "JALR" << endl;
            cout << "rs1: " << dec << get_field_unsigned(15, 19) << ", rd: ";
            cout << dec << rd << ", imm: ";
            cout << setfill('0') << setw(16) << hex << imm << endl;
        }
    }
    if(mnemonic == "jal"){
        uint64_t rd, imm10, imm11, imm19, imm;

        rd = get_field_unsigned(7, 11); 
        imm10 = get_field_unsigned(21, 30) << 1;
        imm11 = instruction_array[20] << 11;
        imm19 = get_field_unsigned(12, 19) << 12;
        imm = imm19 | imm11 | imm10;
        
        if(instruction_array[31] != 0 ) imm |= 0xfffffffffff00000;
        
        CPU->set_reg(rd, CPU->get_pc() + 4);
        CPU->set_pc(CPU->get_pc() + imm - 4);
    }


    // exceptions
    if(mnemonic == "fence") // no operation
    if(mnemonic == "fence.i") // no operation
    if(mnemonic == "ecall") printf("ecall not implemented\n");
    if(mnemonic == "ebreak") printf("ebreak not implemented\n");
}
