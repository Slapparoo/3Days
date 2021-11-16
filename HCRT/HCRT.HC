#define NULL (0(U0*))
#define FALSE 0
#define TRUE 1
#define U8 U8i
#define I8 I8i
U16i union U16
{
  I8i	i8[2];
  U8i	u8[2];
};
I16i union I16
{
  I8i	i8[2];
  U8i	u8[2];
};

U32i union U32
{
  I8i	i8[4];
  U8i	u8[4];
  I16	i16[2];
  U16	u16[2];
};

I32i union I32
{
  I8i	i8[4];
  U8i	u8[4];
  I16	i16[2];
  U16	u16[2];
};

U64i union U64
{
  I8i	i8[8];
  U8i	u8[8];
  I16	i16[4];
  U16	u16[4];
  I32	i32[2];
  U32	u32[2];
};

I64i union I64
{
  I8i	i8[8];
  U8i	u8[8];
  I16	i16[4];
  U16	u16[4];
  I32	i32[2];
  U32	u32[2];
};

#include "FS.HC"
#include "MATH.HC"
#include "BITS.HC"
#include "COMPRESS.HC"
#include "CHAR.HC"
#include "FIFO.HC"
#include "FILE.HC"
#include "HASH.HC"
#include "QSORT.HC"
#include "ASM.HC"
#include "PRINT.HC"
