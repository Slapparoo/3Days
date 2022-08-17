class CGreekWord {
  U8 *str;
  #define GWF_NOUN (1<<0)
  #define GWF_MASCULINE (1<<1)
  #define GWF_FEMININE (1<<2)
  #define GWF_NEUTER (1<<3)
  #define GWF_ADJECTIVE (1<<4)
  #define GWF_VERB (1<<5)
  #define GWF_NOMITIVE (1<<6)
  #define GWF_ACCUSATIVE (1<<7)
  #define GWF_GENITIVE (1<<8)
  #define GWF_DATIVE (1<<9)
  #define GWF_ARTICLE (1<<10) // a/the
  #define GWF_PARTICIPLE (1<<11) //the one -ing 
  #define GWF_TENSE_PRESENT (1<<12)
  #define GWF_IMPERFECT (1<<13)
  #define GWF_TENSE_PAST (1<<14)
  #define GWF_SUBJUNCTIVE (1<<15) //You may
  #define GWF_PASSIVE (1<<16) //I am being X'ed
  #define GWF_1PERSON (1<<17)
  #define GWF_2PERSON (1<<18)
  #define GWF_3PERSON (1<<19)
  #define GWF_SINGULAR (1<<20)
  #define GWF_PLURAL (1<<21)
  #define GWF_ACTIVE (1<<22) //I was Xing
  #define GWF_PARTICIPLE (1<<23) //Xing
  #define GWF_PRONOUN (1<<24) //Him
  #define GWF_PREPOSITION (1<<25)
  #define GWF_3RD_DECLEN (1<<26)
  #define GWF_IMPERATIVE (1<<27)
  #define GWF_INFINITIVE (1<<28)
  #define GWF_TENSE_FUTURE (1<<29)
  #define GWF_CONJUNCTION (1<<30)
  #define GWF_ADVERB (1<<31)
  #define GWF_POST_POSITIVE (1<<32) //https://ancientgreek.pressbooks.com/chapter/10/
  #define GWF_CONJ_BEFORE (1<<33) //Conjunction that appears before a clause
  #define GWF_INTEROGITIVE (1<<34)
  #define GWF_MIDDLE (1<<35) //I moved (myself). Dude is doer and who is being done to
  #define GWF_REFLEXIVE (1<<36) //yourself/myself
  #define GWF_RECIPRICAL (1<<37)
  I64 flags;
};
#define S_1P (GWF_SINGULAR|GWF_1PERSON)
#define S_2P (GWF_SINGULAR|GWF_2PERSON)
#define S_3P (GWF_SINGULAR|GWF_3PERSON)
#define PL_1P (GWF_PLURAL|GWF_1PERSON)
#define PL_2P (GWF_PLURAL|GWF_2PERSON)
#define PL_3P (GWF_PLURAL|GWF_3PERSON)

#define SNM (GWF_SINGULAR|GWF_NOMITIVE|GWF_MASCULINE)
#define SNF (GWF_SINGULAR|GWF_NOMITIVE|GWF_FEMININE )
#define SNN (GWF_SINGULAR|GWF_NOMITIVE|GWF_NEUTER)
#define PNM (GWF_PLURAL|GWF_NOMITIVE|GWF_MASCULINE)
#define PNF (GWF_PLURAL|GWF_NOMITIVE|GWF_FEMININE )
#define PNN (GWF_PLURAL|GWF_NOMITIVE|GWF_NEUTER)

#define SAM (GWF_SINGULAR|GWF_ACCUSATIVE|GWF_MASCULINE)
#define SAF (GWF_SINGULAR|GWF_ACCUSATIVE|GWF_FEMININE )
#define SAN (GWF_SINGULAR|GWF_ACCUSATIVE|GWF_NEUTER)
#define PAM (GWF_PLURAL|GWF_ACCUSATIVE|GWF_MASCULINE)
#define PAF (GWF_PLURAL|GWF_ACCUSATIVE|GWF_FEMININE )
#define PAN (GWF_PLURAL|GWF_ACCUSATIVE|GWF_NEUTER)

#define SGM (GWF_SINGULAR|GWF_GENITIVE|GWF_MASCULINE)
#define SGF (GWF_SINGULAR|GWF_GENITIVE|GWF_FEMININE )
#define SGN (GWF_SINGULAR|GWF_GENITIVE|GWF_NEUTER)
#define PGM (GWF_PLURAL|GWF_GENITIVE|GWF_MASCULINE)
#define PGF (GWF_PLURAL|GWF_GENITIVE|GWF_FEMININE )
#define PGN (GWF_PLURAL|GWF_GENITIVE|GWF_NEUTER)

#define SDM (GWF_SINGULAR|GWF_DATIVE|GWF_MASCULINE)
#define SDF (GWF_SINGULAR|GWF_DATIVE|GWF_FEMININE )
#define SDN (GWF_SINGULAR|GWF_DATIVE|GWF_NEUTER)
#define PDM (GWF_PLURAL|GWF_DATIVE|GWF_MASCULINE)
#define PDF (GWF_PLURAL|GWF_DATIVE|GWF_FEMININE )
#define PDN (GWF_PLURAL|GWF_DATIVE|GWF_NEUTER)

class CDictFileBlk {
  U8 greek[64]; //64
  U8 trans[64]; //64
  I64 flags; //8
  I32 trie_offsets[27+1]; //112
  //total 248
};
class CGTrie {
  CGreekWord *word;
  CGTrie *ents[27+1]; //0 is alternate meaning
};
I64 Letter2TrieIdx(U8 ch) {
  switch(ch) {
   case 'a'...'z': return ch-'a'+1;
   case 'E': return 27;
  }
  "Dude,got a %c(%d)\n",ch,ch;
  throw('Trie');
} 
CGTrie *trie_root=CAlloc(sizeof CGTrie);
CGTrie *__TrieGetWord(U8 *str,CGTrie *trie=trie_root) {
  I64 idx;
  if(!trie) return NULL;
  if(!*str) return trie;
  idx=Letter2TrieIdx(*str);
  return __TrieGetWord(str+1,trie->ents[idx]);
}
CGTrie *TrieGetWord(U8 *str,CGTrie *trie=trie_root) {
  CGTrie *r=__TrieGetWord(str,trie);
  if(!r) return NULL;
  if(!r->word) return NULL;
  return r;
}
Bool HasWordAlready(CGreekWord *word) {
  CGTrie *trie=TrieGetWord(word->str);
  if(!trie) return FALSE;
  if(!trie->word) return FALSE;
  for(;trie;trie=trie->ents[0])
    if(word->flags==trie->word->flags)
      return TRUE;
  return FALSE;
}
U0 TrieAddWord(CGreekWord *word,CGTrie *trie=trie_root,U8 *ptr=NULL) {
  CGTrie *new_trie;
  I64 idx;
  if(!ptr) ptr=word->str;
  if(!*ptr) {
    if(trie->word) {
      new_trie=CAlloc(sizeof CGTrie);
      new_trie->ents[0]=trie->ents[0];
      trie->ents[0]=new_trie;
      new_trie->word=word;
    } else {
      trie->word=word;
    }
  } else {
    loop:
    if(trie->ents[idx=Letter2TrieIdx(*ptr)]) {
      TrieAddWord(word,trie->ents[idx],ptr+1);
    } else {
      new_trie=CAlloc(sizeof CGTrie);
      trie->ents[idx]=new_trie;
      goto loop;
    }
  }
}
#assert sizeof(CDictFileBlk)<256
I64 __SaveTrie(CGTrie *trie,I64 fpos,CFile *file) {
  U8 _buffer[BLK_SIZE];
  U8 *buffer=_buffer;
  I64 opos=fpos,i;
  CDictFileBlk blk;
  if(trie->word)
    StrCpy(blk.greek,trie->word->str);
  else
    StrCpy(blk.greek,"");
  StrCpy(blk.trans,"");
  if(trie->word)
    blk.flags=trie->word->flags;
  //Write dummy data
  if(!(opos%2))
    FBlkWrite(file,buffer,opos/2,1);
  fpos++;
  for(i=0;i!=27+1;i++) {
    if(trie->ents[i]) {
      blk.trie_offsets[i]=fpos; //Next entry will be written at fpos
      fpos=__SaveTrie(trie->ents[i],fpos,file);
      if(fpos<opos) throw('TSave');
    } else
      blk.trie_offsets[i]=-1;
  }
  //Write actuall data
  FBlkRead(file,buffer,opos/2,1);
  if(opos%2) {
    MemCpy(buffer+256,&blk,sizeof(CDictFileBlk));
  } else
    MemCpy(buffer,&blk,sizeof(CDictFileBlk));
  FBlkWrite(file,buffer,opos/2,1);
  return fpos;
}
U0 SaveTrie(U8 *fn) {
  CFile *file=FOpen(fn,"w+b");
  __SaveTrie(trie_root,0,file);
  FClose(file);
}
U0 __LoadTrie(I64 fpos,CFile *file) {
  I64 i;
  U8 buffer[BLK_SIZE];
  CDictFileBlk blk;
  CGreekWord *word;
  FBlkRead(file,buffer,fpos/2,1);
  if(fpos%2)
    MemCpy(&blk,buffer+256,sizeof(CDictFileBlk));
  else
    MemCpy(&blk,buffer,sizeof(CDictFileBlk));
  if(StrLen(blk.greek)) {
    word=CAlloc(sizeof(CGreekWord));
    word->str=StrNew(blk.greek);
    word->flags=blk.flags;
    if(HasWordAlready(word)) {
      Free(word->str);Free(word);
    } else
      TrieAddWord(word);
  }
  for(i=0;i!=27+1;i++) {
    if(blk.trie_offsets[i]!=-1) {
     if(fpos>blk.trie_offsets[i]) throw('TLoad');
      __LoadTrie(blk.trie_offsets[i],file);
    }
  }
}
U0 LoadTrie(U8 *fn) {
  CFile *file=FOpen(fn,"rb");
  __LoadTrie(0,file);
  FClose(file);
}
Bool EndsWith(U8 *text,U8 *suffix) {
  if(StrLen(text)<StrLen(suffix))
    return FALSE;
  return !StrCmp(text+StrLen(text)-StrLen(suffix),suffix);
}
class CWordEnding {
  U8 *text;
  I64 flags;
  #define WE_SET_VERB1 1
  #define WE_SET_VERB2 2
  #define WE_SET_VERB3 3
  #define WE_SET_VERB4 4
  #define WE_SET_NOUN1 5
  #define WE_SET_NOUN2 6
  #define WE_SET_NOUN3 7
  #define WE_SET_NOUN4 8
  #define WE_SET_NOUN5 9
  #define WE_SET_NOUN6 10
  #define WE_SET_NOUN7 11
  #define WE_SET_NOUN8 12
  #define WE_SET_NOUN9 13
  #define WE_SET_NOUN10 14
  #define WE_SET_NOUN11 15
  #define WE_SET_NOUN12 16
  #define WE_SET_VERB5 17
  #define WE_SET_VERB6 18
  #define WE_SET_VERB7 19
  #define WE_SET_VERB8 20
  #define WE_SET_VERB9 21
  #define WE_SET_VERB10 22
  #define WE_SET_VERB11 23
  #define WE_SET_VERB12 24
  #define WE_SET_VERB13 25
  #define WE_SET_VERB14 26
  #define WE_SET_VERB15 27
  #define WE_SET_VERB16 28
  #define WE_SET_NOUN13 29
  #define WE_SET_NOUN14 30
  #define WE_SET_NOUN15 31
  #define WE_SET_NOUN16 32
  #define WE_SET_VERB17 33
  #define WE_SET_VERB18 34
  #define WE_SET_VERB19 35
  #define WE_SET_VERB20 36
  #define WE_SET_VERB21 37
  #define WE_SET_VERB22 38
  #define WE_SET_VERB23 39
  #define WE_SET_VERB24 40
  #define WE_SET_VERB25 41
  #define WE_SET_NOUN17 42
  #define WE_SET_VERB26 43
  #define WE_SET_VERB27 44
  #define WE_SET_VERB28 45
  #define WE_SET_NOUN18 46
  #define WE_SET_NOUN19 47
  #define WE_SET_NOUN20 48
  #define WE_SET_NOUN21 49
  #define WE_SET_NOUN22 50
  #define WE_SET_NOUN23 51
  #define WE_SET_NOUN24 52

  I64 set;
};

