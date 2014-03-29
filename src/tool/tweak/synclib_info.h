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
 * \file synclib_info.h
 */

#ifndef syncLIB_INFO_H
#define syncLIB_INFO_H

#include <analyzed_cap_file.h>

typedef struct {
    u1 class_token;

    constant_pool_entry_info* syncAID_ref;

    u1 SyncApplet_method_token;

    u1 syncRegister_1_method_token;
    u1 syncRegister_2_method_token;
    u1 uninstall_method_token;

    u1 getId_method_token;
    u1 setId_method_token;
    u1 isDuplicated_method_token;
    u1 getPreviousSynchronizableInstance_method_token;
    u1 setPreviousSynchronizableInstance_method_token;
    u1 resetPath_method_token;
    u1 numberPath_method_token;
    u1 serialize_method_token;
    u1 merge_method_token;

    u1 resetStaticPathAll_method_token;
    u1 numberStaticPathAll_method_token;
    u1 serializeStaticAll_method_token;
    u1 mergeStaticAll_method_token;

    u1 getClassId_method_token;
    u1 getNewInstance_method_token;

    u1 getNbStaticBooleanArrays_method_token;
    u1 getAnyStaticBooleanArray_method_token;

    u1 getNbStaticByteArrays_method_token;
    u1 getAnyStaticByteArray_method_token;

    u1 getNbStaticShortArrays_method_token;
    u1 getAnyStaticShortArray_method_token;

    u1 getNbStaticIntArrays_method_token;
    u1 getAnyStaticIntArray_method_token;

    constant_pool_entry_info* resetPathAll_ref;
    constant_pool_entry_info* numberPathAll_ref;
    constant_pool_entry_info* serializeAll_ref;
    constant_pool_entry_info* mergeAll_ref;

    constant_pool_entry_info* syncUninstall_ref;

    type_descriptor_info* resetStaticPathAll_sig;
    type_descriptor_info* numberStaticPathAll_sig;
    type_descriptor_info* serializeStaticAll_sig;
    type_descriptor_info* mergeStaticAll_sig;

    type_descriptor_info* getClassId_sig;
    type_descriptor_info* getNewInstance_sig;

    type_descriptor_info* getNbStaticBooleanArrays_sig;
    type_descriptor_info* getAnyStaticBooleanArray_sig;

    type_descriptor_info* getNbStaticByteArrays_sig;
    type_descriptor_info* getAnyStaticByteArray_sig;

    type_descriptor_info* getNbStaticShortArrays_sig;
    type_descriptor_info* getAnyStaticShortArray_sig;

    type_descriptor_info* getNbStaticIntArrays_sig;
    type_descriptor_info* getAnyStaticIntArray_sig;
} SyncApplet_info;


typedef struct {
    u1 class_token;
    constant_pool_entry_info* class_ref;

    constant_pool_entry_info* newlyCreatedInstance_ref;

    constant_pool_entry_info* resetResume_ref;

    constant_pool_entry_info* read_boolean_ref;
    constant_pool_entry_info* read_byte_ref;
    constant_pool_entry_info* read_short_ref;
    constant_pool_entry_info* read_int_ref;

    constant_pool_entry_info* check_boolean_array_ref;
    constant_pool_entry_info* read_boolean_array_ref;

    constant_pool_entry_info* check_byte_array_ref;
    constant_pool_entry_info* read_byte_array_ref;

    constant_pool_entry_info* check_short_array_ref;
    constant_pool_entry_info* read_short_array_ref;

    constant_pool_entry_info* check_int_array_ref;
    constant_pool_entry_info* read_int_array_ref;

    constant_pool_entry_info* readN_ref;
    constant_pool_entry_info* getReadNValue_ref;

    u4 skip_instance_value;
    u4 new_instance_value;
} Input_info;

typedef struct {
    u1 class_token;
    constant_pool_entry_info* class_ref;

    constant_pool_entry_info* resetResume_ref;
    constant_pool_entry_info* getNextId_ref;
    constant_pool_entry_info* skipInstance_ref;

    constant_pool_entry_info* write_boolean_ref;
    constant_pool_entry_info* write_byte_ref;
    constant_pool_entry_info* write_short_ref;
    constant_pool_entry_info* write_int_ref;
    constant_pool_entry_info* write_boolean_array_ref;
    constant_pool_entry_info* write_byte_array_ref;
    constant_pool_entry_info* write_short_array_ref;
    constant_pool_entry_info* write_int_array_ref;
} Output_info;

