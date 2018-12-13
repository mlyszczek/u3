#!/usr/bin/env sh
## ==========================================================================
#   Licensed under BSD 2clause license See LICENSE file for more information
#   Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
## ==========================================================================


. ./mtest.sh

sleep="../src/sleep"
stderr=sleep-test-stderr


## ==========================================================================
#              ____                     __   _
#             / __/__  __ ____   _____ / /_ (_)____   ____   _____
#            / /_ / / / // __ \ / ___// __// // __ \ / __ \ / ___/
#           / __// /_/ // / / // /__ / /_ / // /_/ // / / /(__  )
#          /_/   \__,_//_/ /_/ \___/ \__//_/ \____//_/ /_//____/
#
## ==========================================================================


mt_cleanup_test()
{
    rm -f ${stderr}
}


## ==========================================================================
#                          __               __
#                         / /_ ___   _____ / /_ _____
#                        / __// _ \ / ___// __// ___/
#                       / /_ /  __/(__  )/ /_ (__  )
#                       \__/ \___//____/ \__//____/
#
## ==========================================================================


## ==========================================================================
## ==========================================================================


sleep_sh_print_help()
{
    ${sleep} -h 2>${stderr}
    mt_fail "grep \"usage: sleep <time>\[.<fraction>\]\" ${stderr} >/dev/null 2>&1"
}


## ==========================================================================
## ==========================================================================


sleep_sh_print_version()
{
    ${sleep} -v 2>${stderr}
    mt_fail "grep \"sleep v\" ${stderr} >/dev/null 2>&1"
}


## ==========================================================================
## ==========================================================================


sleep_sh_no_arguments()
{
    ${sleep} 2>${stderr}
    mt_fail "grep \"wrong number of arguments passed\" ${stderr} >/dev/null 2>&1"
    mt_fail "grep \"usage: sleep <time>\[.<fraction>\]\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_too_many_arguments()
{
    ${sleep} 5 10 2>${stderr}
    mt_fail "grep \"wrong number of arguments passed\" ${stderr} >/dev/null 2>&1"
    mt_fail "grep \"usage: sleep <time>\[.<fraction>\]\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_not_a_number()
{
    ${sleep} "five" 2>${stderr}
    mt_fail "grep \"invalid number passed: 'five'\" ${stderr} >/dev/null 2>&1"
    mt_fail "grep \"error parsing seconds part of argument\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_negative_number()
{
    ${sleep} -5 2>${stderr}
    mt_fail "grep \"negative seconds passed: '-5'\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_bad_characters_in_number()
{
    ${sleep} 5O2 2>${stderr}
    mt_fail "grep \"invalid number passed: '5O2'\" ${stderr} >/dev/null 2>&1"
    mt_fail "grep \"error parsing seconds part of argument\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_too_big_number()
{
    big_num="547839265897234789569236454659234538942658758943755843758987234"
    ${sleep} ${big_num}  2>${stderr}
    mt_fail "grep \"number is out of range: '${big_num}'\" ${stderr} >/dev/null 2>&1"
    mt_fail "grep \"error parsing seconds part of argument\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_fract_not_a_number()
{
    ${sleep} "5.five" 2>${stderr}
    mt_fail "grep \"invalid number passed: 'five'\" ${stderr} >/dev/null 2>&1"
    mt_fail "grep \"error parsing fractions of seconds part of argument\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_fract_negative_number()
{
    ${sleep} "1.-5" 2>${stderr}
    mt_fail "grep \"negative fractions of seconds passed: '-5'\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_fract_bad_characters_in_number()
{
    ${sleep} "5.5O2" 2>${stderr}
    mt_fail "grep \"invalid number passed: '5O2'\" ${stderr} >/dev/null 2>&1"
    mt_fail "grep \"error parsing fractions of seconds part of argument\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_fract_too_big_number()
{
    big_num="547839265897234789569236454659234538942658758943755843758987234"
    ${sleep} 5.${big_num}  2>${stderr}
    mt_fail "grep \"number is out of range: '${big_num}'\" ${stderr} >/dev/null 2>&1"
    mt_fail "grep \"error parsing fractions of seconds part of argument\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_fract_too_big_fractions()
{
    ${sleep} 5.1000000000  2>${stderr}
    mt_fail "grep \"fractions cannot be bigger than 999999999\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_seconds()
{
    ${sleep} 5 2>${stderr}
    mt_fail "grep \"sleep for: 5.0\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_seconds_zero()
{
    ${sleep} 0 2>${stderr}
    mt_fail "grep \"sleep for: 0.0\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_fract_zero()
{
    ${sleep} 5.0 2>${stderr}
    mt_fail "grep \"sleep for: 5.0\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_fract_missing()
{
    ${sleep} 1. 2>${stderr}
    mt_fail "grep \"sleep for: 1.0\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_fract_one()
{
    ${sleep} 2.4 2>${stderr}
    mt_fail "grep \"sleep for: 2.400000000\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_fract_five()
{
    ${sleep} 1234.12345 2>${stderr}
    mt_fail "grep \"sleep for: 1234.123450000\" ${stderr} >/dev/null 2>&1"
}
sleep_sh_fract_max()
{
    ${sleep} 1234.999999999 2>${stderr}
    mt_fail "grep \"sleep for: 1234.999999999\" ${stderr} >/dev/null 2>&1"
}

## ==========================================================================
#                __               __
#               / /_ ___   _____ / /_   ___   _  __ ___   _____
#              / __// _ \ / ___// __/  / _ \ | |/_// _ \ / ___/
#             / /_ /  __/(__  )/ /_   /  __/_>  < /  __// /__
#             \__/ \___//____/ \__/   \___//_/|_| \___/ \___/
#
## ==========================================================================

mt_run sleep_sh_print_help
mt_run sleep_sh_print_version
mt_run sleep_sh_no_arguments
mt_run sleep_sh_too_many_arguments
mt_run sleep_sh_not_a_number
mt_run sleep_sh_negative_number
mt_run sleep_sh_bad_characters_in_number
mt_run sleep_sh_too_big_number
mt_run sleep_sh_fract_not_a_number
mt_run sleep_sh_fract_negative_number
mt_run sleep_sh_fract_bad_characters_in_number
mt_run sleep_sh_fract_too_big_number
mt_run sleep_sh_fract_too_big_fractions
mt_run sleep_sh_seconds
mt_run sleep_sh_seconds_zero
mt_run sleep_sh_fract_zero
mt_run sleep_sh_fract_missing
mt_run sleep_sh_fract_one
mt_run sleep_sh_fract_five
mt_run sleep_sh_fract_max
mt_return
