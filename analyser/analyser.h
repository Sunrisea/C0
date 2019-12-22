#pragma once

#include "error/error.h"
#include "instruction/instruction.h"
#include "tokenizer/token.h"
#include "symboltable/symboltable.h"

#include <vector>
#include <optional>
#include <utility>
#include <map>
#include <cstdint>
#include <cstddef> // for std::size_t

namespace miniplc0 {

	class Analyser final {
	private:
		using uint64_t = std::uint64_t;
		using int64_t = std::int64_t;
		using uint32_t = std::uint32_t;
		using int32_t = std::int32_t;
	public:
		Analyser(std::vector<Token> v)
			: _tokens(std::move(v)), _offset(0), _current_pos(0, 0),
			 _nextaddress(0),_looplevel(0),_systable(),_startcode({}),_programcode({}),_nowcode({}){}
		Analyser(Analyser&&) = delete;
		Analyser(const Analyser&) = delete;
		Analyser& operator=(Analyser) = delete;

		std::optional<CompilationError> Analyse();

		miniplc0::symboltable getsystable();
		std::vector<Instruction> getstartcode();
		std::vector<std::vector<Instruction>> getprogramcode();
	private:
		// 所有的递归子程序

	
		std::optional<CompilationError> analyseProgram();

		std::optional<CompilationError> analyseGlobalVariable();

		std::optional<CompilationError> analyseexpression();

		std::optional<CompilationError> analyseadditiveexpression();

		std::optional<CompilationError> analysemultiplicativeexpression();

		std::optional<CompilationError> analyseunaryexpression();

		std::optional<CompilationError> analyseprimaryexpression(int prefix);

		std::optional<CompilationError> analysefunctioncall(int flag,int ifneed);

		std::optional<CompilationError> analysefunctionvariable();

		std::optional<CompilationError> analysefunction();

		std::optional<CompilationError> analyseparameterclause(int& var);

		std::optional<CompilationError> analysecompoundstatement(int backtype);

		std::optional<CompilationError> analysestatementseq(int backtype, int flag,std::vector<std::pair<int32_t,int32_t>>& breaklist,std::vector<std::pair<int32_t,int32_t>>& continuelist);

		std::optional<CompilationError> analysestatement(int backtype,int flag,std::vector<std::pair<int32_t,int32_t>>& breaklist,std::vector<std::pair<int32_t,int32_t>>& continuelist);

		std::optional<CompilationError> analyseconditionstatement(int backtype,int flag,std::vector<std::pair<int32_t,int32_t>>& breaklist,std::vector<std::pair<int32_t,int32_t>>& continuelist);

		std::optional<CompilationError> analyseloopstatement(int backtype,int flag,std::vector<std::pair<int32_t,int32_t>>& breaklist,std::vector<std::pair<int32_t,int32_t>>& continuelist);

		std::optional<CompilationError> analysejumpstatement(int backtype);

		std::optional<CompilationError> analysedoloop(int backtype,int flag,std::vector<std::pair<int32_t,int32_t>>& breaklist,std::vector<std::pair<int32_t,int32_t>>& continuelist);

		std::optional<CompilationError> analysefor(int backtype,int flag,std::vector<std::pair<int32_t,int32_t>>& breaklist,std::vector<std::pair<int32_t,int32_t>>& continuelist);

		std::optional<CompilationError> analyseforinit();

		std::optional<CompilationError> analyseforupdate();

		std::optional<CompilationError> analyseprintstatement();

		std::optional<CompilationError> analyseprintablelist();

		std::optional<CompilationError> analysescanstatement();

		std::optional<CompilationError> analyseassignment();

		std::optional<CompilationError> analysecondition(std::string& op);

		
		
		std::optional<Token> nextToken();
		
		void unreadToken();


	private:
		std::vector<Token> _tokens;
		std::size_t _offset;
		std::pair<uint64_t, uint64_t> _current_pos;

		int32_t _nextaddress;
		int32_t _looplevel;

		miniplc0::symboltable _systable;
		std::vector<Instruction> _startcode;
		std::vector<std::vector<Instruction>> _programcode;
		std::vector<Instruction> _nowcode;
	};
}
