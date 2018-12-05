# Compiler Lab

Design and implement a compiler for a C-like language: translate C-like language source code to MIPS32 assembly. The generated assembly can be executed by SPIM Simulator. There are four phases of this compiler, and the output of a phase is the input of the following phase:

* Lexical and syntax analysis
     * Use `flex` and `bison`
     * Print parse tree
     * Check basic lexical and syntax error
* Semantic analysis
     * Check semantic errors (undefined/redefined identifiers, ...) and type errors (e.g. LHS of assigment operator is not an lvalue)
     * Support nested scope
* Intermediate code generation
     * Support multidimensional array
* Machine code (MIPS32 Assembly) generation 
     * Consistent with MIPS C-compiler call convention

## Compile and run
To compile the project, type
``` shell
make
```
to get the exectuable `./build/cmm`. Then run 
```
./build/cmm input_file output_file
```
`input_file` is a C-like language source code file, and `output_file` is the generated MIPS32 assembly.

-------------------------
# 编译原理实验

为一个小型的类C语言实现一个编译器，分为四个阶段：
 * 词法和语法分析
 * 语义分析
 * 中间代码生成
 * 目标代码生成
 
每阶段的输出是下一阶段的输入，最终得到一个完整的、能将类C语言源代码转换成MIPS汇编代码的编译器，所得到的汇编代码可以在SPIM Simulator上运行。 

必须层次：检测基本的词法和语法错误、基本的类型错误、生成无优化的代码等。 

可选层次：支持多种数据表示、指数形式浮点数、结构体和高维数组、优化代码等。 
