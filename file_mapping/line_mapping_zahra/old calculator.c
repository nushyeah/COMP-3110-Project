#include <unistd.h>   // for read() and write() system calls
#include <string.h>   // for using strlen()

/*

    Converting a string vlaue like "-12.3" into  double value
    Handling optional minus signs and decimal points
*/
double str_to_double(char *str) {
    double result = 0.0;
    int i = 0;
    int sign = 1;

    if (str[i] == '-') {
        sign = -1;
        i++;
    }

    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }

    if (str[i] == '.') {
        i++;
        double divisor = 10.0;
        while (str[i] >= '0' && str[i] <= '9') {
            result += (str[i] - '0') / divisor;
            divisor *= 10.0;
            i++;
        }
    }

    return result * sign;
}

/*
   
    Converting a double (19.6) into a string like "19.60"
    Rounding to 2 decimal places
*/
void double_to_str(double num, char *buf) {
    int int_part = (int)num;
    double frac_part = num - int_part;
    if (frac_part < 0) frac_part *= -1;

    if (int_part == 0 && num < 0) *buf++ = '-';

    char temp[20];
    int i = 0, j, is_neg = 0;

    if (int_part < 0) {
        is_neg = 1;
        int_part = -int_part;
    }

    do {
        temp[i++] = '0' + (int_part % 10);
        int_part /= 10;
    } while (int_part > 0);

    if (is_neg) temp[i++] = '-';

    for (j = i - 1; j >= 0; j--) *buf++ = temp[j];

    *buf++ = '.';

    frac_part *= 100;
    int frac = (int)(frac_part + 0.5);
    buf[0] = '0' + (frac / 10);
    buf[1] = '0' + (frac % 10);
    buf[2] = '\0';
}

/*
    
    making surethe string is a valid number format
    wil accept digits, optional one dot or any optional starting '-'
    will reject any letters, multiple dots or symbols
*/
int is_valid_number(char *str) {
    int i = 0, has_dot = 0, has_digit = 0;

    if (str[i] == '-') i++;

    for (; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            has_digit = 1;
        } else if (str[i] == '.') {
            if (has_dot) return 0;
            has_dot = 1;
        } else {
            return 0;
        }
    }

    return has_digit;
}

int main() {
    char buffer[100];        //  user input
    char left[50], right[50]; // Parsed operands
    char output[50];         // Final result string 
    char operator;
    int i = 0, j = 0;

    // asking user for input
    char prompt[] = "Enter expression (e.g. 12.5+3.4): ";
    write(1, prompt, strlen(prompt));

    // reading user input
    int bytesRead = read(0, buffer, sizeof(buffer));
    buffer[bytesRead] = '\0';

    // and skipping any leading spaces
    while (buffer[i] == ' ') i++;

    // Parsing left operand
    j = 0;
    if (buffer[i] == '-') left[j++] = buffer[i++];
    while ((buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '.') {
        left[j++] = buffer[i++];
    }
    left[j] = '\0';

    // Skipping spaces before operator
    while (buffer[i] == ' ') i++;

    // again parsing operator
    if (buffer[i] == '+' || buffer[i] == '-') {
        operator = buffer[i++];
    } else {
        char err[] = "Invalid input: expected '+' or '-'\n";
        write(1, err, strlen(err));
        return 1;
    }

    // skipping spaces before second number
    while (buffer[i] == ' ') i++;

    // Parsing right operand
    j = 0;
    if (buffer[i] == '-') right[j++] = buffer[i++];
    while ((buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '.') {
        right[j++] = buffer[i++];
    }
    right[j] = '\0';

    // Validating both operands
    if (!is_valid_number(left) || !is_valid_number(right)) {
        char err[] = "Invalid number format.\n";
        write(1, err, strlen(err));
        return 1;
    }

    // Converting strings to doubles and performing the operation
    double num1 = str_to_double(left);
    double num2 = str_to_double(right);
    double result = (operator == '+') ? num1 + num2 : num1 - num2;

    // Converting result back to string here
    double_to_str(result, output);

    // finally displaying the result using write()
    char resultMsg[] = "Result: ";
    write(1, resultMsg, strlen(resultMsg));
    write(1, output, strlen(output));
    write(1, "\n", 1);

    return 0;
}
