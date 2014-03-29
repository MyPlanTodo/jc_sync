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
 * \file synchronizable_itf_functions.c
 * \brief Add the Synchronizable interface and its methods implementation to a
 * class.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <analyzed_cap_file.h>

#include "tweak_info.h"
#include "javacard_framework_info.h"
#include "synclib_info.h"
#include "generic_tweak_functions.h"
#include "tweak_class_extending_applet_functions.h"
#include "add_field_functions.h"
#include "create_bytecode_functions.h"

static type_descriptor_info* Synchronizable_type = NULL;

static int add_methods(analyzed_cap_file* acf, class_info* class, synclib_info* synclib);
static int add_isDuplicated_field(analyzed_cap_file* acf, class_info* class);
static int add_id_field(analyzed_cap_file* acf, class_info* class);
static int add_previous_field(analyzed_cap_file* acf, class_info* class, constant_pool_entry_info* Synchronizable_ref);
static int add_currentField_field(analyzed_cap_file* acf, class_info* class);
static int add_getId_bytecodes(method_info* method, class_info* class);
static int add_setId_bytecodes(method_info* method, class_info* class);
static int add_isDuplicated_bytecodes(method_info* method, class_info* class);
static int add_getPreviousSynchronizableInstance_bytecodes(method_info* method, class_info* class);
static int add_setPreviousSynchronizableInstance_bytecodes(method_info* method, class_info* class);
static int add_resetPath_bytecodes(method_info* method, class_info* class, synclib_info* synclib);
static int add_numberPath_bytecodes(method_info* method, class_info* class, synclib_info* synclib);
static int add_serialize_bytecodes(method_info* method, class_info* class, synclib_info* synclib);
static int add_merge_bytecodes(method_info* method, class_info* class, synclib_info* synclib);
static int add_init_bytecodes(method_info* method, class_info* class, javacard_framework_info* javacard_framework);



