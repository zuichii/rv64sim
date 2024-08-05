/* ****************************************************************
   RISC-V Instruction Set Simulator
   Computer Architecture, Semester 1, 2024

   Class members for memory

**************************************************************** */

#include <iostream> 
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <cstdio>
#include <algorithm>
#include <cmath>

#include "memory.h"
using namespace std;

// Constructor
memory::memory(bool verbose) {
  // TODO: ...
  if(verbose){
    debug_mode = true;
    cout << "Now showing debug messages!" << endl;
    cout << "Memory initialised" << endl;
  } 
}

void memory::init_block(uint64_t address){
  
  // doubleword alignment
  address = (address / 8) * 8; 
  
  ull block_no = ((address / (1024/8)) * (1024/8)) / (1024/8);
  block_vec.push_back(block_no);

  ull *block = new ull[1024];
  // std::vector<ull> block(1024);
  block_ptr.push_back(block);

  if(debug_mode){
    cout << "\nInit_block\n";
    cout << "Memory address: " << address << endl << "Block number: "<< block_no << endl << "Block address: "<< &block << endl;
    cout << "\n\n";
  }
}

// Read a doubleword of data from a doubleword-aligned address.
// If the address is not a multiple of 8, it is rounded down to a multiple of 8.
uint64_t memory::read_doubleword (uint64_t address) {
  // TODO: ...
  uint64_t res = 0x0000000000000000ULL;

  // word alignment
  address = (address / 4) * 4; 
  ull block_no = ((address / (1024/8)) * (1024/8)) / (1024/8);
  ull index = find(block_vec.begin(), block_vec.end(), block_no) - block_vec.begin();

  // main loop if block already initialised
  if(index < block_vec.size()){
    for(int bit = 0; bit < 64; bit++){
      if(*((block_ptr[index] + 8*(address%(1024/8)))+bit) == 1){
        ll p = pow(2,bit);
        res += p;
      }
      if(debug_mode){
        cout << "Val at bit " << bit << " is: " << *((block_ptr[index] + 8*(address%(1024/8)))+bit);
        cout << "P: " << pow(2,bit) << " res: " << res;
      }
    }
    return res;
  }
  // if block not initialised yet, call init_block and return 0;
  init_block(address);
  return 0x0000000000000000ULL;
}

// Write a doubleword of data to a doubleword-aligned address.
// If the address is not a multiple of 8, it is rounded down to a multiple of 8.
// The mask contains 1s for bytes to be updated and 0s for bytes that are to be unchanged.
void memory::write_doubleword (uint64_t address, uint64_t data, uint64_t mask) {
  // TODO: ...
  
  // doubleword alignment
  address = (address / 8) * 8;

  ull block_no = ((address / (1024/8)) * (1024/8)) / (1024/8);
  ull index = find(block_vec.begin(), block_vec.end(), block_no) - block_vec.begin();

  // main flow if block already initialised
  if(index < block_vec.size()){
    int data_array[64] = {0}, mask_array[64] = {0};
    ll rem;
    
    int i = 0;
    while(data != 0){
      rem = data % 2;
      data_array[i] = rem;
      data /= 2;
      i++;
    }

    int j = 0;
    while(mask != 0){
      rem = mask % 2;
      mask_array[j] = rem;
      mask /= 2;
      j++;
    }

    if(debug_mode){
      cout << "\n\n";
      cout << "Doubleword address: " << &*((block_ptr[index] + 8*(address%(1024/8)))) << endl;
      
      cout << "Data: " << endl;
      for(int i = 0; i < 64; i++) cout << data_array[i];
      cout << "Mask: " << endl;
      for(int j = 0; j < 64; j++) cout << mask_array[i];
      
      cout << "Pre: ";
    }

    for(int bit = 0; bit < 64; bit++){
      if(debug_mode) cout << *((block_ptr[index] + 8*(address%(1024/8))));
      
      if(mask_array[bit] == 1) *((block_ptr[index] + 8*(address%(1024/8)))+bit) = data_array[bit];
    }

    if(debug_mode){
      cout << endl << "Post: ";
      for(int k = 0; k < 64; k++) cout << *((block_ptr[index] + 8*(address%(1024/8))));
      cout << "\n\n";
    }
  }
  else{
    // if block not initialised yet, call init_block and call write_doubleword again
    init_block(address);
    write_doubleword(address, data, mask);
  }
}

