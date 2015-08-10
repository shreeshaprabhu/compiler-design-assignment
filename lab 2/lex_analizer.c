/* program to implement lexical analyzer for c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// buffer to store file content
char buffer[102401];

// array of keywords
char keywords[][10] = { "auto", "break", "case",
"char", "const", "continue", "default", "do",
"double", "else", "enum", "extern", "float",
"for", "goto", "if", "int", "long",
"register", "return", "short", "signed",
"sizeof", "static", "struct", "switch",
"typedef", "union", "unsigned", "void",
"volatile", "while" };

// array of operators
char operators[][4] = { "<<=", ">>=", "+=", "-=",
"*=", "/=", "%=", "&=", "|=", "^=", "++", "--",
"->", "<<", ">>", "<=", ">=", "==", "!=", "&&", "||",
"=", "+", "-", "*", "/", "%", "<", ">", "!",
"&", "|", "~", "^", "?", ":", ".", "#", "[", "]", "(", ")" };

char seperators[] = { ' ', '\t', '\n', '\r' };

int isSpaceChar(char ch)
{
	int i = 0;
	for (i = 0; i < 4; i++)
		if (ch == seperators[i])
			return 1;
	return 0;
}

int isComment(char *cptr)
{
	if (*cptr == '/' && *(cptr + 1) == '/')
		return 1;
	if (*cptr == '/' && *(cptr + 1) == '*')
		return 2;
	return 0;
}

int isOperator(char *cptr)
{
	int i, j;
	for (i = 0; i < 42; i++)
	{
		for (j = 0; operators[i][j]; j++)
			if (operators[i][j] != *(cptr + j))
				break;
		if (!operators[i][j])
			return i;
	}
	return -1;
}

int isKeyword(char *cptr)
{
	int i, j, ch;
	for (i = 0; i < 32; i++)
	{
		for (j = 0; keywords[i][j]; j++)
			if (keywords[i][j] != *(cptr + j))
				break;
		ch = *(cptr + j);
		if (!keywords[i][j] && ch != '_'
			&& (ch < 'a' || ch > 'z')
			&& (ch < 'A' || ch > 'Z')
			&& (ch < '0' || ch > '9'))
			return i;
	}
	return -1;
}

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		printf("Pass filename as command line argument\n");
		return 0;
	}
	
	FILE *fp;
	char *cptr, ch, lit;
	int i;

	/* try to open file and copy content 
	   to character array */
	if (!(fp = fopen(argv[1], "r")))
	{
		printf("File cannot be opened\n");
		exit(1);
	}
	// copy file content to buffer
	i = 0;
	while ((ch = fgetc(fp)) != EOF)
		buffer[i++] = ch;
	buffer[i] = '\0';
	fclose(fp);

	cptr = buffer;
	while (*cptr)
	{
		while (isSpaceChar(*cptr))
			cptr++;
		ch = *cptr;

		// for character/string literal
		if (ch == '\'' || ch =='\"')
		{
			lit = ch;
			printf("%c", lit);
			cptr++;
			while (!(*cptr == lit && *(cptr - 1) != '\\'))
			{
				printf("%c", *cptr);
				if (*cptr == '\\' && *(cptr - 1) == '\\')
					*cptr = '\0';
				cptr++;
			}
			printf("%c :: is string/character literal\n", lit);
			cptr++;
		}

		// for comments
		else if ((lit = isComment(cptr)) != 0)
		{
			printf("%c%c", *cptr, *(cptr + 1));
			cptr++;
			*cptr = '/';
			cptr++;
			while (1)
			{
				if (lit == 2 && *cptr == '/' && *(cptr - 1) == '*')
				{
					printf("/ :: is a multiline comment\n");
					cptr++;
					break;
				}
				if (lit == 1 && (*cptr == '\n' || *cptr == '\0'))
				{
					printf(" :: is a single line comment\n");
					break;
				}
				printf("%c", *cptr);
				cptr++;
			}
		}

		// for identifiers/keywords
		else if ((ch >= 'a' && ch <= 'z')
			|| (ch >= 'A' && ch <= 'Z')
			|| ch == '_')
		{
			if ((lit = isKeyword(cptr)) >= 0)
			{
				printf("%s :: is a keyword\n", keywords[lit]);
				cptr += strlen(keywords[lit]);
			}
			else
			{
				while ((ch >= 'a' && ch <= 'z')
					|| (ch >= 'A' && ch <= 'Z')
					|| (ch >= '0' && ch <= '9')
					|| ch == '_')
				{
					printf("%c", ch);
					cptr++;
					ch = *cptr;
				}
				printf(" :: is an identifier\n");
			}
		}

		// for constants
		else if (ch >= '0' && ch <= '9')
		{
			while ((ch >= '0' && ch <= '9')
				|| (ch >= 'a' && ch <= 'z')
				|| (ch >= 'A' && ch <= 'Z')
				|| ch == '.')
			{
				printf("%c", ch);
				cptr++;
				ch = *cptr;
			}
			printf(" :: is a constant\n");
		}

		// for operators
		else if ((lit = isOperator(cptr)) >= 0)
		{
			printf("%s :: is an operator\n", operators[lit]);
			cptr += strlen(operators[lit]);
		}

		else if (ch == '{' || ch == '}')
		{
			printf("%c :: is a bracket\n", ch);
			cptr++;
		}
		else if (ch == ';' || ch == ',')
		{
			printf("%c :: is a punctuator\n", ch);
			cptr++;
		}
	}
	return 0;
}
