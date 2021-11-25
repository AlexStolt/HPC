Compile
* make

------------

Execute
* ./seq_main -i Image_data/<image> -b -o -n <clusters>

------------

A BENCHMARKING Variable is defined in kmeans.h to hide all output except computation and I/O time.
If BENCHMARKING is not defined output would be the same as the original code.
When BENCHMARKING is defined computation time is printed on stdout and I/O on stderr.
To collect data we redirected stdout to a file (.... >> computation_file) and stderr to another file (... 2>> io_file)
The benchmarking script was not included (you can send us an email at stalexandros@uth.gr or nkoutsoukis@uth.gr if you want it sent)
We benchmarked with all binary files and with different numbers of clusters (100, 2000, 6000).
The report and excel only includes benchmarks for Image_data/texture17695 with 2000 number of clusters.

------------

Contact Information
* Stoltidis Alexandros (2824): stalexandros@uth.gr
* Nikolaos Koutsoukis (2907): nkoutsoukis@uth.gr 
