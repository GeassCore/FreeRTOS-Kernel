/* SPDX-License-Identifier: MIT
 *
 * Minimal empty resource table so Linux remoteproc ELF loader accepts the image.
 * Expand later when OpenAMP/rpmsg is needed.
 */
#include <stdint.h>

struct resource_table {
	uint32_t ver;
	uint32_t num;
	uint32_t reserved[2];
} __attribute__((packed));

__attribute__((section(".resource_table"), used))
const struct resource_table resource_table = {
	.ver = 1,
	.num = 0,
	.reserved = { 0, 0 },
};
