/* gy-pwntabs.c
 *
 * Copyright (C) 2014 Jakub Czartek <kuba@linux.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "gy-pwntabs.h"

const gchar *pwn_encje[] = 
{
  "&IPA502", "&inodot", "&##952", "&##8747", "&eng", "&square", "&squareb", "&pause", "&##163", "&dots",
  "&rArr", "&IPA118", "&##949", "&IPA413", "&IPA424", "&IPA505", "&IPA135", "&IPA305", "&IPA306", "&IPA313",
  "&IPAa313", "&IPA314", "&IPA321", "&IPA326", "&IPA503", "&IPA146", "&IPA170", "&IPA128", "&IPA325", "&IPA301",
  "&IPA155", "&IPA319", "&IPA114", "&IPA134", "&IPA103", "&IPA140", "&IPA119", "&IPA131", "&IPA130", "&schwa.x",
  "&epsi", "&ldquor", "&marker", "&IPA101", "&IPA102", "&IPA104", "&IPA109", "&IPA110", "&IPA116", "&IPA122",
  "&IPA129", "&IPA132", "&IPA133", "&IPA153", "&IPA182", "&IPA183", "&IPA302", "&IPA304", "&IPA307", "&IPA308",
  "&IPA309", "&IPA322", "&IPA426", "&IPA491", "&IPA501", "&comma", "&squ", "&ncaron", "&atildedotbl.x", "&rdquor",
  "&verbar", "&IPA405", "&idot", "&equals", "&lsqb", "&rsqb", "&s224", "&karop", "&s225", "&s241",
  "&#!0,127", "&##37", "&##9553", "&oboczn", "&##1100", "&##1098", "&s172", "&ytilde", "&estrok", "&ndotbl",
  "&yogh", "&ismutne", "&usmutne", "&middot_s", "&##133", "&semi", "&zacute", "&dollar", "&frac13", "&frac15",
  "&ldotbl.x", "&mdotbl.x", "&commat", "&Lstrok", "&Aogon", "&ybreve.x", "&IPA177", "&IPA324", "&IPA432", "&IPA432i",
  "&IPA303", "&IPA138", "&IPA320", "&IPA214", "&epsilontilde", "&auluk", "&ailuk","&apos", "&brvbar", "&reg",
  "&rsquo", "&lsquo","&ccedil","&eacute", "&egrave", "&amp", "&ecirc", "&agrave", "&iuml", "&ocirc",
  "&icirc", "&para", "&mdash", "&rdquo", "&ap", "&rarr", "&pound", "&aelig", "&auml", "&dash",
  "&uuml","&ouml", "&szlig", "&Auml", "&Ouml", "&Uuml", NULL
};

const gchar *pwn_encje_zam[] =
{
  "ˌ", "ɪ", "θ", "ʃ", "ŋ", "…", "•", "―", "£", "…",
  "→", "ɲ", "ε", " ̟", " ̃", " ̆", "ʒ", "ɑ", "ɔ", "ɒ",
  "ɒ", "ʌ", "ʊ", "ɜ", "ː", "h", "w", "f", "æ", "i",
  "l", "ɪ", "m", "ʃ", "t", "x", "ŋ", "ð", "θ", "ə",
  "ε", "“", "►", "p", "b", "d", "k", "g", "n", "r",
  "v", "s", "z", "j", "ɕ", "ʑ", "e", "a", "o", "u",
  "y", "ə", "ˡ", "ǫ", "ˈ", ",", "•", "ň", "ã", "”",
  "|", " ̤", "i", "=", "[", "]", "◊", "◊", "<", ">",
  "▫", "%", "║", "║", "ь", "ъ", "←", "ỹ", "ě", "ṇ",
  "ȝ", "i", "u", "⋅", "…", ";", "ź", "$", "⅓", "⅕",
  "ḷ", "ṃ", "@", "Ł", "Ą", "Ў", "ǀ", "ɐ", " ̯", "i̯",
  "ɛ", "ç", "ʏ", "ʤ", "ε", "au", "ai", "'","|", "®",
  "’", "‘", "ç", "é", "è", "&", "ê", "à", "ï", "ô", 
  "î", "▹", "―", "”", "≈", "↪", "£", "æ", "ä", "-",
  "ü", "ö", "ß", "Ä", "Ö", "Ü", "?", NULL
};

static const gchar *pwndict_cp1250_to_utf8_table[] =
{
  "€", "?", "‚", "?", "„", "…", "†", "‡",
  "?", "‰", "Š", "‹", "Ś", "Ť", "Ž", "Ź",
  "?", "‘", "’", "“", "”", "•", "–", "—",
  "?", "™", "š", "›", "ś", "ť", "ž", "ź",
  " ", "ˇ", "˘", "Ł", "¤", "Ą", "¦", "§",
  "¨", "©", "Ş", "«", "¬", "­", "®", "Ż",
  "°", "±", "˛", "ł", "´", "µ", "¶", "·",
  "¸", "ą", "ş", "»", "Ľ", "˝", "ľ", "ż",
  "Ŕ", "Á", "Â", "Ă", "Ä", "Ĺ", "Ć", "Ç",
  "Č", "É", "Ę", "Ë", "Ě", "Í", "Î", "Ď",
  "Đ", "Ń", "Ň", "Ó", "Ô", "Ő", "Ö", "×",
  "Ř", "Ů", "Ú", "Ű", "Ü", "Ý", "Ţ", "ß",
  "à", "á", "â", "ă", "ä", "ĺ", "ć", "ç",
  "č", "é", "ę", "ë", "ě", "í", "î", "ï",
  "đ", "ń", "ň", "ó", "ô", "ő", "ö", "÷",
  "ř", "ů", "ú", "ű", "ü", "ý", "ţ", "˙",
};

static const gchar *pwndict_iso88592_to_utf8_table[] =
{
  "?", "?", "?", "?", "?", "?", "?", "?",
  "?", "?", "?", "?", "?", "?", "?", "?",
  "?", "?", "?", "?", "?", "?", "?", "?",
  "?", "?", "?", "?", "?", "?", "?", "?",
  " ", "Ą", "˘", "Ł", "¤", "Ľ", "Ś", "§",
  "¨", "Š", "Ş", "Ť", "Ź", "", "Ž", "Ż",
  "°", "ą", "˛", "ł", "´", "ľ", "ś", "ˇ",
  "¸", "š", "ş", "ť", "ź", "˝", "ž", "ż",
  "Ŕ", "Á", "Â", "Ă", "Ä", "Ĺ", "Ć", "Ç",
  "Č", "É", "Ę", "Ë", "Ě", "Í", "Î", "Ď",
  "Đ", "Ń", "Ň", "Ó", "Ô", "Ő", "Ö", "×",
  "Ř", "Ů", "Ú", "Ű", "Ü", "Ý", "Ţ", "ß",
  "à", "á", "â", "ă", "ä", "ĺ", "ć", "ç",
  "č", "é", "ę", "ë", "ě", "í", "î", "ï",
  "đ", "ń", "ň", "ó", "ô", "ő", "ö", "÷",
  "ř", "ů", "ú", "ű", "ü", "ý", "ţ", "˙",
};

const gchar *pwndict_superscript_to_utf8_table[] =
{
  "⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"
};

static const gchar *superscript[] =
{
  "⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"
};


const gchar *(*array_of_pointer_to_arrays_of_character_set[2])[] = {&pwndict_cp1250_to_utf8_table,
  &pwndict_iso88592_to_utf8_table};

void gy_tabs_convert_character (gchar **buffer,
                                const gchar *c,
                                GyDictEncoding encoding)
{
  const gchar *str;
  gchar *b;

  g_return_if_fail (buffer != NULL);
  g_return_if_fail (encoding == GY_ENCODING_CP1250 ||
                    encoding == GY_ENCODING_ISO88592);

  b = *buffer;

  if (encoding == GY_ENCODING_CP1250)
    str = pwndict_cp1250_to_utf8_table[((guchar) *c) - 128];
  else
    str = pwndict_iso88592_to_utf8_table[((guchar) *c) - 128];

  while (*str)
    *b++ = *str++;
  
  *buffer = b;
}


GHashTable*
gy_tabs_get_entity_table (void)
{
  GHashTable *tab = NULL;

  tab = g_hash_table_new (g_str_hash,
                          g_str_equal);

  for (int i=0; pwn_encje[i] != NULL && pwn_encje_zam[i] != NULL; i++)
       g_hash_table_insert (tab, 
                            (gpointer) pwn_encje[i],
                            (gpointer) pwn_encje_zam[i]);

  return tab;

}

const gchar *
gy_tabs_get_superscript (guint n)
{
  g_return_val_if_fail ((n >= 0 && n <= 9), NULL);

  return superscript[n];
}
