#!/bin/bash

# Augment statistical data with min, max and average
data=$(awk -f synth-dacapo-data-min-max-avg.awk)

# Integrate augmented information into given input
header=$(echo "$data" | head -1)
data=$(echo "$data" | tail -n $(( $(echo "$data" | wc -l) - 1)) )

added_data=$(echo "$data" | grep -v ',-,-,-,-')

echo "$header"

for i in $added_data ; do
	key=$(echo "$i" | cut -d ',' -f 1-3)
	added_value=$(echo "$i" | cut -d ',' -f 5-)
	echo "$data" | grep "^$key,[^-]" | sed "s/-,-,-,-/$added_value/g"
done

# Augment statistical data with standard deviation
