#include "postgres.h"
#include "fmgr.h"
#include "libpq/pqformat.h"
#include "utils/builtins.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

typedef uint32 base16_4b_t;

static char base16_chars[16] = "0123456789ABCDEF";
static char char_to_num_b16[] = {
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,0,1,2,3,4,5,6,7,8,9,127,127,127,127,127,127,127,10,11,12,13,14,
    15,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,10,11,12,13,14,15,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127};

#define BASE164BGetDatum(x)	 UInt32GetDatum(x)
#define DatumGetBASE164B(x)	 DatumGetUInt32(x)
#define PG_GETARG_BASE164B(n) DatumGetBASE164B(PG_GETARG_DATUM(n))
#define PG_RETURN_BASE164B(x) return BASE164BGetDatum(x)

Datum base16_4b_in(PG_FUNCTION_ARGS);
Datum base16_4b_out(PG_FUNCTION_ARGS);
Datum base16_4b_to_text(PG_FUNCTION_ARGS);
Datum text_to_base16_4b(PG_FUNCTION_ARGS);
Datum base16_4b_send(PG_FUNCTION_ARGS);
Datum base16_4b_recv(PG_FUNCTION_ARGS);
Datum base16_4b_lt(PG_FUNCTION_ARGS);
Datum base16_4b_le(PG_FUNCTION_ARGS);
Datum base16_4b_eq(PG_FUNCTION_ARGS);
Datum base16_4b_ne(PG_FUNCTION_ARGS);
Datum base16_4b_ge(PG_FUNCTION_ARGS);
Datum base16_4b_gt(PG_FUNCTION_ARGS);
Datum base16_4b_cmp(PG_FUNCTION_ARGS);

static base16_4b_t cstring_to_base16_4b(char *base16_4b_string);
static char *base16_4b_to_cstring(base16_4b_t base16_4b);



/* generic input/output functions */
PG_FUNCTION_INFO_V1(base16_4b_in);
Datum
base16_4b_in(PG_FUNCTION_ARGS)
{
	base16_4b_t	result;

	char   *base16_4b_str = PG_GETARG_CSTRING(0);
	result = cstring_to_base16_4b(base16_4b_str);

	PG_RETURN_BASE164B(result);
}

PG_FUNCTION_INFO_V1(base16_4b_out);
Datum
base16_4b_out(PG_FUNCTION_ARGS)
{
	base16_4b_t	packed_base16_4b;
	char   *base16_4b_string;

	packed_base16_4b = PG_GETARG_BASE164B(0);
	base16_4b_string = base16_4b_to_cstring(packed_base16_4b);

	PG_RETURN_CSTRING(base16_4b_string);
}

/* type to/from text conversion routines */
PG_FUNCTION_INFO_V1(base16_4b_to_text);
Datum
base16_4b_to_text(PG_FUNCTION_ARGS)
{
	char	*base16_4b_string;
	text	*base16_4b_text;

	base16_4b_t	packed_base16_4b =  PG_GETARG_BASE164B(0);

	base16_4b_string = base16_4b_to_cstring(packed_base16_4b);
	base16_4b_text = DatumGetTextP(DirectFunctionCall1(textin, CStringGetDatum(base16_4b_string)));

	PG_RETURN_TEXT_P(base16_4b_text);
}

PG_FUNCTION_INFO_V1(text_to_base16_4b);
Datum
text_to_base16_4b(PG_FUNCTION_ARGS)
{
	text  *base16_4b_text = PG_GETARG_TEXT_P(0);
	char  *base16_4b_str = DatumGetCString(DirectFunctionCall1(textout, PointerGetDatum(base16_4b_text)));
	base16_4b_t base16_4b = cstring_to_base16_4b(base16_4b_str);

	PG_RETURN_BASE164B(base16_4b);
}

/* Functions to convert to/from bytea */
PG_FUNCTION_INFO_V1(base16_4b_send);
Datum
base16_4b_send(PG_FUNCTION_ARGS)
{
	StringInfoData buffer;
	base16_4b_t base16_4b = PG_GETARG_BASE164B(0);

	pq_begintypsend(&buffer);
	pq_sendint(&buffer, (uint32)base16_4b, 4);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buffer));
}

PG_FUNCTION_INFO_V1(base16_4b_recv);
Datum base16_4b_recv(PG_FUNCTION_ARGS)
{
	base16_4b_t	base16_4b;
	StringInfo	buffer = (StringInfo) PG_GETARG_POINTER(0);

	base16_4b = pq_getmsgint(buffer, 4);
	PG_RETURN_BASE164B(base16_4b);
}

/* functions to support btree opclass */
PG_FUNCTION_INFO_V1(base16_4b_lt);
Datum
base16_4b_lt(PG_FUNCTION_ARGS)
{
	base16_4b_t a = PG_GETARG_BASE164B(0);
	base16_4b_t b = PG_GETARG_BASE164B(1);
	PG_RETURN_BOOL(a < b);
}

