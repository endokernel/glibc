/* x86 CET initializers function.
   Copyright (C) 2018-2020 Free Software Foundation, Inc.

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

#include <unistd.h>
#include <errno.h>
#include <libintl.h>
#include <ldsodefs.h>
#include <dl-cet.h>

/* GNU_PROPERTY_X86_FEATURE_1_IBT and GNU_PROPERTY_X86_FEATURE_1_SHSTK
   are defined in <elf.h>, which are only available for C sources.
   X86_FEATURE_1_IBT and X86_FEATURE_1_SHSTK are defined in <sysdep.h>
   which are available for both C and asm sources.  They must match.   */
#if GNU_PROPERTY_X86_FEATURE_1_IBT != X86_FEATURE_1_IBT
# error GNU_PROPERTY_X86_FEATURE_1_IBT != X86_FEATURE_1_IBT
#endif
#if GNU_PROPERTY_X86_FEATURE_1_SHSTK != X86_FEATURE_1_SHSTK
# error GNU_PROPERTY_X86_FEATURE_1_SHSTK != X86_FEATURE_1_SHSTK
#endif

static int
dl_cet_mark_legacy_region (struct link_map *l)
{
  /* Mark PT_LOAD segments with PF_X in legacy code page bitmap.  */
  size_t i, phnum = l->l_phnum;
  const ElfW(Phdr) *phdr = l->l_phdr;
  for (i = 0; i < phnum; i++)
    if (phdr[i].p_type == PT_LOAD && (phdr[i].p_flags & PF_X))
      {
	/* One bit in legacy bitmap represents a page.  */
	ElfW(Addr) start = (phdr[i].p_vaddr + l->l_addr);
	ElfW(Addr) len = (phdr[i].p_memsz);

	dl_cet_mark_legacy_code(start, len, 1);
      }

  return 0;
}

/* Check if object M is compatible with CET.  */

static void
dl_cet_check (struct link_map *m, const char *program)
{
  /* Check how IBT should be enabled.  */
  enum dl_x86_cet_control enable_ibt_type
    = GL(dl_x86_feature_control).ibt;
  /* Check how SHSTK should be enabled.  */
  enum dl_x86_cet_control enable_shstk_type
    = GL(dl_x86_feature_control).shstk;

  /* No legacy object check if both IBT and SHSTK are always on.  */
  // Should not compile GLIBC in this mode!!!!
  if (enable_ibt_type == cet_always_on
      && enable_shstk_type == cet_always_on)
    return;

    /* Check if IBT is enabled by kernel.  */
  bool ibt_enabled
    = (GL(dl_x86_feature_1) & GNU_PROPERTY_X86_FEATURE_1_IBT) != 0;
  /* Check if SHSTK is enabled by kernel.  */
  bool shstk_enabled
    = (GL(dl_x86_feature_1) & GNU_PROPERTY_X86_FEATURE_1_SHSTK) != 0;

  if (ibt_enabled || shstk_enabled)
    {
      struct link_map *l = NULL;

      /* ld.so is CET-enabled by kernel.  But shared objects may not
	 support IBT nor SHSTK.  */
      if (1)
	{
	  unsigned int i;

	  i = m->l_searchlist.r_nlist;
	  while (i-- > 0)
	    {
	      l = m->l_initfini[i];
	      if (l->l_init_called || (l->l_cet & lc_ibt))
	        continue;
#ifdef SHARED
	      if (l == &GL(dl_rtld_map)
	         || l->l_real == &GL(dl_rtld_map))
		 continue;
#endif
	      int res = dl_cet_mark_legacy_region (l);
	      if (res != 0)
	        {
		  if (program)
		    _dl_fatal_printf ("%s: failed to mark legacy code region\n", l->l_name);
		  else 
		    _dl_signal_error (-res, l->l_name, "dlopen",
					  N_("failed to mark legacy code region"));
		}
	    }
	}

	  // CET will be enabled anyway, because our loader has CET enabled
      if (1)
	{ 
	  unsigned int feature_1 = 0;
	  feature_1 |= GNU_PROPERTY_X86_FEATURE_1_IBT;
	  feature_1 |= GNU_PROPERTY_X86_FEATURE_1_SHSTK;
	  struct pthread *self = THREAD_SELF;
	  THREAD_SETMEM (self, header.feature_1, feature_1);
	}
    }
}

void
_dl_cet_open_check (struct link_map *l)
{
  dl_cet_check (l, NULL);
}

#ifdef SHARED

# ifndef LINKAGE
#  define LINKAGE
# endif

LINKAGE
void
_dl_cet_check (struct link_map *main_map, const char *program)
{
  dl_cet_check (main_map, program);
}
#endif /* SHARED */
