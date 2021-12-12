/**
 * @file dma-mapping.h
 * @brief Allocate/free DMA memory
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 05.02.2018
 */

#ifndef LINUX_DMA_MAPPING_H_
#define LINUX_DMA_MAPPING_H_

#include <linux/types.h>

#include <stdint.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <assert.h>

#include <mem/sysmalloc.h>
#include <mem/vmem.h>

#ifdef __mips__
#include <asm/addrspace.h>
#endif

#ifdef __arm__
#include <drivers/dma/dma.h>
#include <drivers/mailbox/bcm2835_mailbox_property.h>
#endif

struct device;

/*
 * These definitions mirror those in pci.h, so they can be used
 * interchangeably with their PCI_ counterparts.
 */
enum dma_data_direction {
	DMA_BIDIRECTIONAL = 0,
	DMA_TO_DEVICE = 1,
	DMA_FROM_DEVICE = 2,
	DMA_NONE = 3,
};

/**
 * Allocate strong-ordered memory
 */
static inline void *dma_alloc_coherent(struct device *dev, size_t size,
		dma_addr_t *handle, gfp_t flag) {
#ifdef __arm__ /* use dma driver */ 
	Dma_mem_handle *mem = dma_malloc_opt(size, BCM2835_MEM_FLAG_COHERENT);
	*handle = (dma_addr_t)mem;
	return (void *)mem->bus_addr;
#else
	#ifndef NOMMU
		void *mem = sysmemalign(MMU_PAGE_SIZE, size);
		mmu_ctx_t ctx = vmem_current_context();
		int flags = PROT_WRITE | PROT_READ | PROT_NOCACHE;

		vmem_set_flags(ctx, (mmu_vaddr_t) mem, size, flags);
	#else
		void *mem = sysmemalign(sizeof(void *) * 4, size);
	#endif

	#ifdef __mips__
		*handle = (uintptr_t)mem - KSEG0;
	#else
		*handle = (uintptr_t)mem;
	#endif
#endif
	return mem;
}

static inline void dma_free_coherent(struct device *dev, size_t size,
		void *cpu_addr, dma_addr_t handle) {
#ifdef __arm__ /* use dma driver */ 
	dma_free((Dma_mem_handle *)handle);
#else
	sysfree(cpu_addr);
#endif
}

static inline dma_addr_t dma_map_single(struct device *dev, void *ptr, size_t size,
                           enum dma_data_direction direction) {
#ifdef __mips__
	return (uintptr_t)ptr - KSEG0;
#else
	return (uintptr_t)ptr;
#endif
}

static inline void dma_unmap_single(struct device *dev, dma_addr_t dma_handle, size_t size,
                       enum dma_data_direction direction) {
	return;
}

#endif /* LINUX_DMA_MAPPING_H_ */
