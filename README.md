# Covariant Script早期原型
+ `covscript.cpp`：探索实现词法分析器的方法
+ `covparser.cpp`: 探索实现语法分析器的方法
+ `dragon.cpp`： 整合了探索成果，是真正的原型产品

实际上在最新版的Covariant Script中依旧可见在早期原型中确定的一些概念，比如作用域分层以及分类明确的语句抽象系统，当然也包括在各类实践中逐渐成熟的Mozart Utilities（原Covstdlib）。熟悉Covariant Script发展历程的朋友可以发现这里的`dragon.cpp`和早期版本的Covariant Basic非常相似，因为两者几乎就是一个东西：CovBasic仅仅是整体重构了一下代码结构而已。  
实际上直到将近两年前的寒假我学习了编译原理之后才具体明白了一个编译器究竟应该是什么样子，这才有了后来的CovBasic和其改良版本CovScript。当然现在的CovScript和CovBasic的关系就像C语言和C++一样：看起来相似，实则完全不同。  
博观而约取，厚积而薄发。只有耐得住寂寞，坚持向一个方向努力付出，积累了足够多看起来微不足道的突破后，才能在机遇到来时爆发出难以置信的力量。