
#include "stdafx.h"

#include <iostream>

#include "MatlabIO.h"

int main(int argc, char** argv)
{

	Matlab::File file("C:\\Users\\NMBL\\Desktop\\[1].mat");

	std::cout << file.description() << std::endl;

	for (Matlab::Matrix* matrix : file.matricies())
	{
		std::cout << "Name: " << matrix->name->to_string() << std::endl;
		std::cout << "--Dims:" << matrix->dimensions->to_string() << std::endl;

		int j = 0;

		for (auto child : matrix->nodes())
			std::cout << "--Child " << j++ << ": " << child->to_string(true) << std::endl;
	
		std::cout << std::endl;
	}
		
	std::cout << "== EOF ==\n";

	std::cin.get();
	return 0;
}

