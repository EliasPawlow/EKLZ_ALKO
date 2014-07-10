/*###ICF### Section handled by ICF editor, don't touch! ****/
/*-Editor annotation file-*/
/* IcfEditorFile="$TOOLKIT_DIR$\config\ide\IcfEditor\cortex_v1_0.xml" */
/*-Specials-*/
define symbol __ICFEDIT_intvec_start__ = 0x00000000;
/*-Memory Regions-*/
define symbol __ICFEDIT_region_ROM1_start__   = 0x00000000;
define symbol __ICFEDIT_region_ROM1_end__     = 0x00000FFF;
define symbol __ICFEDIT_region_ROM2_start__   = 0x00002000;
define symbol __ICFEDIT_region_ROM2_end__     = 0x0007FFFF;
define symbol __ICFEDIT_region_RAM_start__   = 0x10000000;
define symbol __ICFEDIT_region_RAM_end__     = 0x10007FDF; // Оставим 32 байта
/*-Sizes-*/
define symbol __ICFEDIT_size_cstack__   = 0x500;
define symbol __ICFEDIT_size_heap__     = 0x200;
/**** End of ICF editor section. ###ICF###*/

define memory mem with size = 4G;
define region ROM_region   = mem:[from __ICFEDIT_region_ROM1_start__   to __ICFEDIT_region_ROM1_end__]
			    | mem:[from __ICFEDIT_region_ROM2_start__   to __ICFEDIT_region_ROM2_end__];
define region RAM_region   = mem:[from __ICFEDIT_region_RAM_start__   to __ICFEDIT_region_RAM_end__];

define symbol _AHB_RAM_start__  = 0x2007C000;
define symbol _AHB_RAM_end__    = 0x20083FFF;
define region AHB_RAM_region = mem:[from _AHB_RAM_start__ to _AHB_RAM_end__];

define block CSTACK    with alignment = 8, size = __ICFEDIT_size_cstack__   { };
define block HEAP      with alignment = 8, size = __ICFEDIT_size_heap__     { };

initialize by copy with packing = zeros { readwrite };
do not initialize  { section .noinit };

place at address mem:__ICFEDIT_intvec_start__ { section .intvec };
place at address mem:0x2FC { section CRPKEY };
place in ROM_region     { readonly };
place in RAM_region     { readwrite,
                          block CSTACK, block HEAP };
place in AHB_RAM_region { section USB_RAM };
