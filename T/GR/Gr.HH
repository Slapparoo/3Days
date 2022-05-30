#help_index "Sprites/Binary"

class CMeshTri
{
  I32 color;	//Give one of 0-7 colors.
//Colors 8-15 are 0-7 with intensity bit set.
  I32 nums[3];	//Vertex number
};

class CQueMeshTri
{
  CQueMeshTri *next,*last;
  U0 start;
  I32 color;
  I32 nums[3];
};

public class CSpriteBase
{
  U8 type;
};

public class CSpriteColor : CSpriteBase
{
  U8 color;
};

public class CSpriteDitherColor : CSpriteBase
{
  U16 dither_color;
};

public class CSpriteT : CSpriteBase
{
  I32 thick;
};

public class CSpritePt : CSpriteBase
{
  I32 x1;
  I32 y1;
};

public class CSpritePtRad : CSpritePt
{
  I32 radius;
};

public class CSpritePtPt : CSpritePt
{
  I32 x2;
  I32 y2;
};

public class CSpritePtPtAng : CSpritePtPt
{
  F64 angle;
};

public class CSpritePtWH : CSpritePt
{
  I32 width;
  I32 height;
};

public class CSpritePtWHU8s : CSpritePtWH
{
  U8 u[0];
};

public class CSpritePtWHAng : CSpritePtWH
{
  F64 angle;
};

public class CSpritePtWHAngSides : CSpritePtWHAng
{
  I32 sides;
};

public class CSpriteNumU8s : CSpriteBase
{
  I32 num;
  U8  u[0];
};

public class CSpriteNumPtU8s : CSpriteBase
{
  I32 num;
  I32 x;
  I32 y;
  U8  u[0];
};

public class CSpritePtStr : CSpritePt
{
  U8  st[0];
};

public class CSpriteMeshU8s : CSpriteBase
{
  I32 vertex_cnt;
  I32 tri_cnt;
  U8  u[0];
};

public class CSpritePtMeshU8s : CSpriteBase
{
  I32 x;
  I32 y;
  I32 z;
  I32 vertex_cnt;
  I32 tri_cnt;
  U8  u[0];
};



#help_index "Graphics/Sprite;Sprites;Sprites/Binary"

//See $LK,"Sprites",A="HI:Sprites"$.  See $LK,"::/Adam/Gr/GrSpritePlot.HC",A="FL:::/Adam/Gr/GrSpritePlot.HC,1"$ and
//$LK,"::/Demo/Graphics/SpriteRaw.HC"$ for how CSprite are stored.
public class CSprite
{
  CSprite *next,*last;
  U0 start;
  union {
    U8 type;
    CSpriteBase		b;
    CSpriteColor	c;
    CSpriteDitherColor	d;
    CSpriteT		t;
    CSpritePt		p;
    CSpritePtPt		pp;
    CSpritePtPtAng	ppa;
    CSpritePtRad	pr;
    CSpritePtWHU8s	pwhu;
    CSpritePtWHAng	pwha;
    CSpritePtWHAngSides pwhas;
    CSpriteNumU8s	nu;
    CSpriteNumPtU8s	npu;
    CSpriteMeshU8s	mu;
    CSpritePtMeshU8s	pmu;
    CSpritePtStr	ps;
  }
};

extern CDocEntry *DocSprite(CDoc *doc=NULL,U8 *elems,U8 *fmt=NULL);
extern Bool GrPlot0(CDC *dc,I64 x,I64 y);   
