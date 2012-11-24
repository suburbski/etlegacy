/*
 * Wolfenstein: Enemy Territory GPL Source Code
 * Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.
 *
 * ET: Legacy
 * Copyright (C) 2012 Jan Simek <mail@etlegacy.com>
 *
 * This file is part of ET: Legacy - http://www.etlegacy.com
 *
 * ET: Legacy is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ET: Legacy is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ET: Legacy. If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, Wolfenstein: Enemy Territory GPL Source Code is also
 * subject to certain additional terms. You should have received a copy
 * of these additional terms immediately following the terms and conditions
 * of the GNU General Public License which accompanied the source code.
 * If not, please request a copy in writing from id Software at the address below.
 *
 * id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
 *
 * @file q_shared.c
 * @brief stateless support routines that are included in each code dll
 */

#include "q_shared.h"

float Com_Clamp(float min, float max, float value)
{
	if (value < min)
	{
		return min;
	}
	if (value > max)
	{
		return max;
	}
	return value;
}

/*
 * @brief unixifies a pathname
 */
void COM_FixPath(char *pathname)
{
	while (*pathname)
	{
		if (*pathname == '\\')
		{
			*pathname = '/';
		}
		pathname++;
	}
}

/*
============
COM_SkipPath
============
*/
char *COM_SkipPath(char *pathname)
{
	char *last;

	last = pathname;
	while (*pathname)
	{
		if (*pathname == '/')
		{
			last = pathname + 1;
		}
		pathname++;
	}
	return last;
}

/*
============
COM_GetExtension
============
*/
const char *COM_GetExtension(const char *name)
{
	int length, i;

	length = strlen(name) - 1;
	i      = length;

	while (name[i] != '.')
	{
		i--;
		if (name[i] == '/' || i == 0)
		{
			return ""; // no extension
		}
	}

	return &name[i + 1];
}

/*
============
COM_StripExtension
============
*/
void COM_StripExtension(const char *in, char *out, int destsize)
{
	const char *dot = strrchr(in, '.'), *slash;

	if (dot && (!(slash = strrchr(in, '/')) || slash < dot))
	{
		Q_strncpyz(out, in, (destsize < dot - in + 1 ? destsize : dot - in + 1));
	}
	else
	{
		Q_strncpyz(out, in, destsize);
	}

	// DEBUG
	//Com_Printf("##########################################\nCOM_StripExtension: in  %s\n", in);
	//Com_Printf("COM_StripExtension: out %s\n##########################################", out);
}

void COM_StripFilename(char *in, char *out)
{
	char *end;
	Q_strncpyz(out, in, strlen(in) + 1);
	end  = COM_SkipPath(out);
	*end = 0;
}

/*
 * @brief if path doesn't have an extension, then append the specified one
 * (which should include the .)
 */
void COM_DefaultExtension(char *path, int maxSize, const char *extension)
{
	const char *dot = strrchr(path, '.'), *slash;
	if (dot && (!(slash = strrchr(path, '/')) || slash < dot))
	{
		return;
	}
	else
	{
		Q_strcat(path, maxSize, extension);
	}
}

//============================================================================
/*
==================
COM_BitCheck

  Allows bit-wise checks on arrays with more than one item (> 32 bits)
==================
*/
qboolean COM_BitCheck(const int array[], int bitNum)
{
	int i;

	i = 0;
	while (bitNum > 31)
	{
		i++;
		bitNum -= 32;
	}

	return ((array[i] & (1 << bitNum)) != 0);        // (SA) heh, whoops. :)
}

/*
==================
COM_BitSet

  Allows bit-wise SETS on arrays with more than one item (> 32 bits)
==================
*/
void COM_BitSet(int array[], int bitNum)
{
	int i;

	i = 0;
	while (bitNum > 31)
	{
		i++;
		bitNum -= 32;
	}

	array[i] |= (1 << bitNum);
}

/*
==================
COM_BitClear

  Allows bit-wise CLEAR on arrays with more than one item (> 32 bits)
==================
*/
void COM_BitClear(int array[], int bitNum)
{
	int i;

	i = 0;
	while (bitNum > 31)
	{
		i++;
		bitNum -= 32;
	}

	array[i] &= ~(1 << bitNum);
}
//============================================================================

