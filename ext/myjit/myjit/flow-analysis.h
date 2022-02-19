/*
 * MyJIT
 * Copyright (C) 2010, 2015 Petr Krajca, <petr.krajca@upol.cz>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License aint64_t with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "set.h"
#define FLOW_UNCHANGED (0)
#define FLOW_CHANGED (1)
#define FLOW_INITIAL (2)

#define R_FP_POS (0)
#define R_OUT_POS (1)
#define R_IMM_POS (2)

#define R_FIMM_POS (0)

static inline void jit_flw_initialize(struct jit * jit)
{
	jit_op * op = jit_op_first(jit->ops);
	long icnt=3+jit_current_func_info(jit)->gp_reg_count+jit_current_func_info(jit)->general_arg_cnt; //First 3 are R_FP,R_OUT,R_IMM
	long fcnt=1+jit_current_func_info(jit)->fp_reg_count+jit_current_func_info(jit)->float_arg_cnt; //First is R_FIMM
	while (op) {
		op->live_in = jit_set_new();
		op->live_out = jit_set_new();
		op->bi_live_in=TD_CALLOC(sizeof(long),1+icnt/sizeof(long));
		op->bi_live_out=TD_CALLOC(sizeof(long),1+icnt/sizeof(long));
		op->bf_live_in=TD_CALLOC(sizeof(long),1+fcnt/sizeof(long));
		op->bf_live_out=TD_CALLOC(sizeof(long),1+fcnt/sizeof(long));
		op->bi_live_kill=TD_CALLOC(sizeof(long),1+icnt/sizeof(long));
		op->bf_live_kill=TD_CALLOC(sizeof(long),1+fcnt/sizeof(long));
		op = op->next;
	}
}
static inline void add_ibset_to_set(struct jit *jit,jit_set *set,long *bits,long bytes) {
    bytes/=8;
    if(bits[0]&(1<<R_FP_POS)) {
        jit_set_add(set,R_FP);
    } else if(bits[0]&(1<<R_OUT_POS)) {
        jit_set_add(set,R_OUT);
    } else if(bits[0]&(1<<R_IMM_POS)) {
        jit_set_add(set,R_IMM);
    }
    bits[0]&=~0b111l;
    while(--bytes>=0) {
        long find,idx;
        //find is 0 if none is found,other it is 1+idx
        while(find=__builtin_ffsl(bits[bytes])) {
              idx=find-1-3+bytes*sizeof(long)*8;
              if(idx>=jit_current_func_info(jit)->gp_reg_count)
                  jit_set_add(set, jit_mkreg(JIT_RTYPE_INT, JIT_RTYPE_ARG, idx-jit_current_func_info(jit)->gp_reg_count));
              else
                  jit_set_add(set,R(idx));
              bits[bytes]&=~(1l<<(find-1));
        }
    }
}
static inline void add_fbset_to_set(struct jit *jit,jit_set *set,long *bits,long bytes) {
    bytes/=8;
    if(bits[0]&(1<<R_FIMM_POS)) {
        jit_set_add(set,FR_IMM);
    }
    bits[0]&=~0b1l;
    while(--bytes>=0) {
        long find,idx;
        //find is 0 if none is found,other it is 1+idx
        while(find=__builtin_ffsl(bits[bytes])) {
              idx=find-1-1+bytes*sizeof(long)*8;
              if(idx>=jit_current_func_info(jit)->fp_reg_count)
                  jit_set_add(set, jit_mkreg(JIT_RTYPE_FLOAT, JIT_RTYPE_ARG, idx-jit_current_func_info(jit)->fp_reg_count));
              else
                  jit_set_add(set,FR(idx));
              bits[bytes]&=~(1l<<(find-1));
        }
    }
}
static inline void bset_merge(long *dst,long *src,long bytes) {
    bytes/=sizeof(long);
    while(--bytes>=0) {
        dst[bytes]|=src[bytes];
    }
}
static inline void bi_set_remove_reg(struct jit *jit,long *bits,jit_value v) {
    switch(v) {
    default:
    if(v==R_FP) {
        bits[0]&=~(1l<<R_FP_POS);
        break;
    }
    if(v==R_IMM) {
        bits[0]&=~(1l<<R_IMM_POS);
        break;
    }
    if(v==R_OUT) {
        bits[0]&=~(1l<<R_OUT_POS);
        break;
    }
    if(v==FR_IMM)
        break;
    if(!JIT_REG(v).type) {
        long id=JIT_REG(v).id+3;
        if(JIT_REG(v).spec==JIT_RTYPE_ARG) {
            id+=jit_current_func_info(jit)->gp_reg_count;
            bits[id/(sizeof(long)*8)]&=~(1l<<(id%(sizeof(long)*8)));
        } else
            bits[id/(sizeof(long)*8)]&=~(1l<<(id%(sizeof(long)*8)));
    }
    }
}
static inline void bi_set_add_reg(struct jit *jit,long *bits,jit_value v) {
    switch(v) {
    default:
    if(v==R_FP) {
        bits[0]|=(1l<<R_FP_POS);
        break;
    }
    if(v==R_IMM) {
        bits[0]|=(1l<<R_IMM_POS);
        break;
    }
    if(v==R_OUT) {
        bits[0]|=(1l<<R_OUT_POS);
        break;
    }
    if(v==FR_IMM)
        break;
    if(!JIT_REG(v).type) {
        long id=JIT_REG(v).id+3;
        if(JIT_REG(v).spec==JIT_RTYPE_ARG) {
            id+=jit_current_func_info(jit)->gp_reg_count;
            bits[id/(sizeof(long)*8)]|=(1l<<(id%(sizeof(long)*8)));
        } else
            bits[id/(sizeof(long)*8)]|=(1l<<(id%(sizeof(long)*8)));
    }
    }
}
static inline void bf_set_remove_reg(struct jit *jit,long *bits,jit_value v) {
    switch(v) {
    default:
    if(v==R_FP)
        break;
    if(v==R_IMM)
        break;
    if(v==R_OUT)
        break;
    if(v==FR_IMM) {
        bits[0]&=~1;
        break;
    }
    if(JIT_REG(v).type) {
            long id=JIT_REG(v).id+1;
            if(JIT_REG(v).spec==JIT_RTYPE_ARG) {
                id+=jit_current_func_info(jit)->fp_reg_count;
                bits[id/(sizeof(long)*8)]&=~(1l<<(id%(sizeof(long)*8)));
            } else
                bits[id/(sizeof(long)*8)]&=~(1l<<(id%(sizeof(long)*8)));
    }
    }
}
static inline void bf_set_add_reg(struct jit *jit,long *bits,jit_value v) {
    switch(v) {
        default:
    if(v==R_FP)
        break;
    if(v==R_IMM)
        break;
    if(v==R_OUT)
        break;
    if(v==FR_IMM) {
        bits[0]|=1;
        break;
    }
    if(JIT_REG(v).type) {
        long id=JIT_REG(v).id+1;
        if(JIT_REG(v).spec==JIT_RTYPE_ARG) {
            id+=jit_current_func_info(jit)->fp_reg_count;
            bits[id/(sizeof(long)*8)]|=(1l<<(id%(sizeof(long)*8)));
        } else
            bits[id/(sizeof(long)*8)]|=(1l<<(id%(sizeof(long)*8)));
    }
    }
}
static inline int bset_equal(long *s1,long *s2,long bytes) {
    bytes/=sizeof(long);
    while(--bytes>=0)
        if(s1[bytes]!=s2[bytes])
            return 0;
    return 1;
}
static inline int flw_analyze_op(struct jit * jit, jit_op * op, struct jit_func_info * func_info,int initialRun)
{
	long icnt=3+jit_current_func_info(jit)->gp_reg_count+jit_current_func_info(jit)->general_arg_cnt; //First 3 are R_FP,R_OUT,R_IMM
	long fcnt=1+jit_current_func_info(jit)->fp_reg_count+jit_current_func_info(jit)->float_arg_cnt; //First is R_FIMM
	long fbytes=(fcnt/sizeof(long)+1)*sizeof(long),ibytes=(icnt/sizeof(long)+1)*sizeof(long);
	if(initialRun||op->shadow_flow_analysis_state==FLOW_CHANGED) {
		op->shadow_flow_analysis_state=FLOW_CHANGED;
		goto possibleChange;
	}
	op->shadow_flow_analysis_state=FLOW_UNCHANGED;
	if(op->code == (JIT_JMP | IMM)&&op->jmp_addr)
		if(op->jmp_addr->flow_analysis_state==FLOW_CHANGED)
			goto possibleChange;
	if(op->next) if(op->next->flow_analysis_state==FLOW_CHANGED)
		goto possibleChange;
	if (op->code == (JIT_JMP | REG))
		goto possibleChange;
	if (op->jmp_addr && (GET_OP(op) != JIT_REF_CODE) && (GET_OP(op) != JIT_DATA_REF_CODE))
		if(op->jmp_addr->flow_analysis_state==FLOW_CHANGED) goto possibleChange;
	return 0;
	possibleChange:;
	int result;
	long *i_in1 = TD_CALLOC(1,ibytes);
	long *i_out1 = TD_CALLOC(1,ibytes);
    long *f_in1 = TD_CALLOC(1,fbytes);
	long *f_out1 = TD_CALLOC(1,fbytes);
	memcpy(i_in1,op->bi_live_in,ibytes); //YES
	memcpy(i_out1,op->bi_live_out,ibytes); //YES
	memcpy(f_in1,op->bf_live_in,fbytes); //YES
	memcpy(f_out1,op->bf_live_out,fbytes); //YES

	memcpy(op->bi_live_in,op->bi_live_out,ibytes); //YES
	memcpy(op->bf_live_in,op->bf_live_out,fbytes); //YES
	memset(op->bf_live_kill,0,fbytes);
    memset(op->bf_live_kill,0,fbytes);

	for (int i = 0; i < 3; i++) {
		if (ARG_TYPE(op, i + 1) == TREG) {
            bi_set_remove_reg(jit,op->bi_live_in,op->arg[i]); //YE
			bf_set_remove_reg(jit,op->bf_live_in,op->arg[i]); //YE
			bi_set_add_reg(jit,op->bi_live_kill,op->arg[i]); //YES
			bf_set_add_reg(jit,op->bf_live_kill,op->arg[i]); //YES
		}
	}

// FIXME: should be generic Yfor all architectures
// marks registers which are used to pass arguments
#ifdef JIT_ARCH_AMD64
	if (GET_OP(op) == JIT_PROLOG) {
		func_info = (struct jit_func_info *)op->arg[1];

		for (int i = 0; i < func_info->general_arg_cnt + func_info->float_arg_cnt; i++) {
			if (func_info->args[i].type == JIT_FLOAT_NUM) {
				bf_set_remove_reg(jit,op->bf_live_in,jit_mkreg(JIT_RTYPE_FLOAT, JIT_RTYPE_ARG, i)); //YES
				//bf_set_add_reg(jit,op->bf_live_kill,jit_mkreg(JIT_RTYPE_FLOAT, JIT_RTYPE_ARG, i));
			} else {
				bi_set_remove_reg(jit,op->bi_live_in,jit_mkreg(JIT_RTYPE_INT, JIT_RTYPE_ARG, i)); //YES
				//bi_set_add_reg(jit,op->bf_live_kill,jit_mkreg(JIT_RTYPE_INT, JIT_RTYPE_ARG, i));
			}
		}
/*
		int argcount = MIN(func_info->general_arg_cnt, jit->reg_al->gp_arg_reg_cnt);
		for (int j = 0; j < argcount; j++)
			jit_set_remove(op->live_in, jit_mkreg(JIT_RTYPE_INT, JIT_RTYPE_ARG, j));

		argcount = MIN(func_info->float_arg_cnt, jit->reg_al->fp_arg_reg_cnt);
		for (int j = 0; j < argcount; j++)
			jit_set_remove(op->live_in, jit_mkreg(JIT_RTYPE_FLOAT, JIT_RTYPE_ARG, j));
*/
	}
