#include "../include/hooks.h"

typedef struct sce_module_exports {
	uint16_t size;           // size of this structure; 0x20 for Vita 1.x
	uint8_t  lib_version[2]; //
	uint16_t attribute;      // ?
	uint16_t num_functions;  // number of exported functions
	uint16_t num_vars;       // number of exported variables
	uint16_t unk;
	uint32_t num_tls_vars;   // number of exported TLS variables?  <-- pretty sure wrong // yifanlu
	uint32_t lib_nid;        // NID of this specific export list; one PRX can export several names
	char     *lib_name;      // name of the export module
	uint32_t *nid_table;     // array of 32-bit NIDs for the exports, first functions then vars
	void     **entry_table;  // array of pointers to exported functions and then variables
} sce_module_exports_t;

int module_get_offset(SceUID modid, int segidx, uint32_t offset, void *stub_out){

	int res = 0;
	SceKernelModuleInfo info;

	if(segidx > 3){
		return -1;
	}

	if(stub_out == NULL){
		return -2;
	}

	res = sceKernelGetModuleInfo(modid, &info);
	if(res < 0){
		return res;
	}

	if(offset > info.segments[segidx].memsz){
		return -3;
	}

	*(uint32_t *)stub_out = (uint32_t)(info.segments[segidx].vaddr + offset);

	return 0;
}

int module_get_export_func(const char *module_name, int32_t library_nid, uint32_t func_nid, void *func_dst)
{

	tai_module_info_t tai_info;
	tai_info.size = sizeof(tai_info);

	if(taiGetModuleInfo(module_name, &tai_info) < 0){
		return -1;
	}

	uint32_t i = tai_info.exports_start;

	while (i < tai_info.exports_end) {

	  sce_module_exports_t *impInfo = (sce_module_exports_t *)(i);

	  uint32_t *func_nid_table = (uint32_t *)(impInfo->nid_table);
	  uint32_t *func_entry_table = (uint32_t *)(impInfo->entry_table);

	  if(library_nid == impInfo->lib_nid){

	    for(int j = 0; j < impInfo->num_functions; j++) {

	      if(func_nid_table[j] == func_nid){

	        *(uintptr_t *)(func_dst) = (uintptr_t)func_entry_table[j];

	        return 0;

	      }
	    }
	  }

	i += impInfo->size;
	}

	return -1;
}