#include <stdio.h>
// #define NDEBUG
// #include <assert.h>
// #include <stdlib.h>
// #include <string.h>
#include "leptjson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
  do{\
    test_count++;\
    if(equality)\
      test_pass++;\
    else{\
      fprintf(stderr,"%s:%d: expect: " format " actual: " format "\n",__FILE__,__LINE__,expect,actual);\
      main_ret = 1;\
    }\
  }while(0)

#define EXPECT_EQ_INT(expect,actual) EXPECT_EQ_BASE((expect) == (actual),expect, actual,"%d")
#define EXPECT_EQ_DOUBLE(expect,actual) EXPECT_EQ_BASE((expect) == (actual),expect,actual,"%.17g")

#define TEST_NUMBER(expect,json)\
  do{\
    lept_value v;\
    EXPECT_EQ_INT(LEPT_PARSE_OK,lept_parse(&v,json));\
    EXPECT_EQ_INT(LEPT_NUMBER,lept_get_type(&v));\
    EXPECT_EQ_DOUBLE(expect,lept_get_number(&v));\
    printf("number:%f \n",lept_get_number(&v));\
  }while(0)

#define TEST_ERROR(error,json)\
  do{\
    lept_value v;\
    lept_init(&v);\
    v.type = LEPT_FALSE;\
    EXPECT_EQ_INT(error, lept_parse(&v,json));\
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));\
    lept_free(&v);\
  }while(0)


static void test_parse();
static void test_parse_null();
static void test_parse_true();
static void test_parse_false();
static void test_parse_number();
static void test_parse_expect_value();
static void test_parse_invalid_value();
static void test_parse_root_not_singular();
static void test_parse_number_too_big();
static void test_parse_missing_quotation_mark();
static void test_parse_invalid_string_escape();


int main(){
  test_parse();
  printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass*100.0/test_count);
  return main_ret;
}

static void test_parse(){
  test_parse_null();
  test_parse_true();
  test_parse_false();
  test_parse_number();
  test_parse_expect_value();
  test_parse_invalid_value();
  test_parse_root_not_singular();
  test_parse_number_too_big();

  test_parse_missing_quotation_mark();
  test_parse_invalid_string_escape();
}

static void test_parse_null(){
  lept_value v;

  v.type = LEPT_FALSE;
  EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v,"null"));
  EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));

  // whitespace Test
  v.type = LEPT_FALSE;
  EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v,"  null"));
  EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));

  v.type = LEPT_FALSE;
  EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v," \t  null"));
  EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));

  v.type = LEPT_FALSE;
  EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v," \r  null"));
  EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));

  v.type = LEPT_FALSE;
  EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v," \n  null"));
  EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));

  // error test
#if 0
  v.type = LEPT_FALSE;
  EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v," \\  null"));
  EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));
#endif
}
static void test_parse_true(){
  lept_value v;

  v.type = LEPT_NULL;
  EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v,"true"));
  EXPECT_EQ_INT(LEPT_TRUE, lept_get_type(&v));
}
static void test_parse_false(){
  lept_value v;
  v.type = LEPT_NULL;
  EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v,"false"));
  EXPECT_EQ_INT(LEPT_FALSE, lept_get_type(&v));
}

static void test_parse_expect_value(){
  lept_value v;

  v.type = LEPT_FALSE;
  EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE,lept_parse(&v,""));
  EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));

  v.type = LEPT_FALSE;
  EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE, lept_parse(&v," "));
  EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
}
static void test_parse_invalid_value(){
  lept_value v;
  v.type = LEPT_FALSE;
  EXPECT_EQ_INT(LEPT_PARSE_INVALID_VALUE,lept_parse(&v,"nul"));
  EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));
}
static void test_parse_root_not_singular(){
  TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR,"null x");
  // lept_value v;
  // v.type = LEPT_FALSE;
  // EXPECT_EQ_INT(LEPT_PARSE_ROOT_NOT_SINGULAR,lept_parse(&v,"null x"));
  // EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));
}
static void test_parse_number_too_big(){
  // not complete
  TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG,"1e309999999");
  TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG,"-1e30999999");
}
static void test_parse_missing_quotation_mark(){
  TEST_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK,"\"");
  TEST_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK,"\"abc");
}
static void test_parse_invalid_string_escape(){
#if 1
  TEST_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
#endif

}

static void test_parse_number(){
  TEST_NUMBER(0.0,"0");
  TEST_NUMBER(0.0,"-0");
  TEST_NUMBER(0.0,"-0.0");
  TEST_NUMBER(1.0,"1");
  TEST_NUMBER(-1.0, "-1");
  TEST_NUMBER(1.5, "1.5");
  TEST_NUMBER(-1.5, "-1.5");
  TEST_NUMBER(3.1416, "3.1416");

  TEST_NUMBER(1E10,"1E10");
  TEST_NUMBER(1e10, "1e10");
  TEST_NUMBER(1E+10, "1E+10");
  TEST_NUMBER(1E-10, "1E-10");
  TEST_NUMBER(-1E10, "-1E10");
  TEST_NUMBER(-1e10, "-1e10");
  TEST_NUMBER(-1E+10, "-1E+10");
  TEST_NUMBER(-1E-10, "-1E-10");
  TEST_NUMBER(1.234E+10, "1.234E+10");
  TEST_NUMBER(1.234E-10, "1.234E-10");
  TEST_NUMBER(0.0, "1e-10000"); /* must underflow */

  TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
  TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
  TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
  TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
  TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
  TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
  TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
  TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
  TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");  
}