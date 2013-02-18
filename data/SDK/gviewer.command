#!/bin/bash
# SDK Setup Script for Mac OS

echo "Starting script"

export SDK_DIR="`(cd $(dirname \"$0\"); pwd)`"
export PATH="$SDK_DIR/bin:$PATH"

export PS1="[\[\e[1;32m\]Sifteo\[\e[0m\]] \h:\W \u\$ "
export PS2="> "

clear
echo \ \ \_\_\_\ \_\ \ \_\_\ \_\ \ \ \ \ \ \ \ \ \ \ \ \ \_\_\_\ \_\_\_\ \ \_\ \ \_\_
echo \ \/\ \_\_\(\_\)\/\ \_\|\ \|\_\ \_\_\_\ \_\_\_\ \ \/\ \_\_\|\ \ \ \\\|\ \|\/\ \/
echo \ \\\_\_\ \\\ \|\ \ \_\|\ \ \_\/\ \-\_\)\ \_\ \\\ \\\_\_\ \\\ \|\)\ \|\ \'\ \<
echo \ \|\_\_\_\/\_\|\_\|\ \ \\\_\_\\\_\_\_\\\_\_\_\/\ \|\_\_\_\/\_\_\_\/\|\_\|\\\_\\
echo
echo This shell has a PATH and SDK_DIR set up for building
echo Sifteo games using the SDK. This directory contains
echo example projects.
echo

cd $SDK_DIR/
cd ../
cd silt/

if make; then
	if swiss install gviewer.elf; then
		clear
		echo "Successfully installed to cubes!"
	else
		clear
		if siftulator gviewer.elf; then
			clear
			echo "Simulation successful!"
		else
			clear
			echo "Something went wonky.  Try reinstalling your software!"
		fi
	fi
else
	echo "Error compiling.  Maybe crank down the compression."
fi

read -p "Press [Enter] key to continue"

exec bash

