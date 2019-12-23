#include "argparse.hpp"
#include "fmt/core.h"

#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"
#include "fmts.hpp"

#include <iostream>
#include <fstream>

typedef int8_t  i1;
typedef int16_t i2;
typedef int32_t i4;
typedef int64_t i8;

// u2,u3,u4的内容，以大端序（big-endian）写入文件
typedef uint8_t  u1;
typedef uint16_t u2;
typedef uint32_t u4;
typedef uint64_t u8;

u4 swapInt32(u4 value)
{
	return ((value & 0x000000FF) << 24) |
	((value & 0x0000FF00) << 8) |
	((value & 0x00FF0000) >> 8) |
	((value & 0xFF000000) >> 24) ;
}

u2 swapInt16(u2 value){
	return ((value & 0x00FF)<<8)|
	((value & 0xFF00)>>8);
}

void printcode(std::ostream& output,miniplc0::Instruction code){
	std::string name;
	int x1=0,x2=0;
	switch (code.GetOperation())
	{
	case miniplc0::Operation::nop:
		name="nop";
		break;
	case miniplc0::Operation::bipush:
		x1=1;x2=0;
		name="bipush";
		break;
	case miniplc0::Operation::ipush:
		x1=4;
		name="ipush";
		break;
	case miniplc0::Operation::pop:
		name="pop";
		break;
	case miniplc0::Operation::pop2:
		name="pop2";
		break;
	case miniplc0::Operation::popn:
		x1=4;
		name="popn";
		break;
	case miniplc0::Operation::dup:
		name="dup";
		break;
	case miniplc0::Operation::dup2:
		name="dup2";
		break;
	case miniplc0::Operation::loadc:
		x1=2;
		name="loadc";
		break;
	case miniplc0::Operation::loada:
		x1=2;x2=4;
		name="loada";
		break;
	case miniplc0::Operation::_new:
		name="new";
		break;
	case miniplc0::Operation::snew:
		x1=4;
		name="snew";
		break;
	case miniplc0::Operation::iload:
		name="iload";
		break;
	case miniplc0::Operation::dload:
		name="dload";
		break;
	case miniplc0::Operation::aload:
		name="aload";
		break;
	case miniplc0::Operation::iaload:
		name="iaload";
		break;
	case miniplc0::Operation::daload:
		name="daload";
		break;
	case miniplc0::Operation::aaload:
		name="aaload";
		break;
	case miniplc0::Operation::istore:
		name="istore";
		break;
	case miniplc0::Operation::dstore:
		name="dstore";
		break;
	case miniplc0::Operation::astore:
		name="astore";
		break;
	case miniplc0::Operation::iastore:
		name="iastore";
		break;
	case miniplc0::Operation::dastore:
		name="dastore";
		break;
	case miniplc0::Operation::aastore:
		name="aastore";
		break;
	case miniplc0::Operation::iadd:
		name="iadd";
		break;
	case miniplc0::Operation::dadd:
		name="dadd";
		break;
	case miniplc0::Operation::isub:
		name="isub";
		break;
	case miniplc0::Operation::dsub:
		name="dsub";
		break;
	case miniplc0::Operation::imul:
		name="imul";
		break;
	case miniplc0::Operation::dmul:
		name="dmul";
		break;
	case miniplc0::Operation::idiv:
		name="idiv";
		break;
	case miniplc0::Operation::ddiv:
		name="ddiv";
		break;
	case miniplc0::Operation::ineg:
		name="ineg";
		break;
	case miniplc0::Operation::dneg:
		name="dneg";
		break;
	case miniplc0::Operation::icmp:
		name="icmp";
		break;
	case miniplc0::Operation::dcmp:
		name="dcmp";
		break;
	case miniplc0::Operation::i2d:
		name="i2d";
		break;
	case miniplc0::Operation::d2i:
		name="d2i";
		break;
	case miniplc0::Operation::i2c:
		name="i2c";
		break;
	case miniplc0::Operation::jmp:
		x1=2;
		name="jmp";
		break;
	case miniplc0::Operation::je:
		x1=2;
		name="je";
		break;
	case miniplc0::Operation::jne:
		x1=2;
		name="jne";
		break;
	case miniplc0::Operation::jl:
		x1=2;
		name="jl";
		break;
	case miniplc0::Operation::jge:
		x1=2;
		name="jge";
		break;
	case miniplc0::Operation::jg:
		x1=2;
		name="jg";
		break;
	case miniplc0::Operation::jle:
		x1=2;
		name="jle";
		break;
	case miniplc0::Operation::call:
		x1=2;
		name="call";
		break;
	case miniplc0::Operation::ret:
		name="ret";
		break;
	case miniplc0::Operation::iret:
		name="iret";
		break;
	case miniplc0::Operation::dret:
		name="dret";
		break;
	case miniplc0::Operation::aret:
		name="aret";
		break;
	case miniplc0::Operation::iprint:
		name="iprint";
		break;
	case miniplc0::Operation::dprint:
		name="dprint";
		break;
	case miniplc0::Operation::cprint:
		name="cprint";
		break;
	case miniplc0::Operation::sprint:
		name="sprint";
		break;
	case miniplc0::Operation::printl:
		name="printl";
		break;
	case miniplc0::Operation::iscan:
		name="iscan";
		break;
	case miniplc0::Operation::dscan:
		name="dscan";
		break;
	case miniplc0::Operation::cscan:
		name="csan";
		break;
	
	default:
		name="nullerr";
	}
	if(x1==0&&x2==0){
		output<<name<<std::endl;
	}
	else if(x1!=0&&x2==0){
		output<<name<<" "<<code.GetX1()<<" "<<std::endl;
	}
	else output<<name<<" "<<code.GetX1()<<","<<code.GetX2()<<std::endl;
	return;
}

