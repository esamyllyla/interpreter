#include <stdio.h>

#define INPUT_BUFFER_SIZE 64

enum
{
    EXPR, TERM
};

enum
{
    OP, INT
};

typedef struct node
{
    int type;
    int id;
    int left;
    int right;
} node;

#define TREE_SIZE 40

node AST[TREE_SIZE];
int node_counter = 0;

void clear_input_buffer(char *input, int size);
void eat_spaces(char* input, int* input_char_index, int input_length);
int fetch_factor(char* input, int* input_char_index, int input_length, int *result, int* subroot);
int interpreter(char *input, int input_length, int *result);
int expr(char* input, int* input_char_index, int input_length, int *result, int* subroot);
int term(char* input, int* input_char_index, int input_length, int *result, int* subroot);
int fetch_operator(char* input, int* input_char_index, int input_length, char *operator, int rule);
int make_binop(int op, int left, int right);
int expression_ended(char* input, int* input_char_index, int input_length, int rule);
void tree_to_dot_traverse(int node);
void tree_to_dot(int start);
int find_root();
void rpn(int node);
void post_order(int node);
int solve(int node);

void LISP(int node);
void LISP_traverse(int node);

int main()
{
    char input[INPUT_BUFFER_SIZE];

    for(int i = 0; i < TREE_SIZE; i++)
    {
        AST[i].type = -1;
        AST[i].id = -1;
        AST[i].left = -1;
        AST[i].right = -1;
    }

    int input_length = 0;
    clear_input_buffer(input, INPUT_BUFFER_SIZE);
    
    printf("calc>");
    fgets(input, sizeof(input), stdin);

    int result;

    while(input[input_length] != '\n')
    {
        input_length++;
    }
    if(interpreter(input, input_length, &result) == 0)
    {
        printf("Syntax error!\n");
    }

    /* Print nodes for debugging
    for(int i = 0; i < TREE_SIZE; i++)
    {
        // if node is a number
        if(AST[i].left == -1 && AST[i].right == -1)
        {
            printf("i: %d, id: %d, left: %d, right: %d\n", 
            i, AST[i].id, AST[i].left, AST[i].right);
        }
        else // else its an operation
            printf("i: %d, id: %c, left: %d, right: %d\n", 
            i, AST[i].id, AST[i].left, AST[i].right);
    }*/
    
    int root = find_root();
    tree_to_dot(root);
    //post_order(root);
    printf("\n");
    //LISP(root);
    result = solve(root);
    printf("result: %d\n", result);

    return 0;
}

int interpreter(char *input, int input_length, int *result)
{
    int input_char_index = 0;
    
    int subroot = 0;
    return expr(input, &input_char_index, input_length, result, &subroot);
}

int expr(char* input, int* input_char_index, int input_length, int *result, int* subroot)
{
    int l = 0;
    int r = 0;
    int left = 0;
    int right = 0;
    char op = 0;
    if(!term(input, input_char_index, input_length, &l, &left))
        return 0;
    while(!expression_ended(input, input_char_index, input_length, EXPR))
    {
        if(!fetch_operator(input, input_char_index, input_length, &op, EXPR))
            return 0;
        node_counter++;
        if(!term(input, input_char_index, input_length, &r, &right))
            return 0;
    
        //printf("expr. binop. left: %d, op: %c, right: %d\n", left, op, right);
        left = make_binop(op, left, right);
    }
    
    *subroot = left;
}

int term(char* input, int* input_char_index, int input_length, int *result, int* subroot)
{
    int l = 0;
    int r = 0;
    int left = 0;
    int right = 0;
    char op = 0;
    if(!fetch_factor(input, input_char_index, input_length, &l, &left))
        return 0;
    
    while(!expression_ended(input, input_char_index, input_length, TERM))
    {
        if(!fetch_operator(input, input_char_index, input_length, &op, TERM))
            return 0;
        node_counter++;
        if(!fetch_factor(input, input_char_index, input_length, &r, &right))
            return 0;
        
        //AST[right].id = r;
        //AST[right].type = INT;
        //printf("term. binop. left: %d, op: %c, right: %d\n", left, op, right);
        left = make_binop(op, left, right);
    }
    *subroot = left;
    
    //printf("subroot: %d\n", *subroot);
}

