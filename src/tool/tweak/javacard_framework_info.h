/*
 * Copyright Inria:
 * Jean-François Hren
 * 
 * jfhren[at]gmail[dot]com
 * michael[dot]hauspie[at]lifl[dot]com
 * 
 * This software is a computer program whose purpose is to read, analyze,
 * modify, generate and write Java Card 2 CAP file.
 * 
 * This software is governed by the CeCILL-B license under French
 * law and
 * abiding by the rules of distribution of free software.  You can  use, 
 * modify and/ or redistribute the software under the terms of the
 * CeCILL-B
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its
 * terms.
 */

/**
 * \file javacard_framework_info.h
 */

#ifndef JAVACARD_FRAMEWORK_INFO_H
#define JAVACARD_FRAMEWORK_INFO_H

#include <analyzed_cap_file.h>

extern const u1 javacard_framework_package_aid[];
extern const u1 javacard_framework_package_aid_length;

typedef struct {
    u1 class_token;

    u1 Applet_method_token;
    u1 register_1_method_token;   /* No arg */
    u1 register_2_method_token;   /* With args */
    u1 process_method_token;
    u1 select_method_token;
    u1 getShareableInterfaceObject_method_token;
} Applet_info;

typedef struct {
    u1 class_token;
    constant_pool_entry_info* class_ref;

    u1 equals_method_token;
    constant_pool_entry_info* equals_ref;
} AID_info;

typedef struct {
    u1 interface_token;
} Shareable_info;

typedef struct {
    u1 class_token;

    u1 throwIt_method_token;
    constant_pool_entry_info* throwIt_ref;
} ISOException_info;

typedef struct {
    u2 sw_unknown_value;
} ISO7816_info;

typedef struct {
    u1 class_token;
    constant_pool_entry_info* class_ref;

    u1 getReason_method_token;
    constant_pool_entry_info* getReason_ref;
} UserException_info;

typedef struct {
    u1 interface_token;
    constant_pool_entry_info* interface_ref;

    u1 uninstall_method_token;
} AppletEvent_info;

typedef struct {
    u1 package_token;
    imported_package_info* imported_package;

    Applet_info Applet;
    AID_info AID;
    Shareable_info Shareable;
    ISOException_info ISOException;
    ISO7816_info ISO7816;
    UserException_info UserException;
    AppletEvent_info AppletEvent;
} javacard_framework_info;


#endif
