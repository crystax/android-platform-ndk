#include <stdio.h>
#include <utility> // for std::forward
// #include <iostream>


static int total = 0;
static void add(int n)
{
	printf("Adding %d\n", n);
	// std::cout << "Adding " << n << std::endl;
	total += n;
}

static void display(const char message[])
{
	printf(": \n");
	// std::cout << message << ": " << total << std::endl;
}

template <class Callable, typename... ArgTypes>
void* Call(Callable native_func, ArgTypes&&... args) noexcept
{
	printf("in Call\n");
	// std::clog << "in Call" << std::endl;
	return native_func(std::forward<ArgTypes>(args)...);
}

static void* test_lambda(int delta)
{
	printf("in test_lambda\n");
	// std::clog << "in test_lambda" << std::endl;
	return Call([=](int delta)
	{
		printf("in lambda\n");
		// std::clog << "in lambda" << std::endl;
		add(delta);
		display("total");
		return nullptr;
	}, delta);
}

int main(int argc, char* argv[])
{
	printf("start\n");
	test_lambda(5);
	printf("after first call\n");
	test_lambda(20);
	printf("after second call\n");
	test_lambda(-256);
	printf("after third call\n");
    return total != -231;
}
