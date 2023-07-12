#include "../includes/nm.h"

unsigned char   get_special_type(const char *name, char type, unsigned int bind)
{
	char    c;

	c = 0;
	if (
		!ft_strncmp(name, ".symtab", ft_strlen(".symtab")) || 
		!ft_strncmp(name, ".strtab", ft_strlen(".strtab")) ||
		!ft_strncmp(name, ".shstrtab", ft_strlen(".shstrtab"))
		)
		c = 'a';
	if (
		!ft_strncmp(name, ".comment", ft_strlen(".comment")) || 
		!ft_strncmp(name, ".copyright", ft_strlen(".copyright")) ||
		!ft_strncmp(name, ".gnu_debug", ft_strlen(".gnu_debug")) ||
		!ft_strncmp(name, ".ident", ft_strlen(".ident")) ||
		!ft_strncmp(name, ".SUNW", ft_strlen(".SUNW"))
		)
		c = 'n';
	if (!ft_strncmp(name, ".debug_", ft_strlen(".debug_")))
		c = 'N';
	if (
		!ft_strncmp(name, ".gnu.hash", ft_strlen(".gnu.hash")) ||
		!ft_strncmp(name, ".rodata", ft_strlen(".rodata")) ||
		!ft_strncmp(name, ".dynstr", ft_strlen(".dynstr")) ||
		!ft_strncmp(name, ".rela.plt", ft_strlen(".rela.plt")) ||
		!ft_strncmp(name, ".rel.plt", ft_strlen(".rel.plt")) ||
		!ft_strncmp(name, ".SUNW_version", ft_strlen(".SUNW_version")) ||
		!ft_strncmp(name, ".rela.bss", ft_strlen(".rela.bss")) ||
		!ft_strncmp(name, ".rela.got", ft_strlen(".rela.got")) ||
		!ft_strncmp(name, ".gnu.version_r", ft_strlen(".gnu.version_r"))
	)
	{
		c = 'R';
		if (bind == STB_LOCAL)
			c += 32;
	}
	if (c)
		return (c);
	return (type);
}

char get_type64(t_file *file, t_symbol *symbol) {
	unsigned int c, bind, type;
	uint16_t st_shndx;

	c = '?';
	bind = ELF64_ST_BIND(symbol->sym64->st_info);
	type = ELF64_ST_TYPE(symbol->sym64->st_info);
	st_shndx = symbol->sym64->st_shndx;
	uint16_t e_shnum = file->ehdr64->e_shnum;

	if (bind == STB_GNU_UNIQUE)
		c = 'u';
	else if (bind == STB_WEAK && type == STT_OBJECT) {
		c = 'V';
		if (st_shndx == SHN_UNDEF)
			c = 'v';
		} else if (bind == STB_WEAK) {
		c = 'W';
		if (st_shndx == SHN_UNDEF)
			c = 'w';
		} else if (st_shndx == SHN_UNDEF)
		c = 'U';
	else if (st_shndx == SHN_ABS)
		c = 'A';
	else if (st_shndx == SHN_COMMON)
		c = 'C';
	else if (st_shndx < e_shnum) {
		uint32_t sh_type = file->shdr64[st_shndx].sh_type;
		uint64_t sh_flags = file->shdr64[st_shndx].sh_flags;
		/* printf("sh_type: %d, SHT_PROGBITS: %d, SHT_DYNAMIC %d, sh_flags: %lu, (): %d\n", sh_type, SHT_PROGBITS, SHT_DYNAMIC, sh_flags, (SHF_ALLOC | SHF_WRITE)); */
		if (sh_type == SHT_NOBITS && sh_flags == (SHF_ALLOC | SHF_WRITE))
			c = 'B';
		else if ((sh_type == SHT_PROGBITS || sh_type == SHT_RELA ||
				sh_type == SHT_REL || sh_type == SHT_HASH ||
				sh_type == SHT_GNU_versym || sh_type == SHT_GNU_verdef ||
				sh_type == SHT_STRTAB || sh_type == SHT_DYNSYM ||
				sh_type == SHT_NOTE) && sh_flags == SHF_ALLOC)
			c = 'R';
		else if (sh_type == SHT_PROGBITS && sh_flags == (SHF_ALLOC | SHF_WRITE))
			c = 'D';
		else if (sh_type == SHT_DYNAMIC || sh_type == SHT_FINI_ARRAY || sh_type == SHT_INIT_ARRAY)
			c = 'D';
		else if (sh_type == SHT_PROGBITS && sh_flags == (SHF_ALLOC | SHF_EXECINSTR))
			c = 'T';
		else
			c = ('t' - 32);
	}
	if (bind == STB_LOCAL && c != '?')
		c += 32;
	c = get_special_type(symbol->name, c, bind);
	return (c);
}

int	print_symbol64(t_file *file, t_symbol *symbol) {
	char type = get_type64(file, symbol);
	if (*symbol->addr64 == 0 && (type == 'U' || type == 'u' || type =='w' || type == 'W'))
		ft_printf("%16c", ' ');
	else
		ft_printf("%016x", *symbol->addr64);
	ft_printf(" %c ", type);
	ft_printf("%s\n", symbol->name);
	return 0;
}

