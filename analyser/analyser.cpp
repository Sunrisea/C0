#include "analyser.h"

#include <climits>

namespace miniplc0 {
	std::optional<CompilationError> Analyser::Analyse() {
		auto err = analyseProgram();
		if(err.has_value())
			return err;
		else 
			return std::optional<CompilationError>();
	}

	// <C0-program> ::= 
    // {<variable-declaration>}{<function-definition>}
	std::optional<CompilationError> Analyser::analyseProgram() {
		_systable.init();
		auto next= nextToken();
		auto next2=nextToken();
		auto next3=nextToken();
		if(!next.has_value()||!next2.has_value()||!next3.has_value()){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNoMain);
		}
		unreadToken();
		unreadToken();
		unreadToken();
		if(next3.value().GetType()!=TokenType::LEFT_BRACKET){
			auto err=analyseGlobalVariable();
			if(err.has_value()){
				return err;
			}
			_startcode.swap(_nowcode);
		}
		auto err =analysefunction();
		if(err.has_value()){
			return err;
		}
		if(_systable.checkmain()==0){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNoMain);
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analyseGlobalVariable(){
		while(true){
			struct sign _sign;
			auto next=nextToken();
			auto next2=nextToken();
			auto next3=nextToken();
			if(!next.has_value()){
				return {};
			}
			if(!next2.has_value()){
				unreadToken();
				return{};
			}
			if(!next3.has_value()){
				unreadToken();
				unreadToken();
				return {};
			}
			if(next3.value().GetType()==TokenType::LEFT_BRACKET){
				unreadToken();
				unreadToken();
				unreadToken();
				return {};
			}
			unreadToken();
			unreadToken();
			if((next.value().GetType()!=TokenType::CONST&&next.value().GetType()!=TokenType::INT)){
				unreadToken();
				return {};
			}
			if(next.value().GetType()==TokenType::CONST){
				auto next=nextToken();
				if(!next.has_value()||next.value().GetType()!=TokenType::INT){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
				}
				next=nextToken();
				if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
				}
				if(_systable.isvariabledeclared(next.value().GetValueString())!=-1){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
				}
				auto name=next.value().GetValueString();
				next =nextToken();
				if(!next.has_value()||(next.value().GetType()!=TokenType::EQUAL_SIGN)){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
				}
				auto err=analyseexpression();
				if(err.has_value()){
					return err;
				}
				//插入符号表
				_sign.name=name;
				_sign.type=0;
				_sign._initialization=1;
				_systable.addvariable(_sign);
				next=nextToken();
				if(!next.has_value()||(next.value().GetType()!=TokenType::SEMICOLON&&next.value().GetType()!=TokenType::COMMA_SIGN)){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
				}
				if(next.value().GetType()==TokenType::COMMA_SIGN){
					while (true)
					{
						next=nextToken();
						if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						if(_systable.isvariabledeclared(next.value().GetValueString())!=-1){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
						}
						auto name=next.value().GetValueString();
						next =nextToken();
						if(!next.has_value()||(next.value().GetType()!=TokenType::EQUAL_SIGN)){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
						}
						auto err=analyseexpression();
						if(err.has_value()){
							return err;
						}
						//插入符号表
						_sign.name=name;
						_sign.type=0;
						_sign._initialization=1;
						_systable.addvariable(_sign);
						next=nextToken();
						if(!next.has_value()||(next.value().GetType()!=TokenType::SEMICOLON&&next.value().GetType()!=TokenType::COMMA_SIGN)){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						if(next.value().GetType()==TokenType::COMMA_SIGN){
							continue;
						}
						else break;
					}
					continue;	
				}
				else continue;
			}
			else if(next.value().GetType()==TokenType::INT){
				auto next=nextToken();
				if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
				}
				if(_systable.isvariabledeclared(next.value().GetValueString())!=-1){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
				}
				auto name=next.value().GetValueString();
				next =nextToken();
				if(!next.has_value()||(next.value().GetType()!=TokenType::EQUAL_SIGN&&next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::SEMICOLON)){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
				}
				if(next.value().GetType()==TokenType::SEMICOLON){
					_sign.name=name;
					_sign.type=1;
					_sign._initialization=0;
					_systable.addvariable(_sign);
					_nowcode.emplace_back(Operation::snew,1,0);
					continue;
				}
				else if(next.value().GetType()==TokenType::EQUAL_SIGN){
					auto err=analyseexpression();
					if(err.has_value()){
						return err;
					}
					next=nextToken();
					if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::SEMICOLON)){
						return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
					}
					_sign.name=name;
					_sign.type=1;
					_sign._initialization=1;
					_systable.addvariable(_sign);
					if(next.value().GetType()==TokenType::SEMICOLON){
						continue;
					}
					else {
						while(true){
							auto next=nextToken();
							if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
								return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
							}
							if(_systable.isvariabledeclared(next.value().GetValueString())!=-1){
								return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
							}
							auto name=next.value().GetValueString();
							next =nextToken();
							if(!next.has_value()||(next.value().GetType()!=TokenType::EQUAL_SIGN&&next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::SEMICOLON)){
								return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
							}
							if(next.value().GetType()==TokenType::SEMICOLON){
								_sign.name=name;
								_sign.type=1;
								_sign._initialization=0;
								_systable.addvariable(_sign);
								_nowcode.emplace_back(Operation::snew,1,0);
								break;
							}
							else if(next.value().GetType()==TokenType::EQUAL_SIGN){
								auto err=analyseexpression();
								if(err.has_value()){
									return err;
								}
								next=nextToken();
								if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::SEMICOLON)){
									return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
								}
								_sign.name=name;
								_sign.type=1;
								_sign._initialization=1;
								_systable.addvariable(_sign);
								if(next.value().GetType()==TokenType::SEMICOLON){
									break;
								}
								else continue;
							}
							else {
								_sign.name=name;
								_sign.type=1;
								_sign._initialization=0;
								_systable.addvariable(_sign);
								_nowcode.emplace_back(Operation::snew,1,0);
								continue;
							}
						}
						continue;
					}
				}
				else if(next.value().GetType()==TokenType::COMMA_SIGN){
					_sign.name=name;
					_sign.type=1;
					_sign._initialization=0;
					_systable.addvariable(_sign);
					_nowcode.emplace_back(Operation::snew,1,0);
					while(true){
						auto next=nextToken();
						if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						if(_systable.isvariabledeclared(next.value().GetValueString())!=-1){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
						}
						auto name=next.value().GetValueString();
						next =nextToken();
						if(!next.has_value()||(next.value().GetType()!=TokenType::EQUAL_SIGN&&next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::SEMICOLON)){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
						}
						if(next.value().GetType()==TokenType::SEMICOLON){
							_sign.name=name;
							_sign.type=1;
							_sign._initialization=0;
							_systable.addvariable(_sign);
							_nowcode.emplace_back(Operation::snew,1,0);
							break;
						}
						else if(next.value().GetType()==TokenType::EQUAL_SIGN){
							auto err=analyseexpression();
							if(err.has_value()){
								return err;
							}
							next=nextToken();
							if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::SEMICOLON)){
								return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
							}
							_sign.name=name;
							_sign.type=1;
							_sign._initialization=1;
							_systable.addvariable(_sign);
							if(next.value().GetType()==TokenType::SEMICOLON){
								break;
							}
							else continue;
						}
						else {
							_sign.name=name;
							_sign.type=1;
							_sign._initialization=0;
							_systable.addvariable(_sign);
							_nowcode.emplace_back(Operation::snew,1,0);
							continue;
						}
					}
					continue;
				}
			}
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analysefunction(){
		while(true){
			_systable._program_num++;
			_systable._indextable.push_back(_systable._index);
			struct sign _sign;
			auto next=nextToken();
			if(!next.has_value()){
				return {};
			}
			if(next.value().GetType()!=TokenType::INT&&next.value().GetType()!=TokenType::VOID){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
			}
			if(next.value().GetType()==TokenType::INT){
				_sign.backtype=1;
			}
			else _sign.backtype=0;
			next=nextToken();
			if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
			}
			auto funcname=next.value().GetValueString();
			if(_systable.isvariabledeclared(funcname)!=-1){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
			}
			_sign.name=funcname;
			_sign.type=2;
			_systable.addfunction(_sign);
			int paranum=0;
			auto err=analyseparameterclause(paranum);
			if(err.has_value()){
				return err;
			}
			_systable.editfuncitonparamNum(funcname,paranum);
			err=analysecompoundstatement(_sign.backtype);
			if(err.has_value()){
				return err;
			}
			if(_sign.backtype==1){
				_nowcode.emplace_back(Operation::ipush,0,0);
				_nowcode.emplace_back(Operation::iret,0,0);
			}
			else{
				_nowcode.emplace_back(Operation::ret,0,0);
			}
			std::vector<Instruction> _funcode(0);
			_funcode.swap(_nowcode);
			_programcode.push_back(_funcode);
		}
	}

	std::optional<CompilationError> Analyser::analyseparameterclause(int& var){
		auto next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::LEFT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		next=nextToken();
		if(!next.has_value()||(next.value().GetType()!=TokenType::RIGHT_BRACKET&&next.value().GetType()!=TokenType::CONST&&next.value().GetType()!=TokenType::INT)){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		if(next.value().GetType()==TokenType::RIGHT_BRACKET){
			return {};
		}
		if(next.value().GetType()==TokenType::CONST){
			next=nextToken();
			if(!next.has_value()||next.value().GetType()!=TokenType::INT){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
			}
			next=nextToken();
			if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
			}
			auto name=next.value().GetValueString();
			if(_systable.isvariabledeclared(name)!=-1){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
			}
			next=nextToken();
			if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::RIGHT_BRACKET)){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
			}
			struct sign _sign;
			_sign.name=name;
			_sign.type=0;
			_sign._initialization=1;
			_systable.addvariable(_sign);
			var++;
			if(next.value().GetType()==TokenType::RIGHT_BRACKET){
				return {};
			}
			else{
				while(true){
					next=nextToken();
					if(!next.has_value()||(next.value().GetType()!=TokenType::CONST&&next.value().GetType()!=TokenType::INT)){
						return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
					}
					if(next.value().GetType()==TokenType::CONST){
						next=nextToken();
						if(!next.has_value()||next.value().GetType()!=TokenType::INT){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						next=nextToken();
						if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						auto name=next.value().GetValueString();
						if(_systable.isfunctionvariabledeclared(name)!=-1){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
						}
						next=nextToken();
						if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::RIGHT_BRACKET)){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						struct sign _sign;
						_sign.name=name;
						_sign.type=0;
						_sign._initialization=1;
						_systable.addvariable(_sign);
						var++;
						if(next.value().GetType()==TokenType::RIGHT_BRACKET){
							break;
						}
						else continue;
					}
					else{
						next=nextToken();
						if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						auto name=next.value().GetValueString();
						if(_systable.isfunctionvariabledeclared(name)!=-1){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
						}
						next=nextToken();
						if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::RIGHT_BRACKET)){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						struct sign _sign;
						_sign.name=name;
						_sign.type=1;
						_sign._initialization=1;
						_systable.addvariable(_sign);
						var++;
						if(next.value().GetType()==TokenType::RIGHT_BRACKET){
							break;
						}
						else continue;
					}
				}
			}
		}
		else{
			next=nextToken();
			if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
			}
			auto name=next.value().GetValueString();
			if(_systable.isfunctionvariabledeclared(name)!=-1){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
			}
			next=nextToken();
			if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::RIGHT_BRACKET)){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
			}
			struct sign _sign;
			_sign.name=name;
			_sign.type=1;
			_sign._initialization=1;
			_systable.addvariable(_sign);
			var++;
			if(next.value().GetType()==TokenType::RIGHT_BRACKET){
				return {};
			}
			else{
				while(true){
					next=nextToken();
					if(!next.has_value()||(next.value().GetType()!=TokenType::CONST&&next.value().GetType()!=TokenType::INT)){
						return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
					}
					if(next.value().GetType()==TokenType::CONST){
						next=nextToken();
						if(!next.has_value()||next.value().GetType()!=TokenType::INT){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						next=nextToken();
						if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						auto name=next.value().GetValueString();
						if(_systable.isfunctionvariabledeclared(name)!=-1){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
						}
						next=nextToken();
						if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::RIGHT_BRACKET)){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						struct sign _sign;
						_sign.name=name;
						_sign.type=0;
						_sign._initialization=1;
						_systable.addvariable(_sign);
						var++;
						if(next.value().GetType()==TokenType::RIGHT_BRACKET){
							break;
						}
						else continue;
					}
					else{
						next=nextToken();
						if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						auto name=next.value().GetValueString();
						if(_systable.isfunctionvariabledeclared(name)!=-1){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
						}
						next=nextToken();
						if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::RIGHT_BRACKET)){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						struct sign _sign;
						_sign.name=name;
						_sign.type=1;
						_sign._initialization=1;
						_systable.addvariable(_sign);
						var++;
						if(next.value().GetType()==TokenType::RIGHT_BRACKET){
							break;
						}
						else continue;
					}
				}
			}
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analysecompoundstatement(int backtype){
		auto next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::LEFTBIG_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		auto err=analysefunctionvariable();
		if(err.has_value()){
			return err;
		}
		std::vector<std::pair<int32_t,int32_t>> breaklist,continuelist;
		err=analysestatementseq(backtype,0,breaklist,continuelist);
		if(err.has_value()){
			return err;
		}
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::RIGHTBIG_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analysestatementseq(int backtype, int flag,std::vector<std::pair<int32_t,int32_t>>& breaklist,std::vector<std::pair<int32_t,int32_t>>& continuelist){
		while(true){
			auto next=nextToken();
			if(!next.has_value()){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
			}
			if(next.value().GetType()==TokenType::RIGHTBIG_BRACKET){
				unreadToken();
				return{};
			}
			unreadToken();
			auto err=analysestatement(backtype,flag,breaklist,continuelist);
			if(err.has_value()){
				return err;
			}
		}
	}

	std::optional<CompilationError> Analyser::analysestatement(int backtype, int flag,std::vector<std::pair<int32_t,int32_t>>& breaklist,std::vector<std::pair<int32_t,int32_t>>& continuelist){
		auto next=nextToken();
		if(!next.has_value()){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
		}
		switch (next.value().GetType()){
			case TokenType::RIGHTBIG_BRACKET:{
				unreadToken();
				return {};
			}
			case TokenType::LEFTBIG_BRACKET:{
				auto err=analysestatementseq(backtype,flag,breaklist,continuelist);
				if(err.has_value()){
					return err;
				}
				next=nextToken();
				if(!next.has_value()||next.value().GetType()!=RIGHTBIG_BRACKET){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
				}
				break;
			}
			case TokenType::IF:{
				unreadToken();
				auto err=analyseconditionstatement(backtype,flag,breaklist,continuelist);
				if(err.has_value()){
					return err;
				}
				break;
			}
			case TokenType::WHILE:{
				unreadToken();
				auto err=analyseloopstatement(backtype,flag,breaklist,continuelist);
				if(err.has_value()){
					return err;
				}
				break;
			}
			case TokenType::DO:{
				unreadToken();
				auto err=analysedoloop(backtype,flag,breaklist,continuelist);
				if(err.has_value()){
					return err;
				}
				break;
			}
			case TokenType::FOR:{
				unreadToken();
				auto err=analysefor(backtype,flag,breaklist,continuelist);
				if(err.has_value()){
					return err;
				}
				break;
			}
			case TokenType::PRINT:{
				unreadToken();
				auto err=analyseprintstatement();
				if(err.has_value()){
					return err;
				}
				break;
			}
			case TokenType::SCAN:{
				unreadToken();
				auto err=analysescanstatement();
				if(err.has_value()){
					return err;
				}
				break;
			}
			case TokenType::BREAK:{
				next=nextToken();
				if(!next.has_value()||next.value().GetType()!=TokenType::SEMICOLON){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNoSemicolon);
				}
				if(flag==0){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidbreak);
				}
				int x=_nowcode.size();
				_nowcode.emplace_back(Operation::jmp,0,0);
				breaklist.emplace_back(std::make_pair((int32_t)x,(int32_t)_looplevel));
				break;
			}
			case TokenType::CONTINUE:{
				next=nextToken();
				if(!next.has_value()||next.value().GetType()!=TokenType::SEMICOLON){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNoSemicolon);
				}
				if(flag==0){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidbreak);
				}
				int x=_nowcode.size();
				_nowcode.emplace_back(Operation::jmp,0,0);
				continuelist.emplace_back(std::make_pair((int32_t)x,(int32_t)_looplevel));
				break;
			}
			case TokenType::RETURN:{
				unreadToken();
				auto err=analysejumpstatement(backtype);
				if(err.has_value()){
					return err;
				}
				break;
			}
			case TokenType::IDENTIFIER:{
				next=nextToken();
				if(!next.has_value()||(next.value().GetType()!=TokenType::LEFT_BRACKET&&next.value().GetType()!=TokenType::EQUAL_SIGN)){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
				}
				if(next.value().GetType()==TokenType::LEFT_BRACKET){
					unreadToken();
					unreadToken();
					auto err=analysefunctioncall(0,0);
					if(err.has_value()){
						return err;
					}
					next=nextToken();
					if(!next.has_value()||next.value().GetType()!=TokenType::SEMICOLON){
						return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNoSemicolon);
					}
				}
				else if(next.value().GetType()==EQUAL_SIGN){
					unreadToken();
					unreadToken();
					auto err=analyseassignment();
					if(err.has_value()){
						return err;
					}
				}
				break;
			}
			case TokenType::SEMICOLON:{
				break;
			}
			default:
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analysefunctionvariable(){
		while (true){
			struct sign _sign;
			auto next=nextToken();
			if(!next.has_value()){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
			}
			if(next.value().GetType()!=TokenType::CONST&&next.value().GetType()!=TokenType::INT){
				unreadToken();
				return {};
			}
			if(next.value().GetType()==TokenType::CONST){
				auto next=nextToken();
				if(!next.has_value()||next.value().GetType()!=TokenType::INT){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
				}
				next=nextToken();
				if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
				}
				if(_systable.isfunctionvariabledeclared(next.value().GetValueString())!=-1){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
				}
				auto name=next.value().GetValueString();
				next =nextToken();
				if(!next.has_value()||(next.value().GetType()!=TokenType::EQUAL_SIGN)){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
				}
				auto err=analyseexpression();
				if(err.has_value()){
					return err;
				}
				//插入符号表
				_sign.name=name;
				_sign.type=0;
				_sign._initialization=1;
				_systable.addvariable(_sign);
				next=nextToken();
				if(!next.has_value()||(next.value().GetType()!=TokenType::SEMICOLON&&next.value().GetType()!=TokenType::COMMA_SIGN)){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
				}
				if(next.value().GetType()==TokenType::COMMA_SIGN){
					while (true)
					{
						next=nextToken();
						if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						if(_systable.isfunctionvariabledeclared(next.value().GetValueString())!=-1){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
						}
						auto name=next.value().GetValueString();
						next =nextToken();
						if(!next.has_value()||(next.value().GetType()!=TokenType::EQUAL_SIGN)){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
						}
						auto err=analyseexpression();
						if(err.has_value()){
							return err;
						}
						//插入符号表
						_sign.name=name;
						_sign.type=0;
						_sign._initialization=1;
						_systable.addvariable(_sign);
						next=nextToken();
						if(!next.has_value()||(next.value().GetType()!=TokenType::SEMICOLON&&next.value().GetType()!=TokenType::COMMA_SIGN)){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						if(next.value().GetType()==TokenType::COMMA_SIGN){
							continue;
						}
						else break;
					}
					continue;	
				}
				else continue;
			}
			else if(next.value().GetType()==TokenType::INT){
				auto next=nextToken();
				if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
				}
				if(_systable.isfunctionvariabledeclared(next.value().GetValueString())!=-1){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
				}
				auto name=next.value().GetValueString();
				next =nextToken();
				if(!next.has_value()||(next.value().GetType()!=TokenType::EQUAL_SIGN&&next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::SEMICOLON)){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
				}
				if(next.value().GetType()==TokenType::SEMICOLON){
					_sign.name=name;
					_sign.type=1;
					_sign._initialization=0;
					_systable.addvariable(_sign);
					_nowcode.emplace_back(Operation::snew,1,0);
					continue;
				}
				else if(next.value().GetType()==TokenType::EQUAL_SIGN){
					auto err=analyseexpression();
					if(err.has_value()){
						return err;
					}
					next=nextToken();
					if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::SEMICOLON)){
						return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
					}
					_sign.name=name;
					_sign.type=1;
					_sign._initialization=1;
					_systable.addvariable(_sign);
					if(next.value().GetType()==TokenType::SEMICOLON){
						continue;
					}
					else {
						while(true){
							auto next=nextToken();
							if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
								return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
							}
							if(_systable.isfunctionvariabledeclared(next.value().GetValueString())!=-1){
								return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
							}
							auto name=next.value().GetValueString();
							next =nextToken();
							if(!next.has_value()||(next.value().GetType()!=TokenType::EQUAL_SIGN&&next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::SEMICOLON)){
								return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
							}
							if(next.value().GetType()==TokenType::SEMICOLON){
								_sign.name=name;
								_sign.type=1;
								_sign._initialization=0;
								_systable.addvariable(_sign);
								_nowcode.emplace_back(Operation::snew,1,0);
								break;
							}
							else if(next.value().GetType()==TokenType::EQUAL_SIGN){
								auto err=analyseexpression();
								if(err.has_value()){
									return err;
								}
								next=nextToken();
								if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::SEMICOLON)){
									return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
								}
								_sign.name=name;
								_sign.type=1;
								_sign._initialization=1;
								_systable.addvariable(_sign);
								if(next.value().GetType()==TokenType::SEMICOLON){
									break;
								}
								else continue;
							}
							else {
								_sign.name=name;
								_sign.type=1;
								_sign._initialization=0;
								_systable.addvariable(_sign);
								_nowcode.emplace_back(Operation::snew,1,0);
								continue;
							}
						}
						continue;
					}
				}
				else if(next.value().GetType()==TokenType::COMMA_SIGN){
					_sign.name=name;
					_sign.type=1;
					_sign._initialization=0;
					_systable.addvariable(_sign);
					_nowcode.emplace_back(Operation::snew,1,0);
					while(true){
						auto next=nextToken();
						if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
						}
						if(_systable.isfunctionvariabledeclared(next.value().GetValueString())!=-1){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrDuplicateDeclaration);
						}
						auto name=next.value().GetValueString();
						next =nextToken();
						if(!next.has_value()||(next.value().GetType()!=TokenType::EQUAL_SIGN&&next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::SEMICOLON)){
							return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
						}
						if(next.value().GetType()==TokenType::SEMICOLON){
							_sign.name=name;
							_sign.type=1;
							_sign._initialization=0;
							_systable.addvariable(_sign);
							_nowcode.emplace_back(Operation::snew,1,0);
							break;
						}
						else if(next.value().GetType()==TokenType::EQUAL_SIGN){
							auto err=analyseexpression();
							if(err.has_value()){
								return err;
							}
							next=nextToken();
							if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::SEMICOLON)){
								return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrConstantNeedValue);
							}
							_sign.name=name;
							_sign.type=1;
							_sign._initialization=1;
							_systable.addvariable(_sign);
							if(next.value().GetType()==TokenType::SEMICOLON){
								break;
							}
							else continue;
						}
						else {
							_sign.name=name;
							_sign.type=1;
							_sign._initialization=0;
							_systable.addvariable(_sign);
							_nowcode.emplace_back(Operation::snew,1,0);
							continue;
						}
					}
					continue;
				}
			}
			
		}
		

	}

	std::optional<CompilationError> Analyser::analyseexpression(){
		auto err=analyseadditiveexpression();
		if(err.has_value()){
			return err;
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analyseadditiveexpression(){
		auto err=analysemultiplicativeexpression();
		if(err.has_value()){
			return err;
		}
		while(true){
			auto next=nextToken();
			if(!next.has_value()){
				return {};
			}
			if(next.value().GetType()!=TokenType::MINUS_SIGN&&next.value().GetType()!=TokenType::PLUS_SIGN){
				unreadToken();
				return {};
			}
			if(next.value().GetType()==TokenType::MINUS_SIGN){
				auto err=analysemultiplicativeexpression();
				if(err.has_value()){
					return err;
				}
				_nowcode.emplace_back(Operation::isub,0,0);
			}
			else {
				auto err=analysemultiplicativeexpression();
				if(err.has_value()){
					return err;
				}
				_nowcode.emplace_back(Operation::iadd,0,0);
			}
			continue;
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analysemultiplicativeexpression(){
		auto err=analyseunaryexpression();
		if(err.has_value()){
			return err;
		}
		while(true){
			auto next=nextToken();
			if(!next.has_value()){
				return {};
			}
			if(next.value().GetType()!=TokenType::DIVISION_SIGN&&next.value().GetType()!=TokenType::MULTIPLICATION_SIGN){
				unreadToken();
				return{};
			}
			if(next.value().GetType()==TokenType::DIVISION_SIGN){
				auto err = analyseunaryexpression();
				if(err.has_value()){
					return err;
				}
				_nowcode.emplace_back(Operation::idiv,0,0);
			}
			else {
				auto err = analyseunaryexpression();
				if(err.has_value()){
					return err;
				}
				_nowcode.emplace_back(Operation::imul,0,0);
			}
			continue;
		}
	}

	std::optional<CompilationError> Analyser::analyseunaryexpression(){
		int prefix=1;
		auto next=nextToken();
		if(!next.has_value()){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		if(next.value().GetType()==TokenType::PLUS_SIGN){
			prefix=1;
		}
		else if(next.value().GetType()==TokenType::MINUS_SIGN){
			prefix=0;
		}
		else unreadToken();
		auto err = analyseprimaryexpression(prefix);
		if(err.has_value()){
			return err;
		}
		if(prefix==0){
			_nowcode.emplace_back(Operation::ineg,0,0);
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analyseprimaryexpression(int prefix){
		auto next=nextToken();
		if(!next.has_value()){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}

		switch (next.value().GetType())
		{
			case TokenType::LEFT_BRACKET:{
				auto err=analyseexpression();
				if(err.has_value()){
					return err;
				}
				next=nextToken();
				if(!next.has_value()||next.value().GetType()!=TokenType::RIGHT_BRACKET){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
				}
				break;
			}
			case TokenType::DECIMAL_INTEGER:
			case TokenType::HEXADECIMAL_INTEGER:{
				int64_t num;
				std::string strnum=next.value().GetValueString();
				num=std::stoll(strnum);
				if(prefix==1&&num>2147483647){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrIntegerOverflow);
				}
				_nowcode.emplace_back(Operation::ipush,(int32_t)num,0);
				break;
			}

			case TokenType::IDENTIFIER:{

				auto name=next.value().GetValueString();
				next=nextToken();
				if(next.value().GetType()==TokenType::LEFT_BRACKET){
					unreadToken();
					unreadToken();
					auto err = analysefunctioncall(1,1);
					if(err.has_value()){
						return err;
					}
					break;
				}
				unreadToken();
				auto pair=_systable.getvariable(name);
				if(pair.first==-1){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNotDeclared);
				}
				else if(pair.first==-2){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNotInitialized);
				}
				_nowcode.emplace_back(Operation::loada,pair.first,pair.second);
				_nowcode.emplace_back(Operation::iload,0,0);
				break;
			}
				
			default:
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analysefunctioncall(int flag,int ifneed){
		auto next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		auto name=next.value().GetValueString();
		auto x=_systable.getfunction(name);
		if(x.second==-1){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNotDeclared);
		}
		struct sign _sign=x.first.value();
		if(flag==1&&_sign.backtype==0){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidAssignment);
		}
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::LEFT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		int paramnum=_sign.num_parameters;
		int i=0;
		if(paramnum==0){
			next=nextToken();
			if(!next.has_value()||next.value().GetType()!=TokenType::RIGHT_BRACKET){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
			}
			_nowcode.emplace_back(Operation::call,x.second,0);
		}
		if(paramnum==1){
			auto err=analyseexpression();
			if(err.has_value()){
				return err;
			}
			next=nextToken();
			if(!next.has_value()||(next.value().GetType()!=TokenType::RIGHT_BRACKET)){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidFunctioncall);
			}
			_nowcode.emplace_back(Operation::call,x.second,0);
		}
		else{
			auto err=analyseexpression();
			if(err.has_value()){
				return err;
			}
			for(i=0;i<paramnum-1;i++){
				next=nextToken();
				if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN)){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidFunctioncall);
				}
				err=analyseexpression();
				if(err.has_value()){
					return err;
				}
			}
			next=nextToken();
			if(!next.has_value()||(next.value().GetType()!=TokenType::RIGHT_BRACKET)){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidFunctioncall);
			}
			_nowcode.emplace_back(Operation::call,x.second,0);
		}
		if(ifneed==0&&_sign.backtype==1){
			_nowcode.emplace_back(Operation::pop,0,0);
		}
		return {};
	}
	
	std::optional<CompilationError> Analyser::analyseconditionstatement(int backtype,int flag,std::vector<std::pair<int32_t,int32_t>>& breaklist,std::vector<std::pair<int32_t,int32_t>>& continuelist){
		auto next=nextToken();
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::LEFT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		std::string op="null";
		auto err = analysecondition(op);
		if(err.has_value()){
			return err;
		}
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::RIGHT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
		}
		int jumpcodeindex=_nowcode.size();
		if(op=="<"){
			_nowcode.emplace_back(Operation::jge,0,0);
		}
		else if(op=="<="){
			_nowcode.emplace_back(Operation::jge,0,0);
		}
		else if(op==">"){
			_nowcode.emplace_back(Operation::jle,0,0);
		}
		else if(op==">="){
			_nowcode.emplace_back(Operation::jl,0,0);
		}
		else if(op=="=="){
			_nowcode.emplace_back(Operation::jne,0,0);
		}
		else if(op=="!="){
			_nowcode.emplace_back(Operation::je,0,0);
		}
		else {
			_nowcode.emplace_back(Operation::je,0,0);
		}
		err=analysestatement(backtype,flag,breaklist,continuelist);
		if(err.has_value()){
			return err;
		}
		next=nextToken();
		if(!next.has_value()){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
		}
		if(next.value().GetType()!=TokenType::ELSE){
			int jumpdesindex=_nowcode.size();
			_nowcode[jumpcodeindex].changeX1(jumpdesindex);
			unreadToken();
			return{};
		}
		else{
			int jump2codeindex=_nowcode.size();
			_nowcode.emplace_back(Operation::jmp,0,0);
			int jumpdesindex=_nowcode.size();
			_nowcode[jumpcodeindex].changeX1(jumpdesindex);
			err=analysestatement(backtype,flag,breaklist,continuelist);
			if(err.has_value()){
				return err;
			}
			int jump2desindex=_nowcode.size();
			_nowcode[jump2codeindex].changeX1(jump2desindex);
			return {};
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analysecondition(std::string& op){
		auto err=analyseexpression();
		if(err.has_value()){
			return err;
		}
		auto next=nextToken();
		if(!next.has_value()){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
		}
		switch (next.value().GetType())
		{
			case TokenType::LESS_SIGN:
				op="<";
				break;
			case TokenType::LESS_EQUAL_SIGN:
				op="<=";
				break;
			case TokenType::GREATER_SIGN:
				op=">";
				break;
			case TokenType::GREATER_EQUAL_SIGN:
				op=">=";
				break;
			case TokenType::ISEQUAL_SIGN:
				op="==";
				break;
			case TokenType::NOEQUAL_SIGN:
				op="!=";
				break;
			default:{
				unreadToken();
				return {};
			}
		}
		err=analyseexpression();
		if(err.has_value()){
			return err;
		}
		_nowcode.emplace_back(Operation::isub,0,0);
		return {};
	}

	std::optional<CompilationError> Analyser::analyseloopstatement(int backtype,int flag,std::vector<std::pair<int32_t,int32_t>>& breaklist,std::vector<std::pair<int32_t,int32_t>>& continuelist){
		_looplevel++;
		auto next=nextToken();
		int whilestartindex=_nowcode.size();
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::LEFT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		std::string op="null";
		auto err = analysecondition(op);
		if(err.has_value()){
			return err;
		}
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::RIGHT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
		}
		int jumpcodeindex=_nowcode.size();
		if(op=="<"){
			_nowcode.emplace_back(Operation::jge,0,0);
		}
		else if(op=="<="){
			_nowcode.emplace_back(Operation::jge,0,0);
		}
		else if(op==">"){
			_nowcode.emplace_back(Operation::jle,0,0);
		}
		else if(op==">="){
			_nowcode.emplace_back(Operation::jl,0,0);
		}
		else if(op=="=="){
			_nowcode.emplace_back(Operation::jne,0,0);
		}
		else if(op=="!="){
			_nowcode.emplace_back(Operation::je,0,0);
		}
		else {
			_nowcode.emplace_back(Operation::je,0,0);
		}
		flag=1;
		err=analysestatement(backtype,flag,breaklist,continuelist);
		if(err.has_value()){
			return err;
		}
		int continuedesindex=_nowcode.size();
		_nowcode.emplace_back(Operation::jmp,whilestartindex,0);
		int whiledesindex=_nowcode.size();
		_nowcode[jumpcodeindex].changeX1(whiledesindex);
		unsigned long long int i;
		for(i=0;i<breaklist.size();i++){
			if(breaklist[i].second==_looplevel){
				_nowcode[breaklist[i].first].changeX1(whiledesindex);
			}
		}
		for(i=0;i<continuelist.size();i++){
			if(continuelist[i].second==_looplevel){
				_nowcode[continuelist[i].first].changeX1(continuedesindex);
			}
		}
		_looplevel--;
		return {};
	}

	std::optional<CompilationError> Analyser::analysefor(int backtype,int flag,std::vector<std::pair<int32_t,int32_t>>& breaklist,std::vector<std::pair<int32_t,int32_t>>& continuelist){
		_looplevel++;
		auto next=nextToken();
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::LEFT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		next=nextToken();
		if(!next.has_value()){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidfor);
		}
		if(next.value().GetType()!=TokenType::SEMICOLON){
			unreadToken();
			auto err=analyseforinit();
			if(err.has_value()){
				return err;
			}
		}
		int forstartindex=_nowcode.size();
		int jumpcodeindex=-1;
		next=nextToken();
		if(!next.has_value()){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidfor);
		}
		if(next.value().GetType()!=TokenType::SEMICOLON){
			unreadToken();
			std::string op="null";
			auto err=analysecondition(op);
			if(err.has_value()){
				return err;
			}
			jumpcodeindex=_nowcode.size();
			if(op=="<"){
				_nowcode.emplace_back(Operation::jge,0,0);
			}
			else if(op=="<="){
				_nowcode.emplace_back(Operation::jge,0,0);
			}
			else if(op==">"){
				_nowcode.emplace_back(Operation::jle,0,0);
			}
			else if(op==">="){
				_nowcode.emplace_back(Operation::jl,0,0);
			}
			else if(op=="=="){
				_nowcode.emplace_back(Operation::jne,0,0);
			}
			else if(op=="!="){
				_nowcode.emplace_back(Operation::je,0,0);
			}
			else {
				_nowcode.emplace_back(Operation::je,0,0);
			}
		}
		else{
			unreadToken();
		}
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::SEMICOLON){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNoSemicolon);
		}
		next=nextToken();
		if(!next.has_value()){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidfor);
		}
		int hasupdateflag=0;
		std::vector<Instruction> updatecode;
		if(next.value().GetType()!=TokenType::RIGHT_BRACKET){
			hasupdateflag=1;
			int updatestartindex=_nowcode.size();
			unreadToken();
			auto err=analyseforupdate();
			if(err.has_value()){
				return err;
			}
			int updateendindex=_nowcode.size();
			int i;
			for(i=updatestartindex;i<updateendindex;i++){
				updatecode.emplace_back(_nowcode[i]);
			}
			for(i=updatestartindex;i<updateendindex;i++){
				_nowcode.pop_back();
			}
		}
		else unreadToken();
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::RIGHT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
		}
		flag=1;
		auto err=analysestatement(backtype,flag,breaklist,continuelist);
		if(err.has_value()){
			return err;
		}
		int continuedesindex=_nowcode.size();
		unsigned long long int i;
		for(i=0;i<continuelist.size();i++){
			if(continuelist[i].second==_looplevel){
				_nowcode[continuelist[i].first].changeX1(continuedesindex);
			}
		}
		if(hasupdateflag==1){
			unsigned long long int i;
			for(i=0;i<updatecode.size();i++){
				_nowcode.emplace_back(updatecode[i]);
			}
		}
		_nowcode.emplace_back(Operation::jmp,forstartindex,0);
		int breakdesindex=_nowcode.size();
		if(jumpcodeindex!=-1){
			_nowcode[jumpcodeindex].changeX1(breakdesindex);
		}
		for(i=0;i<breaklist.size();i++){
			if(breaklist[i].second==_looplevel){
				_nowcode[breaklist[i].first].changeX1(breakdesindex);
			}
		}
		_looplevel--;
		return {};
	}

	std::optional<CompilationError> Analyser::analyseforupdate(){
		auto next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidfor);
		}
		next=nextToken();
		if(!next.has_value()||(next.value().GetType()!=TokenType::EQUAL_SIGN&&next.value().GetType()!=TokenType::LEFT_BRACKET)){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidfor);
		}
		if(next.value().GetType()==TokenType::EQUAL_SIGN){
			unreadToken();
			unreadToken();
			auto err=analyseassignment();
			if(err.has_value()){
				return err;
			}
		}
		else {
			unreadToken();
			unreadToken();
			auto err=analysefunctioncall(0,0);
			if(err.has_value()){
				return err;
			}
		}
		while(true){
			next=nextToken();
			if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::RIGHT_BRACKET)){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidfor);
			}
			if(next.value().GetType()==TokenType::RIGHT_BRACKET){
				unreadToken();
				return {};
			}
			if(next.value().GetType()==TokenType::COMMA_SIGN){
				next=nextToken();
				if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidfor);
				}
				next=nextToken();
				if(!next.has_value()||(next.value().GetType()!=TokenType::EQUAL_SIGN&&next.value().GetType()!=TokenType::LEFT_BRACKET)){
					return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNoSemicolon);
				}
				if(next.value().GetType()==TokenType::EQUAL_SIGN){
					unreadToken();
					unreadToken();
					auto err=analyseassignment();
					if(err.has_value()){
						return err;
					}
				}
				else {
					unreadToken();
					unreadToken();
					auto err=analysefunctioncall(0,0);
					if(err.has_value()){
						return err;
					}
				}
			}
		}
	}

	std::optional<CompilationError> Analyser::analyseforinit(){
		auto err=analyseassignment();
		if(err.has_value()){
			return err;
		}
		while(true){
			auto next=nextToken();
			if(!next.has_value()||(next.value().GetType()!=TokenType::SEMICOLON&&next.value().GetType()!=TokenType::COMMA_SIGN)){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidfor);
			}
			if(next.value().GetType()==TokenType::SEMICOLON){
				return {};
			}
			else{
				err=analyseassignment();
				if(err.has_value()){
					return err;
				}
			}
		}
	}

	std::optional<CompilationError> Analyser::analysedoloop(int backtype,int flag,std::vector<std::pair<int32_t,int32_t>>& breaklist,std::vector<std::pair<int32_t,int32_t>>& continuelist){
		_looplevel++;
		auto next=nextToken();
		int dowhilestartindex=_nowcode.size();
		flag=1;
		auto err=analysestatement(backtype,flag,breaklist,continuelist);
		if(err.has_value()){
			return err;
		}
		next = nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::WHILE){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvaliddowhile);
		}
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::LEFT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		std::string op="null";
		int doendindex=_nowcode.size();
		err = analysecondition(op);
		if(err.has_value()){
			return err;
		}
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::RIGHT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
		}
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::SEMICOLON){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNoSemicolon);
		}
		if(op=="<"){
			_nowcode.emplace_back(Operation::jl,dowhilestartindex,0);
		}
		else if(op=="<="){
			_nowcode.emplace_back(Operation::jle,dowhilestartindex,0);
		}
		else if(op==">"){
			_nowcode.emplace_back(Operation::jg,dowhilestartindex,0);
		}
		else if(op==">="){
			_nowcode.emplace_back(Operation::jge,dowhilestartindex,0);
		}
		else if(op=="=="){
			_nowcode.emplace_back(Operation::je,dowhilestartindex,0);
		}
		else if(op=="!="){
			_nowcode.emplace_back(Operation::jne,dowhilestartindex,0);
		}
		else {
			_nowcode.emplace_back(Operation::jne,dowhilestartindex,0);
		}
		int dowhileendindex=_nowcode.size();
		unsigned long long int i;
		for(i=0;i<breaklist.size();i++){
			if(breaklist[i].second==_looplevel){
				_nowcode[breaklist[i].first].changeX1(dowhileendindex);
			}
		}
		for(i=0;i<continuelist.size();i++){
			if(continuelist[i].second==_looplevel){
				_nowcode[continuelist[i].first].changeX1(doendindex);
			}
		}
		_looplevel--;
		return{};
	}

	std::optional<CompilationError> Analyser::analyseprintstatement(){
		auto next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::PRINT){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidPrint);
		}
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::LEFT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidprint);
		}
		next=nextToken();
		if(!next.has_value()){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
		}
		if(next.value().GetType()==TokenType::RIGHT_BRACKET){
			_nowcode.emplace_back(Operation::printl,0,0);
			next=nextToken();
			if(!next.has_value()||next.value().GetType()!=TokenType::SEMICOLON){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNoSemicolon);
			}
			return {};
		}
		else{
			unreadToken();
			auto err=analyseprintablelist();
			if(err.has_value()){
				return err;
			}
			next=nextToken();
			if(!next.has_value()||next.value().GetType()!=TokenType::RIGHT_BRACKET){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
			}
			next=nextToken();
			if(!next.has_value()||next.value().GetType()!=TokenType::SEMICOLON){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNoSemicolon);
			}
			_nowcode.emplace_back(Operation::printl,0,0);
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analyseprintablelist(){
		while(true){
			auto err=analyseexpression();
			if(err.has_value()){
				return err;
			}
			// auto next=nextToken();
			// if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
			// 	return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidprint);
			// }
			// auto name=next.value().GetValueString();
			// auto x=_systable.getvariable2(name);
			// if(x.second==-1){
			// 	return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNotDeclared);
			// }
			// struct sign _sign;
			// _sign=x.first.value();
			// if(_sign._initialization==0){
			// 	return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNotInitialized);
			// }
			// _nowcode.emplace_back(Operation::loada,x.second,_sign.address);
			// _nowcode.emplace_back(Operation::iload,0,0);
			_nowcode.emplace_back(Operation::iprint,0,0);
			auto next=nextToken();
			if(!next.has_value()||(next.value().GetType()!=TokenType::COMMA_SIGN&&next.value().GetType()!=TokenType::RIGHT_BRACKET)){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidprint);
			}
			if(next.value().GetType()==TokenType::COMMA_SIGN){
				_nowcode.emplace_back(Operation::bipush,32,0);
				_nowcode.emplace_back(Operation::cprint,0,0);
				continue;
			}
			else{
				unreadToken();
				return {};
			}
		}
		

		return {};
	}

	std::optional<CompilationError> Analyser::analysescanstatement(){
		auto next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::SCAN){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidInput);
		}
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::LEFT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidscan);
		}
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::Errinvalidscan);
		}
		auto name=next.value().GetValueString();
		auto x=_systable.getvariable2(name);
		if(x.second==-1){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNotDeclared);
		}
		struct sign _sign=x.first.value();
		if(_sign.type==0){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrAssignToConstant);
		}
		_nowcode.emplace_back(Operation::loada,x.second,_sign.address);
		_nowcode.emplace_back(Operation::iscan,0,0);
		_nowcode.emplace_back(Operation::istore,0,0);
		_systable.initialize(name);
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::RIGHT_BRACKET){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNorightbracket);
		}
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::SEMICOLON){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNoSemicolon);
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analysejumpstatement(int backtype){
		auto next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::RETURN){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidfunctionreturn);
		}
		if(backtype==0){
			next=nextToken();
			if(!next.has_value()||next.value().GetType()!=TokenType::SEMICOLON){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidfunctionreturn);
			}
			_nowcode.emplace_back(Operation::ret,0,0);
		}
		else {
			auto err = analyseexpression();
			if(err.has_value()){
				return err;
			}
			next=nextToken();
			if(!next.has_value()||next.value().GetType()!=TokenType::SEMICOLON){
				return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidfunctionreturn);
			}
			_nowcode.emplace_back(Operation::iret,0,0);
		}
		return {};
	}

	std::optional<CompilationError> Analyser::analyseassignment(){
		auto next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::IDENTIFIER){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidAssignment);
		}
		auto name=next.value().GetValueString();
		struct sign _sign;
		auto x=_systable.getvariable2(name);
		if(x.second==-1){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrNotDeclared);
		}
		_sign=x.first.value();
		if(_sign.type==0){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrAssignToConstant);
		}
		_nowcode.emplace_back(Operation::loada,x.second,_sign.address);
		next=nextToken();
		if(!next.has_value()||next.value().GetType()!=TokenType::EQUAL_SIGN){
			return std::make_optional<CompilationError>(_current_pos,ErrorCode::ErrInvalidAssignment);
		}
		auto err=analyseexpression();
		if(err.has_value()){
			return err;
		}
		_nowcode.emplace_back(Operation::istore,0,0);
		_systable.initialize(name);
		return {};
	}
	std::optional<Token> Analyser::nextToken() {
		if (_offset == _tokens.size())
			return {};
		// 考虑到 _tokens[0..._offset-1] 已经被分析过了
		// 所以我们选择 _tokens[0..._offset-1] 的 EndPos 作为当前位置
		_current_pos = _tokens[_offset].GetEndPos();
		return _tokens[_offset++];
	}

	void Analyser::unreadToken() {
		if (_offset == 0)
			DieAndPrint("analyser unreads token from the begining.");
		_current_pos = _tokens[_offset - 1].GetEndPos();
		_offset--;
	}

	miniplc0::symboltable Analyser::getsystable(){
		return _systable;
	}

	std::vector<Instruction> Analyser::getstartcode(){
		return _startcode;
	}

	std::vector<std::vector<Instruction>> Analyser::getprogramcode(){
		return _programcode;
	}

	
}