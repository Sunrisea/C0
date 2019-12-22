#pragma once

#include <cstdint>
#include <utility>

namespace miniplc0 {

	enum Operation {
		nop = 0,
		bipush,
		ipush,
		pop = 4,
		pop2,
		popn,
		dup,
		dup2,
		loadc,
		loada,
		_new,
		snew,
		iload = 0x10,
		dload,
		aload,
		iaload = 0x18,
		daload,
		aaload,
		istore = 0x20,
		dstore,
		astore,
		iastore = 0x28,
		dastore,
		aastore,
		iadd = 0x30,
		dadd,
		isub = 0x34,
		dsub,
		imul = 0x38,
		dmul,
		idiv = 0x3c,
		ddiv,
		ineg = 0x40,
		dneg,
		icmp = 0x44,
		dcmp,
		i2d = 0x60,
		d2i,
		i2c,
		jmp = 0x70,
		je,
		jne,
		jl,
		jge,
		jg,
		jle,
		call = 0x80,
		ret = 0x88,
		iret,
		dret,
		aret,
		iprint = 0xa0,
		dprint,
		cprint,
		sprint,
		printl = 0xaf,
		iscan = 0xb0,
		dscan = 0xb1,
		cscan = 0xb2,
	};
	
	class Instruction final {
	private:
		using int32_t = std::int32_t;
	public:
		friend void swap(Instruction& lhs, Instruction& rhs);
	public:
		Instruction(Operation opr, int32_t x1,int32_t x2) : _opr(opr), _x1(x1),_x2(x2) {}
		
		Instruction() : Instruction(Operation::nop, 0,0){}
		Instruction(const Instruction& i) { _opr = i._opr; _x1 = i._x1; _x2=i._x2; }
		Instruction(Instruction&& i) :Instruction() { swap(*this, i); }
		Instruction& operator=(Instruction i) { swap(*this, i); return *this; }
		bool operator==(const Instruction& i) const { return _opr == i._opr && _x1 == i._x1&&_x2==i._x2; }

		Operation GetOperation() const { return _opr; }
		int32_t GetX1() const { return _x1; }
		int32_t GetX2() const {return _x2;}
		void changeX1(int32_t x1){
			_x1=x1;
		}
	private:
		Operation _opr;
		int32_t _x1;
		int32_t _x2;
	};

	inline void swap(Instruction& lhs, Instruction& rhs) {
		using std::swap;
		swap(lhs._opr, rhs._opr);
		swap(lhs._x1, rhs._x1);
		swap(lhs._x2,rhs._x2);
	}
}