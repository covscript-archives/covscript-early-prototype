#include <iostream>
#include <memory>
#include <string>
#include <list>
#include <map>
#include "covany.h"
namespace cst {
	static const std::string keywords[] = {
		"import", "function", "if", "else", "do", "while", "for", "return", "end"
	};
	enum logic_signal {
		And, Or, Equ, Not, Above, Under, NotEqu, AboveEqu, UnderEqu
	};
	bool isLogicSignal(char signal)
	{
		switch(signal) {
		case '>':
			break;
		case '<':
			break;
		case '=':
			break;
		case '!':
			break;
		case '&':
			break;
		case '|':
			break;
		default:
			return false;
		}
		return true;
	}
	logic_signal match_logic_signal(const std::string& signal)
	{
		if(signal=="&&")
			return logic_signal::And;
		if(signal=="||")
			return logic_signal::Or;
		if(signal=="==")
			return logic_signal::Equ;
		if(signal=="!=")
			return logic_signal::NotEqu;
		if(signal=="!")
			return logic_signal::Not;
		if(signal==">")
			return logic_signal::Above;
		if(signal=="<")
			return logic_signal::Under;
		if(signal==">=")
			return logic_signal::AboveEqu;
		if(signal=="<=")
			return logic_signal::UnderEqu;
	}
	bool compare(const std::string & exp)
	{
		bool reverse = false;
		std::list < logic_signal > signals;
		std::list < std::string > conditions;
		std::string tmp;
		for (int i = 0; i < exp.size();) {
			if (std::isspace(exp[i])) {
				++i;
				continue;
			}
			if (exp[i] == '(') {
				int level(1), pos(++i);
				for (; pos < exp.size() && level > 0; ++pos) {
					if (exp[pos] == '(')
						++level;
					if (exp[pos] == ')')
						--level;
				}
				if (level > 0)
					throw std::logic_error("The lack of corresponding brackets.");
				compare(exp.substr(i, pos - i - 1));
				i = pos;
				continue;
			}
			if (isLogicSignal(exp[i])) {
				if(!tmp.empty())
					conditions.push_back(tmp);
				tmp.clear();
				std::string currentSignal(1,exp[i]);
				if(i<exp.size()) {
					if(isLogicSignal(exp[++i]))
						currentSignal+=exp[i];
					else
						tmp+=exp[i];
				}
				signals.push_back(match_logic_signal(currentSignal));
				++i;
				continue;
			}
			else {
				tmp+=exp[i];
				++i;
				continue;
			}
		}
		if(!tmp.empty())
			conditions.push_back(tmp);
		for(auto &it:conditions)
			std::cout<<it<<std::endl;
		for(auto &it:signals) {
			switch(it) {
			case logic_signal::And:
				std::cout<<"&&"<<std::endl;
				break;
			case logic_signal::Or:
				std::cout<<"||"<<std::endl;
				break;
			case logic_signal::Equ:
				std::cout<<"=="<<std::endl;
				break;
			case logic_signal::NotEqu:
				std::cout<<"!="<<std::endl;
				break;
			case logic_signal::Not:
				std::cout<<"!"<<std::endl;
				break;
			case logic_signal::Above:
				std::cout<<">"<<std::endl;
				break;
			case logic_signal::Under:
				std::cout<<"<"<<std::endl;
				break;
			case logic_signal::AboveEqu:
				std::cout<<">="<<std::endl;
				break;
			case logic_signal::UnderEqu:
				std::cout<<"<="<<std::endl;
				break;
			}
		}
	}
	std::string identify(const std::string & sentence)
	{
		for (auto & keyword:keywords) {
			if (sentence.find(keyword) != std::string::npos) {
				return keyword;
			}
		}
		return "NULL";
	}
	namespace Sentence {
		enum Types {
			Null, Import, If, Else, Do, While, For, Function, Lambda
		};
		struct parse_result;
		class basic;
		class simple;
		class complex;
		class import;
		class condition;
		/* class forLoop; class untilLoop; class whileLoop; */
		class function;
		class lambda;

		struct parse_result {
			basic *sentence = nullptr;
			int expected = Types::Null;
			bool necessary = false;
		};

		class basic {
		public:
			basic() = default;
			basic(const basic &) = default;
			virtual ~ basic() = default;
			virtual cov::any call() const = 0;
		};

