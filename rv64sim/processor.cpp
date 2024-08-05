/* ****************************************************************
   RISC-V Instruction Set Simulator
   Computer Architecture, Semester 1, 2024

   Class members for processor

**************************************************************** */

#include <iostream>
#include <iomanip> 
#include <stdlib.h>
#include <cmath>

#include "memory.h"
#include "processor.h"
#include "instruction.h"

using namespace std;

// Consructor
processor::processor (memory* main_memory, bool verbose, bool stage2) {

	// TODO
   debug_mode = verbose;
   
   instruction_count = 0;

   this->main_memory = main_memory;
   pc_array = new int[64];
   register_array = new int*[64];

   // init pc
   for(int i = 0; i < 64; i++) pc_array[i] = 0;
   if(debug_mode) cout << "Init PC" << endl;

   // init registers [0x to 31]
   for(int j = 0; j < 32; j++){
      register_array[j] = new int[64];
      for(int k = 0; k < 64; k++){
         register_array[j][k] = 0;
      }
   }
   if(debug_mode) cout << "Init registers" << endl;
}

// TODO


uint64_t processor::get_pc(){
   ll res = 0, p = 0;
   int exp;
   for(int i = 63; i >= 0; i--){
      if(pc_array[i] == 1){
         exp = 63 - i;
         p = pow(2, exp);
         res += p;
      }
   }
   return res;
}

void processor::show_pc()
{
  cout << setw(16) << setfill('0') << right << hex << get_pc() << endl;
}

void processor::set_pc(uint64_t new_pc){
   if(debug_mode) cout << "PC pre: " << endl;
   for(int i = 0; i < 64; i++){
      if(debug_mode) cout << pc_array[i];

      pc_array[i] = 0;
   }
   int j = 63, rem;
   while(new_pc != 0){
      rem = new_pc % 2;
      pc_array[j] = rem;
      new_pc /= 2;
      j--;
   }
   if(debug_mode){
      cout << "\n PC post: " << endl;
      for(int k = 0; k < 64; k++) cout << pc_array[j];
      cout << endl;
   }
}

uint64_t processor::get_reg(unsigned int reg_num){
   ll res = 0, p = 0;
   int exp;
   for(int i = 63; i >= 0; i--){
      if(register_array[reg_num][i] == 1){
         exp = 63 - i;
         p = pow(2, exp);
         res += p;
      }
   }
   return res;
}

void processor::show_reg(unsigned int reg_num){
   cout << setw(16) << setfill('0') << right << hex << get_reg(reg_num) << endl;
}

void processor::set_reg(unsigned int reg_num, uint64_t new_value){
   // [x0] is always 0
   if(reg_num != 0){
      if(debug_mode) cout << "Register[x" << reg_num << "] pre: ";
      
      for(int i = 0; i < 64; i++){
         if(debug_mode) cout << register_array[reg_num][i];

         register_array[reg_num][i] = 0;
      }
   
      int j = 63, rem;
      while(new_value != 0){
         rem = new_value % 2;
         register_array[reg_num][j] = rem;
         new_value /= 2;
         j--;
      }
      
      if(debug_mode){
         cout << "\n Regiseter[x" << reg_num << "] post: ";
         for(int k = 0; k < 64; k++) cout << register_array[reg_num][k];
         cout << endl;
      }
   }
}

void processor::execute(unsigned int num, bool breakpoint_check, processor* CPU){
   for(unsigned int i = 0; i < num; i++){
      
      // breakpoint reached, exit
      if(breakpoint_check && get_pc() == breakpoint){
         cout << "Breakpoint reached at " << setw(16) << setfill('0') << hex << get_pc() << endl;
         break;
      }
      // main flow
      else{
         decoder* d;
         d = new decoder(main_memory, CPU, debug_mode);

         // misaligned
         if(get_pc() % 4 != 0){
            set_pc(get_pc() - 4);
            instruction_count--;
         } 
         else{
            d->execute_instruction(get_pc());
         } 
         set_pc(get_pc() + 4);
         instruction_count++;
      }
   }
}

void processor::clear_breakpoint(){
   breakpoint = 0xffffffffffffffff;
}

void processor::set_breakpoint(uint64_t address){
   breakpoint = address;
}

// Empty implementation for stage 1, required for stage 2
void processor::show_prv(){
   return;
}

// Empty implementation for stage 1, required for stage 2
void processor::set_prv(unsigned int prv_num){
   return;
}

// Empty implementation for stage 1, required for stage 2
void processor::show_csr(unsigned int csr_num){
   return;
}

// Empty implementation for stage 1, required for stage 2
void processor::set_csr(unsigned int csr_num, uint64_t new_value){
   return;
}

uint64_t processor::get_instruction_count(){
   // to do
   return instruction_count;
}

// Used for Postgraduate assignment. Undergraduate assignment can return 0.
uint64_t processor::get_cycle_count(){
   return 0;
}