/*
============================================================================

                    BYTE ORDER FUNCTIONS

============================================================================
*/
/*
// can't just use function pointers, or dll linkage can
// mess up when qcommon is included in multiple places
static short ( *_BigShort )( short l ) = NULL;
static short ( *_LittleShort )( short l ) = NULL;
static int ( *_BigLong )( int l ) = NULL;
static int ( *_LittleLong )( int l ) = NULL;
static qint64 ( *_BigLong64 )( qint64 l ) = NULL;
static qint64 ( *_LittleLong64 )( qint64 l ) = NULL;
static float ( *_BigFloat )( float l ) = NULL;
static float ( *_LittleFloat )( float l ) = NULL;

short   LittleShort( short l ) {return _LittleShort( l );}
int     LittleLong( int l ) {return _LittleLong( l );}
qint64  LittleLong64( qint64 l ) {return _LittleLong64( l );}
float   LittleFloat( float l ) {return _LittleFloat( l );}

short   BigShort( short l ) {return _BigShort( l );}
int     BigLong( int l ) {return _BigLong( l );}
qint64  BigLong64( qint64 l ) {return _BigLong64( l );}
float   BigFloat( float l ) {return _BigFloat( l );}
*/

short   ShortSwap(short l)
{
	byte b1, b2;

	b1 = l & 255;
	b2 = (l >> 8) & 255;

	return (b1 << 8) + b2;
}

short   ShortNoSwap(short l)
{
	return l;
}

int    LongSwap(int l)
{
	byte b1, b2, b3, b4;

	b1 = l & 255;
	b2 = (l >> 8) & 255;
	b3 = (l >> 16) & 255;
	b4 = (l >> 24) & 255;

	return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
}

int LongNoSwap(int l)
{
	return l;
}

qint64 Long64Swap(qint64 ll)
{
	qint64 result;

	result.b0 = ll.b7;
	result.b1 = ll.b6;
	result.b2 = ll.b5;
	result.b3 = ll.b4;
	result.b4 = ll.b3;
	result.b5 = ll.b2;
	result.b6 = ll.b1;
	result.b7 = ll.b0;

	return result;
}

qint64 Long64NoSwap(qint64 ll)
{
	return ll;
}

float FloatSwap(const float *f)
{
	floatint_t out;

	out.f  = *f;
	out.ui = LongSwap(out.ui);

	return out.f;
}

float FloatNoSwap(float f)
{
	return f;
}

/*
================
Swap_Init
================
*/
/*
void Swap_Init( void ) {
    byte swaptest[2] = {1,0};

// set the byte swapping variables in a portable manner
    if ( *(short *)swaptest == 1 ) {
        _BigShort = ShortSwap;
        _LittleShort = ShortNoSwap;
        _BigLong = LongSwap;
        _LittleLong = LongNoSwap;
        _BigLong64 = Long64Swap;
        _LittleLong64 = Long64NoSwap;
        _BigFloat = FloatSwap;
        _LittleFloat = FloatNoSwap;
    } else
    {
        _BigShort = ShortNoSwap;
        _LittleShort = ShortSwap;
        _BigLong = LongNoSwap;
        _LittleLong = LongSwap;
        _BigLong64 = Long64NoSwap;
        _LittleLong64 = Long64Swap;
        _BigFloat = FloatNoSwap;
        _LittleFloat = FloatSwap;
    }

}
*/


/*
============================================================================

PARSING

============================================================================
*/

static char com_token[MAX_TOKEN_CHARS];
static char com_parsename[MAX_TOKEN_CHARS];
static int  com_lines;

static int  backup_lines;
static char *backup_text;

void COM_BeginParseSession(const char *name)
{
	com_lines = 0;
	Com_sprintf(com_parsename, sizeof(com_parsename), "%s", name);
}

void COM_BackupParseSession(char **data_p)
{
	backup_lines = com_lines;
	backup_text  = *data_p;
}

void COM_RestoreParseSession(char **data_p)
{
	com_lines = backup_lines;
	*data_p   = backup_text;
}

void COM_SetCurrentParseLine(int line)
{
	com_lines = line;
}

int COM_GetCurrentParseLine(void)
{
	return com_lines;
}

char *COM_Parse(char **data_p)
{
	return COM_ParseExt(data_p, qtrue);
}

void COM_ParseError(char *format, ...)
{
	va_list     argptr;
	static char string[4096];

	va_start(argptr, format);
	Q_vsnprintf(string, sizeof(string), format, argptr);
	va_end(argptr);

	Com_Printf("ERROR: %s, line %d: %s\n", com_parsename, com_lines, string);
}

/**
 * @note Unused.
 */
