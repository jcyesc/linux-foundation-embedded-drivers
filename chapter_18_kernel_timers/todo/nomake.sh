#!/bin/bash
#/* **************** LFD5435:5.4 s_18/nomake.sh **************** */
#/*
# * The code herein is: Copyright the Linux Foundation, 2019
# *
# * This Copyright is retained for the purpose of protecting free
# * redistribution of source.
# *
# *     URL:    https://training.linuxfoundation.org
# *     email:  training@linuxfoundation.org
# *
# * This code is distributed under Version 2 of the GNU General Public
# * License, which you should have received with the source.
# *
# */
#!/bin/bash

PATH=../:$PATH

# kernel we are testing to be greater than or equal to
KVNEW=4.14.0  

# normal method to get kernel version from running kernel
KV=$(uname -r)

# extract from KROOT if cross compiling, sepcificed as environment var
[[ $KROOT != "" ]] &&  \
    KV=$(echo $KROOT | sed -e s:/lib/modules/:: | sed -e s:/build::)
# for testing purposes only, can give as argment
[[ $1 != "" ]] && KV=$1   

KVCHECK="$(echo -e $KV\\n$KVNEW | sort -V | head -n1)"

#[[ $KVCHECK = $KVNEW ]] && cp labs_NEW/* . || cp labs_OLD/* .

if [[ $KVCHECK = $KVNEW ]] ; then
    echo "         DOING NEWER TIMER LABS" && cp labs_NEW/* . 
else
    echo "         DOING OLDER TIMER LABS" &&  cp labs_OLD/* .
fi

# OK TIME TO MAKE!

KROOT=$KROOT ../genmake
make