void printcodebinary(std::ostream& output,miniplc0::Instruction code){
	std::string name;
	int x1=0,x2=0;
	switch (code.GetOperation())
	{
	case miniplc0::Operation::nop:
		name="nop";
		break;
	case miniplc0::Operation::bipush:
		x1=1;x2=0;
		name="bipush";
		break;
	case miniplc0::Operation::ipush:
		x1=4;
		name="ipush";
		break;
	case miniplc0::Operation::pop:
		name="pop";
		break;
	case miniplc0::Operation::pop2:
		name="pop2";
		break;
	case miniplc0::Operation::popn:
		x1=4;
		name="popn";
		break;
	case miniplc0::Operation::dup:
		name="dup";
		break;
	case miniplc0::Operation::dup2:
		name="dup2";
		break;
	case miniplc0::Operation::loadc:
		x1=2;
		name="loadc";
		break;
	case miniplc0::Operation::loada:
		x1=2;x2=4;
		name="loada";
		break;
	case miniplc0::Operation::_new:
		name="new";
		break;
	case miniplc0::Operation::snew:
		x1=4;
		name="snew";
		break;
	case miniplc0::Operation::iload:
		name="iload";
		break;
	case miniplc0::Operation::dload:
		name="dload";
		break;
	case miniplc0::Operation::aload:
		name="aload";
		break;
	case miniplc0::Operation::iaload:
		name="iaload";
		break;
	case miniplc0::Operation::daload:
		name="daload";
		break;
	case miniplc0::Operation::aaload:
		name="aaload";
		break;
	case miniplc0::Operation::istore:
		name="istore";
		break;
	case miniplc0::Operation::dstore:
		name="dstore";
		break;
	case miniplc0::Operation::astore:
		name="astore";
		break;
	case miniplc0::Operation::iastore:
		name="iastore";
		break;
	case miniplc0::Operation::dastore:
		name="dastore";
		break;
	case miniplc0::Operation::aastore:
		name="aastore";
		break;
	case miniplc0::Operation::iadd:
		name="iadd";
		break;
	case miniplc0::Operation::dadd:
		name="dadd";
		break;
	case miniplc0::Operation::isub:
		name="isub";
		break;
	case miniplc0::Operation::dsub:
		name="dsub";
		break;
	case miniplc0::Operation::imul:
		name="imul";
		break;
	case miniplc0::Operation::dmul:
		name="dmul";
		break;
	case miniplc0::Operation::idiv:
		name="idiv";
		break;
	case miniplc0::Operation::ddiv:
		name="ddiv";
		break;
	case miniplc0::Operation::ineg:
		name="ineg";
		break;
	case miniplc0::Operation::dneg:
		name="dneg";
		break;
	case miniplc0::Operation::icmp:
		name="icmp";
		break;
	case miniplc0::Operation::dcmp:
		name="dcmp";
		break;
	case miniplc0::Operation::i2d:
		name="i2d";
		break;
	case miniplc0::Operation::d2i:
		name="d2i";
		break;
	case miniplc0::Operation::i2c:
		name="i2c";
		break;
	case miniplc0::Operation::jmp:
		x1=2;
		name="jmp";
		break;
	case miniplc0::Operation::je:
		x1=2;
		name="je";
		break;
	case miniplc0::Operation::jne:
		x1=2;
		name="jne";
		break;
	case miniplc0::Operation::jl:
		x1=2;
		name="jl";
		break;
	case miniplc0::Operation::jge:
		x1=2;
		name="jge";
		break;
	case miniplc0::Operation::jg:
		x1=2;
		name="jg";
		break;
	case miniplc0::Operation::jle:
		x1=2;
		name="jle";
		break;
	case miniplc0::Operation::call:
		x1=2;
		name="call";
		break;
	case miniplc0::Operation::ret:
		name="ret";
		break;
	case miniplc0::Operation::iret:
		name="iret";
		break;
	case miniplc0::Operation::dret:
		name="dret";
		break;
	case miniplc0::Operation::aret:
		name="aret";
		break;
	case miniplc0::Operation::iprint:
		name="iprint";
		break;
	case miniplc0::Operation::dprint:
		name="dprint";
		break;
	case miniplc0::Operation::cprint:
		name="cprint";
		break;
	case miniplc0::Operation::sprint:
		name="sprint";
		break;
	case miniplc0::Operation::printl:
		name="printl";
		break;
	case miniplc0::Operation::iscan:
		name="iscan";
		break;
	case miniplc0::Operation::dscan:
		name="dscan";
		break;
	case miniplc0::Operation::cscan:
		name="csan";
		break;
	
	default:
		name="nullerr";
	}
	auto x=code.GetOperation();
	output.write((char *)&x,sizeof(u1));
	int op1,op2;
	op1=code.GetX1();
	op2=code.GetX2();
	if(x1==1&&x2==0){
		u1 mid;
		mid = (u1)op1;
		output.write((char *)&mid,sizeof(u1));
	}
	if(x1==2&&x2==0){
		u2 mid;
		mid=swapInt16((u2)op1);
		output.write((char *)&mid,sizeof(u2));
	}
	if(x1==4&&x2==0){
		u4 mid;
		mid = swapInt32((u4)op1);
		output.write((char *)&mid,sizeof(u4));
	}
	if(x1==0){
		return ;
	}
	if(x1==2&&x2==4){
		u2 mid1=swapInt16((u2)op1);
		u4 mid2=swapInt32((u4)op2);
		output.write((char *)&mid1,sizeof(u2));
		output.write((char *)&mid2,sizeof(u4));
	}
	return ;
}

