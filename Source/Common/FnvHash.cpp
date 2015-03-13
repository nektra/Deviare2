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

#include <stdlib.h>
#include "FnvHash.h"

//-----------------------------------------------------------

// fnv_32a_buf - perform a 32 bit Fowler/Noll/Vo FNV-1a hash on a buffer
//
// input:
//   buf - start of buffer to hash
//   len - length of buffer in octets
//   hval - previous hash value or 0 if first call
//
// returns:
//   32 bit hash as a static hash type
//
// NOTE: To use the recommended 32 bit FNV-1a hash, use FNV1A_32_INIT as the
//       hval arg on the first call to either fnv_32a_buf() or fnv_32a_str().
Fnv32_t fnv_32a_buf(void *buf, size_t len, Fnv32_t hval)
{
  unsigned char *bp, *be;

  bp = (unsigned char *)buf;
  be = bp + len;
  // FNV-1a hash each octet of the buffer
  while (bp < be) {
    // xor the bottom with the current octet
    hval ^= (Fnv32_t)*bp++;
    // multiply by the 32 bit FNV magic prime mod 2^32
    hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
  }
  return hval;
}

// fnv_64a_buf - perform a 64 bit Fowler/Noll/Vo FNV-1a hash on a buffer
//
// input:
//   buf - start of buffer to hash
//   len - length of buffer in octets
//   hval - previous hash value or 0 if first call
//
// returns:
//   64 bit hash as a static hash type
//
// NOTE: To use the recommended 64 bit FNV-1a hash, use FNV1A_64_INIT as the
//       hval arg on the first call to either fnv_64a_buf() or fnv_64a_str().
Fnv64_t fnv_64a_buf(void *buf, size_t len, Fnv64_t hval)
{
  unsigned char *bp, *be;

  bp = (unsigned char *)buf;
  be = bp + len;
  // FNV-1a hash each octet of the buffer
  while (bp < be) {
    // xor the bottom with the current octet
    hval ^= (Fnv64_t)*bp++;
    // multiply by the 64 bit FNV magic prime mod 2^64
    hval += (hval << 1) + (hval << 4) + (hval << 5) + (hval << 7) + (hval << 8) + (hval << 40);
  }
  return hval;
}
