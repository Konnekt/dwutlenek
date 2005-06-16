/* 
   DwuTlenek

   License information can be found in license.txt.
   Please redistribute only as a whole package.
   
   (c)2004 Rafa≥ Lindemann | Stamina
   http://www.konnekt.info
*/ 

#include "stdafx.h"
/*
 * cp_to_iso()
 *
 * zamienia krzaczki pisane w cp1250 na iso-8859-2, przy okazji maskuj-c
 * znaki, kt°rych nie da si‡ wy¬wietliä, za wyj-tkiem \r i \n.
 *
 *  - buf.
 */
char * _cp2iso(char *buf)
{
	if (!buf)
		return buf;
	char * start = buf;
	while (*buf) {
		if (*buf == 'π') *buf = '±';
		if (*buf == '•') *buf = '°';
		if (*buf == 'ú') *buf = '∂';
		if (*buf == 'å') *buf = '¶';
		if (*buf == 'ü') *buf = 'º';
		if (*buf == 'è') *buf = '¨';
//                if (*buf != 13 && *buf != 10 && (*buf < 32 || (*buf > 127 && *buf < 160)))
//                        *buf = '?';
		buf++;
	}
	return start;
}

/*
 * iso_to_cp()
 *
 * zamienia sensowny format kodowania polskich znaczk°w na bezsensowny.
 *
 *  - buf.
 */
char * _iso2cp(char *buf)
{
	if (!buf)
		return buf;
	char * start = buf;

	while (*buf) {
		if (*buf == '±') *buf = 'π';
		if (*buf == '°') *buf = '•';
		if (*buf == '∂') *buf = 'ú';
		if (*buf == '¶') *buf = 'å';
		if (*buf == 'º') *buf = 'ü';
		if (*buf == '¨') *buf = 'è';
		buf++;
	}
	return start;
}

CStdString iso2cp(const char * buf) {
	CStdString s (buf);
	return _iso2cp((char*)(s.c_str()));
}
CStdString cp2iso(const char * buf) {
	CStdString s (buf);
	return _iso2cp((char*)(s.c_str()));
}
