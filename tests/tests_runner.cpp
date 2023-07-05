
#define SXS_RUN_TESTS

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING
#include "doctest.h"

#include <soraxas_toolbox/clock.h>
#include <soraxas_toolbox/compile_time_dict.h>
#include <soraxas_toolbox/format.h>
#include <soraxas_toolbox/globals.h>
#include <soraxas_toolbox/metaprogramming.h>
#include <soraxas_toolbox/print_utils.h>
#include <soraxas_toolbox/stats/timer.h>
#include <soraxas_toolbox/stats/token.h>
#include <soraxas_toolbox/vector_math.h>

#ifdef HAS_EIGEN_
#include <soraxas_toolbox/eigen_helpers.h>
#include <soraxas_toolbox/eigen_math.h>
#endif