/**
 * \brief Add the Synchronizable interface to the given class, add implementing
 * methods if needed and add supporting fields if needed.
 *
 * If the given class has synclib.SyncApplet as direct superclass or if its direct
 * superclass does not implement Synchronizable or if it does not have a
 * superclass, the short id field, the boolean isDuplicated field and the
 * Synchronizable previous field are added.
 *
 * If the given class has more than one instance field or has at least an instance array
 * field or an instance reference field or has a Synchronizable superclass
 * (except synclib.SyncApplet) and at least one instance field, the short
 * currentField field is added.
 *
 * Methods are added if needed and override inherited methods if needed.
 * Needed super_methodref constant pool entries are also added if needed.
 * Bytecodes are generated for each added method.
 *
 * \param acf The analyzed CAP file.
 * \param class The class to which the Synchronizable interface is added.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
int add_Synchronizable_interface(analyzed_cap_file* acf, class_info* class, javacard_framework_info* javacard_framework, synclib_info* synclib) {

    implemented_interface_info* tmp = NULL;
    tweak_info* tweak = (tweak_info*)class->tweak;

    // Add supporting fields
    if(tweak->is_SyncApplet_immediate_superclass || !tweak->is_superclass_implementing_Synchronizable_interface) {
        if(add_id_field(acf, class) == -1)
            return -1;

        if(add_isDuplicated_field(acf, class) == -1)
            return -1;

        if(add_previous_field(acf, class, synclib->Synchronizable.interface_ref) == -1)
            return -1;
    }

    if((tweak->nb_fields > 1) || ((tweak->nb_fields == 1) && !tweak->is_SyncApplet_immediate_superclass && tweak->is_superclass_implementing_Synchronizable_interface) || (tweak->nb_array_fields > 0) || (tweak->nb_ref_fields > 0))
        if(add_currentField_field(acf, class) == -1)
            return -1;


    // add Synchronizable methods to the class
    if(add_methods(acf, class, synclib) == -1)
        return -1;

    // add the interface to the class
    if((tmp = (implemented_interface_info*)realloc(class->interfaces, sizeof(implemented_interface_info) * (class->interfaces_count + 1))) == NULL) {
        perror("add_Synchronizable_interface");
        return -1;
    }

    class->interfaces = tmp;

    class->interfaces[class->interfaces_count].ref = synclib->Synchronizable.interface_ref;

    class->interfaces[class->interfaces_count].count = 9;
    class->interfaces[class->interfaces_count].index = (implemented_method_info*)malloc(sizeof(implemented_method_info) * class->interfaces[class->interfaces_count].count);
    if(class->interfaces[class->interfaces_count].index == NULL) {
        perror("add_Synchronizable_interface");
        return -1;
    }
    memset(class->interfaces[class->interfaces_count].index, 0, sizeof(implemented_method_info) * class->interfaces[class->interfaces_count].count);

    // Link the implementation
    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.getId_method_token].implementation = tweak->getId_method ? tweak->getId_method : tweak->inherited_getId_method;
    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.getId_method_token].method_token = tweak->inherited_getId_method_token;

    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.setId_method_token].implementation = tweak->setId_method ? tweak->setId_method : tweak->inherited_setId_method;
    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.setId_method_token].method_token = tweak->inherited_setId_method_token;

    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.isDuplicated_method_token].implementation = tweak->isDuplicated_method ? tweak->isDuplicated_method : tweak->inherited_isDuplicated_method;
    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.isDuplicated_method_token].method_token = tweak->inherited_isDuplicated_method_token;

    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.getPreviousSynchronizableInstance_method_token].implementation = tweak->getPreviousSynchronizableInstance_method ? tweak->getPreviousSynchronizableInstance_method : tweak->inherited_getPreviousSynchronizableInstance_method;
    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.getPreviousSynchronizableInstance_method_token].method_token = tweak->inherited_getPreviousSynchronizableInstance_method_token;

    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.setPreviousSynchronizableInstance_method_token].implementation = tweak->setPreviousSynchronizableInstance_method ? tweak->setPreviousSynchronizableInstance_method : tweak->inherited_setPreviousSynchronizableInstance_method;
    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.setPreviousSynchronizableInstance_method_token].method_token = tweak->inherited_setPreviousSynchronizableInstance_method_token;

    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.resetPath_method_token].implementation = tweak->resetPath_method ? tweak->resetPath_method : tweak->inherited_resetPath_method;
    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.resetPath_method_token].method_token = tweak->inherited_resetPath_method_token;

    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.numberPath_method_token].implementation = tweak->numberPath_method ? tweak->numberPath_method : tweak->inherited_numberPath_method;
    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.numberPath_method_token].method_token = tweak->inherited_numberPath_method_token;

    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.serialize_method_token].implementation = tweak->serialize_method ? tweak->serialize_method : tweak->inherited_serialize_method;
    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.serialize_method_token].method_token = tweak->inherited_serialize_method_token;

    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.merge_method_token].implementation = tweak->merge_method ? tweak->merge_method : tweak->inherited_merge_method;
    class->interfaces[class->interfaces_count].index[synclib->Synchronizable.merge_method_token].method_token = tweak->inherited_merge_method_token;

    ++class->interfaces_count;


    // Generate the bytecodes
    if(add_getId_bytecodes(tweak->getId_method, class) == -1)
        return -1;

    if(add_setId_bytecodes(tweak->setId_method, class) == -1)
        return -1;

    if(add_isDuplicated_bytecodes(tweak->isDuplicated_method, class) == -1)
        return -1;

    if(add_getPreviousSynchronizableInstance_bytecodes(tweak->getPreviousSynchronizableInstance_method, class) == -1)
        return -1;

    if(add_setPreviousSynchronizableInstance_bytecodes(tweak->setPreviousSynchronizableInstance_method, class) == -1)
        return -1;

    if(add_resetPath_bytecodes(tweak->resetPath_method, class, synclib) == -1)
        return -1;

    if(add_numberPath_bytecodes(tweak->numberPath_method, class, synclib) == -1)
        return -1;

    if(add_serialize_bytecodes(tweak->serialize_method, class, synclib) == -1)
        return -1;

    if(add_merge_bytecodes(tweak->merge_method, class, synclib) == -1)
        return -1;

    if(add_init_bytecodes(tweak->init_method, class, javacard_framework) == -1)
        return -1;

    return 0;

}


/**
 * \brief Add supercall to super.resetPath, super.serialize and super.merge if
 * the calling method was added.
 *
 * super_methodref constant pool entries are put (if added) respectively in
 * tweak->resetPath_supercall_ref, tweak->serialize_supercall_ref
 * and tweak->merge_supercall_ref fields.
 *
 * \param acf The analyzed CAP file.
 * \param class The class to which the Synchronizable interface is added.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_supercalls(analyzed_cap_file* acf, class_info* class, synclib_info* synclib) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(tweak->resetPath_method)
        if((tweak->resetPath_supercall_ref = add_supercall_to_constant_pool(acf, class, tweak->resetPath_method->token, tweak->resetPath_method->internal_overrided_method, synclib->Synchronizable.resetPath_sig)) == NULL)
            return -1;

    if(tweak->serialize_method)
        if((tweak->serialize_supercall_ref = add_supercall_to_constant_pool(acf, class, tweak->serialize_method->token, tweak->serialize_method->internal_overrided_method, synclib->Synchronizable.serialize_sig)) == NULL)
            return -1;

    if(tweak->merge_method)
        if((tweak->merge_supercall_ref = add_supercall_to_constant_pool(acf, class, tweak->merge_method->token, tweak->merge_method->internal_overrided_method, synclib->Synchronizable.merge_sig)) == NULL)
            return -1;

    return 0;

}


/**
 * \brief Add methods to the given class for implementing the Synchronizable interface.
 * 
 * If the given class direct superclass implements the Synchronizable interface
 * and is not synclib.SyncApplet, the getId method, the setId method, the
 * isDuplicated method, the getPreviousSynchronizableInstance method, the
 * setPreviousSynchronizableInstance method and the numberPath method are not
 * added but simply inherited.
 * 
 * Moreover if the given class does not have at least one primitive instance
 * field or array instance fied, the serialize method and the merge method are
 * simply inherited. Overriding is done when needed.
 * 
 * A constructor with an synclib.Input parameter is also added for building empty
 * object.
 * 
 * \param acf The analyzed CAP file.
 * \param class The class to which the Synchronizable interface is added.
 * \param synclib Information on the synclib package.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_methods(analyzed_cap_file* acf, class_info* class, synclib_info* synclib) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(tweak->has_SyncApplet_in_class_hierarchy) {
        if(tweak->is_SyncApplet_immediate_superclass) {
            if((tweak->getId_method = add_overriding_method(acf, class, synclib->Synchronizable.getId_sig, synclib->SyncApplet.getId_method_token)) == NULL)
                return -1;

            if((tweak->setId_method = add_overriding_method(acf, class, synclib->Synchronizable.setId_sig, synclib->SyncApplet.setId_method_token)) == NULL)
                return -1;

            if((tweak->isDuplicated_method = add_overriding_method(acf, class, synclib->Synchronizable.isDuplicated_sig, synclib->SyncApplet.isDuplicated_method_token)) == NULL)
                return -1;

            if((tweak->getPreviousSynchronizableInstance_method = add_overriding_method(acf, class, synclib->Synchronizable.getPreviousSynchronizableInstance_sig, synclib->SyncApplet.getPreviousSynchronizableInstance_method_token)) == NULL)
                return -1;

            if((tweak->setPreviousSynchronizableInstance_method = add_overriding_method(acf, class, synclib->Synchronizable.setPreviousSynchronizableInstance_sig, synclib->SyncApplet.setPreviousSynchronizableInstance_method_token)) == NULL)
                return -1;
        } else {
            tweak->inherited_getId_method_token = synclib->SyncApplet.getId_method_token;
            tweak->inherited_setId_method_token = synclib->SyncApplet.setId_method_token;
            tweak->inherited_isDuplicated_method_token = synclib->SyncApplet.isDuplicated_method_token;
            tweak->inherited_getPreviousSynchronizableInstance_method_token = synclib->SyncApplet.getPreviousSynchronizableInstance_method_token;
            tweak->inherited_setPreviousSynchronizableInstance_method_token = synclib->SyncApplet.setPreviousSynchronizableInstance_method_token;
        }

        if(tweak->currentField_field || tweak->is_SyncApplet_immediate_superclass) {
            if((tweak->resetPath_method = add_overriding_method(acf, class, synclib->Synchronizable.resetPath_sig, synclib->SyncApplet.resetPath_method_token)) == NULL)
                return -1;
        } else {
            tweak->inherited_resetPath_method_token = synclib->SyncApplet.resetPath_method_token;
        }

        if(tweak->is_SyncApplet_immediate_superclass) {
            if((tweak->numberPath_method = add_overriding_method(acf, class, synclib->Synchronizable.numberPath_sig, synclib->SyncApplet.numberPath_method_token)) == NULL)
                return -1;
        } else {
            tweak->inherited_numberPath_method_token = synclib->SyncApplet.numberPath_method_token;
        }

        if(((tweak->nb_fields + tweak->nb_array_fields) > 0) || tweak->is_SyncApplet_immediate_superclass) {
            if((tweak->serialize_method = add_overriding_method(acf, class, synclib->Synchronizable.serialize_sig, synclib->SyncApplet.serialize_method_token)) == NULL)
                return -1;

            if((tweak->merge_method = add_overriding_method(acf, class, synclib->Synchronizable.merge_sig, synclib->SyncApplet.merge_method_token)) == NULL)
                return -1;
        } else {
            tweak->inherited_serialize_method_token = synclib->SyncApplet.serialize_method_token;
            tweak->inherited_merge_method_token = synclib->SyncApplet.merge_method_token;
        }

        if(!tweak->is_SyncApplet_immediate_superclass)
            if(add_supercalls(acf, class, synclib) == -1)
                return -1;
    } else if(tweak->is_superclass_implementing_Synchronizable_interface) {
        if(class->superclass->flags & CONSTANT_POOL_IS_EXTERNAL) { 
            if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getId", "()S", &(tweak->inherited_getId_method_token)) == -1)
                return -1;

            if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "setId", "(S)V", &(tweak->inherited_setId_method_token)) == -1)
                return -1;

            if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "isDuplicated", "()Z", &(tweak->inherited_isDuplicated_method_token)) == -1)
                return -1;

            if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getPreviousSynchronizableInstance", "()Lsynclib/Synchronizable;", &(tweak->inherited_getPreviousSynchronizableInstance_method_token)) == -1)
                return -1;

            if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "setPreviousSynchronizableInstance", "(Lsynclib/Synchronizable;)V", &(tweak->inherited_setPreviousSynchronizableInstance_method_token)) == -1)
                return -1;

            if(tweak->currentField_field) {
                if((tweak->resetPath_method = add_method(acf, class, synclib->Synchronizable.resetPath_sig, METHOD_PUBLIC)) == NULL)
                    return -1;
                tweak->resetPath_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "resetPath", "(Lsynclib/ResetIdSource;)Z", &(tweak->resetPath_method->token)) == -1)
                    return -1;
            } else {
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "resetPath", "(Lsynclib/ResetIdSource;)Z", &(tweak->inherited_resetPath_method_token)) == -1)
                    return -1;
            }

            if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "numberPath", "(Lsynclib/Output;)Z", &(tweak->inherited_numberPath_method_token)) == -1)
                return -1;

            if((tweak->nb_fields + tweak->nb_array_fields) != 0) {
                if((tweak->serialize_method = add_method(acf, class, synclib->Synchronizable.serialize_sig, METHOD_PUBLIC)) == NULL)
                    return -1;
                tweak->serialize_method->is_overriding = 1;

                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "serialize", "(Lsynclib/Output;)V", &(tweak->serialize_method->token)) == -1)
                    return -1;

                if((tweak->merge_method = add_method(acf, class, synclib->Synchronizable.merge_sig, METHOD_PUBLIC)) == NULL)
                    return -1;
                tweak->merge_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "merge", "(Lsynclib/Input;)V", &(tweak->merge_method->token)) == -1)
                    return -1;
            } else {
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "serialize", "(Lsynclib/Output;)V", &(tweak->inherited_serialize_method_token)) == -1)
                    return -1;

                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "merge", "(Lsynclib/Input;)V", &(tweak->inherited_merge_method_token)) == -1)
                    return -1;
            }
        } else {
            implemented_method_info* implemented_methods = tweak->Synchronizable_methods;

            tweak->inherited_getId_method = implemented_methods[synclib->Synchronizable.getId_method_token].implementation;

            tweak->inherited_setId_method = implemented_methods[synclib->Synchronizable.setId_method_token].implementation;

            tweak->inherited_isDuplicated_method = implemented_methods[synclib->Synchronizable.isDuplicated_method_token].implementation;

            tweak->inherited_getPreviousSynchronizableInstance_method = implemented_methods[synclib->Synchronizable.getPreviousSynchronizableInstance_method_token].implementation;

        tweak->inherited_setPreviousSynchronizableInstance_method = implemented_methods[synclib->Synchronizable.setPreviousSynchronizableInstance_method_token].implementation;

            if(tweak->currentField_field) {
                if((tweak->resetPath_method = add_method(acf, class, synclib->Synchronizable.resetPath_sig, METHOD_PUBLIC)) == NULL)
                return -1;
                tweak->resetPath_method->is_overriding = 1;
                tweak->resetPath_method->internal_overrided_method = implemented_methods[synclib->Synchronizable.resetPath_method_token].implementation;
            } else {
                tweak->inherited_resetPath_method = implemented_methods[synclib->Synchronizable.resetPath_method_token].implementation;
            }

            tweak->inherited_numberPath_method = implemented_methods[synclib->Synchronizable.numberPath_method_token].implementation;

            if((tweak->nb_fields + tweak->nb_array_fields) != 0) {
                if((tweak->serialize_method = add_method(acf, class, synclib->Synchronizable.serialize_sig, METHOD_PUBLIC)) == NULL)
                    return -1;
                tweak->serialize_method->is_overriding = 1;
                tweak->serialize_method->internal_overrided_method = implemented_methods[synclib->Synchronizable.serialize_method_token].implementation;

                if((tweak->merge_method = add_method(acf, class, synclib->Synchronizable.merge_sig, METHOD_PUBLIC)) == NULL)
                    return -1;
                tweak->merge_method->is_overriding = 1;
                tweak->merge_method->internal_overrided_method = implemented_methods[synclib->Synchronizable.merge_method_token].implementation;
            } else {
                tweak->inherited_serialize_method = implemented_methods[synclib->Synchronizable.serialize_method_token].implementation;
                tweak->inherited_merge_method = implemented_methods[synclib->Synchronizable.merge_method_token].implementation;
            }
        }

        if(add_supercalls(acf, class, synclib) == -1)
            return -1;
    } else {
        if((tweak->getId_method = add_method(acf, class, synclib->Synchronizable.getId_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->setId_method = add_method(acf, class, synclib->Synchronizable.setId_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->isDuplicated_method = add_method(acf, class, synclib->Synchronizable.isDuplicated_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->getPreviousSynchronizableInstance_method = add_method(acf, class, synclib->Synchronizable.getPreviousSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->setPreviousSynchronizableInstance_method = add_method(acf, class, synclib->Synchronizable.setPreviousSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->resetPath_method = add_method(acf, class, synclib->Synchronizable.resetPath_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->numberPath_method = add_method(acf, class, synclib->Synchronizable.numberPath_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->serialize_method = add_method(acf, class, synclib->Synchronizable.serialize_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->merge_method = add_method(acf, class, synclib->Synchronizable.merge_sig, METHOD_PUBLIC)) == NULL)
            return -1;
    }

    if((tweak->init_method = add_method(acf, class, synclib->Synchronizable.init_method_sig, METHOD_PUBLIC|METHOD_INIT)) == NULL)
        return -1;

    return 0;

}


/**
 * \brief Add the boolean isDuplicated field to the given class.
 * 
 * The added field is put in tweak->isDuplicated_field field.
 * 
 * \param acf The analyzed CAP file.
 * \param class The class to which the Synchronizable interface is added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_isDuplicated_field(analyzed_cap_file* acf, class_info* class) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    tweak->isDuplicated_field = add_boolean_field(acf, class, FIELD_SHOULD_NOT_SYNCHRONIZE|FIELD_PRIVATE);
    if(tweak->isDuplicated_field == NULL) {
        perror("add_isDuplicated_field");
        return -1;
    }

    return 0;

}


/**
 * \brief Add the short id field to the given class.
 *
 * The added field is put in tweak->id_field field.
 * 
 * \param acf The analyzed CAP file.
 * \param class The class to which the Synchronizable interface is added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_id_field(analyzed_cap_file* acf, class_info* class) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    tweak->id_field = add_short_field(acf, class, FIELD_SHOULD_NOT_SYNCHRONIZE|FIELD_PRIVATE);
    if(tweak->id_field == NULL) {
        perror("add_id_field");
        return -1;
    }

    return 0;

}


/**
 * \brief Add the Synchronizable previous field to the given class.
 * 
 * The added field is put in tweak->previous_field field.
 * 
 * \param acf The analyzed CAP file.
 * \param class The class to which the Synchronizable interface is added.
 * \param Synchronizable_ref The analyzed constant pool entry to the
 * Synchronizable interface.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_previous_field(analyzed_cap_file* acf, class_info* class, constant_pool_entry_info* Synchronizable_ref) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    tweak->previous_field = add_ref_field(acf, class, Synchronizable_ref, &Synchronizable_type, FIELD_SHOULD_NOT_SYNCHRONIZE|FIELD_PRIVATE);
    if(tweak->previous_field == NULL) {
        perror("add_previous_field");
        return -1;
    }

    return 0;

}


/**
 * \brief Add the short currentField field to the given class.
 *
 * The added field is put in class->currentField_field field.
 * 
 * \param acf The analyzed CAP file.
 * \param class The class to which the Synchronizable interface is added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_currentField_field(analyzed_cap_file* acf, class_info* class) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    tweak->currentField_field = add_short_field(acf, class, FIELD_SHOULD_NOT_SYNCHRONIZE|FIELD_PRIVATE);
    if(tweak->currentField_field == NULL) {
        perror("add_currentField_field");
        return -1;
    }

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to return the
 * tweak->id_field field.
 * 
 * \verbatim
// Generated code

return this.id;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the Synchronizable interface is added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getId_bytecodes(method_info* method, class_info* class) {

    bytecode_info* getfield_s_this = NULL;
    bytecode_info* sreturn = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(method == NULL)
        return 0;

    method->bytecodes_count = 2;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
    if(method->bytecodes == NULL) {
        perror("add_getId_bytecodes");
        return -1;
    }

    getfield_s_this = create_getfield_T_this_bytecode(tweak->id_field);
    if(getfield_s_this == NULL)
        return -1;

    sreturn = create_sreturn_bytecode();
    if(sreturn == NULL)
        return -1;

    method->bytecodes[0] = getfield_s_this;
    method->bytecodes[1] = sreturn;
    method->nargs = 1;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to set the
 * tweak->id_field field with the short parameter.
 *
 * \verbatim
// Generated code

this.id = id;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the Synchronizable interface is added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_setId_bytecodes(method_info* method, class_info* class) {

    bytecode_info* sload_1 = NULL;
    bytecode_info* putfield_s_this = NULL;
    bytecode_info* void_return = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(method == NULL)
        return 0;

    sload_1 = create_sload_1_bytecode();
    if(sload_1 == NULL)
        return -1;

    putfield_s_this = create_putfield_T_this_bytecode(tweak->id_field);
    if(putfield_s_this == NULL)
        return -1;

    void_return = create_return_bytecode();
    if(void_return == NULL)
        return -1;

    method->bytecodes_count = 3;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
    if(method->bytecodes == NULL) {
        perror("add_setId_bytecodes");
        return -1;
    }

    method->bytecodes[0] = sload_1;
    method->bytecodes[1] = putfield_s_this;
    method->bytecodes[2] = void_return;

    method->nargs = 2;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to return the
 * tweak->isDuplicated_field field.
 *
 * \verbatim
// Generated code

return this.isDuplicated;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the Synchronizable interface is added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_isDuplicated_bytecodes(method_info* method, class_info* class) {

    bytecode_info* getfield_b_this = NULL;
    bytecode_info* sreturn = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(method == NULL)
        return 0;

    method->bytecodes_count = 2;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info) * method->bytecodes_count);
    if(method->bytecodes == NULL) {
        perror("add_isDuplicated_bytecodes");
        return -1;
    }
    
    getfield_b_this = create_getfield_T_this_bytecode(tweak->isDuplicated_field);
    if(getfield_b_this == NULL)
        return -1;

    sreturn = create_sreturn_bytecode();
    if(sreturn == NULL)
        return -1;

    method->bytecodes[0] = getfield_b_this;
    method->bytecodes[1] = sreturn;
    method->nargs = 1;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to return the
 * tweak->previous_field field.
 * 
 * \verbatim
// Generated code

return this.previous;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the Synchronizable interface is added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getPreviousSynchronizableInstance_bytecodes(method_info* method, class_info* class) {

    bytecode_info* getfield_a_this = NULL;
    bytecode_info* areturn = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(method == NULL)
        return 0;

    method->bytecodes_count = 2;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info) * method->bytecodes_count);
    if(method->bytecodes == NULL) {
        perror("add_getPreviousSynchronizableInstance_bytecodes");
        return -1;
    }

    getfield_a_this = create_getfield_T_this_bytecode(tweak->previous_field);
    if(getfield_a_this == NULL)
        return -1;

    areturn = create_areturn_bytecode();
    if(areturn == NULL)
        return -1;

    method->bytecodes[0] = getfield_a_this;
    method->bytecodes[1] = areturn;
    method->nargs = 1;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to set the
 * tweak->previous_field with the Synchronizable parameter.
 *
 * \verbatim
// Generated code

this.previous = previous;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the Synchronizable interface is added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_setPreviousSynchronizableInstance_bytecodes(method_info* method, class_info* class) {

    bytecode_info* aload_1 = NULL;
    bytecode_info* putfield_a_this = NULL;
    bytecode_info* void_return = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(method == NULL)
        return 0;

    aload_1 = create_aload_1_bytecode();
    if(aload_1 == NULL)
        return -1;

    putfield_a_this = create_putfield_T_this_bytecode(tweak->previous_field);
    if(putfield_a_this == NULL)
        return -1;

    void_return = create_return_bytecode();
    if(void_return == NULL)
        return -1;

    method->bytecodes_count = 3;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
    if(method->bytecodes == NULL) {
        perror("add_setPreviousSynchronizableInstance_bytecodes");
        return -1;
    }

    method->bytecodes[0] = aload_1;
    method->bytecodes[1] = putfield_a_this;
    method->bytecodes[2] = void_return;

    method->nargs = 2;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Generate the bytecode for the given method to reset the supporting
 * field added to the given class and call super.resetPath is available.
 * 
 * If tweak->resetPath_supercall_ref is not NULL then only
 * tweak->currentField_field field is reset and a call to super.resetPath
 * is done.
 *
 * Else tweak->id_field field and tweak->isDuplicated_field field
 * are reset and if tweak->currentField_field field is not NULL, it is
 * reset.
 *
 * \verbatim
// Generated code

currentField = (short)0;
return super.resetPath(source);
\endverbatim
 * or
 * \verbatim
// Generated code

short resetId = source.getResetId();
if(id == resetId)
    return false;
id = resetId;
isDuplicated = false;
currentField = (short)0;
return true;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the Synchronizable interface is added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_resetPath_bytecodes(method_info* method, class_info* class, synclib_info* synclib) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(method == NULL)
        return 0;

    if(tweak->resetPath_supercall_ref) {
        bytecode_info* sconst_0 = NULL;
        bytecode_info* putfield_s_this = NULL;
        bytecode_info* aload_0 = NULL;
        bytecode_info* aload_1 = NULL;
        bytecode_info* invokesp = NULL;
        bytecode_info* sreturn = NULL;

        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * 6);
        if(method->bytecodes == NULL) {
            perror("add_resetPath_bytecodes");
            return -1;
        }
        method->bytecodes_count = 0;

        sconst_0 = create_push_short_value_bytecode(0);
        if(sconst_0 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = sconst_0;

        putfield_s_this = create_putfield_T_this_bytecode(tweak->currentField_field);
        if(putfield_s_this == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = putfield_s_this;

        aload_0 = create_aload_0_bytecode();
        if(aload_0 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = aload_0;

        aload_1 = create_aload_1_bytecode();
        if(aload_1 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = aload_1;

        invokesp = create_invokespecial_bytecode(tweak->resetPath_supercall_ref);
        if(invokesp == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = invokesp;

        sreturn = create_sreturn_bytecode();
        if(sreturn == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = sreturn;

        method->nargs = 2;
        method->max_stack = 2;
    } else {
        bytecode_info* aload_1 = NULL;
        bytecode_info* invokeitf = NULL;
        bytecode_info* sstore_2 = NULL;
        bytecode_info* getfield_s_this = NULL;
        bytecode_info* sload_2_1 = NULL;
        bytecode_info* if_scmpne = NULL;
        bytecode_info* sconst_0_1 = NULL;
        bytecode_info* sreturn_1 = NULL;
        bytecode_info* sload_2_2 = NULL;
        bytecode_info* putfield_s_this_1 = NULL;
        bytecode_info* sconst_0_2 = NULL;
        bytecode_info* putfield_b_this = NULL;
        bytecode_info* sconst_0_3 = NULL;
        bytecode_info* putfield_s_this_2 = NULL;
        bytecode_info* sconst_1 = NULL;
        bytecode_info* sreturn_2 = NULL;

        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * (14 + (tweak->currentField_field ? 2 : 0)));
        if(method->bytecodes == NULL) {
            perror("add_resetPath_bytecodes");
            return -1;
        }
        method->bytecodes_count = 0;

        aload_1 = create_aload_1_bytecode();
        if(aload_1 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = aload_1;

        invokeitf = create_invokeinterface_bytecode(synclib->ResetIdSource.interface_ref, synclib->ResetIdSource.getResetId_method_token, 1);
        if(invokeitf == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = invokeitf;

        sstore_2 = create_sstore_2_bytecode();
        if(sstore_2 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = sstore_2;

        getfield_s_this = create_getfield_T_this_bytecode(tweak->id_field);
        if(getfield_s_this == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = getfield_s_this;

        sload_2_1 = create_sload_2_bytecode();
        if(sload_2_1 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = sload_2_1;

        ++method->bytecodes_count;

        sconst_0_1 = create_push_short_value_bytecode(0);
        if(sconst_0_1 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = sconst_0_1;

        sreturn_1 = create_sreturn_bytecode();
        if(sreturn_1 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = sreturn_1;

        sload_2_2 = create_sload_2_bytecode();
        if(sload_2_2 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = sload_2_2;

        if_scmpne = create_if_scmpne_bytecode(sload_2_2);
        if(if_scmpne == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count - 4] = if_scmpne;

        putfield_s_this_1 = create_putfield_T_this_bytecode(tweak->id_field);
        if(putfield_s_this_1 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = putfield_s_this_1;

        sconst_0_2 = create_push_short_value_bytecode(0);
        if(sconst_0_2 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = sconst_0_2;

        putfield_b_this = create_putfield_T_this_bytecode(tweak->isDuplicated_field);
        if(putfield_b_this == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = putfield_b_this;

        if(tweak->currentField_field != NULL) {
            sconst_0_3 = create_push_short_value_bytecode(0);
            if(sconst_0_3 == NULL)
                return -1;

            method->bytecodes[method->bytecodes_count++] = sconst_0_3;

            putfield_s_this_2 = create_putfield_T_this_bytecode(tweak->currentField_field);
            if(putfield_s_this_2 == NULL)
                return -1;

            method->bytecodes[method->bytecodes_count++] = putfield_s_this_2;
        }

        sconst_1 = create_push_short_value_bytecode(1);
        if(sconst_1 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = sconst_1;

        sreturn_2 = create_sreturn_bytecode();
        if(sreturn_2 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = sreturn_2;

        method->nargs = 2;
        method->max_stack = 2;
        method->max_locals = 1;
    }

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to number the given class
 * instance.
 *
 * \verbatim
// Generated code

if(id > (short)-1) {
    isDuplicated = true;
    return false;
}
id = out.getNextId();
return true;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the Synchronizable interface is added.
 * \param synclib Information on the synclib package.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_numberPath_bytecodes(method_info* method, class_info* class, synclib_info* synclib) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    bytecode_info* getfield_s_this = NULL;
    bytecode_info* sconst_m1 = NULL;
    bytecode_info* if_scmple = NULL;
    bytecode_info* sconst_1_1 = NULL;
    bytecode_info* putfield_b_this = NULL;
    bytecode_info* sconst_0 = NULL;
    bytecode_info* sreturn_1 = NULL;
    bytecode_info* aload_1 = NULL;
    bytecode_info* invokevirtual = NULL;
    bytecode_info* putfield_s_this = NULL;
    bytecode_info* sconst_1_2 = NULL;
    bytecode_info* sreturn_2 = NULL;

    if(method == NULL)
        return 0;

    getfield_s_this = create_getfield_T_this_bytecode(tweak->id_field);
    if(getfield_s_this == NULL)
        return -1;

    sconst_m1 = create_push_short_value_bytecode(-1);
    if(sconst_m1 == NULL)
        return -1;

    sconst_1_1 = create_push_short_value_bytecode(1);
    if(sconst_1_1 == NULL)
        return -1;

    putfield_b_this = create_putfield_T_this_bytecode(tweak->isDuplicated_field);
    if(putfield_b_this == NULL)
        return -1;

    sconst_0 = create_push_short_value_bytecode(0);
    if(sconst_0 == NULL)
        return -1;

    sreturn_1 = create_sreturn_bytecode();
    if(sreturn_1 == NULL)
        return -1;

    aload_1 = create_aload_1_bytecode();
    if(aload_1 == NULL)
        return -1;

    if_scmple = create_if_scmple_bytecode(aload_1);
    if(if_scmple == NULL)
        return -1;

    invokevirtual = create_invokevirtual_bytecode(synclib->Output.getNextId_ref);
    if(invokevirtual == NULL)
        return -1;

    putfield_s_this = create_putfield_T_this_bytecode(tweak->id_field);
    if(putfield_s_this == NULL)
        return -1;

    sconst_1_2 = create_push_short_value_bytecode(1);
    if(sconst_1_2 == NULL)
        return -1;

    sreturn_2 = create_sreturn_bytecode();
    if(sreturn_2 == NULL)
        return -1;

    method->bytecodes_count = 12;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
    if(method->bytecodes == NULL) {
        perror("add_numberPath_bytecodes");
        return -1;
    }

    method->bytecodes[0] = getfield_s_this;
    method->bytecodes[1] = sconst_m1;
    method->bytecodes[2] = if_scmple;
    method->bytecodes[3] = sconst_1_1;
    method->bytecodes[4] = putfield_b_this;
    method->bytecodes[5] = sconst_0;
    method->bytecodes[6] = sreturn_1;
    method->bytecodes[7] = aload_1;
    method->bytecodes[8] = invokevirtual;
    method->bytecodes[9] = putfield_s_this;
    method->bytecodes[10] = sconst_1_2;
    method->bytecodes[11] = sreturn_2;

    method->nargs = 2;
    method->max_stack = 2;

    return 0;

}


/**
 * \brief Append bytecodes for reseting the resume mechanism of the input or
 * output and incrementing the currentField field.
 *
 * \verbatim
// Generated code

out.resetResume()
// or (aload_1 bytecode does not care)
in.resetResume()
++currentField;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param currentField_field An analyzed constant pool entry to a currentField
 * instance field.
 * \param resetResume_ref An analyzed constant pool entry to the resetResume
 * virtual method.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_resetResume_bytecodes(method_info* method, constant_pool_entry_info* currentField_field, constant_pool_entry_info* resetResume_ref) {

    bytecode_info** tmp = NULL;
    bytecode_info* aload_1 = NULL;
    bytecode_info* invokevirtual = NULL;
    bytecode_info* getfield_s_this = NULL;
    bytecode_info* sconst_1 = NULL;
    bytecode_info* sadd = NULL;
    bytecode_info* putfield_s_this = NULL;

    aload_1 = create_aload_1_bytecode();
    if(aload_1 == NULL)
        return -1;

    invokevirtual = create_invokevirtual_bytecode(resetResume_ref);
    if(invokevirtual == NULL)
        return -1;

    getfield_s_this = create_getfield_T_this_bytecode(currentField_field);
    if(getfield_s_this == NULL)
        return -1;

    sconst_1 = create_push_short_value_bytecode(1);
    if(sconst_1 == NULL)
        return -1;

    sadd = create_sadd_bytecode();
    if(sadd == NULL)
        return -1;

    putfield_s_this = create_putfield_T_this_bytecode(currentField_field);
    if(putfield_s_this == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 6));
    if(tmp == NULL) {
        perror("add_resetResume_bytecodes");
        return -1;
    }
    method->bytecodes = tmp;

    method->bytecodes[method->bytecodes_count++] = aload_1;
    method->bytecodes[method->bytecodes_count++] = invokevirtual;
    method->bytecodes[method->bytecodes_count++] = getfield_s_this;
    method->bytecodes[method->bytecodes_count++] = sconst_1;
    method->bytecodes[method->bytecodes_count++] = sadd;
    method->bytecodes[method->bytecodes_count++] = putfield_s_this;

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to serialize every instance
 * primitive and array fields of the given class and call super.serialize if
 * available.
 *
 * \verbatim
// Generated code

return;
\endverbatim
 * or
 * \verbatim
// Generated code

out.write(someField);
// or (depend of the field)
out.write(someOtherField, (short)0);
return;
\endverbatim
 * or
 *\verbatim
// Generated code

switch(currentField) {
    case 0:
        super.serialize(out);
        out.resetResume();
        ++currentField;
    case 1:
        out.write(someField);
        out.resetResume();
        ++currentField;
    case 2:
        out.write(someOtherField, (short)0);
}
currentField = (short)0;
return;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the Synchronizable interface is added.
 * \param synclib Information on the synclib package.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_serialize_bytecodes(method_info* method, class_info* class, synclib_info* synclib) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(method == NULL)
        return 0;

    if((tweak->nb_fields == 0) && (tweak->nb_array_fields == 0) && (tweak->serialize_supercall_ref == NULL)) {
        bytecode_info* vreturn = NULL;
        vreturn = create_return_bytecode();
        if(vreturn == NULL)
            return -1;

        method->bytecodes_count = 1;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info) * method->bytecodes_count);
        if(method->bytecodes == NULL) {
            perror("add_serialize_bytecodes");
            return -1;
        }
        method->bytecodes[0] = vreturn;

        method->nargs = 2;
    } else if((tweak->serialize_supercall_ref == NULL) && ((tweak->nb_fields + tweak->nb_array_fields) == 1)) {
        u2 u2Index = 0;
        bytecode_info* aload_1 = NULL;
        bytecode_info* getfield_T_this = NULL;
        bytecode_info* sconst_0 = NULL;
        bytecode_info* invokevirtual = NULL;
        bytecode_info* vreturn = NULL;

        aload_1 = create_aload_1_bytecode();
        if(aload_1 == NULL)
            return -1;

        for(; u2Index < class->fields_count; ++u2Index)
            if(!(class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) && !(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_REF))
                break;

        getfield_T_this = create_getfield_T_this_bytecode(class->fields[u2Index]->this_field);
        if(getfield_T_this == NULL)
            return -1;

        if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_ARRAY) {
            sconst_0 = create_push_short_value_bytecode(0);
            if(sconst_0 == NULL)
                return -1;
        }

        if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_ARRAY) {
            if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BOOLEAN)
                invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_boolean_array_ref);
            else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BYTE)
                invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_byte_array_ref);
            else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_SHORT)
                invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_short_array_ref);
            else
                invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_int_array_ref);
        } else {
            if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BOOLEAN)
                invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_boolean_ref);
            else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BYTE)
                invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_byte_ref);
            else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_SHORT)
                invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_short_ref);
            else
                invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_int_ref);
        }

        if(invokevirtual == NULL)
            return -1;

        vreturn = create_return_bytecode();
        if(vreturn == NULL)
            return -1;

        method->bytecodes_count = (class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_ARRAY) ? 5 : 4;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info) * method->bytecodes_count);
        if(method->bytecodes == NULL) {
            perror("add_serialize_bytecodes");
            return -1;
        }
        if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_ARRAY) {
            method->bytecodes[0] = aload_1;
            method->bytecodes[1] = getfield_T_this;
            method->bytecodes[2] = sconst_0;
            method->bytecodes[3] = invokevirtual;
            method->bytecodes[4] = vreturn;
        } else {
            method->bytecodes[0] = aload_1;
            method->bytecodes[1] = getfield_T_this;
            method->bytecodes[2] = invokevirtual;
            method->bytecodes[3] = vreturn;
        }

        method->nargs = 2;
        if(class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_INT))
            method->max_stack = 3;
        else
            method->max_stack = 2;
    } else {
        u2 u2Index = 0;
        int16_t crt_boolean_array_field_id = tweak->nb_boolean_array_superclass_fields;
        int16_t crt_byte_array_field_id = tweak->nb_byte_array_superclass_fields;
        int16_t crt_short_array_field_id = tweak->nb_short_array_superclass_fields;
        int16_t crt_int_array_field_id = tweak->nb_int_array_superclass_fields;
        char was_int_serialized = 0;
        bytecode_info** tmp = NULL;
        bytecode_info* getfield_s_this = NULL;

        bytecode_info* stableswitch = NULL;
        u2 stableswitch_index = 0;
        bytecode_info** branches = NULL;
        u2 crt_case = 0;
        u2 nb_cases = 0;

        bytecode_info* sconst_0 = NULL;
        bytecode_info* putfield_s_this = NULL;
        bytecode_info* vreturn = NULL;

        method->bytecodes_count = 0;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * 2);
        if(method->bytecodes == NULL) {
            perror("add_serialize_bytecodes");
            return -1;
        }

        getfield_s_this = create_getfield_T_this_bytecode(tweak->currentField_field);
        if(getfield_s_this == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = getfield_s_this;

        nb_cases = tweak->nb_fields + tweak->nb_array_fields + (tweak->serialize_supercall_ref ? 1 : 0);
        branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_cases);
        if(branches == NULL) {
            perror("add_serialize_bytecodes");
            return -1;
        }

        stableswitch_index = method->bytecodes_count++;

        if(tweak->serialize_supercall_ref) {
            bytecode_info* aload_0 = NULL;
            bytecode_info* aload_1 = NULL;
            bytecode_info* invokesp = NULL;

            aload_0 = create_aload_0_bytecode();
            if(aload_0 == NULL)
                return -1;

            aload_1 = create_aload_1_bytecode();
            if(aload_1 == NULL)
                return -1;

            invokesp = create_invokespecial_bytecode(tweak->serialize_supercall_ref);
            if(invokesp == NULL)
                return -1;

            branches[crt_case++] = aload_0;

            tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 3));
            if(tmp == NULL) {
                perror("add_serialize_bytecodes");
                return -1;
            }
            method->bytecodes = tmp;

            method->bytecodes[method->bytecodes_count++] = aload_0;
            method->bytecodes[method->bytecodes_count++] = aload_1;
            method->bytecodes[method->bytecodes_count++] = invokesp;

            if(add_resetResume_bytecodes(method, tweak->currentField_field, synclib->Output.resetResume_ref) == -1)
                return -1;
        }

        for(; u2Index < class->fields_count; ++u2Index)
                if(!(class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) && !(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_REF)) {
                bytecode_info* aload_1 = NULL;
                bytecode_info* getfield_T_this = NULL;
                bytecode_info* push_value = NULL;
                bytecode_info* invokevirtual = NULL;

                aload_1 = create_aload_1_bytecode();
                if(aload_1 == NULL)
                    return -1;

                getfield_T_this = create_getfield_T_this_bytecode(class->fields[u2Index]->this_field);
                if(getfield_T_this == NULL)
                    return -1;

                if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_ARRAY) {
                    int16_t value = 0;

                    if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BOOLEAN) {
                        value = crt_boolean_array_field_id++;
                        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_boolean_array_ref);
                    } else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BYTE) {
                        value = crt_byte_array_field_id++;
                        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_byte_array_ref);
                    } else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_SHORT) {
                        value = crt_short_array_field_id++;
                        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_short_array_ref);
                    } else {
                        value = crt_int_array_field_id++;
                        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_int_array_ref);
                    }

                    push_value = create_push_short_value_bytecode(value);
                    if(push_value == NULL)
                        return -1;
                } else {

                    if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BOOLEAN)
                        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_boolean_ref);
                    else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BYTE)
                        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_byte_ref);
                    else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_SHORT)
                        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_short_ref);
                    else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_INT) {
                        was_int_serialized = 1;
                        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_int_ref);
                    } else
                        return -1;
                }
 
                if(invokevirtual == NULL)
                    return -1;

                tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 3 + ((class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_ARRAY) ? 1 : 0)));
                if(tmp == NULL) {
                    perror("add_serialize_bytecodes");
                    return -1;
                }
                method->bytecodes = tmp;

                method->bytecodes[method->bytecodes_count++] = aload_1;
                method->bytecodes[method->bytecodes_count++] = getfield_T_this;
                if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_ARRAY)
                    method->bytecodes[method->bytecodes_count++] = push_value;
                method->bytecodes[method->bytecodes_count++] = invokevirtual;

                if((crt_case + 1) < nb_cases)
                    if(add_resetResume_bytecodes(method, tweak->currentField_field, synclib->Output.resetResume_ref) == -1)
                        return -1;

                branches[crt_case++] = aload_1;
            }

        tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 3));
        if(tmp == NULL) {
            perror("add_serialize_bytecodes");
            return -1;
        }
        method->bytecodes = tmp;

        sconst_0 = create_push_short_value_bytecode(0);
        if(sconst_0 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = sconst_0;

        stableswitch = create_stableswitch_bytecode(0, nb_cases - 1, branches, sconst_0);
        if(stableswitch == NULL)
            return -1;

        method->bytecodes[stableswitch_index] = stableswitch;

        putfield_s_this = create_putfield_T_this_bytecode(tweak->currentField_field);
        if(putfield_s_this == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = putfield_s_this;

        vreturn = create_return_bytecode();
        if(vreturn == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = vreturn;

        method->nargs = 2;
        if(was_int_serialized) {    // an int value == two stack slots
            if(tweak->nb_array_fields == 0)
                method->max_stack = 3;
            else
                method->max_stack = 4;
        } else {
            if (tweak->nb_array_fields == 0)
                method->max_stack = 2;
            else
                method->max_stack = 3;
        }
    }

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to merge every instance
 * primitive and array fields of the given class and call super.merge if
 * available.
 *
 * \verbatim
// Generated code

return;
\endverbatim
 * or
 * \verbatim
// Generated code

someField = in.read(someField);
return;
\endverbatim
 * or
 * \verbatim
// Generated code

switch(currentField) {
    case 0:
        super.merge(in);
        in.resetResume();
        ++currentField;
    case 1:
        someField = in.read(someField);
        in.resetResume();
        ++currentField;
    case 2:
        someOtherField = in.checkArray(someOtherField);
        in.resetResume();
        ++currentField;
    case 3:
        in.read(someOtherField);
}
currentField = (short)0;
return;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the Synchronizable interface is added.
 * \param synclib Information on the synclib package.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_merge_bytecodes(method_info* method, class_info* class, synclib_info* synclib) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(method == NULL)
        return 0;

    if((tweak->nb_fields == 0) && (tweak->nb_array_fields == 0) && (tweak->merge_supercall_ref == NULL)) {
        bytecode_info* vreturn = NULL;
        vreturn = create_return_bytecode();
        if(vreturn == NULL)
            return -1;

        method->bytecodes_count = 1;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info) * method->bytecodes_count);
        if(method->bytecodes == NULL) {
            perror("add_merge_bytecodes");
            return -1;
        }
        method->bytecodes[0] = vreturn;

        method->nargs = 2;
    } else if((tweak->nb_fields == 1) && (tweak->nb_array_fields == 0) && (tweak->merge_supercall_ref == NULL)) {
        u2 u2Index = 0;
        bytecode_info* aload_1 = NULL;
        bytecode_info* getfield_T_this = NULL;
        bytecode_info* invokevirtual = NULL;
        bytecode_info* putfield_T_this = NULL;
        bytecode_info* vreturn = NULL;

        aload_1 = create_aload_1_bytecode();
        if(aload_1 == NULL)
            return -1;

        for(; u2Index < class->fields_count; ++u2Index)
            if(!(class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) && !(class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_REF|TYPE_DESCRIPTOR_ARRAY)))
                break;

        getfield_T_this = create_getfield_T_this_bytecode(class->fields[u2Index]->this_field);
        if(getfield_T_this == NULL)
            return -1;

        if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BOOLEAN)
            invokevirtual = create_invokevirtual_bytecode(synclib->Input.read_boolean_ref);
        else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BYTE)
            invokevirtual = create_invokevirtual_bytecode(synclib->Input.read_byte_ref);
        else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_SHORT)
            invokevirtual = create_invokevirtual_bytecode(synclib->Input.read_short_ref);
        else
            invokevirtual = create_invokevirtual_bytecode(synclib->Input.read_int_ref);

        putfield_T_this = create_putfield_T_this_bytecode(class->fields[u2Index]->this_field);
        if(putfield_T_this == NULL)
            return -1;

        vreturn = create_return_bytecode();
        if(vreturn == NULL)
            return -1;

        method->bytecodes_count = 5;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info) * method->bytecodes_count);
        if(method->bytecodes == NULL) {
            perror("add_merge_bytecodes");
            return -1;
        }
        method->bytecodes[0] = aload_1;
        method->bytecodes[1] = getfield_T_this;
        method->bytecodes[2] = invokevirtual;
        method->bytecodes[3] = putfield_T_this;
        method->bytecodes[4] = vreturn;

        method->nargs = 2;
        if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_INT)
            method->max_stack = 3;
        else
            method->max_stack = 2;
    } else {
        u2 u2Index = 0;
        char was_int_merged = 0;
        bytecode_info** tmp = NULL;
        bytecode_info* getfield_s_this = NULL;

        bytecode_info* stableswitch = NULL;
        u2 stableswitch_index = 0;
        bytecode_info** branches = NULL;
        u2 crt_case = 0;
        u2 nb_cases = 0;

        bytecode_info* sconst_0 = NULL;
        bytecode_info* putfield_s_this = NULL;
        bytecode_info* vreturn = NULL;

        method->bytecodes_count = 0;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * 2);
        if(method->bytecodes == NULL) {
            perror("add_merge_bytecodes");
            return -1;
        }

        getfield_s_this = create_getfield_T_this_bytecode(tweak->currentField_field);
        if(getfield_s_this == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = getfield_s_this;

        nb_cases = tweak->nb_fields + (tweak->nb_array_fields * 2) + (tweak->merge_supercall_ref ? 1 : 0);
        branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_cases);
        if(branches == NULL) {
            perror("add_merge_bytecodes");
            return -1;
        }

        stableswitch_index = method->bytecodes_count++;

        if(tweak->merge_supercall_ref) {
            bytecode_info* aload_0 = NULL;
            bytecode_info* aload_1 = NULL;
            bytecode_info* invokesp = NULL;

            aload_0 = create_aload_0_bytecode();
            if(aload_0 == NULL)
                return -1;

            aload_1 = create_aload_1_bytecode();
            if(aload_1 == NULL)
                return -1;

            invokesp = create_invokespecial_bytecode(tweak->merge_supercall_ref);
            if(invokesp == NULL)
                return -1;

            branches[crt_case++] = aload_0;

            tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 3));
            if(tmp == NULL) {
                perror("add_merge_bytecodes");
                return -1;
            }
            method->bytecodes = tmp;

            method->bytecodes[method->bytecodes_count++] = aload_0;
            method->bytecodes[method->bytecodes_count++] = aload_1;
            method->bytecodes[method->bytecodes_count++] = invokesp;

            if(add_resetResume_bytecodes(method, tweak->currentField_field, synclib->Input.resetResume_ref) == -1)
                return -1;
        }

        for(; u2Index < class->fields_count; ++u2Index)
            if(!(class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) && !(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_REF)) {
                if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_ARRAY) {
                    bytecode_info* aload_1_1 = NULL;
                    bytecode_info* getfield_a_this_1 = NULL;
                    bytecode_info* invokevirtual_1 = NULL;
                    bytecode_info* putfield_a_this = NULL;
                    bytecode_info* aload_1_2 = NULL;
                    bytecode_info* getfield_a_this_2 = NULL;
                    bytecode_info* invokevirtual_2 = NULL;

                    aload_1_1 = create_aload_1_bytecode();
                    if(aload_1_1 == NULL)
                        return -1;

                    getfield_a_this_1 = create_getfield_T_this_bytecode(class->fields[u2Index]->this_field);
                    if(getfield_a_this_1 == NULL)
                        return -1;

                    if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BOOLEAN)
                        invokevirtual_1 = create_invokevirtual_bytecode(synclib->Input.check_boolean_array_ref);
                    else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BYTE)
                        invokevirtual_1 = create_invokevirtual_bytecode(synclib->Input.check_byte_array_ref);
                    else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_SHORT)
                        invokevirtual_1 = create_invokevirtual_bytecode(synclib->Input.check_short_array_ref);
                    else
                        invokevirtual_1 = create_invokevirtual_bytecode(synclib->Input.check_int_array_ref);

                    if(invokevirtual_1 == NULL)
                        return -1;

                    putfield_a_this = create_putfield_T_this_bytecode(class->fields[u2Index]->this_field);
                    if(putfield_a_this == NULL)
                        return -1;

                    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 4));
                    if(tmp == NULL) {
                        perror("add_merge_bytecodes");
                        return -1;
                    }
                    method->bytecodes = tmp;

                    method->bytecodes[method->bytecodes_count++] = aload_1_1;
                    method->bytecodes[method->bytecodes_count++] = getfield_a_this_1;
                    method->bytecodes[method->bytecodes_count++] = invokevirtual_1;
                    method->bytecodes[method->bytecodes_count++] = putfield_a_this;

                    if(add_resetResume_bytecodes(method, tweak->currentField_field, synclib->Input.resetResume_ref) == -1)
                        return -1;

                    branches[crt_case++] = aload_1_1;

                    aload_1_2 = create_aload_1_bytecode();
                    if(aload_1_2 == NULL)
                        return -1;

                    getfield_a_this_2 = create_getfield_T_this_bytecode(class->fields[u2Index]->this_field);
                    if(getfield_a_this_2 == NULL)
                        return -1;

                    if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BOOLEAN)
                        invokevirtual_2 = create_invokevirtual_bytecode(synclib->Input.read_boolean_array_ref);
                    else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BYTE)
                        invokevirtual_2 = create_invokevirtual_bytecode(synclib->Input.read_byte_array_ref);
                    else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_SHORT)
                        invokevirtual_2 = create_invokevirtual_bytecode(synclib->Input.read_short_array_ref);
                    else
                        invokevirtual_2 = create_invokevirtual_bytecode(synclib->Input.read_int_array_ref);

                    if(invokevirtual_2 == NULL)
                        return -1;

                    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 3));
                    if(tmp == NULL) {
                        perror("add_merge_bytecodes");
                        return -1;
                    }
                    method->bytecodes = tmp;

                    method->bytecodes[method->bytecodes_count++] = aload_1_2;
                    method->bytecodes[method->bytecodes_count++] = getfield_a_this_2;
                    method->bytecodes[method->bytecodes_count++] = invokevirtual_2;

                    if((crt_case + 1) < nb_cases)
                        if(add_resetResume_bytecodes(method, tweak->currentField_field, synclib->Input.resetResume_ref) == -1)
                            return -1;

                    branches[crt_case++] = aload_1_2;
                } else {
                    bytecode_info* aload_1 = NULL;
                    bytecode_info* getfield_T_this = NULL;
                    bytecode_info* invokevirtual = NULL;
                    bytecode_info* putfield_T_this = NULL;

                    aload_1 = create_aload_1_bytecode();
                    if(aload_1 == NULL)
                        return -1;

                    getfield_T_this = create_getfield_T_this_bytecode(class->fields[u2Index]->this_field);
                    if(getfield_T_this == NULL)
                        return -1;

                    if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BOOLEAN)
                        invokevirtual = create_invokevirtual_bytecode(synclib->Input.read_boolean_ref);
                    else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BYTE)
                        invokevirtual = create_invokevirtual_bytecode(synclib->Input.read_byte_ref);
                    else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_SHORT)
                        invokevirtual = create_invokevirtual_bytecode(synclib->Input.read_short_ref);
                    else {
                        was_int_merged = 1;
                        invokevirtual = create_invokevirtual_bytecode(synclib->Input.read_int_ref);
                    }

                    if(invokevirtual == NULL)
                        return -1;

                    putfield_T_this = create_putfield_T_this_bytecode(class->fields[u2Index]->this_field);
                    if(putfield_T_this == NULL)
                        return -1;

                    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 4));
                    if(tmp == NULL) {
                        perror("add_merge_bytecodes");
                        return -1;
                    }
                    method->bytecodes = tmp;

                    method->bytecodes[method->bytecodes_count++] = aload_1;
                    method->bytecodes[method->bytecodes_count++] = getfield_T_this;
                    method->bytecodes[method->bytecodes_count++] = invokevirtual;
                    method->bytecodes[method->bytecodes_count++] = putfield_T_this;

                    if((crt_case + 1) < nb_cases)
                        if(add_resetResume_bytecodes(method, tweak->currentField_field, synclib->Input.resetResume_ref) == -1)
                            return -1;

                    branches[crt_case++] = aload_1;
                }
            }

        tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 3));
        if(tmp == NULL) {
            perror("add_merge_bytecodes");
            return -1;
        }
        method->bytecodes = tmp;

        sconst_0 = create_push_short_value_bytecode(0);
        if(sconst_0 == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = sconst_0;

        stableswitch = create_stableswitch_bytecode(0, nb_cases - 1, branches, sconst_0);
        if(stableswitch == NULL)
            return -1;

        method->bytecodes[stableswitch_index] = stableswitch;

        putfield_s_this = create_putfield_T_this_bytecode(tweak->currentField_field);
        if(putfield_s_this == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = putfield_s_this;

        vreturn = create_return_bytecode();
        if(vreturn == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = vreturn;

        method->nargs = 2;
        if(was_int_merged) {    // an int value == two stack slots
                method->max_stack = 3;
        } else {
                method->max_stack = 2;
        }
    }
    return 0;

}


/**
 * \brief Generate bytecodes for the given method to call superclass init
 * method.
 * 
 * \verbatim
// Generated code

super.SomeSuperClass(in);
if(in == null)
    ISOException.throwIt(ISO7816.SW_UNKNOWN);
return;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the Synchronizable interface is added.
 * \param javacard_framework Information on the javacard.framework package.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_init_bytecodes(method_info* method, class_info* class, javacard_framework_info* javacard_framework) {

    bytecode_info* aload_1_2 = NULL;
    bytecode_info* ifnonnull = NULL;
    bytecode_info* push_value = NULL;
    bytecode_info* invokestatic = NULL;
    bytecode_info* vreturn = NULL;

    if(class->superclass && !(class->superclass->flags & CONSTANT_POOL_IS_EXTERNAL)) {
        bytecode_info* aload_0 = NULL;
        bytecode_info* aload_1_1 = NULL;
        bytecode_info* invokesp = NULL;

        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * 8);
        if(method->bytecodes == NULL) {
            perror("add_init_bytecodes");
            return -1;
        }

        aload_0 = create_aload_0_bytecode();
        if(aload_0 == NULL)
            return -1;

        aload_1_1 = create_aload_1_bytecode();
        if(aload_1_1 == NULL)
            return -1;

        invokesp = create_invokespecial_bytecode(((tweak_info*)class->superclass->internal_class->tweak)->init_method->this_method);
        if(invokesp == NULL)
            return -1;

        method->bytecodes[method->bytecodes_count++] = aload_0;
        method->bytecodes[method->bytecodes_count++] = aload_1_1;
        method->bytecodes[method->bytecodes_count++] = invokesp;
    } else {
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * 5);
        if(method->bytecodes == NULL) {
            perror("add_init_bytecodes");
            return -1;
        }
    }

    aload_1_2 = create_aload_1_bytecode();
    if(aload_1_2 == NULL)
        return -1;

    push_value = create_push_short_value_bytecode(javacard_framework->ISO7816.sw_unknown_value);
    if(push_value == NULL)
        return -1;

    invokestatic = create_invokespecial_bytecode(javacard_framework->ISOException.throwIt_ref);
    if(invokestatic == NULL)
        return -1;

    vreturn = create_return_bytecode();
    if(vreturn == NULL)
        return -1;

    ifnonnull = create_ifnonnull_bytecode(vreturn);
    if(ifnonnull == NULL)
        return -1;

    method->bytecodes[method->bytecodes_count++] = aload_1_2;
    method->bytecodes[method->bytecodes_count++] = ifnonnull;
    method->bytecodes[method->bytecodes_count++] = push_value;
    method->bytecodes[method->bytecodes_count++] = invokestatic;
    method->bytecodes[method->bytecodes_count++] = vreturn;

    method->nargs = 2;
    method->max_stack = 1;

    return 0;

}
