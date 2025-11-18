#include <stdio.h>
#include <ctype.h>
#include <string>

using namespace std;

int charClass;
int lexLen;
char nextChar;
string lexeme;
string nextToken;
FILE *in_fp, *fopen();
FILE *out_fp;

void addChar();
void getChar();
void getNonBlank();
string lex();

#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

string PROGRAM_KEYWORD("PROGRAM");
string END_KEYWORD("END");

main()
{
    if ((in_fp = fopen("lexical_analyzer.in", "r")) == NULL)
        printf("ERROR - cannot open lexical_analyzer.in \n");
    else
    {
        out_fp = fopen("lexical_analyzer.out", "w");
        getChar();
        do
        {
            lex();
        } while (nextToken != "END_OF_FILE");
    }
    fprintf(out_fp, "\nLexical analysis of the program is complete!");
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
    if (nextToken == "SEMICOLON")
    {
        fprintf(out_fp, "Next token is: %-20s Next lexeme is %s\n\n", nextToken.c_str(), lexeme.c_str());
    }
    else
    {
        fprintf(out_fp, "Next token is: %-20s Next lexeme is %s\n", nextToken.c_str(), lexeme.c_str());
    }
    return nextToken;
}