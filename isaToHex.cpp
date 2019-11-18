#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <iterator>

using namespace std;

/*///////////////////////////////////////
	Immeadiate vals must be have 4
	hex digits w/ the prefix '$0x'
	
	Memory addresses must be 4 hex
	digits w/ the prefix '0x'
	
	Registers must be capitalized
	w/ no spaces (i.e. Reg1)
	
	No commas
	
	This doesn't do many safety checks
	
	rn doesnt account for brackets
	around registers
//////////////////////////////////////*/

// Writes "0000" if the instruction doesn't have a Src/Dest
void writeZeros(ofstream& ofs)
{
	ofs << "0000 ";
}

// Takes the immeadiate value and writes it w/o the '$0x' prefix
void writeImm(string imm, ofstream& ofs)
{
	// Checking if the prefix is right
	if(imm[0] != '$') {return;}
	string immHex = imm.substr(3);
	
	ofs << immHex << " ";
}

// Takes the address (in hex) and writes it (in hex w/o the 0x prefix)
void writeMemAddr(string addr, ofstream& ofs)
{
	// Removing the "0x"
	string addrHex = addr.substr(2);
	
	ofs << addrHex << " ";
}

// Takes the string of the register and writes the register's hex code
void writeRegisterHex(string reg, ofstream& ofs)
{
	// Map for registers
	map<string, int> reg_map =
	{
		{"Reg0", 1}, {"Reg1", 2}, {"Reg2", 3}, {"Reg3", 4},
		{"Reg4", 5}, {"Reg5", 6}, {"Reg6", 7}, {"Reg7", 8},
	};
	map<string, int>::iterator itr;
	
	itr = reg_map.find(reg);
	if(itr == reg_map.end()) {reg_map.erase(itr);} // Should throw a warning
			
	int regDec = itr->second;
	ofs << hex << "000" << regDec;
	ofs << " ";
	
}

// Takes string of instruction and writes its hex code, also returns its decimal val
int writeInstructionHex(string instr, ofstream& ofs)
{
	//Map for instructions to their OP code (in decimal)
	map<string, int> inst_map = 
	{
		{"halt", 0}, {"irmov", 1}, {"rrmov", 2}, {"ldr", 3}, {"ldm", 4}, {"stm", 5},
		{"str", 6}, {"add", 7}, {"sub", 8}, {"mul", 9}, {"jmp", 10}, {"jz", 11}
	};
	map<string, int>::iterator itr;
	
	itr = inst_map.find(instr);
	if(itr == inst_map.end()) {inst_map.erase(itr);} // Should throw an warning
	
	int instDec = itr->second;
	
	switch(instDec)
	{
		case 0:
			writeZeros(ofs);
			break;
		case 1:
			ofs << "000";
			ofs << hex << 1;
			break;
		case 2:
		case 7:
		case 8:
		case 9:
			ofs << "000";
			ofs << hex << 2;
			break;
		case 3:
		case 6:
			ofs << "000";
			ofs << hex << 3;
			break;
		case 4:
		case 5:
			ofs << hex << "000" << 5;
			break;
		case 10:
		case 11:
			ofs << hex << "000" << 4;
			break;
	}
	
	ofs << hex << " 000" << instDec << " ";
	
	return instDec;
	
}


// Parses the line
void parse(string line, ofstream& ofs)
{	
	istringstream iss(line);
	string word;
	stringstream ss;
	
	iss >> word;
	int instDec = writeInstructionHex(word, ofs);
	
	
	if(instDec >= 2)
	{
		iss >> word;
		
		// If Src should be a register
		if(instDec <= 9)
		{
			writeRegisterHex(word, ofs);
			
			// If Dest is a register
			if(instDec != 4 && instDec != 5)
			{
				iss >> word;
				writeRegisterHex(word, ofs);
			}
			// If Dest is an address
			else
			{
				iss >> word;
				writeMemAddr(word, ofs);
			}
		}
		// If Src is an address
		else
		{
			writeMemAddr(word, ofs);
			writeZeros(ofs);
		}
	}
	// In the case of irmov
	else if(instDec == 1)
	{
		iss >> word;
		writeImm(word, ofs);
		iss >> word;
		writeRegisterHex(word, ofs);
	}
	// In the case of halt
	else if(instDec == 0)
	{
		writeZeros(ofs);
		writeZeros(ofs);
	}
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		cout << "Please enter 1 file name with the instructions." << endl;
		return 0;
	}
	
	string filename = argv[1];
	string outname = filename.substr(0, filename.size() - 4);
	string line;
	ifstream myfile (filename.c_str());
	ofstream ofs (outname.c_str());
	
	ofs << "v2.0 raw" << endl;
	int i = 1;
	
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			parse(line, ofs);
			if(i++ % 2 == 0) {ofs << endl;}
		}
		myfile.close();
		ofs.close();
	}

	else cout << "Unable to open file"; 

	return 0;
}