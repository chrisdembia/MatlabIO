
#include "stdafx.h"

#include <iostream>

#include "MatlabInternal.h"

int main(int argc, char** argv)
{

	Matlab::File file("C:\\Users\\NMBL\\Desktop\\[1].mat");

	std::cout << file.description() << std::endl;

	for (Matlab::Matrix* matrix : file.matricies())
			std::cout << matrix->to_string() << std::endl;

	std::cout << "== EOF ==\n";

	std::cin.get();
	return 0;
}

