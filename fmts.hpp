#include "fmt/core.h"
#include "tokenizer/tokenizer.h"
#include "analyser/analyser.h"

namespace fmt {
	template<>
	struct formatter<miniplc0::ErrorCode> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::ErrorCode &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case miniplc0::ErrNoError:
				name = "No error.";
				break;
			case miniplc0::ErrStreamError:
				name = "Stream error.";
				break;
			case miniplc0::ErrEOF:
				name = "EOF";
				break;
			case miniplc0::ErrInvalidInput:
				name = "The input is invalid.";
				break;
			case miniplc0::ErrInvalidIdentifier:
				name = "Identifier is invalid";
				break;
			case miniplc0::ErrIntegerOverflow:
				name = "The integer is too big(int64_t).";
				break;
			case miniplc0::ErrNoBegin:
				name = "The program should start with 'begin'.";
				break;
			case miniplc0::ErrNoEnd:
				name = "The program should end with 'end'.";
				break;
			case miniplc0::ErrNeedIdentifier:
				name = "Need an identifier here.";
				break;
			case miniplc0::ErrConstantNeedValue:
				name = "The constant need a value to initialize.";
				break;
			case miniplc0::ErrNoSemicolon:
				name = "Zai? Wei shen me bu xie fen hao.";
				break;
			case miniplc0::ErrInvalidVariableDeclaration:
				name = "The declaration is invalid.";
				break;
			case miniplc0::ErrIncompleteExpression:
				name = "The expression is incomplete.";
				break;
			case miniplc0::ErrNotDeclared:
				name = "The variable or constant must be declared before being used.";
				break;
			case miniplc0::ErrAssignToConstant:
				name = "Trying to assign value to a constant.";
				break;
			case miniplc0::ErrDuplicateDeclaration:
				name = "The variable or constant has been declared.";
				break;
			case miniplc0::ErrNotInitialized:
				name = "The variable has not been initialized.";
				break;
			case miniplc0::ErrInvalidAssignment:
				name = "The assignment statement is invalid.";
				break;
			case miniplc0::ErrInvalidPrint:
				name = "The output statement is invalid.";
				break;
			case miniplc0::ErrNorightbracket:
				name = "no right bracket";
				break;
			case miniplc0::ErrInvalidFunctioncall:
				name = "the function call is invalid";
				break;
			case miniplc0::ErrInvalidfunctionreturn:
				name = "the function return is invalid";
				break;
			case miniplc0::Errinvalidscan:
				name = "the scan is invalid";
				break;
			case miniplc0::Errinvalidprint:
				name = "the print is invalid";
				break;
			case miniplc0::Errinvalidbreak:
				name = "the break is invalid";
				break;
			case miniplc0::Errinvalidcontinue:
				name = "the continue is invalid";
				break;
			case miniplc0::Errinvaliddowhile:
				name = " the do while loop is invalid";
				break;
			case miniplc0::Errinvalidfor:
				name = "the for loop is invalid";
				break;
			case miniplc0::ErrNoMain:
				name = "there is no main function";
				break;
			case miniplc0::Errinvalidcomment:
				name = "the multi comment is invalid";
				break;
			}
			return format_to(ctx.out(), name);
		}
	};

	template<>
	struct formatter<miniplc0::CompilationError> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::CompilationError &p, FormatContext &ctx) {
			return format_to(ctx.out(), "Line: {} Column: {} Error: {}", p.GetPos().first, p.GetPos().second, p.GetCode());
		}
	};
}

namespace fmt {
	template<>
	struct formatter<miniplc0::Token> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::Token &p, FormatContext &ctx) {
			return format_to(ctx.out(),
				"Line: {} Column: {} Type: {} Value: {}",
				p.GetStartPos().first, p.GetStartPos().second, p.GetType(), p.GetValueString());
		}
	};

	template<>
	struct formatter<miniplc0::TokenType> {
		template <typename ParseContext>
		constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(const miniplc0::TokenType &p, FormatContext &ctx) {
			std::string name;
			switch (p) {
			case miniplc0::NULL_TOKEN:
				name = "NullToken";
				break;
			case miniplc0::DECIMAL_INTEGER:
				name = "deCIMal integer";
				break;
			case miniplc0::IDENTIFIER:
				name = "Identifier";
				break;
			case miniplc0::HEXADECIMAL_INTEGER:
				name = "hexadeciaml";
				break;
			case miniplc0::VOID:
				name = "VOID";
				break;
			case miniplc0::INT:
				name = "int";
				break;
			case miniplc0::CHAR:
				name = "char";
				break;
			case miniplc0::DOUBLE:
				name = "double";
				break;
			case miniplc0::CONST:
				name = "Const";
				break;
			case miniplc0::SCAN:
				name = "scan";
				break;
			case miniplc0::PRINT:
				name = "Print";
				break;
			case miniplc0::STRUCT:
				name ="struct";
				break;
			case miniplc0::IF:
				name = "if";
				break;
			case miniplc0::ELSE:
				name ="else";
				break;
			case miniplc0::SWITCH:
				name ="switch";
				break;
			case miniplc0::CASE:
				name ="case";
				break;
			case miniplc0::DEFAULT:
				name ="default";
				break;
			case miniplc0::WHILE:
				name ="while";
				break;
			case miniplc0::FOR:
				name ="for";
				break;
			case miniplc0::DO:
				name = "do";
				break;
			case miniplc0::RETURN:
				name ="return";
				break;
			case miniplc0::BREAK:
				name ="break";
				break;
			case miniplc0::CONTINUE:
				name="continue";
				break;
			case miniplc0::PLUS_SIGN:
				name = "PlusSign";
				break;
			case miniplc0::MINUS_SIGN:
				name = "MinusSign";
				break;
			case miniplc0::MULTIPLICATION_SIGN:
				name = "MultiplicationSign";
				break;
			case miniplc0::DIVISION_SIGN:
				name = "DivisionSign";
				break;
			case miniplc0::EQUAL_SIGN:
				name = "EqualSign";
				break;
			case miniplc0::LESS_SIGN:
				name ="less";
				break;
			case miniplc0::LESS_EQUAL_SIGN:
				name ="lessequal";
				break;
			case miniplc0::GREATER_SIGN:
				name ="greater";
				break;
			case miniplc0::GREATER_EQUAL_SIGN:
				name ="greaterequal";
				break;
			case miniplc0::ISEQUAL_SIGN:
				name ="isequal";
				break;
			case miniplc0::NOEQUAL_SIGN:
				name ="noequal";
				break;
			case miniplc0::SEMICOLON:
				name = "Semicolon";
				break;
			case miniplc0::LEFT_BRACKET:
				name = "LeftBracket";
				break;
			case miniplc0::RIGHT_BRACKET:
				name = "RightBracket";
				break;
			case miniplc0::LEFTBIG_BRACKET:
				name ="leftbigbracket";
				break;
			case miniplc0::RIGHTBIG_BRACKET:
				name ="rightbig_bracket";
				break;
			case miniplc0::COMMA_SIGN:
				name ="comma";
				break;
			}
			return format_to(ctx.out(), name);
		}
	};
}

