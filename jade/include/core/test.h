#ifdef _JADE_DLL
#define JADE __declspec(dllexport)
#else 
#define JADE __declspec(dllimport)
#endif

namespace Jade {
    JADE void Print();
}