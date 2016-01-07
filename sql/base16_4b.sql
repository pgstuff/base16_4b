/*
 * Author: The maintainer's name
 * Created at: Wed Oct 14 23:12:59 -0400 2015
 *
 */

SET client_min_messages = warning;

-- SQL definitions
CREATE TYPE base16_4b;

-- basic i/o functions
CREATE OR REPLACE FUNCTION base16_4b_in(cstring) RETURNS base16_4b AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base16_4b_out(base16_4b) RETURNS cstring AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base16_4b_send(base16_4b) RETURNS bytea AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base16_4b_recv(internal) RETURNS base16_4b AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE base16_4b (
	input = base16_4b_in,
	output = base16_4b_out,
	send = base16_4b_send,
	receive = base16_4b_recv,
	internallength = 4,
	passedbyvalue
);

-- functions to support btree opclass
CREATE OR REPLACE FUNCTION base16_4b_lt(base16_4b, base16_4b) RETURNS bool AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base16_4b_le(base16_4b, base16_4b) RETURNS bool AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base16_4b_eq(base16_4b, base16_4b) RETURNS bool AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base16_4b_ne(base16_4b, base16_4b) RETURNS bool AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base16_4b_ge(base16_4b, base16_4b) RETURNS bool AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base16_4b_gt(base16_4b, base16_4b) RETURNS bool AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION base16_4b_cmp(base16_4b, base16_4b) RETURNS int4 AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;

-- to/from text conversion
CREATE OR REPLACE FUNCTION base16_4b_to_text(base16_4b) RETURNS text AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION text_to_base16_4b(text) RETURNS base16_4b AS '$libdir/base16_4b'
LANGUAGE C IMMUTABLE STRICT;

-- operators
CREATE OPERATOR < (
	leftarg = base16_4b, rightarg = base16_4b, procedure = base16_4b_lt,
	commutator = >, negator = >=,
	restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR <= (
	leftarg = base16_4b, rightarg = base16_4b, procedure = base16_4b_le,
	commutator = >=, negator = >,
	restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR = (
	leftarg = base16_4b, rightarg = base16_4b, procedure = base16_4b_eq,
	commutator = =, negator = <>,
	restrict = eqsel, join = eqjoinsel,
	merges
);
CREATE OPERATOR <> (
	leftarg = base16_4b, rightarg = base16_4b, procedure = base16_4b_ne,
	commutator = <>, negator = =,
	restrict = neqsel, join = neqjoinsel
);
CREATE OPERATOR > (
	leftarg = base16_4b, rightarg = base16_4b, procedure = base16_4b_gt,
	commutator = <, negator = <=,
	restrict = scalargtsel, join = scalargtjoinsel
);
CREATE OPERATOR >= (
	leftarg = base16_4b, rightarg = base16_4b, procedure = base16_4b_ge,
	commutator = <=, negator = <,
	restrict = scalargtsel, join = scalargtjoinsel
);

-- aggregates
CREATE OR REPLACE FUNCTION base16_4b_smaller(base16_4b, base16_4b)
RETURNS base16_4b
AS '$libdir/base16_4b'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION base16_4b_larger(base16_4b, base16_4b)
RETURNS base16_4b
AS '$libdir/base16_4b'
    LANGUAGE C IMMUTABLE STRICT;

CREATE AGGREGATE min(base16_4b)  (
    SFUNC = base16_4b_smaller,
    STYPE = base16_4b,
    SORTOP = <
);

CREATE AGGREGATE max(base16_4b)  (
    SFUNC = base16_4b_larger,
    STYPE = base16_4b,
    SORTOP = >
);

-- btree operator class
CREATE OPERATOR CLASS base16_4b_ops DEFAULT FOR TYPE base16_4b USING btree AS
	OPERATOR 1 <,
	OPERATOR 2 <=,
	OPERATOR 3 =,
	OPERATOR 4 >=,
	OPERATOR 5 >,
	FUNCTION 1 base16_4b_cmp(base16_4b, base16_4b);

-- cast from/to text
CREATE CAST (base16_4b AS text) WITH FUNCTION base16_4b_to_text(base16_4b) AS ASSIGNMENT;
CREATE CAST (text AS base16_4b) WITH FUNCTION text_to_base16_4b(text) AS ASSIGNMENT;

/* Does this survive a pg_dump?
CREATE CAST (int       AS base16_4b) WITHOUT FUNCTION AS ASSIGNMENT;
CREATE CAST (base16_4b AS int)       WITHOUT FUNCTION;
*/
