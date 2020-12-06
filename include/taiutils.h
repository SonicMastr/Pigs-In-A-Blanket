#ifndef TAIUTILS_H
#define TAIUTILS_H

#include <psp2/kernel/modulemgr.h>

int module_get_offset(SceUID modid, int segidx, uint32_t offset, void *stub_out);
int module_get_export_func(const char *module_name, int32_t library_nid, uint32_t func_nid, void *func_dst);

#endif