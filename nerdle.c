#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

char * expressionToParse;
FILE *fp;

char peek()
{
    return *expressionToParse;
}

char get()
{
    return *expressionToParse++;
}

double expression();

double number()
{
    double result = get() - '0';
    while (peek() >= '0' && peek() <= '9')
    {
        result = 10*result + get() - '0';
    }
    return result;
}

double factor()
{
    if (peek() >= '0' && peek() <= '9')
        return number();
    else if (peek() == '(')
    {
        get(); // '('
        double result = expression();
        get(); // ')'
        return result;
    }
    else if (peek() == '-')
    {
        get();
        return -factor();
    }
    return 0; // error
}

double expon()
{
    double result = factor();
    while (peek() == 's' || peek() == 'c')
        if (get() == 's')
            result *= result;
        else
            result *= (result * result);
    return result;
}

double term()
{
    double result = expon();
    while (peek() == '*' || peek() == '/')
        if (get() == '*')
            result *= expon();
        else
            result /= expon();
    return result;
}

double expression()
{
    double result = term();
    while (peek() == '+' || peek() == '-')
        if (get() == '+')
            result += term();
        else
            result -= term();
    return result;
}



static void gen_nz_digit(int level, int br_level, char * buf, int ndigits);
static void gen_digit(int level, int br_level, char * buf, int ndigits);
static inline void gen_oper(int level, int br_level, char * buf, int ndigits);
static inline void gen_squared(int level, int br_level, char * buf, int ndigits);
static inline void gen_cubed(int level, int br_level, char * buf, int ndigits);
static inline void gen_open(int level, int br_level, char * buf, int ndigits);
static inline void gen_close(int level, int br_level, char * buf, int ndigits);
static inline void gen_equals(int level, int br_level, char * buf, int ndigits);

static void
gen_nz_digit (int level, int br_level, char * buf, int ndigits)
{
    int i;

    if (level >= 8) {
        return;
    }

    int next_level = level + 1;
    
    for (i = 1; i < 10; i++) {
        buf[level] = '0' + i;
        gen_equals(next_level, br_level, buf, 0);
        gen_digit(next_level, br_level, buf, 1);
        gen_oper(next_level, br_level, buf, 0);
        gen_squared(next_level, br_level, buf, 0);
        gen_cubed(next_level, br_level, buf, 0);
        if (br_level > 0) {
            gen_close(next_level, br_level, buf, 0);
        }
    }
}

static void
gen_digit (int level, int br_level, char * buf, int ndigits)
{
    int i;

    if (level >= 8) {
        return;
    }
    if (ndigits >= 4) {
        return;
    }

    int next_level = level + 1;

    for (i = 1; i < 10; i++) {
        buf[level] = '0' + i;
        gen_equals(next_level, br_level, buf, 0);
        gen_digit(next_level, br_level, buf, ndigits + 1);
        gen_oper(next_level, br_level, buf, 0);
        gen_squared(level +1, br_level, buf, 0);
        gen_cubed(next_level, br_level, buf, 0);
        if (br_level > 0) {
            gen_close(next_level, br_level, buf, 0);
        }
    }
    buf[level] = '0';
    gen_equals(next_level, br_level, buf, 0);
    gen_digit(next_level, br_level, buf, ndigits + 1);
    gen_oper(next_level, br_level, buf, 0);
    gen_squared(level +1, br_level, buf, 0);
    gen_cubed(next_level, br_level, buf, 0);
    if (br_level > 0) {
      gen_close(next_level, br_level, buf, 0);
    }

}

static inline void
gen_oper (int level, int br_level, char * buf, int ndigits)
{
    if (level >= 7) {
        return ;
    }

    int next_level = level + 1;

    buf[level] = '-';
    gen_nz_digit(next_level, br_level, buf, 0);
    gen_open(next_level, br_level, buf, 0);

    buf[level] = '+';
    gen_nz_digit(next_level, br_level, buf, 0);
    gen_open(next_level, br_level, buf, 0);

    buf[level] = '*';
    gen_nz_digit(next_level, br_level, buf, 0);
    gen_open(next_level, br_level, buf, 0);

    buf[level] = '/';
    gen_nz_digit(next_level, br_level, buf, 0);
    gen_open(next_level, br_level, buf, 0);
}


static inline void
gen_squared (int level, int br_level, char * buf, int ndigits)
{
    if (level >= 8) {
        return;
    }

    int next_level = level + 1;

    buf[level] = 's';
    if (level >= 3) {
        gen_equals(next_level, br_level, buf, 0);
    }
    gen_oper(next_level, br_level, buf, 0);
    if (br_level > 0) {
        gen_close(next_level, br_level, buf, 0);
    }
}
        
static inline void
gen_cubed (int level, int br_level, char * buf, int ndigits)
{
    if (level >= 8) {
        return;
    }

    int next_level = level + 1;

    buf[level] = 'c';
    if (level >= 2) {
        gen_equals(next_level, br_level, buf, 0);
    }
    gen_oper(next_level, br_level, buf, 0);
    if (br_level > 0) {
        gen_close(next_level, br_level, buf, 0);
    }
}
        
static inline void
gen_open (int level, int br_level, char * buf, int ndigits)
{
    if (level >= 7) {
        return;
    }

    int next_level = level + 1;
    buf[level] = '(';
    br_level++;
    gen_nz_digit(next_level, br_level, buf, 0);
    gen_open(next_level, br_level, buf, 0);
}


static inline void
gen_close (int level, int br_level, char * buf, int ndigits)
{
    if (level >= 8) {
        return;
    }

    int next_level = level + 1;

    buf[level] = ')';
    br_level--;
    gen_equals(next_level, br_level, buf, 0);
    gen_oper(next_level, br_level, buf, 0);
    gen_squared(next_level, br_level, buf, 0);
    gen_cubed(next_level, br_level, buf, 0);
    if (br_level > 0) {
        gen_close(next_level, br_level, buf, 0);
    }
}

static inline int
int_len (int n) {
    if (n < 10) {
        return 1;
    } else if (n < 100) {
        return 2;
    } else if (n < 1000) {
        return 3;
    } else if (n < 10000) {
        return 4;
    } else if (n < 100000) {
        return 5;
    } else {
        return 6;
    }
}
    
static inline void
gen_equals (int level, int br_level, char * buf, int ndigits)
{
    double result;
    double int_part;
    double frac_part;
    
    if (br_level > 0) {
        return;
    }
    buf[level] = '\0';
    expressionToParse = buf;
    result = expression();
    frac_part = modf(result, &int_part);
    if (frac_part == 0) {
        int rhs = (int)int_part;
        if (rhs >= 0) {
            if (level + int_len(rhs) == 9) {
	        char out_str[20];
		char *ptr = out_str;
		char tmp_str[20];
		strcpy(out_str, buf);
		while (NULL != (ptr = strchr(ptr, 's'))) {
		    strcpy(tmp_str, ptr + 1);
		    sprintf(ptr, "²%s", tmp_str);
		}
		ptr = out_str;
		while (NULL != (ptr = strchr(ptr, 'c'))) {
		    strcpy(tmp_str, ptr + 1);
		    sprintf(ptr, "³%s", tmp_str);
		}
			   
	        fprintf(fp, "%s=%d\n", out_str, rhs);
            }
        }
    }
}

int
main (int argc, char ** argv)
{
    fp = fopen("nerdle.txt", "w");
    char buffer[20] = "";
    gen_nz_digit(0, 0, buffer, 0);
    gen_open(0, 0, buffer, 0);
    fclose(fp);
}
