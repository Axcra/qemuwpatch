/*
 * pcie_sriov.h:
 *
 * Implementation of SR/IOV emulation support.
 *
 * Copyright (c) 2015 Knut Omang <knut.omang@oracle.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 *
 */

#ifndef QEMU_PCIE_SRIOV_H
#define QEMU_PCIE_SRIOV_H

#include "hw/pci/pci.h"

typedef struct PCIESriovPF {
    uint8_t vf_bar_type[PCI_NUM_REGIONS];   /* Store type for each VF bar */
    PCIDevice **vf;     /* Pointer to an array of num_vfs VF devices */
    bool vf_user_created; /* If VFs are created by user */
} PCIESriovPF;

typedef struct PCIESriovVF {
    PCIDevice *pf;      /* Pointer back to owner physical function */
    uint16_t vf_number; /* Logical VF number of this function */
} PCIESriovVF;

bool pcie_sriov_pf_init(PCIDevice *dev, uint16_t offset,
                        const char *vfname, uint16_t vf_dev_id,
                        uint16_t init_vfs, uint16_t total_vfs,
                        uint16_t vf_offset, uint16_t vf_stride,
                        Error **errp);
void pcie_sriov_pf_exit(PCIDevice *dev);

/* Set up a VF bar in the SR/IOV bar area */
void pcie_sriov_pf_init_vf_bar(PCIDevice *dev, int region_num,
                               uint8_t type, dma_addr_t size);

/* Instantiate a bar for a VF */
void pcie_sriov_vf_register_bar(PCIDevice *dev, int region_num,
                                MemoryRegion *memory);

/**
 * pcie_sriov_pf_init_from_user_created_vfs() - Initialize PF with user-created
 *                                              VFs, adding ARI to PF
 * @dev: A PCIe device being realized.
 * @offset: The offset of the SR-IOV capability.
 * @errp: pointer to Error*, to store an error if it happens.
 *
 * Initializes a PF with user-created VFs, adding the ARI extended capability to
 * the PF. The VFs should call pcie_ari_init() to form an ARI device.
 *
 * Return: The size of added capabilities. 0 if the user did not create VFs.
 *         -1 if failed.
 */
int16_t pcie_sriov_pf_init_from_user_created_vfs(PCIDevice *dev,
                                                 uint16_t offset,
                                                 Error **errp);

bool pcie_sriov_register_device(PCIDevice *dev, Error **errp);
void pcie_sriov_unregister_device(PCIDevice *dev);

/*
 * Default (minimal) page size support values
 * as required by the SR/IOV standard:
 * 0x553 << 12 = 0x553000 = 4K + 8K + 64K + 256K + 1M + 4M
 */
#define SRIOV_SUP_PGSIZE_MINREQ 0x553

/*
 * Optionally add supported page sizes to the mask of supported page sizes
 * Page size values are interpreted as opt_sup_pgsize << 12.
 */
void pcie_sriov_pf_add_sup_pgsize(PCIDevice *dev, uint16_t opt_sup_pgsize);

/* SR/IOV capability config write handler */
void pcie_sriov_config_write(PCIDevice *dev, uint32_t address,
                             uint32_t val, int len);

void pcie_sriov_pf_post_load(PCIDevice *dev);

/* Reset SR/IOV */
void pcie_sriov_pf_reset(PCIDevice *dev);

/* Get logical VF number of a VF - only valid for VFs */
uint16_t pcie_sriov_vf_number(PCIDevice *dev);

/*
 * Get the physical function that owns this VF.
 * Returns NULL if dev is not a virtual function
 */
PCIDevice *pcie_sriov_get_pf(PCIDevice *dev);

/*
 * Get the n-th VF of this physical function - only valid for PF.
 * Returns NULL if index is invalid
 */
PCIDevice *pcie_sriov_get_vf_at_index(PCIDevice *dev, int n);

/* Returns the current number of virtual functions. */
uint16_t pcie_sriov_num_vfs(PCIDevice *dev);

#endif /* QEMU_PCIE_SRIOV_H */
