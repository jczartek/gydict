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

#include "gy-dict-manager.h"
#include "gy-dict.h"

struct _GyDictManager
{
  GObject             __parent__;

  GHashTable         *dicts;
  GSimpleActionGroup *hactions;
};

G_DEFINE_TYPE (GyDictManager, gy_dict_manager, G_TYPE_OBJECT)

static void
gy_dict_manager_go_back_in_history (GSimpleAction *action,
                                    GVariant      *parameter,
                                    gpointer       data)
{
}

static void
gy_dict_manager_go_next_in_history (GSimpleAction *action,
                                    GVariant      *parameter,
                                    gpointer       data)
{
}

static void
gy_dict_manager_finalize (GObject *object)
{
  GyDictManager *self = (GyDictManager *)object;

  g_hash_table_destroy (self->dicts);
  g_clear_object (&self->hactions);

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
  GSimpleAction *action = NULL;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
  static const GActionEntry entries[] = {
      {"go-back", gy_dict_manager_go_back_in_history, NULL, NULL, NULL},
      {"go-next", gy_dict_manager_go_next_in_history, NULL, NULL, NULL}
    };
#pragma GCC diagnostic pop

  self->dicts = g_hash_table_new_full (g_str_hash, g_str_equal,
                                       NULL, g_object_unref);
  self->hactions = g_simple_action_group_new ();
  g_action_map_add_action_entries (G_ACTION_MAP (self->hactions),
                                   entries, G_N_ELEMENTS (entries), self);

  action = (GSimpleAction *) g_action_map_lookup_action (G_ACTION_MAP (self->hactions), "go-back");
  g_simple_action_set_enabled (action, FALSE);
  action = (GSimpleAction *) g_action_map_lookup_action (G_ACTION_MAP (self->hactions), "go-next");
  g_simple_action_set_enabled (action, FALSE);
}

GyDictManager *
gy_dict_manager_new (void)
{
  return g_object_new (GY_TYPE_DICT_MANAGER, NULL);
}

GyDict *
gy_dict_manager_set_dict (GyDictManager *self,
                          const gchar   *name_dict)
{
  GyDict *dict;
  GError *err = NULL;
  GHashTableIter iter;
  gpointer obj;

  g_return_val_if_fail (name_dict != NULL, NULL);
  g_return_val_if_fail (GY_IS_DICT_MANAGER (self), NULL);

  g_hash_table_iter_init (&iter, self->dicts);
  while (g_hash_table_iter_next (&iter, NULL, &obj))
    {
      g_object_set (obj, "is-used", FALSE, NULL);
    }

  dict = g_hash_table_lookup (self->dicts, name_dict);
  if (dict)
    {
      g_object_set (dict, "is-used", TRUE, NULL);
      return dict;
    }

  dict = GY_DICT (gy_dict_new (name_dict));
  gy_dict_map (dict, &err);
  if (err != NULL)
    {
      g_critical ("Unable to create a new dictionary: %s", err->message);
      g_error_free (err);
      g_object_unref (dict);
      return NULL;
    }

  g_object_set (dict, "is-used", TRUE, NULL);
  g_hash_table_insert (self->dicts, (gpointer) name_dict, (gpointer) dict);

  return dict;
}

GyDict*
gy_dict_manager_get_used_dict (GyDictManager *self)
{
  gpointer dict = NULL;
  GHashTableIter iter;

  g_return_val_if_fail (GY_IS_DICT_MANAGER (self), NULL);

  g_hash_table_iter_init (&iter, self->dicts);
  while (g_hash_table_iter_next (&iter, NULL, &dict))
    {
      if (gy_dict_is_used (dict))
        {
          return dict;
        }
    }

  return dict;
}

GActionGroup *
gy_dict_manager_get_action_group (GyDictManager *self)
{
  g_return_val_if_fail (GY_IS_DICT_MANAGER (self), NULL);

  return G_ACTION_GROUP (self->hactions);
}
