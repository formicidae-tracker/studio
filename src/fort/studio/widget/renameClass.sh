#!/bin/bash


function rename_class() {
	files=$(grep -Rn $1 . | cut -d ":" -f 1)
    sed -i "s/$1/$2/g" $files
}

rename_class $1 $2
