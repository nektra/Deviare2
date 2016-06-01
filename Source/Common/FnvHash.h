//Fowler/Noll/Vo- hash
//
// The basis of this hash algorithm was taken from an idea sent
// as reviewer comments to the IEEE POSIX P1003.2 committee by:
//
//      Phong Vo (http://www.research.att.com/info/kpv/)
//      Glenn Fowler (http://www.research.att.com/~gsf/)
//
// In a subsequent ballot round:
//
//      Landon Curt Noll (http://www.isthe.com/chongo/)
//
// improved on their algorithm.  Some people tried this hash
// and found that it worked rather well.  In an EMail message
// to Landon, they named it the ``Fowler/Noll/Vo'' or FNV hash.
//
// FNV hashes are designed to be fast while maintaining a low
// collision rate. The FNV speed allows one to quickly hash lots
// of data while maintaining a reasonable collision rate.  See:
//
//      http://www.isthe.com/chongo/tech/comp/fnv/index.html
//
// for more details as well as other forms of the FNV hash.
//
//**
//
// Please do not copyright this code.  This code is in the public domain.
//
// LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
// EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
// USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
//
// By:
// chongo <Landon Curt Noll> /\oo/\
//      http://www.isthe.com/chongo/
//
// Share and Enjoy! :-)


//NOTE: This is a stripped down version. Refer to original
//      webpage for full version

//-----------------------------------------------------------

#pragma once


#include <windows.h>
#include <sys/types.h>

#define FNV_VERSION "5.0.2"

typedef ULONG     Fnv32_t;
typedef ULONGLONG Fnv64_t;

#define FNV1A_32_INIT ((Fnv32_t)0x811C9DC5)
#define FNV1A_64_INIT ((Fnv64_t)0xCBF29CE484222325ULL)

//-----------------------------------------------------------

Fnv32_t fnv_32a_buf(void *buf, size_t len, Fnv32_t hashval);
Fnv64_t fnv_64a_buf(void *buf, size_t len, Fnv64_t hashval);

//-----------------------------------------------------------