void COM_ParseWarning(char *format, ...)
{
	va_list     argptr;
	static char string[4096];

	va_start(argptr, format);
	Q_vsnprintf(string, sizeof(string), format, argptr);
	va_end(argptr);

	Com_Printf("WARNING: %s, line %d: %s\n", com_parsename, com_lines, string);
}

/*
==============
COM_Parse

Parse a token out of a string
Will never return NULL, just empty strings

If "allowLineBreaks" is qtrue then an empty
string will be returned if the next token is
a newline.
==============
*/
static char *SkipWhitespace(char *data, qboolean *hasNewLines)
{
	int c;

	while ((c = *data) <= ' ')
	{
		if (!c)
		{
			return NULL;
		}
		if (c == '\n')
		{
			com_lines++;
			*hasNewLines = qtrue;
		}
		data++;
	}

	return data;
}

int COM_Compress(char *data_p)
{
	char     *in, *out;
	int      c;
	qboolean newline = qfalse, whitespace = qfalse;

	in = out = data_p;
	if (in)
	{
		while ((c = *in) != 0)
		{
			// skip double slash comments
			if (c == '/' && in[1] == '/')
			{
				while (*in && *in != '\n')
				{
					in++;
				}
				// skip /* */ comments
			}
			else if (c == '/' && in[1] == '*')
			{
				while (*in && (*in != '*' || in[1] != '/'))
					in++;
				if (*in)
				{
					in += 2;
				}
				// record when we hit a newline
			}
			else if (c == '\n' || c == '\r')
			{
				newline = qtrue;
				in++;
				// record when we hit whitespace
			}
			else if (c == ' ' || c == '\t')
			{
				whitespace = qtrue;
				in++;
				// an actual token
			}
			else
			{
				// if we have a pending newline, emit it (and it counts as whitespace)
				if (newline)
				{
					*out++     = '\n';
					newline    = qfalse;
					whitespace = qfalse;
				}
				if (whitespace)
				{
					*out++     = ' ';
					whitespace = qfalse;
				}

				// copy quoted strings unmolested
				if (c == '"')
				{
					*out++ = c;
					in++;
					while (1)
					{
						c = *in;
						if (c && c != '"')
						{
							*out++ = c;
							in++;
						}
						else
						{
							break;
						}
					}
					if (c == '"')
					{
						*out++ = c;
						in++;
					}
				}
				else
				{
					*out = c;
					out++;
					in++;
				}
			}
		}

		*out = 0;
	}
	return out - data_p;
}

char *COM_ParseExt(char **data_p, qboolean allowLineBreaks)
{
	int      c           = 0, len;
	qboolean hasNewLines = qfalse;
	char     *data;

	data         = *data_p;
	len          = 0;
	com_token[0] = 0;

	// make sure incoming data is valid
	if (!data)
	{
		*data_p = NULL;
		return com_token;
	}

	// RF, backup the session data so we can unget easily
	COM_BackupParseSession(data_p);

	while (1)
	{
		// skip whitespace
		data = SkipWhitespace(data, &hasNewLines);
		if (!data)
		{
			*data_p = NULL;
			return com_token;
		}
		if (hasNewLines && !allowLineBreaks)
		{
			*data_p = data;
			return com_token;
		}

		c = *data;

		// skip double slash comments
		if (c == '/' && data[1] == '/')
		{
			data += 2;
			while (*data && *data != '\n')
			{
				data++;
			}
		}
		// skip /* */ comments
		else if (c == '/' && data[1] == '*')
		{
			data += 2;
			while (*data && (*data != '*' || data[1] != '/'))
			{
				data++;
			}
			if (*data)
			{
				data += 2;
			}
		}
		else
		{
			break;
		}
	}

	// handle quoted strings
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c == '\\' && *(data) == '\"')
			{
				// Arnout: string-in-string
				if (len < MAX_TOKEN_CHARS)
				{
					com_token[len] = '\"';
					len++;
				}
				data++;

				while (1)
				{
					c = *data++;

					if (!c)
					{
						com_token[len] = 0;
						*data_p        = ( char * ) data;
						break;
					}
					if ((c == '\\' && *(data) == '\"'))
					{
						if (len < MAX_TOKEN_CHARS)
						{
							com_token[len] = '\"';
							len++;
						}
						data++;
						c = *data++;
						break;
					}
					if (len < MAX_TOKEN_CHARS - 1)
					{
						com_token[len] = c;
						len++;
					}
				}
			}
			if (c == '\"' || !c)
			{
				com_token[len] = 0;
				*data_p        = ( char * ) data;
				return com_token;
			}
			if (len < MAX_TOKEN_CHARS - 1)
			{
				com_token[len] = c;
				len++;
			}
		}
	}

	// parse a regular word
	do
	{
		if (len < MAX_TOKEN_CHARS - 1)
		{
			com_token[len] = c;
			len++;
		}
		data++;
		c = *data;
		if (c == '\n')
		{
			com_lines++;
		}
	}
	while (c > 32);

	com_token[len] = 0;

	*data_p = ( char * ) data;
	return com_token;
}

