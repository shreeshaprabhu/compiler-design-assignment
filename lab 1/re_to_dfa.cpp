/**
 * Program to convert regular expression into DFA and
 * check whether given string is accepted.
 * 
 * Examples of regular expressions :
 *     0.(0+1)*.0+0
 *     (0+1)*.1.(0+1).(0+1)
 *     (0+1)*.1.1.1.(0+1)*
 *     (1*.0.1*.0.1*)*+1*
 * 
 * Regular expression should not have spaces.
 * Regular expression should contain letters '0', '1', '.', '+', '*' or 'e'.
 * Regular expression should contain atmost 19 symbols ('0', '1' or 'e')
 */

#include <bits/stdc++.h>
#define MAX_SIZE (1 << 20)
using namespace std;

void prepare_postfix();
void parse_postfix();
void construct_dfa();
bool accept();

typedef struct node
{
	node *left;
	node *right;
	bool nullable;
	char data;
	bool firstpos[20];
	bool lastpos[20];
	int pos;
} node;

node *new_node(char data)
{
	node *nd = (node *) malloc(sizeof(node));
	nd->left = nd->right = NULL;
	nd->nullable = (data == '*' || data == 'e' || data == '#');
	nd->data = data;
	for (int i = 0; i < 21; i++)
		nd->firstpos[i] = nd->lastpos[i] = false;
	nd->pos = -1;
	return nd;
}

int incoming_priority(char ch)
{
	switch (ch)
	{
		case '(': return 6;
		case '.': return 4;
		case '+': return 2;
		default: return 0;
	}
}

int instack_priority(char ch)
{
	switch (ch)
	{
		case '.': return 5;
		case '+': return 3;
		case '(': return 1;
		default: return 0;
	}
}

// Regular Expression and its Postfix related data
char re[101]; // regular expression given
int reg_len; // legnth of regular expression
int pos; // symbol count in regular expression
node *postfix[101]; // postfix form of given re
int ind; // length of postfix

// Data related to Parse Tree and Follow POS
node *root; // root of the parse tree
char sym[21]; // sym[pos] gives symbol having position 'pos'
bool follow[20][20]; // follow[a][b] holds whether re[b] follows re[a]

// Data related to DFA
int init_state; // initial state
int num_states; // number of states
int delta[MAX_SIZE][2]; // transition function delta[Q][a] = d(Q, a)
bool visited[MAX_SIZE]; // to check whether a state is visited while constructing DFA

// input string
char str[1000001];

int main()
{
	/* input of regular expression */
	cout << "Enter a valid regular expression of language with alpahabet A = {0, 1} : " << endl;
	cin >> re;
	
	/* convert into postfix */
	prepare_postfix();
	
	/* construct parse tree and follow pos */
	parse_postfix();
	
	/* construct the dfa */
	construct_dfa();
	
	/* input of string to be tested */
	cout << "Enter the string to be tested : ";
	cin >> str;
	
	/* check whether string is accepted by DFA */
	if (accept())
		cout << "String is accepted by the DFA" << endl;
	else
		cout << "String is rejected by the DFA" << endl;
	
	return 0;
}

void prepare_postfix()
{
	int stk_top = 0;
	node *stk[41], *nd;
	
	pos = 0;
	for (int i = 0; re[i]; i++)
	{
		/* create new tree node and add to stack or postfix array */
		nd = new_node(re[i]);
		if (re[i] == '0' || re[i] == '1' || re[i] == 'e' || re[i] == '*')
		{
			if (re[i] != '*')
			{
				nd->pos = pos;
				sym[pos] = re[i];
				pos++;
			}
			postfix[ind] = nd;
			ind++;
		}
		else
		{
			if (re[i] == ')')
			{
				while (stk[stk_top - 1]->data != '(')
				{
					stk_top--;
					postfix[ind] = stk[stk_top];
					ind++;
				}
				stk_top--;
			}
			else
			{
				while (stk_top > 0 &&
					incoming_priority(re[i]) < instack_priority(stk[stk_top - 1]->data))
				{
					stk_top--;
					postfix[ind] = stk[stk_top];
					ind++;
				}
				stk[stk_top] = nd;
				stk_top++;
			}
		}
	}
	while (stk_top > 0)
	{
		stk_top--;
		postfix[ind] = stk[stk_top];
		ind++;
	}
	postfix[ind] = nd = new_node('#');
	nd->pos = pos;
	pos++;
	ind++;
	postfix[ind] = new_node('.');
	ind++;
	
	/* debug */
	/* for (int i = 0; i < ind; i++)
		cout << postfix[i]->data;
	cout << endl; */
}

