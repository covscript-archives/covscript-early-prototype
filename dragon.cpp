#include "core.hpp"
#include <iostream>
namespace cov {
	namespace dragon {
		types infer_type(const cov::any& val)
		{
			if(val.type()==typeid(int))
				return types::Int;
			if(val.type()==typeid(float))
				return types::Float;
			if(val.type()==typeid(bool))
				return types::Bool;
			if(val.type()==typeid(std::string))
				return types::String;
			if(val.type()==typeid(dragon::function))
				return types::Function;
			throw std::logic_error("DE0017");
		}
		template<typename T>T compute(const std::string&);
		template<> bool compute<bool>(const std::string & exp)
		{
			bool reverse = false;
			bool is_str = false;
			std::deque < signals > signals;
			std::deque < std::string > conditions;
			std::string tmp;
			for (int i = 0; i < exp.size();) {
				if (std::isspace(exp[i])&&!is_str) {
					++i;
					continue;
				}
				if(exp[i]=='\"')
					is_str=is_str?false:true;
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
					if(compute<bool>(exp.substr(i, pos - i - 1))?(reverse?false:true):(reverse?true:false))
						conditions.push_back("true");
					else
						conditions.push_back("false");
					reverse=false;
					i = pos;
					continue;
				}
				if (is_signal(exp[i])&&(i<exp.size()?(is_signal(exp[i+1])?true:exp[i]!='!'):exp[i]!='!')) {
					if(!tmp.empty())
						conditions.push_back(tmp);
					tmp.clear();
					std::string currentSignal(1,exp[i]);
					if(i<exp.size()) {
						if(is_signal(exp[++i]))
							currentSignal+=exp[i];
						else
							tmp+=exp[i];
					}
					signals.push_back(match_signal(currentSignal));
					++i;
					continue;
				}
				if(i<exp.size()&&exp[i]=='!'&&exp[i+1]=='(') {
					reverse=true;
					++i;
					continue;
				}
				tmp+=exp[i];
				++i;
			}
			if(!tmp.empty())
				conditions.push_back(tmp);
			cov::any val;
			auto parse=[](const std::string& str) {
				cov::any value;
				if(str[0]=='!') {
					value=infer_value(str.substr(1));
					if(value.type()!=typeid(bool))
						throw std::logic_error("DE0003");
					value=value.val<bool>()?false:true;
				}
				else
					value=infer_value(str);
				return value;
			};
			val=parse(conditions.front());
			conditions.pop_front();
			for(auto &it:conditions) {
				cov::any v=parse(it);
				switch(signals.front()) {
				case signals::And:
					val=v.val<bool>()&&val.val<bool>();
					break;
				case signals::Or:
					val=v.val<bool>()||val.val<bool>();
					break;
				case signals::Above:
					if(val.type()==v.type()) {
						if(val.type()==typeid(int)) {
							val=val.val<int>()>v.val<int>();
							break;
						}
						if(val.type()==typeid(float)) {
							val=val.val<float>()>v.val<float>();
							break;
						}
					}
					throw std::logic_error("DE0003");
					break;
				case signals::Under:
					if(val.type()==v.type()) {
						if(val.type()==typeid(int)) {
							val=val.val<int>()<v.val<int>();
							break;
						}
						if(val.type()==typeid(float)) {
							val=val.val<float>()<v.val<float>();
							break;
						}
					}
					throw std::logic_error("DE0003");
					break;
				case signals::Equ:
					val=(val==v);
					break;
				case signals::NotEqu:
					val=(val!=v);
					break;
				case signals::AboveEqu:
					if(val.type()==v.type()) {
						if(val.type()==typeid(int)) {
							val=val.val<int>()>=v.val<int>();
							break;
						}
						if(val.type()==typeid(float)) {
							val=val.val<float>()>=v.val<float>();
							break;
						}
					}
					throw std::logic_error("DE0003");
					break;
				case signals::UnderEqu:
					if(val.type()==v.type()) {
						if(val.type()==typeid(int)) {
							val=val.val<int>()<=v.val<int>();
							break;
						}
						if(val.type()==typeid(float)) {
							val=val.val<float>()<=v.val<float>();
							break;
						}
					}
					throw std::logic_error("DE0003");
					break;
				}
				signals.pop_front();
			}
			return val.val<bool>();
		}
		template<>float compute<float>(const std::string& exp)
		{
			bool reverse = false;
			std::deque<float> nums;
			std::deque<char>operators;
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
					nums.push_back(compute<float>(exp.substr(i, pos - i - 1)));
					i = pos;
					continue;
				}
				if (std::ispunct(exp[i])&&exp[i]!=':') {
					if (nums.empty()) {
						switch (exp[i]) {
						case '+':
							reverse = false;
							break;
						case '-':
							reverse = true;
							break;
						default:
							throw std::logic_error("Operator does not recognize.~~~");
						}
						++i;
						continue;
					}
					operators.push_back(exp[i]);
					++i;
					continue;
				}
				if (std::isdigit(exp[i]) || exp[i] == '.') {
					tmp.clear();
					for (; i < exp.size() && (isdigit(exp[i]) || exp[i] == '.'); ++i)
						tmp += exp[i];
					nums.push_back(std::stof(tmp));
					continue;
				}
				if (std::isalpha(exp[i]) || exp[i]==':') {
					tmp.clear();
					for (; i < exp.size() && (std::isalnum(exp[i]) || exp[i] == '_' || exp[i]==':'); ++i)
						tmp += exp[i];
					if (get_type(tmp)==types::Function) {
						int level(1), pos(++i);
						for (; pos < exp.size() && level > 0; ++pos) {
							if (exp[pos] == '(')
								++level;
							if (exp[pos] == ')')
								--level;
						}
						if (level > 0)
							throw std::logic_error("The lack of corresponding brackets.");
						std::string arglist = exp.substr(i, pos - i - 1);
						std::string temp;
						std::deque < object_info > args;
						for (int i = 0; i < arglist.size(); ++i) {
							if(arglist[i]=='(')
								++level;
							if(arglist[i]==')')
								--level;
							if (level>0 || arglist[i] != ',') {
								temp += arglist[i];
							}
							else {
								if(exsist(temp))
									args.push_back(get_object(temp));
								else
									args.push_back({true,types::Float,compute<float>(temp),""});
								temp.clear();
							}
						}
						if(exsist(temp))
							args.push_back(get_object(temp));
						else
							args.push_back({true,types::Float,compute<float>(temp),""});
						object_info retval=get_value<dragon::function>(tmp).call(args);
						switch(retval.object_type) {
						case types::Int:
							nums.push_back(retval.object_value.val<int>());
							break;
						case types::Float:
							nums.push_back(retval.object_value.val<float>());
							break;
						default:
							throw std::logic_error("DE0008");
						}
						i = pos;
						continue;
					}
					object_info obj=get_object(tmp);
					switch(obj.object_type) {
					case types::Int:
						nums.push_back(obj.object_value.val<int>());
						break;
					case types::Float:
						nums.push_back(obj.object_value.val<float>());
						break;
					default:
						throw std::logic_error("DE0008");
					}
					continue;
				}
				throw std::logic_error("Operator does not recognize.-----");
			}
			if (nums.empty())
				return -1;
			float left = nums.front();
			float right = 0;
			char signal = 0;
			nums.pop_front();
			for (auto & current:nums) {
				switch (operators.front()) {
				case '+': {
					if (right != 0) {
						switch (signal) {
						case '+':
							left += right;
							break;
						case '-':
							left -= right;
							break;
						}
					}
					right = current;
					signal = '+';
					break;
				}
				case '-': {
					if (right != 0) {
						switch (signal) {
						case '+':
							left += right;
							break;
						case '-':
							left -= right;
							break;
						}
					}
					right = current;
					signal = '-';
					break;
				}
				case '*': {
					if (right != 0)
						right *= current;
					else
						left *= current;
					break;
				}
				case '/': {
					if (right != 0)
						right /= current;
					else
						left /= current;
					break;
				}
				default:
					throw std::logic_error("Operator does not recognize.");
				}
				operators.pop_front();
			}
			float result = 0;
			switch (signal) {
			case '+':
				result = left + right;
				break;
			case '-':
				result = left - right;
				break;
			default:
				result = left;
				break;
			}
			if (reverse)
				result = -result;
			return result;
		}
		object_info parse_function_call(const std::string& str)
		{
			bool is_str=false;
			std::deque<object_info> args;
			std::string sstr;
			for(auto & it:str) {
				if(std::isspace(it)&&!is_str)
					continue;
				if(it=='\"')
					is_str=is_str?false:true;
				sstr+=it;
			}
			std::string fname=sstr.substr(0,sstr.find('('));
			if(!exsist(fname) || (get_type(fname)!=types::Function&&get_type(fname)!=types::Native_Interface))
				throw std::logic_error("DE0009");
			std::string arglist=sstr.substr(sstr.find('(')+1,sstr.rfind(')')-sstr.find('(')-1);
			std::deque<std::string> blocks;
			split_str(',',arglist,blocks);
			for(auto & it:blocks) {
				switch(match_statements(it)) {
				case statements::exp_num:
					args.push_back({true,types::Float,compute<float>(it),""});
					break;
				case statements::exp_bool:
					args.push_back({true,types::Float,compute<bool>(it),""});
					break;
				case statements::Null:
					cov::any v=infer_value(it);
					args.push_back({true,infer_type(v),v,""});
					break;
				}
			}
			if(get_type(fname)==types::Function) {
				dragon::function func=get_object(fname).object_value.val<dragon::function>();
				return func.call(args);
			}
			else {
				dragon::native_interface func=get_object(fname).object_value.val<dragon::native_interface>();
				return func.call(args);
			}
		}
		void parse_statement(const std::string& statement)
		{
			if(statement.find("=")!=std::string::npos&&statement.find("==")==std::string::npos) {
				std::deque<std::string> blocks;
				split_str('=',statement,blocks,true);
				object_info val;
				for(auto &str:blocks) {
					switch(match_statements(str)) {
					case statements::exp_num:
						val= {true,types::Float,compute<float>(str),""};
						break;
					case statements::exp_bool:
						val= {true,types::Bool,compute<bool>(str),""};
						break;
					case statements::Null:
						if(exsist(str)&&val.object_type!=types::Null)
							get_object(str).assign(val);
						else {
							cov::any v=infer_value(str);
							val= {true,infer_type(v),v,""};
						}
						break;
					case statements::declaration:
						std::deque<std::string> b;
						split_str(' ',str,b);
						bool is_static=false;
						bool is_constant=false;
						object_info obj;
						for(auto &it:b) {
							Switch(it) {
								Case("static") {
									is_static=true;
								}
								EndCase;
								Case("const") {
									is_constant=true;
								}
								EndCase;
								Case("int") {
									obj.object_type=types::Int;
								}
								EndCase;
								Case("float") {
									obj.object_type=types::Float;
								}
								EndCase;
								Case("bool") {
									obj.object_type=types::Bool;
								}
								EndCase;
								Case("string") {
									obj.object_type=types::String;
								}
								EndCase;
							}
							EndSwitch;
						}
						obj.object_name=b.back();
						obj.assign(val);
						obj.is_constant=is_constant;
						if(is_static)
							variables.global_domain().add_object(obj);
						else
							variables.top_domain().add_object(obj);
						break;
					}
				}
			}
		}
		std::deque<char> source_code;
		std::deque<std::shared_ptr<basic_statement>> source_program;
		void breakdown(const std::deque<char>& source,std::deque<std::shared_ptr<basic_statement>>& data)
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
					data.push_back(std::make_shared < simple_statement > (tmp));
					tmp.clear();
					++it;
					continue;
				}
				if (*it == '{') {
					++it;
					std::string head = tmp;
					tmp.clear();
					std::deque < char > bodysource;
					std::deque<std::shared_ptr<basic_statement>> body;
					for (int level = 1; it != source.end() && level > 0; ++it) {
						if (*it == '{')
							++level;
						if (*it == '}')
							--level;
						if (level > 0)
							bodysource.push_back(*it);
					}
					breakdown(bodysource, body);
					data.push_back(std::make_shared < complex_statement > (head, body));
				}
			}
		}
	}
}
int main(int args,char** argv)
{
	using namespace cov::dragon;
	native_interface print([&](const std::deque<object_info>& args)->object_info{
		for(auto& it:args)
		{
			std::cout<<it.object_value;
		}
		std::cout<<std::endl;
		return {true,types::Int,10,""};
	});
	variables.global_domain().add_object({false,types::Native_Interface,print,"print"});
	std::cout<<"Dragon Programming Language simple parser"<<std::endl;
	std::string input;
	while(true) {
		std::cout<<"Command:";
		std::getline(std::cin,input);
		switch(match_statements(input)) {
		default:
			parse_statement(input);
			break;
		case statements::function_call:
			parse_function_call(input);
			break;
		}
	}
}