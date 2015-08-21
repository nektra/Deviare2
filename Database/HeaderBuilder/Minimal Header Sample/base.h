//------------------------------------------------------------------------------
//Basic definitions

typedef signed char SCHAR;
typedef char CHAR;
typedef unsigned char UCHAR;

typedef short SHORT;
typedef unsigned short USHORT;
typedef unsigned short WORD;

typedef int INT;
typedef int SDWORD;
typedef int LONG;
typedef unsigned int UINT;
typedef unsigned int DWORD;
typedef unsigned int ULONG;

typedef long long int LONGLONG;
typedef long long int SQWORD;
typedef long long unsigned int ULONGLONG;
typedef long long unsigned int QWORD;

typedef float FLOAT;
typedef double DOUBLE;
typedef long double LONG_DOUBLE;

typedef char *LPSTR;
typedef const char *LPCSTR;
typedef wchar_t *LPWSTR;
typedef const wchar_t *LPCWSTR;

typedef void *LPVOID;
typedef unsigned char *LPBYTE;

typedef long BOOL;

#if defined(_M_IX86) 

typedef LONG SSIZE_T;
typedef LONG ssize_t;
typedef LONG INT_PTR;
typedef LONG LONG_PTR;

typedef ULONG SIZE_T;
typedef ULONG size_t;
typedef ULONG UINT_PTR;
typedef ULONG ULONG_PTR;

#elif defined(_M_X64)

typedef LONGLONG SSIZE_T;
typedef LONGLONG ssize_t;
typedef LONGLONG INT_PTR;
typedef LONGLONG LONG_PTR;

typedef ULONGLONG SIZE_T;
typedef ULONGLONG size_t;
typedef ULONGLONG UINT_PTR;
typedef ULONGLONG ULONG_PTR;

#endif

//------------------------------------------------------------------------------
//Some Microsoft Windows definitions

#define __in
#define __in_opt
#define __out
#define __out_opt
#define __inout
#define __inout_opt

#define WINAPI __stdcall
#define CONST const

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

typedef HANDLE HWND;
typedef HANDLE HMENU;
typedef HANDLE HINSTANCE;

typedef LPVOID HANDLE;
typedef HANDLE *PHANDLE;


//------------------------------------------------------------------------------
//A Sample API
int sample_api(int a, int b);
