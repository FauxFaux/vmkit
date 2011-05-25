#!/bin/bash

if [ -z "$1" ]; then

		echo $0 pattern

else

		grep -n --color --exclude find-it.sh --exclude-dir ".git" --exclude-dir IJvm --exclude-dir Isolate --exclude *.txt --exclude-dir autom4te.cache --exclude configure.bak --exclude config.status --exclude config.log --exclude configure --exclude *.xml --exclude *.html --exclude *.jar --exclude *.bc --exclude MMTkInline.inc --exclude-dir patches --exclude-dir N3 --exclude *.java --exclude *.class -R --exclude-dir .svn --exclude-dir Release --exclude *.s "$1" .

fi