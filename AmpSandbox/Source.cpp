#include <amp.h>                // C++ AMP header file
#include <iostream>             // For std::cout etc

using namespace concurrency;    // Save some typing :)
using std::vector;     // Ditto. Comes from <vector> brought in by amp.h


void perform_calculation(vector<int>& vA, vector<int>& vB, vector<int>& vC, int M, int N)
{
  extent<2> e(M, N);
  array_view<int, 2> a(e, vA), b(e, vB);
  array_view<int, 2> c(e, vC);

  index<2> idx(0, 0);
  parallel_for_each(e, [=](index<2> idx) restrict(amp)
  {
    c[idx] = a[idx] + b[idx];
  });

  c.synchronize();
}

void MatMul(vector<int>& vC, const vector<int>& vA, const vector<int>& vB, int M, int N, int W)
{
  for (int row = 0; row < M; row++)
  {
    for (int col = 0; col < N; col++)
    {
      int sum = 0;
      for (int i = 0; i < W; i++)
        sum += vA[row * W + i] * vB[i * N + col];
      vC[row * N + col] = sum;
    }
  }
}

void MatMulAmp(vector<int>& vC, const vector<int>& vA, const vector<int>& vB, int M, int N, int W)
{
  array_view<const int, 2> a(M, W, vA), b(W, N, vB);
  array_view<int, 2> c(M, N, vC);
  c.discard_data();

  std::wcout << "Beginning AMP calc" << std::endl;

  parallel_for_each(c.extent, [=](index<2> idx) restrict(amp)
  {
    int row = idx[0]; int col = idx[1];
    int sum = 0;
    for (int i = 0; i < b.extent[0]; i++)
      sum += a(row, i) * b(i, col);
    c[idx] = sum;
  });
  c.synchronize();

  std::wcout << "Finished AMP calc" << std::endl;
}

void do_it_CPU()
{
  // Rows and columns for matrix
  const int M = 1024;
  const int N = 1024;
  const int W = 1024;

  // Create storage for a matrix of above size
  vector<int> vA(M * N);
  vector<int> vB(M * N);

  // Populate matrix objects
  int i = 0;
  std::generate(vA.begin(), vA.end(), [&i]() {return i++; });
  std::generate(vB.begin(), vB.end(), [&i]() {return i--; });

  // Output storage for matrix calculation
  vector<int> vC(M * N);

  //perform_calculation(vA, vB, vC, M, N);

  auto t1 = std::chrono::high_resolution_clock::now();
  MatMul(vA, vB, vC, M, N, W);
  auto t2 = std::chrono::high_resolution_clock::now();
  std::cout << "MatMul took "
    << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
    << " milliseconds\n"; 
}

void do_it_AMP()
{
  // Rows and columns for matrix
  const int M = 1024;
  const int N = 1024;
  const int W = 1024;

  // Create storage for a matrix of above size
  vector<int> vA(M * N);
  vector<int> vB(M * N);

  // Populate matrix objects
  int i = 0;
  std::generate(vA.begin(), vA.end(), [&i]() {return i++; });
  std::generate(vB.begin(), vB.end(), [&i]() {return i--; });

  // Output storage for matrix calculation
  vector<int> vC(M * N);

  //perform_calculation(vA, vB, vC, M, N);
 
  auto t3 = std::chrono::high_resolution_clock::now();
  MatMulAmp(vA, vB, vC, M, N, W);
  auto t4 = std::chrono::high_resolution_clock::now();
  std::wcout << "AMP took "
    << std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count()
    << " milliseconds\n";
}

bool pick_accelerator()
{
  std::vector<accelerator> accs = accelerator::get_all();
  accelerator chosen_one; // defaults to system choosen one

  auto result =
    std::find_if(accs.begin(), accs.end(), [](const accelerator& acc)
  {
    return acc.supports_cpu_shared_memory;
    //acc.supports_double_precision &&
    //!acc.has_display;
  });

  if (result != accs.end())
    chosen_one = *(result);

  std::wcout << std::endl << "Chosen GPU: " << chosen_one.description << std::endl;

  bool success = accelerator::set_default(chosen_one.device_path);
  return success;
}

void list_all_accelerators()
{
  std::wcout << std::endl << "---------------------------" << std::endl;
  std::wcout << "All accelerators: " << std::endl;
  std::vector<accelerator> accs = accelerator::get_all();
  for (size_t i = 0; i < accs.size(); i++) {
    std::wcout << std::endl;
    std::wcout << accs[i].description << std::endl;
    std::wcout << accs[i].device_path << std::endl;
    std::wcout << accs[i].dedicated_memory << std::endl;
    std::wcout << (accs[i].supports_cpu_shared_memory ?
      "CPU shared memory: true" : "CPU shared memory: false") << std::endl;
    std::wcout << (accs[i].supports_double_precision ?
      "double precision: true" : "double precision: false") << std::endl;
    std::wcout << (accs[i].supports_limited_double_precision ?
      "limited double precision: true" : "limited double precision: false") << std::endl;    
  }
  std::wcout << std::endl << "---------------------------" << std::endl;
}

void printSelectedAccelerator()
{
  accelerator chosen_one;
  std::wcout << std::endl << "---------------------------" << std::endl;
  std::wcout << "Chosen accelerator: " << chosen_one.description << std::endl;
  std::wcout << "Chosen device path: " << chosen_one.device_path << std::endl;
  std::wcout << "---------------------------" << std::endl << std::endl;
}


int main()
{
  list_all_accelerators();
  //pick_accelerator();
  printSelectedAccelerator();

  std::wcout << "Begining calc" << std::endl;
  do_it_CPU();
  do_it_AMP();

  /*default_properties();
  list_all_accelerators();
  pick_accelerator();*/

#ifdef DEBUG
  std::wcout << std::endl << "Hit return to exit..." << std::endl;
  std::wcin.get();
#endif // DEBUG
 
}




void default_properties() {
  accelerator default_acc;
  std::wcout << "device name: " << default_acc.description << std::endl;
  std::wcout << "device path: " << default_acc.device_path << std::endl;
  std::wcout << "Dedicated memory: " << default_acc.dedicated_memory << "Mb" << std::endl;
  std::wcout << (default_acc.supports_cpu_shared_memory ?
    "CPU shared memory: true" : "CPU shared memory: false") << std::endl;
  std::wcout << (default_acc.supports_double_precision ?
    "double precision: true" : "double precision: false") << std::endl;
  std::wcout << (default_acc.supports_limited_double_precision ?
    "limited double precision: true" : "limited double precision: false") << std::endl;
}