PG_FUNCTION_INFO_V1(base16_4b_le);
Datum
base16_4b_le(PG_FUNCTION_ARGS)
{
	base16_4b_t a = PG_GETARG_BASE164B(0);
	base16_4b_t b = PG_GETARG_BASE164B(1);
	PG_RETURN_BOOL (a <= b);
}

PG_FUNCTION_INFO_V1(base16_4b_eq);
Datum
base16_4b_eq(PG_FUNCTION_ARGS)
{
	base16_4b_t a = PG_GETARG_BASE164B(0);
	base16_4b_t b = PG_GETARG_BASE164B(1);
	PG_RETURN_BOOL(a == b);
}

PG_FUNCTION_INFO_V1(base16_4b_ne);
Datum
base16_4b_ne(PG_FUNCTION_ARGS)
{
	base16_4b_t a = PG_GETARG_BASE164B(0);
	base16_4b_t b = PG_GETARG_BASE164B(1);
	PG_RETURN_BOOL(a != b);
}

PG_FUNCTION_INFO_V1(base16_4b_ge);
Datum
base16_4b_ge(PG_FUNCTION_ARGS)
{
	base16_4b_t a = PG_GETARG_BASE164B(0);
	base16_4b_t b = PG_GETARG_BASE164B(1);
	PG_RETURN_BOOL(a >= b);
}

PG_FUNCTION_INFO_V1(base16_4b_gt);
Datum
base16_4b_gt(PG_FUNCTION_ARGS)
{
	base16_4b_t a = PG_GETARG_BASE164B(0);
	base16_4b_t b = PG_GETARG_BASE164B(1);
	PG_RETURN_BOOL(a > b);
}

static int32
base16_4b_cmp_internal(base16_4b_t a, base16_4b_t b)
{
    //return a - b;  Does not work with unsigned.

    if (a < b)
        return -1;
    else if (a > b)
        return 1;

    return 0;
}

PG_FUNCTION_INFO_V1(base16_4b_cmp);
Datum
base16_4b_cmp(PG_FUNCTION_ARGS)
{
	base16_4b_t a = PG_GETARG_BASE164B(0);
	base16_4b_t b = PG_GETARG_BASE164B(1);

	PG_RETURN_INT32(base16_4b_cmp_internal(a, b));
}

/*****************************************************************************
 * Aggregate functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(base16_4b_smaller);

Datum
base16_4b_smaller(PG_FUNCTION_ARGS)
{
   base16_4b_t left  = PG_GETARG_BASE164B(0);
   base16_4b_t right = PG_GETARG_BASE164B(1);
   int32 result;

   result = base16_4b_cmp_internal(left, right) < 0 ? left : right;
   PG_RETURN_BASE164B(result);
}

PG_FUNCTION_INFO_V1(base16_4b_larger);

Datum
base16_4b_larger(PG_FUNCTION_ARGS)
{
   base16_4b_t left  = PG_GETARG_BASE164B(0);
   base16_4b_t right = PG_GETARG_BASE164B(1);
   int32 result;

   result = base16_4b_cmp_internal(left, right) > 0 ? left : right;
   PG_RETURN_BASE164B(result);
}


/*
 * Convert a cstring to a base16_4b.
 */
static base16_4b_t
cstring_to_base16_4b(char *base16_4b_str)
{
    char                *ptr;
    unsigned long int   total = 0;
    unsigned int        digit_value;

    ptr = base16_4b_str;

    if (*ptr == 0)
        ereport(ERROR,
            (errmsg("base16_4b number \"%s\" must be 1 digits or larger.", base16_4b_str)));

    for (; ; ptr += 1) {
        if (*ptr == 0)
            PG_RETURN_BASE164B(total);

        digit_value = char_to_num_b16[(unsigned)*ptr];

        if (digit_value == 127)
            ereport(ERROR,
                (errmsg("base16_4b number \"%s\" must only contain digits 0 to 9 and A to F.", base16_4b_str)));

        //total *= 16;
        //total += digit_value;
        total = (total << 4) | digit_value;

        if (total > 4294967295)
            ereport(ERROR,
                (errmsg("base16_4b number \"%s\" must be 8 digits or smaller.", base16_4b_str)));
    }

    PG_RETURN_BASE164B(total);
}

/* Convert the internal representation to output string */
static char *
base16_4b_to_cstring(base16_4b_t base16_4b)
{
    char buffer[9];
    unsigned int offset = sizeof(buffer);
    long unsigned int remainder = base16_4b;
    char   *base16_4b_str;

    buffer[--offset] = '\0';
    do {
        buffer[--offset] = base16_chars[remainder & 0xF];
        remainder = remainder >> 4;
    } while (remainder > 0);

    base16_4b_str = palloc(sizeof(buffer) - offset);
    memcpy(base16_4b_str, &buffer[offset], sizeof(buffer) - offset);
    return base16_4b_str;
}
