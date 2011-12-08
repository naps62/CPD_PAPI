/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/

/* 
* File:    map-p6-M.h
* CVS:     $Id: map-p6-m.h,v 1.5 2010/04/15 18:32:42 bsheely Exp $
* Author:  Harald Servat
*          redcrash@gmail.com
*/

#ifndef FreeBSD_MAP_P6_M
#define FreeBSD_MAP_P6_M

enum NativeEvent_Value_P6_M_Processor {
	/* P6 common events */
	PNE_P6_M_BACLEARS = PAPI_NATIVE_MASK,
	PNE_P6_M_BR_BOGUS,
	PNE_P6_M_BR_INST_DECODED,
	PNE_P6_M_BR_INST_RETIRED,
	PNE_P6_M_BR_MISS_PRED_RETIRED,
	PNE_P6_M_BR_MISS_PRED_TAKEN_RET,
	PNE_P6_M_BR_TAKEN_RETIRED,
	PNE_P6_M_BTB_MISSES,
	PNE_P6_M_BUS_BNR_DRV,
	PNE_P6_M_BUS_DATA_RCV,
	PNE_P6_M_BUS_DRDY_CLOCKS,
	PNE_P6_M_BUS_HIT_DRV,
	PNE_P6_M_BUS_HITM_DRV,
	PNE_P6_M_BUS_LOCK_CLOCKS,
	PNE_P6_M_BUS_REQ_OUTSTANDING,
	PNE_P6_M_BUS_SNOOP_STALL,
	PNE_P6_M_BUS_TRAN_ANY,
	PNE_P6_M_BUS_TRAN_BRD,
	PNE_P6_M_BUS_TRAN_BURST,
	PNE_P6_M_BUS_TRAN_DEF,
	PNE_P6_M_BUS_TRAN_IFETCH,
	PNE_P6_M_BUS_TRAN_INVAL,
	PNE_P6_M_BUS_TRAN_MEM,
	PNE_P6_M_BUS_TRAN_POWER,
	PNE_P6_M_BUS_TRAN_RFO,
	PNE_P6_M_BUS_TRANS_IO,
	PNE_P6_M_BUS_TRANS_P,
	PNE_P6_M_BUS_TRANS_WB,
	PNE_P6_M_CPU_CLK_UNHALTED,
	PNE_P6_M_CYCLES_DIV_BUSY,
	PNE_P6_M_CYCLES_IN_PENDING_AND_MASKED,
	PNE_P6_M_CYCLES_INT_MASKED,
	PNE_P6_M_DATA_MEM_REFS,
	PNE_P6_M_DCU_LINES_IN,
	PNE_P6_M_DCU_M_LINES_IN,
	PNE_P6_M_DCU_M_LINES_OUT,
	PNE_P6_M_DCU_MISS_OUTSTANDING,
	PNE_P6_M_DIV,
	PNE_P6_M_FLOPS,
	PNE_P6_M_FP_ASSIST,
	PNE_P6_M_FTP_COMPS_OPS_EXE,
	PNE_P6_M_HW_INT_RX,
	PNE_P6_M_IFU_FETCH,
	PNE_P6_M_IFU_FETCH_MISS,
	PNE_P6_M_IFU_MEM_STALL,
	PNE_P6_M_ILD_STALL,
	PNE_P6_M_INST_DECODED,
	PNE_P6_M_INST_RETIRED,
	PNE_P6_M_ITLB_MISS,
	PNE_P6_M_L2_ADS,
	PNE_P6_M_L2_DBUS_BUSY,
	PNE_P6_M_L2_DBUS_BUSY_RD,
	PNE_P6_M_L2_IFETCH,
	PNE_P6_M_L2_LD,
	PNE_P6_M_L2_LINES_IN,
	PNE_P6_M_L2_LINES_OUT,
	PNE_P6_M_L2M_LINES_INM,
	PNE_P6_M_L2M_LINES_OUTM,
	PNE_P6_M_L2_RQSTS,
	PNE_P6_M_L2_ST,
	PNE_P6_M_LD_BLOCKS,
	PNE_P6_M_MISALIGN_MEM_REF,
	PNE_P6_M_MUL,
	PNE_P6_M_PARTIAL_RAT_STALLS,
	PNE_P6_M_RESOURCE_STALL,
	PNE_P6_M_SB_DRAINS,
	PNE_P6_M_SEGMENT_REG_LOADS,
	PNE_P6_M_UOPS_RETIRED,
	/* Pentium 3 specific events */
	PNE_P6_M_FP_MMX_TRANS,
	PNE_P6_M_MMX_ASSIST,
	PNE_P6_M_MMX_INSTR_EXEC,
	PNE_P6_M_MMX_INSTR_RET,
	PNE_P6_M_MMX_SAT_INSTR_EXEC,
	PNE_P6_M_MMX_UOPS_EXEC,
	PNE_P6_M_RET_SEG_RENAMES,
	PNE_P6_M_SEG_RENAME_STALLS,
	PNE_P6_M_EMON_KNI_COMP_INST_RET,
	PNE_P6_M_EMON_KNI_INST_RETIRED,
	PNE_P6_M_EMON_KNI_PREF_DISPATCHED,
	PNE_P6_M_EMON_KNI_PREF_MISS,
	/* Pentium M specific events */
	PNE_P6_M_BR_BAC_MISSP_EXEC,
	PNE_P6_M_BR_CALL_EXEC,
	PNE_P6_M_BR_CALL_MISSP_EXEC,
	PNE_P6_M_BR_CND_EXEC,
	PNE_P6_M_BR_CND_MISSP_EXEC,
	PNE_P6_M_BR_IND_CALL_EXEC,
	PNE_P6_M_BR_IND_EXEC,
	PNE_P6_M_BR_IND_MISSP_EXEC,
	PNE_P6_M_BR_INST_EXEC,
	PNE_P6_M_BR_MISSP_EXEC,
	PNE_P6_M_BR_RET_BAC_MISSP_EXEC,
	PNE_P6_M_BR_RET_EXEC,
	PNE_P6_M_BR_RET_MISSP_EXEC,
	PNE_P6_M_EMON_ESP_UOPS,
	PNE_P6_M_EMON_EST_TRANS,
	PNE_P6_M_EMON_FUSED_UOPS_RET,
	PNE_P6_M_EMON_PREF_RQSTS_DN,
	PNE_P6_M_EMON_PREF_RQSTS_UP,
	PNE_P6_M_EMON_SIMD_INSTR_RETIRD,
	PNE_P6_M_EMON_SSE_SSE2_COMP_INST_RETIRED,
	PNE_P6_M_EMON_SSE_SSE2_INST_RETIRED,
	PNE_P6_M_EMON_SYNCH_UOPS,
	PNE_P6_M_EMON_THERMAL_TRIP,
	PNE_P6_M_EMON_UNFUSION,
	PNE_P6_M_NATNAME_GUARD
};

extern Native_Event_LabelDescription_t P6_M_Processor_info[];
extern hwi_search_t P6_M_Processor_map[];

#endif