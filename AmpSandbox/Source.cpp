#include <amp.h>                // C++ AMP header file
#include <iostream> 

int main()
{
  std::wcout << L"Creating accelerator with standard constructor" << std::endl;
  
  concurrency::accelerator theAcc;

  std::wcout << L"Done" << std::endl;
}