std::vector<miniplc0::Token> _tokenize(std::istream& input) {
	miniplc0::Tokenizer tkz(input);
	auto p = tkz.AllTokens();
	if (p.second.has_value()) {
		fmt::print(stderr, "Tokenization error: {}\n", p.second.value());
		exit(2);
	}
	return p.first;
}

void Tokenize(std::istream& input, std::ostream& output) {
	auto v = _tokenize(input);
	for (auto& it : v)
		output << fmt::format("{}\n", it);
	return;
}

void Analyse(std::istream& input, std::ostream& output){
	auto tks = _tokenize(input);
	miniplc0::Analyser analyser(tks);
	auto p = analyser.Analyse();
	if (p.has_value()) {
		fmt::print(stderr, "Syntactic analysis error: {}\n", p.value());
		exit(2);
	}
	miniplc0::symboltable x=analyser.getsystable();
	auto constantable = x.getconstanttable();
	output<<".constants:"<<std::endl;
	unsigned long long int i;
	for(i=0;i<constantable.size();i++){
		output<<i<<"\t"<<"S"<<"\t"<<"\""<<constantable[i].name<<"\""<<std::endl;
	}
	auto startcode=analyser.getstartcode();
	output<<".start:"<<std::endl;
	for(i=0;i<startcode.size();i++){
		output<<i<<"\t";
		printcode(output,startcode[i]);
	}
	auto functiontable = x.getfunctiontable();
	output<<".functions:"<<std::endl;
	for(i=0;i<functiontable.size();i++){
		output<<i<<"\t"<<i<<"\t"<<functiontable[i].num_parameters<<"\t"<<"1"<<std::endl;
	}
	auto programcodes=analyser.getprogramcode();
	for(i=0;i<programcodes.size();i++){
		output<<".F"<<i<<":"<<std::endl;
		long long unsigned int j;
		for(j=0;j<programcodes[i].size();j++){
			output<<j<<"\t";
			printcode(output,programcodes[i][j]);
		}
	}
}

