(*
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
 *)

type udev
type udev_monitor
type udev_device

type devnum = int

type device_info = {
	dev_path: string option;
	dev_subsystem: string option;
	dev_type: string option;
	dev_syspath: string option;
	dev_sysname: string option;
	dev_sysnum: string option;
	dev_node: string option;
	dev_driver: string option;
	dev_action: string option;
}

external make : unit -> udev = "stub_udev_new"
external get_sys_path : udev -> string = "stub_udev_get_sys_path"
external get_dev_path : udev -> string = "stub_udev_get_dev_path"
external get_log_priority : udev -> int = "stub_udev_get_log_priority"
external set_log_priority : udev -> int -> unit = "stub_udev_set_log_priority"

external device_new_from_syspath : udev -> string -> udev_device = "stub_udev_device_new_from_syspath"
external device_new_from_devnum : udev -> string -> devnum -> udev_device = "stub_udev_device_new_from_devnum"
external device_get_parent : udev_device -> udev_device = "stub_udev_get_parent"
external device_get : udev_device -> device_info = "stub_udev_device_get"
external device_get_properties : udev_device -> (string * string) list = "stub_udev_device_get_properties"
external device_get_devlinks : udev_device -> (string * string) list = "stub_udev_device_get_devlinks"

external monitor_new_from_netlink : udev -> string -> udev_monitor = "stub_udev_monitor_new_from_netlink"
external monitor_new_from_socket : udev -> string -> udev_monitor = "stub_udev_monitor_new_from_socket"
external monitor_get_fd : udev_monitor -> Unix.file_descr = "stub_udev_monitor_get_fd"
external monitor_receive_device : udev_monitor -> udev_device = "stub_udev_monitor_receive_device"
external monitor_enable_receiving : udev_monitor -> unit = "stub_udev_monitor_enable_receiving"
external monitor_filter : udev_monitor -> string -> string -> unit = "stub_udev_monitor_filter"
external monitor_filter_update : udev_monitor -> unit = "stub_udev_monitor_filter_update"
external monitor_filter_remove : udev_monitor -> unit = "stub_udev_monitor_filter_remove"
