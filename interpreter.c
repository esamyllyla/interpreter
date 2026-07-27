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
int interpreter(char *input, int input_length);
struct token get_token(char *input, int token_index);
void eat_token(char *input, int *token_index, struct token *token);
int update_result(int operation, int left_operand, int right_operand);

int main()
{
    char input[32];

    while(1)
    {
        int input_length = 0;
        clear_input_buffer(input, 32);
        
        printf("calc>");
        fgets(input, sizeof(input), stdin);
    
        while(input[input_length] != '\n')
        {
            input_length++;
        }
        if(interpreter(input, input_length) == 0)
        {
            printf("Invalid input!\n");
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
}

/*
Summing up arbitrary amount of plus and minus operations
1                | left: 1
1 + 3            | left: 1, right: 3, op: +, res: 4
1 + 3 - 2        | left: 4, right: 2, op: -, res: 2
1 + 3 - 2 + 7    | left: 2, right: 7, op: +, res: 9
*/

int interpreter(char *input, int input_length)
{
    int token_index = 0;
    int left_operand = 0;
    int right_operand = 0;

    struct token current_token;

    int operator_eaten = 0;
    int operation = 0;
    
    int result = 0;

    while(token_index < input_length)
    {
        eat_token(input, &token_index, &current_token);

        if(current_token.type == INTEGER)
        {
            if(operator_eaten)
            {
                right_operand *= 10;
                right_operand += current_token.value;
            }
            else
            {
                left_operand *= 10;
                left_operand += current_token.value;
            }
        }
        if((current_token.type != INTEGER)
         & (current_token.type != INVALID)
         & (current_token.type != SPACE))
        // If token type is NOT integer, space nor invalid, it must be a operator
        {
            operator_eaten++;
            // To sum up arbitrary amount of plus and minus operations
            // left operand has to be updated if more than one operators
            // have been eaten
            if(operator_eaten > 1)
            {
                left_operand = update_result(operation, left_operand, right_operand);
                right_operand = 0;
                operator_eaten = 1;
            }
            operation = current_token.type;
        }
        if(current_token.type == INVALID)
        {
            return 0;
        }
    }
    
    result = update_result(operation, left_operand, right_operand);
    
    printf("%d\n", result);
    return 1;
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
