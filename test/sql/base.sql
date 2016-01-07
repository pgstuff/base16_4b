\set ECHO none
\i sql/base16_4b.sql
\set ECHO all

CREATE TABLE base16_4bs(id serial primary key, base16_4b base16_4b unique);
INSERT INTO base16_4bs(base16_4b) VALUES('01234567');
INSERT INTO base16_4bs(base16_4b) VALUES('89ABCDEF');
INSERT INTO base16_4bs(base16_4b) VALUES('20000004');
INSERT INTO base16_4bs(base16_4b) VALUES('40000002');
INSERT INTO base16_4bs(base16_4b) VALUES('30000003');
INSERT INTO base16_4bs(base16_4b) VALUES('abcdef');
INSERT INTO base16_4bs(base16_4b) VALUES('0'); -- test limits
INSERT INTO base16_4bs(base16_4b) VALUES('FFFFFFFF');

SELECT * FROM base16_4bs ORDER BY base16_4b;

SELECT MIN(base16_4b) AS min FROM base16_4bs;
SELECT MAX(base16_4b) AS max FROM base16_4bs;

-- index scan
TRUNCATE base16_4bs;
INSERT INTO base16_4bs(base16_4b) SELECT '4444'||id FROM generate_series(5678, 8000) id;

SET enable_seqscan = false;
SELECT id,base16_4b::text FROM base16_4bs WHERE base16_4b = '44446000';
SELECT id,base16_4b FROM base16_4bs WHERE base16_4b >= '44447000' LIMIT 5;
SELECT count(id) FROM base16_4bs;
SELECT count(id) FROM base16_4bs WHERE base16_4b <> ('44446500'::text)::base16_4b;
RESET enable_seqscan;

-- operators and conversions
SELECT '0'::base16_4b < '0'::base16_4b;
SELECT '0'::base16_4b > '0'::base16_4b;
SELECT '0'::base16_4b < '1'::base16_4b;
SELECT '0'::base16_4b > '1'::base16_4b;
SELECT '0'::base16_4b <= '0'::base16_4b;
SELECT '0'::base16_4b >= '0'::base16_4b;
SELECT '0'::base16_4b <= '1'::base16_4b;
SELECT '0'::base16_4b >= '1'::base16_4b;
SELECT '0'::base16_4b <> '0'::base16_4b;
SELECT '0'::base16_4b <> '1'::base16_4b;
SELECT '0'::base16_4b = '0'::base16_4b;
SELECT '0'::base16_4b = '1'::base16_4b;

-- COPY FROM/TO
TRUNCATE base16_4bs;
COPY base16_4bs(base16_4b) FROM STDIN;
00000000
FFFFFFFF
\.
COPY base16_4bs TO STDOUT;

-- clean up --
DROP TABLE base16_4bs;

-- errors
SELECT ''::base16_4b;
SELECT '123456789'::base16_4b;
SELECT 'Z'::base16_4b;
