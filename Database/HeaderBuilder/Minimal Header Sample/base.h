typedef signed char SCHAR;
typedef char CHAR;
typedef unsigned char UCHAR;
typedef short int SHORT;
typedef short unsigned int USHORT;
typedef int SDWORD;
typedef int LONG;
typedef unsigned int DWORD;
typedef unsigned int ULONG;
typedef long long int SQWORD;
typedef long long unsigned int QWORD;
typedef long long int LONGLONG;
typedef long long unsigned int ULONGLONG;
typedef float FLOAT;
typedef double DOUBLE;
typedef long double LONG_DOUBLE;
typedef char *LPSTR;
typedef wchar_t *LPWSTR;
typedef void *LPVOID;

#if defined(_M_IX86) 

typedef ULONG SIZE_T;
typedef ULONG size_t;

#elif defined(_M_X64)

typedef ULONGLONG SIZE_T;
typedef ULONGLONG size_t;

#endif

int pepe(int a, int b);
