#include "pvr_regs.h"
#include "pvr_mem.h"
#include "Renderer_if.h"
#include "spg.h"
#include "ta.h"

extern bool pal_needs_update;
extern u32 _pal_rev_256[4];
extern u32 _pal_rev_16[64];
extern u32 pal_rev_256[4];
extern u32 pal_rev_16[64];
extern u32 palette_ram[1024];

bool fog_needs_update=true;

u8 pvr_regs[pvr_RegSize];

void pvr_WriteReg(u32 paddr,u32 data)
{
	u32 addr=paddr&pvr_RegMask;

   switch (addr)
   {
      case ID_addr: /* read-only */
      case REVISION_addr:
      case TA_YUV_TEX_CNT_addr:
         return;
      case STARTRENDER_addr:
         //start render
         rend_start_render();
         return;
      case TA_LIST_INIT_addr:
         if (data>>31)
         {
            ta_vtx_ListInit();
            data=0;
         }
         break;
      case SOFTRESET_addr:
         if (data!=0)
         {
            if (data & 1)
               ta_vtx_SoftReset();
            data=0;
         }
         break;
      case TA_LIST_CONT_addr:
         //a write of anything works ?
         ta_vtx_ListCont();
         break;
      case FB_R_CTRL_addr:
      case SPG_CONTROL_addr:
      case SPG_LOAD_addr:
         PvrReg(addr,u32)=data;
         CalculateSync();
         return;
      case TA_YUV_TEX_BASE_addr:
         YUV_init();
         break;
	}

	if (addr>=PALETTE_RAM_START_addr)
	{
		if (PvrReg(addr,u32)!=data)
		{
			u32 pal=(addr/4)&1023;

			pal_needs_update=true;
			_pal_rev_256[pal>>8]++;
			_pal_rev_16[pal>>4]++;
		}
	}

	if (  addr >= FOG_TABLE_START_addr && 
         addr <= FOG_TABLE_END_addr   &&
         PvrReg(addr,u32) != data)
		fog_needs_update=true;

	PvrReg(addr,u32)=data;
}

void Regs_Reset(bool Manual)
{
	ID                  = 0x17FD11DB;
	REVISION            = 0x00000011;
	SOFTRESET           = 0x00000007;
	SPG_HBLANK_INT.full = 0x031D0000;
	SPG_VBLANK_INT.full = 0x01500104;
	FPU_PARAM_CFG       = 0x0007DF77;
	HALF_OFFSET         = 0x00000007;
	ISP_FEED_CFG        = 0x00402000;
	SDRAM_REFRESH       = 0x00000020;
	SDRAM_ARB_CFG       = 0x0000001F;
	SDRAM_CFG           = 0x15F28997;
	SPG_HBLANK.full     = 0x007E0345;
	SPG_LOAD.full       = 0x01060359;
	SPG_VBLANK.full     = 0x01500104;
	SPG_WIDTH.full      = 0x07F1933F;
	VO_CONTROL.full     = 0x00000108;
	VO_STARTX.full      = 0x0000009D;
	VO_STARTY.full      = 0x00000015;
	SCALER_CTL.full     = 0x00000400;
	FB_BURSTCTRL        = 0x00090639;
	PT_ALPHA_REF        = 0x000000FF;
}
