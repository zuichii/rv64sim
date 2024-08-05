#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <iostream>
#include <string>
#include "memory.h"
#include "processor.h"

class decoder{
 private:
  bool debug_mode;
    
  memory* main_memory;
  processor* CPU;
  
  int* instruction_array;
  void init_instruction_array(uint64_t mem_addr);
  string get_instruction();

  uint32_t get_field_signed(int start, int end);
  uint32_t get_field_unsigned(int start, int end);

 public:
  decoder(memory* main_memory, processor* CPU, bool verbose);
  void execute_instruction(uint64_t mem_addr);


};
#endif
