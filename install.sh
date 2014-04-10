#!/bin/bash -e
shopt -s nocasematch

# check for necessary SDL libraries; use images only if all are found
SDL[0]=$(/sbin/ldconfig -NXv | grep 'libSDL\.so' | wc -l)
SDL[1]=$(/sbin/ldconfig -NXv | grep 'libSDL_image.so' | wc -l)
SDL[2]=$(/sbin/ldconfig -NXv | grep 'libSDL_ttf.so' | wc -l)
SDL[3]=$(/sbin/ldconfig -NXv | grep 'libSDL_gfx.so' | wc -l)
SDLNAMES=("SDL" "SDL_image" "SDL_ttf" "SDL_gfx")
NOYES=("NO" "YES")
for i in $(seq 0 3); do
    if [ ${SDL[$i]} -le 0 ]; then
	echo -e "* Missing library:" ${SDLNAMES[$i]}
	echo "* Disabling support for images and graphical interface"
	export image=0
	break
    else
	export image=1
    fi
done
if [ $image -eq 1 ]; then
    echo "* Image and graphics support ENABLED"
fi

CAMSHOT=$(which camshot | wc -l)
if [ $CAMSHOT -le 0 ]; then
    echo "* Camshot program not found"
    echo "* Camera support DISABLED"
    camera=0
else
    echo "* Camera support ENABLED"
    camera=1
fi

if [[ -e /usr/local/lib/libpigpio.a || -e /usr/lib/libpigpio.a ]]; then
    export pi=1
    echo "* Raspberry Pi GPIO support ENABLED"
else
    export pi=0
    echo "* PiGPIO library not found"
    echo "* Raspberry Pi support DISABLED"
fi

cat<<EOF
Q: Do you want to enable multithreading? [y/n]
EOF
read -s -n 1 ANS
if [[ $ANS == "y" ]]; then
    export threads=1
    echo -e "*  Multithreading enabled.\n"
else
    export threads=0
    echo -e "*  Multithreading disabled.\n"
fi

cat<<EOF
Q: Do you want to install a debugging version of MINT? [y/n] 
EOF
read -s -n 1 ANS
if [[ $ANS == "y" ]]; then
    DEBUG=1
    echo -e "*  Debugging version libmint-debug.a enabled.\n"
else
    DEBUG=0
    echo -e "*  Debugging version disabled.\n"
fi

cat<<EOF
Q: Do you want to install a profiling version of MINT? [y/n] 
EOF
read -s -n 1 ANS
if [[ $ANS == "y" ]]; then
    PROFILE=1
    echo -e "*  Profiling version libmint-profile.a enabled.\n"
else
    PROFILE=0
    echo -e "*  Profiling version disabled.\n"
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
		echo "* Cannot create directory. Try again."
	    else
		echo "* Directory created"
		break
	    fi
	else
	    echo "Enter new directory name:"
	fi
    fi
done

echo -e "* Building libmint.a and MINT documentation ***\n"
./yruba mint 
if [[ $? ]]; then
    echo "* Build successful ***"
    install=$INSTALL ./yruba install || exit
else
    echo "* Build failed ***"
    exit
fi

if [[ $DEBUG -eq 1 ]]; then
    echo -e "* Building debugging version of MINT ***\n"
    debug=1 ./yruba mint
    if [[ $? ]]; then
	echo "* Build successful ***"
	install=$INSTALL ./yruba install || exit
    else 
	echo "* Build failed ***"
	exit
    fi
fi

if [[ $PROFILE -eq 1 ]]; then
    echo -e "* Building profiling version of MINT ***\n"
    profile=1 ./yruba mint
    if [[ $? ]]; then
	echo "* Build successful ***"
	install=$INSTALL ./yruba install || exit
    else 
	echo "* Build failed ***"
	exit
    fi
fi