#endif

#if defined(JIT_ARCH_SPARC)
	if (GET_OP(op) == JIT_PROLOG) {
		func_info = (struct jit_func_info *)op->arg[1];
		int assoc_gp = 0;
		int argcount = func_info->general_arg_cnt + func_info->float_arg_cnt;
		for (i`nt j = 0; j < argcount; j++) {
			if (assoc_gp >= jit->reg_al->gp_arg_reg_cnt) break;
			if (func_info->args[j].type != JIT_FLOAT_NUM) {
				jit_set_remove(op->live_in, jit_mkreg(JIT_RTYPE_INT, JIT_RTYPE_ARG, j));
				assoc_gp++;
			} else {
				jit_set_remove(op->live_in, jit_mkreg(JIT_RTYPE_FLOAT, JIT_RTYPE_ARG, j));
				assoc_gp++;
				if (func_info->args[j].size == sizeof(double)) {
					jit_set_remove(op->live_in, jit_mkreg_ex(JIT_RTYPE_FLOAT, JIT_RTYPE_ARG, j));
					assoc_gp++;
				}
			}
		}
	}
#endif

	for (int i = 0; i < 3; i++)
		if (ARG_TYPE(op, i + 1) == REG) {
            bi_set_add_reg(jit,op->bi_live_in,op->arg[i]); //YES
            bf_set_add_reg(jit,op->bf_live_in,op->arg[i]); //YES
		}

#if defined(JIT_ARCH_AMD64) || defined(JIT_ARCH_SPARC)
	if (GET_OP(op) == JIT_GETARG) {
		int arg_id = op->arg[1];
		if (func_info->args[arg_id].type != JIT_FLOAT_NUM) {
			bi_set_add_reg(jit,op->bi_live_in,jit_mkreg(JIT_RTYPE_INT, JIT_RTYPE_ARG, arg_id)); //YES
		} else {
			bf_set_add_reg(jit,op->bf_live_in, jit_mkreg(JIT_RTYPE_FLOAT, JIT_RTYPE_ARG, arg_id)); //YES
		}
	}
#endif

	memset(op->bi_live_out,0,ibytes);
	memset(op->bf_live_out,0,fbytes);

	if ((GET_OP(op) == JIT_RET) || (GET_OP(op) == JIT_FRET)) {
		goto skip;
	}

	if ((op->code == (JIT_JMP | IMM)) && op->jmp_addr) { //YES
		memcpy(op->bi_live_out,op->jmp_addr->bi_live_in,ibytes); //YES
        memcpy(op->bf_live_out,op->jmp_addr->bf_live_in,fbytes); //YES
		goto skip;
	}

	if (op->code == (JIT_JMP | REG)||(GET_OP(op)==JIT_END_ASM_BLK)) { //YES
		jit_op *xop = func_info->first_op->next;
		while (xop && (GET_OP(xop) != JIT_PROLOG)) {
		  if ((GET_OP(xop) == JIT_TAINT_LABEL)||(GET_OP(xop) == JIT_REF_CODE) || (GET_OP(xop) == JIT_DATA_REF_CODE)||(GET_OP(xop)==JIT_DATA_CODE_OFFSET)) {
				bset_merge(op->bf_live_out, xop->jmp_addr->bf_live_in,fbytes);
				bset_merge(op->bi_live_out, xop->jmp_addr->bi_live_in,ibytes);
			}
			xop = xop->next;
		}
		goto skip;
	}
	if(op->next) {
        bset_merge(op->bi_live_out ,op->next->bi_live_in,ibytes); //YES
        bset_merge(op->bf_live_out ,op->next->bf_live_in,fbytes); //YES
	} else {
        //Already cleared
	}

	if (op->jmp_addr && (GET_OP(op) != JIT_REF_CODE) && (GET_OP(op) != JIT_DATA_REF_CODE)&&(GET_OP(op)!=JIT_DATA_CODE_OFFSET)&&(GET_OP(op) != JIT_TAINT_LABEL)) {
        bset_merge(op->bi_live_out ,op->jmp_addr->bi_live_in,ibytes); //YES
        bset_merge(op->bf_live_out ,op->jmp_addr->bf_live_in,fbytes); //YES
	}
skip:

	result = !(
            bset_equal(i_in1, op->bi_live_in,ibytes)
            &&bset_equal(i_out1, op->bi_live_out,ibytes)
            &&bset_equal(f_in1, op->bf_live_in,fbytes)
            &&bset_equal(f_out1, op->bf_live_out,fbytes));
	if(result||initialRun) op->flow_analysis_state=op->shadow_flow_analysis_state=FLOW_CHANGED;
	else op->shadow_flow_analysis_state=FLOW_UNCHANGED;
	TD_FREE(i_in1);
	TD_FREE(i_out1);
	TD_FREE(f_in1);
	TD_FREE(f_out1);
	return result;
}

static inline void jit_flw_analysis(struct jit * jit)
{
	jit_flw_initialize(jit);
	int changed;
	struct jit_func_info * func_info = NULL;
	int initialRun=1;
	//Expectes function info to be found
	struct jit_op *first=jit_op_first(jit->ops);
	for(;first;first=first->next) {
		if (GET_OP(first) == JIT_PROLOG)
			func_info = (struct jit_func_info *)first->arg[1];
		first->flow_analysis_state=FLOW_CHANGED;
		first->live_kill=jit_set_new();
	}
	do {
		changed = 0;
		//Do in "reverse" order
		jit_op * op = jit_op_last(jit->ops);
		while (op) {
			changed |= flw_analyze_op(jit, op, func_info,initialRun);
			op = op->prev;
		}
		op = jit_op_last(jit->ops);
		while (op) {
			op->flow_analysis_state=op->shadow_flow_analysis_state;
			op=op->prev;
		}
		initialRun=0;
	} while (changed);
	first=jit_op_first(jit->ops);
	long icnt=3+jit_current_func_info(jit)->gp_reg_count+jit_current_func_info(jit)->general_arg_cnt; //First 3 are R_FP,R_OUT,R_IMM
	long fcnt=1+jit_current_func_info(jit)->fp_reg_count+jit_current_func_info(jit)->float_arg_cnt; //First is R_FIMM
	long fbytes=fcnt/sizeof(long)+sizeof(long),ibytes=icnt/sizeof(long)+sizeof(long);
	for(;first;first=first->next) {
        add_ibset_to_set(jit,first->live_in,first->bi_live_in,ibytes);
        add_fbset_to_set(jit,first->live_in,first->bf_live_in,fbytes);
        add_ibset_to_set(jit,first->live_out,first->bi_live_out,ibytes);
        add_fbset_to_set(jit,first->live_out,first->bf_live_out,fbytes);
        add_ibset_to_set(jit,first->live_kill,first->bi_live_kill,ibytes);
        add_fbset_to_set(jit,first->live_kill,first->bf_live_kill,fbytes);
        JIT_FREE(first->bf_live_in);
        JIT_FREE(first->bf_live_out);
        JIT_FREE(first->bf_live_kill);
        JIT_FREE(first->bi_live_in);
        JIT_FREE(first->bi_live_out);
        JIT_FREE(first->bi_live_kill);
	}
}

static inline void mark_livecode(jit_op *op)
{
	while (op) {
		if (op->in_use) return;
		op->in_use = 1;
		if (op->jmp_addr) mark_livecode(op->jmp_addr);
		if (GET_OP(op) == JIT_RET) return;
		if (GET_OP(op) == JIT_FRET) return;
		if (GET_OP(op) == JIT_JMP) return;
		op = op->next;
	}
}


static void jit_dead_code_analysis(struct jit *jit, int remove_dead_code)
{
	for (jit_op *op = jit_op_first(jit->ops); op; op = op->next)
		op->in_use = 0;

	// marks ordinary operations
	for (jit_op *op = jit_op_first(jit->ops); op; op = op->next) {
		if (GET_OP(op) == JIT_PROLOG) mark_livecode(op);
		if (GET_OP(op) == JIT_DATA_REF_CODE) mark_livecode(op->jmp_addr);
        if (GET_OP(op) == JIT_DATA_CODE_OFFSET) mark_livecode(op->jmp_addr);
	}


	// marks directives
	for (jit_op *op = jit_op_first(jit->ops); op; op = op->next) {
		if (GET_OP(op) == JIT_CODESTART) op->in_use = 1;
		if (GET_OP(op) == JIT_DATA_BYTE) op->in_use = 1;
		if (GET_OP(op) == JIT_DATA_REF_CODE) op->in_use = 1;
		if (GET_OP(op) == JIT_DATA_REF_DATA) op->in_use = 1;
		if (GET_OP(op) == JIT_DATA_CODE_OFFSET) op->in_use = 1;
		if (GET_OP(op) == JIT_DUMP_PTR) op->in_use=1;
        if (GET_OP(op) == JIT_CODE_ALIGN) op->in_use = 1;
		if (GET_OP(op) == JIT_LABEL) op->in_use = 1;
		if (GET_OP(op) == JIT_PATCH) op->in_use = 1;
		if (GET_OP(op) == JIT_COMMENT) op->in_use = 1;
	}

	if (!remove_dead_code) return;

	jit_op *op = jit_op_first(jit->ops);

	while (op) {
		if (!op->in_use) {
			if (GET_OP(op) == JIT_FULL_SPILL) goto skip; /* only marks whether code is accessible or not */
			jit_op *next = op->next;
			jit_op_delete(op);
			op = next;
			continue;
		}
skip:
		op = op->next;
	}

}
