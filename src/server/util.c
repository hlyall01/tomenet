/* $Id$ */
/* File: util.c */

/* Purpose: Angband utilities -BEN- */


#define SERVER

#include "angband.h"
#include "../world/world.h"


#ifndef HAS_MEMSET

/*
 * For those systems that don't have "memset()"
 *
 * Set the value of each of 'n' bytes starting at 's' to 'c', return 's'
 * If 'n' is negative, you will erase a whole lot of memory.
 */
char *memset(char *s, int c, huge n)
{
	char *t;
	for (t = s; n--; ) *t++ = c;
	return (s);
}

#endif



#ifndef HAS_STRICMP

/*
 * For those systems that don't have "stricmp()"
 *
 * Compare the two strings "a" and "b" ala "strcmp()" ignoring case.
 */
int stricmp(cptr a, cptr b)
{
	cptr s1, s2;
	char z1, z2;

	/* Scan the strings */
	for (s1 = a, s2 = b; TRUE; s1++, s2++)
	{
		z1 = FORCEUPPER(*s1);
		z2 = FORCEUPPER(*s2);
		if (z1 < z2) return (-1);
		if (z1 > z2) return (1);
		if (!z1) return (0);
	}
}

#endif

bool in_banlist(char *addr){
	struct ip_ban *ptr;
	for(ptr=banlist; ptr!=(struct ip_ban*)NULL; ptr=ptr->next){
		if(!strcmp(addr, ptr->ip)) return(TRUE);
	}
	return(FALSE);
}

void check_banlist(){
	struct ip_ban *ptr, *new, *old=(struct ip_ban*)NULL;
	ptr=banlist;
	while(ptr!=(struct ip_ban*)NULL){
		if(ptr->time){
			if(!(--ptr->time)){
				s_printf("Unbanning connections from %s\n", ptr->ip);
				if(!old){
					banlist=ptr->next;
					new=banlist;
				}
				else{
					old->next=ptr->next;
					new=old->next;
				}
				free(ptr);
				ptr=new;
				continue;
			}
		}
		ptr=ptr->next;
	}
}


#ifdef SET_UID

# ifndef HAS_USLEEP

/*
 * For those systems that don't have "usleep()" but need it.
 *
 * Fake "usleep()" function grabbed from the inl netrek server -cba
 */
static int usleep(huge microSeconds)
{
	struct timeval      Timer;

	int                 nfds = 0;

#ifdef FD_SET
	fd_set          *no_fds = NULL;
#else
	int                     *no_fds = NULL;
#endif


	/* Was: int readfds, writefds, exceptfds; */
	/* Was: readfds = writefds = exceptfds = 0; */


	/* Paranoia -- No excessive sleeping */
	if (microSeconds > 4000000L) core("Illegal usleep() call");


	/* Wait for it */
	Timer.tv_sec = (microSeconds / 1000000L);
	Timer.tv_usec = (microSeconds % 1000000L);

	/* Wait for it */
	if (select(nfds, no_fds, no_fds, no_fds, &Timer) < 0)
	{
		/* Hack -- ignore interrupts */
		if (errno != EINTR) return -1;
	}

	/* Success */
	return 0;
}

# endif


/*
 * Hack -- External functions
 */
extern struct passwd *getpwuid();
extern struct passwd *getpwnam();

/*
 * Find a default user name from the system.
 */
void user_name(char *buf, int id)
{
	struct passwd *pw;

	/* Look up the user name */
	if ((pw = getpwuid(id)))
	{
		(void)strcpy(buf, pw->pw_name);
		buf[16] = '\0';

#ifdef CAPITALIZE_USER_NAME
		/* Hack -- capitalize the user name */
		if (islower(buf[0])) buf[0] = toupper(buf[0]);
#endif

		return;
	}

	/* Oops.  Hack -- default to "PLAYER" */
	strcpy(buf, "PLAYER");
}

#endif /* SET_UID */




/*
 * The concept of the "file" routines below (and elsewhere) is that all
 * file handling should be done using as few routines as possible, since
 * every machine is slightly different, but these routines always have the
 * same semantics.
 *
 * In fact, perhaps we should use the "path_parse()" routine below to convert
 * from "canonical" filenames (optional leading tilde's, internal wildcards,
 * slash as the path seperator, etc) to "system" filenames (no special symbols,
 * system-specific path seperator, etc).  This would allow the program itself
 * to assume that all filenames are "Unix" filenames, and explicitly "extract"
 * such filenames if needed (by "path_parse()", or perhaps "path_canon()").
 *
 * Note that "path_temp" should probably return a "canonical" filename.
 *
 * Note that "my_fopen()" and "my_open()" and "my_make()" and "my_kill()"
 * and "my_move()" and "my_copy()" should all take "canonical" filenames.
 *
 * Note that "canonical" filenames use a leading "slash" to indicate an absolute
 * path, and a leading "tilde" to indicate a special directory, and default to a
 * relative path, but MSDOS uses a leading "drivename plus colon" to indicate the
 * use of a "special drive", and then the rest of the path is parsed "normally",
 * and MACINTOSH uses a leading colon to indicate a relative path, and an embedded
 * colon to indicate a "drive plus absolute path", and finally defaults to a file
 * in the current working directory, which may or may not be defined.
 *
 * We should probably parse a leading "~~/" as referring to "ANGBAND_DIR". (?)
 */


#ifdef ACORN


/*
 * Most of the "file" routines for "ACORN" should be in "main-acn.c"
 */


#else /* ACORN */


#ifdef SET_UID

/*
 * Extract a "parsed" path from an initial filename
 * Normally, we simply copy the filename into the buffer
 * But leading tilde symbols must be handled in a special way
 * Replace "~user/" by the home directory of the user named "user"
 * Replace "~/" by the home directory of the current user
 */
errr path_parse(char *buf, int max, cptr file)
{
	cptr            u, s;
	struct passwd   *pw;
	char            user[128];


	/* Assume no result */
	buf[0] = '\0';

	/* No file? */
	if (!file) return (-1);

	/* File needs no parsing */
	if (file[0] != '~')
	{
		strcpy(buf, file);
		return (0);
	}

	/* Point at the user */
	u = file+1;

	/* Look for non-user portion of the file */
	s = strstr(u, PATH_SEP);

	/* Hack -- no long user names */
	if (s && (s >= u + sizeof(user))) return (1);

	/* Extract a user name */
	if (s)
	{
		int i;
		for (i = 0; u < s; ++i) user[i] = *u++;
		user[i] = '\0';
		u = user;
	}

	/* Look up the "current" user */
	if (u[0] == '\0') u = getlogin();

	/* Look up a user (or "current" user) */
	if (u) pw = getpwnam(u);
	else pw = getpwuid(getuid());

	/* Nothing found? */
	if (!pw) return (1);

	/* Make use of the info */
	(void)strcpy(buf, pw->pw_dir);

	/* Append the rest of the filename, if any */
	if (s) (void)strcat(buf, s);

	/* Success */
	return (0);
}


#else /* SET_UID */


