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
 * \file synchronizable_array_fields_itf_functions.c
 * \brief Add the SynchronizableArrayFields interface and its methods
 * implementation to a class.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <analyzed_cap_file.h>

#include "tweak_info.h"
#include "javacard_framework_info.h"
#include "synclib_info.h"
#include "generic_tweak_functions.h"
#include "add_field_functions.h"
#include "create_bytecode_functions.h"

static type_descriptor_info* SynchronizableArrayFields_type = NULL;

static int add_methods(analyzed_cap_file* acf, class_info* class, synclib_info* synclib, char is_using_int);
static int add_next_field(analyzed_cap_file* acf, class_info* class, constant_pool_entry_info* SynchronizableArrayFields_ref);
static int add_setNext_bytecodes(method_info* method, class_info* class);
static int add_getNext_bytecodes(method_info* method, class_info* class);
static int add_getNbTArrays_bytecodes(method_info* method, int16_t nb_T_array_superclass_fields, int16_t nb_T_array_fields);
static int add_getTArray_bytecodes(method_info* method, class_info* class, javacard_framework_info* javacard_framework, constant_pool_entry_info* supercall_ref, int16_t nb_T_array_superclass_fields, int16_t nb_T_tarray_fields, u1 array_type);



/**
 * \brief Add the SynchronizableArrayFields interface to the given class.
 *
 * Should be called only if there is at least one instance array field or if the
 * superclass is implementing the SynchronizableArrayFields interface.
 *
 * If the given class superclass does not implements the
 * SynchronizableArrayFields interface then the SynchronizableArrayFields next
 * field is added.
 *
 * Only the methods defined by the SynchronizableArrayFields interface and not
 * inherited by its superinterfaces are taken into account.
 *
 * The methods setNext and getNext are added only if the given class
 * superclasses do not implement the SynchronizableArrayFields interface
 * otherwise they are only inherited.
 *
 * The methods getNbTArrays and getTArray are added only if the given class
 * superclasses do not implement the SynchronizableArrayFields or if there is at
 * least one instance T array field in the given class otherwise they are only
 * inherited.
 *
 * Bytecodes are generated for each added method.
 *
 * \param acf The analyzed CAP file.
 * \param class The class to which the SynchonizableArrayFields interface is
 * added.
 * \param synclib Information on the synclib package.
 * \param javacard_framework Information on the javacard.framework package.
 * \param is_using_int Is the int type used?
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
int add_SynchronizableArrayFields_interface(analyzed_cap_file* acf, class_info* class, synclib_info* synclib, javacard_framework_info* javacard_framework, char is_using_int) {

    implemented_interface_info* tmp = NULL;
    tweak_info* tweak = (tweak_info*)class->tweak;

    // add SynchronizableArrayFields methods to the class
    if(add_methods(acf, class, synclib, is_using_int) == -1)
        return -1;


    // add the interface to the class
    if((tmp = (implemented_interface_info*)realloc(class->interfaces, sizeof(implemented_interface_info) * (class->interfaces_count + 1))) == NULL) {
        perror("add_SynchronizableArrayFields_interface");
        return -1;
    }

    class->interfaces = tmp;

    class->interfaces[class->interfaces_count].ref = synclib->SynchronizableArrayFields.interface_ref;

    class->interfaces[class->interfaces_count].count = is_using_int ? 19 : 17;
    class->interfaces[class->interfaces_count].index = (implemented_method_info*)malloc(sizeof(implemented_method_info) * class->interfaces[class->interfaces_count].count);
    if(class->interfaces[class->interfaces_count].index == NULL) {
        perror("add_SynchronizableArrayFields_interface");
        return -1;
    }
    memset(class->interfaces[class->interfaces_count].index, 0, sizeof(implemented_method_info) * class->interfaces[class->interfaces_count].count);


    // Link the implementation
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getId_method_token].implementation = tweak->getId_method ? tweak->getId_method : tweak->inherited_getId_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getId_method_token].method_token = tweak->inherited_getId_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.setId_method_token].implementation = tweak->setId_method ? tweak->setId_method : tweak->inherited_setId_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.setId_method_token].method_token = tweak->inherited_setId_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.isDuplicated_method_token].implementation = tweak->isDuplicated_method ? tweak->isDuplicated_method  : tweak->inherited_isDuplicated_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.isDuplicated_method_token].method_token = tweak->inherited_isDuplicated_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getPreviousSynchronizableInstance_method_token].implementation = tweak->getPreviousSynchronizableInstance_method ? tweak->getPreviousSynchronizableInstance_method : tweak->inherited_getPreviousSynchronizableInstance_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getPreviousSynchronizableInstance_method_token].method_token = tweak->inherited_getPreviousSynchronizableInstance_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.setPreviousSynchronizableInstance_method_token].implementation = tweak->setPreviousSynchronizableInstance_method ? tweak->setPreviousSynchronizableInstance_method : tweak->inherited_setPreviousSynchronizableInstance_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.setPreviousSynchronizableInstance_method_token].method_token = tweak->inherited_setPreviousSynchronizableInstance_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.resetPath_method_token].implementation = tweak->resetPath_method ? tweak->resetPath_method : tweak->inherited_resetPath_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.resetPath_method_token].method_token = tweak->inherited_resetPath_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.numberPath_method_token].implementation = tweak->numberPath_method ? tweak->numberPath_method : tweak->inherited_numberPath_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.numberPath_method_token].method_token = tweak->inherited_numberPath_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.serialize_method_token].implementation = tweak->serialize_method ? tweak->serialize_method : tweak->inherited_serialize_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.serialize_method_token].method_token = tweak->inherited_serialize_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.merge_method_token].implementation = tweak->merge_method ? tweak->merge_method : tweak->inherited_merge_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.merge_method_token].method_token = tweak->inherited_merge_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getNext_method_token].implementation = tweak->getNext_method ? tweak->getNext_method : tweak->inherited_getNext_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getNext_method_token].method_token = tweak->inherited_getNext_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.setNext_method_token].implementation = tweak->setNext_method ? tweak->setNext_method : tweak->inherited_setNext_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.setNext_method_token].method_token = tweak->inherited_setNext_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getNbBooleanArrays_method_token].implementation = tweak->getNbBooleanArrays_method ? tweak->getNbBooleanArrays_method : tweak->inherited_getNbBooleanArrays_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getNbBooleanArrays_method_token].method_token = tweak->inherited_getNbBooleanArrays_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getBooleanArray_method_token].implementation = tweak->getBooleanArray_method ? tweak->getBooleanArray_method : tweak->inherited_getBooleanArray_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getBooleanArray_method_token].method_token = tweak->inherited_getBooleanArray_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getNbByteArrays_method_token].implementation = tweak->getNbByteArrays_method ? tweak->getNbByteArrays_method : tweak->inherited_getNbByteArrays_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getNbByteArrays_method_token].method_token = tweak->inherited_getNbByteArrays_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getByteArray_method_token].implementation = tweak->getByteArray_method ? tweak->getByteArray_method : tweak->inherited_getByteArray_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getByteArray_method_token].method_token = tweak->inherited_getByteArray_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getNbShortArrays_method_token].implementation = tweak->getNbShortArrays_method ? tweak->getNbShortArrays_method : tweak->inherited_getNbShortArrays_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getNbShortArrays_method_token].method_token = tweak->inherited_getNbShortArrays_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getShortArray_method_token].implementation = tweak->getShortArray_method ? tweak->getShortArray_method : tweak->inherited_getShortArray_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getShortArray_method_token].method_token = tweak->inherited_getShortArray_method_token;

    if(is_using_int) {
        class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getNbIntArrays_method_token].implementation = tweak->getNbIntArrays_method ? tweak->getNbIntArrays_method : tweak->inherited_getNbIntArrays_method;
        class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getNbIntArrays_method_token].method_token = tweak->inherited_getNbIntArrays_method_token;

        class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getIntArray_method_token].implementation = tweak->getIntArray_method ? tweak->getIntArray_method : tweak->inherited_getIntArray_method;
        class->interfaces[class->interfaces_count].index[synclib->SynchronizableArrayFields.getIntArray_method_token].method_token = tweak->inherited_getIntArray_method_token;
    }

    ++class->interfaces_count;


    // Add supporting field
    if(!tweak->is_superclass_implementing_SynchronizableArrayFields_interface)
        if(add_next_field(acf, class, synclib->SynchronizableArrayFields.interface_ref) == -1)
            return -1;


    // Generate the bytecodes
    if(add_setNext_bytecodes(tweak->setNext_method, class) == -1)
        return -1;

    if(add_getNext_bytecodes(tweak->getNext_method, class) == -1)
        return -1;

    if(add_getNbTArrays_bytecodes(tweak->getNbBooleanArrays_method, tweak->nb_boolean_array_superclass_fields, tweak->nb_boolean_array_fields) == -1)
        return -1;

    if(add_getTArray_bytecodes(tweak->getBooleanArray_method, class, javacard_framework, tweak->getBooleanArray_supercall_ref, tweak->nb_boolean_array_superclass_fields, tweak->nb_boolean_array_fields, TYPE_DESCRIPTOR_BOOLEAN) == -1)
        return -1;

    if(add_getNbTArrays_bytecodes(tweak->getNbByteArrays_method, tweak->nb_byte_array_superclass_fields, tweak->nb_byte_array_fields) == -1)
        return -1;

    if(add_getTArray_bytecodes(tweak->getByteArray_method, class, javacard_framework, tweak->getByteArray_supercall_ref, tweak->nb_byte_array_superclass_fields, tweak->nb_byte_array_fields, TYPE_DESCRIPTOR_BYTE) == -1)
        return -1;

    if(add_getNbTArrays_bytecodes(tweak->getNbShortArrays_method, tweak->nb_short_array_superclass_fields, tweak->nb_short_array_fields) == -1)
        return -1;

    if(add_getTArray_bytecodes(tweak->getShortArray_method, class, javacard_framework, tweak->getShortArray_supercall_ref, tweak->nb_short_array_superclass_fields, tweak->nb_short_array_fields, TYPE_DESCRIPTOR_SHORT) == -1)
        return -1;

    if(is_using_int) {
        if(add_getNbTArrays_bytecodes(tweak->getNbIntArrays_method, tweak->nb_int_array_superclass_fields, tweak->nb_int_array_fields) == -1)
            return -1;

        if(add_getTArray_bytecodes(tweak->getIntArray_method, class, javacard_framework, tweak->getIntArray_supercall_ref, tweak->nb_int_array_superclass_fields, tweak->nb_int_array_fields, TYPE_DESCRIPTOR_INT) == -1)
            return -1;
    }

    return 0;

}


/**
 * \brief Add supercall to super.getTArray if there is at least one instance T array
 * field.
 *
 * Each added super_methodref constant entries is put in its respective
 * class->tweak.getTArray_supercall_ref field.
 *
 * \param acf The analalyzed CAP file.
 * \param class The class to which the SynchronizableArrayFields interface is
 * added.
 * \param synclib Information on the synclib package.
 * \param is_using_int Is the int type used?
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_supercalls(analyzed_cap_file* acf, class_info* class, synclib_info* synclib, char is_using_int) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(tweak->nb_boolean_array_fields > 0)
        if((tweak->getBooleanArray_supercall_ref = add_supercall_to_constant_pool(acf, class, tweak->getBooleanArray_method->token, tweak->getBooleanArray_method->internal_overrided_method, synclib->SynchronizableArrayFields.getBooleanArray_sig)) == NULL)
            return -1;

    if(tweak->nb_byte_array_fields > 0)
        if((tweak->getByteArray_supercall_ref = add_supercall_to_constant_pool(acf, class, tweak->getByteArray_method->token, tweak->getByteArray_method->internal_overrided_method, synclib->SynchronizableArrayFields.getByteArray_sig)) == NULL)
            return -1;

    if(tweak->nb_short_array_fields > 0)
        if((tweak->getShortArray_supercall_ref = add_supercall_to_constant_pool(acf, class, tweak->getShortArray_method->token, tweak->getShortArray_method->internal_overrided_method, synclib->SynchronizableArrayFields.getShortArray_sig)) == NULL)
            return -1;

    if(is_using_int && (tweak->nb_int_array_fields > 0)) {
        if((tweak->getIntArray_supercall_ref = add_supercall_to_constant_pool(acf, class, tweak->getIntArray_method->token, tweak->getIntArray_method->internal_overrided_method, synclib->SynchronizableArrayFields.getIntArray_sig)) == NULL)
            return -1;
    }

    return 0;

}


/**
 * \brief Add methods to the given class for implementing the
 * SynchronizableArrayFields interface.
 *
 * If the given class superclass implements the SynchronizableArrayFields then
 * the setNext and getNext methods are not added.
 *
 * If the given class superclass does not implements the
 * SynchronizableArrayFields, each getNbTArrays and getTarray methods are added.
 * Else if there is at least one instance T array field, the methods
 * getNbTarrays and getTArray are added and override the inherited methods.
 *
 * \param acf The analalyzed CAP file.
 * \param class The class to which the SynchronizableArrayFields interface is
 * added.
 * \param synclib Information on the synclib package.
 * \param is_using_int Is the int type used?
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_methods(analyzed_cap_file* acf, class_info* class, synclib_info* synclib, char is_using_int) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(tweak->is_superclass_implementing_SynchronizableArrayFields_interface) {
        if(class->superclass->flags & CONSTANT_POOL_IS_EXTERNAL) {
            if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getNext", "()Lsynclib/SynchronizableArrayFields;", &(tweak->inherited_getNext_method_token)) == -1)
                return -1;

            if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "setNext", "(Lsynclib/SynchronizableArrayFields;)V", &(tweak->inherited_setNext_method_token)) == -1)
                return -1;


            if(tweak->nb_boolean_array_fields > 0) {
                if((tweak->getNbBooleanArrays_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNbBooleanArrays_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getNbBooleanArrays_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getNbBooleanArrays", "()S", &(tweak->getNbBooleanArrays_method->token)) == -1)
                    return -1;

               if((tweak->getBooleanArray_method = add_method(acf, class, synclib->SynchronizableArrayFields.getBooleanArray_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getBooleanArray_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getBooleanArray", "()[Z", &(tweak->getBooleanArray_method->token)) == -1)
                    return -1;
            } else {
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getNbBooleanArrays", "()S", &(tweak->inherited_getNbBooleanArrays_method_token)) == -1)
                    return -1;

                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getBooleanArray", "()[Z", &(tweak->inherited_getBooleanArray_method_token)) == -1)
                    return -1;
            }


            if(tweak->nb_byte_array_fields > 0) {
                if((tweak->getNbByteArrays_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNbByteArrays_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getNbByteArrays_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getNbByteArrays", "()S", &(tweak->getNbByteArrays_method->token)) == -1)
                    return -1;

                if((tweak->getByteArray_method = add_method(acf, class, synclib->SynchronizableArrayFields.getByteArray_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getByteArray_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getByteArray", "()[B", &(tweak->getByteArray_method->token)) == -1)
                    return -1;
            } else {
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getNbByteArrays", "()S", &(tweak->inherited_getNbByteArrays_method_token)) == -1)
                    return -1;

                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getByteArray", "()[B", &(tweak->inherited_getByteArray_method_token)) == -1)
                    return -1;
            }


            if(tweak->nb_short_array_fields > 0) {
                if((tweak->getNbShortArrays_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNbShortArrays_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getNbShortArrays_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getNbShortArrays", "()S", &(tweak->getNbShortArrays_method->token)) == -1)
                    return -1;

                if((tweak->getShortArray_method = add_method(acf, class, synclib->SynchronizableArrayFields.getShortArray_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getShortArray_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getShortArray", "()[S", &(tweak->getShortArray_method->token)) == -1)
                    return -1;
            } else {
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getNbShortArrays", "()S", &(tweak->inherited_getNbShortArrays_method_token)) == -1)
                    return -1;

                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getShortArray", "()[S", &(tweak->inherited_getShortArray_method_token)) == -1)
                    return -1;
            }


            if(is_using_int) {
                if(tweak->nb_int_array_fields > 0) {
                    if((tweak->getNbIntArrays_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNbIntArrays_sig, METHOD_PUBLIC)) == NULL)
                        return -1;

                    tweak->getNbIntArrays_method->is_overriding = 1;
                    if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getNbIntArrays", "()S", &(tweak->getNbIntArrays_method->token)) == -1)
                        return -1;

                    if((tweak->getIntArray_method = add_method(acf, class, synclib->SynchronizableArrayFields.getIntArray_sig, METHOD_PUBLIC)) == NULL)
                        return -1;

                    tweak->getIntArray_method->is_overriding = 1;
                    if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getIntArray", "()[I", &(tweak->getIntArray_method->token)) == -1)
                        return -1;
                } else {
                    if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getNbIntArrays", "()S", &(tweak->inherited_getNbIntArrays_method_token)) == -1)
                        return -1;

                    if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getIntArray", "()[I", &(tweak->inherited_getIntArray_method_token)) == -1)
                        return -1;
                }
            }
        } else {
            implemented_method_info* implemented_methods = tweak->SynchronizableArrayFields_methods;

            tweak->inherited_getNext_method = implemented_methods[synclib->SynchronizableArrayFields.getNext_method_token].implementation;

            tweak->inherited_setNext_method = implemented_methods[synclib->SynchronizableArrayFields.setNext_method_token].implementation;


            if(tweak->nb_boolean_array_fields > 0) {
                if((tweak->getNbBooleanArrays_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNbBooleanArrays_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getNbBooleanArrays_method->is_overriding = 1;
                tweak->getNbBooleanArrays_method->internal_overrided_method = implemented_methods[synclib->SynchronizableArrayFields.getNbBooleanArrays_method_token].implementation;

                if((tweak->getBooleanArray_method = add_method(acf, class, synclib->SynchronizableArrayFields.getBooleanArray_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getBooleanArray_method->is_overriding = 1;
                tweak->getBooleanArray_method->internal_overrided_method = implemented_methods[synclib->SynchronizableArrayFields.getBooleanArray_method_token].implementation;
            } else {
                tweak->inherited_getNbBooleanArrays_method = implemented_methods[synclib->SynchronizableArrayFields.getNbBooleanArrays_method_token].implementation;

                tweak->inherited_getBooleanArray_method = implemented_methods[synclib->SynchronizableArrayFields.getBooleanArray_method_token].implementation;
            }


            if(tweak->nb_byte_array_fields > 0) {
                if((tweak->getNbByteArrays_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNbByteArrays_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getNbByteArrays_method->is_overriding = 1;
                tweak->getNbByteArrays_method->internal_overrided_method = implemented_methods[synclib->SynchronizableArrayFields.getNbByteArrays_method_token].implementation;

                if((tweak->getByteArray_method = add_method(acf, class, synclib->SynchronizableArrayFields.getByteArray_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getByteArray_method->is_overriding = 1;
                tweak->getByteArray_method->internal_overrided_method = implemented_methods[synclib->SynchronizableArrayFields.getByteArray_method_token].implementation;
            } else {
                tweak->inherited_getNbByteArrays_method = implemented_methods[synclib->SynchronizableArrayFields.getNbByteArrays_method_token].implementation;

                tweak->inherited_getByteArray_method = implemented_methods[synclib->SynchronizableArrayFields.getByteArray_method_token].implementation;
            }


            if(tweak->nb_short_array_fields > 0) {
                if((tweak->getNbShortArrays_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNbShortArrays_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getNbShortArrays_method->is_overriding = 1;
                tweak->getNbShortArrays_method->internal_overrided_method = implemented_methods[synclib->SynchronizableArrayFields.getNbShortArrays_method_token].implementation;

                if((tweak->getShortArray_method = add_method(acf, class, synclib->SynchronizableArrayFields.getShortArray_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getShortArray_method->is_overriding = 1;
                tweak->getShortArray_method->internal_overrided_method = implemented_methods[synclib->SynchronizableArrayFields.getShortArray_method_token].implementation;
            } else {
                tweak->inherited_getNbShortArrays_method = implemented_methods[synclib->SynchronizableArrayFields.getNbShortArrays_method_token].implementation;

                tweak->inherited_getShortArray_method = implemented_methods[synclib->SynchronizableArrayFields.getShortArray_method_token].implementation;
            }


            if(is_using_int) {
                if(tweak->nb_int_array_fields > 0) {
                    if((tweak->getNbIntArrays_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNbIntArrays_sig, METHOD_PUBLIC)) == NULL)
                        return -1;

                    tweak->getNbIntArrays_method->is_overriding = 1;
                    tweak->getNbIntArrays_method->internal_overrided_method = implemented_methods[synclib->SynchronizableArrayFields.getNbIntArrays_method_token].implementation;

                    if((tweak->getIntArray_method = add_method(acf, class, synclib->SynchronizableArrayFields.getIntArray_sig, METHOD_PUBLIC)) == NULL)
                        return -1;

                    tweak->getIntArray_method->is_overriding = 1;
                    tweak->getIntArray_method->internal_overrided_method = implemented_methods[synclib->SynchronizableArrayFields.getIntArray_method_token].implementation;
                } else {
                    tweak->inherited_getNbIntArrays_method = implemented_methods[synclib->SynchronizableArrayFields.getNbIntArrays_method_token].implementation;

                    tweak->inherited_getIntArray_method = implemented_methods[synclib->SynchronizableArrayFields.getIntArray_method_token].implementation;
                }
            }
        }
        if(add_supercalls(acf, class, synclib, is_using_int) == -1)
            return -1;
    } else {
        if((tweak->getNext_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNext_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->setNext_method = add_method(acf, class, synclib->SynchronizableArrayFields.setNext_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->getNbBooleanArrays_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNbBooleanArrays_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->getBooleanArray_method = add_method(acf, class, synclib->SynchronizableArrayFields.getBooleanArray_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->getNbByteArrays_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNbByteArrays_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->getByteArray_method = add_method(acf, class, synclib->SynchronizableArrayFields.getByteArray_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->getNbShortArrays_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNbShortArrays_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->getShortArray_method = add_method(acf, class, synclib->SynchronizableArrayFields.getShortArray_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if(is_using_int) {
            if((tweak->getNbIntArrays_method = add_method(acf, class, synclib->SynchronizableArrayFields.getNbIntArrays_sig, METHOD_PUBLIC)) == NULL)
                return -1;

            if((tweak->getIntArray_method = add_method(acf, class, synclib->SynchronizableArrayFields.getIntArray_sig, METHOD_PUBLIC)) == NULL)
                return -1;
        }
    }

    return 0;

}


/**
 * \brief Add the SynchronizableArrayFields next field to the given class.
 * The added field is put in the tweak->next_field field.
 *
 * \param acf The analalyzed CAP file.
 * \param class The class to which the SynchronizableArrayFields interface is
 * added.
 * \param SynchronizableArrayFields_ref An analyzed constant pool entry to the
 * SynchronizableArrayFields interface.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_next_field(analyzed_cap_file* acf, class_info* class, constant_pool_entry_info* SynchronizableArrayFields_ref) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    tweak->next_field = add_ref_field(acf, class, SynchronizableArrayFields_ref, &SynchronizableArrayFields_type, FIELD_SHOULD_NOT_SYNCHRONIZE|FIELD_PRIVATE);
    if(tweak->next_field == NULL) {
        perror("add_next_field");
        return -1;
    }

    return 0;

}


/**
 * Generate bytecode for the given method to set the next field with the
 * SynchronizableArrayFields parameter.
 *
 * \verbatim
// Generated code

this.next = next;
return;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the SynchronizableArrayFields interface is
 * added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_setNext_bytecodes(method_info* method, class_info* class) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    bytecode_info* aload_1 = NULL;
    bytecode_info* putfield_a_this = NULL;
    bytecode_info* vreturn = NULL;

    if(method == NULL)
        return 0;

    aload_1 = create_aload_1_bytecode();
    if(aload_1 == NULL)
        return -1;

    putfield_a_this = create_putfield_T_this_bytecode(tweak->next_field);
    if(putfield_a_this == NULL)
        return -1;

    vreturn = create_return_bytecode();
    if(vreturn == NULL)
        return -1;

    method->bytecodes_count = 3;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
    if(method->bytecodes == NULL) {
        perror("add_setNext_bytecodes");
        return -1;
    }

    method->bytecodes[0] = aload_1;
    method->bytecodes[1] = putfield_a_this;
    method->bytecodes[2] = vreturn;

    method->nargs = 2;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Generate bytecode for the given method to return the next field.
 *
 * \verbatim
// Generated code

return this.next;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the SynchronizableArrayFields interface is
 * added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getNext_bytecodes(method_info* method, class_info* class) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    bytecode_info* getfield_a_this = NULL;
    bytecode_info* areturn = NULL;

    if(method == NULL)
        return 0;

    getfield_a_this = create_getfield_T_this_bytecode(tweak->next_field);
    if(getfield_a_this == NULL)
        return -1;

    areturn = create_areturn_bytecode();
    if(areturn == NULL)
        return -1;

    method->bytecodes_count = 2;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
    if(method->bytecodes == NULL) {
        perror("add_getNext_bytecodes");
        return -1;
    }

    method->bytecodes[0] = getfield_a_this;
    method->bytecodes[1] = areturn;

    method->nargs = 1;
    method->max_stack = 1;

    return 0;

}


/**
 * Generate bytecodes for the given method to return the number of instance T
 * array fields in the given class and its super classes.
 *
 * \todo Does not work when the super class is external.
 *
 * \verbatim
// Generated code

return nb_T_array_fields + nb_T_array_superclass_fields;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param nb_T_array_superclass_fields The number of T type arrays in a class
 * superclass hierarchy.
 * \param nb_T_array_fields The number of T type arrays in a class.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getNbTArrays_bytecodes(method_info* method, int16_t nb_T_array_superclass_fields, int16_t nb_T_array_fields) {

    int16_t total_nb_T_array_fields = 0;
    bytecode_info* push_value = NULL;
    bytecode_info* sreturn = NULL;

    if(method == NULL)
        return 0;

    total_nb_T_array_fields = nb_T_array_fields + nb_T_array_superclass_fields;

    push_value = create_push_short_value_bytecode(total_nb_T_array_fields);
    if(push_value == NULL)
        return -1;

    sreturn = create_sreturn_bytecode();
    if(sreturn == NULL)
        return -1;

    method->bytecodes_count = 2;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info));
    if(method->bytecodes == NULL) {
        perror("add_getNbTArrays_bytecodes");
        return -1;
    }

    method->bytecodes[0] = push_value;
    method->bytecodes[1] = sreturn;

    method->nargs = 1;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to return an instance T array given
 * its id which be in the given class or its super classes.
 *
 * \todo Does not work when the super class is external.
 *
 * \verbatim
// Generated code

if(fieldId < (short) nb_T_array_superclass_fields)
    return super.getTArray(fieldId);
switch(fieldId) {
   case nb_T_array_superclass_fields:
       return someTArrayField;
}
ISOException.throwIt(ISO7816.SW_UNKNOWN); 
return null;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the SynchronizableArrayFields interface is
 * added.
 * \param javacard_framework Information on the javacard.framework package.
 * \param supercall_ref An analyzed constant pool entry to the supercall of a
 * getTarray supermethod of the class.
 * \param nb_T_array_superclass_fields The number of T type arrays in a class
 * superclass hierarchy.
 * \param nb_T_array_fields The number of T type arrays in a class.
 * \param is_boolean_array Is the array a boolean array?
 * \param is_byte_array Is the array a byte array?
 * \param is_short_array Is the array a short array?
 * \param is_int_array Is the array an int array?
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getTArray_bytecodes(method_info* method, class_info* class, javacard_framework_info* javacard_framework, constant_pool_entry_info* supercall_ref, int16_t nb_T_array_superclass_fields, int16_t nb_T_array_fields, u1 array_type) {

    u2 u2Index = 0;
    u2 total_nb_T_array_fields = 0;
    bytecode_info** tmp = NULL;

    bytecode_info* sload_1_1 = NULL;
    bytecode_info* push_value_1 = NULL;
    bytecode_info* if_scmpge = NULL;
    bytecode_info* aload_0 = NULL;
    bytecode_info* sload_1_2 = NULL;
    bytecode_info* invokesp = NULL;
    bytecode_info* areturn_1 = NULL;

    bytecode_info* sload_1_3 = NULL;
    bytecode_info* stableswitch = NULL;
    u2 stableswitch_index = 0;
    bytecode_info** branches = NULL;
    u2 crt_case = 0;

    bytecode_info* sspush = NULL;
    bytecode_info* invokestatic = NULL;
    bytecode_info* aconst_null = NULL;
    bytecode_info* areturn_3 = NULL;

    if(method == NULL)
        return 0;

    method->bytecodes = NULL;
    method->bytecodes_count = 0;

    total_nb_T_array_fields = nb_T_array_fields + nb_T_array_superclass_fields;

    if(supercall_ref && (nb_T_array_superclass_fields > 0)) {
        sload_1_1 = create_sload_1_bytecode();
        if(sload_1_1 == NULL)
            return -1;

        push_value_1 = create_push_short_value_bytecode(nb_T_array_superclass_fields);
        if(push_value_1 == NULL)
            return -1;

        aload_0 = create_aload_0_bytecode();
        if(aload_0 == NULL)
            return -1;

        sload_1_2 = create_sload_1_bytecode();
        if(sload_1_2 == NULL)
            return -1;

        invokesp = create_invokespecial_bytecode(supercall_ref);
        if(invokesp == NULL)
            return -1;

        areturn_1 = create_areturn_bytecode();
        if(areturn_1 == NULL)
            return -1;

        method->bytecodes_count = 7;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
        if(method->bytecodes == NULL) {
            perror("add_getTArray_bytecodes");
            return -1;
        }

        method->bytecodes[0] = sload_1_1;
        method->bytecodes[1] = push_value_1;
        method->bytecodes[3] = aload_0;
        method->bytecodes[4] = sload_1_2;
        method->bytecodes[5] = invokesp;
        method->bytecodes[6] = areturn_1;

        method->max_stack = 2;
    } else {
        method->max_stack = 1;
    }

    sspush = create_push_short_value_bytecode(javacard_framework->ISO7816.sw_unknown_value);
    if(sspush == NULL)
        return -1;

    if(nb_T_array_fields != 0) {
        sload_1_3 = create_sload_1_bytecode();
        if(sload_1_3 == NULL)
            return -1;

        if(supercall_ref && (nb_T_array_superclass_fields > 0)) {
            if_scmpge = create_if_scmpge_bytecode(sload_1_3);
            if(if_scmpge == NULL)
                return -1;
            method->bytecodes[2] = if_scmpge;
        }

        branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_T_array_fields);
        if(branches == NULL)
            return -1;

        tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 2));
        if(tmp == NULL) {
            perror("add_getTArray_bytecodes");
            return -1;
        }
        method->bytecodes = tmp;
        method->bytecodes[method->bytecodes_count++] = sload_1_3;
        stableswitch_index = method->bytecodes_count;
        method->bytecodes[method->bytecodes_count++] = NULL;

        for(; u2Index < class->fields_count; ++u2Index)
            if(!(class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) && ((class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|array_type)) == (TYPE_DESCRIPTOR_ARRAY|array_type))) {
                bytecode_info* getfield_a_this = NULL;
                bytecode_info* areturn_2 = NULL;

                getfield_a_this = create_getfield_T_this_bytecode(class->fields[u2Index]->this_field);
                if(getfield_a_this == NULL)
                    return -1;

                areturn_2 = create_areturn_bytecode();
                if(areturn_2 == NULL)
                    return -1;

                tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 2));
                if(tmp == NULL) {
                    perror("add_getTArray_bytecodes");
                    return -1;
                }
                method->bytecodes = tmp;
                method->bytecodes[method->bytecodes_count++] = getfield_a_this;
                method->bytecodes[method->bytecodes_count++] = areturn_2;
                branches[crt_case++] = getfield_a_this;
            }

        stableswitch = create_stableswitch_bytecode(nb_T_array_superclass_fields, total_nb_T_array_fields - 1, branches, sspush);

        if(stableswitch == NULL)
            return -1;
        method->bytecodes[stableswitch_index] = stableswitch;
    } else {
        if(supercall_ref && (nb_T_array_superclass_fields > 0)) {
            if_scmpge = create_if_scmpge_bytecode(sspush);
            if(if_scmpge == NULL)
                return -1;
            method->bytecodes[2] = if_scmpge;
        }
    }

    invokestatic = create_invokestatic_bytecode(javacard_framework->ISOException.throwIt_ref);
    if(invokestatic == NULL)
        return -1;

    aconst_null = create_aconst_null_bytecode();
    if(aconst_null == NULL)
        return -1;

    areturn_3 = create_areturn_bytecode();
    if(areturn_3 == NULL)
        return -1;
    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 4));
    if(tmp == NULL) {
        perror("add_getTArray_bytecodes");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[method->bytecodes_count++] = sspush;
    method->bytecodes[method->bytecodes_count++] = invokestatic;
    method->bytecodes[method->bytecodes_count++] = aconst_null;
    method->bytecodes[method->bytecodes_count++] = areturn_3;

    method->nargs = 2;

    return 0;

}