// Load a hex image file and provide the start address for execution from the file in start_address.
// Return true if the file was read without error, or false otherwise.
bool memory::load_file(string file_name, uint64_t &start_address) {
  ifstream input_file(file_name);
  string input;
  unsigned int line_count = 0;
  unsigned int byte_count = 0;
  char record_start;
  char byte_string[3];
  char halfword_string[5];
  unsigned int record_length;
  unsigned int record_address;
  unsigned int record_type;
  unsigned int record_data;
  unsigned int record_checksum;
  bool end_of_file_record = false;
  uint64_t load_address;
  uint64_t load_data;
  uint64_t load_mask;
  uint64_t load_base_address = 0x0000000000000000ULL;
  start_address = 0x0000000000000000ULL;
  if (input_file.is_open()) {
    while (true) {
      line_count++;
      input_file >> record_start;
      if (record_start != ':') {
	cout << "Input line " << dec << line_count << " does not start with colon character" << endl;
	return false;
      }
      input_file.get(byte_string, 3);
      sscanf(byte_string, "%x", &record_length);
      input_file.get(halfword_string, 5);
      sscanf(halfword_string, "%x", &record_address);
      input_file.get(byte_string, 3);
      sscanf(byte_string, "%x", &record_type);
      switch (record_type) {
      case 0x00:  // Data record
	for (unsigned int i = 0; i < record_length; i++) {
	  input_file.get(byte_string, 3);
	  sscanf(byte_string, "%x", &record_data);
	  load_address = (load_base_address | (uint64_t)(record_address)) + i;
	  load_data = (uint64_t)(record_data) << ((load_address % 8) * 8);
	  load_mask = 0x00000000000000ffULL << ((load_address % 8) * 8);
	  write_doubleword(load_address & 0xfffffffffffffff8ULL, load_data, load_mask);
	  byte_count++;
	}
	break;
      case 0x01:  // End of file
	end_of_file_record = true;
	break;
      case 0x02:  // Extended segment address (set bits 19:4 of load base address)
	load_base_address = 0x0000000000000000ULL;
	for (unsigned int i = 0; i < record_length; i++) {
	  input_file.get(byte_string, 3);
	  sscanf(byte_string, "%x", &record_data);
	  load_base_address = (load_base_address << 8) | (record_data << 4);
	}
	break;
      case 0x03:  // Start segment address (ignored)
	for (unsigned int i = 0; i < record_length; i++) {
	  input_file.get(byte_string, 3);
	  sscanf(byte_string, "%x", &record_data);
	}
	break;
      case 0x04:  // Extended linear address (set upper halfword of load base address)
	load_base_address = 0x0000000000000000ULL;
	for (unsigned int i = 0; i < record_length; i++) {
	  input_file.get(byte_string, 3);
	  sscanf(byte_string, "%x", &record_data);
	  load_base_address = (load_base_address << 8) | (record_data << 16);
	}
	break;
      case 0x05:  // Start linear address (set execution start address)
	start_address = 0x0000000000000000ULL;
	for (unsigned int i = 0; i < record_length; i++) {
	  input_file.get(byte_string, 3);
	  sscanf(byte_string, "%x", &record_data);
	  start_address = (start_address << 8) | record_data;
	}
	break;
      }
      input_file.get(byte_string, 3);
      sscanf(byte_string, "%x", &record_checksum);
      input_file.ignore();
      if (end_of_file_record)
	break;
    }
    input_file.close();
    cout << dec << byte_count << " bytes loaded, start address = "
	 << setw(16) << setfill('0') << hex << start_address << endl;
    return true;
  }
  else {
    cout << "Failed to open file" << endl;
    return false;
  }
}


uint64_t memory::read_unaligned (uint64_t address) {
  // TODO: ...
  uint64_t res = 0x0000000000000000ULL;
  // no address alignment

  ull block_no = ((address / (1024/8)) * (1024/8)) / (1024/8);
  ull index = find(block_vec.begin(), block_vec.end(), block_no) - block_vec.begin();

  // main loop if block already initialised
  if(index < block_vec.size()){
    for(int bit = 0; bit < 64; bit++){
      if(*((block_ptr[index] + 8*(address%(1024/8)))+bit) == 1){
        ll p = pow(2,bit);
        res += p;
      }
      if(debug_mode){
        cout << "Val at bit " << bit << " is: " << *((block_ptr[index] + 8*(address%(1024/8)))+bit);
        cout << "P: " << pow(2,bit) << " res: " << res;
      }
    }
    return res;
  }
  // if block not initialised yet, call init_block and return 0;
  init_block(address);
  return 0x0000000000000000ULL;
}