/*
 * Extract a "parsed" path from an initial filename
 *
 * This requires no special processing on simple machines,
 * except for verifying the size of the filename.
 */
errr path_parse(char *buf, int max, cptr file)
{
	/* Accept the filename */
	strnfmt(buf, max, "%s", file);

	/* Success */
	return (0);
}


#endif /* SET_UID */


/*
 * Hack -- acquire a "temporary" file name if possible
 *
 * This filename is always in "system-specific" form.
 */
errr path_temp(char *buf, int max)
{
	cptr s;

	/* Temp file */
	s = tmpnam(NULL);

	/* Oops */
	if (!s) return (-1);

	/* Format to length */
	strnfmt(buf, max, "%s", s);

	/* Success */
	return (0);
}

/*
 * Hack -- replacement for "fopen()"
 */
FILE *my_fopen(cptr file, cptr mode)
{
	char                buf[1024];

	/* Hack -- Try to parse the path */
	if (path_parse(buf, 1024, file)) return (NULL);

	/* Attempt to fopen the file anyway */
	return (fopen(buf, mode));
}


/*
 * Hack -- replacement for "fclose()"
 */
errr my_fclose(FILE *fff)
{
	/* Require a file */
	if (!fff) return (-1);

	/* Close, check for error */
	if (fclose(fff) == EOF) return (1);

	/* Success */
	return (0);
}


#endif /* ACORN */


/*
 * Hack -- replacement for "fgets()"
 *
 * Read a string, without a newline, to a file
 *
 * Process tabs, strip internal non-printables
 */
errr my_fgets(FILE *fff, char *buf, huge n, bool conv)
{
	huge i = 0;

	char *s;

	char tmp[1024];

	/* Read a line */
	if (fgets(tmp, 1024, fff))
	{
		/* Convert weirdness */
		for (s = tmp; *s; s++)
		{
			/* Handle newline */
			if (*s == '\n')
			{
				/* Terminate */
				buf[i] = '\0';

				/* Success */
				return (0);
			}

			/* Handle tabs */
			else if (*s == '\t')
			{
				/* Hack -- require room */
				if (i + 8 >= n) break;

				/* Append a space */
				buf[i++] = ' ';

				/* Append some more spaces */
				while (!(i % 8)) buf[i++] = ' ';
			}

			/* Handle printables */
			else if (isprint(*s) || *s=='\377')
			{
				/* easier to edit perma files */
				if(conv && *s=='{' && *(s+1)!='{')
					*s='\377';
				/* Copy */
				buf[i++] = *s;

				/* Check length */
				if (i >= n) break;
			}
		}
	}

	/* Nothing */
	buf[0] = '\0';

	/* Failure */
	return (1);
}


/*
 * Hack -- replacement for "fputs()"
 *
 * Dump a string, plus a newline, to a file
 *
 * XXX XXX XXX Process internal weirdness?
 */
errr my_fputs(FILE *fff, cptr buf, huge n)
{
	/* XXX XXX */
	n = n ? n : 0;

	/* Dump, ignore errors */
	(void)fprintf(fff, "%s\n", buf);

	/* Success */
	return (0);
}


#ifdef ACORN


/*
 * Most of the "file" routines for "ACORN" should be in "main-acn.c"
 *
 * Many of them can be rewritten now that only "fd_open()" and "fd_make()"
 * and "my_fopen()" should ever create files.
 */


#else /* ACORN */


/*
 * Code Warrior is a little weird about some functions
 */
#ifdef BEN_HACK
extern int open(const char *, int, ...);
extern int close(int);
extern int read(int, void *, unsigned int);
extern int write(int, const void *, unsigned int);
extern long lseek(int, long, int);
#endif /* BEN_HACK */


/*
 * The Macintosh is a little bit brain-dead sometimes
 */
#ifdef MACINTOSH
# define open(N,F,M) open((char*)(N),F)
# define write(F,B,S) write(F,(char*)(B),S)
#endif /* MACINTOSH */


/*
 * Several systems have no "O_BINARY" flag
 */
#ifndef O_BINARY
# define O_BINARY 0
#endif /* O_BINARY */


/*
 * Hack -- attempt to delete a file
 */
errr fd_kill(cptr file)
{
	char                buf[1024];

	/* Hack -- Try to parse the path */
	if (path_parse(buf, 1024, file)) return (-1);

	/* Remove */
	(void)remove(buf);

	/* XXX XXX XXX */
	return (0);
}


/*
 * Hack -- attempt to move a file
 */
errr fd_move(cptr file, cptr what)
{
	char                buf[1024];
	char                aux[1024];

	/* Hack -- Try to parse the path */
	if (path_parse(buf, 1024, file)) return (-1);

	/* Hack -- Try to parse the path */
	if (path_parse(aux, 1024, what)) return (-1);

	/* Rename */
	(void)rename(buf, aux);

	/* XXX XXX XXX */
	return (0);
}


/*
 * Hack -- attempt to copy a file
 */
errr fd_copy(cptr file, cptr what)
{
	char                buf[1024];
	char                aux[1024];

	/* Hack -- Try to parse the path */
	if (path_parse(buf, 1024, file)) return (-1);

	/* Hack -- Try to parse the path */
	if (path_parse(aux, 1024, what)) return (-1);

	/* Copy XXX XXX XXX */
	/* (void)rename(buf, aux); */

	/* XXX XXX XXX */
	return (1);
}


/*
 * Hack -- attempt to open a file descriptor (create file)
 *
 * This function should fail if the file already exists
 *
 * Note that we assume that the file should be "binary"
 *
 * XXX XXX XXX The horrible "BEN_HACK" code is for compiling under
 * the CodeWarrior compiler, in which case, for some reason, none
 * of the "O_*" flags are defined, and we must fake the definition
 * of "O_RDONLY", "O_WRONLY", and "O_RDWR" in "A-win-h", and then
 * we must simulate the effect of the proper "open()" call below.
 */
int fd_make(cptr file, int mode)
{
	char                buf[1024];

	/* Hack -- Try to parse the path */
	if (path_parse(buf, 1024, file)) return (-1);

#ifdef BEN_HACK

	/* Check for existance */
	/* if (fd_close(fd_open(file, O_RDONLY | O_BINARY))) return (1); */

	/* Mega-Hack -- Create the file */
	(void)my_fclose(my_fopen(file, "wb"));

	/* Re-open the file for writing */
	return (open(buf, O_WRONLY | O_BINARY, mode));

#else /* BEN_HACK */

	/* Create the file, fail if exists, write-only, binary */
	return (open(buf, O_CREAT | O_EXCL | O_WRONLY | O_BINARY, mode));

#endif /* BEN_HACK */

}


/*
 * Hack -- attempt to open a file descriptor (existing file)
 *
 * Note that we assume that the file should be "binary"
 */
int fd_open(cptr file, int flags)
{
	char                buf[1024];

	/* Hack -- Try to parse the path */
	if (path_parse(buf, 1024, file)) return (-1);

	/* Attempt to open the file */
	return (open(buf, flags | O_BINARY, 0));
}