		class simple:public basic {
		protected:
			std::string mBody;
		public:
			simple() = default;
			simple(const std::string & body):mBody(body)
			{
			}
			simple(const simple &) = default;
			virtual ~ simple() = default;
			virtual cov::any call() const override
			{
				std::cout << "<Simple><Sentence Type> " << identify(mBody) << std::
				          endl << mBody << std::endl;
				return 0;
			}
		};

		class complex:public basic {
		protected:
			std::string mHead;
			std::list < std::shared_ptr < basic > >mBody;
		public:
			complex() = default;
			complex(const std::string & head,
			        const std::list < std::shared_ptr < basic > >&body):mHead(head), mBody(body)
			{
			}
			complex(const complex &) = default;
			virtual ~ complex() = default;
			virtual cov::any call() const override
			{
				if(identify(mHead)=="function") {
					std::string fname,fargs;
					int i=std::string("function").size();
					for(; i<mHead.size()&&mHead[i]!='('; ++i) {
						fname+=mHead[i];
					}
					for(++i; i<mHead.size()&&mHead[i]!=')'; ++i) {
						fargs+=mHead[i];
					}
					std::string res="auto"+fname+"=[&]("+fargs+")";
					std::cout<<res<<std::endl;
				}
				std::cout << "<Complex><Sentence Type> " << identify(mHead) << std::
				          endl << mHead << std:: endl;
				for (auto & it:mBody)
					it->call();
				std::cout << "<EndComplex:" << identify(mHead) << ">" << std::endl;
				return 0;
			}
		};

		class import:public simple {
		public:
			static parse_result parse(const std::string &);
			import() = default;
			import(const std::string & pkg)
			{
				this->mBody = pkg;
			}
			import(const import &) = default;
			virtual ~ import() = default;
			virtual cov::any call() const override;
		};

		class condition:public complex {
		protected:
			std::shared_ptr < complex > mElse = nullptr;
		public:
			static parse_result parse(const std::string &);
			condition() = default;
			using complex::complex;
			condition(const condition &) = default;
			virtual ~ condition() = default;
			virtual cov::any call() const override;
		};
	}
}

static std::list < char >code;
static std::list < std::shared_ptr < cst::Sentence::basic > >sentences;

void breakdown(const std::list < char >&source,
               std::list < std::shared_ptr < cst::Sentence::basic > >&data)
{
	bool escape = false;
	bool strings = false;
	int bracket = 0;
	std::string tmp;
	for (auto it = source.begin(); it != source.end();) {
		if (*it == '\\')
			escape = true;
		if (*it == '\"')
			strings = strings ? false : true;
		if (!strings) {
			if (*it == '(')
				++bracket;
			if (*it == ')')
				--bracket;
		}
		if ((*it != ';' && *it != '{' && *it != '}') || (escape || strings || bracket > 0)) {
			if (escape)
				escape = false;
			tmp += *it;
			++it;
			continue;
		}
		if (*it == ';') {
			data.push_back(std::make_shared < cst::Sentence::simple > (tmp));
			tmp.clear();
			++it;
			continue;
		}
		if (*it == '{') {
			++it;
			std::string head = tmp;
			tmp.clear();
			std::list < char >bodysource;
			std::list < std::shared_ptr < cst::Sentence::basic > >body;
			for (int level = 1; it != source.end() && level > 0; ++it) {
				if (*it == '{')
					++level;
				if (*it == '}')
					--level;
				if (level > 0)
					bodysource.push_back(*it);
			}
			breakdown(bodysource, body);
			data.push_back(std::make_shared < cst::Sentence::complex > (head, body));
		}
	}
}

#include <fstream>
int main()
{
	std::ifstream in("test.csf");
	std::string line;
	while (std::getline(in, line)) {
		if (line == "end")
			break;
		if (line[0] == '#')
			continue;
		for (auto & it:line) {
			code.push_back(it);
		}
	}
	std::cout << std::endl;
	for (auto & it:code)
		std::cout << it;
	std::cout << std::endl;
	breakdown(code, sentences);
	std::cout << std::endl;
	for (auto & it:sentences) {
		it->call();
	}
}
/*int main()
{
	cst::compare("!x>1&&y<10");
}*/