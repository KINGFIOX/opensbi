// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane(正确) information in its header.
 */

/// @brief return the total size of the device tree
/// @param fdt 
/// @return 负数表示: 错误, 正数返回: fdt_totalsize(fdt)
int32_t fdt_ro_probe_(const void *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);  // 一开始就获取了 fdt 的 totalsize, 下面只是检查是否合法

	if (can_assume(VALID_DTB))
		return totalsize;

	/* The device tree must be at an 8-byte aligned address */
	if ((uintptr_t)fdt & 7) return -FDT_ERR_ALIGNMENT;

	if (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		// expend to 
		// if (!can_assume_(ASSUME_LAST)) {
		// 其中 	ASSUME_LATEST = 1 << 2,
		if (!can_assume(LATEST)) {
		 	// 版本不兼容
			if (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)  
				return -FDT_ERR_BADVERSION;
			if (fdt_last_comp_version(fdt) > FDT_LAST_SUPPORTED_VERSION)
				return -FDT_ERR_BADVERSION;
		}
	} else if (fdt_magic(fdt) == FDT_SW_MAGIC) {  // 这里的 FDT_SW_MAGIC 是 FDT_MAGIC 取反，意味着: 未完成的序列化写入 blob
		/* Unfinished sequential-write blob */
		if (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			return -FDT_ERR_BADSTATE;
	} else {
		return -FDT_ERR_BADMAGIC;
	}

	if (totalsize < INT32_MAX)
		return totalsize;
	else
		return -FDT_ERR_TRUNCATED;
}

static int check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	return (off >= hdrsize) && (off <= totalsize);
}

static int check_block_(uint32_t hdrsize, uint32_t totalsize, uint32_t base, uint32_t size)
{
	if (!check_off_(hdrsize, totalsize, base))
		return 0; /* block start out of bounds */
	if ((base + size) < base)
		return 0; /* overflow */
	if (!check_off_(hdrsize, totalsize, base + size))
		return 0; /* block end out of bounds */
	return 1;
}

