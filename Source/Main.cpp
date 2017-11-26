#include "Application/Application.hpp"

#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
	// std::wcout.imbue(std::locale("Korean_Korea.949"));

	try
	{
		Application app;
		app.run();

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << '\n';
		std::cout << "Press Enter to exit... ";
		std::cin.get();

		return EXIT_FAILURE;
	}
}
