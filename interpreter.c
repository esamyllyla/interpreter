#include <stdio.h>

enum
{
    INTEGER, PLUS, MINUS, MULTIPLY, DIVISION, SPACE, INVALID
};

struct token
{
    int type;
    int value;
};

void clear_input_buffer(char *input, int size);
int interpreter(char *input, int input_length, int *result);
int update_result(int operation, int left_operand, int right_operand);
int fetch_factor(char *input, int *input_char_index, int *result, int input_length);
int fetch_operator(char *input, int *input_char_index, int *operator, int input_length);
int operate_term(char *input, int *input_char_index, int *result, int input_length);

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
    int input_char_index = 0;
    int operator = 0;

    int term;
    int term2;

    int rval = 0;
    /*
    rval = operate_term(input, &input_char_index, &term, input_length);
    printf("result: %d, char index: %d\n", term, input_char_index);
    
    *result = term;
    */
    if(operate_term(input, &input_char_index, &term, input_length))
    {
        while(input_char_index < input_length)
        {
            if(fetch_operator(input, &input_char_index, &operator, input_length))
            {
                if(!operate_term(input, &input_char_index, &term2, input_length))
                {
                    return 0;
                }
                else
                {
                    term = update_result(operator, term, term2);
                    *result = term;
                }
            }
            else
            {
                return 0;
            }
        }
        *result = term;
        return 1;
    }
    else return 0;
    
    return rval;
}

int operate_term(char *input, int *input_char_index, int *result, int input_length)
{
    int operator = 0;
    
    int term;
    int term2;
    
    if(fetch_factor(input, input_char_index, &term, input_length))
    {
        while(!(input[*input_char_index] == '+')
            ^ !(input[*input_char_index] == '-')
            ^ (*input_char_index < input_length))
            /**/
        {
            if(fetch_operator(input, input_char_index, &operator, input_length))
            {
                if(!fetch_factor(input, input_char_index, &term2, input_length))
                {
                    return 0;
                }
                else
                {
                    term = update_result(operator, term, term2);
                    *result = term;
                }
            }
            else
            {
                return 0;
            }
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
        case DIVISION:
            return left_operand / right_operand;
    }
    
    return left_operand;
}

int fetch_factor(char *input, int *input_char_index, int *result, int input_length)
// Returns boolean telling if succeeded
{
    while(input[*input_char_index] == ' ' && *input_char_index < input_length)
    {
        *input_char_index = *input_char_index + 1;
    }
    
    if(!(48 <= input[*input_char_index] && input[*input_char_index] <= 57))
    {
        return 0;
    }
    
    int term = 0;
    while(48 <= input[*input_char_index] && input[*input_char_index] <= 57)
    {
        term *= 10;
        term += input[*input_char_index] - 48;
        
        *input_char_index = *input_char_index + 1;
    }
    
    *result = term;
    
    while(input[*input_char_index] == ' ' && *input_char_index < input_length)
    {
        *input_char_index = *input_char_index + 1;
    }
    return 1;
}

int fetch_operator(char *input, int *input_char_index, int *operator, int input_length)
{
    while(*input_char_index < input_length)
    {
        if(input[*input_char_index] == ' ')
        {
            *input_char_index = *input_char_index + 1;
            continue;
        }
        if(input[*input_char_index] == '+')
        {   
            *operator = PLUS;
            *input_char_index = *input_char_index + 1;
            return 1;
        }
        if(input[*input_char_index] == '-')
        {
            *operator = MINUS;
            *input_char_index = *input_char_index + 1;
            return 1;
        }
        if(input[*input_char_index] == '*')
        {
            *operator = MULTIPLY;
            *input_char_index = *input_char_index + 1;
            return 1;
        }
        if(input[*input_char_index] == '/')
        {
            *operator = DIVISION;
            *input_char_index = *input_char_index + 1;
            return 1;
        }
        *input_char_index = *input_char_index + 1;
    }
    
    return 0;
}
