/*
 *	Copyright (C) 2009-2010 Vincent Hanquez <vincent@snarc.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 only. with the special
 * exception on linking described in file LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Udev binding
 */

#include <libudev.h>

#define CAML_NAME_SPACE

#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/fail.h>
#include <caml/signals.h>
#include <caml/callback.h>

#define Val_none 		(Val_int(0))
#define caml_alloc_variant_param(val, tag, p) \
	do { val = caml_alloc_small(1, tag); Field(val, 0) = (p); } while (0)
#define caml_alloc_some(val, param) 	caml_alloc_variant_param(val, 0, param)

#define caml_append_list(list, tmp, e)                  \
	do {                                            \
		tmp = caml_alloc_small(2, Tag_cons);    \
		Field(tmp, 0) = (e);                    \
		Field(tmp, 1) = list;                   \
		list = tmp;                             \
	} while (0)

#define UDev_val(v) 		(*((struct udev **) Data_custom_val(v)))
#define UDevDevice_val(v)	(*((struct udev_device **) Data_custom_val(v)))
#define UDevMonitor_val(v)	(*((struct udev_monitor **) Data_custom_val(v)))

#define SIZEOF_FINALPTR         (2 * sizeof (void *))

#define voidstar_alloc(o_void, c_void, final_fct)				 \
	do {									 \
		o_void = caml_alloc_final(SIZEOF_FINALPTR, final_fct,		 \
		                          SIZEOF_FINALPTR, 10 * SIZEOF_FINALPTR);\
		*((unsigned long *) Data_custom_val(o_void)) = (unsigned long) c_void; \
	} while (0)

value caml_copy_string_maybe(const char *s)
{
	value opt;
	if (!s)
		return Val_none;
	caml_alloc_some(opt, caml_copy_string(s));
	return opt;
}

void finalize_udev(value v)
{
	udev_unref(UDev_val(v));
}

void finalize_udev_device(value v)
{
	udev_device_unref(UDevDevice_val(v));
}

void finalize_udev_monitor(value v)
{
	udev_monitor_unref(UDevMonitor_val(v));
}

/***********************************************************************/
value stub_udev_new(value unit)
{
	CAMLparam1(unit);
	CAMLlocal1(u);
	struct udev *udev;

	udev = udev_new();
	voidstar_alloc(u, udev, finalize_udev);
	CAMLreturn(u);
}

value stub_udev_get_sys_path(value udev)
{
	CAMLparam1(udev);
	CAMLlocal1(path);
	const char *r;
	
	r = udev_get_sys_path(UDev_val(udev));
	path = caml_copy_string(r);
	CAMLreturn(path);
}

value stub_udev_get_dev_path(value udev)
{
	CAMLparam1(udev);
	CAMLlocal1(path);
	const char *r;
	
	r = udev_get_dev_path(UDev_val(udev));
	path = caml_copy_string(r);
	CAMLreturn(path);
}

value stub_udev_get_log_priority(value udev)
{
	CAMLparam1(udev);
	int priority;
	
	priority = udev_get_log_priority(UDev_val(udev));
	CAMLreturn(Val_int(priority));
}

value stub_udev_set_log_priority(value udev, value priority)
{
	CAMLparam2(udev, priority);
	udev_set_log_priority(UDev_val(udev), Int_val(priority));
	CAMLreturn(Val_unit);
}

/***********************************************************************/

value stub_udev_device_new_from_syspath(value udev, value syspath)
{
	CAMLparam2(udev, syspath);
	CAMLlocal1(dev);
	struct udev_device *d;

	d = udev_device_new_from_syspath(UDev_val(udev), String_val(syspath));
	voidstar_alloc(dev, d, finalize_udev_device);
	CAMLreturn(dev);
}

value stub_udev_device_new_from_devnum(value udev, value type, value devnum)
{
	CAMLparam3(udev, type, devnum);
	CAMLlocal1(dev);
	struct udev_device *d;
	dev_t c_devnum;

	c_devnum = 0; /* FIXME */
	d = udev_device_new_from_devnum(UDev_val(udev), Int_val(type), c_devnum);
	voidstar_alloc(dev, d, finalize_udev_device);
	CAMLreturn(dev);
}

value stub_udev_device_get_parent(value dev)
{
	CAMLparam1(dev);
	CAMLlocal1(devparent);
	struct udev_device *d;

	d = udev_device_get_parent(UDevDevice_val(dev));
	voidstar_alloc(devparent, d, finalize_udev_device);
	CAMLreturn(devparent);
}

