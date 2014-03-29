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
#ifndef TWEAK_INFO_H
#define TWEAK_INFO_H
#include <analyzed_cap_file.h>

#define FIELD_SHOULD_NOT_SYNCHRONIZE 0x80

/**
 * \brief Information about an analyzed class and tweaking done on it.
 */
typedef struct {
    char is_SyncApplet_immediate_superclass;     /**< Is syncApplet immediate
                                                     superclass?*/
    char has_SyncApplet_in_class_hierarchy;      /**< Is syncApplet in class
                                                     hierarchy? Should be true
                                                     if SyncApplet is immediate
                                                     superclass. */

    char is_superclass_implementing_Synchronizable_interface;   /**< Is
        superclass implementing the Synchronizable interface? */
    implemented_method_info* Synchronizable_methods;    /**< Is NULL if the
        superclass is external or if it does not implement the interface. */

    char is_superclass_implementing_SynchronizableArrayFields_interface; /**< Is
        superclass implementing the SynchronizableArrayFields interface? */
    implemented_method_info* SynchronizableArrayFields_methods; /**< Is NULL if
        the superclass is external or if it does not implement the interface. */

    char is_superclass_implementing_SynchronizableInstanceFields_interface; /**<
        Is superclass implementing the SynchronizableInstanceFields interface? */
    implemented_method_info* SynchronizableInstanceFields_methods; /**< Is NULL
        if the superclass is external or if it does not implement the interface. */

    u2 nb_methods_before_tweak; /**< For going through original methods only (if
                                     new methods were not inserted though). */

    u2 nb_fields;       /**< Number of primitive type instance fields. Do not
                             include inherited ones. */
    u2 nb_array_fields; /**< Number of primitive type array type instance
                             fields. Do not include inherited ones. */
    int16_t nb_boolean_array_fields;    /**< Number of instance boolean array
                                             type fields. Do not include
                                             inherited ones. */
    int16_t nb_boolean_array_superclass_fields; /**< Do not include array from
                                                     external superclasses (it
                                                     should though). */
    int16_t nb_byte_array_fields;   /**< Number of instance byte array type
                                         fields. Do not include inherited ones. */
    int16_t nb_byte_array_superclass_fields;    /**< Do not include array from
                                                     external superclasses (it
                                                     should though). */
    int16_t nb_short_array_fields;  /**< Number of instance short array type
                                         fields. Do not include inherited ones. */
    int16_t nb_short_array_superclass_fields;   /**< Do not include array from
                                                     external superclasses (it
                                                     should though). */
    int16_t nb_int_array_fields;    /**< Number of instance int array type
                                         field. Do not include inherited ones. */
    int16_t nb_int_array_superclass_fields; /**< Do not include array from
                                                 external superclasses (it
                                                 should though). */
    int16_t nb_ref_fields;  /**< Number of class reference type instance fields. */

    u2 nb_static_fields;    /**< Number of primitive type static fields. Do not
                                 include inherited ones. */
    u2 nb_static_array_fields;  /**< Number of primitive type array type static
                                     fields. Do not include inherited ones. */
    int16_t nb_static_boolean_array_fields; /**< Number of static boolean array
                                                 fields. Do not include
                                                 inherited ones. */
    int16_t nb_static_byte_array_fields;    /**< Number of static byte array
                                                 fields. Do not include
                                                 inherited ones. */
    int16_t nb_static_short_array_fields;   /**< Number of static short array
                                                 fields. Do not include
                                                 inherited ones. */
    int16_t nb_static_int_array_fields; /**< Number of static int array fields.
                                             Do not include inherited ones. */
    int16_t nb_static_ref_fields;   /**< Number of class reference type static
                                         fields. Do not include inherited ones. */

    // Added fields
    constant_pool_entry_info* id_field; /**< NULL if not added. */
    constant_pool_entry_info* isDuplicated_field;   /**< NULL if not added. */
    constant_pool_entry_info* currentClass_field;   /**< NULL if not added. */
    constant_pool_entry_info* currentStaticField_field; /**< NULL if not added. */
    constant_pool_entry_info* currentField_field;   /**< NULL if not added. */
    constant_pool_entry_info* previous_field;   /**< NULL if not added. */
    constant_pool_entry_info* next_field;   /**< NULL if not added. */

    // Synchronizable interface methods
    method_info* getId_method;  /**< NULL if not added. Set by \link
        synchronizable_itf_functions.c add_methods \endlink. */
    method_info* inherited_getId_method;    /**< NULL if external or not
        inherited. Set by \link synchronizable_itf_functions.c add_methods
        \endlink. */
    u1 inherited_getId_method_token;    /**< Not set if not inherited. Set by
        \link synchronizable_itf_functions.c add_methods \endlink. */

    method_info* setId_method;  /**< NULL if not added. Set by \link
synchronizable_itf_functions.c add_methods \endlink. */
    method_info* inherited_setId_method;    /**< NULL if external or not
                                                 inherited. Set by \link
synchronizable_itf_functions.c add_methods \endlink. */
    u1 inherited_setId_method_token;    /**< Not set if not inherited. Set by
        \link synchronizable_itf_functions.c add_methods \endlink. */

    method_info* isDuplicated_method;   /**< NULL if not added. Set by \link
synchronizable_itf_functions.c add_methods \endlink. */
    method_info* inherited_isDuplicated_method; /**< NULL if external or not
                                                     inherited. Set by \link
synchronizable_itf_functions.c add_methods \endlink. */
    u1 inherited_isDuplicated_method_token; /**< Not set if not inherited. Set
        by \link synchronizable_itf_functions.c add_methods \endlink. */

    method_info* getPreviousSynchronizableInstance_method;  /**< NULL if not
        added. Set by \link synchronizable_itf_functions.c add_methods \endlink. */
    method_info* inherited_getPreviousSynchronizableInstance_method; /**< NULL
        if external or not inherited. Set by \link synchronizable_itf_functions.c
        add_methods \endlink. */
    u1 inherited_getPreviousSynchronizableInstance_method_token;    /**< Not set
        if not inherited. Set by \link synchronizable_itf_functions.c add_methods
        \endlink. */

    method_info* setPreviousSynchronizableInstance_method;  /**< NULL if not
        added. Set by \link synchronizable_itf_functions.c add_methods \endlink. */
    method_info* inherited_setPreviousSynchronizableInstance_method; /**< NULL
        if external or not inherited. Set by \link synchronizable_itf_functions.c
        add_methods \endlink. */
    u1 inherited_setPreviousSynchronizableInstance_method_token;    /**< Not set
        if not inherited. Set by \link synchronizable_itf_functions.c
        add_methods \endlink. */

    method_info* resetPath_method;  /**< Set by \link
        synchronizable_itf_functions.c add_methods \endlink. */
    method_info* inherited_resetPath_method;    /**< Set by \link
        synchronizable_itf_functions.c add_methods \endlink. */
    u1 inherited_resetPath_method_token;    /**< Set by \link
        synchronizable_itf_functions.c add_methods \endlink. */

    method_info* numberPath_method; /**< Set by \link
        synchronizable_itf_functions.c add_methods \endlink. */
    method_info* inherited_numberPath_method;   /**< Set by \link
        synchronizable_itf_functions.c add_methods \endlink. */
    u1 inherited_numberPath_method_token;   /**< Set by \link
        synchronizable_itf_functions.c add_methods \endlink. */

    method_info* serialize_method;  /**< Set by \link
        synchronizable_itf_functions.c add_methods \endlink. */
    method_info* inherited_serialize_method;    /**< Set by \link
        synchronizable_itf_functions.c add_methods \endlink. */
    u1 inherited_serialize_method_token;    /**< Set by \link
        synchronizable_itf_functions.c add_methods \endlink. */

    method_info* merge_method;  /**< Set by \link synchronizable_itf_functions.c
        add_methods \endlink. */
    method_info* inherited_merge_method;    /**< Set by \link
        synchronizable_itf_functions.c add_methods \endlink. */
    u1 inherited_merge_method_token;    /**< Set by \link
        synchronizable_itf_functions.c add_methods \endlink. */

    method_info* init_method;   /**< Should be added to every tweaked class. set
        by synchronizable_itf_functions.c add_methods \endlink. */

    // SynchronizableArrayFields interface methods
    method_info* getNext_method;    /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_getNext_method;  /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    u1 inherited_getNext_method_token;  /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */

    method_info* setNext_method;    /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_setNext_method;  /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    u1 inherited_setNext_method_token;  /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */

    method_info* getNbBooleanArrays_method; /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_getNbBooleanArrays_method;   /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    u1 inherited_getNbBooleanArrays_method_token;   /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */

    method_info* getBooleanArray_method;    /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_getBooleanArray_method;  /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    u1 inherited_getBooleanArray_method_token;  /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */

    method_info* getNbByteArrays_method;    /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_getNbByteArrays_method;  /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    u1 inherited_getNbByteArrays_method_token;  /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */

    method_info* getByteArray_method;   /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_getByteArray_method; /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    u1 inherited_getByteArray_method_token; /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */

    method_info* getNbShortArrays_method;   /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_getNbShortArrays_method; /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    u1 inherited_getNbShortArrays_method_token; /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */

    method_info* getShortArray_method;  /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_getShortArray_method;    /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    u1 inherited_getShortArray_method_token;    /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */

    method_info* getNbIntArrays_method; /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_getNbIntArrays_method;   /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    u1 inherited_getNbIntArrays_method_token;   /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */

    method_info* getIntArray_method;    /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_getIntArray_method;  /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */
    u1 inherited_getIntArray_method_token;  /**< Set by \link
        synchronizable_array_fields_itf_functions.h add_methods \endlink. */

    // SynchronizableInstanceFields interface methods
    method_info* hasCurrentSynchronizableInstance_method;   /**< Set by \link
        synchronizable_instance_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_hasCurrentSynchronizableInstance_method; /**< Set by
        \link synchronizable_instance_fields_itf_functions.h add_methods
        \endlink. */
    u1 inherited_hasCurrentSynchronizableInstance_method_token; /**< Set by
        \link synchronizable_instance_fields_itf_functions.h add_methods
        \endlink. */

    method_info* selectNextSynchronizableInstance_method;   /**< Set by \link
        synchronizable_instance_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_selectNextSynchronizableInstance_method; /**< Set by
        \link synchronizable_instance_fields_itf_functions.h add_methods
        \endlink. */
    u1 inherited_selectNextSynchronizableInstance_method_token; /**< Set by
        \link synchronizable_instance_fields_itf_functions.h add_methods
        \endlink. */

    method_info* getCurrentSynchronizableInstance_method;   /**< Set by \link
        synchronizable_instance_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_getCurrentSynchronizableInstance_method; /**< Set by
        \link synchronizable_instance_fields_itf_functions.h add_methods
        \endlink. */
    u1 inherited_getCurrentSynchronizableInstance_method_token; /**< Set by
        \link synchronizable_instance_fields_itf_functions.h add_methods
        \endlink. */

    method_info* setCurrentSynchronizableInstance_method;   /**< Set by \link
        synchronizable_instance_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_setCurrentSynchronizableInstance_method; /**< Set by
        \link synchronizable_instance_fields_itf_functions.h add_methods
        \endlink. */
    u1 inherited_setCurrentSynchronizableInstance_method_token; /**< Set by \link
        synchronizable_instance_fields_itf_functions.h add_methods \endlink. */

    method_info* resetNextSynchronizableInstanceSelect_method;  /**< Set by \link
        synchronizable_instance_fields_itf_functions.h add_methods \endlink. */
    method_info* inherited_resetNextSynchronizableInstanceSelect_method;    /**<
        Set by \link synchronizable_instance_fields_itf_functions.h add_methods
        \endlink. */
    u1 inherited_resetNextSynchronizableInstanceSelect_method_token;    /**<
        Set by \link synchronizable_instance_fields_itf_functions.h add_methods
        \endlink. */

    // Protected methods of SyncApplet
    method_info* getClassId_method; /**< Set by \link
        add_inherited_protected_methods() \endlink. */
    method_info* getNewInstance_method; /**< Set by \link
        add_inherited_protected_methods() \endlink. */

    method_info* getNbStaticBooleanArrays_method;   /**< Set by \link
        add_inherited_protected_methods() \endlink. */
    method_info* getAnyStaticBooleanArray_method;   /**< Set by \link
        add_inherited_protected_methods() \endlink. */
    method_info* getNbStaticByteArrays_method;  /**< Set by \link
        add_inherited_protected_methods() \endlink. */
    method_info* getAnyStaticByteArray_method;  /**< Set by \link
        add_inherited_protected_methods() \endlink. */
    method_info* getNbStaticShortArrays_method; /**< Set by \link
        add_inherited_protected_methods() \endlink. */
    method_info* getAnyStaticShortArray_method; /**< Set by \link
        add_inherited_protected_methods() \endlink. */
    method_info* getNbStaticIntArrays_method;   /**< Set by \link
        add_inherited_protected_methods() \endlink. */
    method_info* getAnyStaticIntArray_method;   /**< Set by \link
        add_inherited_protected_methods() \endlink. */

    method_info* resetStaticPathAll_method; /**< Set by \link
        add_inherited_protected_methods() \endlink. */
    method_info* numberStaticPathAll_method;    /**< Set by \link
        add_inherited_protected_methods() \endlink. */
    method_info* serializeStaticAll_method; /**< Set by \link
        add_inherited_protected_methods() \endlink. */
    method_info* mergeStaticAll_method; /**< Set by \link
        add_inherited_protected_methods() \endlink. */

    // Static methods for processing static fields
    method_info* getStaticBooleanArray_method;  /**< Set by \link
        add_getStaticBooleanArray_method() \endlink. */
    method_info* getStaticByteArray_method; /**< Set by \link
        add_getStaticByteArray_method() \endlink. */
    method_info* getStaticShortArray_method;    /**< Set by \link
        add_getStaticShortArray_method() \endlink. */
    method_info* getStaticIntArray_method;  /**< Set by \link
        add_getStaticIntArray_method() \endlink. */

    method_info* resetStaticPath_method;    /**< Set by \link
        add_resetStaticPath_method() \endlink. */
    method_info* numberStaticPath_method;   /**< Set by \link
        add_numberStaticPath_method() \endlink. */
    method_info* serializeStatic_method;    /**< Set by \link
        add_serializeStatic_method() \endlink. */
    method_info* mergeStatic_method;    /**< Set by \link
        add_mergeStatic_method() \endlink. */

    // supercall ref for invokespecial bytecode
    constant_pool_entry_info* resetPath_supercall_ref;  /**< Set by \link
        synchronizable_itf_functions.c add_supercalls() \endlink. */
    constant_pool_entry_info* serialize_supercall_ref;  /**< Set by \link
        synchronizable_itf_functions.c add_supercalls() \endlink. */
    constant_pool_entry_info* merge_supercall_ref;  /**< Set by \link
        synchronizable_itf_functions.c add_supercalls() \endlink. */

    constant_pool_entry_info* getBooleanArray_supercall_ref;    /**< Set by
        \link synchronizable_array_fields_itf_functions.c add_supercalls()
        \endlink. */
    constant_pool_entry_info* getByteArray_supercall_ref;   /**< Set by \link
        synchronizable_array_fields_itf_functions.c add_supercalls() \endlink. */
    constant_pool_entry_info* getShortArray_supercall_ref;  /**< Set by \link
        synchronizable_array_fields_itf_functions.c add_supercalls() \endlink. */
    constant_pool_entry_info* getIntArray_supercall_ref;    /**< Set by \link
        synchronizable_array_fields_itf_functions.c add_supercalls() \endlink. */

    constant_pool_entry_info* hasCurrentSynchronizableInstance_supercall_ref;/**<
        Set by \link synchronizable_instance_fields_itf_functions.c
        add_supercalls() \endlink. */
    constant_pool_entry_info* selectNextSynchronizableInstance_supercall_ref;/**<
        Set by \link synchronizable_instance_fields_itf_functions.c
        add_supercalls() \endlink. */
    constant_pool_entry_info* getCurrentSynchronizableInstance_supercall_ref;/**<
        Set by \link synchronizable_instance_fields_itf_functions.c
        add_supercalls() \endlink. */
    constant_pool_entry_info* setCurrentSynchronizableInstance_supercall_ref;/**<
        Set by \link synchronizable_instance_fields_itf_functions.c
        add_supercalls() \endlink. */
    constant_pool_entry_info* resetNextSynchronizableInstanceSelect_supercall_ref;/**<
        Set by \link synchronizable_instance_fields_itf_functions.c
        add_supercalls() \endlink. */
} tweak_info;

#endif
