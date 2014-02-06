
#include "stdafx.h"

#include <iostream>

#include "MatlabInternal.h"

int main(int argc, char** argv)
{

	Matlab::File file("C:\\Users\\NMBL\\Desktop\\[1].mat");

	/*
		Reading the file forms a tree structure of the data:
	
		File
		-->ElementContainer
			-->Matrix
				or
			-->CompressedNode
				-->ElementContainer
					-->Matrix
						--> (Data)
					-->Matrix (?)
						--> (Data)
			-->Matrix
				--> (Data)
	*/

	std::cout << file.description() << std::endl;

	for (Matlab::Element* elem : file.nodes())
	{
		// elem should be a compressed element
		// --> go two layers lower and print out
		// --> as far as i've seen, there is usually
		//     only one element at the bottom of 
		//     the compressed element tree (though
		//     the standard allows for as many)

		Matlab::Matrix* mat = dynamic_cast<Matlab::Matrix*>
			(elem->nodes().front()->nodes().front());

		std::cout << mat->to_string() << std::endl;
	}

	std::cin.get();
	return 0;
}