void clear_input_buffer(char *input, int size)
{
    for(int i = 0; i < size; i++)
    {
        input[i] = ' ';
    }
}

void eat_spaces(char* input, int* input_char_index, int input_length)
{
    while(input[*input_char_index] == ' ' && *input_char_index < input_length)
    {
        *input_char_index = *input_char_index + 1;
    }
}

int expression_ended(char* input, int* input_char_index, int input_length, int rule)
{
    if(*input_char_index == input_length) return 1;
    if((input[*input_char_index] == '+'
    ||  input[*input_char_index] == '-') && (rule == TERM)) return 1;
    if((input[*input_char_index] == '*'
    ||  input[*input_char_index] == '/') && (rule == EXPR)) return 1;
    if(input[*input_char_index] == ')') return 1;
    else return 0;
}

// Process a non-terminal factor
int fetch_factor(char* input, int* input_char_index, int input_length, int *result, int* subroot)
{
    // Eat spaces from the beginning of the string,
    // until next non-space symbol occurs
    eat_spaces(input, input_char_index, input_length);
    
    if(input[*input_char_index] == '+'
    || input[*input_char_index] == '-')
    {
        
        *subroot = node_counter;
        int unop_index = node_counter;
        AST[unop_index].id = input[*input_char_index];
        AST[unop_index].type = OP;
        AST[unop_index].right = -1;
        *input_char_index = *input_char_index + 1;
        eat_spaces(input, input_char_index, input_length);
        node_counter++;
        int left = 0;
        if(!fetch_factor(input, input_char_index, input_length, result, &left))
            return 0;
        AST[unop_index].left = left;
        return 1;
    }
    
    // If character is LPAR, execute expr
    if(input[*input_char_index] == '(')
    {
        //par_counter++;
        *input_char_index = *input_char_index + 1;
        
        //node_counter++;
        if(expr(input, input_char_index, input_length, result, subroot))
        {
            if(input[*input_char_index] == ')')
            {
                //par_counter--;
                *input_char_index = *input_char_index + 1;
                eat_spaces(input, input_char_index, input_length);
                
                return 1;
            }
            else return 0;
        }
        else return 0;
    }
    
    // Look for characters that are not numbers 0-9
    // 48 == '0', 57 == '9'
    if(!(48 <= input[*input_char_index] && input[*input_char_index] <= 57))
    {
        return 0;
    }
    
    int term = 0;
    // non-terminal factor consists of terminal of type INTEGER
    // Compute the integer value of the terminal
    // from input characters that are numbers 0-9    
    while(48 <= input[*input_char_index] && input[*input_char_index] <= 57)
    {
        term *= 10;
        term += input[*input_char_index] - 48;
        
        *input_char_index = *input_char_index + 1;
    }
    
    *result = term;
    
    // Eat spaces after the integer until next
    // non-space symbol occurs
    eat_spaces(input, input_char_index, input_length);
    
    *subroot = node_counter;
    AST[node_counter].id = term;
    AST[node_counter].type = INT;
    return 1;
}

int fetch_operator(char* input, int* input_char_index, int input_length, char *operator, int rule)
{
    if((input[*input_char_index] == '*'
    ||  input[*input_char_index] == '/') && rule == TERM)
    {
        *operator = input[*input_char_index];
        
        *input_char_index = *input_char_index + 1;
        return 1;
    }
    if((input[*input_char_index] == '+'
    ||  input[*input_char_index] == '-') && rule == EXPR)
    {
        *operator = input[*input_char_index];
        
        *input_char_index = *input_char_index + 1;
        return 1;
    }
    else
    {
        *input_char_index = *input_char_index + 1;
        return 0;
    }
}

