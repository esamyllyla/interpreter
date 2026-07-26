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

int interpreter(char *input, int input_length)
{
    int token_index = 0;
    int left_operand = 0;
    int right_operand = 0;

    struct token current_token;

    int operator_eaten = 0;
    int operation = 0;

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
            operator_eaten = 1;
            operation = current_token.type;
        }
        if(current_token.type == INVALID)
        {
            return 0;
        }
    }
    
    int result = 0;
    
    switch (operation)
    {
        case PLUS:
            result = left_operand + right_operand;
            break;
        case MINUS:
            result = left_operand - right_operand;
            break;
        case MULTIPLY:
            result = left_operand * right_operand;
            break;
    }
    
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
