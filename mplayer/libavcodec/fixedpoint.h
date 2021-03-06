
#define FRAC_BIT   8
#define EXP_FRAC_BIT   16
#define FIX_DIV(a, b)   ((int)((((long long)(a))<<FRAC_BIT) / (b)))
#define FIX14_DIV(a, b)   ((int)((((long long)(a))<<14) / (b)))
#define FIX16_DIV(a, b)   ((int)((((long long)(a))<<16) / (b)))
#define FIX14_DIVINT(a, b)   ((int)(((((long long)(a))<<14) / (b)) >> 14))
#define FIX16_DIVINT(a, b)   ((int)(((((long long)(a))<<16) / (b)) >> 16))
#define FIX16_DIVLSFT(a, b, n)   ((int)(((((long long)(a))<<32) / (b)) >> (16-n)))
#define FIX_MULT(a, b)  ((int)((((long long)(a)) * (b)) >> FRAC_BIT))
#define FIX_MULT12(a, b)  ((int)((((long long)(a)) * (b)) >> 12))
#define FIX_MULT14(a, b)  ((int)((((long long)(a)) * (b)) >> 14))
#define FIX_MULT16(a, b)  ((int)((((long long)(a)) * (b)) >> 16))
#define FIX_MULT18(a, b)  ((int)((((long long)(a)) * (b)) >> 18))
#define FIX_MULT31(a, b)  ((int)((((long long)(a)) * (b)) >> 31))
#define FIXF2_MULT(a, b)  ((int)((((long long)(a)) * (b)) >> (2*FRAC_BIT)))
#define FIX_MULT3(a, b, c)  ((int)((((((long long)(a)) * (b)) >> FRAC_BIT) * (c)) >> FRAC_BIT))
#define FLOAT(a)  (float)((a) / ((float)(1 << FRAC_BIT)))
#define FLOAT16(a)  (float)((a) / ((float)(1 << 16)))
#define FLOAT14(a)  (float)((a) / ((float)(1 << 14)))
#define FIXED(a)  ((int)(((float)(a)) * (1 << FRAC_BIT)))
#define FIXED31(a)  ((int)(((float)(a)) * ((1 << 31)-1)))
#define FIXED16(a)  ((int)(((float)(a)) * (1 << 16)))
#define FIXED14(a)  ((int)(((float)(a)) * (1 << 14)))

