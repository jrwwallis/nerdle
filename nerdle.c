#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef NSEC_DIFF
static uint64_t prev;
#endif /* NSEC_DIFF */

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



static void gen_nz_digits(int level, int br_level, char * buf, int is_float);
static void gen_digits(int level, int br_level, char * buf, int ndigits, int is_float);
static inline void gen_oper(int level, int br_level, char * buf, int is_float);
static inline void gen_squared(int level, int br_level, char * buf, int ndigits, int is_float);
static inline void gen_cubed(int level, int br_level, char * buf, int ndigits, int is_float);
static inline void gen_open(int level, int br_level, char * buf, int is_float);
static inline void gen_close(int level, int br_level, char * buf, int is_float);
static inline void gen_equals(int level, int br_level, char * buf, int is_float);

static void
gen_digit (int level, int br_level, int digit, char * buf, int ndigits, int is_float)
{
    int next_level = level + 1;

    //if (strncmp(buf, "1-65536", 7) == 0) {
    //  puts("this");
    //}
    buf[level] = '0' + digit;
    gen_equals(next_level, br_level, buf, is_float);
    gen_digits(next_level, br_level, buf, ndigits, is_float);
    gen_oper(next_level, br_level, buf, is_float);
    gen_squared(next_level, br_level, buf, 1, is_float);
    gen_cubed(next_level, br_level, buf, 1, is_float);
    if (br_level > 0) {
        gen_close(next_level, br_level, buf, is_float);
    }
}

static void
gen_nz_digits (int level, int br_level, char * buf, int is_float)
{
    int i;
    if (level >= 8) {
        return;
    }

    //int next_level = level + 1;
    
    for (i = 1; i < 10; i++) {
        gen_digit(level, br_level, i, buf, 0, is_float);
#if 0
        buf[level] = '0' + i;
        gen_equals(next_level, br_level, buf, is_float);
        gen_digits(next_level, br_level, buf, 1, is_float);
        gen_oper(next_level, br_level, buf, is_float);
        gen_squared(next_level, br_level, buf, 1, is_float);
        gen_cubed(next_level, br_level, buf, 1, is_float);
        if (br_level > 0) {
            gen_close(next_level, br_level, buf, is_float);
        }
#endif
    }
}

static void
gen_digits (int level, int br_level, char * buf, int ndigits, int is_float)
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
        gen_digit(level, br_level, i, buf, ndigits, is_float);
#if 0
        buf[level] = '0' + i;
        gen_equals(next_level, br_level, buf, is_float);
        gen_digits(next_level, br_level, buf, ndigits, is_float);
        gen_oper(next_level, br_level, buf, is_float);
        gen_squared(level + 1, br_level, buf, ndigits, is_float);
        gen_cubed(next_level, br_level, buf, ndigits, is_float);
        if (br_level > 0) {
            gen_close(next_level, br_level, buf, is_float);
        }
#endif
    }
    gen_digit(level, br_level, 0, buf, ndigits, is_float);
#if 0
    buf[level] = '0';
    gen_equals(next_level, br_level, buf, is_float);
    gen_digits(next_level, br_level, buf, ndigits, is_float);
    gen_oper(next_level, br_level, buf, is_float);
    gen_squared(level + 1, br_level, buf, ndigits, is_float);
    gen_cubed(next_level, br_level, buf, ndigits, is_float);
    if (br_level > 0) {
        gen_close(next_level, br_level, buf, is_float);
    }
#endif
}

static inline void
gen_oper (int level, int br_level, char * buf, int is_float)
{
    if (level >= 7) {
        return ;
    }

    int next_level = level + 1;

    buf[level] = '-';
    gen_nz_digits(next_level, br_level, buf, is_float);
    gen_open(next_level, br_level, buf, is_float);

    buf[level] = '+';
    gen_nz_digits(next_level, br_level, buf, is_float);
    gen_open(next_level, br_level, buf, is_float);

    buf[level] = '*';
    gen_nz_digits(next_level, br_level, buf, is_float);
    gen_open(next_level, br_level, buf, is_float);

    buf[level] = '/';
    gen_nz_digits(next_level, br_level, buf, 1);
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
        gen_equals(next_level, br_level, buf, 1);
    }
    gen_oper(next_level, br_level, buf, is_float);
    if (br_level > 0) {
        gen_close(next_level, br_level, buf, 1);
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
    gen_nz_digits(next_level, br_level, buf, is_float);
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
    if (is_float) {
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
#ifdef NSEC_DIFF
            struct timespec ts;
            static uint64_t now;
            clock_gettime(CLOCK_REALTIME, &ts);
            now = (ts.tv_sec * 1000000000) + ts.tv_nsec;
            fprintf(fp, "%" PRIu64 ", %s=%d\n", now - prev, buf, rhs);
            prev = now;
#else
            fprintf(fp, "%s=%d\n", buf, rhs);
#endif /* NSEC_DIFF */
	}
    }
}


int
main (int argc, char ** argv)
{
    fp = fopen("nerdle.txt", "w");
    char buffer[20] = "";
    
#ifdef NSEC_DIFF   
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    prev = (ts.tv_sec * 1000000000) + ts.tv_nsec;
#endif /* NSEC_DIFF */
    gen_nz_digits(0, 0, buffer, 0);
    gen_open(0, 0, buffer, 0);
    fclose(fp);
}
