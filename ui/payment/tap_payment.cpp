#include <iostream>
using namespace std;

int main() {
  cout << "Creating server socket..." << endl;
  int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        cerr << "Can't create a socket!";
        return -1;
    }
  cout << "Hello World! \n";
  return 0;
}