/*
 * Hack -- attempt to lock a file descriptor
 *
 * Legal lock types -- F_UNLCK, F_RDLCK, F_WRLCK
 */
errr fd_lock(int fd, int what)
{
	/* XXX XXX */
	what = what ? what : 0;

	/* Verify the fd */
	if (fd < 0) return (-1);

#ifdef SET_UID

# ifdef USG

	/* Un-Lock */
	if (what == F_UNLCK)
	{
		/* Unlock it, Ignore errors */
		lockf(fd, F_ULOCK, 0);
	}

	/* Lock */
	else
	{
		/* Lock the score file */
		if (lockf(fd, F_LOCK, 0) != 0) return (1);
	}

#else

	/* Un-Lock */
	if (what == F_UNLCK)
	{
		/* Unlock it, Ignore errors */
		(void)flock(fd, LOCK_UN);
	}

	/* Lock */
	else
	{
		/* Lock the score file */
		if (flock(fd, LOCK_EX) != 0) return (1);
	}

# endif

#endif

	/* Success */
	return (0);
}


/*
 * Hack -- attempt to seek on a file descriptor
 */
errr fd_seek(int fd, huge n)
{
	long p;

	/* Verify fd */
	if (fd < 0) return (-1);

	/* Seek to the given position */
	p = lseek(fd, n, SEEK_SET);

	/* Failure */
	if (p < 0) return (1);

	/* Failure */
	if (p != n) return (1);

	/* Success */
	return (0);
}


/*
 * Hack -- attempt to truncate a file descriptor
 */
errr fd_chop(int fd, huge n)
{
	/* XXX XXX */
	n = n ? n : 0;

	/* Verify the fd */
	if (fd < 0) return (-1);

#if defined(sun) || defined(ultrix) || defined(NeXT)
	/* Truncate */
	ftruncate(fd, n);
#endif

	/* Success */
	return (0);
}


/*
 * Hack -- attempt to read data from a file descriptor
 */
errr fd_read(int fd, char *buf, huge n)
{
	/* Verify the fd */
	if (fd < 0) return (-1);

#ifndef SET_UID

	/* Read pieces */
	while (n >= 16384)
	{
		/* Read a piece */
		if (read(fd, buf, 16384) != 16384) return (1);

		/* Shorten the task */
		buf += 16384;

		/* Shorten the task */
		n -= 16384;
	}

#endif

	/* Read the final piece */
	if (read(fd, buf, n) != n) return (1);

	/* Success */
	return (0);
}


/*
 * Hack -- Attempt to write data to a file descriptor
 */
errr fd_write(int fd, cptr buf, huge n)
{
	/* Verify the fd */
	if (fd < 0) return (-1);

#ifndef SET_UID

	/* Write pieces */
	while (n >= 16384)
	{
		/* Write a piece */
		if (write(fd, buf, 16384) != 16384) return (1);

		/* Shorten the task */
		buf += 16384;

		/* Shorten the task */
		n -= 16384;
	}

#endif

	/* Write the final piece */
	if (write(fd, buf, n) != n) return (1);

	/* Success */
	return (0);
}


/*
 * Hack -- attempt to close a file descriptor
 */
errr fd_close(int fd)
{
	/* Verify the fd */
	if (fd < 0) return (-1);

	/* Close */
	(void)close(fd);

	/* XXX XXX XXX */
	return (0);
}


#endif /* ACORN */

/*
 * Convert a decimal to a single digit octal number
 */
static char octify(uint i)
{
	return (hexsym[i%8]);
}

/*
 * Convert a decimal to a single digit hex number
 */
static char hexify(uint i)
{
	return (hexsym[i%16]);
}


/*
 * Convert a octal-digit into a decimal
 */
static int deoct(char c)
{
	if (isdigit(c)) return (D2I(c));
	return (0);
}

/*
 * Convert a hexidecimal-digit into a decimal
 */
static int dehex(char c)
{
	if (isdigit(c)) return (D2I(c));
	if (islower(c)) return (A2I(c) + 10);
	if (isupper(c)) return (A2I(tolower(c)) + 10);
	return (0);
}


/*
 * Hack -- convert a printable string into real ascii
 *
 * I have no clue if this function correctly handles, for example,
 * parsing "\xFF" into a (signed) char.  Whoever thought of making
 * the "sign" of a "char" undefined is a complete moron.  Oh well.
 */
void text_to_ascii(char *buf, cptr str)
{
	char *s = buf;

	/* Analyze the "ascii" string */
	while (*str)
	{
		/* Backslash codes */
		if (*str == '\\')
		{
			/* Skip the backslash */
			str++;

			/* Hex-mode XXX */
			if (*str == 'x')
			{
				*s = 16 * dehex(*++str);
				*s++ += dehex(*++str);
			}

			/* Hack -- simple way to specify "backslash" */
			else if (*str == '\\')
			{
				*s++ = '\\';
			}

			/* Hack -- simple way to specify "caret" */
			else if (*str == '^')
			{
				*s++ = '^';
			}

			/* Hack -- simple way to specify "space" */
			else if (*str == 's')
			{
				*s++ = ' ';
			}

			/* Hack -- simple way to specify Escape */
			else if (*str == 'e')
			{
				*s++ = ESCAPE;
			}

			/* Backspace */
			else if (*str == 'b')
			{
				*s++ = '\b';
			}

			/* Newline */
			else if (*str == 'n')
			{
				*s++ = '\n';
			}

			/* Return */
			else if (*str == 'r')
			{
				*s++ = '\r';
			}

			/* Tab */
			else if (*str == 't')
			{
				*s++ = '\t';
			}

			/* Octal-mode */
			else if (*str == '0')
			{
				*s = 8 * deoct(*++str);
				*s++ += deoct(*++str);
			}

			/* Octal-mode */
			else if (*str == '1')
			{
				*s = 64 + 8 * deoct(*++str);
				*s++ += deoct(*++str);
			}

			/* Octal-mode */
			else if (*str == '2')
			{
				*s = 64 * 2 + 8 * deoct(*++str);
				*s++ += deoct(*++str);
			}

			/* Octal-mode */
			else if (*str == '3')
			{
				*s = 64 * 3 + 8 * deoct(*++str);
				*s++ += deoct(*++str);
			}

			/* Skip the final char */
			str++;
		}

		/* Normal Control codes */
		else if (*str == '^')
		{
			str++;
			*s++ = (*str++ & 037);
		}

		/* Normal chars */
		else
		{
			*s++ = *str++;
		}
	}

	/* Terminate */
	*s = '\0';
}


/*
 * Hack -- convert a string into a printable form
 */
