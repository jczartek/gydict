/* gy-dict-manager.c
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

#include <libpeas/peas.h>
#include "gy-dict-manager.h"
#include "gy-dict.h"

struct _GyDictManager
{
  GObject     __parent__;
  GHashTable  *dicts;
};

G_DEFINE_TYPE (GyDictManager, gy_dict_manager, G_TYPE_OBJECT)

static void
gy_dict_manager_finalize (GObject *object)
{
  GyDictManager *self = (GyDictManager *)object;

  g_hash_table_destroy (self->dicts);

  G_OBJECT_CLASS (gy_dict_manager_parent_class)->finalize (object);
}

static void
gy_dict_manager_class_init (GyDictManagerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gy_dict_manager_finalize;
}

static void
gy_dict_manager_init (GyDictManager *self)
{
  self->dicts = g_hash_table_new_full (g_str_hash, g_str_equal,
                                       g_free, g_object_unref);
}

GyDictManager *
gy_dict_manager_new (void)
{
  return g_object_new (GY_TYPE_DICT_MANAGER, NULL);
}

void
gy_dict_manager_insert_dict (GyDictManager *self,
                             GyDict        *dict,
                             const gchar   *idx)
{
  g_return_if_fail (GY_IS_DICT_MANAGER (self));
  g_return_if_fail (GY_IS_DICT (dict));
  g_return_if_fail (idx != NULL);

  if (g_hash_table_lookup (self->dicts, idx))
    {
      g_warning ("The identifier [%s] already exists. The previous value will be lost.", idx);
    }

  g_hash_table_insert (self->dicts, (gpointer) g_strdup (idx), g_object_ref_sink (dict));
}

void
gy_dict_manager_remove_dict (GyDictManager *self,
                             const gchar   *idx)
{
  g_return_if_fail (GY_IS_DICT_MANAGER (self));
  g_return_if_fail (idx != NULL);

  g_hash_table_remove (self->dicts, idx);
}

/**
 * gy_dict_manager_lookup_dict:
 * @self: a #GyDictManager
 * @idx: the identifier to look up
 *
 * Returns: (transfer none) (nullable): A #GyDict, or %NULL if @idx is not found
 */
GyDict *
gy_dict_manager_lookup_dict (GyDictManager *self,
                             const gchar   *idx)
{

  g_return_val_if_fail (GY_IS_DICT_MANAGER (self), NULL);
  g_return_val_if_fail (idx != NULL, NULL);

  return (GyDict *) g_hash_table_lookup (self->dicts, idx);
}
