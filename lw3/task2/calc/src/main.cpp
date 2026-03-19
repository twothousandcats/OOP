#include "Calculator.h"
#include <iostream>

int main() {
	calc::Calculator calculator(std::cin, std::cout);
	calculator.run();
	return 0;
}