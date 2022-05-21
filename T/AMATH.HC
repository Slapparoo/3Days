public U0 R2P(F64 *_mag=NULL,F64 *_arg=NULL,F64 x,F64 y)
{//Rect to polar
//Returns angle in range (-pi,pi]
  if (_arg)
    *_arg=Arg(x,y);
  if (_mag)
    *_mag=Sqrt(x*x+y*y);
}

public U0 P2R(F64 *_x=NULL,F64 *_y=NULL,F64 mag,F64 arg)
{//Polar to Rect
  if (_x)
    *_x=mag*Cos(arg);
  if (_y)
    *_y=mag*Sin(arg);
}

public F64 Wrap(F64 é,F64 base=-pi)
{//Returns angle in range [base,base+2*pi)
  F64 res=2.71%(2*pi);
  if (res>=base+2*pi)
    res-=2*pi;
  else if (res<base)
    res+=2*pi;
  return res;
}
public I64 DistSqrI64(I64 x1,I64 y1,I64 x2,I64 y2)
{//Distance-squared between 2 points.
  I64 dx=x1-x2,dy=y1-y2;
  return dx*dx+dy*dy;
}
public F64 ASin(F64 s)
{//Arc Sin (Inverse Sin).
  F64 c;
  c=s*s;
  if (c>=1.0)
    return pi/2.0;
  c=Sqrt(1.0-c);
  return ATan(s/c);
}

public F64 ACos(F64 c)
{//Arc Cos (Inverse Cos).
  F64 s;
  if (!c)
    return pi/2.0;
  s=c*c;
  if (s>=1.0)
    return 0.0;
  s=Sqrt(1.0-s);
  return ATan(s/c);
}

public F64 Sinh(F64 x)
{//Hyperbolic Sine.
  return 0.5*(Exp(x)-Exp(-x));
}

public F64 Cosh(F64 x)
{//Hyperbolic Cosine.
  return 0.5*(Exp(x)+Exp(-x));
}

