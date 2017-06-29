/** @file
    Testy jednostkowe funkcji `PolyCompose`

    @author Aliaksandr Sarokin <as372525@students.mimuw.edu.pl>
    @copyright Uniwersytet Warszawski
    @date 2017-06-03
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>

#include "cmocka.h"
#include "poly.h"

/** Bufor służący do jump'a */
static jmp_buf jmp_at_exit;

/** Status zakończenia programu */
static int exit_status;

extern Poly PolyCompose(const Poly *p, unsigned count, const Poly x[]);

/** Funkcja main modułu calc_poly.c jest zamieniana na funkcję calculator_main. */
extern int calc_poly_main(void);

/**
 * Atrapa funkcji main
 */
int mock_main(void) {
    if (!setjmp(jmp_at_exit))
        return calc_poly_main();
    return exit_status;
}

/**
 * Atrapa funkcji exit
 */
void mock_exit(int status) {
    exit_status = status;
    longjmp(jmp_at_exit, 1);
}

int mock_fprintf(FILE* const file, const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(2, 3);
int mock_printf(const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(1, 2);

/**
 * Pomocniczy bufor, do którego pisze atrapa funkcji fprintf.
 */
static char fprintf_buffer[256];

/**
 * Pomocniczy bufor, do którego pisze atrapa funkcji printf.
 */
static char printf_buffer[256];

/**
 * Wskazuje pozycje dla bufora wyjścia diagnostycznego.
 */
static int fprintf_position = 0;

/**
 * Wskazuje pozycje dla bufora wyjścia standartowego.
 */
static int printf_position = 0;

/**
 * Atrapa funkcji fprintf sprawdzająca poprawność wypisywania na stderr.
 */
int mock_fprintf(FILE* const file, const char *format, ...) {
    int return_value;
    va_list args;

    assert_true(file == stderr);
    /* Poniższa asercja sprawdza też, czy fprintf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));

    va_start(args, format);
    return_value = vsnprintf(fprintf_buffer + fprintf_position,
                             sizeof(fprintf_buffer) - fprintf_position,
                             format,
                             args);
    va_end(args);

    fprintf_position += return_value;
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));
    return return_value;
}

/**
 * Atrapa funkcji printf sprawdzająca poprawność wypisywania na stderr.
 */
int mock_printf(const char *format, ...) {
    int return_value;
    va_list args;

    /* Poniższa asercja sprawdza też, czy printf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
    assert_true((size_t)printf_position < sizeof(printf_buffer));

    va_start(args, format);
    return_value = vsnprintf(printf_buffer + printf_position,
                             sizeof(printf_buffer) - printf_position,
                             format,
                             args);
    va_end(args);

    printf_position += return_value;
    assert_true((size_t)printf_position < sizeof(printf_buffer));
    return return_value;
}

/**
 *  Pomocniczy bufor, z którego korzystają atrapy funkcji operujących na stdin.
 */
static char input_stream_buffer[256];

/**
 * Wskazuje na pozycje w buforze wejścia standartowego.
 */
static int input_stream_position = 0;

/**
 * Wskazuje na koniec bufora standartowego.
 */
static int input_stream_end = 0;

/**
 * Liczy wczytane znaki.
 */
int read_char_count;

/**
 * Atrapa funkcji scanf używana do przechwycenia czytania z stdin.
 */
int mock_scanf(const char *format, ...) {
    va_list fmt_args;
    int ret;

    va_start(fmt_args, format);
    ret = vsscanf(input_stream_buffer + input_stream_position, format, fmt_args);
    va_end(fmt_args);

    if (ret < 0) { /* ret == EOF */
        input_stream_position = input_stream_end;
    }
    else {
        assert_true(read_char_count >= 0);
        input_stream_position += read_char_count;
        if (input_stream_position > input_stream_end) {
            input_stream_position = input_stream_end;
        }
    }
    return ret;
}

/**
 * Atrapa funkcji getchar używana do przechwycenia czytania z stdin.
 */
int mock_getchar() {
    if (input_stream_position < input_stream_end)
        return input_stream_buffer[input_stream_position++];
    else
        return EOF;
}

/**
 * Atrapa funkcji ungetc.
 * Obsługiwane jest tylko standardowe wejście.
 */
int mock_ungetc(int c, FILE *stream) {
    assert_true(stream == stdin);
    if (input_stream_position > 0)
        return input_stream_buffer[--input_stream_position] = c;
    else
        return EOF;
}

/**
 * Test funkcji `PolyCompose`,
 * gdy `p` wielomian zerowy, `count` równe 0.
 */
static void test_poly_zero_count_zero(void **state)
{
    (void)state;

    Poly tmp1 = PolyZero();
    Poly tmp2 = PolyCompose(&tmp1, 0, NULL);

    assert_true(PolyIsEq(&tmp1, &tmp2));

    PolyDestroy(&tmp1);
    PolyDestroy(&tmp2);
}

/**
 * Test funkcji `PolyCompose`,
 * gdy `p` wielomian zerowy, `count` równe 1, `x[0]` wielomian stały.
 */
static void test_poly_zero_count_one_poly_const(void **state)
{
    (void)state;

    Poly tmp1 = PolyZero();
    Poly tmp2 = PolyFromCoeff(1);
    Poly tmp3 = PolyCompose(&tmp1, 1, &tmp2);

    assert_true(PolyIsEq(&tmp3, &tmp1));

    PolyDestroy(&tmp1);
    PolyDestroy(&tmp2);
    PolyDestroy(&tmp3);
}

/**
 * Test funkcji `PolyCompose`,
 * gdy `p` wielomian @f$x_0@f$, `count` równe 0.
 */
static void test_poly_const_count_zero(void **state)
{
    (void)state;

    Poly tmp1 = PolyFromCoeff(1);
    Poly tmp2 = PolyCompose(&tmp1, 0, NULL);

    assert_true(PolyIsEq(&tmp2, &tmp1));

    PolyDestroy(&tmp1);
    PolyDestroy(&tmp2);
}

/**
 * Test funkcji `PolyCompose`, gdy
 * `p` wielomian stały, `count` równe 1, `x[0]` wielomian stały różny od `p`.
 */
static void test_poly_const_count_one_poly_diff_const(void **state)
{
    (void)state;

    Poly tmp1 = PolyFromCoeff(1);
    Poly tmp2 = PolyFromCoeff(-1);
    Poly tmp3 = PolyCompose(&tmp1, 1, &tmp2);

    assert_true(PolyIsEq(&tmp3, &tmp1));

    PolyDestroy(&tmp1);
    PolyDestroy(&tmp2);
    PolyDestroy(&tmp3);
}

/**
 * Test funkcji `PolyCompose`,
 * gdy `p` wielomian @f$x_0@f$, `count` równe 0.
 */
static void test_poly_count_zero(void **state)
{
    (void)state;

    Poly tmp1 = PolyZero();
    Poly tmp2 = PolyFromCoeff(1);
    Mono m = MonoFromPoly(&tmp2, 1);
    Poly tmp3 = PolyAddMonos(1, &m);
    Poly tmp4 = PolyCompose(&tmp3, 0, NULL);

    assert_true(PolyIsEq(&tmp4, &tmp1));

    PolyDestroy(&tmp1);
    PolyDestroy(&tmp4);
    PolyDestroy(&tmp3);
}

/**
 * Test funkcji `PolyCompose`,
 * gdy `p` wielomian @f$x_0@f$, `count` równe 1, `x[0]` wielomian stały.
 */
static void test_poly_count_one_poly_const(void **state)
{
    (void)state;

    Poly tmp1 = PolyFromCoeff(2);
    Poly tmp2 = PolyFromCoeff(1);
    Mono m = MonoFromPoly(&tmp2, 1);
    Poly tmp3 = PolyAddMonos(1, &m);
    Poly tmp4 = PolyCompose(&tmp3, 1, &tmp1);

    assert_true(PolyIsEq(&tmp4, &tmp1));

    PolyDestroy(&tmp1);
    PolyDestroy(&tmp4);
    PolyDestroy(&tmp3);
}


/**
 * Test funkcji `PolyCompose`,
 * gdy `p` wielomian @f$x_0@f$, `count` równe 1, `x[0]` wielomian @f$x_0@f$.
 */
static void test_poly_count_one_poly(void **state)
{
    (void)state;


    Poly tmp2 = PolyFromCoeff(1);
    Mono m = MonoFromPoly(&tmp2, 1);
    Poly tmp3 = PolyAddMonos(1, &m);
    Poly tmp1 = PolyClone(&tmp3);
    Poly tmp4 = PolyCompose(&tmp3, 1, &tmp1);

    assert_true(PolyIsEq(&tmp4, &tmp1));

    PolyDestroy(&tmp1);
    PolyDestroy(&tmp4);
    PolyDestroy(&tmp3);
}

/**
 * Funkcja wołana przed każdym testem.
 */
static int test_setup(void **state) {
    (void)state;

    memset(fprintf_buffer, 0, sizeof(fprintf_buffer));
    memset(printf_buffer, 0, sizeof(printf_buffer));
    printf_position = 0;
    fprintf_position = 0;

    /* Zwrócenie zera oznacza sukces. */
    return 0;
}

/**
 * Funkcja inicjująca dane wejściowe dla programu korzystającego ze stdin.
 */
static void init_input_stream(const char *str) {
    memset(input_stream_buffer, 0, sizeof(input_stream_buffer));
    input_stream_position = 0;
    input_stream_end = strlen(str);
    assert_true((size_t)input_stream_end < sizeof(input_stream_buffer));
    strcpy(input_stream_buffer, str);
}

/**
 * Test parsowania polecenia `COMPOSE`,
 * gdy `count` to brak parametru.
 */
static void test_parse_empty(void **state) {
    (void)state;

    init_input_stream("COMPOSE\n");

    assert_int_equal(mock_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test parsowania polecenia `COMPOSE`,
 * gdy `count` to minimalna wartość, czyli 0.
 */
static void test_parse_min(void **state) {
    (void)state;

    init_input_stream("COMPOSE 0\n");

    assert_int_equal(mock_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 STACK UNDERFLOW\n");
}

/**
 * Test parsowania polecenia `COMPOSE`,
 * gdy `count` to maksymalna wartość reprezentowana w type `unsigned`.
 */
static void test_parse_max(void **state) {
    (void)state;

    init_input_stream("COMPOSE 4294967295\n");

    assert_int_equal(mock_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 STACK UNDERFLOW\n");
}

/**
 * Test parsowania polecenia `COMPOSE`,
 * gdy `count` to wartość o jeden mniejsza od minimalnej, czyli -1.
 */
static void test_parse_under_min(void **state) {
    (void)state;

    init_input_stream("COMPOSE -1\n");

    assert_int_equal(mock_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test parsowania polecenia `COMPOSE`,
 * gdy `count` to wartość o jeden większa
 * od maksymalnej reprezentowanej w typie `unsigned`.
 */
static void test_parse_over_max(void **state) {
    (void)state;

    init_input_stream("COMPOSE 4294967296\n");

    assert_int_equal(mock_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test parsowania polecenia `COMPOSE`,
 * gdy `count` to duża dodatnia wartość,
 * znacznie przekraczająca zakres typu `unsigned`.
 */
static void test_parse_very_big(void **state) {
    (void)state;

    init_input_stream("COMPOSE 99999999999999999999999999\n");

    assert_int_equal(mock_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test parsowania polecenia `COMPOSE`,
 * gdy `count` to kombinacja liter.
 */
static void test_parse_letters(void **state) {
    (void)state;

    init_input_stream("COMPOSE qwerty\n");

    assert_int_equal(mock_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test parsowania polecenia `COMPOSE`,
 * gdy `count` to kombinacja cyfr i liter, rozpoczynająca się cyfrą.
 */
static void test_parse_digits_letters(void **state) {
    (void)state;

    init_input_stream("COMPOSE 123qwerty\n");

    assert_int_equal(mock_main(), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Uruchamia dwie grupy testów jednostkowych.
 */
int main(void)
{
    const struct CMUnitTest tests_poly_compose[] = {
        cmocka_unit_test(test_poly_zero_count_zero),
        cmocka_unit_test(test_poly_zero_count_one_poly_const),
        cmocka_unit_test(test_poly_const_count_zero),
        cmocka_unit_test(test_poly_const_count_one_poly_diff_const),
        cmocka_unit_test(test_poly_count_zero),
        cmocka_unit_test(test_poly_count_one_poly_const),
        cmocka_unit_test(test_poly_count_one_poly)
    };


    const struct CMUnitTest tests_parse_poly_compose[] = {
        cmocka_unit_test_setup(test_parse_empty, test_setup),
        cmocka_unit_test_setup(test_parse_min, test_setup),
        cmocka_unit_test_setup(test_parse_max, test_setup),
        cmocka_unit_test_setup(test_parse_under_min, test_setup),
        cmocka_unit_test_setup(test_parse_over_max, test_setup),
        cmocka_unit_test_setup(test_parse_very_big, test_setup),
        cmocka_unit_test_setup(test_parse_letters, test_setup),
        cmocka_unit_test_setup(test_parse_digits_letters, test_setup)
    };

    int res = cmocka_run_group_tests(tests_poly_compose, NULL, NULL);
    res |= cmocka_run_group_tests(tests_parse_poly_compose, NULL, NULL);

    return res;
}
