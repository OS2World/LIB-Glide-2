SST_CHIP_MASK .EQU 0x3c00

#----------------------------------------------------------------------
# Assembler offsets for SSTREGS struct
#----------------------------------------------------------------------

FVAX			 .EQU        136
FVAY			 .EQU        140
FVBX			 .EQU        144
FVBY			 .EQU        148
FVCX			 .EQU        152
FVCY			 .EQU        156
FTRIANGLECMD		 .EQU        256

FDPDX_OFFSET		 .EQU         32
FDPDY_OFFSET		 .EQU         64

#----------------------------------------------------------------------
# Assembler offsets for GC struct
#----------------------------------------------------------------------

base_ptr		 .EQU          0
reg_ptr			 .EQU          4
lfb_ptr			 .EQU         12
cull_mode		 .EQU        400
dataList		 .EQU         24
fifoFree		 .EQU        408
SIZEOF_GrState		 .EQU        256
SIZEOF_GrHwConfiguration	 .EQU        148
SIZEOF_GC		 .EQU     201556


#----------------------------------------------------------------------
# Assembler offsets for GlideRoot struct
#----------------------------------------------------------------------

p6Fencer		 .EQU          0
current_sst		 .EQU          4
CPUType			 .EQU          8
curGC			 .EQU         12
curTriSize		 .EQU         16
trisProcessed		 .EQU        124
trisDrawn		 .EQU        128
SIZEOF_GlideRoot	 .EQU     806520


#----------------------------------------------------------------------
# Assembler offsets for dataList struct
#----------------------------------------------------------------------

dl_i			 .EQU          0
dl_addr			 .EQU          4
SIZEOF_dataList		 .EQU          8

