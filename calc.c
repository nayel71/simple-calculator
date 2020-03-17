#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool is_digit(char c) {
	return '0' <= c && c <= '9';
}

bool is_space(char c) {
	return c == ' ';
}

void ignore(char **s) {
	while (*s && (is_digit(**s) || is_space(**s))) {
		(*s)++;
	}
}

// calculates an expression involving only numbers, + and -
int simple_calc(char *s) {
	int result = 0;
	
	while (*s) {
		switch (*s) {
		case '+': 
			switch (*(s + 1)) {
			// evaluate +- to - (for example, 1+-2 should be 1-2)
			case '-':
				++s;
				result -= atoi(++s);
				break;
			default:
				result += atoi(++s);
			}
			break;
		case '-':
			switch (*(s + 1)) {
			// evaluate -- to + (for example, 1--2 should be 1+2)
			case '-':
				++s;
				result += atoi(++s);
				break;
			default:
				result -= atoi(++s);
			}
			break;
		case '*':
			switch (*(s + 1)) {
			// *-
			case '-':
				++s;
				result *= -atoi(++s);
				break;
			default:
				result *= atoi(++s);
			}
			break;
		default:
			result = atoi(s);
		}

		ignore(&s);
	}
	
	return result;
}

int calc(char *s) {
	int len = strlen(s), open_pos = -1, close_pos;

	for (int i = 0; i < len; i++) {
		// evaluate innnermost brackets first
		switch (s[i]) {
		case '(':
			open_pos = i;
			break;
		case ')':
			// extract innermost part
			close_pos = i;
			char inner[close_pos - open_pos];
			memcpy(inner, s + open_pos + 1, close_pos - open_pos - 1);
			inner[close_pos - open_pos - 1] = '\0';

			// evaluate
			int result = simple_calc(inner);

			// convert to string
			int result_len = snprintf(NULL, 0, "%d", result);
			char result_str[result_len + 1];
			snprintf(result_str, result_len + 1, "%d", result);

			// substitute into string
			s[open_pos] = '\0';
			strcat(s, result_str);

			// append the remaining part
			char tail[len - close_pos];
			memcpy(tail, s + close_pos + 1, len - close_pos);
			strcat(s, tail);

			// update len and go back to beginning
			len -= close_pos - open_pos + 1 - result_len;
			i = -1;
		}
	}

	return simple_calc(s);
}			

int main(int argc, char **argv) {
	if (argc > 1) {
		printf("%d\n", calc(argv[1]));
	}
}
