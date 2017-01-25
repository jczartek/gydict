/* test-dict-manager.c
 *
 * Copyright (C) 2017 Jakub Czartek <kuba@linux.pl>
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

#include <gtk/gtk.h>
#include <gydict.h>

#define ENGLISH_PWN "GyEnglishPwn-english"
#define POLISH_PWN  "GyEnglishPwn-polish"


gint
main (gint   argc,
      gchar *argv[])
{
  GyDict *dict[2];
  GyDictManager *manager;
  GtkTextBuffer  *buffer;

  gy_dict_initialize ();
  g_set_prgname ("test-dict-manager");
  gtk_init (NULL, NULL);

  buffer = GTK_TEXT_BUFFER (gy_text_buffer_new ());
  manager = gy_dict_manager_new ();

  dict[0] = gy_dict_manager_set_dict (manager, ENGLISH_PWN, buffer);

  g_return_val_if_fail (gy_dict_is_used (dict[0]), 1);
  g_return_val_if_fail (gy_dict_manager_get_used_dict (manager) == dict[0], 1);

  dict[1] = gy_dict_manager_set_dict (manager, POLISH_PWN, buffer);

  g_return_val_if_fail (gy_dict_is_used (dict[1]), 1);
  g_return_val_if_fail (gy_dict_manager_get_used_dict (manager) == dict[1], 1);
  g_return_val_if_fail (!gy_dict_is_used (dict[0]), 1);

  g_return_val_if_fail (dict[0] == gy_dict_manager_set_dict (manager, ENGLISH_PWN, buffer), 1);
  g_return_val_if_fail (gy_dict_is_used (dict[0]), 1);
  g_return_val_if_fail (gy_dict_manager_get_used_dict (manager) == dict[0], 1);
  g_return_val_if_fail (!gy_dict_is_used (dict[1]), 1);

  g_return_val_if_fail (dict[1] == gy_dict_manager_set_dict (manager, POLISH_PWN, buffer), 1);
  g_return_val_if_fail (gy_dict_is_used (dict[1]), 1);
  g_return_val_if_fail (gy_dict_manager_get_used_dict (manager) == dict[1], 1);
  g_return_val_if_fail (!gy_dict_is_used (dict[0]), 1);


  g_object_unref (manager);

  return 0;
}
