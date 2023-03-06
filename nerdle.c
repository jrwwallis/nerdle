#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

char * expressionToParse;
FILE *fp;

static inline char
peek ()
{
    return *expressionToParse;
}

static inline char
get ()
{
    return *expressionToParse++;
}

int expression();

int number()
{
    int n;
    int result = get() - '0';
    while (n = peek(), n >= '0' && n <= '9')
    {
        result = 10*result + get() - '0';
    }
    return result;
}

int factor()
{
    if (peek() >= '0' && peek() <= '9')
        return number();
    else if (peek() == '(')
    {
        get(); // '('
        int result = expression();
        get(); // ')'
        return result;
    }
    return 0; // error
}

int expon()
{
    int result = factor();
    while (peek() == 's' || peek() == 'c')
        if (get() == 's')
            result *= result;
        else
            result *= (result * result);
    return result;
}

int term()
{
    int result = expon();
    while (peek() == '*' || peek() == '/')
        if (get() == '*')
            result *= expon();
        else
            result /= expon();
    return result;
}

int expression()
{
    int result = term();
    while (peek() == '+' || peek() == '-')
        if (get() == '+')
            result += term();
        else
            result -= term();
    return result;
}



double expressiond();

double numberd()
{
    double n;
    double result = get() - '0';
    while (n = peek(), n >= '0' && n <= '9')
    {
        result = 10*result + get() - '0';
    }
    return result;
}

double factord()
{
    if (peek() >= '0' && peek() <= '9')
        return numberd();
    else if (peek() == '(')
    {
        get(); // '('
        double result = expressiond();
        get(); // ')'
        return result;
    }
    return 0; // error
}

double expond()
{
    double result = factord();
    while (peek() == 's' || peek() == 'c')
        if (get() == 's')
            result *= result;
        else
            result *= (result * result);
    return result;
}

double termd()
{
    double result = expond();
    while (peek() == '*' || peek() == '/')
        if (get() == '*')
            result *= expond();
        else
            result /= expond();
    return result;
}

double expressiond()
{
    double result = termd();
    while (peek() == '+' || peek() == '-')
        if (get() == '+')
            result += termd();
        else
            result -= termd();
    return result;
}



static void gen_nz_digit(int level, int br_level, char * buf, int is_float);
static void gen_digit(int level, int br_level, char * buf, int ndigits, int is_float);
static inline void gen_oper(int level, int br_level, char * buf, int is_float);
static inline void gen_squared(int level, int br_level, char * buf, int ndigits, int is_float);
static inline void gen_cubed(int level, int br_level, char * buf, int ndigits, int is_float);
static inline void gen_open(int level, int br_level, char * buf, int is_float);
static inline void gen_close(int level, int br_level, char * buf, int is_float);
static inline void gen_equals(int level, int br_level, char * buf, int is_float);

static void
gen_nz_digit (int level, int br_level, char * buf, int is_float)
{
    int i;
    if (level >= 8) {
        return;
    }

    int next_level = level + 1;
    
    for (i = 1; i < 10; i++) {
        buf[level] = '0' + i;
        gen_equals(next_level, br_level, buf, is_float);
        gen_digit(next_level, br_level, buf, 1, is_float);
        gen_oper(next_level, br_level, buf, is_float);
        gen_squared(next_level, br_level, buf, 1, is_float);
        gen_cubed(next_level, br_level, buf, 1, is_float);
        if (br_level > 0) {
            gen_close(next_level, br_level, buf, is_float);
        }
    }
}

static void
gen_digit (int level, int br_level, char * buf, int ndigits, int is_float)
{
    int i;

    if (level >= 8) {
        return;
    }

    if (ndigits >= 4) {
        return;
    }
    int next_level = level + 1;

    ndigits++;
    for (i = 1; i < 10; i++) {
        buf[level] = '0' + i;
        gen_equals(next_level, br_level, buf, is_float);
        gen_digit(next_level, br_level, buf, ndigits, is_float);
        gen_oper(next_level, br_level, buf, is_float);
        gen_squared(level + 1, br_level, buf, ndigits, is_float);
        gen_cubed(next_level, br_level, buf, ndigits, is_float);
        if (br_level > 0) {
            gen_close(next_level, br_level, buf, is_float);
        }
    }
    buf[level] = '0';
    gen_equals(next_level, br_level, buf, is_float);
    gen_digit(next_level, br_level, buf, ndigits, is_float);
    gen_oper(next_level, br_level, buf, is_float);
    gen_squared(level + 1, br_level, buf, ndigits, is_float);
    gen_cubed(next_level, br_level, buf, ndigits, is_float);
    if (br_level > 0) {
        gen_close(next_level, br_level, buf, is_float);
    }

}