void parse_postfix()
{
	int stk_top = 0;
	node *stk[41], *nd;
	
	/* initialize the table */
	for (int i = 0; i < pos; i++)
		for (int j = 0; j < pos; j++)
			follow[i][j] = false;
	
	for (int i = 0; i < ind; i++)
	{
		nd = postfix[i];
		switch (nd->data)
		{
		case '0':
		case '1':
		case '#':
			nd->firstpos[nd->pos] = nd->lastpos[nd->pos] = true;
		case 'e':
			stk[stk_top] = nd;
			stk_top++;
			break;
		case '+':
			stk_top--;
			nd->right = stk[stk_top];
			stk_top--;
			nd->left = stk[stk_top];
			nd->nullable = nd->left->nullable || nd->right->nullable;
			for (int j = 0; j < pos; j++)
			{
				nd->firstpos[j] = nd->left->firstpos[j] || nd->right->firstpos[j];
				nd->lastpos[j] = nd->left->lastpos[j] || nd->right->lastpos[j];
			}
			stk[stk_top] = nd;
			stk_top++;
			break;
		case '.':
			stk_top--;
			nd->right = stk[stk_top];
			stk_top--;
			nd->left = stk[stk_top];
			nd->nullable = nd->left->nullable && nd->right->nullable;
			for (int j = 0; j < pos; j++)
			{
				if (nd->left->nullable)
					nd->firstpos[j] = nd->left->firstpos[j] || nd->right->firstpos[j];
				else
					nd->firstpos[j] = nd->left->firstpos[j];
				if (nd->right->nullable)
					nd->lastpos[j] = nd->left->lastpos[j] || nd->right->lastpos[j];
				else
					nd->lastpos[j] = nd->right->lastpos[j];
			}
			
			for (int j = 0; j < pos; j++)
				if (nd->left->lastpos[j])
					for (int k = 0; k < pos; k++)
						follow[j][k] = follow[j][k] || nd->right->firstpos[k];
			
			stk[stk_top] = nd;
			stk_top++;
			break;
		case '*':
			stk_top--;
			nd->left = stk[stk_top];
			for (int j = 0; j < pos; j++)
			{
				nd->firstpos[j] = nd->left->firstpos[j];
				nd->lastpos[j] = nd->left->lastpos[j];
			}
			
			for (int j = 0; j < pos; j++)
				if (nd->lastpos[j])
					for (int k = 0; k < pos; k++)
						follow[j][k] = follow[j][k] || nd->firstpos[k];
			
			stk[stk_top] = nd;
			stk_top++;
			break;
		}
		
		/* debug */
		/* cout << nd->data << " ";
		for (int j = 0; j < pos; j++)
			cout << nd->firstpos[j];
		cout << " ";
		for (int j = 0; j < pos; j++)
			cout << nd->lastpos[j];
		cout << endl; */
	}
	root = stk[0];
	
	/* debug */
	/* for (int i = 0; i < pos; i++)
	{
		for (int j = 0; j < pos; j++)
			cout << follow[i][j] << " ";
		cout << endl;
	} */
}

void construct_dfa()
{
	for (int i = 0; i < (1 << pos); i++)
	{
		delta[i][0] = delta[i][1] = 0;
		visited[i] = false;
	}
	
	init_state = 0;
	for (int i = 0; i < pos; i++)
		if (root->firstpos[i])
			init_state |= (1 << i);
	
	queue<int> q;
	q.push(init_state);
	visited[init_state] = true;
	
	int cur_state, trans_0, trans_1;
	while (!q.empty())
	{
		cur_state = q.front();
		q.pop();
		
		for (int i = 0; i < pos; i++)
			for (int j = 0; (cur_state & (1 << i)) && j < pos; j++)
				if (follow[i][j])
					delta[cur_state][sym[i] - '0'] |= (1 << j);
		
		if (!visited[delta[cur_state][0]])
		{
			q.push(delta[cur_state][0]);
			visited[delta[cur_state][0]] = true;
		}
		if (!visited[delta[cur_state][1]])
		{
			q.push(delta[cur_state][1]);
			visited[delta[cur_state][1]] = true;
		}
	}
	
	/* debug */
	/* for (int i = 0; i < (1 << pos); i++)
		cout << i << " " << delta[i][0] << " " << delta[i][1] << endl; */
}

bool accept()
{
	int state = init_state;
	
	for (int i = 0; str[i]; i++)
		state = delta[state][str[i] - '0'];
	
	return (state & (1 << (pos - 1)));
}
