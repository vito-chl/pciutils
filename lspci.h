/*
 *	The PCI Utilities -- List All PCI Devices
 *
 *	Copyright (c) 1997--2018 Martin Mares <mj@ucw.cz>
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#define PCIUTILS_LSPCI
#include "pciutils.h"

/*
 *  If we aren't being compiled by GCC, use xmalloc() instead of alloca().
 *  This increases our memory footprint, but only slightly since we don't
 *  use alloca() much.
 */
#if defined (__FreeBSD__) || defined (__NetBSD__) || defined (__OpenBSD__) || defined (__DragonFly__) || defined (__DJGPP__)
/* alloca() is defined in stdlib.h */
#elif defined(__GNUC__) && !defined(PCI_OS_WINDOWS)
#include <alloca.h>
#else
#undef alloca
#define alloca xmalloc
#endif

/*** Options ***/

extern int verbose;
extern struct pci_filter filter;
extern char *opt_pcimap;

/*** PCI devices and access to their config space ***/

struct device {
  struct device *next;
  struct pci_dev *dev;
  /* Bus topology calculated by grow_tree() */
  struct device *bus_next;
  struct bus *parent_bus;
  struct bridge *bridge;
  /* Cache */
  unsigned int config_cached, config_bufsize;
  byte *config;				/* Cached configuration space data */
  byte *present;			/* Maps which configuration bytes are present */
};

extern struct device *first_dev;
extern struct pci_access *pacc;

struct device *scan_device(struct pci_dev *p);
void show_device(struct device *d);

int config_fetch(struct device *d, unsigned int pos, unsigned int len);
u32 get_conf_long(struct device *d, unsigned int pos);
word get_conf_word(struct device *d, unsigned int pos);
byte get_conf_byte(struct device *d, unsigned int pos);

/* Useful macros for decoding of bits and bit fields */

#define FLAG(x,y) ((x & y) ? '+' : '-')
#define BITS(x,at,width) (((x) >> (at)) & ((1 << (width)) - 1))
#define TABLE(tab,x,buf) ((x) < sizeof(tab)/sizeof((tab)[0]) ? (tab)[x] : (sprintf((buf), "??%d", (x)), (buf)))

/* ls-vpd.c */

void cap_vpd(struct device *d);

/* ls-caps.c */

void show_caps(struct device *d, int where);

/* ls-ecaps.c */

void show_ext_caps(struct device *d, int type);

/* ls-caps-vendor.c */

void show_vendor_caps(struct device *d, int where, int cap);

/* ls-kernel.c */

void show_kernel_machine(struct device *d UNUSED);
void show_kernel(struct device *d UNUSED);
void show_kernel_cleanup(void);

/* ls-tree.c */

struct bridge {
  struct bridge *chain;			/* Single-linked list of bridges */
  struct bridge *next, *child;		/* Tree of bridges */
  struct bus *first_bus, *last_bus;	/* List of buses connected to this bridge */
  unsigned int domain;
  unsigned int primary, secondary, subordinate;	/* Bus numbers */
  struct device *br_dev;
};

struct bus {
  unsigned int domain;
  unsigned int number;
  struct bus *sibling;
  struct bridge *parent_bridge;
  struct device *first_dev, **last_dev;
};

extern struct bridge host_bridge;

void grow_tree(void);
void show_forest(struct pci_filter *filter);

/* ls-map.c */

void map_the_bus(void);