void ascii_to_text(char *buf, cptr str)
{
	char *s = buf;

	/* Analyze the "ascii" string */
	while (*str)
	{
		byte i = (byte)(*str++);

		if (i == ESCAPE)
		{
			*s++ = '\\';
			*s++ = 'e';
		}
		else if (i == ' ')
		{
			*s++ = '\\';
			*s++ = 's';
		}
		else if (i == '\b')
		{
			*s++ = '\\';
			*s++ = 'b';
		}
		else if (i == '\t')
		{
			*s++ = '\\';
			*s++ = 't';
		}
		else if (i == '\n')
		{
			*s++ = '\\';
			*s++ = 'n';
		}
		else if (i == '\r')
		{
			*s++ = '\\';
			*s++ = 'r';
		}
		else if (i == '^')
		{
			*s++ = '\\';
			*s++ = '^';
		}
		else if (i == '\\')
		{
			*s++ = '\\';
			*s++ = '\\';
		}
		else if (i < 32)
		{
			*s++ = '^';
			*s++ = i + 64;
		}
		else if (i < 127)
		{
			*s++ = i;
		}
		else if (i < 64)
		{
			*s++ = '\\';
			*s++ = '0';
			*s++ = octify(i / 8);
			*s++ = octify(i % 8);
		}
		else
		{
			*s++ = '\\';
			*s++ = 'x';
			*s++ = hexify(i / 16);
			*s++ = hexify(i % 16);
		}
	}

	/* Terminate */
	*s = '\0';
}

/*
 * Local variable -- we are inside a "control-underscore" sequence
 */
/*static bool parse_under = FALSE;*/

/*
 * Local variable -- we are inside a "control-backslash" sequence
 */
/*static bool parse_slash = FALSE;*/

/*
 * Local variable -- we are stripping symbols for a while
 */
/*static bool strip_chars = FALSE;*/

/*
 * Flush the screen, make a noise
 */
void bell(void)
{
}


/*
 * Mega-Hack -- Make a (relevant?) sound
 */
void sound(int Ind, int val)
{
	/* Make a sound */
	Send_sound(Ind, val);
}

/*
 * We use a global array for all inscriptions to reduce the memory
 * spent maintaining inscriptions.  Of course, it is still possible
 * to run out of inscription memory, especially if too many different
 * inscriptions are used, but hopefully this will be rare.
 *
 * We use dynamic string allocation because otherwise it is necessary
 * to pre-guess the amount of quark activity.  We limit the total
 * number of quarks, but this is much easier to "expand" as needed.
 *
 * Any two items with the same inscription will have the same "quark"
 * index, which should greatly reduce the need for inscription space.
 *
 * Note that "quark zero" is NULL and should not be "dereferenced".
 */

/*
 * Add a new "quark" to the set of quarks.
 */
s16b quark_add(cptr str)
{
	int i;

	/* Look for an existing quark */
	for (i = 1; i < quark__num; i++)
	{
		/* Check for equality */
		if (streq(quark__str[i], str)) return (i);
	}

	/* Paranoia -- Require room */
	if (quark__num == QUARK_MAX) return (0);

	/* New maximal quark */
	quark__num = i + 1;

	/* Add a new quark */
	quark__str[i] = string_make(str);

	/* Return the index */
	return (i);
}


/*
 * This function looks up a quark
 */
cptr quark_str(s16b i)
{
	cptr q;

	/* Verify */
	if ((i < 0) || (i >= quark__num)) i = 0;

	/* Access the quark */
	q = quark__str[i];

	/* Return the quark */
	return (q);
}

/*
 * Check to make sure they haven't inscribed an item against what
 * they are trying to do -Crimson
 * look for "!*Erm" type, and "!* !A !f" type.
 */

bool check_guard_inscription( s16b quark, char what ) {
    const char  *   ax;     
    ax=quark_str(quark);
    if( ax == NULL ) { return FALSE; };
    while( (ax=strchr(ax,'!')) != NULL ) {
	while( ax++ != NULL ) {
	    if (*ax==0)  {
		 return FALSE; /* end of quark, stop */
	    }
	    if (*ax==' ' || *ax=='@') {
		 break; /* end of segment, stop */
	    }
	    if (*ax==what) {
		return TRUE; /* exact match, stop */
	    }
	    if(*ax =='*') {
		/* why so much hassle? * = all, that's it */
/*		return TRUE; -- well, !'B'ash if it's on the ground sucks ;) */

		switch( what ) { /* check for paranoid tags */
		    case 'd': /* no drop */
		    case 'h': /* no house ( sell a a key ) */
		    case 'k': /* no destroy */
		    case 's': /* no sell */
		    case 'v': /* no thowing */
		    case '=': /* force pickup */
		    /* you forgot important ones */
//		    case 'w': /* no wear/wield */
		    case 't': /* no take off */
		      return TRUE;
		};
	    };  
	};  
    };  
    return FALSE;
}  


/*
 * Output a message to the top line of the screen.
 *
 * Break long messages into multiple pieces (40-72 chars).
 *
 * Allow multiple short messages to "share" the top line.
 *
 * Prompt the user to make sure he has a chance to read them.
 *
 * These messages are memorized for later reference (see above).
 *
 * We could do "Term_fresh()" to provide "flicker" if needed.
 *
 * XXX XXX XXX Note that we must be very careful about using the
 * "msg_print()" functions without explicitly calling the special
 * "msg_print(NULL)" function, since this may result in the loss
 * of information if the screen is cleared, or if anything is
 * displayed on the top line.
 *
 * XXX XXX XXX Note that "msg_print(NULL)" will clear the top line
 * even if no messages are pending.  This is probably a hack.
 */
bool suppress_message = FALSE;

void msg_print(int Ind, cptr msg)
{
	/* Pfft, sorry to bother you.... --JIR-- */
	if (suppress_message) return;

	/* Ahh, the beautiful simplicity of it.... --KLJ-- */
	Send_message(Ind, msg);
}

void msg_broadcast(int Ind, cptr msg)
{
	int i;
	
	/* Tell every player */
	for (i = 1; i <= NumPlayers; i++)
	{
		/* Skip disconnected players */
		if (Players[i]->conn == NOT_CONNECTED) 
			continue;
			
		/* Skip the specified player */
		if (i == Ind)
			continue;       
			
		/* Tell this one */
		msg_print(i, msg);
	 }
}

void msg_broadcast_format(int Ind, cptr fmt, ...)
{
//	int i;
	
	va_list vp;

	char buf[1024];

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);
	
	/* Format the args, save the length */
	(void)vstrnfmt(buf, 1024, fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	msg_broadcast(Ind, buf);
}

/* Send a formatted message only to admin chars.	-Jir- */
void msg_admin(cptr fmt, ...)
//void msg_admin(int Ind, cptr fmt, ...)
{
	int i;
	player_type *p_ptr;
	
	va_list vp;

	char buf[1024];

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);
	
	/* Format the args, save the length */
	(void)vstrnfmt(buf, 1024, fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Tell every admin */
	for (i = 1; i <= NumPlayers; i++)
	{
		p_ptr = Players[i];

		/* Skip disconnected players */
		if (p_ptr->conn == NOT_CONNECTED) 
			continue;
			

		/* Tell Mama */
		if (is_admin(p_ptr))
			msg_print(i, buf);
	 }
}



/*
 * Display a formatted message, using "vstrnfmt()" and "msg_print()".
 */
