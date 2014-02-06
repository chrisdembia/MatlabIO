
#include "stdafx.h"

#include <iostream>

#include "MatlabIO.h"

int main(int argc, char** argv)
{

	Matlab::File file("C:\\Users\\NMBL\\Desktop\\imag.mat");

	std::cout << "METADATA:\n\n";
	std::cout << file.description() << std::endl << std::endl;

	std::cout << "============\n\nMATRICIES:\n\n";
	for (Matlab::Matrix* matrix : file.matricies())
	{
		std::cout << "Name: " << matrix->name->to_string() << std::endl;
		std::cout << "--Dimensions: " << matrix->dimensions->to_string() << std::endl;

		int j = 0;

		for (auto child : matrix->nodes())
			std::cout << "--Child " << j++ << ": " << child->to_string(true) << std::endl;
	
		std::cout << std::endl;
	}
		
	std::cout << "============\n";

	std::cin.get();
	return 0;
}