/*
==================
COM_MatchToken
==================
*/
void COM_MatchToken(char **buf_p, char *match)
{
	char *token;

	token = COM_Parse(buf_p);
	if (strcmp(token, match))
	{
		Com_Error(ERR_DROP, "COM_MatchToken: %s != %s\n", token, match);
	}
}

/*
=================
SkipBracedSection_Depth

=================
*/
void SkipBracedSection_Depth(char **program, int depth)
{
	char *token;

	do
	{
		token = COM_ParseExt(program, qtrue);
		if (token[1] == 0)
		{
			if (token[0] == '{')
			{
				depth++;
			}
			else if (token[0] == '}')
			{
				depth--;
			}
		}
	}
	while (depth && *program);
}

/*
=================
SkipBracedSection

The next token should be an open brace.
Skips until a matching close brace is found.
Internal brace depths are properly skipped.
=================
*/
void SkipBracedSection(char **program)
{
	char *token;
	int  depth;

	depth = 0;
	do
	{
		token = COM_ParseExt(program, qtrue);
		if (token[1] == 0)
		{
			if (token[0] == '{')
			{
				depth++;
			}
			else if (token[0] == '}')
			{
				depth--;
			}
		}
	}
	while (depth && *program);
}

/*
=================
SkipRestOfLine
=================
*/
void SkipRestOfLine(char **data)
{
	char *p;
	int  c;

	p = *data;
	while ((c = *p++) != 0)
	{
		if (c == '\n')
		{
			com_lines++;
			break;
		}
	}

	*data = p;
}


void Parse1DMatrix(char **buf_p, int x, float *m)
{
	char *token;
	int  i;

	COM_MatchToken(buf_p, "(");

	for (i = 0 ; i < x ; i++)
	{
		token = COM_Parse(buf_p);
		m[i]  = atof(token);
	}

	COM_MatchToken(buf_p, ")");
}

void Parse2DMatrix(char **buf_p, int y, int x, float *m)
{
	int i;

	COM_MatchToken(buf_p, "(");

	for (i = 0 ; i < y ; i++)
	{
		Parse1DMatrix(buf_p, x, m + i * x);
	}

	COM_MatchToken(buf_p, ")");
}

void Parse3DMatrix(char **buf_p, int z, int y, int x, float *m)
{
	int i;

	COM_MatchToken(buf_p, "(");

	for (i = 0 ; i < z ; i++)
	{
		Parse2DMatrix(buf_p, y, x, m + i * x * y);
	}

	COM_MatchToken(buf_p, ")");
}


/*
===============
Com_ParseInfos
===============
*/
int Com_ParseInfos(char *buf, int max, char infos[][MAX_INFO_STRING])
{
	const char *token;
	int        count;
	char       key[MAX_TOKEN_CHARS];

	count = 0;

	while (1)
	{
		token = COM_Parse(&buf);
		if (!token[0])
		{
			break;
		}
		if (strcmp(token, "{"))
		{
			Com_Printf("Missing { in info file\n");
			break;
		}

		if (count == max)
		{
			Com_Printf("Max infos exceeded\n");
			break;
		}

		infos[count][0] = 0;
		while (1)
		{
			token = COM_Parse(&buf);
			if (!token[0])
			{
				Com_Printf("Unexpected end of info file\n");
				break;
			}
			if (!strcmp(token, "}"))
			{
				break;
			}
			Q_strncpyz(key, token, sizeof(key));

			token = COM_ParseExt(&buf, qfalse);
			if (!token[0])
			{
				token = "<NULL>";
			}
			Info_SetValueForKey(infos[count], key, token);
		}
		count++;
	}

	return count;
}