void msg_format(int Ind, cptr fmt, ...)
{
	va_list vp;

	char buf[1024];

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);
	
	/* Format the args, save the length */
	(void)vstrnfmt(buf, 1024, fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Display */
	msg_print(Ind, buf);
}


/*
 * Display a message to everyone who is in sight on another player.
 *
 * This is mainly used to keep other players advised of actions done
 * by a player.  The message is not sent to the player who performed
 * the action.
 */
void msg_print_near(int Ind, cptr msg)
{
	player_type *p_ptr = Players[Ind];
	int y, x, i;
	struct worldpos *wpos;
	wpos=&p_ptr->wpos;

	if(p_ptr->admin_dm) return;

	y = p_ptr->py;
	x = p_ptr->px;

	/* Check each player */
	for (i = 1; i <= NumPlayers; i++)
	{
		/* Check this player */
		p_ptr = Players[i];

		/* Make sure this player is in the game */
		if (p_ptr->conn == NOT_CONNECTED) continue;

		/* Don't send the message to the player who caused it */
		if (Ind == i) continue;

		/* Make sure this player is at this depth */
		if (!inarea(&p_ptr->wpos, wpos)) continue;

		/* Can he see this player? */
		if (p_ptr->cave_flag[y][x] & CAVE_VIEW)
		{
			/* Send the message */
			msg_print(i, msg);
		}
	}
}


/*
 * Same as above, except send a formatted message.
 */
