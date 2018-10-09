#!/bin/sh

vals="0 1 2 3 5 13 21 34 55 89 127"

for last in ${vals}
do
    seq "${last}" > "${last}"

    if [ "${last}" -eq 0 ]
    then
        continue
    fi

    seq "-${last}" > "-${last}"
done

for first in ${vals}
do
    for last in ${vals}
    do
        seq "${first}" "${last}" > "${first}_${last}"

        if [ "${first}" -eq 0 ] || [ "${last}" -eq 0 ]
        then
            continue
        fi

        seq "-${first}" "-${last}" > "-${first}_-${last}"
    done
done

for first in ${vals}
do
    for increment in ${vals}
    do
        if [ "${increment}" -eq 0 ]
        then
            continue
        fi

        for last in ${vals}
        do
            seq "${first}" "${increment}" "${last}" > \
                "${first}_${increment}_${last}"

            if [ "${first}" -eq 0 ] || [ "${last}" -eq 0 ]
            then
                continue
            fi

            seq "-${first}" "-${increment}" "-${last}" > \
                "-${first}_-${increment}_-${last}"
        done
    done
done