/*
===================
Com_HexStrToInt
===================
*/
int Com_HexStrToInt(const char *str)
{
	if (!str || !str[0])
	{
		return -1;
	}

	// check for hex code
	if (str[0] == '0' && str[1] == 'x')
	{
		int i, n = 0;

		for (i = 2; i < strlen(str); i++)
		{
			char digit;

			n *= 16;

			digit = tolower(str[i]);

			if (digit >= '0' && digit <= '9')
			{
				digit -= '0';
			}
			else if (digit >= 'a' && digit <= 'f')
			{
				digit = digit - 'a' + 10;
			}
			else
			{
				return -1;
			}

			n += digit;
		}

		return n;
	}

	return -1;
}

/*
============================================================================

                    LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/

int Q_isprint(int c)
{
	if (c >= 0x20 && c <= 0x7E)
	{
		return (1);
	}
	return (0);
}

int Q_islower(int c)
{
	if (c >= 'a' && c <= 'z')
	{
		return (1);
	}
	return (0);
}

int Q_isupper(int c)
{
	if (c >= 'A' && c <= 'Z')
	{
		return (1);
	}
	return (0);
}

int Q_isalpha(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
	{
		return (1);
	}
	return (0);
}

int Q_isnumeric(int c)
{
	if (c >= '0' && c <= '9')
	{
		return (1);
	}
	return (0);
}

int Q_isalphanumeric(int c)
{
	if (Q_isalpha(c) ||
	    Q_isnumeric(c))
	{
		return(1);
	}
	return (0);
}

int Q_isforfilename(int c)
{
	if ((Q_isalphanumeric(c) || c == '_') && c != ' ')         // space not allowed in filename
	{
		return(1);
	}
	return (0);
}

char *Q_strrchr(const char *string, int c)
{
	char cc = c;
	char *s;
	char *sp = (char *)0;

	s = (char *)string;

	while (*s)
	{
		if (*s == cc)
		{
			sp = s;
		}
		s++;
	}
	if (cc == 0)
	{
		sp = s;
	}

	return sp;
}

/*
=============
Q_strncpyz

Safe strncpy that ensures a trailing zero
=============
*/
void Q_strncpyz(char *dest, const char *src, int destsize)
{
	if (!dest)
	{
		Com_Error(ERR_FATAL, "Q_strncpyz: NULL dest\n");
	}
	if (!src)
	{
		Com_Error(ERR_FATAL, "Q_strncpyz: NULL src\n");
	}
	if (destsize < 1)
	{
		Com_Error(ERR_FATAL, "Q_strncpyz: destsize < 1\n");
	}

	strncpy(dest, src, destsize - 1);
	dest[destsize - 1] = 0;
}

int Q_stricmpn(const char *s1, const char *s2, int n)
{
	int c1, c2;

	if (s1 == NULL)
	{
		if (s2 == NULL)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else if (s2 == NULL)
	{
		return 1;
	}

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if (!n--)
		{
			return 0;       // strings are equal until end point
		}

		if (c1 != c2)
		{
			if (c1 >= 'a' && c1 <= 'z')
			{
				c1 -= ('a' - 'A');
			}
			if (c2 >= 'a' && c2 <= 'z')
			{
				c2 -= ('a' - 'A');
			}
			if (c1 != c2)
			{
				return c1 < c2 ? -1 : 1;
			}
		}
	}
	while (c1);

	return 0;       // strings are equal
}

int Q_strncmp(const char *s1, const char *s2, int n)
{
	int c1, c2;

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if (!n--)
		{
			return 0;       // strings are equal until end point
		}

		if (c1 != c2)
		{
			return c1 < c2 ? -1 : 1;
		}
	}
	while (c1);

	return 0;       // strings are equal
}

int Q_stricmp(const char *s1, const char *s2)
{
	return (s1 && s2) ? Q_stricmpn(s1, s2, 99999) : -1;
}

char *Q_strlwr(char *s1)
{
	char *s;

	for (s = s1; *s; ++s)
	{
		if (('A' <= *s) && (*s <= 'Z'))
		{
			*s -= 'A' - 'a';
		}
	}

	return s1;
}

char *Q_strupr(char *s1)
{
	char *cp;

	for (cp = s1 ; *cp ; ++cp)
	{
		if (('a' <= *cp) && (*cp <= 'z'))
		{
			*cp += 'A' - 'a';
		}
	}

	return s1;
}


// never goes past bounds or leaves without a terminating 0
void Q_strcat(char *dest, int size, const char *src)
{
	int l1;

	l1 = strlen(dest);
	if (l1 >= size)
	{
		Com_Error(ERR_FATAL, "Q_strcat: already overflowed\n");
	}
	Q_strncpyz(dest + l1, src, size - l1);
}