int make_binop(int op, int left, int right)
{
    node_counter++;
    AST[node_counter].id = op;
    AST[node_counter].left = left;
    AST[node_counter].right = right;
    AST[node_counter].type = OP;
    
    return node_counter;
}

void tree_to_dot_traverse(int node)
{
    //if(AST[node].id == -1) return;
    
    if(AST[node].type == INT)
        printf("\tnx%p [label=\"%d\"]\n", &AST[node], AST[node].id);
    else printf("\tnx%p [label=\"%c\"]\n", &AST[node], AST[node].id);
    if(AST[node].left != -1)
    {
        tree_to_dot_traverse(AST[node].left);
        printf("\tnx%p -> nx%p\n", &AST[node], &AST[AST[node].left]);
    }
    if(AST[node].right != -1)
    {
        tree_to_dot_traverse(AST[node].right);
        printf("\tnx%p -> nx%p\n", &AST[node], &AST[AST[node].right]);
    }
}

void tree_to_dot(int start)
{
    printf("digraph {\n\tnode [shape=circle]\n");
    tree_to_dot_traverse(start);
    printf("}\n");
}

int find_root()
{
    for(int i = 0; i < TREE_SIZE; i++)
    {
        int node = i;
        int isroot = 1;
        for(int j = 0; j < TREE_SIZE; j++)
        {
            if(AST[j].left == i) 
            {
                isroot = 0;
                break;
            }
            if(AST[j].right == i)
            {
                isroot = 0;
                break;
            }
        }
        
        if(isroot) return i;
    }
}

void rpn(int node)
{
    post_order(node);
    printf("\n");
}

void post_order(int node)
{
    if(AST[node].left != -1)
    {
        post_order(AST[node].left);
    }
    if(AST[node].right != -1)
    {
        post_order(AST[node].right);
    }
    if(AST[node].type == INT)
        printf("%d ", AST[node].id);
    else printf("%c ", AST[node].id);
}

void LISP(int node)
{
    LISP_traverse(node);
    printf("\n");
}

void LISP_traverse(int node)
{
    int is_leaf = 0;
    
    if(AST[node].left == -1)
    {
        is_leaf = 1;
    }
    if(AST[node].right == -1)
    {
        is_leaf = 1;
    }
    
    if(!is_leaf)
    {
        printf("(");
        printf("%c", AST[node].id);
        LISP_traverse(AST[node].left);
    }
    else printf("% d", AST[node].id);
    if(!is_leaf)
    {
        LISP_traverse(AST[node].right);
        printf(")");
    }
}
/*
int solve(int node)
{
    int l;
    int r;
    int isLeaf = 1;
    
    if(AST[node].left != -1)
    {
        l = solve(AST[node].left);
        isLeaf = 0;
    }
    if(AST[node].right != -1)
    {
        r = solve(AST[node].right);
        isLeaf = 0;
    }
    if(isLeaf) return AST[node].id;
    else
    {
        if(AST[node].id == '*') return l * r;
        if(AST[node].id == '/') return l / r;
        if(AST[node].id == '+') return l + r;
        if(AST[node].id == '-') return l - r;
    }
}*/

int solve(int node)
{
    int isLeaf = 0;
    int isUnop = 0;
    
    if((AST[node].left == -1) && (AST[node].right == -1))
        isLeaf = 1;
    if((AST[node].left != -1) && (AST[node].right == -1))
        isUnop = 1;
    
    int l;
    int r;
    if(isLeaf)
        return AST[node].id;
    if(isUnop)
    {
        l = solve(AST[node].left);
        if(AST[node].id == '+') return l;
        if(AST[node].id == '-') return l * (-1);
    }
    if(!isUnop && !isLeaf) // isBinOp
    {
        l = solve(AST[node].left);
        r = solve(AST[node].right);
        if(AST[node].id == '*') return l * r;
        if(AST[node].id == '/') return l / r;
        if(AST[node].id == '+') return l + r;
        if(AST[node].id == '-') return l - r;
    }
}
