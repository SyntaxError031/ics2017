#include "nemu.h"
#include "cpu/reg.h"
#include "memory/memory.h"
#include "cpu/cpu.h"
#include "symtab.h"

#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ,
	DEC_NUM, HEX_NUM,
	REG,VAR

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// white space
	{"\\+", '+'},
	{"-", '-'},
	{"\\*", '*'},
	{"\\/", '/'},
	{"\\(", '('},
	{"\\)", ')'},
	{"==", EQ},
	{"[1-9][0-9]*", DEC_NUM},
	{"0[xX][0-9a-fA-F]+", HEX_NUM},
	{"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip)", REG},
	{"[a-zA-Z_][a-zA-Z_0-9]*", VAR}

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

typedef struct element {
	int type;
	int num;
	int priority;
} Element;

Token tokens[32];
Element elements[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				//char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				//printf("match regex[%d] at position %d with len %d: %.*s", i, position, substr_len, substr_len, substr_start);

				int j;
				switch(rules[i].token_type) {
					case NOTYPE: break;
					case DEC_NUM: tokens[nr_token].type = 10;
						      for(j = 0; j < substr_len; j++) {
							      tokens[nr_token].str[j] = e[j+position];
						      }
						      tokens[nr_token].str[j] = '\0';
						      nr_token++; break;
					case HEX_NUM: tokens[nr_token].type = 16;
						      for(j = 0; j < substr_len; j++) {
							      tokens[nr_token].str[j] = e[j+position];
						      }
						      tokens[nr_token].str[j] = '\0';
						      nr_token++; break;
					case REG: tokens[nr_token].type = 0;
							  for(j = 0; j < substr_len; j++) {
							      tokens[nr_token].str[j] = e[j+position];
						      }
						      tokens[nr_token].str[j] = '\0';
						      nr_token++; break;
					case VAR: tokens[nr_token].type = 2;
							  for(j = 0; j < substr_len; j++) {
							      tokens[nr_token].str[j] = e[j+position];
						      }
						      tokens[nr_token].str[j] = '\0';
						      nr_token++; break;
					default: tokens[nr_token].type = rules[i].token_type;
							 nr_token ++;
				}
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */
				
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

bool check_parentheses() {
	int count = 0;
	for (int i = 0; i < nr_token; i++)
	{
		if (elements[i].type == 2 && elements[i].num == 5)
			count++;
		else if (elements[i].type == 2 && elements[i].num == 6)
			count--;
		if (count < 0)
			return false;
	}
	if (count != 0)
		return false;
	return true;
}

void check_priority() {
	int left_count = 0;
	for (int i = 0; i < nr_token; i++)
	{
		if (elements[i].type == 2 && elements[i].num == 5)
			left_count++;
		else if (elements[i].type == 2 && elements[i].num == 6)
			left_count--;
		if (elements[i].type == 2 && elements[i].num != 5 && elements[i].num != 6)
			elements[i].priority += left_count * 3;
	}
}

void remove_parentheses() {
	for(int i = 0; i < nr_token;) {
		if(elements[i].type == 2 && (elements[i].num == 5 || elements[i].num == 6)) {
			for(int j = i; j < nr_token-1; j++) {
				elements[j].type = elements[j+1].type;
				elements[j].num = elements[j+1].num;
				elements[j].priority = elements[j+1].priority;
			}
			nr_token--;
		}
		else
			i++;
	}
}
		
uint32_t eval(int s, int e) {
	if(s > e) {
		printf("Bad Expression!\n");
		return -1;
	}
	else if(s == e)
		return elements[e].num;
	else {
		int min_prio = 100;
		int min = 0;
		for(int i = s; i <= e; i++) {
			if (elements[i].type == 2 && elements[i].priority <= min_prio) {
				min_prio = elements[i].priority;
				min = i;
			}
		}
		if (elements[min].type == 2 && elements[min].num == 1)
			return eval(s, min-1) + eval(min+1, e);
		else if (elements[min].type == 2 && elements[min].num == 2)
			return eval(s, min-1) - eval(min+1, e);
		else if (elements[min].type == 2 && elements[min].num == 3)
			return eval(s, min-1) * eval(min+1, e);
		else if (elements[min].type == 2 && elements[min].num == 4)
			return eval(s, min-1) + eval(min+1, e);	
		else if (elements[min].type == 2 && elements[min].num == 7) {
			if (min == 0 || (!(elements[min-1].type == 2 && (elements[min-1].num == 7 || elements[min-1].num == 8))) || ((elements[min-1].type == 2 && (elements[min-1].num == 7 || elements[min-1].num == 8)) && elements[min-1].priority != elements[min].priority))
				return (-1)*eval(min+1, e);
			else {
				int j = min - 1;
				uint32_t temp = (-1)*eval(min+1, e);
				while (j >= 0 && elements[j].type == 2 && (elements[j].num == 7 || elements[j].num == 8))
				{
					if (elements[j].num == 7)
						temp = (-1)*temp;
					else if (elements[j].num == 8)
						temp = vaddr_read(temp, 0, 4);
					j--;
				}
				return temp;
			}
		}
		else if (elements[min].type == 2 && elements[min].num == 8) {
			if (min == 0 || (!(elements[min-1].type == 2 && (elements[min-1].num == 7 || elements[min-1].num == 8))) || ((elements[min-1].type == 2 && (elements[min-1].num == 7 || elements[min-1].num == 8)) && elements[min-1].priority != elements[min].priority))
				return vaddr_read(eval(min+1, e), 0, 4);
			else {
				int j = min - 1;
				uint32_t temp = vaddr_read(eval(min+1, e), 0, 4);
				while (j >= 0 && elements[j].type == 2 && (elements[j].num == 7 || elements[j].num == 8))
				{
					if (elements[j].num == 7)
						temp = (-1)*temp;
					else if (elements[j].num == 8)
						temp = vaddr_read(temp, 0, 4);
					j--;
				}
				return temp;
			}
		}	
		else
			return -1;
	}	
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	for(int i = 0; i < nr_token; i++) {
		switch(tokens[i].type) {
			case 10: elements[i].type = 1;
					elements[i].num = atoi(tokens[i].str);
					elements[i].priority = 0; break;
			case 16: elements[i].type = 1;
					int t;
					elements[i].num = 0;
					for(int j = 2; j < strlen(tokens[i].str); j++) {
						if(tokens[i].str[j] >= '0' && tokens[i].str[j] <= '9')
							t = tokens[i].str[j] - '0';
						else if(tokens[i].str[j] >= 'a' && tokens[i].str[j] <= 'z')
							t = tokens[i].str[j] - 'a' + 10;
						else
							t = tokens[i].str[j] - 'A' + 10;
						elements[i].num = elements[i].num * 16 + t;
					}
					elements[i].priority = 0; break;
			case '+': elements[i].type = 2;
						elements[i].num = 1;
						elements[i].priority = 1; break;
			case '-': elements[i].type = 2;
						elements[i].num = 2;
						elements[i].priority = 1; break;
			case '*': elements[i].type = 2;
						elements[i].num = 3;
						elements[i].priority = 2; break;
			case '/': elements[i].type = 2;
						elements[i].num = 4;
						elements[i].priority = 2; break;
			case '(': elements[i].type = 2;
						elements[i].num = 5;
						elements[i].priority = 0; break;
			case ')': elements[i].type = 2;
						elements[i].num = 6;
						elements[i].priority = 0; break;
			
			case 0: elements[i].type = 1;
					if(tokens[i].str[2]=='a' && tokens[i].str[3] == 'x') elements[i].num = cpu.eax;
					if(tokens[i].str[2]=='c' && tokens[i].str[3] == 'x') elements[i].num = cpu.ecx;
					if(tokens[i].str[2]=='d' && tokens[i].str[3] == 'x') elements[i].num = cpu.edx;
					if(tokens[i].str[2]=='b' && tokens[i].str[3] == 'x') elements[i].num = cpu.ebx;
					if(tokens[i].str[2]=='s' && tokens[i].str[3] == 'p') elements[i].num = cpu.esp;
					if(tokens[i].str[2]=='b' && tokens[i].str[3] == 'p') elements[i].num = cpu.ebp;
					if(tokens[i].str[2]=='s' && tokens[i].str[3] == 'i') elements[i].num = cpu.esi;
					if(tokens[i].str[2]=='d' && tokens[i].str[3] == 'i') elements[i].num = cpu.edi;
					if(tokens[i].str[2]=='i' && tokens[i].str[3] == 'p') elements[i].num = cpu.eip;
					elements[i].priority = 0; break;
			case 2: {	*success = true;
						look_up_symtab(tokens[i].str,success);
						if(*success == true) {
							elements[i].type = 1;
							elements[i].num = look_up_symtab(tokens[i].str,success); 
							elements[i].priority = 0;
						}
						else {
							printf("Symtab not found!\n"); 
						}
					}
					break;
		}
	}

//deal with operators having different meanings
	
	for(int i = 0; i < nr_token; i++) {
		if(elements[i].type == 2 && elements[i].num == 2) {
			if(i == 0 || (elements[i-1].type == 2 && elements[i-1].num == 5)) {      //'-' is negative sign
				elements[i].num = 7;
				elements[i].priority = 3;
			}
		}
		else if(elements[i].type == 2 && elements[i].num == 3) {
			if(i == 0 || (elements[i-1].type == 2 && (elements[i-1].num == 5 || elements[i-1].num == 1 || elements[i-1].num == 2 || elements[i-1].num == 3 || elements[i-1].num == 4))) {       //'*' is pointer
				elements[i].num = 8;
				elements[i].priority = 3;
			}
		}
	}

	/*
	for(int i = 0; i < nr_token; i++) {
		printf("%d ",tokens[i].type);
		printf("%s\n",tokens[i].str);
	}
	for(int i = 0; i < nr_token; i++) {
		printf("%d ",elements[i].type);
		printf("%d ",elements[i].num);
		printf("%d\n",elements[i].priority);
	}
	*/

	if(check_parentheses()) {
		check_priority();
		remove_parentheses();
		/*
		for(int i = 0; i < nr_token; i++) {
		printf("%d ",elements[i].type);
		printf("%d ",elements[i].num);
		printf("%d\n",elements[i].priority);
		}
		*/
		return eval(0, nr_token-1);
	}
	else {
		printf("Parentheses do not match!\n");
		return -1;
	}

	//printf("\nPlease implement expr at expr.c\n");
	//assert(0);

}

