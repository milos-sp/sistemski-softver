%{
  //defs
  #include <cstdio>
  #include "parser.hpp"
  //#include "../inc/assembler.hpp"

  int linenum = 1;
%}

%option outfile="lexer.cpp" header-file="lexer.hpp"
%option noyywrap

L_PAR "["
R_PAR "]"
COMMA ","
PLUS "+"
MINUS "-"
SEMICOLON ";"
PERCENT "%"
DOLLAR "$"

GLOBAL (\.global)
EXTERN (\.extern)
SECTION (\.section)
WORD (\.word)
SKIP (\.skip)
END (\.end)

HALT (halt)
INT (int)
IRET (iret)
JMP (jmp)
CALL (call)
RET (ret)
BEQ (beq)
BNE (bne)
BGT (bgt)
PUSH (push)
POP (pop)
XCHG (xchg)
ADD (add)
SUB (sub)
MUL (mul)
DIV (div)
NOT (not)
AND (and)
OR (or)
XOR (xor)
SHL (shl)
SHR (shr)
LD (ld)
ST (st)
CSRRD (csrrd)
CSRWR (csrwr)

REGISTER (r[0-9]|r10|r11|r12|r13|r14|r15|sp|pc)
STATUS (%status)
HANDLER (%handler)
CAUSE (%cause)
LITERAL ($[0-9]+|0[xX][0-9a-fA-F]+)
SYMBOL [a-zA-Z_.][a-zA-Z0-9_$]*
LABEL [a-zA-Z_.][a-zA-Z0-9_$]*:
STRING \s


%%
{GLOBAL} { yylval.sval = strdup(yytext); return GLOBAL; }
{EXTERN} { yylval.sval = strdup(yytext); return EXTERN; }
{SECTION} { yylval.sval = strdup(yytext); return SECTION; }
{WORD} { yylval.sval = strdup(yytext); return WORD; }
{SKIP} { yylval.sval = strdup(yytext); return SKIP; }
{END} { return END; }
{REGISTER} { yylval.sval = strdup(yytext); return REGISTER; }

{HALT} { yylval.ival = 0b00000000; return INSTR; }
{INT} { yylval.ival = 0b00010000; return INSTR; }
{CALL} { yylval.ival = 0b00100000; return CALL; }
{RET} { yylval.ival = 0b10010011; return INSTR; }
{IRET} { yylval.ival = 1; return INSTR; }
{PUSH} { yylval.ival = 0b10000001; return INSTR; }
{POP} { yylval.ival = 0b10010011; return INSTR; }
{JMP} { yylval.ival = 0b00110000; return INSTR; }
{BEQ} { yylval.ival = 0b00110001; return INSTR; }
{BNE} { yylval.ival = 0b00110010; return INSTR; }
{BGT} { yylval.ival = 0b00110011; return INSTR; }
{XCHG} { yylval.ival = 0b01000000; return INSTR; }
{ADD} { yylval.ival = 0b01010000; return INSTR; }
{SUB} { yylval.ival = 0b01010001; return INSTR; }
{MUL} { yylval.ival = 0b01010010; return INSTR; }
{DIV} { yylval.ival = 0b01010011; return INSTR; }
{NOT} { yylval.ival = 0b01100000; return INSTR; }
{AND} { yylval.ival = 0b01100001; return INSTR; }
{OR} { yylval.ival = 0b01100010; return INSTR; }
{XOR} { yylval.ival = 0b01100011; return INSTR; }
{SHL} { yylval.ival = 0b01110000; return INSTR; }
{SHR} { yylval.ival = 0b01110001; return INSTR; }
{LD} { yylval.ival = 0b10010000; return LD; }
{ST} { yylval.ival = 0b10000000; return ST; }
{CSRRD} { yylval.ival = 0b10010000; return CSR; }
{CSRWR} { yylval.ival = 0b10010100; return CSR; }
{STATUS} { yylval.sval = strdup(yytext); return SREGISTER; }
{HANDLER} { yylval.sval = strdup(yytext); return SREGISTER; }
{CAUSE} { yylval.sval = strdup(yytext); return SREGISTER; }

"[" { return L_PAR; }
"]" { return R_PAR; }
"+" { return PLUS; }
"-" { return MINUS; }
"," { return COMMA; }
":" { return COLON; }
";" { return SEMICOLON; }
"%" { return PERCENT; }
"$" { return DOLLAR; }

\n { linenum++; return ENDL; }
"#"[^\n\r]* { return COMMENT; }

[ \t]   ;
0[xX][0-9a-fA-F]+ { sscanf(yytext, "%x", &yylval.ival); return HEX; }
[0-9]+ { yylval.ival = atoi(yytext); return INTEGER; }

[a-zA-Z][a-zA-Z0-9_]+ { yylval.sval = strdup(yytext); return STRING; }

%%