void msg_format_near(int Ind, cptr fmt, ...)
{
	va_list vp;

	char buf[1024];

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Format the args, save the length */
	(void)vstrnfmt(buf, 1024, fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Display */
	msg_print_near(Ind, buf);
}

/* location-based */
void msg_print_near_site(int y, int x, worldpos *wpos, cptr msg)
{
	int i;
	player_type *p_ptr;

	/* Check each player */
	for (i = 1; i <= NumPlayers; i++)
	{
		/* Check this player */
		p_ptr = Players[i];

		/* Make sure this player is in the game */
		if (p_ptr->conn == NOT_CONNECTED) continue;

		/* Make sure this player is at this depth */
		if (!inarea(&p_ptr->wpos, wpos)) continue;

		/* Can (s)he see the site? */
		if (p_ptr->cave_flag[y][x] & CAVE_VIEW)
		{
			/* Send the message */
			msg_print(i, msg);
		}
	}
}

/*
 * Same as above, except send a formatted message.
 */
void msg_format_near_site(int y, int x, worldpos *wpos, cptr fmt, ...)
{
	va_list vp;

	char buf[1024];

	/* Begin the Varargs Stuff */
	va_start(vp, fmt);

	/* Format the args, save the length */
	(void)vstrnfmt(buf, 1024, fmt, vp);

	/* End the Varargs Stuff */
	va_end(vp);

	/* Display */
	msg_print_near_site(y, x, wpos, buf);
}

/*
 * Send a message about a monster to everyone who can see it.
 * Monster name is appended at the beginning. (XXX kludgie!)	- Jir -
 *
 * Example: msg_print_near_monster(m_idx, "wakes up.");
 */
/*
 * TODO: allow format
 * TODO: distinguish 'witnessing' and 'hearing'
 */
void msg_print_near_monster(int m_idx, cptr msg)
{
	int i;
	player_type *p_ptr;
	cave_type **zcave;
	char m_name[80];

	monster_type	*m_ptr = &m_list[m_idx];
	worldpos *wpos=&m_ptr->wpos;

	if(!(zcave=getcave(wpos))) return;


	/* Check each player */
	for (i = 1; i <= NumPlayers; i++)
	{
		/* Check this player */
		p_ptr = Players[i];

		/* Make sure this player is in the game */
		if (p_ptr->conn == NOT_CONNECTED) continue;

		/* Make sure this player is at this depth */
		if (!inarea(&p_ptr->wpos, wpos)) continue;

		/* Skip if not visible */
		if (!p_ptr->mon_vis[m_idx]) continue;

		/* Can he see this player? */
//		if (!p_ptr->cave_flag[y][x] & CAVE_VIEW) continue;

		/* Acquire the monster name */
		monster_desc(i, m_name, m_idx, 0);

		msg_format(i, "%^s %s", m_name, msg);
	}
}

/*
 * Dodge Chance Feedback.
 */
void use_ability_blade(int Ind)
{
	player_type *p_ptr = Players[Ind];
	int dun_level = getlevel(&p_ptr->wpos);
	int chance = p_ptr->dodge_chance - (dun_level * 5 / 6);

	if (chance < 0) chance = 0;
	if (chance > DODGE_MAX_CHANCE) chance = DODGE_MAX_CHANCE;	// see DODGE_MAX_CHANCE in melee1.c
	if (is_admin(p_ptr))
	{
		msg_format(Ind, "You have exactly %d%% chances of dodging a level %d monster.", chance, dun_level);
	}

	if (chance < 5)
	{
		msg_format(Ind, "You have almost no chance of dodging a level %d monster.", dun_level);
	}
	else if (chance < 10)
	{
		msg_format(Ind, "You have a slight chance of dodging a level %d monster.", dun_level);
	}
	else if (chance < 20)
	{
		msg_format(Ind, "You have a significant chance of dodging a level %d monster.", dun_level);
	}
	else if (chance < 40)
	{
		msg_format(Ind, "You have a large chance of dodging a level %d monster.", dun_level);
	}
	else if (chance < 70)
	{
		msg_format(Ind, "You have a high chance of dodging a level %d monster.", dun_level);
	}
	else
	{
		msg_format(Ind, "You will usually dodge succesfully a level %d monster.", dun_level);
	}

	return;
}



static int checkallow(char *buff, int pos){
	if(!pos) return(0);
	if(pos==1) return(buff[0]==' ' ? 0 : 1); /* allow things like brass lantern */
	if(buff[pos-1]==' ' || buff[pos-2]=='\377') return(0);	/* swearing in colour */
	return(1);
}

static int censor(char *line){
	int i, j;
	char *word;
	char lcopy[100];
	int level=0;
	strcpy(lcopy, line);
	for(i=0; lcopy[i]; i++){
		lcopy[i]=tolower(lcopy[i]);
	}
	for(i=0; strlen(swear[i].word); i++){
		if((word=strstr(lcopy, swear[i].word))){
			if(checkallow(lcopy, word-lcopy)) continue;
			word=(&line[(word-lcopy)]);
			for(j=0; j<strlen(swear[i].word); j++){
				word[j]='*';
			}
			level=MAX(level, swear[i].level);
		}
	}
	return(level);
}

/*
 * A message prefixed by a player name is sent only to that player.
 * Otherwise, it is sent to everyone.
 */
/*
 * This function is hacked *ALOT* to add extra-commands w/o
 * client change.		- Jir -
 */
static void player_talk_aux(int Ind, char *message)
{
 	int i, len, target = 0;
	char search[80], sender[80];
	player_type *p_ptr = Players[Ind], *q_ptr;
 	char *colon;
	bool me = FALSE;
	char c = 'B';
	int mycolor = 0;
	bool admin=0;
	bool broadcast = FALSE;

#ifdef TOMENET_WORLDS
	char tmessage[160];		/* TEMPORARY! We will not send the name soon */
#endif

	/* Get sender's name */
	if (Ind)
	{
		/* Get player name */
		strcpy(sender, p_ptr->name);
		admin = is_admin(p_ptr);
	}
	else
	{
		/* Default name */
		strcpy(sender, "Server Admin");
	}

	/* Default to no search string */
	strcpy(search, "");

	/* Look for a player's name followed by a colon */
	colon = strchr(message, ':');

	/* Ignore "smileys" or URL */
//	if (colon && strchr(")(-/:", *(colon + 1)))
	/* (C. Blue) changing colon parsing. :: becomes
	    textual :  - otherwise : stays control char */
	if (colon) {
		/* if another colon followed this one then
		   it was not meant to be a control char */
		switch(*(colon + 1)){
		/* accept these chars for smileys */
		case '(':	case ')':
		case '[':	case ']':
		case '{':	case '}':
		case '<':	case '>':
		case '\\':
			colon = NULL;
			break;
		case '-':
			if (!strchr("123456789", *(colon + 2))) colon = NULL;
			break;
		case '/':
		/* only accept / at the end of a chat message */
			if ('\0' == *(colon + 2)) colon = NULL;
			break;
		case ':':
			/* remove the 1st colon found */
			i = (int) (colon - message);
			do message[i] = message[i+1];
			while(message[i++]!='\0');

			/* Pretend colon wasn't there */
			colon = NULL;
			break;
		}
	}

	/* no big brother */
	if(cfg.log_u && !colon) s_printf("[%s] %s\n", sender, message);

	/* Special - shutdown command (for compatibility) */
	if (prefix(message, "@!shutdown") && admin)
	{
		/*world_reboot();*/
		shutdown_server();
		return;
	}
	
	if(message[0]=='/'){
		if(!strncmp(message, "/me", 3)) me=TRUE;
		else if(!strncmp(message, "/broadcast ", 11)) broadcast = TRUE;
		else{
			do_slash_cmd(Ind, message);	/* add check */
			return;
		}
	}
	
	p_ptr->msgcnt++;
	if(p_ptr->msgcnt>12){
		time_t last=p_ptr->msg;
		time(&p_ptr->msg);
		if(p_ptr->msg-last < 6){
			p_ptr->spam++;
			switch(p_ptr->spam){
				case 1:
					msg_print(Ind, "\377yPlease don't spam the server");
					break;
				case 3:
				case 4:
					msg_print(Ind, "\377rWarning! this behaviour is unacceptable!");
					break;
				case 5:
					p_ptr->chp=-3;
					strcpy(p_ptr->died_from, "hypoxia");
					p_ptr->spam=1;
					p_ptr->deathblow = 0;
					player_death(Ind);
					return;
			}
		}
		if(p_ptr->msg-last > 240 && p_ptr->spam) p_ptr->spam--;
		p_ptr->msgcnt=0;
	}
	if(p_ptr->spam > 1) return;

	process_hooks(HOOK_CHAT, "d", Ind);

	if(++p_ptr->talk>10){
		imprison(Ind, 30, "talking too much.");
		return;
	}

	for(i=1; i<=NumPlayers; i++){
		if(Players[i]->conn==NOT_CONNECTED) continue;
		Players[i]->talk=0;
	}

	/* Form a search string if we found a colon */
	if (colon)
	{
		if(p_ptr->inval){
			msg_print(Ind, "Your account is not valid! Ask an admin to validate it.");
			return;
		}
		/* Copy everything up to the colon to the search string */
		strncpy(search, message, colon - message);

		/* Add a trailing NULL */
		search[colon - message] = '\0';
	}

	/* Acquire length of search string */
	len = strlen(search);

	/* Look for a recipient who matches the search string */
	if (len)
	{
		struct rplist *w_player;
		if(!stricmp(search, "Guild")){
			if(!p_ptr->guild){
				msg_print(Ind, "You are not in a guild");
			}
			else guild_msg_format(p_ptr->guild, "\377v[\377w%s\377v]\377y %s", p_ptr->name, colon+1);
			return;
		}
		w_player=world_find_player(search, 0);
		/* NAME_LOOKUP_LOOSE DESPERATELY NEEDS WORK */
		if(!w_player)
			target = name_lookup_loose(Ind, search, TRUE);
		else{
			world_pmsg_send(p_ptr->id, p_ptr->name, w_player->name, colon+1);
			msg_format(Ind, "\377o[%s:%s] %s", p_ptr->name, w_player->name, colon+1);
			return;
		}

		/* Move colon pointer forward to next word */
		while (*colon && (isspace(*colon) || *colon == ':')) colon++;

		/* lookup failed */
		if (!target)
		{
			/* Bounce message to player who sent it */
			msg_format(Ind, "(no receipient for: %s)", colon);

			/* Give up */
			return;
		}
	}

	/* Send to appropriate player */
	if ((len && target > 0) && (!check_ignore(target, Ind)))
	{
		/* Set target player */
		q_ptr = Players[target];

		/* Send message to target */
		msg_format(target, "\377g[%s:%s] %s", q_ptr->name, sender, colon);

		/* Also send back to sender */
		msg_format(Ind, "\377g[%s:%s] %s", q_ptr->name, sender, colon);

		if (q_ptr->afk) msg_format(Ind, "\377o%s seems to be Away From Keyboard.", q_ptr->name);

		/* Done */
		return;
	}

	/* Send to appropriate party */
	if (len && target < 0)
	{
		/* Send message to target party */
		party_msg_format_ignoring(Ind, 0 - target, "\377G[%s:%s] %s",
				 parties[0 - target].name, sender, colon);

		/* Also send back to sender if not in that party */
		if(!player_in_party(0-target, Ind)){
			msg_format(Ind, "\377G[%s:%s] %s",
			   parties[0 - target].name, sender, colon);
		}

		/* Done */
		return;
	}

	if (strlen(message) > 1) mycolor = (prefix(message, "}") && (color_char_to_attr(*(message + 1)) != -1))?2:0;

	if (!Ind) c = 'y';
	/* Disabled this for now to avoid confusion. */
	else if (mycolor) c = *(message + 1);
	else
	{
		if (p_ptr->mode & MODE_HELL) c = 'W';
		if (p_ptr->mode & MODE_NO_GHOST) c = 'D';
		if (p_ptr->total_winner) c = 'v';
		else if (p_ptr->ghost) c = 'r';
		/* Dungeon Master / Dungeon Wizard have their own colour now :) */
		if (is_admin(p_ptr)) c = 'b';
	}
	switch((i=censor(message))){
		case 0:
			break;
		default:
			imprison(Ind, i*20, "swearing");
		case 1:	msg_print(Ind, "Please do not swear");
	}

#ifdef TOMENET_WORLDS
	if(broadcast)
		sprintf(tmessage, "\377r[\377%c%s\377r] \377B%s", c, sender, message + 11);
	else if (!me)
		sprintf(tmessage, "\377%c[%s] \377B%s", c, sender, message + mycolor);
	else{
		/* Why not... */
		if (strlen(message) > 4) mycolor = (prefix(&message[4], "}") && (color_char_to_attr(*(message + 5)) != -1))?2:0;
		else return;
		if(mycolor) c=message[5];
		sprintf(tmessage, "\377%c[%s %s]", c, sender, message + 4+mycolor);
	}
	world_chat(p_ptr->id, tmessage);	/* no ignores... */
	for(i = 1; i <= NumPlayers; i++){
		q_ptr=Players[i];

		if (!admin)
		{
			if (check_ignore(i, Ind)) continue;
			if (!broadcast && (p_ptr->limit_chat || q_ptr->limit_chat) &&
					!inarea(&p_ptr->wpos, &q_ptr->wpos)) continue;
		}
		msg_print(i, tmessage);
	}
#else
	/* Send to everyone */
	for (i = 1; i <= NumPlayers; i++)
	{
		q_ptr = Players[i];

		if (!admin)
		{
			if (check_ignore(i, Ind)) continue;
			if (!broadcast && (p_ptr->limit_chat || q_ptr->limit_chat) &&
					!inarea(&p_ptr->wpos, &q_ptr->wpos)) continue;
		}

		/* Send message */
		if (broadcast)
			msg_format(i, "\377r[\377%c%s\377r] \377B%s", c, sender, message + 11);
		else if (!me)
		{
			msg_format(i, "\377%c[%s] \377B%s", c, sender, message + mycolor);
			/* msg_format(i, "\377%c[%s] %s", Ind ? 'B' : 'y', sender, message); */
		} 
		else msg_format(i, "%s %s", sender, message + 4);
	}
#endif
}

/*
 * toggle AFK mode on/off.	- Jir -
 */
void toggle_afk(int Ind, char *msg)
{
	player_type *p_ptr = Players[Ind];

	if (p_ptr->afk)
	{
		if (strlen(p_ptr->afk_msg) == 0)
			msg_print(Ind, "AFK mode is turned \377GOFF\377w.");
		else
			msg_format(Ind, "AFK mode is turned \377GOFF\377w. (%s)", p_ptr->afk_msg);
		if (!p_ptr->admin_dm)
		{
			if (strlen(p_ptr->afk_msg) == 0)
				msg_broadcast(Ind, format("\377o%s has returned from AFK.", p_ptr->name));
			else
				msg_broadcast(Ind, format("\377o%s has returned from AFK. (%s)", p_ptr->name, p_ptr->afk_msg));
		}
		p_ptr->afk = FALSE;
	}
	else
	{
	        /* Stop searching */
	        if (p_ptr->searching)
		{
			/* Clear the searching flag */
			p_ptr->searching = FALSE;

			/* Recalculate bonuses */
			p_ptr->update |= (PU_BONUS);

		        /* Redraw the state */
		        p_ptr->redraw |= (PR_STATE);
		}

		strcpy(p_ptr->afk_msg, msg);
		if (strlen(p_ptr->afk_msg) == 0)
			msg_print(Ind, "AFK mode is turned \377rON\377w.");
		else
			msg_format(Ind, "AFK mode is turned \377rON\377w. (%s)", p_ptr->afk_msg);
		if (!p_ptr->admin_dm)
		{
			if (strlen(p_ptr->afk_msg) == 0)
				msg_broadcast(Ind, format("\377o%s seems to be AFK now.", p_ptr->name));
			else
				msg_broadcast(Ind, format("\377o%s seems to be AFK now. (%s)", p_ptr->name, p_ptr->afk_msg));
		}
		p_ptr->afk = TRUE;

		/* still too many starvations, so give a warning - C. Blue */
		if (p_ptr->food < PY_FOOD_ALERT) msg_print(Ind, "\377RWARNING: Going AFK while hungry or weak can result in starvation! Eat first!");
	}
	return;
}

/*
 * A player has sent a message to the rest of the world.
 *
 * Parse it and send to everyone or to only the person(s) he requested.
 *
 * Note that more than one message may get sent at once, seperated by
 * tabs ('\t').  Thus, this function splits them and calls
 * "player_talk_aux" to do the dirty work.
 */
void player_talk(int Ind, char *message)
{
	char *cur, *next;

	/* Start at the beginning */
	cur = message;

	/* Process until out of messages */
	while (cur)
	{
		/* Find the next tab */
		next = strchr(cur, '\t');

		/* Stop out the tab */
		if (next)
		{
			/* Replace with \0 */
			*next = '\0';
		}

		/* Process this message */
		player_talk_aux(Ind, cur);

		/* Move to the next one */
		if (next)
		{
			/* One step past the \0 */
			cur = next + 1;
		}
		else
		{
			/* No more message */
			cur = NULL;
		}
	}
}
	

/*
 * Check a char for "vowel-hood"
 */
bool is_a_vowel(int ch)
{
	switch (ch)
	{
		case 'a':
		case 'e':
		case 'i':
		case 'o':
		case 'u':
		case 'A':
		case 'E':
		case 'I':
		case 'O':
		case 'U':
		return (TRUE);
	}

	return (FALSE);
}

/*
 * Look up a player/party name, allowing abbreviations.  - Jir -
 * (looking up party name this way can be rather annoying though)
 *
 * returns 0 if not found/error(, minus value if party.)
 *
 * if 'party' is TRUE, party name is also looked up.
 */
int name_lookup_loose(int Ind, cptr name, bool party)
{
	int i, len, target = 0;
	player_type *q_ptr, *p_ptr;
	cptr problem = "";

	p_ptr=Players[Ind];

	/* don't waste time */
	if(p_ptr==(player_type*)NULL) return(0);

	/* Acquire length of search string */
	len = strlen(name);

	/* Look for a recipient who matches the search string */
	if (len)
	{
		if (party)
		{
			/* First check parties */
			for (i = 1; i < MAX_PARTIES; i++)
			{
				/* Skip if empty */
				if (!parties[i].members) continue;

				/* Check name */
				if (!strncasecmp(parties[i].name, name, len))
				{
					/* Set target if not set already */
					if (!target)
					{
						target = 0 - i;
					}
					else
					{
						/* Matching too many parties */
						problem = "parties";
					}

					/* Check for exact match */
					if (len == strlen(parties[i].name))
					{
						/* Never a problem */
						target = 0 - i;
						problem = "";

						/* Finished looking */
						break;
					}
				}
			}
		}

		/* Then check players */
		for (i = 1; i <= NumPlayers; i++)
		{
			/* Check this one */
			q_ptr = Players[i];

			/* Skip if disconnected */
			if (q_ptr->conn == NOT_CONNECTED) continue;

			/* let admins chat */
			if (q_ptr->admin_dm && !is_admin(p_ptr)) continue;

			/* Check name */
			if (!strncasecmp(q_ptr->name, name, len))
			{
				/* Set target if not set already */
				if (!target)
				{
					target = i;
				}
				else
				{
					/* Matching too many people */
					problem = "players";
				}

				/* Check for exact match */
				if (len == strlen(q_ptr->name))
				{
					/* Never a problem */
					target = i;
					problem = "";

					/* Finished looking */
					break;
				}
			}
		}
	}

	/* Check for recipient set but no match found */
	if ((len && !target) || (target < 0 && !party))
	{
		/* Send an error message */
		msg_format(Ind, "Could not match name '%s'.", name);

		/* Give up */
		return 0;
	}

	/* Check for multiple recipients found */
	if (strlen(problem))
	{
		/* Send an error message */
		msg_format(Ind, "'%s' matches too many %s.", name, problem);

		/* Give up */
		return 0;
	}

	/* Success */
	return target;
}

/*
 * Convert bracer '{' into '\377'
 */
void bracer_ff(char *buf)
{
	int i, len = strlen(buf);

	for(i=0;i<len;i++){
		if(buf[i]=='{') buf[i]='\377';
	}
}

/*
 * make strings from worldpos '-1550ft of (17,15)'	- Jir -
 */
char *wpos_format(int Ind, worldpos *wpos)
{
	if (!Ind || Players[Ind]->depth_in_feet)
	return (format("%dft of (%d,%d)", wpos->wz * 50, wpos->wx, wpos->wy));
	else return (format("Lev %d of (%d,%d)", wpos->wz, wpos->wx, wpos->wy));
}


byte count_bits(u32b array)
{
	byte k = 0, i;        

	if(array)
		for(i = 0; i < 32; i++)
			if(array & (1 << i)) k++;

	return k;
}

/*
 * Find a player
 */
int get_playerind(char *name)
{
        int i;

        if (name == (char*)NULL)
                return(-1);
        for(i=1; i<=NumPlayers; i++)
        {
                if(Players[i]->conn==NOT_CONNECTED) continue;
                if(!stricmp(Players[i]->name, name)) return(i);
        }
        return(-1);
}

/*
 * Tell the player of the floor feeling.	- Jir -
 * NOTE: differs from traditional 'boring' etc feeling!
 */
bool show_floor_feeling(int Ind)
{
	player_type *p_ptr = Players[Ind];
	worldpos *wpos = &p_ptr->wpos;
	dun_level *l_ptr = getfloor(wpos);
	bool felt = FALSE;

	/* XXX devise a better formula */
	if (p_ptr->lev * ((p_ptr->lev >= 40) ? 3 : 2) + 5 < getlevel(wpos))
	{
		msg_print(Ind, "\377oYou feel an imminent danger!");
		felt = TRUE;
	}

	if (!l_ptr) return(felt);

	/* Hack^2 -- display the 'feeling' */
	if (l_ptr->flags1 & LF1_NO_MAGIC)
		msg_print(Ind, "\377oYou feel a suppressive air...");
	if (l_ptr->flags1 & LF1_NO_GENO)
		msg_print(Ind, "\377oYou have a feeling of peace...");
	if (l_ptr->flags1 & LF1_NOMAP)
		msg_print(Ind, "\377oYou lose all sense of direction...");
	if (l_ptr->flags1 & LF1_NO_MAGIC_MAP)
		msg_print(Ind, "\377oYou feel like a stranger...");
	if (l_ptr->flags1 & LF1_NO_DESTROY)
		msg_print(Ind, "\377oThe walls here seem very solid.");
	if (l_ptr->flags1 & LF1_NO_GHOST)
		msg_print(Ind, "\377oYou feel that your life hangs in the balance!");
#if 0
	if (l_ptr->flags1 & DF1_NO_RECALL)
		msg_print(Ind, "\377oThere is strong magic enclosing this dungeon.");
#endif
	return(l_ptr->flags1 & LF1_FEELING_MASK ? TRUE : felt);
}

/*
 * Given item name as string, return the index in k_info array. Name
 * must exactly match (look out for commas and the like!), or else 0 is 
 * returned. Case doesn't matter. -DG-
 */

int test_item_name(cptr name)
{
       int i;

       /* Scan the items */
       for (i = 1; i < max_k_idx; i++)
       {
		object_kind *k_ptr = &k_info[i];
		cptr obj_name = k_name + k_ptr->name;

		/* If name matches, give us the number */
		if (stricmp(name, obj_name) == 0) return (i);
       }
       return (0);
}

/* 
 * Middle-Earth (Imladris) calendar code from ToME
 */
/*
 * Break scalar time
 */
s32b bst(s32b what, s32b t)
{
	s32b turns = t + (10 * DAY_START);

	switch (what)
	{
		case MINUTE:
			return ((turns / 10 / MINUTE) % 60);
		case HOUR:
			return (turns / 10 / (HOUR) % 24);
		case DAY:
			return (turns / 10 / (DAY) % 365);
		case YEAR:
			return (turns / 10 / (YEAR));
		default:
			return (0);
	}
}	    

cptr get_month_name(int day, bool full, bool compact)
{
	int i = 8;
	static char buf[40];

	/* Find the period name */
	while ((i > 0) && (day < month_day[i]))
	{
		i--;
	}

	switch (i)
	{
		/* Yestare/Mettare */
		case 0:
		case 8:
		{
			char buf2[20];

			sprintf(buf2, get_day(day + 1));
			if (full) sprintf(buf, "%s (%s day)", month_name[i], buf2);
			else sprintf(buf, "%s", month_name[i]);
			break;
		}
		/* 'Normal' months + Enderi */
		default:
		{
			char buf2[20];
			char buf3[20];

			sprintf(buf2, get_day(day + 1 - month_day[i]));
			sprintf(buf3, get_day(day + 1));

			if (full) sprintf(buf, "%s day of %s (%s day)", buf2, month_name[i], buf3);
			else if (compact) sprintf(buf, "%s day of %s", buf2, month_name[i]);
			else sprintf(buf, "%s %s", buf2, month_name[i]);
			break;
		}
	}

	return (buf);
}

cptr get_day(int day)
{
	static char buf[20];
	cptr p = "th";

	if ((day / 10) == 1) ;
	else if ((day % 10) == 1) p = "st";
	else if ((day % 10) == 2) p = "nd";
	else if ((day % 10) == 3) p = "rd";

	sprintf(buf, "%d%s", day, p);
	return (buf);
}

int gold_colour(int amt)
{
	int i, unit = 1;

	for (i = amt; i > 99 ; i >>= 1, unit++) /* naught */;
	if (unit > SV_GOLD_MAX) unit = SV_GOLD_MAX;
	return (lookup_kind(TV_GOLD, unit));
}

/* Catching bad players who hack their client.. nasty! */
void lua_intrusion(int Ind, char *problem_diz)
{
	s_printf(format("LUA INTRUSION: %s : %s\n", Players[Ind]->name, problem_diz));
	take_hit(Ind, Players[Ind]->chp - 1, "");
	msg_print(Ind, "\377rThat was close huh?!");
}