typedef struct {
    u1 interface_token;
    constant_pool_entry_info* interface_ref;

    u1 getId_method_token;
    u1 setId_method_token;
    u1 isDuplicated_method_token;

    u1 getPreviousSynchronizableInstance_method_token;
    u1 setPreviousSynchronizableInstance_method_token;

    u1 resetPath_method_token;
    u1 numberPath_method_token;
    u1 serialize_method_token;
    u1 merge_method_token;

    type_descriptor_info* getId_sig;
    type_descriptor_info* setId_sig;
    type_descriptor_info* isDuplicated_sig;

    type_descriptor_info* getPreviousSynchronizableInstance_sig;
    type_descriptor_info* setPreviousSynchronizableInstance_sig;

    type_descriptor_info* resetPath_sig;
    type_descriptor_info* numberPath_sig;
    type_descriptor_info* serialize_sig;
    type_descriptor_info* merge_sig;

    type_descriptor_info* init_method_sig;
} Synchronizable_info;

typedef struct {
    u1 interface_token;
    constant_pool_entry_info* interface_ref;

    // Inherited methods
    u1 getId_method_token;
    u1 setId_method_token;
    u1 isDuplicated_method_token;

    u1 getPreviousSynchronizableInstance_method_token;
    u1 setPreviousSynchronizableInstance_method_token;

    u1 resetPath_method_token;
    u1 numberPath_method_token;
    u1 serialize_method_token;
    u1 merge_method_token;


    u1 getNext_method_token;
    u1 setNext_method_token;

    u1 getNbBooleanArrays_method_token;
    u1 getBooleanArray_method_token;

    u1 getNbByteArrays_method_token;
    u1 getByteArray_method_token;

    u1 getNbShortArrays_method_token;
    u1 getShortArray_method_token;

    u1 getNbIntArrays_method_token;
    u1 getIntArray_method_token;

    type_descriptor_info* getNext_sig;
    type_descriptor_info* setNext_sig;

    type_descriptor_info* getNbBooleanArrays_sig;
    type_descriptor_info* getBooleanArray_sig;

    type_descriptor_info* getNbByteArrays_sig;
    type_descriptor_info* getByteArray_sig;

    type_descriptor_info* getNbShortArrays_sig;
    type_descriptor_info* getShortArray_sig;

    type_descriptor_info* getNbIntArrays_sig;
    type_descriptor_info* getIntArray_sig;
} SynchronizableArrayFields_info;

typedef struct {
    u1 interface_token;
    constant_pool_entry_info* interface_ref;

    // Inherited methods
    u1 getId_method_token;
    u1 setId_method_token;
    u1 isDuplicated_method_token;

    u1 getPreviousSynchronizableInstance_method_token;
    u1 setPreviousSynchronizableInstance_method_token;

    u1 resetPath_method_token;
    u1 numberPath_method_token;
    u1 serialize_method_token;
    u1 merge_method_token;


    u1 getCurrentSynchronizableInstance_method_token;
    u1 setCurrentSynchronizableInstance_method_token;
    u1 hasCurrentSynchronizableInstance_method_token;
    u1 selectNextSynchronizableInstance_method_token;
    u1 resetNextSynchronizableInstanceSelect_method_token;

    type_descriptor_info* getCurrentSynchronizableInstance_sig;
    type_descriptor_info* setCurrentSynchronizableInstance_sig;
    type_descriptor_info* hasCurrentSynchronizableInstance_sig;
    type_descriptor_info* selectNextSynchronizableInstance_sig;
    type_descriptor_info* resetNextSynchronizableInstanceSelect_sig;
} SynchronizableInstanceFields_info;

typedef struct {
    u1 interface_token;
    constant_pool_entry_info* interface_ref;
    u1 getResetId_method_token;
} ResetIdSource_info;

typedef struct {
    imported_package_info* imported_package;

    SyncApplet_info SyncApplet;
    Input_info Input;
    Output_info Output;
    Synchronizable_info Synchronizable;
    SynchronizableArrayFields_info SynchronizableArrayFields;
    SynchronizableInstanceFields_info SynchronizableInstanceFields;
    ResetIdSource_info ResetIdSource;
} synclib_info;

#endif
