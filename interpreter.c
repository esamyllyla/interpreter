#include <stdio.h>

#define INPUT_BUFFER_SIZE 64

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
int operate_expr(char *input, int *input_char_index, int *result, int input_length);

int main()
{
    char input[INPUT_BUFFER_SIZE];

    while(1)
    {
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

    return operate_expr(input, &input_char_index, result, input_length);
}

int operate_expr(char *input, int *input_char_index, int *result, int input_length)
{
    int operator = 0;
    
    int term;
    int term2;
    
    if(operate_term(input, input_char_index, &term, input_length))
    {
        while(!(input[*input_char_index] == ')')
            && (*input_char_index < input_length))
        {

            //printf("Calling fetch operator, inside operate_expr for character %c\n", input[*input_char_index]);
            if(fetch_operator(input, input_char_index, &operator, input_length))
            {
                if(!operate_term(input, input_char_index, &term2, input_length))
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

int continue_ok(char *input, int input_char_index, int input_length)
{
    if(input[input_char_index] == '+') return 0;
    if(input[input_char_index] == '-') return 0;
    if(input[input_char_index] == ')') return 0;
    if(input_char_index >= input_length) return 0;
    return 1;
}

int operate_term(char *input, int *input_char_index, int *result, int input_length)
{
    int operator = 0;
    
    int term;
    int term2;
    
    if(fetch_factor(input, input_char_index, &term, input_length))
    {
        //printf("Fetch factor returned (term): %d\n", term);
        while(continue_ok(input, *input_char_index, input_length))/*!(input[*input_char_index] == '+')
            ^ !(input[*input_char_index] == '-')
            ^ (*input_char_index < input_length))*/
        {
            //printf("Calling fetch operator, inside operate_term for character %c\n", input[*input_char_index]);
            if(fetch_operator(input, input_char_index, &operator, input_length))
            {
                if(!fetch_factor(input, input_char_index, &term2, input_length))
                {
                    return 0;
                }
                else
                {
                    //printf("Fetch factor returned (term2): %d\n", term2);
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
    
    if(input[*input_char_index] == '(')
    {
        // parse expression from this point on
        *input_char_index = *input_char_index + 1;
        //printf("Recursive expr call with param %d\n", *result);
        operate_expr(input, input_char_index, result, input_length);
        //printf("Recursive expr gave result: %d\n", *result);
        //printf("character at index when returned from recursion: %c\n", input[*input_char_index]);
        if(input[*input_char_index] == ')')
        {
            *input_char_index = *input_char_index + 1;
            return 1;
        }
        else return 0;
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
    // TODO: better way to check for false symbols
    
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
        if(input[*input_char_index] == '(')
        {
            //printf("Fetch operator returned 0!\n");
            return 0;
        }
        *input_char_index = *input_char_index + 1;
    }
    
    //printf("Fetch operator returned 0!\n");
    return 0;
}
