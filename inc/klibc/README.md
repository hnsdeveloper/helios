# HeliOS klibc

This folder (and its source counterpart) contain code that pertains to HeliOS klibc. HeliOS klibc is a mostly but not entirely compatible (and incomplete) implementation of the C standard library. Functionality is added as components are added to the kernel, thus functionality might be missing and some extra functionality that is not part of the C standard library but that makes sense to be part of klibc might be present. Notably, printf is not part of this library, given that it is inherently error prone and the kernel provides its own set of facilities to print strings. 
