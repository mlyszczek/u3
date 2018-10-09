#!/usr/bin/env sh
## ==========================================================================
#   Licensed under BSD 2clause license See LICENSE file for more information
#   Author: Michał Łyszczek <michal.lyszczek@bofc.pl>
## ==========================================================================

. ./mtest.sh

rev="../src/rev"
rev_test_file="rev-test-file"
rev_test_data="rev-test-data"
rev_expected_data="rev-expected-data"

rev_line_max=$(cat ../config.h | grep U3_REV_LINE_MAX | cut -f3 -d' ')
enable_malloc=$(cat ../config.h | grep ENABLE_MALLOC | cut -f3 -d' ')


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
    rm -f stderr
    rm -f "${rev_test_data}"
    rm -f "${rev_test_file}"
    rm -f "${rev_expected_data}"
}


## ==========================================================================
#   checks if string $2 is inside of string $1
## ==========================================================================


strcmp()
{
    string="$1"
    substring="$2"

    if expr "${string}" : "${substring}" > /dev/null
    then
        # $substring is in $string
        return 0
    else
        return 1
    fi
}


gen_data()
{
    nl="${1}"
    shift
    data="1234567890"
    rdata="0987654321"

    while [ ${#} -gt 0 ]
    do
        n="${1}"
        shift

        buf=
        bufe=

        for i in $(seq 1 1 $(( n / 10 )) )
        do
            # first do full 10-byte rounds first

            buf="${buf}${data}"
            bufe="${bufe}${rdata}"
        done

        for i in $(seq 1 1 $(( n % 10 )) )
        do
            # not byte-by-byte what is left

            val=$( seq $((i % 10)) 1 $((i % 10)) )
            buf="${buf}${val}"
            bufe="${val}${bufe}"
        done

        if [ ${#} -eq 0 ]
        then
            # this is last line to generate

            if [ ${nl} -eq 1 ]
            then
                # and caller wants new line at the end

                echo "${buf}" >> "${rev_test_data}"
                echo "${bufe}" >> "${rev_expected_data}"
            else
                # no new line added at the end

                printf "${buf}" >> "${rev_test_data}"
                printf "${bufe}" >> "${rev_expected_data}"
            fi
        else
            # there are more lines ahead to generate, add newline
            # unconditionally

            echo "${buf}" >> "${rev_test_data}"
            echo "${bufe}" >> "${rev_expected_data}"
        fi
    done
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


rev_sh_print_help()
{
    ${rev} -h 2>stderr
    mt_fail "strcmp \"$(cat stderr)\" \"usage: rev [ -v | -h | <file> ]\""
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_single_empty_line()
{
    out="$(echo "" | ${rev})"
    expected="$(echo "")"
    mt_fail "[ \"${out}\" = \"${expected}\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_single_line()
{
    out="$(echo "123456" | ${rev})"
    expected="$(echo "654321")"
    mt_fail "[ \"${out}\" = \"${expected}\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_single_full_line()
{
    gen_data 1 ${rev_line_max}
    out="$(cat "${rev_test_data}" | ${rev})"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_single_overflow_line()
{
    gen_data 1 $(( 2 * ${rev_line_max} ))
    out="$(cat "${rev_test_data}" | ${rev} 2>stderr)"
    empty=

    if [ "${enable_malloc}" = "1" ]
    then
        mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
        mt_fail "[ \"$(cat stderr)\" = \"${empty}\" ]"
    else
        mt_fail "strcmp \"$(cat stderr)\" \"e/line is longer than\""
        mt_fail "[ \"${out}\" = \"${empty}\" ]"
    fi
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_single_empty_line_no_nl()
{
    out="$(printf "" | ${rev})"
    expected=
    mt_fail "[ \"${out}\" = \"${expected}\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_single_line_no_nl()
{
    out="$(printf "123456" | ${rev})"
    expected="$(printf "654321")"
    mt_fail "[ \"${out}\" = \"${expected}\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_single_full_line_no_nl()
{
    gen_data 0 $(( ${rev_line_max} ))
    out="$(cat "${rev_test_data}" | ${rev})"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_single_overflow_line_no_nl()
{
    gen_data 0 $(( 2 * ${rev_line_max} ))
    out="$(cat "${rev_test_data}" | ${rev} 2>stderr)"
    empty=

    if [ "${enable_malloc}" = "1" ]
    then
        mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
        mt_fail "[ \"$(cat stderr)\" = \"${empty}\" ]"
    else
        mt_fail "strcmp \"$(cat stderr)\" \"e/line is longer than\""
        mt_fail "[ \"${out}\" = \"${empty}\" ]"
    fi
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_multi_empty_line()
{
    gen_data 1  4 5 0 0 1 4 0 0 0 2 0 0
    out="$(cat "${rev_test_data}" | ${rev})"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}

## ==========================================================================
## ==========================================================================


rev_sh_pipe_multi_line()
{
    gen_data 1  4 5 4 5 2 3 20 32
    out="$(cat "${rev_test_data}" | ${rev})"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_multi_full_line()
{
    gen_data 1  ${rev_line_max} ${rev_line_max} 31 ${rev_line_max}
    out="$(cat "${rev_test_data}" | ${rev})"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_multi_overflow_line()
{
    gen_data 1 $(( 2 * ${rev_line_max} )) $(( 3 * ${rev_line_max} )) 42 \
        $(( 2 * ${rev_line_max} ))
    out="$(cat "${rev_test_data}" | ${rev} 2>stderr)"
    empty=

    if [ "${enable_malloc}" = "1" ]
    then
        mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
        mt_fail "[ \"$(cat stderr)\" = \"${empty}\" ]"
    else
        mt_fail "strcmp \"$(cat stderr)\" \"e/line is longer than\""
        mt_fail "[ \"${out}\" = \"${empty}\" ]"
    fi
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_multi_empty_line_no_nl()
{
    gen_data 0  4 5 0 0 1 4 0 0 0 2 0 0 2
    out="$(cat "${rev_test_data}" | ${rev})"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_multi_line_no_nl()
{
    gen_data 0  4 5 4 5 2 3 20 32
    out="$(cat "${rev_test_data}" | ${rev})"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_multi_full_line_no_nl()
{
    gen_data 0  ${rev_line_max} ${rev_line_max} 31 ${rev_line_max}
    out="$(cat "${rev_test_data}" | ${rev})"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_pipe_multi_overflow_line_no_nl()
{
    gen_data 1 $(( 2 * ${rev_line_max} )) $(( 3 * ${rev_line_max} )) 42 \
        $(( 2 * ${rev_line_max} ))
    out="$(cat "${rev_test_data}" | ${rev} 2>stderr)"
    empty=

    if [ "${enable_malloc}" = "1" ]
    then
        mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
        mt_fail "[ \"$(cat stderr)\" = \"${empty}\" ]"
    else
        mt_fail "strcmp \"$(cat stderr)\" \"e/line is longer than\""
        mt_fail "[ \"${out}\" = \"${empty}\" ]"
    fi
}


## ==========================================================================
## ==========================================================================


rev_sh_file_single_empty_line()
{
    echo "" > "${rev_test_file}"
    out="$(${rev} "${rev_test_file}")"
    expected="$(echo "")"
    mt_fail "[ \"${out}\" = \"${expected}\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_file_single_line()
{
    echo "123456" > "${rev_test_file}"
    out="$(${rev} "${rev_test_file}")"
    expected="$(echo "654321")"
    mt_fail "[ \"${out}\" = \"${expected}\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_file_single_full_line()
{
    gen_data 1 ${rev_line_max}
    out="$(${rev} "${rev_test_data}")"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_file_single_overflow_line()
{
    gen_data 1 $(( 2 * ${rev_line_max} ))
    out="$(${rev} "${rev_test_data}" 2>stderr)"
    empty=

    if [ "${enable_malloc}" = "1" ]
    then
        mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
        mt_fail "[ \"$(cat stderr)\" = \"${empty}\" ]"
    else
        mt_fail "strcmp \"$(cat stderr)\" \"e/line is longer than\""
        mt_fail "[ \"${out}\" = \"${empty}\" ]"
    fi
}


## ==========================================================================
## ==========================================================================


rev_sh_file_single_empty_line_no_nl()
{
    touch "${rev_test_file}"
    out="$(${rev} "${rev_test_file}")"
    expected=
    mt_fail "[ \"${out}\" = \"${expected}\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_file_single_line_no_nl()
{
    printf "123456" > "${rev_test_file}"
    out="$(${rev} "${rev_test_file}")"
    expected="$(printf "654321")"
    mt_fail "[ \"${out}\" = \"${expected}\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_file_single_full_line_no_nl()
{
    gen_data 0 $(( ${rev_line_max} ))
    out="$(${rev} "${rev_test_data}")"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_file_single_overflow_line_no_nl()
{
    gen_data 0 $(( 2 * ${rev_line_max} ))
    out="$(${rev} "${rev_test_data}" 2>stderr)"
    empty=

    if [ "${enable_malloc}" = "1" ]
    then
        mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
        mt_fail "[ \"$(cat stderr)\" = \"${empty}\" ]"
    else
        mt_fail "strcmp \"$(cat stderr)\" \"e/line is longer than\""
        mt_fail "[ \"${out}\" = \"${empty}\" ]"
    fi
}


## ==========================================================================
## ==========================================================================


rev_sh_file_multi_empty_line()
{
    gen_data 1  4 5 0 0 1 4 0 0 0 2 0 0
    out="$(${rev} "${rev_test_data}")"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}

## ==========================================================================
## ==========================================================================


rev_sh_file_multi_line()
{
    gen_data 1  4 5 4 5 2 3 20 32
    out="$(${rev} "${rev_test_data}")"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_file_multi_full_line()
{
    gen_data 1  ${rev_line_max} ${rev_line_max} 31 ${rev_line_max}
    out="$(${rev} "${rev_test_data}")"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_file_multi_overflow_line()
{
    gen_data 1 $(( 2 * ${rev_line_max} )) $(( 3 * ${rev_line_max} )) 42 \
        $(( 2 * ${rev_line_max} ))
    out="$(${rev} "${rev_test_data}" 2>stderr)"
    empty=

    if [ "${enable_malloc}" = "1" ]
    then
        mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
        mt_fail "[ \"$(cat stderr)\" = \"${empty}\" ]"
    else
        mt_fail "strcmp \"$(cat stderr)\" \"e/line is longer than\""
        mt_fail "[ \"${out}\" = \"${empty}\" ]"
    fi
}


## ==========================================================================
## ==========================================================================


rev_sh_file_multi_empty_line_no_nl()
{
    gen_data 0  4 5 0 0 1 4 0 0 0 2 0 0 2
    out="$(${rev} "${rev_test_data}")"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_file_multi_line_no_nl()
{
    gen_data 0  4 5 4 5 2 3 20 32
    out="$(${rev} "${rev_test_data}")"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_file_multi_full_line_no_nl()
{
    gen_data 0  ${rev_line_max} ${rev_line_max} 31 ${rev_line_max}
    out="$(${rev} "${rev_test_data}")"
    mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
}


## ==========================================================================
## ==========================================================================


rev_sh_file_multi_overflow_line_no_nl()
{
    gen_data 1 $(( 2 * ${rev_line_max} )) $(( 3 * ${rev_line_max} )) 42 \
        $(( 2 * ${rev_line_max} ))
    out="$(${rev} "${rev_test_data}" 2>stderr)"
    empty=

    if [ "${enable_malloc}" = "1" ]
    then
        mt_fail "[ \"${out}\" = \"$(cat ${rev_expected_data})\" ]"
        mt_fail "[ \"$(cat stderr)\" = \"${empty}\" ]"
    else
        mt_fail "strcmp \"$(cat stderr)\" \"e/line is longer than\""
        mt_fail "[ \"${out}\" = \"${empty}\" ]"
    fi
}


## ==========================================================================
## ==========================================================================


rev_sh_three_args()
{
    ${rev} file1 file2 2>stderr
    mt_fail "strcmp \"$(cat stderr)\" \"usage: rev [ -v | -h | <file> ]\""
}


## ==========================================================================
## ==========================================================================


rev_sh_invalid_arg()
{
    ${rev} -a 2>stderr
    mt_fail "strcmp \"$(cat stderr)\" \"e/invalid option -a\""
}


## ==========================================================================
## ==========================================================================


rev_sh_file_not_found()
{
    ${rev} "/i/dont/exist" 2>stderr
    mt_fail "strcmp \"$(cat stderr)\" \"e/fopen(): \""
}


## ==========================================================================
## ==========================================================================


rev_sh_permision_denied()
{
    echo "test" > "${rev_test_file}"
    chmod 200 "${rev_test_file}"
    ${rev} "${rev_test_file}" 2>stderr
    mt_fail "strcmp \"$(cat stderr)\" \"e/fopen(): \""
}


## ==========================================================================
#                __               __
#               / /_ ___   _____ / /_   ___   _  __ ___   _____
#              / __// _ \ / ___// __/  / _ \ | |/_// _ \ / ___/
#             / /_ /  __/(__  )/ /_   /  __/_>  < /  __// /__
#             \__/ \___//____/ \__/   \___//_/|_| \___/ \___/
#
## ==========================================================================


mt_run rev_sh_print_help
mt_run rev_sh_pipe_single_empty_line
mt_run rev_sh_pipe_single_line
mt_run rev_sh_pipe_single_full_line
mt_run rev_sh_pipe_single_overflow_line
mt_run rev_sh_pipe_single_empty_line_no_nl
mt_run rev_sh_pipe_single_line_no_nl
mt_run rev_sh_pipe_single_full_line_no_nl
mt_run rev_sh_pipe_single_overflow_line_no_nl
mt_run rev_sh_pipe_multi_empty_line
mt_run rev_sh_pipe_multi_line
mt_run rev_sh_pipe_multi_full_line
mt_run rev_sh_pipe_multi_overflow_line
mt_run rev_sh_pipe_multi_empty_line_no_nl
mt_run rev_sh_pipe_multi_line_no_nl
mt_run rev_sh_pipe_multi_full_line_no_nl
mt_run rev_sh_pipe_multi_overflow_line_no_nl
mt_run rev_sh_file_single_empty_line
mt_run rev_sh_file_single_line
mt_run rev_sh_file_single_full_line
mt_run rev_sh_file_single_overflow_line
mt_run rev_sh_file_single_empty_line_no_nl
mt_run rev_sh_file_single_line_no_nl
mt_run rev_sh_file_single_full_line_no_nl
mt_run rev_sh_file_single_overflow_line_no_nl
mt_run rev_sh_file_multi_empty_line
mt_run rev_sh_file_multi_line
mt_run rev_sh_file_multi_full_line
mt_run rev_sh_file_multi_overflow_line
mt_run rev_sh_file_multi_empty_line_no_nl
mt_run rev_sh_file_multi_line_no_nl
mt_run rev_sh_file_multi_full_line_no_nl
mt_run rev_sh_file_multi_overflow_line_no_nl
mt_run rev_sh_three_args
mt_run rev_sh_invalid_arg
mt_run rev_sh_file_not_found
mt_run rev_sh_permision_denied

mt_return
