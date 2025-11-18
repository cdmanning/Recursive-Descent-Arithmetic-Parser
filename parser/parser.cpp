#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int charClass;
int lexLen;
char c;
char nextChar;
string lexeme;
string nextToken;
FILE *in_fp, *fopen();
FILE *out_fp;

void addChar();
void getChar();
void getNonBlank();
void term();
void expr();
void factor();
void assign();
void program();
string lex();

#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

string PROGRAM_KEYWORD("PROGRAM");
string END_KEYWORD("END");

main()
{
    if ((in_fp = fopen("parser.in", "r")) == NULL)
        printf("ERROR - cannot open parser.in \n");
    else
    {
        out_fp = fopen("parser.out", "w");
        c = fgetc(in_fp);
        while (c != EOF)
        {
            fputc(c, out_fp);
            c = fgetc(in_fp);
        }
        fclose(in_fp);
        if ((in_fp = fopen("parser.in", "r")) == NULL)
            printf("ERROR - cannot open parser.in \n");
        else
        {
            getChar();
            while (nextToken != "END_OF_FILE")
            {
                lex();
                if (nextToken != "END_OF_FILE")
                {
                    program();
                }
            }
        }
    }
    fprintf(out_fp, "\nParsing of the program is complete!");
    fclose(out_fp);
}

string lookup(char ch)
{
    switch (ch)
    {
    case '(':
        addChar();
        nextToken = "LEFT_PAREN";
        break;
    case ')':
        addChar();
        nextToken = "RIGHT_PAREN";
        break;
    case '+':
        addChar();
        nextToken = "ADD_OP";
        break;
    case '-':
        addChar();
        nextToken = "SUB_OP";
        break;
    case '*':
        addChar();
        nextToken = "MULT_OP";
        break;
    case '/':
        addChar();
        nextToken = "DIV_OP";
        break;
    case ';':
        addChar();
        nextToken = "SEMICOLON";
        break;
    case '=':
        addChar();
        nextToken = "ASSIGN_OP";
        break;
    default:
        addChar();
        nextToken = "END_OF_FILE";
        break;
    }
    return nextToken;
}

void addChar()
{
    if (lexLen <= 98)
    {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
    else
        printf("Error - lexeme is too long \n");
}

void getChar()
{
    if ((nextChar = getc(in_fp)) != EOF)
    {
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else
            charClass = UNKNOWN;
    }
    else
        charClass = EOF;
}

void getNonBlank()
{
    while (isspace(nextChar))
        getChar();
}

string lex()
{
    lexLen = 0;
    lexeme = " ";
    getNonBlank();
    switch (charClass)
    {
    /* Parse identifiers and Keywords* */
    case LETTER:
        addChar();
        getChar();
        while (charClass == LETTER || charClass == DIGIT)
        {
            addChar();
            getChar();
        }
        if (lexLen == 7)
        {
            if (lexeme.c_str() == PROGRAM_KEYWORD)
            {
                nextToken = "PROGRAM_KEYWORD";
            }
            else
            {
                nextToken = "IDENT";
            }
        }
        else if (lexLen == 3)
        {
            if (lexeme.c_str() == END_KEYWORD)
            {
                nextToken = "END_KEYWORD";
            }
            else
            {
                nextToken = "IDENT";
            }
        }
        else
        {
            nextToken = "IDENT";
        }
        break;
    /* Parse integer literals */
    case DIGIT:
        addChar();
        getChar();
        while (charClass == DIGIT)
        {
            addChar();
            getChar();
        }
        nextToken = "INT_LIT";
        break;
    /* Parentheses and operators */
    case UNKNOWN:
        lookup(nextChar);
        getChar();
        break;
    /* EOF */
    case EOF:
        nextToken = "END_OF_FILE";
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = 0;
        break;
    }
    if (nextToken != "END_OF_FILE")
    {
        fprintf(out_fp, "Next token is: %s\n", nextToken.c_str());
    }
    return nextToken;
}

/* <program> → PROGRAM {<assign>;} END */
void program()
{
    fprintf(out_fp, "Enter <program>\n");
    if (nextToken == "PROGRAM_KEYWORD")
    {
        lex();
        while (nextToken != "END_KEYWORD" && nextToken != "END_OF_FILE")
        {
            assign();
            if (nextToken == "SEMICOLON")
            {
                lex();
            }
            else
            {
                fprintf(out_fp, "**ERROR** – expected semicolon\n");
                fprintf(out_fp, "Exit <program>\n");
                return;
            }
        }
        if (nextToken != "END_KEYWORD")
        {
            fprintf(out_fp, "**ERROR** – expected end keyword\n");
        }
    }
    else
    {
        fprintf(out_fp, "**ERROR** – expected program keyword\n");
    }
    fprintf(out_fp, "Exit <program>\n");
}

/* <assign> -> IDENT = <expr> */
void assign()
{
    fprintf(out_fp, "Enter <assign>\n");
    if (nextToken == "IDENT")
    {
        lex();
        if (nextToken == "ASSIGN_OP")
        {
            lex();
            expr();
        }
    }
    fprintf(out_fp, "Exit <assign>\n\n");
}

/* <expr> -> <term> {(+ | -) <term>} */
void expr()
{
    fprintf(out_fp, "Enter <expr>\n");
    term();
    while (nextToken == "ADD_OP" || nextToken == "SUB_OP")
    {
        lex();
        term();
    }
    fprintf(out_fp, "Exit <expr>\n");
}

/* <term> -> <factor> {(* | /) <factor>) */
void term()
{
    fprintf(out_fp, "Enter <term>\n");
    factor();
    while (nextToken == "MULT_OP" || nextToken == "DIV_OP")
    {
        lex();
        factor();
    }
    fprintf(out_fp, "Exit <term>\n");
}

/* <factor> -> id | int_constant | ( <expr ) */
void factor()
{
    fprintf(out_fp, "Enter <factor>\n");
    if (nextToken == "IDENT" || nextToken == "INT_LIT")
    {
        lex();
    }
    else
    {
        if (nextToken == "LEFT_PAREN")
        {
            lex();
            expr();
            if (nextToken == "RIGHT_PAREN")
            {
                lex();
            }
            else
            {
                fprintf(out_fp, "**ERROR** – expected right-parenthesis\n");
            }
        }
        else
        {
            fprintf(out_fp, "**ERROR** – expected identifier, integer or left-parenthesis\n");
        }
    }
    fprintf(out_fp, "Exit <factor>\n");
}