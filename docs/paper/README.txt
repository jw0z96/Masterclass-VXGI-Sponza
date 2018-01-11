To build your document, first create a subdirectory and enter it:
> mkdir build
> cd build

Now call CMake on the directory below to start the build process:
> cmake ..

This will automatically generate a Makefile which you can execute to construct your final PDF document:
> make

The final paper should now be available in the build directory - you can view it using your favourite PDF viewer:
> evince paper.pdf

Any questions, let me know.

Richard Southern
rsouthern@bournemouth.ac.uk
