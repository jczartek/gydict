/* gy-german-pwn.c
 *
 * Copyright (C) 2015 Jakub Czartek <kuba@linux.pl>
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

#include "gy-german-pwn.h"

struct _GyGermanPwn
{
  GyDict parent;

  GFile *file_dict;
};

G_DEFINE_TYPE (GyGermanPwn, gy_german_pwn, G_TYPE_OBJECT)

enum {
  PROP_0,
  LAST_PROP
};

static GParamSpec *gParamSpecs [LAST_PROP];

static void
gy_german_pwn_initialize (GyDict  *self,
                          GError **err)
{

}

static guint
gy_german_pwn_set_dictionary (GyDict *self)
{
  return GY_OK;
}

static guint
gy_german_pwn_init_list (GyDict *self)
{
  return GY_OK;
}

static gpointer
gy_german_pwn_read_definition (GyDict *self,
                               guint   index)
{
  return NULL;
}

static void
gy_german_pwn_finalize (GObject *object)
{
  GyGermanPwn *self = (GyGermanPwn *)object;

  G_OBJECT_CLASS (gy_german_pwn_parent_class)->finalize (object);
}

static void
gy_german_pwn_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GyGermanPwn *self = GY_GERMAN_PWN (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_german_pwn_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GyGermanPwn *self = GY_GERMAN_PWN (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gy_german_pwn_class_init (GyGermanPwnClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GyDictClass  *dict_class = GY_DICT_CLASS (klass);

  object_class->finalize = gy_german_pwn_finalize;
  object_class->get_property = gy_german_pwn_get_property;
  object_class->set_property = gy_german_pwn_set_property;

  dict_class->set_dictionary = gy_german_pwn_set_dictionary;
  dict_class->init_list = gy_german_pwn_init_list;
  dict_class->read_definition = gy_german_pwn_read_definition;
  dict_class->initialize = gy_german_pwn_initialize;
}

static void
gy_german_pwn_init (GyGermanPwn *self)
{
}
