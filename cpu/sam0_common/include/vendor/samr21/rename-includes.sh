#!/bin/bash

# The following is needed to compare component/component_ac.h with component/ac.h
# The latter is the newer names as used by ASF.
# This can be used to prepare a update of ASF and still have GIT history.

cd include || { exit 1; }
pushd component
for f in component_*.h
do
    F=${f##*_}
    git mv $f $F
done
popd

pushd instance
for f in instance_*.h
do
    F=${f##*_}
    git mv $f $F
done
popd

pushd pio
for f in pio_*.h
do
    F=${f##*_}
    git mv $f $F
done
popd