/*
* Find the first occurrence of find in s.
*/
const char *Q_stristr(const char *s, const char *find)
{
	char   c, sc;
	size_t len;

	if ((c = *find++) != 0)
	{
		if (c >= 'a' && c <= 'z')
		{
			c -= ('a' - 'A');
		}
		len = strlen(find);
		do
		{
			do
			{
				if ((sc = *s++) == 0)
				{
					return NULL;
				}
				if (sc >= 'a' && sc <= 'z')
				{
					sc -= ('a' - 'A');
				}
			}
			while (sc != c);
		}
		while (Q_stricmpn(s, find, len) != 0);
		s--;
	}
	return s;
}


int Q_PrintStrlen(const char *string)
{
	int        len;
	const char *p;

	if (!string)
	{
		return 0;
	}

	len = 0;
	p   = string;
	while (*p)
	{
		if (Q_IsColorString(p))
		{
			p += 2;
			continue;
		}
		p++;
		len++;
	}

	return len;
}


char *Q_CleanStr(char *string)
{
	char *d;
	char *s;
	int  c;

	s = string;
	d = string;
	while ((c = *s) != 0)
	{
		if (Q_IsColorString(s))
		{
			s++;
		}
		else if (c >= 0x20 && c <= 0x7E)
		{
			*d++ = c;
		}
		s++;
	}
	*d = '\0';

	return string;
}

// strips whitespaces and bad characters
qboolean Q_isBadDirChar(char c)
{
	char badchars[] = { ';', '&', '(', ')', '|', '<', '>', '*', '?', '[', ']', '~', '+', '@', '!', '\\', '/', ' ', '\'', '\"', '\0' };
	int  i;

	for (i = 0; badchars[i] != '\0'; i++)
	{
		if (c == badchars[i])
		{
			return qtrue;
		}
	}

	return qfalse;
}

char *Q_CleanDirName(char *dirname)
{
	char *d;
	char *s;

	s = dirname;
	d = dirname;

	// clear trailing .'s
	while (*s == '.')
	{
		s++;
	}

	while (*s != '\0')
	{
		if (!Q_isBadDirChar(*s))
		{
			*d++ = *s;
		}
		s++;
	}
	*d = '\0';

	return dirname;
}

int Q_CountChar(const char *string, char tocount)
{
	int count;

	for (count = 0; *string; string++)
	{
		if (*string == tocount)
		{
			count++;
		}
	}

	return count;
}

int QDECL Com_sprintf(char *dest, int size, const char *fmt, ...)
{
	int     len;
	va_list argptr;

	va_start(argptr, fmt);
	len = Q_vsnprintf(dest, size, fmt, argptr);
	va_end(argptr);

	if (len >= size)
	{
		Com_Printf("Com_sprintf: Output length %d too short, require %d bytes.\n", size, len + 1);
	}

	return len;
}


/*
 * @brief Does a varargs printf into a temp buffer, so I don't need to have
 * varargs versions of all text functions.

char *QDECL va(char *format, ...)
{
    va_list     argptr;
    static char string[2][32000]; // in case va is called by nested functions
    static int  index = 0;
    char        *buf;

    buf = string[index & 1];
    index++;

    va_start(argptr, format);
    Q_vsnprintf(buf, sizeof(*string), format, argptr);
    va_end(argptr);

    return buf;
}
 */

#define MAX_VA_STRING   32000
/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday

Ridah, modified this into a circular list, to further prevent stepping on
previous strings
============
*/
char *QDECL va(const char *format, ...)
{
	va_list     argptr;
	static char temp_buffer[MAX_VA_STRING];
	static char string[MAX_VA_STRING];  // in case va is called by nested functions
	static int  index = 0;
	char        *buf;
	int         len;

	va_start(argptr, format);
	vsprintf(temp_buffer, format, argptr); // Q_vsnprintf ???
	va_end(argptr);

	if ((len = strlen(temp_buffer)) >= MAX_VA_STRING)
	{
		Com_Error(ERR_DROP, "Attempted to overrun string in call to va()\n");
	}

	if (len + index >= MAX_VA_STRING - 1)
	{
		index = 0;
	}

	buf = &string[index];
	memcpy(buf, temp_buffer, len + 1);

	index += len + 1;

	return buf;
}


/*
 * @brief Assumes buffer is atleast TRUNCATE_LENGTH big
 */