static inline void
gen_oper (int level, int br_level, char * buf, int is_float)
{
    if (level >= 7) {
        return ;
    }

    int next_level = level + 1;

    buf[level] = '-';
    gen_nz_digit(next_level, br_level, buf, is_float);
    gen_open(next_level, br_level, buf, is_float);

    buf[level] = '+';
    gen_nz_digit(next_level, br_level, buf, is_float);
    gen_open(next_level, br_level, buf, is_float);

    buf[level] = '*';
    gen_nz_digit(next_level, br_level, buf, is_float);
    gen_open(next_level, br_level, buf, is_float);

    buf[level] = '/';
    gen_nz_digit(next_level, br_level, buf, 1);
    gen_open(next_level, br_level, buf, 1);
}


static inline void
gen_squared (int level, int br_level, char * buf, int ndigits, int is_float)
{
    if (level >= 8) {
        return;
    }

    if (ndigits >= 4) {
        return;
    }
    int next_level = level + 1;

    buf[level] = 's';
    if (level >= 3) {
        gen_equals(next_level, br_level, buf, is_float);
    }
    gen_oper(next_level, br_level, buf, is_float);
    if (br_level > 0) {
        gen_close(next_level, br_level, buf, is_float);
    }
}
        
static inline void
gen_cubed (int level, int br_level, char * buf, int ndigits, int is_float)
{
    if (level >= 8) {
        return;
    }

    if (ndigits >= 4) {
        return;
    }
    int next_level = level + 1;

    buf[level] = 'c';
    if (level >= 2) {
        gen_equals(next_level, br_level, buf, 1);
    }
    gen_oper(next_level, br_level, buf, 1);
    if (br_level > 0) {
        gen_close(next_level, br_level, buf, 1);
    }
}
        
static inline void
gen_open (int level, int br_level, char * buf, int is_float)
{
    if (level >= 7) {
        return;
    }

    if (br_level > 3) {
        return;
    }
    int next_level = level + 1;
    buf[level] = '(';
    br_level++;
    gen_nz_digit(next_level, br_level, buf, is_float);
    gen_open(next_level, br_level, buf, is_float);
}


static inline void
gen_close (int level, int br_level, char * buf, int is_float)
{
    if (level >= 8) {
        return;
    }

    int next_level = level + 1;

    buf[level] = ')';
    br_level--;
    gen_equals(next_level, br_level, buf, is_float);
    gen_oper(next_level, br_level, buf, is_float);
    gen_squared(next_level, br_level, buf, 0, is_float);
    gen_cubed(next_level, br_level, buf, 0, is_float);
    if (br_level > 0) {
        gen_close(next_level, br_level, buf, is_float);
    }
}


static inline int
int_len (int n) {
    if (n > 9999) {
        if (n > 99999) {
	    return 6;
	} else {
	    return 5;
	}
    } else {
        if (n > 999) {
	    return 4;
	} else if (n > 99) {
	    return 3;
	} else if (n > 9) {
	    return 2;
	} else {
	    return 1;
	}
    }
}
   
static inline void
gen_equals (int level, int br_level, char * buf, int is_float)
{
    double resultd;
    double int_part;
    double frac_part;
    int rhs;
    
    if (br_level > 0) {
        return;
    }
    buf[level] = '\0';
    expressionToParse = buf;
    if (1 || is_float) {
        resultd = expressiond();
	frac_part = modf(resultd, &int_part);
	if (frac_part != 0) {
	    return;
	}
	rhs = (int)int_part;
    } else {
        rhs = expression();
    }
    if (rhs >= 0) {
        if (level + int_len(rhs) == 9) {
	  #if 0
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
	     #endif
	    
	      fprintf(fp, "%s=%d\n", buf, rhs);
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
