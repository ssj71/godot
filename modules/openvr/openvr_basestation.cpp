/*************************************************************************/
/*  openvr_basestation.cpp                                               */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2016 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "openvr_basestation.h"
 
#include <stdio.h>
#include <string.h>

void OpenVRBaseStation::_bind_methods() {
	ObjectTypeDB::bind_method( _MD("get_basestation_index"),&OpenVRBaseStation::get_basestation_index );
	ObjectTypeDB::bind_method( _MD("set_basestation_index","basestation_index"),&OpenVRBaseStation::set_basestation_index );

	ADD_PROPERTYNZ( PropertyInfo( Variant::INT, "basestation_index"), _SCS("set_basestation_index"),_SCS("get_basestation_index"));
//	ADD_PROPERTY( PropertyInfo( Variant::STRING, "device_name"), _SCS("set_device_name"), _SCS("get_device_name"));

	ADD_SIGNAL( MethodInfo("activated",PropertyInfo(Variant::OBJECT,"controller")));
	ADD_SIGNAL( MethodInfo("deactivated",PropertyInfo(Variant::OBJECT,"controller")));
};

uint32_t OpenVRBaseStation::get_basestation_index() const {
	return basestation_index + 1; // we store from 0 but show from index 1
};

void OpenVRBaseStation::set_basestation_index(uint32_t p_index) {
	if ((p_index > 0) && (p_index <= vr::k_unMaxTrackedDeviceCount)) {
		basestation_index = p_index - 1;
	};
};

void OpenVRBaseStation::process(float pDelta) {
	// @TODO: we should find a way to update the transforms of all our trackers so we don't trickle this data in during processing..
	if (ovr != NULL) {
		// update our tracked device data...
		set_transform(ovr->getUpdatedTransform(ovr->getBaseStationIndex(basestation_index)));

		// check if we're initialized...
		bool currently_initialized = true;
		if (device_name[0] == '\0') {
			currently_initialized = false;
		} else if (strcmp(device_name, "Not initialized") == 0) {
			currently_initialized = false;
		};

		// Check of our device has changed...
		char new_device_name[256];
		strcpy(new_device_name, ovr->getTrackedDeviceName(ovr->getBaseStationIndex(basestation_index), 255));
		if (strcmp(device_name, new_device_name) != 0) {
			strcpy(device_name, new_device_name);
			update_gizmo();

			if (!currently_initialized) {
				emit_signal("activated", this);
			} else if (strcmp(device_name, "Not initialized") == 0) {
				emit_signal("deactivated", this);
			};
		};

		return;
	};
};

void OpenVRBaseStation::_notification(int p_what) {
	switch (p_what ) {
		case NOTIFICATION_ENTER_TREE: {
			if (ovr == NULL) {
				ovr = openVR_interface::get_singleton(get_tree()->is_editor_hint());
			}

			set_process(true);
		}; break;
		case NOTIFICATION_EXIT_TREE: {
			set_process(false);

			if (ovr != NULL) {
				openVR_interface::release();
				ovr = NULL;
			};
		}; break;
		case NOTIFICATION_PROCESS: {
			process(get_process_delta_time());
		}; break;
		default : {
			// don't do anything here
		}; break;
	};
};

OpenVRBaseStation::OpenVRBaseStation() {
	ovr = NULL;
	basestation_index = 0;
	device_name[0] = '\0';
};

OpenVRBaseStation::~OpenVRBaseStation() {
	if (ovr != NULL) {
		openVR_interface::release();
		ovr = NULL;
	};
};
