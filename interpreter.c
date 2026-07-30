#include <stdio.h>

enum
{
    INTEGER, PLUS, MINUS, MULTIPLY, SPACE, INVALID
};

struct token
{
    int type;
    int value;
};

void clear_input_buffer(char *input, int size);
int interpreter(char *input, int input_length, int *result);
struct token get_token(char *input, int token_index);
void eat_token(char *input, int *token_index, struct token *token);
int update_result(int operation, int left_operand, int right_operand);
int fetch_term(char *input, int *token_index, int *result, int input_length);
int fetch_operator(char *input, int *token_index, int *operator, int input_length);

int main()
{
    char input[32];

    while(1)
    {
        int input_length = 0;
        clear_input_buffer(input, 32);
        
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
        else
        {
            printf("%d\n", result);
        }
    }

    return 0;
}

void clear_input_buffer(char *input, int size)
{
    for(int i = 0; i < size; i++)
    {
        input[i] = ' ';
    }
}

/*
Summing up arbitrary amount of plus and minus operations
1                | left: 1
1 + 3            | left: 1, right: 3, op: +, res: 4
1 + 3 - 2        | left: 4, right: 2, op: -, res: 2
1 + 3 - 2 + 7    | left: 2, right: 7, op: +, res: 9
*/

int interpreter(char *input, int input_length, int *result)
{
    int token_index = 0;
    int left_operand = 0;
    int right_operand = 0;

    struct token current_token;

    int operator_eaten = 0;
    int operation = 0;

    int term;
    int term2;

    if(fetch_term(input, &token_index, &term, input_length))
    {
        while(token_index < input_length)
        {
            if(fetch_operator(input, &token_index, &operation, input_length))
            {
                if(!fetch_term(input, &token_index, &term2, input_length))
                {
                    return 0;
                }
                else
                {
                    term = update_result(operation, term, term2);
                    *result = term;
                }
            }
            else return 0;
        }
        *result = term;
        return 1;
    }
    else return 0;
}

int update_result(int operation, int left_operand, int right_operand)
{
    switch (operation)
    {
        case PLUS:
            return left_operand + right_operand;
        case MINUS:
            return left_operand - right_operand;
        case MULTIPLY:
            return left_operand * right_operand;
    }
    
    return left_operand;
}

int fetch_term(char *input, int *token_index, int *result, int input_length)
// Returns boolean telling if succeeded
{
    while(input[*token_index] == ' ' && *token_index < input_length)
    {
        *token_index = *token_index + 1;
    }
    
    if(!(48 <= input[*token_index] && input[*token_index] <= 57))
    {
        return 0;
    }
    
    int term = 0;
    while(48 <= input[*token_index] && input[*token_index] <= 57)
    {
        term *= 10;
        term += input[*token_index] - 48;
        
        *token_index = *token_index + 1;
    }
    
    *result = term;
    
    while(input[*token_index] == ' ' && *token_index < input_length)
    {
        *token_index = *token_index + 1;
    }
    return 1;
}

int fetch_operator(char *input, int *token_index, int *operator, int input_length)
{
    while(*token_index < input_length)
    {
        if(input[*token_index] == ' ')
        {
            *token_index = *token_index + 1;
            continue;
        }
        if(input[*token_index] == '+')
        {   
            *operator = PLUS;
            *token_index = *token_index + 1;
            return 1;
        }
        if(input[*token_index] == '-')
        {
            *operator = MINUS;
            *token_index = *token_index + 1;
            return 1;
        }
        *token_index = *token_index + 1;
    }
    
    return 0;
}

void eat_token(char *input, int *token_index, struct token *token)
{
    struct token thistoken = get_token(input, *token_index);
    token->type = thistoken.type;
    token->value = thistoken.value;
    
    *token_index = *token_index + 1;
}

struct token get_token(char *input, int token_index)
{
    struct token token;
    token.type = INVALID;
    token.value = 0;
    
    if(48 <= input[token_index] && input[token_index] <= 57)
    {
        token.type = INTEGER;
        token.value = input[token_index] - 48;
    }
    
    if(input[token_index] == '+')
    {
        token.type = PLUS;
        token.value = '+';
    }
    if(input[token_index] == '-')
    {
        token.type = MINUS;
        token.value = '-';
    }
    if(input[token_index] == '*')
    {
        token.type = MULTIPLY;
        token.value = '*';
    }
    if(input[token_index] == ' ')
    {
        token.type = SPACE;
        token.value = ' ';
    }
    
    return token;
}
