BEGIN {
	FS = ",";

	getline;
	print $0 FS "avg_" $4 FS "min_duration_ms" FS "max_duration_ms" FS "std_dev_duration_ms"

	FS = ",";
	getline;

	bench_suite		= $1;
	benchmark		= $2;
	vm				= $3;
	avg_duration_ms	= $4;
	min_duration_ms	= $4;
	max_duration_ms	= $4;
	count			= 1;

	printf("%s" FS "-" FS "-" FS "-" FS "-" RS, $0);
}
/^$/ { /* Ignore empty lines */ }
{
	if ($1 == bench_suite && $2 == benchmark && $3 == vm) {
		avg_duration_ms += $4;
		if ($4 < min_duration_ms) min_duration_ms = $4;
		if ($4 > max_duration_ms) max_duration_ms = $4;
		count++;
	} else {
		avg_duration_ms /= count;
		printf("%s" FS "%s" FS "%s" FS "-" FS "%.2f" FS "%.2f" FS "%.2f" FS "-" RS,
			bench_suite, benchmark, vm,
			avg_duration_ms, min_duration_ms, max_duration_ms);

		bench_suite		= $1;
		benchmark		= $2;
		vm				= $3;
		avg_duration_ms	= $4;
		min_duration_ms	= $4;
		max_duration_ms	= $4;
		count			= 1;
	}

	printf("%s" FS "-" FS "-" FS "-" FS "-" RS, $0);
}
END {
	avg_duration_ms /= count;
	printf("%s" FS "%s" FS "%s" FS "-" FS "%.2f" FS "%.2f" FS "%.2f" FS "-" RS,
		bench_suite, benchmark, vm,
		avg_duration_ms, min_duration_ms, max_duration_ms);
}