size_t fdt_header_size_(uint32_t version)
{
	if (version <= 1)
		return FDT_V1_SIZE;
	else if (version <= 2)
		return FDT_V2_SIZE;
	else if (version <= 3)
		return FDT_V3_SIZE;
	else if (version <= 16)
		return FDT_V16_SIZE;
	else
		return FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	return can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

int fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	if ((uintptr_t)fdt & 7)
		return -FDT_ERR_ALIGNMENT;

	if (fdt_magic(fdt) != FDT_MAGIC)
		return -FDT_ERR_BADMAGIC;
	if (!can_assume(LATEST)) {
		if ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			return -FDT_ERR_BADVERSION;
		if (fdt_version(fdt) < fdt_last_comp_version(fdt))
			return -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	if (!can_assume(VALID_DTB)) {

		if ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			return -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		if (!check_off_(hdrsize, fdt_totalsize(fdt), fdt_off_mem_rsvmap(fdt)))
			return -FDT_ERR_TRUNCATED;
	}

	if (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		if (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			if (!check_off_(hdrsize, fdt_totalsize(fdt), fdt_off_dt_struct(fdt)))
				return -FDT_ERR_TRUNCATED;
		} else {
			if (!check_block_(hdrsize, fdt_totalsize(fdt), fdt_off_dt_struct(fdt), fdt_size_dt_struct(fdt)))
				return -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		if (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			return -FDT_ERR_TRUNCATED;
	}

	return 0;
}

/// @brief 
/// @param fdt 
/// @param offset 
/// @param len 
/// @return 
const void *fdt_offset_ptr(const void *fdt, int offset, unsigned int len)
{
	unsigned int uoffset = offset;
	unsigned int absoffset = offset + fdt_off_dt_struct(fdt);

	if (offset < 0)
		return NULL;

	if (!can_assume(VALID_INPUT))
		if ((absoffset < uoffset)  // 检查有没有溢出
		    || ((absoffset + len) < absoffset)  // 检查溢出
		    || (absoffset + len) > fdt_totalsize(fdt) // 是否发生了截断
			)
			return NULL;

	if (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		if (((uoffset + len) < uoffset) // 发生溢出
		|| ((offset + len) > fdt_size_dt_struct(fdt))  // 超过长度
		)
			return NULL;

	return fdt_offset_ptr_(fdt, offset);
}

/// @brief 
/// @param fdt 
/// @param startoffset 
/// @param nextoffset 作为返回值, 返回下一个 tag 的偏移地址
/// @return 返回对应的是哪个 tag
uint32_t fdt_next_tag(const void *fdt, int startoffset, int *nextoffset)
{
	int offset = startoffset;

	*nextoffset = -FDT_ERR_TRUNCATED;
 	const fdt32_t *	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	if (!can_assume(VALID_DTB) && !tagp)
		return FDT_END; /* premature end */
	uint32_t tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;  // 跳过 tag

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	case FDT_BEGIN_NODE:  // 说明是: subnode 之类的
		/* skip name */
		const char *p;
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} while (p && (*p != '\0'));
		if (!can_assume(VALID_DTB) && !p)
			return FDT_END; /* premature end */  // 可能没了
		break;

	case FDT_PROP:
		const fdt32_t *	lenp = fdt_offset_ptr(fdt, offset, sizeof(fdt32_t));  // lenp = offset 对应的 addr
		if (!can_assume(VALID_DTB) && !lenp) return FDT_END; /* premature end */
		/* skip-name offset, length and value */
		// offset + sizeof(struct fdt_property) - FDT_TAGSIZE 跳过当前的 property
		// offset + fdt32_to_cpu(*lenp) 跳过 property 的 value
		// 这个要看 struct fdt_property
		offset += sizeof(struct fdt_property) - FDT_TAGSIZE + fdt32_to_cpu(*lenp);
		if (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 &&
			fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		break;

	case FDT_END:
	case FDT_END_NODE:
	case FDT_NOP:
		break;

	default:
		return FDT_END;
	}

	// 再次检查
	if (!fdt_offset_ptr(fdt, startoffset, offset - startoffset)) return FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);  // 下一个 tag 的 offset
	return tag;
}

/// @brief 
/// @param fdt 
/// @param offset 
/// @return 
int fdt_check_node_offset_(const void *fdt, int offset)
{
	if (!can_assume(VALID_INPUT) && ((offset < 0) || (offset % FDT_TAGSIZE))) return -FDT_ERR_BADOFFSET;

	// 检查给定的 offset 是否开启了一个新的节点
	if (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE) return -FDT_ERR_BADOFFSET;

	return offset;
}

int fdt_check_prop_offset_(const void *fdt, int offset)
{
	if (!can_assume(VALID_INPUT) && ((offset < 0) || (offset % FDT_TAGSIZE != 0)))
		return -FDT_ERR_BADOFFSET;

	if (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		return -FDT_ERR_BADOFFSET;

	return offset;
}

/// @brief 
/// @param fdt 
/// @param offset 
/// @param depth 返回: subnode 的深度
/// @return 
int fdt_next_node(const void *fdt, int offset, int *depth)
{
	int nextoffset = 0;

	if (offset >= 0)
		if ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0) return nextoffset;

	uint32_t tag;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_PROP:
		case FDT_NOP:
			break;

		case FDT_BEGIN_NODE:
			if (depth) (*depth)++;
			break;

		case FDT_END_NODE:
			if (depth && ((--(*depth)) < 0)) return nextoffset;
			break;

		case FDT_END:
			if ((nextoffset >= 0) || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth)) return -FDT_ERR_NOTFOUND;
			else return nextoffset;
		}
	} while (tag != FDT_BEGIN_NODE);

	return offset;
}

int fdt_first_subnode(const void *fdt, int offset)
{
	int depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	if (offset < 0 || depth != 1) return -FDT_ERR_NOTFOUND; // 会检查深度, 代码质量很高

	return offset;
}

int fdt_next_subnode(const void *fdt, int offset)
{
	int depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		if (offset < 0 || depth < 1) return -FDT_ERR_NOTFOUND;
	} while (depth > 1);  // 确实, 很聪明的想法

	return offset;
}

/// @brief 
/// @param strtab 主串
/// @param tabsize 主串的长度
/// @param s 子串(模式串)
/// @return 
const char *fdt_find_string_(const char *strtab, int tabsize, const char *s)
{
	int len = strlen(s) + 1;

	const char *last = strtab + tabsize - len;

	for (const char * p = strtab; p <= last; p++)
		if (memcmp(p, s, len) == 0) return p;
	return NULL;
}

int fdt_move(const void *fdt, void *buf, int bufsize)
{
	if (!can_assume(VALID_INPUT) && bufsize < 0) return -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);  // 检查 fdt 的合法性

	if (fdt_totalsize(fdt) > (unsigned int)bufsize) return -FDT_ERR_NOSPACE;  // 检查长度

	memmove(buf, fdt, fdt_totalsize(fdt));  // 复制
	return 0;
}
