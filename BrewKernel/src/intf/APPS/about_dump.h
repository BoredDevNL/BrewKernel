/*
 * Brew Kernel
 * Copyright (C) 2025-2026 boreddevnl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef APPS_ABOUT_DUMP_H
#define APPS_ABOUT_DUMP_H

#include <stdint.h>
#include <stddef.h>
#include "../filesys.h"
#include "../print.h"
#include "../pci.h"
#include "../memory.h"

// Helper function to convert integer to string
static void int_to_str(int num, char* buffer, int base) {
    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    
    int is_negative = 0;
    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }
    
    char temp[32];
    int i = 0;
    while (num > 0) {
        int digit = num % base;
        if (digit < 10) {
            temp[i++] = '0' + digit;
        } else {
            temp[i++] = 'A' + (digit - 10);
        }
        num /= base;
    }
    
    if (is_negative) {
        temp[i++] = '-';
    }
    
    // Reverse the string
    int j = 0;
    for (int k = i - 1; k >= 0; k--) {
        buffer[j++] = temp[k];
    }
    buffer[j] = '\0';
}

// Helper to format hex values
static void uint32_to_hex_str(uint32_t num, char* buffer) {
    buffer[0] = '0';
    buffer[1] = 'x';
    int_to_str(num, buffer + 2, 16);
}

// Helper to format hex with padding
static void uint16_to_hex_str(uint16_t num, char* buffer) {
    const char* hex = "0123456789ABCDEF";
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[2] = hex[(num >> 12) & 0xF];
    buffer[3] = hex[(num >> 8) & 0xF];
    buffer[4] = hex[(num >> 4) & 0xF];
    buffer[5] = hex[num & 0xF];
    buffer[6] = '\0';
}

// CPU info gathering function with detailed CPUID information
static void get_cpu_info(char* buffer, size_t buffer_size) {
    size_t pos = 0;
    
    // Get CPUID information
    uint32_t eax, ebx, ecx, edx;
    
    // CPUID function 0x00 - Vendor ID and max leaf
    __asm__ __volatile__(
        "cpuid"
        : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
        : "a" (0)
    );
    
    uint32_t max_leaf = eax;
    char vendor[13];
    *((uint32_t*)(vendor + 0)) = ebx;
    *((uint32_t*)(vendor + 4)) = edx;
    *((uint32_t*)(vendor + 8)) = ecx;
    vendor[12] = '\0';
    
    // CPUID function 0x80000000 - Extended leaf info
    __asm__ __volatile__(
        "cpuid"
        : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
        : "a" (0x80000000)
    );
    uint32_t max_ext_leaf = eax;
    
    // CPUID function 0x01 - Features and stepping
    __asm__ __volatile__(
        "cpuid"
        : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
        : "a" (1)
    );
    
    uint32_t family = (eax >> 8) & 0xF;
    uint32_t model = (eax >> 4) & 0xF;
    uint32_t stepping = eax & 0xF;
    uint32_t features_edx = edx;
    uint32_t features_ecx = ecx;
    
    // Check for feature flags
    uint32_t has_pse = (features_edx >> 3) & 1;   // Page Size Extension
    uint32_t has_apic = (features_edx >> 9) & 1;  // APIC
    uint32_t has_msr = (features_edx >> 5) & 1;   // Model Specific Register
    uint32_t has_pae = (features_edx >> 6) & 1;   // Physical Address Extension
    
    // CPUID function 0x80000001 - Extended features
    uint32_t ext_features_edx = 0;
    if (max_ext_leaf >= 0x80000001) {
        __asm__ __volatile__(
            "cpuid"
            : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
            : "a" (0x80000001)
        );
        ext_features_edx = edx;
    }
    uint32_t has_pfu = (ext_features_edx >> 20) & 1; // Page Flush Unit
    
    // CPUID function 0x0B - Core and thread count (if available)
    uint32_t core_count = 1, thread_count = 1, smt_count = 1;
    if (max_leaf >= 0x0B) {
        __asm__ __volatile__(
            "cpuid"
            : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
            : "a" (0x0B), "c" (0)
        );
        thread_count = ebx & 0xFFFF;
        
        if (thread_count > 0) smt_count = thread_count;
        
        __asm__ __volatile__(
            "cpuid"
            : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
            : "a" (0x0B), "c" (1)
        );
        core_count = ebx & 0xFFFF;
    }
    
    // CPUID function 0x80000008 - Physical and virtual address sizes
    uint32_t phys_addr_bits = 32, virt_addr_bits = 32;
    if (max_ext_leaf >= 0x80000008) {
        __asm__ __volatile__(
            "cpuid"
            : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
            : "a" (0x80000008)
        );
        phys_addr_bits = eax & 0xFF;
        virt_addr_bits = (eax >> 8) & 0xFF;
    }
    
    // Build the CPU info string
    const char* cpu_header = "CPU Information\n";
    size_t len = 0;
    while (cpu_header[len] && pos < buffer_size - 1) {
        buffer[pos++] = cpu_header[len++];
    }
    
    const char* vendor_prefix = "Vendor ID: ";
    len = 0;
    while (vendor_prefix[len] && pos < buffer_size - 1) {
        buffer[pos++] = vendor_prefix[len++];
    }
    len = 0;
    while (vendor[len] && pos < buffer_size - 1) {
        buffer[pos++] = vendor[len++];
    }
    
    const char* arch_str = "\nArchitecture: x86_64\n";
    len = 0;
    while (arch_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = arch_str[len++];
    }
    
    const char* family_str = "Family: ";
    len = 0;
    while (family_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = family_str[len++];
    }
    char num_str[32];
    int_to_str(family, num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* model_str = "\nModel: ";
    len = 0;
    while (model_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = model_str[len++];
    }
    int_to_str(model, num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* stepping_str = "\nStepping: ";
    len = 0;
    while (stepping_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = stepping_str[len++];
    }
    int_to_str(stepping, num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* max_leaf_str = "\nMax CPUID Leaf: 0x";
    len = 0;
    while (max_leaf_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = max_leaf_str[len++];
    }
    int_to_str(max_leaf, num_str, 16);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* max_ext_leaf_str = "\nMax Extended CPUID Leaf: 0x";
    len = 0;
    while (max_ext_leaf_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = max_ext_leaf_str[len++];
    }
    int_to_str(max_ext_leaf, num_str, 16);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* core_str = "\nCore Count: ";
    len = 0;
    while (core_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = core_str[len++];
    }
    int_to_str(core_count, num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* thread_str = "\nThread Count: ";
    len = 0;
    while (thread_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = thread_str[len++];
    }
    int_to_str(thread_count, num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* smt_str = "\nSMT Count: ";
    len = 0;
    while (smt_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = smt_str[len++];
    }
    int_to_str(smt_count, num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* phys_str = "\nMax Physical Address Bits: ";
    len = 0;
    while (phys_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = phys_str[len++];
    }
    int_to_str(phys_addr_bits, num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* virt_str = "\nMax Virtual Address Bits: ";
    len = 0;
    while (virt_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = virt_str[len++];
    }
    int_to_str(virt_addr_bits, num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* features_str = "\nFeatures:\n";
    len = 0;
    while (features_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = features_str[len++];
    }
    
    const char* pfu_str = has_pfu ? "  PFU (Page Flush Unit): Yes\n" : "  PFU (Page Flush Unit): No\n";
    len = 0;
    while (pfu_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = pfu_str[len++];
    }
    
    const char* pse_str = has_pse ? "  PSE (Page Size Extension): Yes\n" : "  PSE (Page Size Extension): No\n";
    len = 0;
    while (pse_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = pse_str[len++];
    }
    
    const char* msr_str = has_msr ? "  MSR (Model Specific Register): Yes\n" : "  MSR (Model Specific Register): No\n";
    len = 0;
    while (msr_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = msr_str[len++];
    }
    
    const char* pae_str = has_pae ? "  PAE (Physical Address Extension): Yes\n" : "  PAE (Physical Address Extension): No\n";
    len = 0;
    while (pae_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = pae_str[len++];
    }
    
    const char* apic_str = has_apic ? "  APIC (Advanced Programmable Interrupt Controller): Yes\n" : "  APIC (Advanced Programmable Interrupt Controller): No\n";
    len = 0;
    while (apic_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = apic_str[len++];
    }
    
    const char* newline = "\n";
    len = 0;
    while (newline[len] && pos < buffer_size - 1) {
        buffer[pos++] = newline[len++];
    }
    
    buffer[pos] = '\0';
}

// PCI device information gathering
static void get_pci_info(char* buffer, size_t buffer_size) {
    size_t pos = 0;
    
    const char* pci_header = "PCI Devices\n";
    size_t len = 0;
    while (pci_header[len] && pos < buffer_size - 1) {
        buffer[pos++] = pci_header[len++];
    }
    
    // Enumerate PCI devices
    pci_device_t devices[32];
    int num_devices = pci_enumerate_devices(devices, 32);
    
    if (num_devices == 0) {
        const char* no_devices = "No PCI devices found\n\n";
        len = 0;
        while (no_devices[len] && pos < buffer_size - 1) {
            buffer[pos++] = no_devices[len++];
        }
    } else {
        for (int i = 0; i < num_devices && pos < buffer_size - 100; i++) {
            const char* device_info = "Bus: ";
            len = 0;
            while (device_info[len] && pos < buffer_size - 1) {
                buffer[pos++] = device_info[len++];
            }
            
            char hex_str[16];
            int_to_str(devices[i].bus, hex_str, 16);
            len = 0;
            while (hex_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = hex_str[len++];
            }
            
            const char* dev_str = " Device: ";
            len = 0;
            while (dev_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = dev_str[len++];
            }
            
            int_to_str(devices[i].device, hex_str, 16);
            len = 0;
            while (hex_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = hex_str[len++];
            }
            
            const char* func_str = " Function: ";
            len = 0;
            while (func_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = func_str[len++];
            }
            
            int_to_str(devices[i].function, hex_str, 16);
            len = 0;
            while (hex_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = hex_str[len++];
            }
            
            const char* vendor_str = " Vendor ID: ";
            len = 0;
            while (vendor_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = vendor_str[len++];
            }
            
            uint16_to_hex_str(devices[i].vendor_id, hex_str);
            len = 0;
            while (hex_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = hex_str[len++];
            }
            
            const char* dev_id_str = " Device ID: ";
            len = 0;
            while (dev_id_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = dev_id_str[len++];
            }
            
            uint16_to_hex_str(devices[i].device_id, hex_str);
            len = 0;
            while (hex_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = hex_str[len++];
            }
            
            const char* class_str = " Class: 0x";
            len = 0;
            while (class_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = class_str[len++];
            }
            
            int_to_str(devices[i].class_code, hex_str, 16);
            len = 0;
            while (hex_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = hex_str[len++];
            }
            
            const char* comma_newline = "\n";
            len = 0;
            while (comma_newline[len] && pos < buffer_size - 1) {
                buffer[pos++] = comma_newline[len++];
            }
        }
        
        const char* end_pci = "\n";
        len = 0;
        while (end_pci[len] && pos < buffer_size - 1) {
            buffer[pos++] = end_pci[len++];
        }
    }
    
    // Null terminate the buffer
    buffer[pos] = '\0';
}

// Memory information with temp read buffer details
static void get_memory_info(char* buffer, size_t buffer_size) {
    size_t pos = 0;
    
    const char* mem_header = "Memory Information\n";
    size_t len = 0;
    while (mem_header[len] && pos < buffer_size - 1) {
        buffer[pos++] = mem_header[len++];
    }
    
    size_t total_ram = sys_get_total_ram();
    size_t used_ram = sys_get_used_ram();
    size_t free_ram = sys_get_free_ram();
    size_t total_fs_mem = fs_get_total_memory();
    size_t used_fs_mem = fs_get_used_memory();
    size_t free_fs_mem = fs_get_free_memory();
    
    const char* total_str = "Total RAM: ";
    len = 0;
    while (total_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = total_str[len++];
    }
    
    char num_str[32];
    int_to_str(total_ram / (1024 * 1024), num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* mb_str = " MB\n";
    len = 0;
    while (mb_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = mb_str[len++];
    }
    
    const char* used_str = "Used RAM: ";
    len = 0;
    while (used_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = used_str[len++];
    }
    
    int_to_str(used_ram / (1024 * 1024), num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* mb_str2 = " MB\n";
    len = 0;
    while (mb_str2[len] && pos < buffer_size - 1) {
        buffer[pos++] = mb_str2[len++];
    }
    
    const char* free_str = "Free RAM: ";
    len = 0;
    while (free_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = free_str[len++];
    }
    
    int_to_str(free_ram / (1024 * 1024), num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* mb_str3 = " MB\n";
    len = 0;
    while (mb_str3[len] && pos < buffer_size - 1) {
        buffer[pos++] = mb_str3[len++];
    }
    
    const char* temp_buf_header = "Temp Read Buffer (File System Memory Pool)\n";
    len = 0;
    while (temp_buf_header[len] && pos < buffer_size - 1) {
        buffer[pos++] = temp_buf_header[len++];
    }
    
    const char* total_fs_str = "Total Buffer: ";
    len = 0;
    while (total_fs_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = total_fs_str[len++];
    }
    
    int_to_str(total_fs_mem / (1024 * 1024), num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* mb_fs = " MB\n";
    len = 0;
    while (mb_fs[len] && pos < buffer_size - 1) {
        buffer[pos++] = mb_fs[len++];
    }
    
    const char* used_fs_str = "Used Buffer: ";
    len = 0;
    while (used_fs_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = used_fs_str[len++];
    }
    
    int_to_str(used_fs_mem / (1024 * 1024), num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* mb_fs2 = " MB\n";
    len = 0;
    while (mb_fs2[len] && pos < buffer_size - 1) {
        buffer[pos++] = mb_fs2[len++];
    }
    
    const char* free_fs_str = "Free Buffer: ";
    len = 0;
    while (free_fs_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = free_fs_str[len++];
    }
    
    int_to_str(free_fs_mem / (1024 * 1024), num_str, 10);
    len = 0;
    while (num_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = num_str[len++];
    }
    
    const char* mb_fs3 = " MB\n\n";
    len = 0;
    while (mb_fs3[len] && pos < buffer_size - 1) {
        buffer[pos++] = mb_fs3[len++];
    }
    
    buffer[pos] = '\0';
}

// BIOS data with detailed information
static void get_bios_data(char* buffer, size_t buffer_size) {
    size_t pos = 0;
    
    const char* bios_header = "BIOS Information\n";
    size_t len = 0;
    while (bios_header[len] && pos < buffer_size - 1) {
        buffer[pos++] = bios_header[len++];
    }
    
    // Read BIOS memory location (0xFFFF0 contains the BIOS entry point)
    uint32_t* bios_ptr = (uint32_t*)0xFFFF0;
    uint32_t bios_value = *bios_ptr;
    
    const char* bios_entry = "BIOS Entry Point: 0x";
    len = 0;
    while (bios_entry[len] && pos < buffer_size - 1) {
        buffer[pos++] = bios_entry[len++];
    }
    
    char hex_str[32];
    int_to_str(bios_value, hex_str, 16);
    len = 0;
    while (hex_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = hex_str[len++];
    }
    
    // Read BIOS signature at 0xFFFF5
    uint8_t* sig_ptr = (uint8_t*)0xFFFF5;
    const char* sig_str = "\nBIOS Signature: ";
    len = 0;
    while (sig_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = sig_str[len++];
    }
    
    for (int i = 0; i < 4 && pos < buffer_size - 1; i++) {
        if (sig_ptr[i] >= 32 && sig_ptr[i] < 127) {
            buffer[pos++] = sig_ptr[i];
        } else {
            buffer[pos++] = '.';
        }
    }
    
    // Read OEM ID from RSDP (Revision, OEM ID at 0x00009E00 area, but this varies)
    const char* oem_str = "\nOEM ID (from RSDP): ";
    len = 0;
    while (oem_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = oem_str[len++];
    }
    
    // Search for RSDP signature
    uint8_t* search_ptr = (uint8_t*)0x9E000;
    int found_rsdp = 0;
    for (uint32_t addr = 0x9E000; addr < 0x9FFFF && !found_rsdp; addr += 16) {
        uint8_t* test_ptr = (uint8_t*)addr;
        if (test_ptr[0] == 'R' && test_ptr[1] == 'S' && test_ptr[2] == 'D' && test_ptr[3] == 'P') {
            for (int i = 0; i < 6 && pos < buffer_size - 1; i++) {
                if (test_ptr[9 + i] >= 32 && test_ptr[9 + i] < 127) {
                    buffer[pos++] = test_ptr[9 + i];
                } else {
                    buffer[pos++] = '.';
                }
            }
            found_rsdp = 1;
        }
    }
    if (!found_rsdp) {
        const char* not_found = "Not Found";
        len = 0;
        while (not_found[len] && pos < buffer_size - 1) {
            buffer[pos++] = not_found[len++];
        }
    }
    
    // Revision
    const char* rev_str = "\nACPI Revision: ";
    len = 0;
    while (rev_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = rev_str[len++];
    }
    
    search_ptr = (uint8_t*)0x9E000;
    found_rsdp = 0;
    char num_str[16];
    for (uint32_t addr = 0x9E000; addr < 0x9FFFF && !found_rsdp; addr += 16) {
        uint8_t* test_ptr = (uint8_t*)addr;
        if (test_ptr[0] == 'R' && test_ptr[1] == 'S' && test_ptr[2] == 'D' && test_ptr[3] == 'P') {
            int_to_str(test_ptr[15], num_str, 10);
            len = 0;
            while (num_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = num_str[len++];
            }
            found_rsdp = 1;
        }
    }
    if (!found_rsdp) {
        const char* unknown = "Unknown";
        len = 0;
        while (unknown[len] && pos < buffer_size - 1) {
            buffer[pos++] = unknown[len++];
        }
    }
    
    // COM ports (serial ports)
    const char* com_header = "\n\nSerial Ports (COM Ports):\n";
    len = 0;
    while (com_header[len] && pos < buffer_size - 1) {
        buffer[pos++] = com_header[len++];
    }
    
    // Read from BIOS data area (0x400-0x4FF)
    uint16_t* com_base = (uint16_t*)0x400;
    for (int i = 0; i < 4 && pos < buffer_size - 50; i++) {
        const char* com_str = "COM";
        len = 0;
        while (com_str[len] && pos < buffer_size - 1) {
            buffer[pos++] = com_str[len++];
        }
        
        int_to_str(i + 1, num_str, 10);
        len = 0;
        while (num_str[len] && pos < buffer_size - 1) {
            buffer[pos++] = num_str[len++];
        }
        
        const char* port_str = " Base Address: 0x";
        len = 0;
        while (port_str[len] && pos < buffer_size - 1) {
            buffer[pos++] = port_str[len++];
        }
        
        if (com_base[i] != 0) {
            int_to_str(com_base[i], num_str, 16);
            len = 0;
            while (num_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = num_str[len++];
            }
        } else {
            const char* disabled = "0000 (Not Installed)";
            len = 0;
            while (disabled[len] && pos < buffer_size - 1) {
                buffer[pos++] = disabled[len++];
            }
        }
        
        const char* newline = "\n";
        len = 0;
        while (newline[len] && pos < buffer_size - 1) {
            buffer[pos++] = newline[len++];
        }
    }
    
    // RSDT address
    const char* rsdt_str = "\nRSDT Address: 0x";
    len = 0;
    while (rsdt_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = rsdt_str[len++];
    }
    
    search_ptr = (uint8_t*)0x9E000;
    found_rsdp = 0;
    for (uint32_t addr = 0x9E000; addr < 0x9FFFF && !found_rsdp; addr += 16) {
        uint8_t* test_ptr = (uint8_t*)addr;
        if (test_ptr[0] == 'R' && test_ptr[1] == 'S' && test_ptr[2] == 'D' && test_ptr[3] == 'P') {
            uint32_t rsdt = *((uint32_t*)(test_ptr + 16));
            int_to_str(rsdt, num_str, 16);
            len = 0;
            while (num_str[len] && pos < buffer_size - 1) {
                buffer[pos++] = num_str[len++];
            }
            found_rsdp = 1;
        }
    }
    
    const char* final_newline = "\n\n";
    len = 0;
    while (final_newline[len] && pos < buffer_size - 1) {
        buffer[pos++] = final_newline[len++];
    }
    
    buffer[pos] = '\0';
}

// Memory map information
static void get_memory_map_info(char* buffer, size_t buffer_size) {
    size_t pos = 0;
    
    const char* map_header = "Memory Map\n";
    size_t len = 0;
    while (map_header[len] && pos < buffer_size - 1) {
        buffer[pos++] = map_header[len++];
    }
    
    const char* note = "Memory map retrieved from multiboot (GRUB) information\n";
    len = 0;
    while (note[len] && pos < buffer_size - 1) {
        buffer[pos++] = note[len++];
    }
    
    const char* newline = "\n";
    len = 0;
    while (newline[len] && pos < buffer_size - 1) {
        buffer[pos++] = newline[len++];
    }
    
    buffer[pos] = '\0';
}

// Kernel size and page information
static void get_kernel_info(char* buffer, size_t buffer_size) {
    size_t pos = 0;
    
    const char* kernel_header = "Kernel Information\n";
    size_t len = 0;
    while (kernel_header[len] && pos < buffer_size - 1) {
        buffer[pos++] = kernel_header[len++];
    }
    
    const char* page_str = "Page Size: 4096 Bytes (4 KB)\n\n";
    len = 0;
    while (page_str[len] && pos < buffer_size - 1) {
        buffer[pos++] = page_str[len++];
    }
    
    buffer[pos] = '\0';
}

// Main function to create log.txt file
void create_log_txt_file(void) {
    // Allocate a large buffer for all system info
    char* content = (char*)fs_allocate(32768);  // 32KB buffer for more comprehensive info
    if (!content) {
        return;  // Allocation failed
    }
    
    size_t pos = 0;
    const size_t buffer_size = 32768;
    
    // Header
    const char* header = "=== Brew Kernel System Log ===\n\n";
    size_t len = 0;
    while (header[len] && pos < buffer_size - 1) {
        content[pos++] = header[len++];
    }
    
    // Get CPU info - allocate from heap to avoid stack overflow
    char* cpu_buffer = (char*)fs_allocate(4096);
    if (cpu_buffer) {
        get_cpu_info(cpu_buffer, 4096);
        len = 0;
        while (cpu_buffer[len] && pos < buffer_size - 1) {
            content[pos++] = cpu_buffer[len++];
        }
        fs_free(cpu_buffer);
    }
    
    // Get Memory info - allocate from heap
    char* mem_buffer = (char*)fs_allocate(2048);
    if (mem_buffer) {
        get_memory_info(mem_buffer, 2048);
        len = 0;
        while (mem_buffer[len] && pos < buffer_size - 1) {
            content[pos++] = mem_buffer[len++];
        }
        fs_free(mem_buffer);
    }
    
    // Get PCI info - allocate from heap
    char* pci_buffer = (char*)fs_allocate(8192);
    if (pci_buffer) {
        get_pci_info(pci_buffer, 8192);
        len = 0;
        while (pci_buffer[len] && pos < buffer_size - 1) {
            content[pos++] = pci_buffer[len++];
        }
        fs_free(pci_buffer);
    }
    
    // Get BIOS data - allocate from heap
    char* bios_buffer = (char*)fs_allocate(2048);
    if (bios_buffer) {
        get_bios_data(bios_buffer, 2048);
        len = 0;
        while (bios_buffer[len] && pos < buffer_size - 1) {
            content[pos++] = bios_buffer[len++];
        }
        fs_free(bios_buffer);
    }
    
    // Get memory map - allocate from heap
    char* memmap_buffer = (char*)fs_allocate(1024);
    if (memmap_buffer) {
        get_memory_map_info(memmap_buffer, 1024);
        len = 0;
        while (memmap_buffer[len] && pos < buffer_size - 1) {
            content[pos++] = memmap_buffer[len++];
        }
        fs_free(memmap_buffer);
    }
    
    // Get kernel info - allocate from heap
    char* kernel_buffer = (char*)fs_allocate(512);
    if (kernel_buffer) {
        get_kernel_info(kernel_buffer, 512);
        len = 0;
        while (kernel_buffer[len] && pos < buffer_size - 1) {
            content[pos++] = kernel_buffer[len++];
        }
        fs_free(kernel_buffer);
    }
    
    // Footer
    const char* footer = "=== End of System Log ===\n";
    len = 0;
    while (footer[len] && pos < buffer_size - 1) {
        content[pos++] = footer[len++];
    }
    
    content[pos] = '\0';
    
    // Write to file
    fs_write_file_at_path("/log.txt", content, pos);
    
    // Free the buffer
    fs_free(content);
}

#endif
