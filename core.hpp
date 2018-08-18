#pragma once
// Covariant Dragon Core Head
// Version 1.16.09.01
#include "covstdlib.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>
#include <deque>
#include <list>
namespace cov {
	namespace dragon {
		enum class types {
			Null,Int,Float,Bool,String,Function,Native_Interface
		};
		enum class keywords {
			Null,Import,If,Else,Do,While,For,Return,Continue,Break,Static,Constant
		};
		enum class signals {
			Null,And,Or,Not,Above,Under,
			Equ,NotEqu,AboveEqu,UnderEqu
		};
		enum class operators {
			Null,Add,Sub,Mul,Div,Assign
		};
		enum class statements {
			Null,declaration,assignment,exp_num,exp_bool,function_call
		};
		static const std::string types_string[]= {
			"int ","float ","bool ","string ","function ","native_api "
		};
		static const std::string keywords_string[]= {
			"import", "if", "else", "do", "while", "for", "return","continue","break","static","const"
		};
		static const std::string signals_string[]= {
			"&&","||","!",">","<","==","!=",">=","<="
		};
		static const std::string operators_string[]= {
			"+","-","*","/","="
		};
		struct object_info final {
			bool is_constant=false;
			types object_type=types::Null;
			cov::any object_value;
			std::string object_name;
			object_info()=default;
			object_info(const object_info&)=default;
			object_info(object_info&&)=default;
			object_info& operator=(const object_info&)=default;
			object_info& operator=(object_info&&)=default;
			object_info& assign(const object_info& obj)
			{
				if(this->is_constant)
					throw std::logic_error("DE0006");
				if(this->object_type!=obj.object_type)
					throw std::logic_error("DE0007");
				this->object_value=obj.object_value;
				return *this;
			}
		};
		class domain final {
			std::list<object_info> mObjects;
		public:
			domain()=default;
			domain(const domain&)=delete;
			domain(domain&&)=delete;
			~domain()=default;
			object_info& find(const std::string& name)
			{
				for(auto& it:mObjects)
					if(name==it.object_name)
						return it;
				throw std::logic_error("DE0001");
			}
			const object_info& find(const std::string& name) const
			{
				for(auto& it:mObjects)
					if(name==it.object_name)
						return it;
				throw std::logic_error("E0001");
			}
			bool exsist(const std::string& str) const
			{
				for(auto& it:mObjects)
					if(str==it.object_name)
						return true;
				return false;
			}
			void add_object(const object_info& obj)
			{
				mObjects.push_back(obj);
			}
		};
		class domain_manager final {
			domain mGlobal;
			std::deque<domain> mDomains;
		public:
			typedef std::deque<domain>::iterator iterator;
			typedef std::deque<domain>::const_iterator const_iterator;
			domain_manager()=default;
			domain_manager(const domain_manager&)=delete;
			domain_manager(domain_manager&&)=delete;
			~domain_manager()=default;
			void add_domain()
			{
				mDomains.emplace_front();
			}
			void remove_top_domain()
			{
				if(!mDomains.empty())
					mDomains.pop_front();
			}
			domain& top_domain()
			{
				if(mDomains.empty())
					return mGlobal;
				else
					return mDomains.front();
			}
			const domain& top_domain() const
			{
				if(mDomains.empty())
					return mGlobal;
				else
					return mDomains.front();
			}
			domain& global_domain()
			{
				return mGlobal;
			}
			const domain& global_domain() const
			{
				return mGlobal;
			}
			iterator begin()
			{
				return mDomains.begin();
			}
			iterator end()
			{
				return mDomains.end();
			}
			const_iterator begin() const
			{
				return mDomains.begin();
			}
			const_iterator end() const
			{
				return mDomains.end();
			}
		};
		class basic_statement;
		struct parse_result final {
			std::shared_ptr<basic_statement> statement;
			statements expected=statements::Null;
			bool necessary=false;
		};
		class basic_statement {
		public:
			basic_statement() = default;
			basic_statement(const basic_statement&) = default;
			basic_statement(basic_statement&&)=default;
			virtual ~ basic_statement() = default;
			virtual const std::type_info& type() const final
			{
				return typeid(*this);
			}
			virtual cov::any call() const = 0;
		};
		class simple_statement:public basic_statement {
		protected:
			std::string mBody;
		public:
			simple_statement() = default;
			simple_statement(const std::string & body):mBody(body) {}
			simple_statement(const simple_statement&) = default;
			simple_statement(simple_statement&&) = default;
			virtual ~ simple_statement() = default;
			virtual cov::any call() const override {}
		};
		class complex_statement:public basic_statement {
		protected:
			std::string mHead;
			std::deque<std::shared_ptr<basic_statement>>mBody;
		public:
			complex_statement() = default;
			complex_statement(const std::string & head, const std::deque < std::shared_ptr < basic_statement > >&body):mHead(head), mBody(body) {}
			complex_statement(const complex_statement&) = default;
			complex_statement(complex_statement&&) = default;
			virtual ~ complex_statement() = default;
			virtual cov::any call() const override {}
		};
		void split_str(char sig,const std::string& str,std::deque<std::string>& block,bool reverse=false)
		{
			std::string tmp;
			for(auto& it:str) {
				if(it==sig) {
					if(reverse)
						block.push_front(tmp);
					else
						block.push_back(tmp);
					tmp.clear();
				}
				else
					tmp+=it;
			}
			if(reverse)
				block.push_front(tmp);
			else
				block.push_back(tmp);
		}
		statements match_statements(const std::string& str)
		{
			for(auto & it:types_string) {
				if(str.find(it)!=std::string::npos)
					return statements::declaration;
			}
			if(str.find("=")!=std::string::npos&&str.find("==")==std::string::npos)
				return statements::assignment;
			std::string tmp;
			for(auto & it:str) {
				if(it==' ')
					continue;
				if(it=='(') {
					if(!tmp.empty())
						return statements::function_call;
					else
						break;
				}
				if(std::isalpha(it))
					tmp+=it;
			}
			for(auto & it:operators_string) {
				if(str.find(it)!=std::string::npos)
					return statements::exp_num;
			}
			for(auto & it:signals_string) {
				if(str.find(it)!=std::string::npos)
					return statements::exp_bool;
			}
			return statements::Null;
		}
		keywords match_keywords(const std::string& str)
		{
			keywords ret;
			Switch(str) {
				Default {
					ret=keywords::Null;
				} EndCase;
				Case("import") {
					ret=keywords::Import;
				}
				EndCase;
				Case("if") {
					ret=keywords::If;
				}
				EndCase;
				Case("else") {
					ret=keywords::Else;
				}
				EndCase;
				Case("do") {
					ret=keywords::Do;
				}
				EndCase;
				Case("while") {
					ret=keywords::While;
				}
				EndCase;
				Case("for") {
					ret=keywords::For;
				}
				EndCase;
				Case("return") {
					ret=keywords::Return;
				}
				EndCase;
				Case("continue") {
					ret=keywords::Continue;
				}
				EndCase;
				Case("break") {
					ret=keywords::Break;
				}
				EndCase;
			}
			EndSwitch;
			return ret;
		}
		signals match_signal(const std::string& str)
		{
			signals ret;
			Switch(str) {
				Default {
					ret=signals::Null;
				} EndCase;
				Case("&&") {
					ret=signals::And;
				}
				EndCase;
				Case("||") {
					ret=signals::Or;
				}
				EndCase;
				Case("!") {
					ret=signals::Not;
				}
				EndCase;
				Case(">") {
					ret=signals::Above;
				}
				EndCase;
				Case("<") {
					ret=signals::Under;
				}
				EndCase;
				Case("==") {
					ret=signals::Equ;
				}
				EndCase;
				Case("!=") {
					ret=signals::NotEqu;
				}
				EndCase;
				Case(">=") {
					ret=signals::AboveEqu;
				}
				EndCase;
				Case("<=") {
					ret=signals::UnderEqu;
				}
				EndCase;
			}
			EndSwitch;
			return ret;
		}
		bool is_signal(char signal)
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
		domain_manager variables;
		object_info& get_object(const std::string& name)
		{
			auto pos=name.find("::");
			if(pos!=std::string::npos&&name.substr(0,pos)=="global") {
				std::string n=name.substr(pos+2);
				if(variables.global_domain().exsist(n))
					return variables.global_domain().find(n);
			}
			for(auto& it:variables)
				if(it.exsist(name))
					return it.find(name);
			if(variables.global_domain().exsist(name))
				return variables.global_domain().find(name);
		}
		template<typename T>const T& get_value(const std::string& name)
		{
			auto pos=name.find("::");
			if(pos!=std::string::npos&&name.substr(0,pos)=="global") {
				std::string n=name.substr(pos+2);
				if(variables.global_domain().exsist(n))
					return variables.global_domain().find(n).object_value.val<T>();
			}
			for(auto& it:variables)
				if(it.exsist(name))
					return it.find(name).object_value.val<T>();
			if(variables.global_domain().exsist(name))
				return variables.global_domain().find(name).object_value.val<T>();
		}
		const cov::any& get_value(const std::string& name)
		{
			auto pos=name.find("::");
			if(pos!=std::string::npos&&name.substr(0,pos)=="global") {
				std::string n=name.substr(pos+2);
				if(variables.global_domain().exsist(n))
					return variables.global_domain().find(n).object_value;
			}
			for(auto& it:variables)
				if(it.exsist(name))
					return it.find(name).object_value;
			if(variables.global_domain().exsist(name))
				return variables.global_domain().find(name).object_value;
		}
		types get_type(const std::string& name)
		{
			auto pos=name.find("::");
			if(pos!=std::string::npos&&name.substr(0,pos)=="global") {
				std::string n=name.substr(pos+2);
				if(variables.global_domain().exsist(n))
					return variables.global_domain().find(n).object_type;
			}
			for(auto& it:variables)
				if(it.exsist(name))
					return it.find(name).object_type;
			if(variables.global_domain().exsist(name))
				return variables.global_domain().find(name).object_type;
		}
		bool get_constant(const std::string& name)
		{
			auto pos=name.find("::");
			if(pos!=std::string::npos&&name.substr(0,pos)=="global") {
				std::string n=name.substr(pos+2);
				if(variables.global_domain().exsist(n))
					return variables.global_domain().find(n).is_constant;
			}
			for(auto& it:variables)
				if(it.exsist(name))
					return it.find(name).is_constant;
			if(variables.global_domain().exsist(name))
				return variables.global_domain().find(name).is_constant;
		}
		bool exsist(const std::string& name)
		{
			auto pos=name.find("::");
			if(pos!=std::string::npos&&name.substr(0,pos)=="global") {
				std::string n=name.substr(pos+2);
				return variables.global_domain().exsist(n);
			}
			for(auto& it:variables)
				if(it.exsist(name))
					return true;
			return variables.global_domain().exsist(name);
		}
		cov::any infer_value(const std::string& str)
		{
			if(exsist(str))
				return get_value(str);
			if(str.find('\"')!=std::string::npos)
				return str.substr(str.find('\"')+1,str.rfind('\"')-str.find('\"')-1);
			if(str.find('.')!=std::string::npos)
				return std::stof(str);
			if(str=="true")
				return true;
			if(str=="false")
				return false;
			bool is_digit=true;
			for(auto& it:str) {
				if(!std::isdigit(it)) {
					is_digit=false;
					break;
				}
			}
			if(is_digit)
				return std::stoi(str);
			else
				throw std::logic_error(str);
		}
		types infer_type(const cov::any&);
		class return_statement final:public simple_statement {
			cov::any mRetval;
		public:
			return_statement()=default;
			return_statement(const cov::any& val):mRetval(val) {}
			parse_result parse(const std::string&);
			cov::any call() const override
			{
				return this->mRetval;
			}
		};
		parse_result return_statement::parse(const std::string& str)
		{
			std::string tmp;
			for(std::size_t i=str.find(' '); i<str.size(); ++i) {
				if(str.at(i)==' ')
					continue;
				tmp+=str.at(i);
			}
			return {std::make_shared<return_statement>(infer_value(tmp)),statements::Null,false};
		}
		class function final {
			mutable std::deque<object_info>mArgs;
			std::deque<std::shared_ptr<basic_statement>>mBody;
		public:
			function()=default;
			function(const std::deque<object_info>& args,const std::deque<std::shared_ptr<basic_statement>>& body):mArgs(args),mBody(body) {}
			function(const function&)=default;
			function(function&&)=default;
			~function()=default;
			object_info call(const std::deque<object_info>& args) const
			{
				if(args.size()!=this->mArgs.size())
					throw std::logic_error("DE0005");
				variables.add_domain();
				for(std::size_t i=0; i<this->mArgs.size(); ++i) {
					mArgs[i].assign(args[i]);
					variables.top_domain().add_object(mArgs[i]);
				}
				for(auto&it:this->mBody) {
					if(it->type()==typeid(return_statement)) {
						cov::any ret=it->call();
						variables.remove_top_domain();
						return {true,infer_type(ret),ret,""};
					}
					it->call();
				}
				variables.remove_top_domain();
			}
		};
		class native_interface final {
			cov::function<object_info(const std::deque<object_info>&)> mFunc;
		public:
			native_interface()=default;
			template<typename T>
			native_interface(T func):mFunc(func) {}
			~native_interface()=default;
			object_info call(const std::deque<object_info>& args) const
			{
				return mFunc(args);
			}
			bool operator==(const native_interface&) const
			{
				return false;
			}
		};
	}
}