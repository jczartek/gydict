/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * gy-pwn.h
 * Copyright (C) 2014 kuba <kuba@fedora>
 *
 * gy-pwn.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gy-pwn.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GY_PWN_H_
#define _GY_PWN_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GY_TYPE_PWN             (gy_pwn_get_type ())
#define GY_PWN(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GY_TYPE_PWN, GyPwn))
#define GY_PWN_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GY_TYPE_PWN, GyPwnClass))
#define GY_IS_PWN(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GY_TYPE_PWN))
#define GY_IS_PWN_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GY_TYPE_PWN))
#define GY_PWN_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GY_TYPE_PWN, GyPwnClass))


typedef struct _GyPwn GyPwn;
typedef struct _GyPwnClass GyPwnClass;

struct _GyPwn
{
    GyDict parent_instance;
};

struct _GyPwnClass
{
    GyDictClass parent_class;

    /* Virtual Method */
    guint (*set_dictionary) (GyDict *dict);
    guint (*init_list) (GyDict *dict);
    gpointer (*read_definition) (GyDict *dict, guint index);
};

GType gy_pwn_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* _GY_PWN_H_ */