void Com_TruncateLongString(char *buffer, const char *s)
{
	int length = strlen(s);

	if (length <= TRUNCATE_LENGTH)
	{
		Q_strncpyz(buffer, s, TRUNCATE_LENGTH);
	}
	else
	{
		Q_strncpyz(buffer, s, (TRUNCATE_LENGTH / 2) - 3);
		Q_strcat(buffer, TRUNCATE_LENGTH, " ... ");
		Q_strcat(buffer, TRUNCATE_LENGTH, s + length - (TRUNCATE_LENGTH / 2) + 3);
	}
}

/*
=============
TempVector

(SA) this is straight out of g_utils.c around line 210

This is just a convenience function
for making temporary vectors for function calls
=============
*/
float *tv(float x, float y, float z)
{
	static int    index;
	static vec3_t vecs[8];
	float         *v;

	// use an array so that multiple tempvectors won't collide
	// for a while
	v     = vecs[index];
	index = (index + 1) & 7;

	v[0] = x;
	v[1] = y;
	v[2] = z;

	return v;
}

/*
=====================================================================

  INFO STRINGS

=====================================================================
*/

/*
===============
Info_ValueForKey

Searches the string for the given
key and returns the associated value, or an empty string.
FIXME: overflow check?
===============
*/
char *Info_ValueForKey(const char *s, const char *key)
{
	char        pkey[BIG_INFO_KEY];
	static char value[2][BIG_INFO_VALUE];   // use two buffers so compares
	// work without stomping on each other
	static int valueindex = 0;
	char       *o;

	if (!s || !key)
	{
		return "";
	}

	if (strlen(s) >= BIG_INFO_STRING)
	{
		Com_Error(ERR_DROP, "Info_ValueForKey: oversize infostring [%s] [%s]\n", s, key);
	}

	valueindex ^= 1;
	if (*s == '\\')
	{
		s++;
	}
	while (1)
	{
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
			{
				return "";
			}
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s)
		{
			*o++ = *s++;
		}
		*o = 0;

		if (!Q_stricmp(key, pkey))
		{
			return value[valueindex];
		}

		if (!*s)
		{
			break;
		}
		s++;
	}

	return "";
}


/*
===================
Info_NextPair

Used to itterate through all the key/value pairs in an info string
===================
*/
void Info_NextPair(const char **head, char *key, char *value)
{
	char       *o;
	const char *s;

	s = *head;

	if (*s == '\\')
	{
		s++;
	}
	key[0]   = 0;
	value[0] = 0;

	o = key;
	while (*s != '\\')
	{
		if (!*s)
		{
			*o    = 0;
			*head = s;
			return;
		}
		*o++ = *s++;
	}
	*o = 0;
	s++;

	o = value;
	while (*s != '\\' && *s)
	{
		*o++ = *s++;
	}
	*o = 0;

	*head = s;
}


/*
===================
Info_RemoveKey
===================
*/
void Info_RemoveKey(char *s, const char *key)
{
	char *start;
	char pkey[MAX_INFO_KEY];
	char value[MAX_INFO_VALUE];
	char *o;

	if (strlen(s) >= MAX_INFO_STRING)
	{
		Com_Error(ERR_DROP, "Info_RemoveKey: oversize infostring [%s] [%s]\n", s, key);
	}

	if (strchr(key, '\\'))
	{
		return;
	}

	while (1)
	{
		start = s;
		if (*s == '\\')
		{
			s++;
		}
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
			{
				return;
			}
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
			{
				return;
			}
			*o++ = *s++;
		}
		*o = 0;

		if (!Q_stricmp(key, pkey))
		{
			// rain - arguments to strcpy must not overlap
			//strcpy (start, s);    // remove this part
			memmove(start, s, strlen(s) + 1);     // remove this part
			return;
		}

		if (!*s)
		{
			return;
		}
	}

}

/*
===================
Info_RemoveKey_Big
===================
*/
void Info_RemoveKey_Big(char *s, const char *key)
{
	char *start;
	char pkey[BIG_INFO_KEY];
	char value[BIG_INFO_VALUE];
	char *o;

	if (strlen(s) >= BIG_INFO_STRING)
	{
		Com_Error(ERR_DROP, "Info_RemoveKey_Big: oversize infostring [%s] [%s]\n", s, key);
	}

	if (strchr(key, '\\'))
	{
		return;
	}

	while (1)
	{
		start = s;
		if (*s == '\\')
		{
			s++;
		}
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
			{
				return;
			}
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
			{
				return;
			}
			*o++ = *s++;
		}
		*o = 0;

		if (!Q_stricmp(key, pkey))
		{
			strcpy(start, s);    // remove this part
			return;
		}

		if (!*s)
		{
			return;
		}
	}

}




