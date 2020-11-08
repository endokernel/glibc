/* Support for GNU properties.  x86 version.
   Copyright (C) 2018-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#ifndef _DL_PROP_H
#define _DL_PROP_H

extern void _dl_cet_check (struct link_map *, const char *)
    attribute_hidden;
extern void _dl_cet_open_check (struct link_map *)
    attribute_hidden;

static inline void __attribute__ ((always_inline))
_rtld_main_check (struct link_map *m, const char *program)
{
#if CET_ENABLED
  _dl_cet_check (m, program);
#endif
}

static inline void __attribute__ ((always_inline))
_dl_open_check (struct link_map *m)
{
#if CET_ENABLED
  _dl_cet_open_check (m);
#endif
}

static inline void __attribute__ ((unused))
_dl_process_cet_property_note (struct link_map *l,
			      const ElfW(Nhdr) *note,
			      const ElfW(Addr) size,
			      const ElfW(Addr) align)
{
#if CET_ENABLED
  // force CET mode to ibt+ss on glibc side
  /* We get here only if there is one or no GNU property note.  */
  l->l_cet = lc_none;
  l->l_cet |= lc_ibt;
  l->l_cet |= lc_shstk;
#endif
}

static inline void __attribute__ ((unused))
_dl_process_pt_note (struct link_map *l, const ElfW(Phdr) *ph)
{
  const ElfW(Nhdr) *note = (const void *) (ph->p_vaddr + l->l_addr);
  _dl_process_cet_property_note (l, note, ph->p_memsz, ph->p_align);
}

static inline int __attribute__ ((always_inline))
_dl_process_gnu_property (struct link_map *l, uint32_t type, uint32_t datasz,
			  void *data)
{
  return 0;
}

#endif /* _DL_PROP_H */