Bool StartsWith(U8 *str,U8 *b) {
  return !StrNICmp(str,b,StrLen(b));
}
U0 StrCat(U8 *a,U8 *b) {
  StrCpy(a+StrLen(a),b);
}
//Verb rules of contract
//3rd Declension merge
U8 *GreekMergeCat3(U8 *_word,U8 *other,I64 flags) {
//https://en.wikipedia.org/wiki/Ancient_Greek_nouns
  I64 trim=0;
  U8 *word=MAlloc(STR_LEN);
  StrCpy(word,_word);
  if((
        EndsWith(word,"k")||
        EndsWith(word,"g")||
        EndsWith(word,"ch")
        )&&other[0]=='s') {
    trim=1;
    if(!(flags&GWF_DATIVE))
      StrCat(word,"z");
    else
      StrCat(word,"zi");  
  } else if((
        EndsWith(word,"p")||
        EndsWith(word,"b")||
        EndsWith(word,"ph")
        )&&other[0]=='s') {
    trim=1;
    if(!(flags&GWF_DATIVE))
      StrCat(word,"ps");
    else
      StrCat(word,"psi");  
  } else if((
        EndsWith(word,"t")||
        EndsWith(word,"d")||
        EndsWith(word,"th")
        )&&other[0]=='s') {
    trim=1;
    if(EndsWith(word,"th")) word[StrLen(word)-2]=0;
    else word[StrLen(word)-1]=0;
    if(!(flags&GWF_DATIVE))
      StrCat(word,"ps");
    else
      StrCat(word,"psi");  
  }/* else if(EndsWith(word,"nt")&&flags&SNF==SNF){
    
  }*/
  
  StrCat(word,other+trim);
  return word;

}
CWordEnding endings[44*8+5+3*9+5*7]={
  //8(1)
  {"numi",S_1P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"numen",PL_1P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"nus",S_2P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"nute",PL_2P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"nusi",S_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"nuasi",PL_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"nuisin",S_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"nuasin",PL_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  //8(2)
  {"wmi",S_1P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"omen",PL_1P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"ws",S_2P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"ote",PL_2P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"wsi",S_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"osi",PL_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"wsin",S_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  {"osin",PL_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB2},
  //8(3)
  {"Emai",S_1P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB3},
  {"emen",PL_1P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB3},
  {"Es",S_2P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB3},
  {"ete",PL_2P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB3},
  {"Esi",S_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB3},
  {"esi",PL_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB3},
  {"Esin",S_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB3},
  {"esin",PL_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB3},
  //8(4)
  {"Emai",S_1P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB4},
  {"amen",PL_1P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB4},
  {"Es",S_2P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB4},
  {"ate",PL_2P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB4},
  {"Esi",S_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB4},
  {"asi",PL_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB4},
  {"Esin",S_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB4},
  {"asin",PL_3P|GWF_VERB|GWF_TENSE_PRESENT,WE_SET_VERB4},
  //8(5)
  {"s",SNM|GWF_NOUN,WE_SET_NOUN1},
  {"es",PNM|GWF_NOUN,WE_SET_NOUN1},
  {"a",SAM|GWF_NOUN,WE_SET_NOUN1},
  {"as",PAM|GWF_NOUN,WE_SET_NOUN1},
  {"os",SGM|GWF_NOUN,WE_SET_NOUN1},
  {"wn",PGM|GWF_NOUN,WE_SET_NOUN1},
  {"i",SDM|GWF_NOUN,WE_SET_NOUN1},
  {"is",PDM|GWF_NOUN,WE_SET_NOUN1},
  //1(1)
  {"in",SDM|GWF_NOUN,WE_SET_NOUN1},
  //8(6)
  {"s", SNM|GWF_NOUN,WE_SET_NOUN2},
  {"des", PNM|GWF_NOUN,WE_SET_NOUN2},
  {"dos", SGM|GWF_NOUN,WE_SET_NOUN2},
  {"dwn", PGM|GWF_NOUN,WE_SET_NOUN2},
  {"di", SDM|GWF_NOUN,WE_SET_NOUN2},
  {"si", PDM|GWF_NOUN,WE_SET_NOUN2},
  {"da", SAM|GWF_NOUN,WE_SET_NOUN2},
  {"das", PAM|GWF_NOUN,WE_SET_NOUN2},
  //1(2)
  {"sin",SDM|GWF_NOUN,WE_SET_NOUN2},
  //8(7)
  {"wn",SNM|GWF_NOUN,WE_SET_NOUN3},
  {"mones",PNM|GWF_NOUN,WE_SET_NOUN3},
  {"monos",SGM|GWF_NOUN,WE_SET_NOUN3},
  {"monwn",PGM|GWF_NOUN,WE_SET_NOUN3},
  {"moni",SDM|GWF_NOUN,WE_SET_NOUN3},
  {"mosi",PDM|GWF_NOUN,WE_SET_NOUN3},
  {"mona",SAM|GWF_NOUN,WE_SET_NOUN3},
  {"monas",PAM|GWF_NOUN,WE_SET_NOUN3},
  //1(3)
  {"mosin",SDM|GWF_NOUN,WE_SET_NOUN3},
  //8(8)
  {"wn",SNM|GWF_NOUN,WE_SET_NOUN4},
  {"ontes",PNM|GWF_NOUN,WE_SET_NOUN4},
  {"ontos",SGM|GWF_NOUN,WE_SET_NOUN4},
  {"ontwn",PGM|GWF_NOUN,WE_SET_NOUN4},
  {"onti",SDM|GWF_NOUN,WE_SET_NOUN4},
  {"ousi",PDM|GWF_NOUN,WE_SET_NOUN4},
  {"onta",SAM|GWF_NOUN,WE_SET_NOUN4},
  {"ontas",PAM|GWF_NOUN,WE_SET_NOUN4},
  //1(4)
  {"ousin",SDM|GWF_NOUN,WE_SET_NOUN4},
  //8(9)
  {"wn",SNM|GWF_NOUN,WE_SET_NOUN5},
  {"wnes",PNM|GWF_NOUN,WE_SET_NOUN5},
  {"wnos",SGM|GWF_NOUN,WE_SET_NOUN5},
  {"wnwn",PGM|GWF_NOUN,WE_SET_NOUN5},
  {"wnni",SDM|GWF_NOUN,WE_SET_NOUN5},
  {"wnsi",PDM|GWF_NOUN,WE_SET_NOUN5},
  {"wna",SAM|GWF_NOUN,WE_SET_NOUN5},
  {"wnas",PAM|GWF_NOUN,WE_SET_NOUN5},
  //1(5)
  {"wnsin",SDM|GWF_NOUN,WE_SET_NOUN5},
  //8(10)
  {"s",SNF|GWF_NOUN,WE_SET_NOUN6},
  {"es",PNF|GWF_NOUN,WE_SET_NOUN6},
  {"a",SAF|GWF_NOUN,WE_SET_NOUN6},
  {"as",PAF|GWF_NOUN,WE_SET_NOUN6},
  {"os",SGF|GWF_NOUN,WE_SET_NOUN6},
  {"wn",PGF|GWF_NOUN,WE_SET_NOUN6},
  {"i",SDF|GWF_NOUN,WE_SET_NOUN6},
  {"si",SDF|GWF_NOUN,WE_SET_NOUN6},
  //8(11)
  {"s",SNF|GWF_NOUN,WE_SET_NOUN7},
  {"des",PNF|GWF_NOUN,WE_SET_NOUN7},
  {"dos",SGF|GWF_NOUN,WE_SET_NOUN7},
  {"dwn",PGF|GWF_NOUN,WE_SET_NOUN7},
  {"di",SDF|GWF_NOUN,WE_SET_NOUN7},
  {"si",PDF|GWF_NOUN,WE_SET_NOUN7},
  {"da",SAF|GWF_NOUN,WE_SET_NOUN7},
  {"das",PAF|GWF_NOUN,WE_SET_NOUN7},
  //8(12)
  {"x",SNF|GWF_NOUN,WE_SET_NOUN8},
  {"tes",PNF|GWF_NOUN,WE_SET_NOUN8},
  {"tos",SGF|GWF_NOUN,WE_SET_NOUN8},
  {"twn",PGF|GWF_NOUN,WE_SET_NOUN8},
  {"ti",SDF|GWF_NOUN,WE_SET_NOUN8},
  {"xi",PDF|GWF_NOUN,WE_SET_NOUN8},
  {"ta",SAF|GWF_NOUN,WE_SET_NOUN8},
  {"tas",PAF|GWF_NOUN,WE_SET_NOUN8},
  //8(13)
  {"",SNN|GWF_NOUN,WE_SET_NOUN9},
  {"a",PNN|GWF_NOUN,WE_SET_NOUN9},
  {"os",SGN|GWF_NOUN,WE_SET_NOUN9},
  {"wn",PGN|GWF_NOUN,WE_SET_NOUN9},
  {"i",SDN|GWF_NOUN,WE_SET_NOUN9},
  {"si",PDN|GWF_NOUN,WE_SET_NOUN9},
  {"",SAN|GWF_NOUN,WE_SET_NOUN9},
  {"a",PAN|GWF_NOUN,WE_SET_NOUN9},
  //8(14)
  {"a",SNN|GWF_NOUN,WE_SET_NOUN10},
  {"ata",PNN|GWF_NOUN,WE_SET_NOUN10},
  {"atos",SGN|GWF_NOUN,WE_SET_NOUN10},
  {"atwn",PGN|GWF_NOUN,WE_SET_NOUN10},
  {"ati",SDN|GWF_NOUN,WE_SET_NOUN10},
  {"atasi",PDN|GWF_NOUN,WE_SET_NOUN10},
  {"a",SAN|GWF_NOUN,WE_SET_NOUN10},
  {"ata",PAN|GWF_NOUN,WE_SET_NOUN10},
  //8(15)
  {"w",GWF_TENSE_PRESENT|GWF_VERB|S_1P,WE_SET_VERB5},
  {"omen",GWF_TENSE_PRESENT|GWF_VERB|PL_1P,WE_SET_VERB5},
  {"eis",GWF_TENSE_PRESENT|GWF_VERB|S_2P,WE_SET_VERB5},
  {"ete",GWF_TENSE_PRESENT|GWF_VERB|PL_2P,WE_SET_VERB5},
  {"ei",GWF_TENSE_PRESENT|GWF_VERB|S_3P,WE_SET_VERB5},
  {"ousi",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB5},
  {"ousin",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB5},
  {"ein",GWF_VERB|GWF_INFINITIVE,WE_SET_VERB5},
  //8(16)
  {"anw",GWF_TENSE_PRESENT|GWF_VERB|S_1P,WE_SET_VERB6},
  {"anomen",GWF_TENSE_PRESENT|GWF_VERB|PL_1P,WE_SET_VERB6},
  {"aneis",GWF_TENSE_PRESENT|GWF_VERB|S_2P,WE_SET_VERB6},
  {"anete",GWF_TENSE_PRESENT|GWF_VERB|PL_2P,WE_SET_VERB6},
  {"anei",GWF_TENSE_PRESENT|GWF_VERB|S_3P,WE_SET_VERB6},
  {"anousi",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB6},
  {"anousin",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB6},
  {"anein",GWF_VERB|GWF_INFINITIVE,WE_SET_VERB6},
  //8(17)
  {"skw",GWF_TENSE_PRESENT|GWF_VERB|S_1P,WE_SET_VERB7},
  {"skomen",GWF_TENSE_PRESENT|GWF_VERB|PL_1P,WE_SET_VERB7},
  {"skeis",GWF_TENSE_PRESENT|GWF_VERB|S_2P,WE_SET_VERB7},
  {"skete",GWF_TENSE_PRESENT|GWF_VERB|PL_2P,WE_SET_VERB7},
  {"skei",GWF_TENSE_PRESENT|GWF_VERB|S_3P,WE_SET_VERB7},
  {"skousi",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB7},
  {"skousin",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB7},
  {"skein",GWF_VERB|GWF_INFINITIVE,WE_SET_VERB7},
  //8(18)
  {"sw",GWF_TENSE_FUTURE|GWF_VERB|S_1P,WE_SET_VERB8},
  {"somen",GWF_TENSE_FUTURE|GWF_VERB|PL_1P,WE_SET_VERB8},
  {"seis",GWF_TENSE_FUTURE|GWF_VERB|S_2P,WE_SET_VERB8},
  {"sete",GWF_TENSE_FUTURE|GWF_VERB|PL_2P,WE_SET_VERB8},
  {"sei",GWF_TENSE_FUTURE|GWF_VERB|S_3P,WE_SET_VERB8},
  {"sousi",GWF_TENSE_FUTURE|GWF_VERB|PL_3P,WE_SET_VERB8},
  {"sousin",GWF_TENSE_FUTURE|GWF_VERB|PL_3P,WE_SET_VERB8},
  {"sein",GWF_TENSE_FUTURE|GWF_VERB|GWF_INFINITIVE,WE_SET_VERB8},
  //8(19)
  {"xw",GWF_TENSE_FUTURE|GWF_VERB|S_1P,WE_SET_VERB9},
  {"xomen",GWF_TENSE_FUTURE|GWF_VERB|PL_1P,WE_SET_VERB9},
  {"xeis",GWF_TENSE_FUTURE|GWF_VERB|S_2P,WE_SET_VERB9},
  {"xete",GWF_TENSE_FUTURE|GWF_VERB|PL_2P,WE_SET_VERB9},
  {"xei",GWF_TENSE_FUTURE|GWF_VERB|S_3P,WE_SET_VERB9},
  {"xousi",GWF_TENSE_FUTURE|GWF_VERB|PL_3P,WE_SET_VERB9},
  {"xousin",GWF_TENSE_FUTURE|GWF_VERB|PL_3P,WE_SET_VERB9},
  {"xein",GWF_TENSE_FUTURE|GWF_VERB|GWF_INFINITIVE,WE_SET_VERB9},
  //8(20)
  {"psw",GWF_TENSE_FUTURE|GWF_VERB|S_1P,WE_SET_VERB10},
  {"psomen",GWF_TENSE_FUTURE|GWF_VERB|PL_1P,WE_SET_VERB10},
  {"pseis",GWF_TENSE_FUTURE|GWF_VERB|S_2P,WE_SET_VERB10},
  {"psete",GWF_TENSE_FUTURE|GWF_VERB|PL_2P,WE_SET_VERB10},
  {"psei",GWF_TENSE_FUTURE|GWF_VERB|S_3P,WE_SET_VERB10},
  {"psousi",GWF_TENSE_FUTURE|GWF_VERB|PL_3P,WE_SET_VERB10},
  {"psousin",GWF_TENSE_FUTURE|GWF_VERB|PL_3P,WE_SET_VERB10},
  {"psein",GWF_TENSE_FUTURE|GWF_VERB|GWF_INFINITIVE,WE_SET_VERB10},
  //8(21)
  {"w",GWF_TENSE_PRESENT|GWF_VERB|S_1P,WE_SET_VERB11},
  {"oumen",GWF_TENSE_PRESENT|GWF_VERB|PL_1P,WE_SET_VERB11},
  {"eis",GWF_TENSE_PRESENT|GWF_VERB|S_2P,WE_SET_VERB11},
  {"eite",GWF_TENSE_PRESENT|GWF_VERB|PL_2P,WE_SET_VERB11},
  {"ei",GWF_TENSE_PRESENT|GWF_VERB|S_3P,WE_SET_VERB11},
  {"ousi",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB11},
  {"ousin",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB11},
  {"ein",GWF_TENSE_PRESENT|GWF_VERB|GWF_INFINITIVE,WE_SET_VERB11},
  //8(22)
  {"ew",GWF_TENSE_PRESENT|GWF_VERB|S_1P,WE_SET_VERB12},
  {"eomen",GWF_TENSE_PRESENT|GWF_VERB|PL_1P,WE_SET_VERB12},
  {"eis",GWF_TENSE_PRESENT|GWF_VERB|S_2P,WE_SET_VERB12},
  {"eite",GWF_TENSE_PRESENT|GWF_VERB|PL_2P,WE_SET_VERB12},
  {"ei",GWF_TENSE_PRESENT|GWF_VERB|S_3P,WE_SET_VERB12},
  {"eousi",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB12},
  {"eousin",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB12},
  {"ein",GWF_TENSE_PRESENT|GWF_VERB|GWF_INFINITIVE,WE_SET_VERB12},
  //8(23)
  {"Esw",GWF_TENSE_FUTURE|GWF_VERB|S_1P,WE_SET_VERB13},
  {"Esomen",GWF_TENSE_FUTURE|GWF_VERB|PL_1P,WE_SET_VERB13},
  {"Eseis",GWF_TENSE_FUTURE|GWF_VERB|S_2P,WE_SET_VERB13},
  {"Esete",GWF_TENSE_FUTURE|GWF_VERB|PL_2P,WE_SET_VERB13},
  {"Esei",GWF_TENSE_FUTURE|GWF_VERB|S_3P,WE_SET_VERB13},
  {"Esousi",GWF_TENSE_FUTURE|GWF_VERB|PL_3P,WE_SET_VERB13},
  {"Esousin",GWF_TENSE_FUTURE|GWF_VERB|PL_3P,WE_SET_VERB13},
  {"Esein",GWF_TENSE_FUTURE|GWF_VERB|GWF_INFINITIVE,WE_SET_VERB13},
  //8(24)
  {"w",GWF_TENSE_PRESENT|GWF_VERB|S_1P,WE_SET_VERB14},
  {"wmen",GWF_TENSE_PRESENT|GWF_VERB|PL_1P,WE_SET_VERB14},
  {"wtais",GWF_TENSE_PRESENT|GWF_VERB|S_2P,WE_SET_VERB14},
  {"ate",GWF_TENSE_PRESENT|GWF_VERB|PL_2P,WE_SET_VERB14},
  {"ai",GWF_TENSE_PRESENT|GWF_VERB|S_3P,WE_SET_VERB14},
  {"wsi",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB14},
  {"wsin",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB14},
  {"an",GWF_TENSE_PRESENT|GWF_VERB|GWF_INFINITIVE,WE_SET_VERB14},
  //8(25)
  {"w",GWF_TENSE_PRESENT|GWF_VERB|S_1P,WE_SET_VERB15},
  {"oumen",GWF_TENSE_PRESENT|GWF_VERB|PL_1P,WE_SET_VERB15},
  {"ois",GWF_TENSE_PRESENT|GWF_VERB|S_2P,WE_SET_VERB15},
  {"oute",GWF_TENSE_PRESENT|GWF_VERB|PL_2P,WE_SET_VERB15},
  {"oi",GWF_TENSE_PRESENT|GWF_VERB|S_3P,WE_SET_VERB15},
  {"oisi",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB15},
  {"ousin",GWF_TENSE_PRESENT|GWF_VERB|PL_3P,WE_SET_VERB15},
  {"oun",GWF_TENSE_PRESENT|GWF_VERB|GWF_INFINITIVE,WE_SET_VERB15},
  //8(26)
  {"wsw",GWF_TENSE_FUTURE|GWF_VERB|S_1P,WE_SET_VERB16},
  {"wsomen",GWF_TENSE_FUTURE|GWF_VERB|PL_1P,WE_SET_VERB16},
  {"wseis",GWF_TENSE_FUTURE|GWF_VERB|S_2P,WE_SET_VERB16},
  {"wsete",GWF_TENSE_FUTURE|GWF_VERB|PL_2P,WE_SET_VERB16},
  {"wsei",GWF_TENSE_FUTURE|GWF_VERB|S_3P,WE_SET_VERB16},
  {"wsousi",GWF_TENSE_FUTURE|GWF_VERB|PL_3P,WE_SET_VERB16},
  {"wsousin",GWF_TENSE_FUTURE|GWF_VERB|PL_3P,WE_SET_VERB16},
  {"wsein",GWF_TENSE_FUTURE|GWF_VERB|GWF_INFINITIVE,WE_SET_VERB16},
  //8(27)
  {"E",SNF|GWF_NOUN,WE_SET_NOUN13},
  {"ai",PNF|GWF_NOUN,WE_SET_NOUN13},
  {"Es",SGF|GWF_NOUN,WE_SET_NOUN13},
  {"wn",SGF|GWF_NOUN,WE_SET_NOUN13},
  {"Ei",SDF|GWF_NOUN,WE_SET_NOUN13},
  {"ais",PDF|GWF_NOUN,WE_SET_NOUN13},
  {"En",SAF|GWF_NOUN,WE_SET_NOUN13},
  {"as",PAF|GWF_NOUN,WE_SET_NOUN13},
  //8(28)
  {"a",SNF|GWF_NOUN,WE_SET_NOUN14},
  {"ai",PNF|GWF_NOUN,WE_SET_NOUN14},
  {"as",SGF|GWF_NOUN,WE_SET_NOUN14},
  {"wn",SGF|GWF_NOUN,WE_SET_NOUN14},
  {"ai",SDF|GWF_NOUN,WE_SET_NOUN14},
  {"ais",PDF|GWF_NOUN,WE_SET_NOUN14},
  {"an",SAF|GWF_NOUN,WE_SET_NOUN14},
  {"as",PAF|GWF_NOUN,WE_SET_NOUN14},
  //8(29)
  {"a",SNF|GWF_NOUN,WE_SET_NOUN15},
  {"ai",PNF|GWF_NOUN,WE_SET_NOUN15},
  {"Es",SGF|GWF_NOUN,WE_SET_NOUN15},
  {"wn",SGF|GWF_NOUN,WE_SET_NOUN15},
  {"Ei",SDF|GWF_NOUN,WE_SET_NOUN15},
  {"ais",PDF|GWF_NOUN,WE_SET_NOUN15},
  {"an",SAF|GWF_NOUN,WE_SET_NOUN15},
  {"as",PAF|GWF_NOUN,WE_SET_NOUN15},
  //8(30)
  {"ou",SNF|GWF_NOUN,WE_SET_NOUN16},
  {"ai",PNF|GWF_NOUN,WE_SET_NOUN16},
  {"ou",SGF|GWF_NOUN,WE_SET_NOUN16},
  {"wn",SGF|GWF_NOUN,WE_SET_NOUN16},
  {"Ei",SDF|GWF_NOUN,WE_SET_NOUN16},
  {"ais",PDF|GWF_NOUN,WE_SET_NOUN16},
  {"En",SAF|GWF_NOUN,WE_SET_NOUN16},
  {"as",PAF|GWF_NOUN,WE_SET_NOUN16}
  //8(31)
  {"os",GWF_MASCULINE|SNF|GWF_NOUN,WE_SET_NOUN17},
  {"oi",GWF_MASCULINE|PNF|GWF_NOUN,WE_SET_NOUN17},
  {"ou",GWF_MASCULINE|SGF|GWF_NOUN,WE_SET_NOUN17},
  {"wn",GWF_MASCULINE|SGF|GWF_NOUN,WE_SET_NOUN17},
  {"wi",GWF_MASCULINE|SDF|GWF_NOUN,WE_SET_NOUN17},
  {"ois",GWF_MASCULINE|PDF|GWF_NOUN,WE_SET_NOUN17},
  {"on",GWF_MASCULINE|SAF|GWF_NOUN,WE_SET_NOUN17},
  {"ous",GWF_MASCULINE|PAF|GWF_NOUN,WE_SET_NOUN17}
  //8(32)
  {"on",SNN|GWF_NOUN,WE_SET_NOUN17},
  {"a",PNN|GWF_NOUN,WE_SET_NOUN17},
  {"ou",SGN|GWF_NOUN,WE_SET_NOUN17},
  {"wn",SGN|GWF_NOUN,WE_SET_NOUN17},
  {"wi",SDN|GWF_NOUN,WE_SET_NOUN17},
  {"ois",PDN|GWF_NOUN,WE_SET_NOUN17},
  {"on",SAN|GWF_NOUN,WE_SET_NOUN17},
  {"a",PAN|GWF_NOUN,WE_SET_NOUN17}
  //7(1)
  {"mai",GWF_MIDDLE|S_1P,WE_SET_VERB17},
  {"metha",GWF_MIDDLE|PL_1P,WE_SET_VERB17},
  {"sai",GWF_MIDDLE|S_2P,WE_SET_VERB17},
  {"sthe",GWF_MIDDLE|PL_2P,WE_SET_VERB17},
  {"tai",GWF_MIDDLE|S_3P,WE_SET_VERB17},
  {"ntai",GWF_MIDDLE|PL_3P,WE_SET_VERB17},
  {"sthai",GWF_MIDDLE|GWF_INFINITIVE,WE_SET_VERB17},
  //9(1)
  {"omai",GWF_MIDDLE|S_1P,WE_SET_VERB18},
  {"ometha",GWF_MIDDLE|PL_1P,WE_SET_VERB18},
  {"esai",GWF_MIDDLE|S_2P,WE_SET_VERB18},
  {"ei",GWF_MIDDLE|S_2P,WE_SET_VERB18},
  {"Ei",GWF_MIDDLE|S_2P,WE_SET_VERB18},
  {"esthai",GWF_MIDDLE|PL_2P,WE_SET_VERB18},
  {"etai",GWF_MIDDLE|S_3P,WE_SET_VERB18},
  {"ontai",GWF_MIDDLE|PL_3P,WE_SET_VERB18},
  {"esthai",GWF_MIDDLE|GWF_INFINITIVE,WE_SET_VERB18},
  //7(2)
  {"numai",GWF_MIDDLE|S_1P,WE_SET_VERB19},
  {"numetha",GWF_MIDDLE|PL_1P,WE_SET_VERB19},
  {"nusai",GWF_MIDDLE|S_2P,WE_SET_VERB19},
  {"nusthe",GWF_MIDDLE|PL_2P,WE_SET_VERB19},
  {"nutai",GWF_MIDDLE|S_3P,WE_SET_VERB19},
  {"nuntai",GWF_MIDDLE|PL_3P,WE_SET_VERB19},
  {"nuthai",GWF_MIDDLE|GWF_INFINITIVE,WE_SET_VERB19},
  //9(2)
  {"xomai",GWF_MIDDLE|S_1P,WE_SET_VERB20},
  {"xometha",GWF_MIDDLE|PL_1P,WE_SET_VERB20},
  {"xesai",GWF_MIDDLE|S_2P,WE_SET_VERB20},
  {"xei",GWF_MIDDLE|S_2P,WE_SET_VERB20},
  {"xEi",GWF_MIDDLE|S_2P,WE_SET_VERB20},
  {"xesthe",GWF_MIDDLE|PL_2P,WE_SET_VERB20},
  {"xetai",GWF_MIDDLE|S_3P,WE_SET_VERB20},
  {"xontai",GWF_MIDDLE|PL_3P,WE_SET_VERB20},
  {"xesthai",GWF_MIDDLE|GWF_INFINITIVE,WE_SET_VERB20},
  //9(3)
  {"somai",GWF_MIDDLE|S_1P,WE_SET_VERB21},
  {"sometha",GWF_MIDDLE|PL_1P,WE_SET_VERB21},
  {"sesai",GWF_MIDDLE|S_2P,WE_SET_VERB21},
  {"sei",GWF_MIDDLE|S_2P,WE_SET_VERB21},
  {"sEi",GWF_MIDDLE|S_2P,WE_SET_VERB21},
  {"sesthe",GWF_MIDDLE|PL_2P,WE_SET_VERB21},
  {"setai",GWF_MIDDLE|S_3P,WE_SET_VERB21},
  {"sontai",GWF_MIDDLE|PL_3P,WE_SET_VERB21},
  {"sesthai",GWF_MIDDLE|GWF_INFINITIVE,WE_SET_VERB21},
  //8(33)
  {"oumai",GWF_MIDDLE|S_1P,WE_SET_VERB22},
  {"oumetha",GWF_MIDDLE|PL_1P,WE_SET_VERB22},
  {"ei",GWF_MIDDLE|S_2P,WE_SET_VERB22},
  {"Ei",GWF_MIDDLE|S_2P,WE_SET_VERB22},
  {"eisthe",GWF_MIDDLE|PL_2P,WE_SET_VERB22},
  {"eitai",GWF_MIDDLE|S_3P,WE_SET_VERB22},
  {"ountai",GWF_MIDDLE|PL_3P,WE_SET_VERB22},
  {"eisthai",GWF_MIDDLE|GWF_INFINITIVE,WE_SET_VERB22},  
  //8(34)
  {"wmai",GWF_MIDDLE|S_1P,WE_SET_VERB23},
  {"wmetha",GWF_MIDDLE|PL_1P,WE_SET_VERB23},
  {"ai",GWF_MIDDLE|S_2P,WE_SET_VERB23},
  {"ai",GWF_MIDDLE|S_2P,WE_SET_VERB23},
  {"asthe",GWF_MIDDLE|PL_2P,WE_SET_VERB23},
  {"atai",GWF_MIDDLE|S_3P,WE_SET_VERB23},
  {"wntai",GWF_MIDDLE|PL_3P,WE_SET_VERB23},
  {"asthai",GWF_MIDDLE|GWF_INFINITIVE,WE_SET_VERB23},
  //8(35)
  {"oumai",GWF_MIDDLE|S_1P,WE_SET_VERB25},
  {"oumetha",GWF_MIDDLE|PL_1P,WE_SET_VERB25},
  {"oi",GWF_MIDDLE|S_2P,WE_SET_VERB25},
  {"oi",GWF_MIDDLE|S_2P,WE_SET_VERB25},
  {"ousthe",GWF_MIDDLE|PL_2P,WE_SET_VERB25},
  {"outai",GWF_MIDDLE|S_3P,WE_SET_VERB25},
  {"ountai",GWF_MIDDLE|PL_3P,WE_SET_VERB25},
  {"ousthai",GWF_MIDDLE|GWF_INFINITIVE,WE_SET_VERB25},
  //7(3)
  {"emai",GWF_MIDDLE|S_1P,WE_SET_VERB26},
  {"emetha",GWF_MIDDLE|PL_1P,WE_SET_VERB26},
  {"esai",GWF_MIDDLE|S_2P,WE_SET_VERB26},
  {"esthe",GWF_MIDDLE|PL_2P,WE_SET_VERB26},
  {"etai",GWF_MIDDLE|S_3P,WE_SET_VERB26},
  {"entai",GWF_MIDDLE|PL_3P,WE_SET_VERB26},
  {"esthai",GWF_MIDDLE|GWF_INFINITIVE,WE_SET_VERB26},
  //7(4)
  {"amai",GWF_MIDDLE|S_1P,WE_SET_VERB27},
  {"ametha",GWF_MIDDLE|PL_1P,WE_SET_VERB27},
  {"asai",GWF_MIDDLE|S_2P,WE_SET_VERB27},
  {"asthe",GWF_MIDDLE|PL_2P,WE_SET_VERB27},
  {"atai",GWF_MIDDLE|S_3P,WE_SET_VERB27},
  {"antai",GWF_MIDDLE|PL_3P,WE_SET_VERB27},
  {"asthai",GWF_MIDDLE|GWF_INFINITIVE,WE_SET_VERB27},
  //7(5)
  {"psomai",GWF_TENSE_FUTURE|GWF_MIDDLE|S_1P,WE_SET_VERB28},
  {"psometha",GWF_TENSE_FUTURE|GWF_MIDDLE|PL_1P,WE_SET_VERB28},
  {"psosai",GWF_MIDDLE|S_2P,WE_SET_VERB28},
  {"psosthe",GWF_TENSE_FUTURE|GWF_MIDDLE|PL_2P,WE_SET_VERB28},
  {"psotai",GWF_TENSE_FUTURE|GWF_MIDDLE|S_3P,WE_SET_VERB28},
  {"psontai",GWF_TENSE_FUTURE|GWF_MIDDLE|PL_3P,WE_SET_VERB28},
  {"psosthai",GWF_TENSE_FUTURE|GWF_MIDDLE|GWF_INFINITIVE,WE_SET_VERB28},
  //8(36)
  {"",SNN|GWF_NOUN,WE_SET_NOUN18},
  {"a",PNN|GWF_NOUN,WE_SET_NOUN18},
  {"os",SGN|GWF_NOUN,WE_SET_NOUN18},
  {"wn",PGN|GWF_NOUN,WE_SET_NOUN18},
  {"i",SDN|GWF_NOUN,WE_SET_NOUN18},
  {"si",PDN|GWF_NOUN,WE_SET_NOUN18},
  {"",SAN|GWF_NOUN,WE_SET_NOUN18},
  {"a",PAN|GWF_NOUN,WE_SET_NOUN18},
  //8(37)
  {"Ep",SNN|GWF_NOUN,WE_SET_NOUN19},
  {"epes",PNN|GWF_NOUN,WE_SET_NOUN19},
  {"pos",SGN|GWF_NOUN,WE_SET_NOUN19},
  {"erwn",PGN|GWF_NOUN,WE_SET_NOUN19},
  {"ri",SDN|GWF_NOUN,WE_SET_NOUN19},
  {"rasi",PDN|GWF_NOUN,WE_SET_NOUN19},
  {"era",SAN|GWF_NOUN,WE_SET_NOUN19},
  {"eras",PAN|GWF_NOUN,WE_SET_NOUN19},
  //8(38)
  {"Er",SNN|GWF_NOUN,WE_SET_NOUN19},
  {"dpes",PNN|GWF_NOUN,WE_SET_NOUN19},
  {"dpos",SGN|GWF_NOUN,WE_SET_NOUN19},
  {"drwn",PGN|GWF_NOUN,WE_SET_NOUN19},
  {"dri",SDN|GWF_NOUN,WE_SET_NOUN19},
  {"drasi",PDN|GWF_NOUN,WE_SET_NOUN19},
  {"dera",SAN|GWF_NOUN,WE_SET_NOUN19},
  {"deras",PAN|GWF_NOUN,WE_SET_NOUN19},
  //8(39)
  {"os",SNN|GWF_NOUN,WE_SET_NOUN19},
  {"E",PNN|GWF_NOUN,WE_SET_NOUN19},
  {"ous",SGN|GWF_NOUN,WE_SET_NOUN19},
  {"wn",PGN|GWF_NOUN,WE_SET_NOUN19},
  {"ei",SDN|GWF_NOUN,WE_SET_NOUN19},
  {"esi",PDN|GWF_NOUN,WE_SET_NOUN19},
  {"os",SAN|GWF_NOUN,WE_SET_NOUN19},
  {"E",PAN|GWF_NOUN,WE_SET_NOUN19},
  //8(40)
  {"is",SNF|GWF_NOUN,WE_SET_NOUN20},
  {"eis",PNF|GWF_NOUN,WE_SET_NOUN20},
  {"ews",SGF|GWF_NOUN,WE_SET_NOUN20},
  {"ewn",PGF|GWF_NOUN,WE_SET_NOUN20},
  {"ei",SDF|GWF_NOUN,WE_SET_NOUN20},
  {"esi",PDF|GWF_NOUN,WE_SET_NOUN20},
  {"in",SAF|GWF_NOUN,WE_SET_NOUN20},
  {"eis",PAF|GWF_NOUN,WE_SET_NOUN20},
  //8(41)
  {"us",SNM|GWF_NOUN,WE_SET_NOUN21},
  {"eis",PNM|GWF_NOUN,WE_SET_NOUN21},
  {"ews",SGM|GWF_NOUN,WE_SET_NOUN21},
  {"ewn",PGM|GWF_NOUN,WE_SET_NOUN21},
  {"ei",SDM|GWF_NOUN,WE_SET_NOUN21},
  {"esi",PDM|GWF_NOUN,WE_SET_NOUN21},
  {"un",SAM|GWF_NOUN,WE_SET_NOUN21},
  {"eis",PAM|GWF_NOUN,WE_SET_NOUN21},
  //8(42)
  {"eus",SNM|GWF_NOUN,WE_SET_NOUN22},
  {"Es",PNM|GWF_NOUN,WE_SET_NOUN22},
  {"ews",SGM|GWF_NOUN,WE_SET_NOUN22},
  {"ewn",PGM|GWF_NOUN,WE_SET_NOUN22},
  {"ei",SDM|GWF_NOUN,WE_SET_NOUN22},
  {"eusi",PDM|GWF_NOUN,WE_SET_NOUN22},
  {"ea",SAM|GWF_NOUN,WE_SET_NOUN22},
  {"eas",PAM|GWF_NOUN,WE_SET_NOUN22},
  //8(43)
  {"aus",SNM|GWF_NOUN,WE_SET_NOUN23},
  {"Ees",PNM|GWF_NOUN,WE_SET_NOUN23},
  {"ews",SGM|GWF_NOUN,WE_SET_NOUN23},
  {"ewn",PGM|GWF_NOUN,WE_SET_NOUN23},
  {"Ei",SDM|GWF_NOUN,WE_SET_NOUN23},
  {"ausi",PDM|GWF_NOUN,WE_SET_NOUN23},
  {"ean",SAM|GWF_NOUN,WE_SET_NOUN23},
  {"eas",PAM|GWF_NOUN,WE_SET_NOUN23},
  //8(44)
  {"us",SNM|GWF_NOUN,WE_SET_NOUN24},
  {"es",PNM|GWF_NOUN,WE_SET_NOUN24},
  {"os",SGM|GWF_NOUN,WE_SET_NOUN24},
  {"wn",PGM|GWF_NOUN,WE_SET_NOUN24},
  {"oi",SDM|GWF_NOUN,WE_SET_NOUN24},
  {"usi",PDM|GWF_NOUN,WE_SET_NOUN24},
  {"un",SAM|GWF_NOUN,WE_SET_NOUN24},
  {"us",PAM|GWF_NOUN,WE_SET_NOUN24},


};
U0 ClassifyWord(U8 *str,I64 flags,CWordEnding *decls,I64 cnt) {
  I64 i,trim=-1,set;
  CFifoI64 *matches=FifoI64New(0x80);
  CGreekWord *word;
  for(i=0;i!=cnt;i++) {
    if(flags&(GWF_NOUN|GWF_VERB|GWF_ADJECTIVE)) {
      if(flags&GWF_NOUN&&!(decls[i].flags&GWF_NOUN)) goto next;
      if(flags&GWF_VERB&&!(decls[i].flags&GWF_VERB)) goto next;
      if(flags&GWF_ADJECTIVE&&!(decls[i].flags&GWF_ADJECTIVE)) goto next;
    }
    if(EndsWith(str,decls[i].text)) {
      if(StrLen(decls[i].text)>trim) {
        FifoI64Flush(matches);
        FifoI64Ins(matches,decls[i].set);
        trim=StrLen(decls[i].text);
      } else if(StrLen(decls[i].text)==trim)
        FifoI64Ins(matches,decls[i].set);
    }
next:
  }
  if(trim==-1) {
    FifoI64Del(matches);
    return;
  }
pass:
  trim=StrLen(str)-trim;
  while(FifoI64Rem(matches,&set)) {
    for(i=0;i!=cnt;i++) {
      if(set==decls[i].set) {
        word=CAlloc(sizeof(CGreekWord));
        word->str=MAlloc(STR_LEN);
        MemCpy(word->str,str,trim);
        StrCat(word->str,decls[i].text);
        word->flags=decls[i].flags|flags;
        if(HasWordAlready(word)) {
	  Free(word->str);Free(word);
        } else
	  TrieAddWord(word);
      }
    }
  }
  FifoI64Del(matches);
}

U0 AddTemplatedWord(U8 *str,I64 type=0) {
  I64 trim=0;
  ClassifyWord(str,type,endings,sizeof(endings)/sizeof(CWordEnding));
}
//Will make a MAlloced str
U0 AddWords(CGreekWord *words,I64 cnt) {
  CGreekWord *word;
  I64 i=0;
  for(;i!=cnt;i++) {
    if(!words[i].str) break;
    word=CAlloc(sizeof(CGreekWord));
    word->str=StrNew(words[i].str);
    word->flags=words[i].flags;
    if(HasWordAlready(word)) {
      Free(word->str);Free(word);
    } else
      TrieAddWord(word);
  }
}
  CGreekWord articles[25]={
  {"ho",SNM|GWF_ARTICLE},
        {"hE",SNF|GWF_ARTICLE},
        {"to",SNN|GWF_ARTICLE},
        {"ohi",PNM|GWF_ARTICLE},
        {"ahi",PNF|GWF_ARTICLE},
        {"ta",PNN|GWF_ARTICLE},
        {"ton",SAM|GWF_ARTICLE},
        {"tEn",SAF|GWF_ARTICLE},
        {"to",SAN|GWF_ARTICLE},
        {"tous",PAM|GWF_ARTICLE},
        {"tas",PAF|GWF_ARTICLE},
        {"ta",PAN|GWF_ARTICLE},
        {"tou",SGM|GWF_ARTICLE},
        {"tEs",SGF|GWF_ARTICLE},
        {"tou",SGN|GWF_ARTICLE},
        {"twn",PGM|GWF_ARTICLE},
        {"twn",PGF|GWF_ARTICLE},
        {"twn",PGN|GWF_ARTICLE},
        {"twi",SDM|GWF_ARTICLE},
        {"tEi",SDF|GWF_ARTICLE},
        {"twi",SDN|GWF_ARTICLE},
        {"tois",PDM|GWF_ARTICLE},
        {"tais",PDF|GWF_ARTICLE},
        {"tois",PDN|GWF_ARTICLE},
        {NULL,0},
        }; 
  CGreekWord am[10]={
  {"eimi",S_1P|GWF_VERB|GWF_TENSE_PRESENT},
        {"ei",S_2P|GWF_VERB|GWF_TENSE_PRESENT},
        {"esti",S_3P|GWF_VERB|GWF_TENSE_PRESENT},
        {"estin",S_3P|GWF_VERB|GWF_TENSE_PRESENT},
        {"esmen",PL_1P|GWF_VERB|GWF_TENSE_PRESENT},
        {"este",PL_2P|GWF_VERB|GWF_TENSE_PRESENT},
        {"esi",PL_3P|GWF_VERB|GWF_TENSE_PRESENT},
        {"esin",PL_3P|GWF_VERB|GWF_TENSE_PRESENT},
        {"enai",GWF_INFINITIVE|GWF_TENSE_PRESENT},
        {NULL,0},
        };
  CGreekWord may_be[8]={
  {"w",S_1P|GWF_VERB|GWF_TENSE_PRESENT|GWF_SUBJUNCTIVE},
        {"Es",S_2P|GWF_VERB|GWF_TENSE_PRESENT|GWF_SUBJUNCTIVE},
        {"E",S_3P|GWF_VERB|GWF_TENSE_PRESENT|GWF_SUBJUNCTIVE},
        {"wmen",PL_1P|GWF_VERB|GWF_TENSE_PRESENT|GWF_SUBJUNCTIVE},
        {"Ete",PL_2P|GWF_VERB|GWF_TENSE_PRESENT|GWF_SUBJUNCTIVE},
        {"wsi",PL_3P|GWF_VERB|GWF_TENSE_PRESENT|GWF_SUBJUNCTIVE},
        {"wsin",PL_3P|GWF_VERB|GWF_TENSE_PRESENT|GWF_SUBJUNCTIVE},
        {NULL,0},
        };
  CGreekWord was[8]={
  {"E",S_1P|GWF_VERB|GWF_TENSE_PAST},
        {"En",S_1P|GWF_VERB|GWF_TENSE_PAST},
        {"Estha",S_2P|GWF_VERB|GWF_TENSE_PAST},
        {"En",S_3P|GWF_VERB|GWF_TENSE_PAST},
        {"Emen",PL_1P|GWF_VERB|GWF_TENSE_PAST},
        {"Ete",PL_2P|GWF_VERB|GWF_TENSE_PAST},
        {"Esan",PL_3P|GWF_VERB|GWF_TENSE_PAST},
        {NULL,0},
        };
  CGreekWord pronouns0[25]={
        {"autos",GWF_PRONOUN|SNM},
        {"autE",GWF_PRONOUN|SNF},
        {"auto",GWF_PRONOUN|SNN},

        {"autov",GWF_PRONOUN|SAM},
        {"autEn",GWF_PRONOUN|SAF},
        {"auto",GWF_PRONOUN|SAN},

        {"autou",GWF_PRONOUN|SGM},
        {"autou",GWF_PRONOUN|SGF},
        {"autEs",GWF_PRONOUN|SGN},

        {"autwi",GWF_PRONOUN|SDM},
        {"autEi",GWF_PRONOUN|SDF},
        {"autwi",GWF_PRONOUN|SDN},

        {"autoi",GWF_PRONOUN|PNM},
        {"autai",GWF_PRONOUN|PNF},
        {"auta",GWF_PRONOUN|PNN},

        {"autous",GWF_PRONOUN|PAM},
        {"autas",GWF_PRONOUN|PAF},
        {"auta",GWF_PRONOUN|PAN},

        {"autwn",GWF_PRONOUN|PGM},
        {"autwn",GWF_PRONOUN|PGF},
        {"autwn",GWF_PRONOUN|PGN},

        {"autois",GWF_PRONOUN|PDM},
        {"autais",GWF_PRONOUN|PDF},
        {"autois",GWF_PRONOUN|PDN},
        {NULL,0},
        };
  CGreekWord pronouns1[25]={
  {"ohutos",GWF_PRONOUN|SNM},
        {"ahutE",GWF_PRONOUN|SNF},
        {"touto",GWF_PRONOUN|SNN},

        {"touton",GWF_PRONOUN|SAM},
        {"tautEn",GWF_PRONOUN|SAF},
        {"touto",GWF_PRONOUN|SAN},

        {"toutou",GWF_PRONOUN|SGM},
        {"tautou",GWF_PRONOUN|SGF},
        {"toutou",GWF_PRONOUN|SGN},

        {"toutwi",GWF_PRONOUN|SDM},
        {"toutEi",GWF_PRONOUN|SDF},
        {"toutwi",GWF_PRONOUN|SDN},

        {"ohutoi",GWF_PRONOUN|PNM},
        {"ahutai",GWF_PRONOUN|PNF},
        {"tauta",GWF_PRONOUN|PNN},

        {"toutous",GWF_PRONOUN|PAM},
        {"tautas",GWF_PRONOUN|PAF},
        {"tauta",GWF_PRONOUN|PAN},

        {"toutwn",GWF_PRONOUN|PGM},
        {"toutwn",GWF_PRONOUN|PGF},
        {"toutwn",GWF_PRONOUN|PGN},

        {"toutois",GWF_PRONOUN|PDM},
        {"tautais",GWF_PRONOUN|PDF},
        {"toutois",GWF_PRONOUN|PDN},
        {NULL,0},
        };
  CGreekWord pronouns2[38-19+1]={
    {"egw",GWF_PRONOUN|GWF_NOMITIVE|S_1P},
    {"su",GWF_PRONOUN|GWF_NOMITIVE|S_2P},
    {"hEmeis",GWF_PRONOUN|GWF_NOMITIVE|PL_1P},
    {"humeis",GWF_PRONOUN|GWF_NOMITIVE|PL_2P},
    {"eme",GWF_PRONOUN|GWF_ACCUSATIVE|S_1P},
    {"me",GWF_PRONOUN|GWF_ACCUSATIVE|S_1P},
    {"se",GWF_PRONOUN|GWF_ACCUSATIVE|S_2P},
    {"hEmas",GWF_PRONOUN|GWF_ACCUSATIVE|PL_1P},
    {"humas",GWF_PRONOUN|GWF_ACCUSATIVE|PL_2P},
    {"emou",GWF_PRONOUN|GWF_GENITIVE|S_1P},
    {"mou",GWF_PRONOUN|GWF_GENITIVE|S_1P},
    {"sou",GWF_PRONOUN|GWF_GENITIVE|S_2P},
    {"hEmwn",GWF_PRONOUN|GWF_GENITIVE|PL_1P},
    {"humwn",GWF_PRONOUN|GWF_GENITIVE|PL_2P},
    {"emoi",GWF_PRONOUN|GWF_DATIVE|S_1P},
    {"moi",GWF_PRONOUN|GWF_DATIVE|S_1P},
    {"soi",GWF_PRONOUN|GWF_DATIVE|S_2P},
    {"hEmiv",GWF_PRONOUN|GWF_DATIVE|PL_1P},
    {"humiv",GWF_PRONOUN|GWF_DATIVE|PL_2P},
    {NULL,0},
  };
//Reflexive pronouns
  CGreekWord pronouns3[44]={
    {"emautou",SGM|GWF_REFLEXIVE|GWF_1PERSON},
    {"emautEs",SGF|GWF_REFLEXIVE|GWF_1PERSON},
    {"hEmwn",PGM|PGF|GWF_REFLEXIVE|GWF_1PERSON},
    {"autwn",PGF|GWF_REFLEXIVE|GWF_1PERSON},
    {"emautwi",SDM|GWF_REFLEXIVE|GWF_1PERSON},
    {"emautEi",SDF|GWF_REFLEXIVE|GWF_1PERSON},
    {"hEmin",PDM|PDF|GWF_REFLEXIVE|GWF_1PERSON},
    {"autois",PDM|GWF_REFLEXIVE|GWF_1PERSON},
    {"autais",PDM|GWF_REFLEXIVE|GWF_1PERSON},
    {"emauton",SAM|GWF_REFLEXIVE|GWF_1PERSON},
    {"emautEn",SAF|GWF_REFLEXIVE|GWF_1PERSON},
    {"hEmas",PAM|PAF|GWF_REFLEXIVE|GWF_1PERSON},
    {"autous",PAM|GWF_REFLEXIVE|GWF_1PERSON},
    {"autas",PAF|GWF_REFLEXIVE|GWF_1PERSON},
    {"seautou",SGM|GWF_REFLEXIVE|GWF_2PERSON},
    {"seautEs",SGF|GWF_REFLEXIVE|GWF_2PERSON},
    {"hEmwn",PGM|PGF|GWF_REFLEXIVE|GWF_2PERSON},
    {"autwn",PGF|GWF_REFLEXIVE|GWF_2PERSON},
    {"seautwi",SDM|GWF_REFLEXIVE|GWF_2PERSON},
    {"seautEi",SDF|GWF_REFLEXIVE|GWF_2PERSON},
    {"hEmin",PDM|PDF|GWF_REFLEXIVE|GWF_2PERSON},
    {"autois",PDM|GWF_REFLEXIVE|GWF_2PERSON},
    {"autais",PDM|GWF_REFLEXIVE|GWF_2PERSON},
    {"seauton",SAM|GWF_REFLEXIVE|GWF_2PERSON},
    {"seautEn",SAF|GWF_REFLEXIVE|GWF_2PERSON},
    {"hEmas",PAM|PAF|GWF_REFLEXIVE|GWF_2PERSON},
    {"autous",PAM|GWF_REFLEXIVE|GWF_2PERSON},
    {"autas",PAF|GWF_REFLEXIVE|GWF_2PERSON},
    {"eautou",SGM|GWF_REFLEXIVE|GWF_3PERSON},
    {"eautEs",SGF|GWF_REFLEXIVE|GWF_3PERSON},
    {"eautou",SGN|GWF_REFLEXIVE|GWF_3PERSON},
    {"eautwn",GWF_PLURAL|GWF_GENITIVE|GWF_REFLEXIVE|GWF_3PERSON},
    {"eautwi",SDM|GWF_REFLEXIVE|GWF_3PERSON},
    {"eautEi",SDF|GWF_REFLEXIVE|GWF_3PERSON},
    {"eautwi",SDN|GWF_REFLEXIVE|GWF_3PERSON},
    {"eautois",PDM|GWF_REFLEXIVE|GWF_3PERSON},
    {"eautais",PDF|GWF_REFLEXIVE|GWF_3PERSON},
    {"eautois",PDN|GWF_REFLEXIVE|GWF_3PERSON},
    {"eauton",SAM|GWF_REFLEXIVE|GWF_3PERSON},
    {"eautEn",SAF|GWF_REFLEXIVE|GWF_3PERSON},
    {"eauto",SAN|GWF_REFLEXIVE|GWF_3PERSON},
    {"eautous",PAM|GWF_REFLEXIVE|GWF_3PERSON},
    {"eautas",PAF|GWF_REFLEXIVE|GWF_3PERSON},
    {"eauta",PAN|GWF_REFLEXIVE|GWF_3PERSON},
  };
//When combined with dative/gentive/accusative,it tells how they go togheter
 CGreekWord prepositions[34]={
   {"eis",GWF_PREPOSITION|GWF_ACCUSATIVE}, //into
   {"es",GWF_PREPOSITION|GWF_ACCUSATIVE}, //into
   {"en",GWF_PREPOSITION|GWF_DATIVE}, //in
   {"ek",GWF_PREPOSITION|GWF_GENITIVE}, //out of
   {"ex",GWF_PREPOSITION|GWF_GENITIVE}, //out of
   {"amphi",GWF_PREPOSITION|GWF_ACCUSATIVE}, //around
   {"ana",GWF_PREPOSITION|GWF_ACCUSATIVE}, //through
   {"epi",GWF_PREPOSITION|GWF_ACCUSATIVE}, //against
   {"kata",GWF_PREPOSITION|GWF_ACCUSATIVE}, //down
   {"meta",GWF_PREPOSITION|GWF_ACCUSATIVE}, //after
   {"para",GWF_PREPOSITION|GWF_ACCUSATIVE}, //beside
   {"peri",GWF_PREPOSITION|GWF_ACCUSATIVE}, //near
   {"pros",GWF_PREPOSITION|GWF_ACCUSATIVE}, //towards
   {"humer",GWF_PREPOSITION|GWF_ACCUSATIVE}, //above
   {"hupo",GWF_PREPOSITION|GWF_ACCUSATIVE}, //under

   {"amphi",GWF_PREPOSITION|GWF_DATIVE}, //around,near
   {"ana",GWF_PREPOSITION|GWF_DATIVE}, //upon
   {"epi",GWF_PREPOSITION|GWF_DATIVE}, //because of
   {"para",GWF_PREPOSITION|GWF_DATIVE}, //with,near
   {"peri",GWF_PREPOSITION|GWF_DATIVE}, //near
   {"pros",GWF_PREPOSITION|GWF_DATIVE}, //in addition to
   {"sun",GWF_PREPOSITION|GWF_DATIVE}, //with the help of
   {"hupo",GWF_PREPOSITION|GWF_DATIVE}, //under
   
   {"amphi",GWF_PREPOSITION|GWF_GENITIVE}, //for the sake of
   {"anti",GWF_PREPOSITION|GWF_GENITIVE}, //opposite
   {"apu",GWF_PREPOSITION|GWF_GENITIVE}, //from
   {"dia",GWF_PREPOSITION|GWF_GENITIVE}, //through
   {"kata",GWF_PREPOSITION|GWF_GENITIVE}, //agianst
   {"meta",GWF_PREPOSITION|GWF_GENITIVE}, //with
   {"peri",GWF_PREPOSITION|GWF_GENITIVE}, //about
   {"pro",GWF_PREPOSITION|GWF_GENITIVE}, //pro
   {"pros",GWF_PREPOSITION|GWF_GENITIVE}, //toward
   {"humer",GWF_PREPOSITION|GWF_GENITIVE}, //on behalf of
   {"hupo",GWF_PREPOSITION|GWF_GENITIVE}, //by
 };
CGreekWord conjunctions[24]={
  {"kai",GWF_CONJUNCTION|GWF_POST_POSITIVE}, //and
  {"te",GWF_CONJUNCTION|GWF_POST_POSITIVE}, //te
  {"de",GWF_CONJUNCTION|GWF_POST_POSITIVE}, //but
  {"men",GWF_CONJUNCTION|GWF_POST_POSITIVE}, //you do...,but(de)
  {"oute",GWF_CONJUNCTION|GWF_CONJ_BEFORE} //and not
  {"mEte",GWF_CONJUNCTION|GWF_CONJ_BEFORE} //and not
  {"alla",GWF_CONJUNCTION|GWF_CONJ_BEFORE} //but
  {"de",GWF_CONJUNCTION|GWF_CONJ_BEFORE}, //but
  {"men",GWF_CONJUNCTION|GWF_CONJ_BEFORE}, //you do...,but(de)
  {"eite",GWF_CONJUNCTION|GWF_CONJ_BEFORE}, //either or
  {"gar",GWF_CONJUNCTION|GWF_CONJ_BEFORE},//because
  {"oti",GWF_CONJUNCTION|GWF_CONJ_BEFORE},//because
  {"ara",GWF_CONJUNCTION|GWF_CONJ_BEFORE},//therefore
  {"dio",GWF_CONJUNCTION|GWF_CONJ_BEFORE},//therefore
  {"ouv",GWF_CONJUNCTION|GWF_POST_POSITIVE},//therefore
  {"toinun",GWF_CONJUNCTION|GWF_POST_POSITIVE},//therefore
  {"ei",GWF_CONJUNCTION|GWF_CONJ_BEFORE},//if
  {"eiper",GWF_CONJUNCTION|GWF_CONJ_BEFORE},//if
  {"eos",GWF_CONJUNCTION|GWF_CONJ_BEFORE},//while
  {"ina",GWF_CONJUNCTION|GWF_CONJ_BEFORE},//where
  {"mechri",GWF_CONJUNCTION|GWF_CONJ_BEFORE},//until
  {"othen",GWF_CONJUNCTION|GWF_CONJ_BEFORE},//from where
  {"ote",GWF_CONJUNCTION|GWF_CONJ_BEFORE},//when
  {"hws",GWF_CONJUNCTION|GWF_CONJ_BEFORE},//as
};
CGreekWord adverbs[4]={
  {"ou",GWF_ADVERB},
  {"ouk",GWF_ADVERB},
  {"ouch",GWF_ADVERB},
  {"mE",GWF_ADVERB},
};
CGreekWord pronouns4[24]={
  {"ode",GWF_PRONOUN|SNM}, //this/these
  {"Ede",GWF_PRONOUN|SNF},
  {"tode",GWF_PRONOUN|SNN},
  {"toude",GWF_PRONOUN|SGM},
  {"tEsde",GWF_PRONOUN|SGF},
  {"toude",GWF_PRONOUN|SGN},
  {"twide",GWF_PRONOUN|SDM},
  {"tEide",GWF_PRONOUN|SDF},
  {"twide",GWF_PRONOUN|SDN},
  {"tonde",GWF_PRONOUN|SAM},
  {"tEnde",GWF_PRONOUN|SAF},
  {"tode",GWF_PRONOUN|SAN},
//
  {"oide",GWF_PRONOUN|PNM},
  {"aide",GWF_PRONOUN|PNF},
  {"tade",GWF_PRONOUN|PNN},
  {"twnde",GWF_PRONOUN|PGM},
  {"twnde",GWF_PRONOUN|PGF},
  {"twnde",GWF_PRONOUN|PGN},
  {"toisde",GWF_PRONOUN|PDM},
  {"taisde",GWF_PRONOUN|PDF},
  {"toisde",GWF_PRONOUN|PDN},
  {"tousde",GWF_PRONOUN|PAM},
  {"tasde",GWF_PRONOUN|PAF},
  {"tade",GWF_PRONOUN|PAN},
};

//This kind of
CGreekWord pronouns5[24]={
        {"toioutos",GWF_PRONOUN|SNM},
        {"toiautE",GWF_PRONOUN|SNF},
        {"toiouto",GWF_PRONOUN|SNN},

        {"toioutov",GWF_PRONOUN|SAM},
        {"toioautEn",GWF_PRONOUN|SAF},
        {"toiouto",GWF_PRONOUN|SAN},

        {"toioutou",GWF_PRONOUN|SGM},
        {"toioautEs",GWF_PRONOUN|SGF},
        {"toioutou",GWF_PRONOUN|SGN},

        {"toioutwi",GWF_PRONOUN|SDM},
        {"toiautEi",GWF_PRONOUN|SDF},
        {"toioutwi",GWF_PRONOUN|SDN},

        {"toioutoi",GWF_PRONOUN|PNM},
        {"toiautoi",GWF_PRONOUN|PNF},
        {"toiauta",GWF_PRONOUN|PNN},

        {"toiutous",GWF_PRONOUN|PAM},
        {"toiautas",GWF_PRONOUN|PAF},
        {"toiauta",GWF_PRONOUN|PAN},

        {"toioutwn",GWF_PRONOUN|PGM},
        {"toioutwn",GWF_PRONOUN|PGF},
        {"toioutwn",GWF_PRONOUN|PGN},

        {"toioutois",GWF_PRONOUN|PDM},
        {"toiautais",GWF_PRONOUN|PDF},
        {"toioutois",GWF_PRONOUN|PDN},
        };
//large,great,epic
CGreekWord pronouns6[24]={
        {"tosoutos",GWF_PRONOUN|SNM},
        {"tosautE",GWF_PRONOUN|SNF},
        {"tosouto",GWF_PRONOUN|SNN},

        {"tosoutov",GWF_PRONOUN|SAM},
        {"tosautEn",GWF_PRONOUN|SAF},
        {"tosouto",GWF_PRONOUN|SAN},

        {"tosoutou",GWF_PRONOUN|SGM},
        {"tosautEs",GWF_PRONOUN|SGF},
        {"tosoutou",GWF_PRONOUN|SGN},

        {"tosoutwi",GWF_PRONOUN|SDM},
        {"tosautEi",GWF_PRONOUN|SDF},
        {"tosoutwi",GWF_PRONOUN|SDN},

        {"tosoutoi",GWF_PRONOUN|PNM},
        {"tosautoi",GWF_PRONOUN|PNF},
        {"tosauta",GWF_PRONOUN|PNN},

        {"tosoutous",GWF_PRONOUN|PAM},
        {"tosautas",GWF_PRONOUN|PAF},
        {"tosauta",GWF_PRONOUN|PAN},

        {"tosoutwn",GWF_PRONOUN|PGM},
        {"tosoutwn",GWF_PRONOUN|PGF},
        {"tosoutwn",GWF_PRONOUN|PGN},

        {"tosoutois",GWF_PRONOUN|PDM},
        {"tosautais",GWF_PRONOUN|PDF},
        {"tosoutois",GWF_PRONOUN|PDN},
        };
//all,every,whole
//https://pressbooks.pub/ancientgreek/chapter/26/
//if precedes noun,"the whole thing"
//if precdes article,"all the thing"
//no article,"every thing"
CGreekWord pronouns7[24]={
        {"pas",GWF_PRONOUN|SNM},
        {"pasa",GWF_PRONOUN|SNF},
        {"pan",GWF_PRONOUN|SNN},

        {"panta",GWF_PRONOUN|SAM},
        {"pasan",GWF_PRONOUN|SAF},
        {"pan",GWF_PRONOUN|SAN},

        {"pantos",GWF_PRONOUN|SGM},
        {"pasEs",GWF_PRONOUN|SGF},
        {"pantos",GWF_PRONOUN|SGN},

        {"panti",GWF_PRONOUN|SDM},
        {"pansEi",GWF_PRONOUN|SDF},
        {"panti",GWF_PRONOUN|SDN},

        {"pantes",GWF_PRONOUN|PNM},
        {"pasai",GWF_PRONOUN|PNF},
        {"panta",GWF_PRONOUN|PNN},

        {"pantas",GWF_PRONOUN|PAM},
        {"pasas",GWF_PRONOUN|PAF},
        {"panta",GWF_PRONOUN|PAN},

        {"pantwn",GWF_PRONOUN|PGM},
        {"pantwn",GWF_PRONOUN|PGF},
        {"pantwn",GWF_PRONOUN|PGN},

        {"pasi",GWF_PRONOUN|PDM},
        {"pasais",GWF_PRONOUN|PDF},
        {"pasi",GWF_PRONOUN|PDN},
        };
//All togheter
CGreekWord pronouns8[24]={
        {"apas",GWF_PRONOUN|SNM},
        {"apasa",GWF_PRONOUN|SNF},
        {"apan",GWF_PRONOUN|SNN},

        {"apanta",GWF_PRONOUN|SAM},
        {"apasan",GWF_PRONOUN|SAF},
        {"apan",GWF_PRONOUN|SAN},

        {"apantos",GWF_PRONOUN|SGM},
        {"apasEs",GWF_PRONOUN|SGF},
        {"apantos",GWF_PRONOUN|SGN},

        {"apanti",GWF_PRONOUN|SDM},
        {"apansEi",GWF_PRONOUN|SDF},
        {"apanti",GWF_PRONOUN|SDN},

        {"apantes",GWF_PRONOUN|PNM},
        {"apasai",GWF_PRONOUN|PNF},
        {"apanta",GWF_PRONOUN|PNN},

        {"apantas",GWF_PRONOUN|PAM},
        {"apasas",GWF_PRONOUN|PAF},
        {"apanta",GWF_PRONOUN|PAN},

        {"apantwn",GWF_PRONOUN|PGM},
        {"apantwn",GWF_PRONOUN|PGF},
        {"apantwn",GWF_PRONOUN|PGN},

        {"apasi",GWF_PRONOUN|PDM},
        {"apasais",GWF_PRONOUN|PDF},
        {"apasi",GWF_PRONOUN|PDN},
        };
//One
CGreekWord pronouns9[12]={
  {"eis",SNM|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mia",SNF|GWF_ADJECTIVE|GWF_PRONOUN},
  {"hen",SNN|GWF_ADJECTIVE|GWF_PRONOUN},
  {"henos",SGM|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mias",SGF|GWF_ADJECTIVE|GWF_PRONOUN},
  {"henos",SGN|GWF_ADJECTIVE|GWF_PRONOUN},
  {"heni",SDM|GWF_ADJECTIVE|GWF_PRONOUN},
  {"miai",SDF|GWF_ADJECTIVE|GWF_PRONOUN},
  {"heni",SDN|GWF_ADJECTIVE|GWF_PRONOUN},
  {"hena",SAM|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mian",SAF|GWF_ADJECTIVE|GWF_PRONOUN},
  {"hen",SAN|GWF_ADJECTIVE|GWF_PRONOUN},
};
//nothing
CGreekWord pronouns10[24]={
  {"oudeis",SNM|GWF_ADJECTIVE|GWF_PRONOUN},
  {"oudemia",SNF|GWF_ADJECTIVE|GWF_PRONOUN},
  {"oudhen",SNN|GWF_ADJECTIVE|GWF_PRONOUN},
  {"oudeenos",SGM|GWF_ADJECTIVE|GWF_PRONOUN},
  {"oudemias",SGF|GWF_ADJECTIVE|GWF_PRONOUN},
  {"oudenos",SGN|GWF_ADJECTIVE|GWF_PRONOUN},
  {"oudeni",SDM|GWF_ADJECTIVE|GWF_PRONOUN},
  {"oudemiai",SDF|GWF_ADJECTIVE|GWF_PRONOUN},
  {"oudeni",SDN|GWF_ADJECTIVE|GWF_PRONOUN},
  {"oudena",SAM|GWF_ADJECTIVE|GWF_PRONOUN},
  {"oudemian",SAF|GWF_ADJECTIVE|GWF_PRONOUN},
  {"ouden",SAN|GWF_ADJECTIVE|GWF_PRONOUN}, 

  {"mEdeis",SNM|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mEdemia",SNF|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mEdhen",SNN|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mEdeenos",SGM|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mEdemias",SGF|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mEdenos",SGN|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mEdeni",SDM|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mEdemiai",SDF|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mEdeni",SDN|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mEdena",SAM|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mEdemian",SAF|GWF_ADJECTIVE|GWF_PRONOUN},
  {"mEden",SAN|GWF_ADJECTIVE|GWF_PRONOUN}, 

};
CGreekWord ints1[24]={
  {"tis",GWF_INTEROGITIVE|SNM},
  {"tis",GWF_INTEROGITIVE|SNF},
  {"ti",GWF_INTEROGITIVE|SNN},
  {"tinos",GWF_INTEROGITIVE|SGM},
  {"tinos",GWF_INTEROGITIVE|SGF},
  {"tinos",GWF_INTEROGITIVE|SGN},
  {"tini",GWF_INTEROGITIVE|SDM},
  {"tini",GWF_INTEROGITIVE|SDF},
  {"tini",GWF_INTEROGITIVE|SDN},
  {"tina",GWF_INTEROGITIVE|SAM},
  {"tina",GWF_INTEROGITIVE|SAF},
  {"ti",GWF_INTEROGITIVE|SAN},
  //Plural
  {"tines",GWF_INTEROGITIVE|PNM},
  {"tines",GWF_INTEROGITIVE|PNF},
  {"tina",GWF_INTEROGITIVE|PNN},
  {"tinwn",GWF_INTEROGITIVE|PGM},
  {"tinwn",GWF_INTEROGITIVE|PGF},
  {"tina",GWF_INTEROGITIVE|PGN},
  {"tisi",GWF_INTEROGITIVE|PDM},
  {"tisi",GWF_INTEROGITIVE|PDF},
  {"tisi",GWF_INTEROGITIVE|PDN},
  {"tinas",GWF_INTEROGITIVE|PAM},
  {"tinas",GWF_INTEROGITIVE|PAF},
  {"tina",GWF_INTEROGITIVE|PAN},
};
CGreekWord say[10]={
  {"phEmi",S_1P|GWF_VERB|GWF_TENSE_PRESENT},
        {"phEis",S_2P|GWF_VERB|GWF_TENSE_PRESENT},
        {"phEsi",S_3P|GWF_VERB|GWF_TENSE_PRESENT},
        {"phEsiv",S_3P|GWF_VERB|GWF_TENSE_PRESENT},
        {"phamen",PL_1P|GWF_VERB|GWF_TENSE_PRESENT},
        {"phate",PL_2P|GWF_VERB|GWF_TENSE_PRESENT},
        {"phasi",PL_3P|GWF_VERB|GWF_TENSE_PRESENT},
        {"phasin",PL_3P|GWF_VERB|GWF_TENSE_PRESENT},
        {"phanai",GWF_INFINITIVE|GWF_TENSE_PRESENT},
        {NULL,0},
        };
CGreekWord reflexive[5]={
  {"allElwn",GWF_GENITIVE|GWF_RECIPRICAL|GWF_PLURAL},
  {"allElois",GWF_DATIVE|GWF_RECIPRICAL|GWF_PLURAL},
  {"allElais",GWF_DATIVE|GWF_RECIPRICAL|GWF_PLURAL},
  {"allElous",GWF_ACCUSATIVE|GWF_RECIPRICAL|GWF_PLURAL},
  {"allElas",GWF_ACCUSATIVE|GWF_RECIPRICAL|GWF_PLURAL},
};
U0 GenerateVocab() {
//Defintite article
  AddWords(articles,sizeof(articles)/sizeof(CGreekWord));
  AddWords(am,sizeof(am)/sizeof(CGreekWord));
  AddWords(may_be,sizeof(may_be)/sizeof(CGreekWord));
  AddWords(was,sizeof(was)/sizeof(CGreekWord));
  AddWords(say,sizeof(say)/sizeof(CGreekWord));
  AddWords(adverbs,sizeof(adverbs)/sizeof(CGreekWord));
//Pronouns,he
  AddWords(pronouns0,sizeof(pronouns0)/sizeof(CGreekWord));
  AddWords(pronouns1,sizeof(pronouns1)/sizeof(CGreekWord));
  AddWords(pronouns2,sizeof(pronouns2)/sizeof(CGreekWord));
  AddWords(pronouns3,sizeof(pronouns3)/sizeof(CGreekWord));
  AddWords(pronouns4,sizeof(pronouns4)/sizeof(CGreekWord));
  AddWords(pronouns5,sizeof(pronouns5)/sizeof(CGreekWord));
  AddWords(pronouns6,sizeof(pronouns6)/sizeof(CGreekWord));
  AddWords(pronouns7,sizeof(pronouns7)/sizeof(CGreekWord));
  AddWords(pronouns8,sizeof(pronouns8)/sizeof(CGreekWord));
  AddWords(pronouns9,sizeof(pronouns9)/sizeof(CGreekWord));
  AddWords(pronouns10,sizeof(pronouns10)/sizeof(CGreekWord));
  AddWords(reflexive,sizeof(reflexive)/sizeof(CGreekWord));
  AddTemplatedWord("ekienos");
//INTEROGITIVES
  AddWords(ints1,sizeof(ints1)/sizeof(CGreekWord));
//CONJUNCTIONS
  AddWords(conjunctions,sizeof(conjunctions)/sizeof(CGreekWord));
//PREPOSITIONS
  AddWords(prepositions,sizeof(prepositions)/sizeof(CGreekWord));
//https://ancientgreek.pressbooks.com/chapter/4/
  AddTemplatedWord("deiknumi",GWF_VERB); //show
  AddTemplatedWord("mignumi",GWF_VERB); //mix
  AddTemplatedWord("apollumi",GWF_VERB); //destroy
  AddTemplatedWord("omnumi",GWF_VERB); //swear an oath
//https://ancientgreek.pressbooks.com/chapter/6/
  AddTemplatedWord("didwmi",GWF_VERB); //give
  AddTemplatedWord("tethEmi",GWF_VERB); //make
  AddTemplatedWord("istEmi",GWF_VERB); //stand
  AddTemplatedWord("iEmi",GWF_VERB); //throw
  AddTemplatedWord("anestEmi",GWF_VERB); //anointed
  AddTemplatedWord("apodidwmai",GWF_VERB); //give back
  AddTemplatedWord("aphiEmi",GWF_VERB); //sendout
  AddTemplatedWord("epitithEmi",GWF_VERB); //put on
  AddTemplatedWord("kathistEmi",GWF_VERB); //establish
  AddTemplatedWord("paradidwmi",GWF_VERB); //deliver
  AddTemplatedWord("paraistEmi",GWF_VERB); //present
//https://ancientgreek.pressbooks.com/chapter/7/
  AddTemplatedWord("agwnos",GWF_NOUN); //context
  AddTemplatedWord("aiwnos",GWF_NOUN); //eterinity
  AddTemplatedWord("archontos",GWF_NOUN); //ruler
  AddTemplatedWord("daimonos",GWF_NOUN); //Deity
  AddTemplatedWord("hEgemonos",GWF_NOUN); //guide
  AddTemplatedWord("paidos",GWF_NOUN); //child
  AddTemplatedWord("podos",GWF_NOUN); //foot
//https://ancientgreek.pressbooks.com/chapter/9/
  AddTemplatedWord("onomatos",GWF_NOUN); //name
  AddTemplatedWord("swmatos",GWF_NOUN); //body
  AddTemplatedWord("muriados",GWF_NOUN); //countless
  AddTemplatedWord("vuktos",GWF_NOUN); //night
  AddTemplatedWord("aimatos",GWF_NOUN); //blood
  AddTemplatedWord("grammatos",GWF_NOUN); //letter
  AddTemplatedWord("thelEmatos",GWF_NOUN); //wish
  AddTemplatedWord("rEmatos",GWF_NOUN); //word(saying)
  AddTemplatedWord("spermatos",GWF_NOUN); //seed
  AddTemplatedWord("schEmatos",GWF_NOUN); //from,appearance
  AddTemplatedWord("udatos",GWF_NOUN); //water
  AddTemplatedWord("huparchontwn",GWF_NOUN); //property
  AddTemplatedWord("phwtos",GWF_NOUN); //light
  AddTemplatedWord("chrEmatos",GWF_NOUN); //money$$$$$$
//https://ancientgreek.pressbooks.com/chapter/14/
  AddTemplatedWord("akouw",GWF_VERB); //hear
  AddTemplatedWord("bouleuw",GWF_VERB);//resolve
  AddTemplatedWord("thuw",GWF_VERB); //sacrifice
  AddTemplatedWord("keleuw",GWF_VERB); //order
  AddTemplatedWord("klaiw",GWF_VERB); //cry out
  AddTemplatedWord("kwluw",GWF_VERB); //prevent
  AddTemplatedWord("legw",GWF_VERB); //say
  AddTemplatedWord("luw",GWF_VERB); //destroy
  AddTemplatedWord("apoluw",GWF_VERB); //forgive
  AddTemplatedWord("pauww",GWF_VERB); //stop
  AddTemplatedWord("perisseuw",GWF_VERB); //increase
  AddTemplatedWord("pisteuw",GWF_VERB); //believe
  AddTemplatedWord("pereuw",GWF_VERB); //carry
  AddTemplatedWord("phuw",GWF_VERB); //grow
  AddTemplatedWord("amartanw",GWF_VERB); //fail  
  AddTemplatedWord("bainw",GWF_VERB); //come  
  AddTemplatedWord("elaunww",GWF_VERB); //set in motion
  AddTemplatedWord("lambanw",GWF_VERB); //take
  AddTemplatedWord("katalmabanw",GWF_VERB); //seize
  AddTemplatedWord("paralambanw",GWF_VERB); //accept
  AddTemplatedWord("hupolambanw",GWF_VERB); //take up/reply
  AddTemplatedWord("lanthanw",GWF_VERB); //escape notice of
  AddTemplatedWord("mathanw",GWF_VERB); //learn
  AddTemplatedWord("pinw",GWF_VERB); //drink
  AddTemplatedWord("temnw",GWF_VERB); //cut
  AddTemplatedWord("tugchanw",GWF_VERB); //greet
  AddTemplatedWord("thnEiskw",GWF_VERB); //die
  AddTemplatedWord("apothnEiskw",GWF_VERB); //die
  AddTemplatedWord("gignwskw",GWF_VERB); //know
  AddTemplatedWord("anagignwskw",GWF_VERB); //read
  AddTemplatedWord("didaskw",GWF_VERB); //teach
  AddTemplatedWord("euriskw",GWF_VERB); //find
  AddTemplatedWord("mimnEskw",GWF_VERB); //remind
//https://ancientgreek.pressbooks.com/chapter/16/
  AddTemplatedWord("apsw",GWF_VERB); //will touch
  AddTemplatedWord("grapsw",GWF_VERB); //will write
  AddTemplatedWord("leipsw",GWF_VERB); //will leave
  AddTemplatedWord("pempsw",GWF_VERB); //will send
  AddTemplatedWord("trepsw",GWF_VERB); //will turn
  AddTemplatedWord("threpsw",GWF_VERB); //will nourish
  AddTemplatedWord("thaumazw",GWF_VERB); //will wonder
  AddTemplatedWord("kataskeuazw",GWF_VERB); //will supply
  AddTemplatedWord("onomazw",GWF_VERB); //will call by name
  AddTemplatedWord("peithw",GWF_VERB); //will persuade
  AddTemplatedWord("swizw",GWF_VERB); //will save
  AddTemplatedWord("phrazw",GWF_VERB); //will tell
  AddTemplatedWord("azw",GWF_VERB); //will lead
  AddTemplatedWord("humazw",GWF_VERB); //will go away
  AddTemplatedWord("phrazw",GWF_VERB); //will tell
  AddTemplatedWord("archzw",GWF_VERB); //will begin
  AddTemplatedWord("humarchw",GWF_VERB); //will exist
  AddTemplatedWord("didazw",GWF_VERB); //will teach
  AddTemplatedWord("diwzw",GWF_VERB); //will pursue
  AddTemplatedWord("Ezw",GWF_VERB); //will come
  AddTemplatedWord("lezw",GWF_VERB); //will say
  AddTemplatedWord("prazw",GWF_VERB); //will do
  AddTemplatedWord("tazw",GWF_VERB); //will arrange
  AddTemplatedWord("phulazw",GWF_VERB); //will defend
  //https://ancientgreek.pressbooks.com/chapter/17/  
  AddTemplatedWord("adikw",GWF_VERB); //commit injustice
  AddTemplatedWord("airw",GWF_VERB); //grab
  AddTemplatedWord("anairw",GWF_VERB); //raise up
  AddTemplatedWord("aphairw",GWF_VERB); //take away
  AddTemplatedWord("aitw",GWF_VERB); //beg
  AddTemplatedWord("boEthw",GWF_VERB); //help
  AddTemplatedWord("dw",GWF_VERB); //miss
  AddTemplatedWord("dokw",GWF_VERB); //suppose
  AddTemplatedWord("zEtw",GWF_VERB); //seek
  AddTemplatedWord("katEgerw",GWF_VERB); //accuse
  AddTemplatedWord("katoikw",GWF_VERB); //settle
  AddTemplatedWord("kinw",GWF_VERB); //move
  AddTemplatedWord("kratw",GWF_VERB); //rule over
  AddTemplatedWord("lalw",GWF_VERB); //talk
  AddTemplatedWord("misw",GWF_VERB); //hate
  AddTemplatedWord("oikw",GWF_VERB); //dwell
  AddTemplatedWord("oikodumw",GWF_VERB); //build
  AddTemplatedWord("homlogw",GWF_VERB); //agree
  AddTemplatedWord("plw",GWF_VERB); //sail
  AddTemplatedWord("poiw",GWF_VERB); //make
  AddTemplatedWord("skopw",GWF_VERB); //look at
  AddTemplatedWord("phbow",GWF_VERB); //frighten
  AddTemplatedWord("phronw",GWF_VERB); //think
  AddTemplatedWord("agapw",GWF_VERB); //love
  AddTemplatedWord("drw",GWF_VERB); //do
  AddTemplatedWord("ew",GWF_VERB); //allow
  AddTemplatedWord("erwtw",GWF_VERB); //ask
  AddTemplatedWord("nikw",GWF_VERB); //conquer
  AddTemplatedWord("hormw",GWF_VERB); //hurry
  AddTemplatedWord("peirw",GWF_VERB); //try
  AddTemplatedWord("teleutw",GWF_VERB); //finish
  AddTemplatedWord("timw",GWF_VERB); //honor
  AddTemplatedWord("tolmw",GWF_VERB); //dare
  AddTemplatedWord("axiw",GWF_VERB); //worthy
  AddTemplatedWord("dElw",GWF_VERB); //show
  AddTemplatedWord("phanerw",GWF_VERB); //reveal
  //https://ancientgreek.pressbooks.com/chapter/18/
  AddTemplatedWord("aggellw",GWF_VERB); //tell
  AddTemplatedWord("aggelw",GWF_VERB); //tell
  AddTemplatedWord("airw",GWF_VERB); //raise
  AddTemplatedWord("ballw",GWF_VERB); //throw
  AddTemplatedWord("balw",GWF_VERB); //throw
  AddTemplatedWord("diatherw",GWF_VERB); //destroy
  AddTemplatedWord("diatheirw",GWF_VERB); //destroy
  AddTemplatedWord("krinw",GWF_VERB);//judge
  AddTemplatedWord("phainw",GWF_VERB);//appear
  AddTemplatedWord("phanw",GWF_VERB);//appear
  AddTemplatedWord("komiw",GWF_VERB);//provide
  AddTemplatedWord("nomiw",GWF_VERB);//believe
  AddTemplatedWord("menw",GWF_VERB);//stop
//https://ancientgreek.pressbooks.com/chapter/19/
  AddTemplatedWord("angkEs",GWF_NOUN); //necessity
  AddTemplatedWord("aretE",GWF_NOUN); //goodness
  AddTemplatedWord("archE",GWF_NOUN); //begining
  AddTemplatedWord("bouls",GWF_NOUN); //plan advice
  AddTemplatedWord("yE",GWF_NOUN); //earth
  AddTemplatedWord("gnwmE",GWF_NOUN); //idea
  AddTemplatedWord("graphE",GWF_NOUN); //writing
  AddTemplatedWord("dikEs",GWF_NOUN); //justice
  AddTemplatedWord("eirEnEs",GWF_NOUN); //peace
  AddTemplatedWord("epistEmE",GWF_NOUN); //knowledge
  AddTemplatedWord("EdonEs",GWF_NOUN); //pleaseure
  AddTemplatedWord("kephalEs",GWF_NOUN); //head
  AddTemplatedWord("machEs",GWF_NOUN); //battle
  AddTemplatedWord("vikEs",GWF_NOUN); //victory
  AddTemplatedWord("orgEs",GWF_NOUN); //anger
  AddTemplatedWord("technEs",GWF_NOUN); //art
  AddTemplatedWord("timEs",GWF_NOUN); //honor
  AddTemplatedWord("trophEs",GWF_NOUN); //food
  AddTemplatedWord("tuchEs",GWF_NOUN); //fate
  AddTemplatedWord("phwnE",GWF_NOUN); //sound
  AddTemplatedWord("psuchEs",GWF_NOUN); //soul
  AddTemplatedWord("aitias",GWF_NOUN); //accusation
  AddTemplatedWord("diaphoras",GWF_NOUN); //differnece
  AddTemplatedWord("ekklEsiass",GWF_NOUN); //gathering
  AddTemplatedWord("humeras",GWF_NOUN); //day
  AddTemplatedWord("oikias",GWF_NOUN); //house
  AddTemplatedWord("ousias",GWF_NOUN); //essence
  AddTemplatedWord("sophias",GWF_NOUN); //wisdom
  AddTemplatedWord("stratia",GWF_NOUN); //army
  AddTemplatedWord("sumphora",GWF_NOUN); //disaster
  AddTemplatedWord("swtErias",GWF_NOUN); //saftey
  AddTemplatedWord("chwras",GWF_NOUN); //place
  AddTemplatedWord("alEtheias",GWF_NOUN); //truth
  AddTemplatedWord("grattEs",GWF_NOUN); //language
  AddTemplatedWord("doxEs",GWF_NOUN); //opinion
  AddTemplatedWord("thalattEs",GWF_NOUN); //sea
  AddTemplatedWord("despotou",GWF_NOUN); //master
  AddTemplatedWord("poiEtou",GWF_NOUN); //creator
//https://ancientgreek.pressbooks.com/chapter/20/
  AddTemplatedWord("adelphou",GWF_NOUN); //brother
  AddTemplatedWord("anthrwpos",GWF_NOUN); //human
  AddTemplatedWord("arithmou",GWF_NOUN); //number
  AddTemplatedWord("biou",GWF_NOUN); //life
  AddTemplatedWord("doulos",GWF_NOUN); //slave
  AddTemplatedWord("ippos",GWF_NOUN); //horse
  AddTemplatedWord("Elios",GWF_NOUN); //sun
  AddTemplatedWord("thanatos",GWF_NOUN); //death
  AddTemplatedWord("theos",GWF_NOUN); //God
  AddTemplatedWord("kindunos",GWF_NOUN); //danger
  AddTemplatedWord("kosmos",GWF_NOUN); //decoration
  AddTemplatedWord("kuklos",GWF_NOUN); //cirlce
  AddTemplatedWord("kurios",GWF_NOUN); //master
  AddTemplatedWord("lithos",GWF_NOUN); //stone
  AddTemplatedWord("nomos",GWF_NOUN); //tradition
  AddTemplatedWord("nous",GWF_NOUN); //mind
  AddTemplatedWord("xenos",GWF_NOUN); //foriegner
  AddTemplatedWord("ouranos",GWF_NOUN); //sky
  AddTemplatedWord("ophthalmos",GWF_NOUN); //eye
  AddTemplatedWord("polemioiwm",GWF_NOUN); //enemy
  AddTemplatedWord("polemos",GWF_NOUN); //war
  AddTemplatedWord("ponos",GWF_NOUN); //pain 
  AddTemplatedWord("potamos",GWF_NOUN); //river
  AddTemplatedWord("topos",GWF_NOUN); //topic
  AddTemplatedWord("tropos",GWF_NOUN); //way/turn
  AddTemplatedWord("uios",GWF_NOUN); //son
  AddTemplatedWord("phobos",GWF_NOUN); //fear
  AddTemplatedWord("chronos",GWF_NOUN); //time
  AddTemplatedWord("nEsos",GWF_NOUN); //island
  AddTemplatedWord("nosos",GWF_NOUN); //disease
  AddTemplatedWord("hodos",GWF_NOUN); //round
  AddTemplatedWord("argurion",GWF_NOUN); //money
  AddTemplatedWord("biblion",GWF_NOUN); //book
  AddTemplatedWord("ergon",GWF_NOUN); //energy
  AddTemplatedWord("zwion",GWF_NOUN); //living thing
  AddTemplatedWord("hieron",GWF_NOUN); //temple
  AddTemplatedWord("oplon",GWF_NOUN); //weapon
  AddTemplatedWord("proswpon",GWF_NOUN); //face
  AddTemplatedWord("sEmeion",GWF_NOUN); //sign
  AddTemplatedWord("teknon",GWF_NOUN); //child
  AddTemplatedWord("chwrion",GWF_NOUN); //zone
  //https://ancientgreek.pressbooks.com/chapter/21/
  AddTemplatedWord("aisthanomai",GWF_VERB); //percieve
  AddTemplatedWord("aisthEsomai",GWF_VERB); //percieve
  AddTemplatedWord("apokrinoumai",GWF_VERB); //awnser
  AddTemplatedWord("aphinxomai",GWF_VERB); //come to
  AddTemplatedWord("boulomai",GWF_VERB); //want
  AddTemplatedWord("gignomai",GWF_VERB); //be born
  AddTemplatedWord("dexomai",GWF_VERB); //welcome
  AddTemplatedWord("dunamai",GWF_VERB); //be able
  AddTemplatedWord("dunEsomai",GWF_VERB); //be able
  AddTemplatedWord("epomai",GWF_VERB); //follow
  AddTemplatedWord("ephomai",GWF_VERB); //follow
  AddTemplatedWord("ergasomai",GWF_VERB); //work
  AddTemplatedWord("hEgEsomai",GWF_VERB); //lead
  AddTemplatedWord("ktEsomai",GWF_VERB); //acquire
  AddTemplatedWord("ktaomai",GWF_VERB); //acquire
  AddTemplatedWord("machomai",GWF_VERB); //fight
  AddTemplatedWord("machoumai",GWF_VERB); //fight
  AddTemplatedWord("poreuomai",GWF_VERB); //march
  AddTemplatedWord("poreusomai",GWF_VERB); //march
  AddTemplatedWord("skopew",GWF_VERB); //exampine
  AddTemplatedWord("skepsomai",GWF_VERB); //exampine
  AddTemplatedWord("chraomai",GWF_VERB); //use
  AddTemplatedWord("chrEsomai",GWF_VERB); //use
  //https://ancientgreek.pressbooks.com/chapter/22/
  AddTemplatedWord("lEpsomai",GWF_VERB);//recieve
  AddTemplatedWord("esomai",GWF_VERB); //will be
  AddTemplatedWord("apothanoumai",GWF_VERB); //die
  AddTemplatedWord("apothnEiskw",GWF_VERB); //die
  AddTemplatedWord("bEsomai",GWF_VERB); //walk
  AddTemplatedWord("blepw",GWF_VERB); //see
  AddTemplatedWord("bEsomai",GWF_VERB); //see
  AddTemplatedWord("manthanw",GWF_VERB); //learn
  AddTemplatedWord("matheEsomai",GWF_VERB); //learn
  AddTemplatedWord("pheugw",GWF_VERB); //flee
  AddTemplatedWord("pheuxomai",GWF_VERB); //flee
  AddTemplatedWord("eleusomai",GWF_VERB); //come
  AddTemplatedWord("pheugw",GWF_VERB); //eat
  AddTemplatedWord("schEsw",GWF_VERB); //have
  AddTemplatedWord("exw",GWF_VERB); //have
  AddTemplatedWord("opsomai",GWF_VERB); //see
  AddTemplatedWord("peisomai",GWF_VERB); //suffer
  AddTemplatedWord("pesoumaw",GWF_VERB); //fall
  AddTemplatedWord("pleusoumai",GWF_VERB); //sail
  AddTemplatedWord("plew",GWF_VERB); //sail
  AddTemplatedWord("punthanomai",GWF_VERB); //learn
  AddTemplatedWord("peusomai",GWF_VERB); //learn
  AddTemplatedWord("tikw",GWF_VERB); //give birth
  AddTemplatedWord("texomai",GWF_VERB); //give birth
  AddTemplatedWord("oisw",GWF_VERB); //endure
  AddTemplatedWord("diapherw",GWF_VERB); //spread
  AddTemplatedWord("diosw",GWF_VERB); //spread
  AddTemplatedWord("sumpherw",GWF_VERB); //profit
  AddTemplatedWord("sunoisw",GWF_VERB); //profit
//https://pressbooks.pub/ancientgreek/chapter/23/
  AddTemplatedWord("andros",GWF_NOUN); //man
  AddTemplatedWord("thugatEr",GWF_NOUN); //daugher
  AddTemplatedWord("martus",GWF_NOUN); //witness
  AddTemplatedWord("patEr",GWF_NOUN); //pater
  AddTemplatedWord("purosr",GWF_NOUN); //fire
  AddTemplatedWord("cheiros",GWF_NOUN); //hand
  AddTemplatedWord("genous",GWF_NOUN); //clan
  AddTemplatedWord("ethnous",GWF_NOUN); //nation
  AddTemplatedWord("eidous",GWF_NOUN); //shape
  AddTemplatedWord("epous",GWF_NOUN); //word
  AddTemplatedWord("etous",GWF_NOUN); //year
  AddTemplatedWord("megethous",GWF_NOUN); //magnitude
  AddTemplatedWord("merous",GWF_NOUN); //part
  AddTemplatedWord("orous",GWF_NOUN); //hill
  AddTemplatedWord("pathous",GWF_NOUN); //emotion
  AddTemplatedWord("plEthous",GWF_NOUN); //crowd
  AddTemplatedWord("teichous",GWF_NOUN); //wall
  AddTemplatedWord("telous",GWF_NOUN); //end
//https://pressbooks.pub/ancientgreek/chapter/24/
  AddTemplatedWord("dunamews",GWF_NOUN); //power
  AddTemplatedWord("pistews",GWF_NOUN); //proof
  AddTemplatedWord("polews",GWF_NOUN); //city
  AddTemplatedWord("praxews",GWF_NOUN); //action
  AddTemplatedWord("taxews",GWF_NOUN); //order
  AddTemplatedWord("polews",GWF_NOUN); //nature
  AddTemplatedWord("phusews",GWF_NOUN); //city
  AddTemplatedWord("charitos",GWF_NOUN); //favor
  AddTemplatedWord("basilews",GWF_NOUN); //king
  AddTemplatedWord("boos",GWF_NOUN); //bull/cow
  AddTemplatedWord("hierews",GWF_NOUN); //preist
  AddTemplatedWord("hippews",GWF_NOUN); //horseman
  AddTemplatedWord("vews",GWF_NOUN); //ship
  AddTemplatedWord("presbews",GWF_NOUN); //old dude
  //Other 
  AddTemplatedWord("oikos",GWF_NOUN); //home
  AddTemplatedWord("monos",GWF_NOUN); //only
  //MARK
  AddTemplatedWord("archontes",GWF_NOUN); //begining
  AddTemplatedWord("euaggelion",GWF_NOUN); //good stuff
  AddTemplatedWord("graphw",GWF_VERB); //write
  AddTemplatedWord("prophEtEi",GWF_NOUN); //prophe
  AddTemplatedWord("apostellw",GWF_VERB); //go forth
  AddTemplatedWord("aggelon",GWF_NOUN); //angel
  AddTemplatedWord("proswpon",GWF_NOUN); //face
  AddTemplatedWord("kataskeuazw",GWF_VERB); //prepare
  AddTemplatedWord("hodos",GWF_NOUN); //path
  AddTemplatedWord("phwnE",GWF_NOUN); //sound
  AddTemplatedWord("boaw",GWF_VERB); //shout
  AddTemplatedWord("erEmwi",GWF_NOUN); //desert
  AddTemplatedWord("hetoimazw",GWF_VERB); //to preapre
  AddTemplatedWord("koriou",GWF_NOUN); //posssesor
  AddTemplatedWord("eutheias",GWF_ADJECTIVE); //straight
  AddTemplatedWord("poiete",GWF_VERB); //make
  AddTemplatedWord("tribous",GWF_NOUN); //way
  AddTemplatedWord("iEsou",GWF_NOUN); // Jesus
  AddTemplatedWord("chritou",GWF_NOUN); // Christ
  AddTemplatedWord("theon",GWF_NOUN); // God
  AddTemplatedWord("uiou",GWF_NOUN); // Son
  AddTemplatedWord("gignomai",GWF_VERB); //To begin
  AddTemplatedWord("baptizwn",GWF_VERB); //Baptize
  AddTemplatedWord("kErusswn",GWF_VERB); //To proclaim
  AddTemplatedWord("baptisma",GWF_NOUN); //Imesion
  AddTemplatedWord("metanoia",GWF_NOUN); //Achange of mind
  AddTemplatedWord("apsesis",GWF_NOUN); //Forgiveness
  AddTemplatedWord("hamartia",GWF_NOUN); //sin
}
GenerateVocab;
SaveTrie("VOCAB.DICT");
LoadTrie("VOCAB.DICT");
extern class CCYKGrammarEnt;
class CCYKToken:CQue {
  #define CYK_HEAD 0
  $ER$$ER$#define CYK_WORD 1
  #define CYK_COMMA 2
  #define CYK_COLON 3
  #define CYK_PERIOD 4
  #define CYK_QUESTION_MARK 5
  #define CYK_UNKNOWN 6
  #define CYK_RULE 7
  I64 type;
  CGTrie *trie;
  CCYKGrammarEnt *rule;
};
U8 *SkipWhitespace(U8 *str) {
  while(*str&&Bt(char_bmp_white_space,*str)) str++;
  return str;
}
CCYKToken *TokenizeString(U8 *str) {
  CCYKToken *ret=CAlloc(sizeof(CCYKToken)),*tmp;
  CGTrie *t;
  U8 buf[STR_LEN];
  I64 i;
  QueInit(ret);
  str=SkipWhitespace(str);
  for(;*str;str=SkipWhitespace(str)) {
    tmp=CAlloc(sizeof(CCYKToken));
    QueInit(tmp);
    i=0;
loop:;
    switch(*str) {
      case 'a'...'z':
      case 'A'...'Z':
        buf[i++]=*str++;
        goto loop;
      case '?':
        if(i) break;
      case '.':
        if(i) break;
      case ':':
        if(i) break;
    }
    if(!i) {
      switch(*str) {
        case '?':
	  tmp->type=CYK_QUESTION_MARK;
	  break;
        case '.':
	  tmp->type=CYK_PERIOD;
	  break;
        case ':':
	  tmp->type=CYK_COLON;
	  break;
        default:
	  throw('invChar');
      }
      QueIns(tmp,ret->last);
    } else {
      buf[i]=0;
      if(t=TrieGetWord(buf)) {
found:
        tmp->trie=t;
        tmp->type=CYK_WORD;
      } else {
        "Unknown word:%s\n",buf;
        AddTemplatedWord(buf); //???
        if(t=TrieGetWord(buf)) goto found;
        tmp->type=CYK_UNKNOWN;
      }
      QueIns(tmp,ret->last);
    }
  }
  return ret;
}
class CCYKGrammarEnt:CHash {
  U8 *a,*b;
  F64 freq;
};
CHashTable *cyk_grammar=HashTableNew(0x200);
CHashTable *cyk_grammar_rules=HashTableNew(0x1000); //CHashGeneric Keys are the values "a,b",user_data0 points to CCYKGrammarEnt
CCYKGrammarEnt *GetGrammarEnt(U8 *name) {
  CCYKGrammarEnt *ent;
  if(!(ent=HashFind(name,cyk_grammar,1))) {
    ent=CAlloc(sizeof(CCYKGrammarEnt));
    ent->str=StrNew(name);
    ent->type=1;
    ent->freq=100;
  }
  return ent;
}

U0 LoadCYKGrammar(U8 *fn="GRAMMAR.CYK") {
  if(!FileFind(fn)) return;
  U8 *ftxt=FileRead(fn),abuf[STR_LEN],bbuf[STR_LEN],rbuf[STR_LEN];
  CCYKGrammarEnt *r_ent;
  CHashGeneric *r_ent2;
  U8 *a,*b,*c,*ptr=ftxt,*optr;
  loop:
  a=abuf,b=bbuf,c=rbuf;
  optr=ptr;
  if(*ptr=='"'&&(ptr=StrScan(ptr,"\"%s\",\"%s\"->\"%s\"",&a,&b,&c))!=optr) {
    r_ent=CAlloc(sizeof(CCYKGrammarEnt));
    r_ent->type=1;
    r_ent->freq=0;
    r_ent->a=StrNew(abuf);
    r_ent->b=StrNew(bbuf);
    r_ent->str=StrNew(rbuf);
    r_ent2=CAlloc(sizeof(CHashGeneric));
    r_ent2->type=1;
    r_ent2->str=MStrPrint("%s,%s",abuf,bbuf);
    r_ent2->user_data0=r_ent;
    if(*ptr=='(') {
      ptr=StrScan(ptr,"(%d)",&r_ent->freq);
    }
    HashAdd(r_ent,cyk_grammar);
    HashAdd(r_ent2,cyk_grammar_rules);
    nl:
    ptr=StrFirstOcc(ptr,"\n");
    if(ptr) {
      ptr++;
      goto loop;
    }
  } else goto nl;
  Free(ftxt);
}
U0 SaveCYKGrammar(U8 *fn="GRAMMAR.CYK") {
  CCYKGrammarEnt *hash;
  I64 i;
  CDoc *doc=DocNew(fn);
  for(i=0;i!=cyk_grammar->mask+1;i++) {
    for(hash=cyk_grammar->body[i];hash;hash=hash->next) {
      if(hash->a&&hash->b)
        DocPrint(doc,"\"%s\",\"%s\"->\"%s\"(%d)\n",
	      hash->a,
	      hash->b,
	      hash->str,
	      hash->freq
	      );
    }
  }
  DocSave(doc);
  DocDel(doc);
}
static U0 Classify(CCYKToken *ret,U8 *as) {
  CCYKToken *tmp;
  CCYKGrammarEnt *gent;
  if(as) {
    gent=GetGrammarEnt(as);
    tmp=CAlloc(sizeof CCYKToken);
    tmp->type=CYK_RULE;
    tmp->rule=gent;
    QueIns(tmp,ret->last);
  }
}
CCYKToken *CYKClassifyTerminals(CCYKToken *token) {
  CCYKToken *ret=CAlloc(sizeof(CCYKToken));
  QueInit(ret);
  CGTrie *trie;
  CGreekWord *gword;
  CCYKGrammarEnt *gent;
  U8 *as;
  if(token->type==CYK_WORD) {
    for(trie=token->trie;trie;trie=trie->ents[0]) {
      gword=trie->word;
      if(!gword) break;
      as=NULL;
      if(gword->flags&GWF_PREPOSITION){
        Classify(ret,"PREP");
      }
      if(gword->flags&GWF_INTEROGITIVE) {
        if(gword->flags&SNM==SNM) {
	  as="INTER-SNM";
	  Classify(ret,as);
        }
        if(gword->flags&SNF==SNF) {
	  as="INTER-SNF";
	  Classify(ret,as);;
        }
        if(gword->flags&SNN==SNN) {
	  as="INTER-SNN";
	  Classify(ret,as);;
        }
        if(gword->flags&SAM==SAM) {
	  as="INTER-SAM";
	  Classify(ret,as);;
        }
        if(gword->flags&SAF==SAF) {
	  as="INTER-SAF";
	  Classify(ret,as);;
        }
        if(gword->flags&SAN==SAN) {
	  as="INTER-SAN";
	  Classify(ret,as);;
        }
        if(gword->flags&SGM==SGM) {
	  as="INTER-SGM";
	  Classify(ret,as);;
        }
        if(gword->flags&SGF==SGF) {
	  as="INTER-SGF";
	  Classify(ret,as);;
        }
        if(gword->flags&SGN==SGN) {
	  as="INTER-SGN";
	  Classify(ret,as);;
        }
        if(gword->flags&SDM==SDM) {
	  as="INTER-SDM";
	  Classify(ret,as);;
        }
        if(gword->flags&SDF==SDF) {
	  as="INTER-SDF";
	  Classify(ret,as);;
        }
        if(gword->flags&SDN==SDN) {
	  as="INTER-SDN";
	  Classify(ret,as);;
        }
        if(gword->flags&PNM==PNM) {
	  as="INTER-PNM";
	  Classify(ret,as);;
        }
        if(gword->flags&PNF==PNF) {
	  as="INTER-PNF";
	  Classify(ret,as);;
        }
        if(gword->flags&PNN==PNN) {
	  as="INTER-PNN";
	  Classify(ret,as);;
        }
        if(gword->flags&PAM==PAM) {
	  as="INTER-PAM";
	  Classify(ret,as);;
        }
        if(gword->flags&PAF==PAF) {
	  as="INTER-PAF";
	  Classify(ret,as);;
        }
        if(gword->flags&PAN==PAN) {
	  as="INTER-PAN";
	  Classify(ret,as);;
        }
        if(gword->flags&PGM==PGM) {
	  as="INTER-PGM";
	  Classify(ret,as);;
        }
        if(gword->flags&PGF==PGF) {
	  as="INTER-PGF";
	  Classify(ret,as);;
        }
        if(gword->flags&PGN==PGN) {
	  as="INTER-PGN";
	  Classify(ret,as);;
        }
        if(gword->flags&PDM==PDM) {
	  as="INTER-PDM";
	  Classify(ret,as);;
        }
        if(gword->flags&PDF==PDF) {
	  as="INTER-PDF";
	  Classify(ret,as);;
        }
        if(gword->flags&PDN==PDN) {
	  as="INTER-PDN";
	  Classify(ret,as);;
        }
      }
      if(gword->flags&GWF_CONJUNCTION) {
        if(gword->flags&GWF_POST_POSITIVE) {
	  as="CONJ-PP";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_CONJ_BEFORE) {
	  as="CONJ-PRE";
	  Classify(ret,as);;
        }
        as="CONJ";
        Classify(ret,as);;
      }
      if(gword->flags&GWF_NOUN) {
//Nomitive
        if(gword->flags&SNM==SNM) {
	  as="N-SNM";
	  Classify(ret,as);;
        }
        if(gword->flags&SNF==SNF) {
	  as="N-SNF";
	  Classify(ret,as);;
        }
        if(gword->flags&SNN==SNN) {
	  as="N-SNN";
	  Classify(ret,as);;
        }
        if(gword->flags&PNM==PNM) {
	  as="N-PNM";
	  Classify(ret,as);;
        }
        if(gword->flags&PNF==PNF) {
	  as="N-PNF";
	  Classify(ret,as);;
        }
        if(gword->flags&PNN==PNN) {
	  as="N-PNN";
	  Classify(ret,as);;
        }
//Accusative
        if(gword->flags&SAM==SAM) {
	  as="N-SAM";
	  Classify(ret,as);;
        }
        if(gword->flags&SAF==SAF) {
	  as="N-SAF";
	  Classify(ret,as);;
        }
        if(gword->flags&SAN==SAN) {
	  as="N-SAN";
	  Classify(ret,as);;
        } 
        if(gword->flags&PAM==PAM) {
	  as="N-PAM";
	  Classify(ret,as);;
        } 
        if(gword->flags&PAF==PAF) {
	  as="N-PAF";
	  Classify(ret,as);;
        } 
        if(gword->flags&PAN==PAN) {
	  as="N-PAN";
	  Classify(ret,as);;
        }
//Genitive
        if(gword->flags&SGM==SGM) {
	  as="N-SGM";
	  Classify(ret,as);;
        }
        if(gword->flags&SGF==SGF) {
	  as="N-SGF";
	  Classify(ret,as);;
        }
        if(gword->flags&SGN==SGN) {
	  as="N-SGN";
	  Classify(ret,as);;
        }
        if(gword->flags&PGM==PGM) {
	  as="N-PGM";
	  Classify(ret,as);;
        }
        if(gword->flags&PGF==PGF) {
	  as="N-PGF";
	  Classify(ret,as);;
        }
        if(gword->flags&PGN==PGN) {
	  as="N-PGN";
	  Classify(ret,as);;
        }
//Dative
        if(gword->flags&SDM==SDM) {
	  as="N-SDM";
	  Classify(ret,as);;
        }
        if(gword->flags&SDF==SDF) {
	  as="N-SDF";
	  Classify(ret,as);;
        }
        if(gword->flags&SDN==SDN) {
	  as="N-SDN";
	  Classify(ret,as);;
        }
        if(gword->flags&PDM==PDM) {
	  as="N-PDM";
	  Classify(ret,as);;
        }
        if(gword->flags&PDF==PDF) {
	  as="N-PDF";
	  Classify(ret,as);;
        }
        if(gword->flags&PDN==PDN) {
	  as="N-PDN";
	  Classify(ret,as);;
        }
      }
      if(gword->flags&GWF_ADJECTIVE) {
//Nomitive
        if(gword->flags&SNM==SNM) {
	  as="A-SNM";
	  Classify(ret,as);;
        }
        if(gword->flags&SNF==SNF) {
	  as="A-SNF";
	  Classify(ret,as);;
        }
        if(gword->flags&SNN==SNN) {
	  as="A-SNN";
	  Classify(ret,as);;
        }
        if(gword->flags&PNM==PNM) {
	  as="A-PNM";
	  Classify(ret,as);;
        }
        if(gword->flags&PNF==PNF) {
	  as="A-PNF";
	  Classify(ret,as);;
        }
        if(gword->flags&PNN==PNN) {
	  as="A-PNN";
	  Classify(ret,as);;
        }
//Accusative
        if(gword->flags&SAM==SAM) {
	  as="A-SAM";
	  Classify(ret,as);;
        }
        if(gword->flags&SAF==SAF) {
	  as="A-SAF";
	  Classify(ret,as);;
        }
        if(gword->flags&SAN==SAN) {
	  as="A-SAN";
	  Classify(ret,as);;
        }
        if(gword->flags&PAM==PAM) {
	  as="A-PAM";
	  Classify(ret,as);;
        }
        if(gword->flags&PAF==PAF) {
	  as="A-PAF";
	  Classify(ret,as);;
        }
        if(gword->flags&PAN==PAN) {
	  as="A-PAN";
	  Classify(ret,as);;
        }
//Genitive
        if(gword->flags&SGM==SGM) {
	  as="A-SGM";
	  Classify(ret,as);;
        }
        if(gword->flags&SGF==SGF) {
	  as="A-SGF";
	  Classify(ret,as);;
        }
        if(gword->flags&SGN==SGN) {
	  as="A-SGN";
	  Classify(ret,as);;
        }
        if(gword->flags&PGM==PGM) {
	  as="A-PGM";
	  Classify(ret,as);;
        }
        if(gword->flags&PGF==PGF) {
	  as="A-PGF";
	  Classify(ret,as);;
        }
        if(gword->flags&PGN==PGN) {
	  as="A-PGN";
	  Classify(ret,as);;
        }
//Dative
        if(gword->flags&SDM==SDM) {
	  as="A-SDM";
	  Classify(ret,as);;
        }
        if(gword->flags&SDF==SDF) {
	  as="A-SDF";
	  Classify(ret,as);;
        }
        if(gword->flags&SDN==SDN) {
	  as="A-SDN";
	  Classify(ret,as);;
        }
        if(gword->flags&PDM==PDM) {
	  as="A-PDM";
	  Classify(ret,as);;
        }
        if(gword->flags&PDF==PDF) {
	  as="A-PDF";
	  Classify(ret,as);;
        }
        if(gword->flags&PDN==PDN) {
	  as="A-PDN";
	  Classify(ret,as);;
        }
      }
      if(gword->flags&GWF_ARTICLE) {
//Nomitive
        if(gword->flags&SNM==SNM) {
	  as="ART-SNM";
	  Classify(ret,as);;
        }
        if(gword->flags&SNF==SNF) {
	  as="ART-SNF";
	  Classify(ret,as);;
        }
        if(gword->flags&SNN==SNN) {
	  as="ART-SNN";
	  Classify(ret,as);;
        }
        if(gword->flags&PNM==PNM) {
	  as="ART-PNM";
	  Classify(ret,as);;
        }
        if(gword->flags&PNF==PNF) {
	  as="ART-PNF";
	  Classify(ret,as);;
        }
        if(gword->flags&PNN==PNN) {
	  as="ART-PNN";
	  Classify(ret,as);;
        }
//Accusative
        if(gword->flags&SAM==SAM) {
	  as="ART-SAM";
	  Classify(ret,as);;
        }
        if(gword->flags&SAF==SAF) {
	  as="ART-SAF";
	  Classify(ret,as);;
        }
        if(gword->flags&SAN==SAN) {
	  as="ART-SAN";
	  Classify(ret,as);;
        }
        if(gword->flags&PAM==PAM) {
	  as="ART-PAM";
	  Classify(ret,as);;
        }
        if(gword->flags&PAF==PAF) {
	  as="ART-PAF";
	  Classify(ret,as);;
        }
        if(gword->flags&PAN==PAN) {
	  as="ART-PAN";
	  Classify(ret,as);;
        }
//Genitive
        if(gword->flags&SGM==SGM) {
	  as="ART-SGM";
	  Classify(ret,as);;
        }
        if(gword->flags&SGF==SGF) {
	  as="ART-SGF";
	  Classify(ret,as);;
        }
        if(gword->flags&SGN==SGN) {
	  as="ART-SGN";
	  Classify(ret,as);;
        }
        if(gword->flags&PGM==PGM) {
	  as="ART-PGM";
	  Classify(ret,as);;
        }
        if(gword->flags&PGF==PGF) {
	  as="ART-PGF";
	  Classify(ret,as);;
        }
        if(gword->flags&PGN==PGN) {
	  as="ART-PGN";
	  Classify(ret,as);;
        }
//Dative
        if(gword->flags&SDM==SDM) {
	  as="ART-SDM";
	  Classify(ret,as);;
        }
        if(gword->flags&SDF==SDF) {
	  as="ART-SDF";
	  Classify(ret,as);;
        }
        if(gword->flags&SDN==SDN) {
	  as="ART-SDN";
	  Classify(ret,as);;
        }
        if(gword->flags&PDM==PDM) {
	  as="ART-PDM";
	  Classify(ret,as);;
        }
        if(gword->flags&PDF==PDF) {
	  as="ART-PDF";
	  Classify(ret,as);;
        }
        if(gword->flags&PDN==PDN) {
	  as="ART-PDN";
	  Classify(ret,as);;
        }
      }
      if(gword->flags&GWF_PRONOUN) {
//Nomitive
        if(gword->flags&SNM==SNM) {
	  as="PRO-SNM";
	  Classify(ret,as);;
        }
        if(gword->flags&SNF==SNF) {
	  as="PRO-SNF";
	  Classify(ret,as);;
        }
        if(gword->flags&SNN==SNN) {
	  as="PRO-SNN";
	  Classify(ret,as);;
        }
        if(gword->flags&PNM==PNM) {
	  as="PRO-PNM";
	  Classify(ret,as);;
        }
        if(gword->flags&PNF==PNF) {
	  as="PRO-PNF";
	  Classify(ret,as);;
        }
        if(gword->flags&PNN==PNN) {
	  as="PRO-PNN";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_NOMITIVE&&gword->flags&S_1P) {
	  as="PRO-N-S1P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_NOMITIVE&&gword->flags&S_2P) {
	  as="PRO-N-S2P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_NOMITIVE&&gword->flags&S_3P) {
	  as="PRO-N-S3P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_NOMITIVE&&gword->flags&PL_1P) {
	  as="PRO-N-PL1P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_NOMITIVE&&gword->flags&PL_2P) {
	  as="PRO-N-PL2P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_NOMITIVE&&gword->flags&PL_3P) {
	  as="PRO-N-PL3P";
	  Classify(ret,as);;
        }
//Accusative
        if(gword->flags&SAM==SAM) {
	  as="PRO-SAM";
	  Classify(ret,as);;
        }
        if(gword->flags&SAF==SAF) {
	  as="PRO-SAF";
	  Classify(ret,as);;
        }
        if(gword->flags&SAN==SAN) {
	  as="PRO-SAN";
	  Classify(ret,as);;
        }
        if(gword->flags&PAM==PAM) {
	  as="PRO-PAM";
	  Classify(ret,as);;
        }
        if(gword->flags&PAF==PAF) {
	  as="PRO-PAF";
	  Classify(ret,as);;
        }
        if(gword->flags&PAN==PAN) {
	  as="PRO-PAN";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_ACCUSATIVE&&gword->flags&S_1P) {
	  as="PRO-A-S1P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_ACCUSATIVE&&gword->flags&S_2P) {
	  as="PRO-A-S2P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_ACCUSATIVE&&gword->flags&S_3P) {
	  as="PRO-A-S3P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_ACCUSATIVE&&gword->flags&PL_1P) {
	  as="PRO-A-PL1P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_ACCUSATIVE&&gword->flags&PL_2P) {
	  as="PRO-A-PL2P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_ACCUSATIVE&&gword->flags&PL_3P) {
	  as="PRO-A-PL3P";
	  Classify(ret,as);;
        }
//Genitive
        if(gword->flags&SGM==SGM) {
	  as="PRO-SGM";
	  Classify(ret,as);;
        }
        if(gword->flags&SGF==SGF) {
	  as="PRO-SGF";
	  Classify(ret,as);;
        }
        if(gword->flags&SGN==SGN) {
	  as="PRO-SGN";
	  Classify(ret,as);;
        }
        if(gword->flags&PGM==PGM) {
	  as="PRO-PGM";
	  Classify(ret,as);;
        }
        if(gword->flags&PGF==PGF) {
	  as="PRO-PGF";
	  Classify(ret,as);;
        }
        if(gword->flags&PGN==PGN) {
	  as="PRO-PGN";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_GENITIVE&&gword->flags&S_1P) {
	  as="PRO-G-S1P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_GENITIVE&&gword->flags&S_2P) {
	  as="PRO-G-S2P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_GENITIVE&&gword->flags&S_3P) {
	  as="PRO-G-S3P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_GENITIVE&&gword->flags&PL_1P) {
	  as="PRO-G-PL1P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_GENITIVE&&gword->flags&PL_2P) {
	  as="PRO-G-PL2P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_GENITIVE&&gword->flags&PL_3P) {
	  as="PRO-G-PL3P";
	  Classify(ret,as);;
        }
//Dative
        if(gword->flags&SDM==SDM) {
	  as="PRO-SDM";
	  Classify(ret,as);;
        }
        if(gword->flags&SDF==SDF) {
	  as="PRO-SDF";
	  Classify(ret,as);;
        }
        if(gword->flags&SDN==SDN) {
	  as="PRO-SDN";
	  Classify(ret,as);;
        }
        if(gword->flags&PDM==PDM) {
	  as="PRO-PDM";
	  Classify(ret,as);;
        }
        if(gword->flags&PDF==PDF) {
	  as="PRO-PDF";
	  Classify(ret,as);;
        }
        if(gword->flags&PDN==PDN) {
	  as="PRO-PDN";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_DATIVE&&gword->flags&S_1P) {
	  as="PRO-D-S1P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_DATIVE&&gword->flags&S_2P) {
	  as="PRO-D-S2P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_DATIVE&&gword->flags&S_3P) {
	  as="PRO-D-S3P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_DATIVE&&gword->flags&PL_1P) {
	  as="PRO-D-PL1P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_DATIVE&&gword->flags&PL_2P) {
	  as="PRO-D-PL2P";
	  Classify(ret,as);;
        }
        if(gword->flags&GWF_DATIVE&&gword->flags&PL_3P) {
	  as="PRO-D-PL3P";
	  Classify(ret,as);;
        }
      } else if(gword->flags&GWF_VERB) {
        if(gword->flags&S_1P==S_1P) {
	  as="V-S1P";
	  Classify(ret,as);;
        }
        if(gword->flags&S_2P==S_2P) {
	  as="V-S2P";
	  Classify(ret,as);;
        }
        if(gword->flags&S_3P==S_3P) {
	  as="V-S3P";
	  Classify(ret,as);;
        }
        if(gword->flags&PL_1P==PL_1P) {
	  as="V-PL1P";
	  Classify(ret,as);;
        }
        if(gword->flags&PL_2P==PL_2P) {
	  as="V-PL2P";
	  Classify(ret,as);;
        }
        if(gword->flags&PL_3P==PL_3P) {
	  as="V-PL3P";
	  Classify(ret,as);;
        }
      }
      if(gword->flags&GWF_ADVERB) {
        as="ADV";
        Classify(ret,as);
      }
    }
  }
  return ret;
}
U0 AddAliasedRules(CCYKToken *head) {
  CCYKToken *cur,*cur2,*tmp;
  CHashGeneric *gen;
  U8 *pair;
  Bool found;
start_over:
  found=FALSE;
  for(cur=head->next;cur!=head;cur=cur->next) {
    if(cur->type==CYK_RULE) {
      pair=MStrPrint("%s,",cur->rule->str);
      if(gen=HashFind(pair,cyk_grammar_rules,1)) {
        loop:
        for(cur2=head->next;cur2!=head;cur2=cur2->next)
	  if(cur2->rule)
	    if(!StrCmp(cur2->rule->str,gen->user_data0(CCYKGrammarEnt*)->str))
	      goto fail;
        tmp=CAlloc(sizeof CCYKToken);
        tmp->type=CYK_RULE;
        tmp->rule=gen->user_data0;
        QueInit(tmp);
        QueIns(tmp,head->last);
        found=TRUE;\
        fail:
        if(gen) gen=gen->next;
        for(;gen;gen=gen->next) {
          if(!StrCmp(gen->str,pair))
            goto loop;
        }
      }
      Free(pair);
    }
  }
  if(found) goto start_over;
}
CCYKToken *CYKRun(U8 *str) {
  CCYKToken *terminals=TokenizeString(str),**matrix,*token,*token2,*head,*head2,*token0,*head0;
  CHashGeneric *find;
  I64 cnt=QueCnt(terminals),x,y,i,l,j,k;

  U8 *pair;
  matrix=CAlloc(cnt*cnt*sizeof(CCYKToken));
  token=terminals;
  for(x=0;x!=cnt;x++) {
    y=x;
    token=token->next;
    matrix[x+y*cnt]=CYKClassifyTerminals(token);
    AddAliasedRules(matrix[x+y*cnt]);
  }
  for(l=2;l<=cnt;l++) {
    for(i=1;i<=cnt-l+1;i++) {
      j=i+l-1;
      for(k=1;k<=j-1;k++) {
        head=token=matrix[i-1+(k-1)*cnt];
        head2=token2=matrix[k+(j-1)*cnt];
        if(!(head0=matrix[i-1+(j-1)*cnt])) {
          head0=CAlloc(sizeof CCYKToken);
          QueInit(head0);
        }
        if(token&&token2) {
	  token=token->next,token2=token2->next;
	  for(;token!=head;token=token->next) {
	    token2=head2->next;
	    for(;token2!=head2;token2=token2->next) {
	      if(token->type==CYK_RULE&&token2->type==CYK_RULE) {
	        pair=MStrPrint("%s,%s",token->rule->str,token2->rule->str);
	        for(find=HashFind(pair,cyk_grammar_rules,1);find;find=find->next) {
		  if(!StrCmp(find->str,pair)){
		    token0=CAlloc(sizeof(CCYKToken));
		    QueInit(token0);
		    QueIns(token0,head0->last);
		    token0->type=CYK_RULE;
		    token0->rule=find->user_data0;
		    token0->rule->freq++;
		  }
	        }
	        Free(pair);
	      }
	    }
	  }
        }
        matrix[i-1+(j-1)*cnt]=head0;
        AddAliasedRules(matrix[i-1+(j-1)*cnt]);
      }
    }
  }
  "<TABLE BORDER>\n";
  for(x=0;x!=cnt;x++) {
    "<TR>";
    for(y=0;y!=cnt;y++) {
      "<TD>(%d,%d)",x,y;
      if(matrix[x+y*cnt]) {
        head=matrix[x+y*cnt];
        for(head=head->next;head!=matrix[x+y*cnt];head=head->next) {
	  "%s,\n",head->rule->str;
        }
        QueDel(matrix[x+y*cnt]);
      }
      "</TD>";
    }
    "</TR>\n";
  }
  "</TABLE>\n";;
  Free(matrix);
}
LoadCYKGrammar;
class CTranslitPair {
  U32 i;
  U8 *translit;
};
//https://en.wikipedia.org/wiki/Greek_script_in_Unicode
CTranslitPair alpha[60]={
  {0x03B1,"a"},
  {0x1D6C2,"a"},
  {0x1D6FC,"a"},
  {0x1D736,"a"},
  {0x1D770,"a"},
  {0x1D7AA,"a"},
  {0x391,"a"},
  {0x1D6A8,"a"},
  {0x1D6E2,"a"},
  {0x1D71C,"a"},
  {0x1D756,"a"},
  {0x1D790,"a"},
  {0x1F00,"a"},
  {0x1F08,"a"},
  {0x1F04,"a"},
  {0x1F0C,"a"},
  {0x1F84,"ai"},
  {0x1F8C,"ai"},
  {0x1F02,"a"},
  {0x1F0A,"a"},
  {0x1F82,"ai"},
  {0x1F8A,"ai"},
  {0x1F06,"ta"},
  {0x1F0E,"ta"},
  {0x1F86,"tai"},
  {0x1F8E,"tai"},
  {0x1F80,"ai"},
  {0x1F88,"ai"},
  {0x1F01,"ha"},
  {0x1F09,"ha"},
  {0x1F05,"ha"},
  {0x1F0D,"ha"},
  {0x1F85,"hai"},
  {0x1F8D,"hai"},
  {0x1F03,"a"},
  {0x1F0B,"a"},
  {0x1F83,"hai"},
  {0x1F8B,"hai"},
  {0x1F07,"ta"},
  {0x1F0F,"ta"},
  {0x1F87,"tai"},
  {0x1F8F,"tai"},
  {0x1F81,"hai"},
  {0x1F89,"hai"},
  {0x3AC,"a"},
  {0x1F71,"a"},
  {0x386,"a"},
  {0x1FBB,"a"},
  {0x1FB4,"ai"},
  {0x1F70,"a"},
  {0x1FBA,"a"},
  {0x1FB2,"ai"},
  {0x1FB0,"a"},
  {0x1FB8,"a"},
  {0x1FB6,"a"},
  {0x1FB1,"a"},
  {0x1FB8,"a"},
  {0x1FB9,"a"},
  {0x1FB3,"ai"},
  {0x1FB8,"ai"},
};
CTranslitPair beta[15]={
  {0x3B2,"b"},
  {0x1D6C3,"b"},
  {0x1D6FD,"b"},
  {0x1D737,"b"},
  {0x1D771,"b"},
  {0x1D7AB,"b"},
  {0x392,"b"},
  {0x1D6A9,"b"},
  {0x1D6E3,"b"},
  {0x1D71D,"b"},
  {0x1D757,"b"},
  {0x1D791,"b"},
  {0x1D5D,"b"},
  {0x1D66,"b"},
  {0x3D0,"b"},
};
CTranslitPair gamma[11]={
  {0x3B3,"g"},
  {0x1D6C4,"g"},
  {0x1D6FE,"g"},
  {0x1D738,"g"},
  {0x1D772,"g"},
  {0x1D7AC,"g"},
  {0x393,"g"},
  {0x1D6AA,"g"},
  {0x1D6E4,"g"},
  {0x1D71E,"g"},
  {0x1D758,"g"},
  {0x1D792,"g"},
  {0x213D,"g"},
  {0x213E,"g"},
  {0x1D5E,"g"},
  {0x1D67,"g"},
  {0x1D26,"g"},
};
CTranslitPair delta[13]={
  {0x3B4,"d"},
  {0x1D6C5,"d"},
  {0x1D6FF,"d"},
  {0x1D739,"d"},
  {0x1D773,"d"},
  {0x1D7AD,"d"},
  {0x394,"d"},
  {0x1D6AB,"d"},
  {0x1D6E5,"d"},
  {0x1D71F,"d"},
  {0x1D759,"d"},
  {0x1D793,"d"},
  {0x1D5F,"d"},
};

CTranslitPair epsilon[36]={
  {0x3B5,"e"},
  {0x3F5,"e"},
  {0x1D6C6,"e"},
  {0x1D6DC,"e"},
  {0x1D700,"e"},
  {0x1D716,"e"},
  {0x1D73A,"e"},
  {0x1D750,"e"},
  {0x1D774,"e"},
  {0x1D78A,"e"},
  {0x1D7AE,"e"},
  {0x1D7C4,"e"},
  {0x0395,"e"},
  {0x1D6AC,"e"},
  {0x1D6E6,"e"},
  {0x1D720,"e"},
  {0x1D75A,"e"},
  {0x1D794,"e"},
  {0x1F10,"e"},
  {0x1F18,"e"},
  {0x1F14,"e"},
  {0x1F1C,"e"},
  {0x1F12,"e"},
  {0x1F1A,"e"},
  {0x1F11,"e"},
  {0x1F19,"he"},
  {0x1F15,"he"},
  {0x1F1D,"he"},
  {0x1F13,"he"},
  {0x1F1B,"he"},
  {0x3AD,"e"},
  {0x1F73,"he"},
  {0x388,"he"},
  {0x1FC9,"e"},
  {0x1F73,"e"},
  {0x388,"e"},
  {0x1FC9,"e"},
  {0x1F72,"e"},
  {0x1FC8,"e"},
};
CTranslitPair zeta[12]={
  {0x03B6,"z"},
  {0x1D6C7,"z"},
  {0x1D701,"z"},
  {0x1D73B,"z"},
  {0x1D775,"z"},
  {0x1D7AF,"z"},
  {0x0396,"z"},
  {0x1D6AD,"z"},
  {0x1D6E7,"z"},
  {0x1D721,"z"},
  {0x1D75B,"z"},
  {0x1D795,"z"},
};
CTranslitPair eta[12]={
  {0x3B7,"E"},
  {0x1D6C8,"E"},
  {0x1D702,"E"},
  {0x1D73C,"E"},
  {0x1D776,"E"},
  {0x1D7B0,"E"},
  {0x397,"E"},
  {0x1D6AE,"E"},
  {0x1D6E8,"E"},
  {0x1D722,"E"},
  {0x1D75C,"E"},
  {0x1D796,"E"},
  {0x1F20,"E"},
  {0x1F28,"E"},
  {0x1F24,"hE"},
  {0x1F2C,"hE"},
  {0x1F94,"hEi"},
  {0x1F9C,"hEi"},
  {0x1F22."E"},
  {0x1F2A,"E"},
  {0x1F92,"Ei"},
  {0x1F9A,"Ei"},
  {0x1F26,"E"},
  {0x1F2E,"E"},
  {0x1F96,"Ei"},
  {0x1F9E,"Ei"},
  {0x1F90,"Ei"},
  {0x1F98,"Ei"},
  {0x1F21,"hE"},
  {0x1F29,"hE"},
  {0x1F25,"hE"},
  {0x1F2D,"hE"},
  {0x1F95,"hEi"},
  {0x1F9D,"hEi"},
  {0x1F23,"hEi"},
  {0x1F2B,"hE"},
  {0x1F93,"hEi"},
  {0x1F9B,"hEi"},
  {0x1F27,"E"},
  {0x1F2F,"E"},
  {0x1F97,"Ei"},
  {0x1F9F,"Ei"},
  {0x1F91,"hEi"},
  {0x1F99,"hEi"},
  {0x3AE,"E"},
  {0x1F75,"E"},
  {0x389,"E"},
  {0x1FCB,"E"},
  {0x1FC4,"Ei"},
  {0x1F74,"E"},
  {0x1FCA,"E"},
  {0x1FC2,"Ei"},
  {0x1FC6,"E"},
  {0x1FC7,"Ei"},
  {0x1FC3,"Ei"},
  {0x1FCC,"Ei"},
};
CTranslitPair theta[]={
  {0x03B8,"th"},
  {0x03D1,"th"},
  {0x1D6C9,"th"},
  {0x1D6DD,"th"},
  {0x1D703,"th"},
  {0x1D717,"th"},
  {0x1D73D,"th"},
  {0x1D751,"th"},
  {0x1D777,"th"},
  {0x1D78B,"th"},
  {0x1D7B1,"th"},
  {0x1D7C5,"th"},
  {0x398,"th"},
  {0x3F4,"th"},
  {0x1D6AF,"th"},
  {0x1D6B9,"th"},
  {0x1D6E9,"th"},
  {0x1D6F3,"th"},
  {0x1D723,"th"},
  {0x1D72D,"th"},
  {0x1D75D,"th"},
  {0x1D767,"th"},
  {0x1D797,"th"},
  {0x1D7A1,"th"},
  {0x1DBF,"th"},
};
CTranslitPair iota[]={
  {0x3B9,"i"},
  {0x1D6CA,"i"},
  {0x1D704,"i"},
  {0x1D73E,"i"},
  {0x1D778,"i"},
  {0x1D7B2,"i"},
  {0x399,"i"},
  {0x1D6B0,"i"},
  {0x1D6EA,"i"},
  {0x1D724,"i"},
  {0x1D75E,"i"},
  {0x1D798,"i"},
  {0x1F30,"i"},
  {0x1F38,"i"},
  {0x1F34,"hi"},
  {0x1F3C,"hi"},
  {0x1F32,"i"},
  {0x1F3A,"i"},
  {0x1F36,"i"},
  {0x1F3E,"i"},
  {0x1F31,"hi"},
  {0x1F39,"hi"},
  {0x1F35,"hi"},
  {0x1F3D,"hi"},
  {0x1F33,"hi"},
  {0x1F3B,"hi"},
  {0x1F37,"i"},
  {0x1F3F,"i"},
  {0x3AF,"i"},
  {0x1F77,"i"},
  {0x38A,"i"},
  {0x1FDB,"i"},
  {0x1F76,"i"},
  {0x1FDA,"i"},
  {0x1FD0,"i"},
  {0x1FD8,"i"},
  {0x1FD6,"i"},
  {0x3CA,"i"},
  {0x3AA,"i"},
  {0x390,"i"},
  {0x1FD3,"i"},
  {0x1FD2,"i"},
  {0x1FD7,"i"},
  {0x1FD1,"i"},
  {0x1FD9,"i"},
  {0x1FBE,"i"},
  {0x37A,"i"},
};
CTranslitPair kappa[]={
  {0x3BA,"k"},
  {0x3F0,"k"},
  {0x1D6CB,"k"},
  {0x1D6DE,"k"},
  {0x1D705,"k"},
  {0x1D718,"k"},
  {0x1D73F,"k"},
  {0x1D752,"k"},
  {0x1D779,"k"},
  {0x1D78C,"k"},
  {0x1D7B3,"k"},
  {0x1D7C6,"k"},
  {0x39A,"k"},
  {0x1D6B1,"k"},
  {0x1D6EB,"k"},
  {0x1D725,"k"},
  {0x1D75F,"k"},
  {0x1D799,"k"},
  {0x3D7,"k"},
  {0x3CF,"k"},
};
CTranslitPair lambda[]={
  {0x3BB,"l"},
  {0x1D6CC,"l"},
  {0x1D706,"l"},
  {0x1D740,"l"},
  {0x1D77A,"l"},
  {0x1D7B4,"l"},
  {0x39B,"l"},
  {0x1D6EC,"l"},
  {0x1D726,"l"},
  {0x1D760,"l"},
  {0x1D79A,"l"},
  {0x1D27,"l"},
  {0x19B,"l"},
};
CTranslitPair mu[]={
  {0x3BC,"m"},
  {0x1D6CD,"m"},
  {0x1D707,"m"},
  {0x1D741,"m"},
  {0x1D77B,"m"},
  {0x1D7B5,"m"},
  {0x39C,"m"},
  {0x1D6B3,"m"},
  {0x1D6ED,"m"},
  {0x1D727,"m"},
  {0x1D761,"m"},
  {0x1D79B,"m"},
  {0xB5,"m"},
};
CTranslitPair nu[12]={
  {0x3BD,"n"},
  {0x1D6CE,"n"},
  {0x1D708,"n"},
  {0x1D742,"n"},
  {0x1D77C,"n"},
  {0x1D7B6,"n"},
  {0x39D,"n"},
  {0x1D6B4,"n"},
  {0x1D6EE,"n"},
  {0x1D728,"n"},
  {0x1D762,"n"},
  {0x1D76C,"n"},
};
CTranslitPair xi[12]={
  {0x03BE,"x"},
  {0x1D6CF,"x"},
  {0x1D709,"x"},
  {0x1D743,"x"},
  {0x1D77D,"x"},
  {0x1D7B7,"x"},
  {0x39E,"x"},
  {0x1D6B5,"x"},
  {0x1D6EF,"x"},
  {0x1D729,"x"},
  {0x1D763,"x"},
  {0x1D79D,"x"},
};
CTranslitPair omricon[30]={
  {0x03BF,"o"},
  {0x1D6D0,"o"},
  {0x1D70A,"o"},
  {0x1D744,"o"},
  {0x1D77E,"o"},
  {0x1D7B8,"o"},
  {0x39F,"o"},
  {0x1D6B6,"o"},
  {0x1D6F0,"o"},
  {0x1D72A,"o"},
  {0x1D764,"o"},
  {0x1D79E,"o"},
  {0x1F40,"o"},
  {0x1F48,"o"},
  {0x1F44,"o"},
  {0x1F4C,"o"},
  {0x1F4A,"o"},
  {0x1F41,"ho"},
  {0x1F49,"ho"},
  {0x1F45,"ho"},
  {0x1F4D,"ho"},
  {0x1F43,"ho"},
  {0x1F4B,"ho"},
  {0x3CC,"o"},
  {0x1F79,"o"},
  {0x38C,"o"},
  {0x1FF9,"o"},
  {0x1F78,"o"},
  {0x1FF8,"o"},
};
CTranslitPair _pi[21]={$WW,1$
  {0x3C0,"pi"},
  {0x3D6,"pi"},
  {0x1D6D1,"pi"},
  {0x1D6E1,"pi"},
  {0x1D70B,"pi"},
  {0x1D71B,"pi"},
  {0x1D745,"pi"},
  {0x1D755,"pi"},
  {0x1D77F,"pi"},
  {0x1D78F,"pi"},
  {0x1D7B9,"pi"},
  {0x1D7C9,"pi"},
  {0x3A0,"pi"},
  {0x1D6B7,"pi"},
  {0x1D6F1,"pi"},
  {0x1D72B,"pi"},
  {0x1D765,"pi"},
  {0x1D79F,"pi"},
  {0x213C,"pi"},
  {0x213F,"pi"},
  {0x1D28,"pi"},
};
CTranslitPair rho[24]={
  {0x3C1,"r"},
  {0x3F1,"r"},
  {0x1D6D2,"r"},
  {0x1D6E0,"r"},
  {0x1D70C,"r"},
  {0x1D71A,"r"},
  {0x1D746,"r"},
  {0x1D754,"r"},
  {0x1D780,"r"},
  {0x1D78E,"r"},
  {0x1D7BA,"r"},
  {0x1D7C8,"r"},
  {0x3A1,"r"},
  {0x1D6B8,"r"},
  {0x1D6F2,"r"},
  {0x1D72C,"r"},
  {0x1D76,"r"6},
  {0x1D7A0,"r"},
  {0x1D68,"r"},
  {0x1FE4,"r"},
  {0x1FE5,"r"},
  {0x1FEC,"r"},
  {0x1D29,"r"},
  {0x3FC,"r"},
};
CTranslitPair sigma[20]={
  {0x3C3,"s"},
  {0x3C2,"s"},
  {0x1D6D3,"s"},
  {0x1D6D4,"s"},
  {0x1D70D,"s"},
  {0x1D70E,"s"},
  {0x1D747,"s"},
  {0x1D748,"s"},
  {0x1D781,"s"},
  {0x1D782,"s"},
  {0x1D7BB,"s"},
  {0x1D7BC,"s"},
  {0x3A3,"s"},
  {0x1D6BA,"s"},
  {0x1D6F4,"s"},
  {0x1D72E,"s"},
  {0x1D768,"s"},
  {0x1D7A2,"s"},
};
CTranslitPair tua[12]={
  {0x3C4,"t"},
  {0x1D6D5,"t"},
  {0x1D70F,"t"},
  {0x1D749,"t"},
  {0x1D783,"t"},
  {0x1D7BD,"t"},
  {0x3A4,"t"},
  {0x1D6BB,"t"},
  {0x1D6F5,"t"},
  {0x1D72F,"t"},
  {0x1D769,"t"},
  {0x1D7A3,"t"},
};
CTranslitPair upsilon[38]={
  {0x3C5,"u"},
  {0x1D6D6,"u"},
  {0x1D710,"u"},
  {0x1D74A,"u"},
  {0x1D784,"u"},
  {0x1D7BE,"u"},
  {0x3A5,"u"},
  {0x3D2,"u"},
  {0x1D6BC,"u"},
  {0x1D6F6,"u"},
  {0x1D730,"u"},
  {0x1D76A,"u"},
  {0x1D7A4,"u"},
  {0x1F50,"u"},
  {0x1F54,"hu"},
  {0x1F52,"u"},
  {0x1F56,"u"},
  {0x1F51,"hu"},
  {0x1F59,"hu"},
  {0x1F55,"hu"},
  {0x1F5D,"hu"},
  {0x1F53,"hu"},
  {0x1F5B,"hu"},
  {0x1F57,"u"},
  {0x1F5F,"u"},
  {0x3CD,"u"},
  {0x1F7B,"u"},
  {0x38E,"u"},
  {0x1FEB,"u"},
  {0x3D3,"u"},
  {0x1F7A,"u"},
  {0x1FEA,"u"},
  {0x1FE0,"u"},
  {0x1FE8,"u"},
  {0x1FE6,"u"},
  {0x3CB,"u"},
  {0x3AB,"u"},
  {0x3D4,"u"},
  {0x3B0,"u"},
  {0x1FE3,"u"},
  {0x1FE2,"u"},
  {0x1FE7,"u"},
  {0x1FE1,"u"},
  {0x1FE9,"u"},
};
CTranslitPair phi[20]={
  {0x3C6,"ph"},
  {0x3D5,"ph"},
  {0x1D6D7,"ph"},
  {0x1D6DF,"ph"},
  {0x1D711,"ph"},
  {0x1D719,"ph"},
  {0x1D74B,"ph"},
  {0x1D753,"ph"},
  {0x1D786,"ph"},
  {0x1D78D,"ph"},
  {0x1D7BF,"ph"},
  {0x1D7C7,"ph"},
  {0x3A6,"ph"},
  {0x1D6BD,"ph"},
  {0x1D6F7,"ph"},
  {0x1D731,"ph"},
  {0x1D76B,"ph"},
  {0x1D7A5,"ph"},
  {0x1D60,"ph"},
  {0x1D69,"ph"},
};
CTranslitPair chi[14]={
  {0x3C7,"ch"},
  {0x1D6D8,"ch"},
  {0x1D712,"ch"},
  {0x1D74C,"ch"},
  {0x1D786,"ch"},
  {0x1D7C0,"ch"},
  {0x3A7,"ch"},
  {0x1D6BE,"ch"},
  {0x1D6F8,"ch"},
  {0x1D732,"ch"},
  {0x1D76C,"ch"},
  {0x1D7A6,"ch"},
  {0x1D61,"ch"},
  {0x1D6A,"ch"},
};
CTranslitPair psi[]={
  {0x3C8,"ps"},
  {0x1D6D9,"ps"},
  {0x1D713,"ps"},
  {0x1D74D,"ps"},
  {0x1D787,"ps"},
  {0x1D7C1,"ps"},
  {0x3A8,"ps"},
  {0x1D6BF,"ps"},
  {0x12DF9,"ps"},
  {0x1D733,"ps"},
  {0x1D76D,"ps"},
  {0x1D7A7,"ps"},
  {0x1D2A,"ps"},
};
CTranslitPair omega[58]={
  {0x3C9,"w"},
  {0x1D6DA,"w"},
  {0x1D714,"w"},
  {0x1D74E,"w"},
  {0x1D788,"w"},
  {0x1D7C2,"w"},
  {0x3A9,"w"},
  {0x1D6FA,"w"},
  {0x1D734,"w"},
  {0x1D76E,"w"},
  {0x1D7A8,"w"},
  {0x1F60,"w"},
  {0x1F68,"w"},
  {0x1F64,"w"},
  {0x1F6C,"w"},
  {0x1FA4,"wi"},
  {0x1FAC,"wi"},
  {0x1F62,"w"},
  {0x1F6A,"w"},
  {0x1FA2,"wi"},
  {0x1FAA,"w"},
  {0x1F66,"w"},
  {0x1F6E,"w"},
  {0x1FA6,"wi"},
  {0x1FAE,"wi"},
  {0x1FA0,"wi"},
  {0x1FA8,"wi"},
  {0x1F61,"hwi"},
  {0x1F69,"hwi "},
  {0x1F65},
  {0x1F6D},
  {0x1FA5},
  {0x1FAD},
  {0x1F63},
  {0x1F6B},
  {0x1FA3},
  {0x1FAB},
  {0x1F67},
  {0x1F6F},
  {0x1FA7},
  {0x1FAF},
  {0x1FF4},
  {0x1F7C},
  {0x1FFA},
  {0x1FF2},
  {0x1FF6},
  {0x1FF7},
  {0x1FF3},
  {0x1FFC},
  {0xAB65},
  {0x2126},


};
I64 __TransliteralGreek(U32 *utf32,U8 *res=NULL) {
  I64 len=0;
  U8 *str;
  while(*utf32) {
    str="";
    
  }
}
//CYKRun("ohi archontes"); //Yeah(manualy inspected)
//CYKRun("ho pais didwsi tois archousi ton paida tou paidos"); //Yeah(manually inspected)
//CYKRun("chrEmata kai humarchonta didomen"); //Yeah (Manually checked)
//CYKRun("didomen ta spermata kai didote swmata"); //Yeah(mnaually tested)
//CYKRun("eite didomen ta chrEmata eite didote humarchontas"); //Yes?(manualy validated by me)
//CYKRun("ohi archontes didoasin autEn autois"); //Yes(manually checked)
//CYKRun("ohi archontes didoasin tEn elpida tois paisi"); //Yes manually checked
//CYKRun("ohi autoi archontes didoasin tEn autEn elpida tois autois paisi"); //Maually checked
//CYKRun("oide didoasin tEn elpida tois paisi toisde"); //Manually tested
//CYKRun("ti ohi archontes didoasin"); //Manually tested
//CYKRun("didoasin tines tEn elpida tois paisi"); //Manually tested
//CYKRun("ouk esin ohi archontes sou de paides"); //LXX 2 Kings 19:7(simplified)
//CYKRun("pou estin oikos archontos"); //LXX Job 21:28 
//CYKRun("su monos ei archwn"); //LXX Is. 10:8
//CYKRun("alEthws egnwsan ohi archontes hoti outos estin ho christos"); //TODOtest
//CYKRun("ti to onoma sou estin"); //LXX Gen. 32:28 
//CYKRun("touto to rEma estin"); //LXX Ex. 16:23