value stub_udev_device_get(value dev)
{
	CAMLparam1(dev);
	CAMLlocal2(info, x);
	const char *s;

	info = caml_alloc_tuple(9);

	s = udev_device_get_devpath(UDevDevice_val(dev));
	x = caml_copy_string_maybe(s);
	Store_field(info, 0, x);

	s = udev_device_get_subsystem(UDevDevice_val(dev));
	x = caml_copy_string_maybe(s);
	Store_field(info, 1, x);

	s = udev_device_get_devtype(UDevDevice_val(dev));
	x = caml_copy_string_maybe(s);
	Store_field(info, 2, x);
	
	s = udev_device_get_syspath(UDevDevice_val(dev));
	x = caml_copy_string_maybe(s);
	Store_field(info, 3, x);

	s = udev_device_get_sysname(UDevDevice_val(dev));
	x = caml_copy_string_maybe(s);
	Store_field(info, 4, x);

	s = udev_device_get_sysnum(UDevDevice_val(dev));
	x = caml_copy_string_maybe(s);
	Store_field(info, 5, x);

	s = udev_device_get_devnode(UDevDevice_val(dev));
	x = caml_copy_string_maybe(s);
	Store_field(info, 6, x);

	s = udev_device_get_driver(UDevDevice_val(dev));
	x = caml_copy_string_maybe(s);
	Store_field(info, 7, x);

	s = udev_device_get_action(UDevDevice_val(dev));
	x = caml_copy_string_maybe(s);
	Store_field(info, 8, x);

	CAMLreturn(info);
}

value stub_udev_device_get_properties(value dev)
{
	CAMLparam1(dev);
	CAMLlocal4(list, tmp, kv, x);
	struct udev_list_entry *ents, *iter;
	const char *s;

	ents = udev_device_get_properties_list_entry(UDevDevice_val(dev));

	list = tmp = Val_emptylist;
	udev_list_entry_foreach(iter, ents) {
		kv = caml_alloc_tuple(2);

		s = udev_list_entry_get_name(iter);
		x = caml_copy_string(s);
		Store_field(kv, 0, x);

		s = udev_list_entry_get_value(iter);
		x = caml_copy_string(s);
		Store_field(kv, 1, x);

		caml_append_list(list, tmp, kv);
	}
	CAMLreturn(list);
}

value stub_udev_device_get_devlinks(value dev)
{
	CAMLparam1(dev);
	CAMLlocal4(list, tmp, kv, x);
	struct udev_list_entry *ents, *iter;
	const char *s;

	ents = udev_device_get_devlinks_list_entry(UDevDevice_val(dev));

	list = tmp = Val_emptylist;
	udev_list_entry_foreach(iter, ents) {
		kv = caml_alloc_tuple(2);

		s = udev_list_entry_get_name(iter);
		x = caml_copy_string(s);
		Store_field(kv, 0, x);

		s = udev_list_entry_get_value(iter);
		x = caml_copy_string(s);
		Store_field(kv, 1, x);

		caml_append_list(list, tmp, kv);
	}
	CAMLreturn(list);
}

/***********************************************************************/

value stub_udev_monitor_new_from_netlink(value udev, value name)
{
	CAMLparam2(udev, name);
	CAMLlocal1(udevmonitor);
	struct udev_monitor *um;

	um = udev_monitor_new_from_netlink(UDev_val(udev), String_val(name));
	voidstar_alloc(udevmonitor, um, finalize_udev_monitor);
	CAMLreturn(udevmonitor);
}

value stub_udev_monitor_new_from_socket(value udev, value socket_path)
{
	CAMLparam2(udev, socket_path);
	CAMLlocal1(udevmonitor);
	struct udev_monitor *um;

	um = udev_monitor_new_from_socket(UDev_val(udev), String_val(socket_path));
	voidstar_alloc(udevmonitor, um, finalize_udev_monitor);
	CAMLreturn(udevmonitor);
}

value stub_udev_monitor_get_fd(value udevmonitor)
{
	CAMLparam1(udevmonitor);
	int fd;

	fd = udev_monitor_get_fd(UDevMonitor_val(udevmonitor));
	CAMLreturn(Val_int(fd));
}

value stub_udev_monitor_receive_device(value udevmonitor)
{
	CAMLparam1(udevmonitor);
	CAMLlocal1(dev);
	struct udev_device *d;

	d = udev_monitor_receive_device(UDevMonitor_val(udevmonitor));
	voidstar_alloc(dev, d, finalize_udev_device);
	CAMLreturn(dev);
}

value stub_udev_monitor_enable_receiving(value udevmonitor)
{
	CAMLparam1(udevmonitor);
	int r;

	r = udev_monitor_enable_receiving(UDevMonitor_val(udevmonitor));
	if (r != 0)
		caml_failwith("udev_monitor enable receiving");
	CAMLreturn(Val_unit);
}

value stub_udev_monitor_filter(value udevmonitor, value subsystem, value devtype)
{
	CAMLparam3(udevmonitor, subsystem, devtype);
	int r;

	r = udev_monitor_filter_add_match_subsystem_devtype(UDevMonitor_val(udevmonitor),
	                                                    String_val(subsystem), 
	                                                    String_val(devtype));
	if (r != 0)
		caml_failwith("udev_monitor_filer_add_match");
	CAMLreturn(Val_unit);
}

value stub_udev_monitor_filter_update(value udevmonitor)
{
	CAMLparam1(udevmonitor);
	udev_monitor_filter_update(UDevMonitor_val(udevmonitor));
	CAMLreturn(Val_unit);
}

value stub_udev_monitor_filter_remove(value udevmonitor)
{
	CAMLparam1(udevmonitor);
	udev_monitor_filter_remove(UDevMonitor_val(udevmonitor));
	CAMLreturn(Val_unit);
}
