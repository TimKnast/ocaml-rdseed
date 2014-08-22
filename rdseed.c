#include <stdint.h>

// TODO http://wiki.osdev.org/CPUID
// more rigorous Intel test needed?

#define CAML_NAME_SPACE 1
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h> 

CAMLprim value rdseed32_native(value unit)
{
  uint32 rdseed_out=0;
  uint32 result=0;
  char retry_ctr=0;
  CAMLparam1 (unit);

//cpuid eax=07h ecx=0h
// 07h a.k.a. "IntelFeatures"
  __asm__ __volatile__ (
    "movl $0x7, %%eax;"
    "xorl %%ecx, %%ecx;"
    "cpuid;"
    : "=b" (result)
    : : "eax", "ecx"
  );
  // if supported: ebx.rdseed[bit 18]=1
  if( 0 == (result & (1<<17)) )
    caml_failwith("RDSEED instruction not supported by CPU");
  result = 0;

  // https://software.intel.com/en-us/blogs/2012/11/17/the-difference-between-rdrand-and-rdseed
  // clears cf flag if no entropy (temporary error)
  // intel manual states that retries should be limited to prevent busyloops, recommends 10 tries
  while(result==0 && retry_ctr++ < 10)
  __asm__ __volatile__ (
    "rdseed %%eax;"
    // write 32 bits of entropy to eax (rdseed_out)
    "setc %%bl;"
    // save carry flag to bl (result)
    : "=a" (rdseed_out) // set rdseed_out to value of eax
    , "=b" (result)
    : : "cc" // we touch carry flag
  );

  // cf=1 on valid entropy returned
  if(result != 1)
    caml_failwith("No entropy available to RDSEED instruction");
    // raise Failure "..."

  CAMLreturn (caml_copy_int32(rdseed_out));
}

CAMLprim value rdseed32_bytecode(value * argv, int argc){
  // TODO assume 1 arg passed?
  return rdseed32_native(argv[0]);
}