void Analyse2(std::istream& input, std::ostream& output){
	auto tks = _tokenize(input);
	miniplc0::Analyser analyser(tks);
	auto p = analyser.Analyse();
	if (p.has_value()) {
		fmt::print(stderr, "Syntactic analysis error: {}\n", p.value());
		exit(2);
	}
	miniplc0::symboltable x=analyser.getsystable();
	auto constantable = x.getconstanttable();
	int magic=0x43303a29;
	magic=swapInt32(magic);
	output.write((char *)&magic,sizeof(magic));
	u4 version=swapInt32(1);
	output.write((char *)&version,sizeof(version));
	u2 constants_count= swapInt16((u2)constantable.size());
	output.write((char *)&constants_count,sizeof(u2));
	unsigned long long int i;
	for(i=0;i<constantable.size();i++){
		u1 type=0;
		output.write((char*)&type,sizeof(u1));
		u2 length=swapInt16((u2)constantable[i].name.length());
		output.write((char *)&length,sizeof(u2));
		output<<constantable[i].name;
	}
	auto startcode=analyser.getstartcode();
	u2 startcode_count=swapInt16((u2)startcode.size());
	output.write((char *)&startcode_count,sizeof(u2));
	for(i=0;i<startcode.size();i++){
		printcodebinary(output,startcode[i]);
	}
	auto programcodes=analyser.getprogramcode();
	u2 program_num=swapInt16((u2)programcodes.size());
	output.write((char *)&program_num,sizeof(u2));
	auto functiontable=x.getfunctiontable();
	for(i=0;i<programcodes.size();i++){
		u2 index=(u2)i;
		index=swapInt16(index);
		output.write((char *)&index,sizeof(u2));
		u2 params_size=(u2)functiontable[i].num_parameters;
		params_size=swapInt16(params_size);
		output.write((char *)&params_size,sizeof(u2));
		u2 level=1;
		level=swapInt16(level);
		output.write((char *)&level,sizeof(u2));
		u2 instructcount=(u2)programcodes[i].size();
		instructcount=swapInt16(instructcount);
		output.write((char *)&instructcount,sizeof(u2));
		unsigned long long int j;
		for(j=0;j<programcodes[i].size();j++){
			printcodebinary(output,programcodes[i][j]);
		}

	}
}




int main(int argc, char** argv) {
	argparse::ArgumentParser program("c0");
	program.add_argument("input")
		.help("speicify the file to be compiled.");
	program.add_argument("-s")
		.default_value(false)
		.implicit_value(true)
		.help("create assemble code");
	program.add_argument("-c")
		.default_value(false)
		.implicit_value(true)
		.help("create binary file");
	program.add_argument("-o", "--output")
		.required()
		.default_value(std::string("-"))
		.help("specify the output file.");

	try {
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err) {
		fmt::print(stderr, "{}\n\n", err.what());
		std::cout<<program;
		exit(2);
	}
	miniplc0::symboltable x;

	auto input_file = program.get<std::string>("input");
	auto output_file = program.get<std::string>("--output");
	std::istream* input;
	std::ostream* output;
	std::ifstream inf;
	std::ofstream outf;
	if (input_file != "-") {
		inf.open(input_file, std::ios::in);
		if (!inf) {
			fmt::print(stderr, "Fail to open {} for reading.\n", input_file);
			exit(2);
		}
		input = &inf;
	}
	else
		input = &std::cin;
	
	if (program["-s"] == true && program["-c"] == true) {
		fmt::print(stderr, "You can only one thing at one time.");
		exit(2);
	}
	else if(program["-s"] == true){
		if (output_file != "-") {
			outf.open(output_file, std::ios::out | std::ios::trunc);
			if (!outf) {
				fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
				exit(2);
			}
			output = &outf;
		}
		else{
			outf.open("out",std::ios::out | std::ios::trunc);
			if(!outf){
				fmt::print(stderr,"Fail to open out for waiting.\n");
				exit(2);
			}
			output =&outf;
		}
		Analyse(*input, *output);
	}
	else if(program["-c"] == true){
		if (output_file != "-") {
		outf.open(output_file, std::ios::out | std::ios::trunc | std::ios::binary);
		if (!outf) {
			fmt::print(stderr, "Fail to open {} for writing.\n", output_file);
			exit(2);
		}
		output = &outf;
	}
	else{
			outf.open("out",std::ios::out | std::ios::trunc | std::ios::binary);
			if(!outf){
				fmt::print(stderr,"Fail to open out for waiting.\n");
				exit(2);
			}
			output =&outf;
		}
		Analyse2(*input, *output);
	}
	else {
		fmt::print(stderr, "You must choose -c or -s.");
		exit(2);
	}
	return 0;
}