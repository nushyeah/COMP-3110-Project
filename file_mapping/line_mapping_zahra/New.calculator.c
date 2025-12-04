#include <unistd.h>   // read(), write()
#include <string.h>   // strlen()

#define STDIN_FD   0
#define STDOUT_FD  1

#define INPUT_BUF_SIZE  128
#define TOKEN_BUF_SIZE   64

/*
    Converts a numeric string like "-12.3" into a double value.
    Handles:
      - optional leading minus sign
      - optional single decimal point
*/
double parse_double(const char *str) {
    double value = 0.0;
    int   index  = 0;
    int   sign   = 1;

    // sign handling
    if (str[index] == '-') {
        sign = -1;
        index++;
    }

    // integer part
    while (str[index] >= '0' && str[index] <= '9') {
        value = value * 10 + (str[index] - '0');
        index++;
    }

    // fractional part
    if (str[index] == '.') {
        index++;
        double divisor = 10.0;

        while (str[index] >= '0' && str[index] <= '9') {
            value += (str[index] - '0') / divisor;
            divisor *= 10.0;
            index++;
        }
    }

    return value * sign;
}

/*
    Converts a double (e.g., 19.6) into a string like "19.60".
    Always produces exactly two digits after the decimal point.
*/
void format_double(double number, char *buffer) {
    int    integerPart = (int)number;
    double fracPart    = number - integerPart;

    if (fracPart < 0) {
        fracPart *= -1;
    }

    // handle negative zero cases like -0.25
    if (integerPart == 0 && number < 0) {
        *buffer++ = '-';
    }

    char temp[TOKEN_BUF_SIZE];
    int  pos    = 0;
    int  isNeg  = 0;

    if (integerPart < 0) {
        isNeg       = 1;
        integerPart = -integerPart;
    }

    // build integer part in reverse
    do {
        temp[pos++] = '0' + (integerPart % 10);
        integerPart /= 10;
    } while (integerPart > 0);

    if (isNeg) {
        temp[pos++] = '-';
    }

    // copy integer part back in correct order
    for (int i = pos - 1; i >= 0; --i) {
        *buffer++ = temp[i];
    }

    *buffer++ = '.';

    // two decimal places, rounded
    fracPart *= 100.0;
    int frac = (int)(fracPart + 0.5);

    buffer[0] = '0' + (frac / 10);
    buffer[1] = '0' + (frac % 10);
    buffer[2] = '\0';
}

/*
    Validates whether the string is a numeric token.
    Rules:
      - optional single leading '-'
      - digits 0-9
      - at most one '.'
*/
int is_valid_number(const char *str) {
    int index    = 0;
    int hasDot   = 0;
    int hasDigit = 0;

    if (str[index] == '-') {
        index++;
    }

    for (; str[index] != '\0'; index++) {
        if (str[index] >= '0' && str[index] <= '9') {
            hasDigit = 1;
        } else if (str[index] == '.') {
            if (hasDot) {
                return 0;  // multiple dots
            }
            hasDot = 1;
        } else {
            return 0;      // invalid char
        }
    }

    return hasDigit;
}

/* Skip spaces starting from *idx and update *idx */
static void skip_spaces(const char *buffer, int *idx) {
    while (buffer[*idx] == ' ' || buffer[*idx] == '\t') {
        (*idx)++;
    }
}

int main(void) {
    char input[INPUT_BUF_SIZE];       // raw user input
    char left[TOKEN_BUF_SIZE];        // left operand
    char right[TOKEN_BUF_SIZE];       // right operand
    char resultStr[TOKEN_BUF_SIZE];   // formatted result
    char op;                          // operator (+ or -)

    int i = 0;
    int j = 0;

    const char prompt[] =
        "Simple calculator\n"
        "Enter expression (example: 12.5 + 3.4 or 12.5-3.4): ";

    write(STDOUT_FD, prompt, strlen(prompt));

    // read user input
    int bytesRead = read(STDIN_FD, input, sizeof(input) - 1);
    if (bytesRead <= 0) {
        const char errMsg[] = "No input received.\n";
        write(STDOUT_FD, errMsg, strlen(errMsg));
        return 1;
    }
    input[bytesRead] = '\0';

    // skip leading spaces
    skip_spaces(input, &i);

    // parse left operand
    j = 0;
    if (input[i] == '-') {           // optional minus sign
        left[j++] = input[i++];
    }
    while ((input[i] >= '0' && input[i] <= '9') || input[i] == '.') {
        left[j++] = input[i++];
    }
    left[j] = '\0';

    // spaces before operator
    skip_spaces(input, &i);

    // parse operator
    if (input[i] == '+' || input[i] == '-') {
        op = input[i++];
    } else {
        const char errOp[] = "Invalid input: expected '+' or '-'.\n";
        write(STDOUT_FD, errOp, strlen(errOp));
        return 1;
    }

    // spaces before right operand
    skip_spaces(input, &i);

    // parse right operand
    j = 0;
    if (input[i] == '-') {
        right[j++] = input[i++];
    }
    while ((input[i] >= '0' && input[i] <= '9') || input[i] == '.') {
        right[j++] = input[i++];
    }
    right[j] = '\0';

    // validate operands
    if (!is_valid_number(left) || !is_valid_number(right)) {
        const char errNum[] = "Invalid number format in expression.\n";
        write(STDOUT_FD, errNum, strlen(errNum));
        return 1;
    }

    // convert and compute
    double leftVal  = parse_double(left);
    double rightVal = parse_double(right);
    double result   = (op == '+') ? (leftVal + rightVal) : (leftVal - rightVal);

    // format result as string
    format_double(result, resultStr);

    // print output
    const char resultMsg[] = "Result: ";
    write(STDOUT_FD, resultMsg, strlen(resultMsg));
    write(STDOUT_FD, resultStr, strlen(resultStr));
    write(STDOUT_FD, "\n", 1);

    return 0;
}