/*
==================
Info_Validate

Some characters are illegal in info strings because they
can mess up the server's parsing
==================
*/
qboolean Info_Validate(const char *s)
{
	if (strchr(s, '\"'))
	{
		return qfalse;
	}
	if (strchr(s, ';'))
	{
		return qfalse;
	}
	return qtrue;
}

/*
==================
Info_SetValueForKey

Changes or adds a key/value pair
==================
*/
void Info_SetValueForKey(char *s, const char *key, const char *value)
{
	char newi[MAX_INFO_STRING];

	if (strlen(s) >= MAX_INFO_STRING)
	{
		Com_Error(ERR_DROP, "Info_SetValueForKey: oversize infostring [%s] [%s] [%s]\n", s, key, value);
	}

	if (strchr(key, '\\') || strchr(value, '\\'))
	{
		Com_Printf("Can't use keys or values with a \\\n");
		return;
	}

	if (strchr(key, ';') || strchr(value, ';'))
	{
		Com_Printf("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strchr(key, '\"') || strchr(value, '\"'))
	{
		Com_Printf("Can't use keys or values with a \"\n");
		return;
	}

	Info_RemoveKey(s, key);
	if (!value || !strlen(value))
	{
		return;
	}

	Com_sprintf(newi, sizeof(newi), "\\%s\\%s", key, value);

	if (strlen(newi) + strlen(s) > MAX_INFO_STRING)
	{
		Com_Printf("Info string length exceeded\n");
		return;
	}

	strcat(s, newi);
}

/*
==================
Info_SetValueForKey_Big

Changes or adds a key/value pair
==================
*/
void Info_SetValueForKey_Big(char *s, const char *key, const char *value)
{
	char newi[BIG_INFO_STRING];

	if (strlen(s) >= BIG_INFO_STRING)
	{
		Com_Error(ERR_DROP, "Info_SetValueForKey_Big: oversize infostring [%s] [%s] [%s]\n", s, key, value);
	}

	if (strchr(key, '\\') || strchr(value, '\\'))
	{
		Com_Printf("Can't use keys or values with a \\\n");
		return;
	}

	if (strchr(key, ';') || strchr(value, ';'))
	{
		Com_Printf("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strchr(key, '\"') || strchr(value, '\"'))
	{
		Com_Printf("Can't use keys or values with a \"\n");
		return;
	}

	Info_RemoveKey_Big(s, key);
	if (!value || !strlen(value))
	{
		return;
	}

	Com_sprintf(newi, sizeof(newi), "\\%s\\%s", key, value);

	if (strlen(newi) + strlen(s) > BIG_INFO_STRING)
	{
		Com_Printf("BIG Info string length exceeded\n");
		return;
	}

	strcat(s, newi);
}




//====================================================================

/*
==================
Com_CharIsOneOfCharset
==================
*/
static qboolean Com_CharIsOneOfCharset(char c, char *set)
{
	int i;

	for (i = 0; i < strlen(set); i++)
	{
		if (set[i] == c)
		{
			return qtrue;
		}
	}

	return qfalse;
}

/*
==================
Com_SkipCharset
==================
*/
char *Com_SkipCharset(char *s, char *sep)
{
	char *p = s;

	while (p)
	{
		if (Com_CharIsOneOfCharset(*p, sep))
		{
			p++;
		}
		else
		{
			break;
		}
	}

	return p;
}

/*
==================
Com_SkipTokens
==================
*/
char *Com_SkipTokens(char *s, int numTokens, char *sep)
{
	int  sepCount = 0;
	char *p       = s;

	while (sepCount < numTokens)
	{
		if (Com_CharIsOneOfCharset(*p++, sep))
		{
			sepCount++;
			while (Com_CharIsOneOfCharset(*p, sep))
				p++;
		}
		else if (*p == '\0')
		{
			break;
		}
	}

	if (sepCount == numTokens)
	{
		return p;
	}
	else
	{
		return s;
	}
}

#if defined _MSC_VER
float rint(float v)
{
	if (v >= 0.5f)
	{
		return ceilf(v);
	}
	else
	{
		return floorf(v);
	}
}
#endif
