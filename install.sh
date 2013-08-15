#!/bin/bash
shopt -s nocasematch

echo -e "*** Configuring MINT:\n"

FREEIMAGE=$(/sbin/ldconfig -NXv | grep libfreeimage | wc -l)
if [[ $FREEIMAGE -gt 0 ]]; then
    cat<<EOF
It seems that you have FreeImage installed.
MINT can use FreeImage to read image files,
and to capture images from a webcam.
Do you want to enable image support only (i),
image and camera support (c), or neither (n)?
EOF
    read -s -n 1 ANS
    if [[ $ANS == "n" ]]; then
	echo -e "*** Image and camera support disabled.\n"
    elif [[ $ANS == "c" ]]; then
	export image=1
	export camera=1
	echo -e "*** Image and camera support enabled.\n"
    else
	export image=1
	export camera=0
	echo -e "*** Image support enabled, camera support disabled.\n"
    fi
else
    cat<<EOF
FreeImage library not found. Image and camera support disabled."
EOF
fi

cat<<EOF
Do you want to enable multithreading? [y/n]
EOF
read -s -n 1 ANS
if [[ $ANS == "y" ]]; then
    export threads=1
    echo -e "*** Multithreading enabled.\n"
else
    export threads=0
    echo -e "*** Multithreading disabled.\n"
fi


if [[ -e /usr/local/lib/libpigpio.a || -e /usr/lib/libpigpio.a ]]; then
    cat<<EOF
It seems that you have the PIGPIO library installed.
Do you want to enable Raspberry Pi GPIO support? [y/n]
EOF
    read -s -n 1 ANS
    if [[ $ANS == "y" ]]; then
	export pi=1
	echo -e "*** GPIO support enabled.\n"
    else
	export pi=0
	echo -e "*** GPIO support disabled.\n"
    fi
fi


cat<<EOF
Do you want to install a debugging version of MINT? [y/n] 
EOF
read -s -n 1 ANS
if [[ $ANS == "y" ]]; then
    DEBUG=1
    echo -e "*** Debugging version libmint-debug.a enabled.\n"
else
    DEBUG=0
    echo -e "*** Debugging version disabled.\n"
fi


cat<<EOF
Do you want to install a profiling version of MINT? [y/n] 
EOF
read -s -n 1 ANS
if [[ $ANS == "y" ]]; then
    PROFILE=1
    echo -e "*** Profiling version libmint-profile.a enabled.\n"
else
    PROFILE=0
    echo -e "*** Profiling version disabled.\n"
fi

cat<<EOF
Where do you want to install MINT? (Default: /usr/local)
Include files will be under 'include/', library files under 'lib/'.
EOF
while true; do
    read -e -i "/usr/local" INSTALL
    if [ -d $INSTALL ]; then
	echo -e "*** Installing in $INSTALL.\n"
	break
    else
	cat<<EOF
That directory does not exist. 
Do you want to create it (c) or to enter a new one (e)?
EOF
	read -s -n 1 ANS
	if [[ $ANS == "c" ]]; then
	    mkdir -p $INSTALL
	    if [[ ! $? ]]; then
		echo "*** Cannot create directory. Try again."
	    else
		echo "*** Directory created"
		break
	    fi
	else
	    echo "Enter new directory name:"
	fi
    fi
done

echo -e "*** Building libmint.a and MINT documentation ***\n"
./yruba mint 
if [[ $? ]]; then
    echo "*** Build successful ***"
    install=$INSTALL ./yruba install || exit
else
    echo "*** Build failed ***"
    exit
fi

if [[ $DEBUG -eq 1 ]]; then
    echo -e "*** Building debugging version of MINT ***\n"
    debug=1 ./yruba mint
    if [[ $? ]]; then
	echo "*** Build successful ***"
	install=$INSTALL ./yruba install || exit
    else 
	echo "*** Build failed ***"
	exit
    fi
fi

if [[ $PROFILE -eq 1 ]]; then
    echo -e "*** Building profiling version of MINT ***\n"
    profile=1 ./yruba mint
    if [[ $? ]]; then
	echo "*** Build successful ***"
	install=$INSTALL ./yruba install || exit
    else 
	echo "*** Build failed ***"
	exit
    fi
fi
