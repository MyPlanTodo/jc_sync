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
 * \file synchronizable_instance_fields_itf_functions.c
 * \brief Add the SynchronizableInstanceFields interface and its methods
 * implementation to a class.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <analyzed_cap_file.h>

#include "tweak_info.h"
#include "synclib_info.h"
#include "generic_tweak_functions.h"
#include "create_bytecode_functions.h"

static int add_methods(analyzed_cap_file* acf, class_info* class, synclib_info* synclib);
static int add_hasCurrentSynchronizableInstance_bytecodes(method_info* method, class_info* class);
static int add_selectNextSynchronizableInstance_bytecodes(method_info* method, class_info* class);
static int add_resetNextSynchronizableInstanceSelect_bytecodes(method_info* method, class_info* class);
static int add_getCurrentSynchronizableInstance_bytecodes(method_info* method, class_info* class);
static int add_setCurrentSynchronizableInstance_bytecodes(method_info* method, class_info* class);



/**
 * \brief Add the SynchronizableInstanceFields to the given class.
 *
 * Should be called only if there is least one instance reference field or if
 * the given class super class implements the SynchronizableInstanceFields
 * interface.
 *
 * The short currentField field should already be added by a call to the
 * add_Synchronizable_interface method.
 *
 * If the given class has at least one instance reference field then the methods
 * defined by the SynchronizableInstanceFields interface are added otherwise
 * they are only inherited.
 *
 * Bytecodes are generated for each added method.
 *
 * \param acf The analyzed CAP file.
 * \param class The class to which the SynchronizableInstanceFields interface is
 * added.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
int add_SynchronizableInstanceFields_interface(analyzed_cap_file* acf, class_info* class, synclib_info* synclib) {

    implemented_interface_info* tmp = NULL;
    tweak_info* tweak = (tweak_info*)class->tweak;

    // add SynchronizableInstanceFields methods to the class
    if(add_methods(acf, class, synclib) == -1)
        return -1;


    // add the interface to the class
    if((tmp = (implemented_interface_info*)realloc(class->interfaces, sizeof(implemented_interface_info) * (class->interfaces_count + 1))) == NULL) {
        perror("add_SynchronizableInstanceFields_interface");
        return -1;
    }

    class->interfaces = tmp;

    class->interfaces[class->interfaces_count].ref = synclib->SynchronizableInstanceFields.interface_ref;

    class->interfaces[class->interfaces_count].count = 14;
    class->interfaces[class->interfaces_count].index = (implemented_method_info*)malloc(sizeof(implemented_method_info) * class->interfaces[class->interfaces_count].count);
    if(class->interfaces[class->interfaces_count].index == NULL) {
        perror("add_SynchronizableInstanceFields_interface");
        return -1;
    }


    // Link the implementation
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.getId_method_token].implementation = tweak->getId_method ? tweak->getId_method : tweak->inherited_getId_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.getId_method_token].method_token = tweak->inherited_getId_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.setId_method_token].implementation = tweak->setId_method ? tweak->setId_method : tweak->inherited_setId_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.setId_method_token].method_token = tweak->inherited_setId_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.isDuplicated_method_token].implementation = tweak->isDuplicated_method ? tweak->isDuplicated_method  : tweak->inherited_isDuplicated_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.isDuplicated_method_token].method_token = tweak->inherited_isDuplicated_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.getPreviousSynchronizableInstance_method_token].implementation = tweak->getPreviousSynchronizableInstance_method ? tweak->getPreviousSynchronizableInstance_method : tweak->inherited_getPreviousSynchronizableInstance_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.getPreviousSynchronizableInstance_method_token].method_token = tweak->inherited_getPreviousSynchronizableInstance_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.setPreviousSynchronizableInstance_method_token].implementation = tweak->setPreviousSynchronizableInstance_method ? tweak->setPreviousSynchronizableInstance_method : tweak->inherited_setPreviousSynchronizableInstance_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.setPreviousSynchronizableInstance_method_token].method_token = tweak->inherited_setPreviousSynchronizableInstance_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.resetPath_method_token].implementation = tweak->resetPath_method ? tweak->resetPath_method : tweak->inherited_resetPath_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.resetPath_method_token].method_token = tweak->inherited_resetPath_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.numberPath_method_token].implementation = tweak->numberPath_method ? tweak->numberPath_method : tweak->inherited_numberPath_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.numberPath_method_token].method_token = tweak->inherited_numberPath_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.serialize_method_token].implementation = tweak->serialize_method ? tweak->serialize_method : tweak->inherited_serialize_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.serialize_method_token].method_token = tweak->inherited_serialize_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.merge_method_token].implementation = tweak->merge_method ? tweak->merge_method : tweak->inherited_merge_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.merge_method_token].method_token = tweak->inherited_merge_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.hasCurrentSynchronizableInstance_method_token].implementation = tweak->hasCurrentSynchronizableInstance_method ? tweak->hasCurrentSynchronizableInstance_method : tweak->inherited_hasCurrentSynchronizableInstance_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.hasCurrentSynchronizableInstance_method_token].method_token = tweak->inherited_hasCurrentSynchronizableInstance_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.selectNextSynchronizableInstance_method_token].implementation = tweak->selectNextSynchronizableInstance_method ? tweak->selectNextSynchronizableInstance_method : tweak->inherited_selectNextSynchronizableInstance_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.selectNextSynchronizableInstance_method_token].method_token = tweak->inherited_selectNextSynchronizableInstance_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.getCurrentSynchronizableInstance_method_token].implementation = tweak->getCurrentSynchronizableInstance_method ? tweak->getCurrentSynchronizableInstance_method : tweak->inherited_getCurrentSynchronizableInstance_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.getCurrentSynchronizableInstance_method_token].method_token = tweak->inherited_getCurrentSynchronizableInstance_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.setCurrentSynchronizableInstance_method_token].implementation = tweak->setCurrentSynchronizableInstance_method ? tweak->setCurrentSynchronizableInstance_method : tweak->inherited_setCurrentSynchronizableInstance_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.setCurrentSynchronizableInstance_method_token].method_token = tweak->inherited_setCurrentSynchronizableInstance_method_token;

    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.resetNextSynchronizableInstanceSelect_method_token].implementation = tweak->resetNextSynchronizableInstanceSelect_method ? tweak->resetNextSynchronizableInstanceSelect_method : tweak->inherited_resetNextSynchronizableInstanceSelect_method;
    class->interfaces[class->interfaces_count].index[synclib->SynchronizableInstanceFields.resetNextSynchronizableInstanceSelect_method_token].method_token = tweak->inherited_resetNextSynchronizableInstanceSelect_method_token;

    ++class->interfaces_count;


    // Generate the bytecodes
    if(add_hasCurrentSynchronizableInstance_bytecodes(tweak->hasCurrentSynchronizableInstance_method, class) == -1)
        return -1;

    if(add_selectNextSynchronizableInstance_bytecodes(tweak->selectNextSynchronizableInstance_method, class) == -1)
        return -1;

    if(add_resetNextSynchronizableInstanceSelect_bytecodes(tweak->resetNextSynchronizableInstanceSelect_method, class) == -1)
        return -1;

    if(add_getCurrentSynchronizableInstance_bytecodes(tweak->getCurrentSynchronizableInstance_method, class) == -1)
        return -1;

    if(add_setCurrentSynchronizableInstance_bytecodes(tweak->setCurrentSynchronizableInstance_method, class) == -1)
        return -1;

    return 0;

}


/**
 * \brief Add supercall to each superclass method implementing the
 * SynchronizableInstanceFields interface.
 * 
 * \param acf The analyzed CAP file.
 * \param class The class to which the SynchronizableInstanceFields interface is
 * added.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_supercalls(analyzed_cap_file* acf, class_info* class, synclib_info* synclib) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(tweak->hasCurrentSynchronizableInstance_method)
        if((tweak->hasCurrentSynchronizableInstance_supercall_ref = add_supercall_to_constant_pool(acf, class, tweak->hasCurrentSynchronizableInstance_method->token, tweak->hasCurrentSynchronizableInstance_method->internal_overrided_method, synclib->SynchronizableInstanceFields.hasCurrentSynchronizableInstance_sig)) == NULL)
            return -1;

    if(tweak->selectNextSynchronizableInstance_method)
        if((tweak->selectNextSynchronizableInstance_supercall_ref = add_supercall_to_constant_pool(acf, class, tweak->selectNextSynchronizableInstance_method->token, tweak->selectNextSynchronizableInstance_method->internal_overrided_method, synclib->SynchronizableInstanceFields.selectNextSynchronizableInstance_sig)) == NULL)
            return -1;

    if(tweak->getCurrentSynchronizableInstance_method)
        if((tweak->getCurrentSynchronizableInstance_supercall_ref = add_supercall_to_constant_pool(acf, class, tweak->getCurrentSynchronizableInstance_method->token, tweak->getCurrentSynchronizableInstance_method->internal_overrided_method, synclib->SynchronizableInstanceFields.getCurrentSynchronizableInstance_sig)) == NULL)
            return -1;

    if(tweak->setCurrentSynchronizableInstance_method)
        if((tweak->setCurrentSynchronizableInstance_supercall_ref = add_supercall_to_constant_pool(acf, class, tweak->setCurrentSynchronizableInstance_method->token, tweak->setCurrentSynchronizableInstance_method->internal_overrided_method, synclib->SynchronizableInstanceFields.setCurrentSynchronizableInstance_sig)) == NULL)
            return -1;

    if(tweak->resetNextSynchronizableInstanceSelect_method)
        if((tweak->resetNextSynchronizableInstanceSelect_supercall_ref = add_supercall_to_constant_pool(acf, class, tweak->resetNextSynchronizableInstanceSelect_method->token, tweak->resetNextSynchronizableInstanceSelect_method->internal_overrided_method, synclib->SynchronizableInstanceFields.resetNextSynchronizableInstanceSelect_sig)) == NULL)
            return -1;

    return 0;

}


/**
 * \brief Add methods to the given class to implement the
 * SynchronizableInstanceFields interface.
 * 
 * \param acf The analyzed CAP file.
 * \param class The class to which the SynchronizableInstanceFields interface is
 * added.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_methods(analyzed_cap_file* acf, class_info* class, synclib_info* synclib) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(tweak->is_superclass_implementing_SynchronizableInstanceFields_interface) {
        if(class->superclass->flags & CONSTANT_POOL_IS_EXTERNAL) {
            if(tweak->nb_ref_fields > 0) {
                if((tweak->hasCurrentSynchronizableInstance_method = add_method(acf, class, synclib->SynchronizableInstanceFields.hasCurrentSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->hasCurrentSynchronizableInstance_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "hasCurrentSynchronizableInstance", "()Z", &(tweak->hasCurrentSynchronizableInstance_method->token)) == -1)
                    return -1;

                if((tweak->setCurrentSynchronizableInstance_method = add_method(acf, class, synclib->SynchronizableInstanceFields.setCurrentSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->setCurrentSynchronizableInstance_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "setCurrentSynchronizableInstance", "(Lsynclib/Synchronizable;)V", &(tweak->setCurrentSynchronizableInstance_method->token)) == -1)
                    return -1;

                if((tweak->getCurrentSynchronizableInstance_method = add_method(acf, class, synclib->SynchronizableInstanceFields.getCurrentSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getCurrentSynchronizableInstance_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getCurrentSynchronizableInstance", "()Lsynclib/Synchronizable;", &(tweak->getCurrentSynchronizableInstance_method->token)) == -1)
                    return -1;

                if((tweak->selectNextSynchronizableInstance_method = add_method(acf, class, synclib->SynchronizableInstanceFields.selectNextSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->selectNextSynchronizableInstance_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "selectNextSynchronizableInstance", "()V", &(tweak->selectNextSynchronizableInstance_method->token)) == -1)
                    return -1;

                if((tweak->resetNextSynchronizableInstanceSelect_method = add_method(acf, class, synclib->SynchronizableInstanceFields.resetNextSynchronizableInstanceSelect_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->resetNextSynchronizableInstanceSelect_method->is_overriding = 1;
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "resetNextSynchronizableInstanceSelect", "()V", &(tweak->resetNextSynchronizableInstanceSelect_method->token)) == -1)
                    return -1;

                if(add_supercalls(acf, class, synclib) == -1)
                    return -1;
            } else {
                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "hasCurrentSynchronizableInstance", "()Z", &(tweak->inherited_hasCurrentSynchronizableInstance_method_token)) == -1)
                    return -1;

                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "setCurrentSynchronizableInstance", "(Lsynclib/Synchronizable;)V", &(tweak->inherited_setCurrentSynchronizableInstance_method_token)) == -1)
                    return -1;

                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "getCurrentSynchronizableInstance", "()Lsynclib/Synchronizable;", &(tweak->inherited_getCurrentSynchronizableInstance_method_token)) == -1)
                    return -1;

                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "selectNextSynchronizableInstance", "()V", &(tweak->inherited_selectNextSynchronizableInstance_method_token)) == -1)
                    return -1;

                if(get_method_token_from_export_file(class->superclass->external_package->ef, class->superclass->external_class_token, "resetNextSynchronizableInstanceSelect", "()V", &(tweak->inherited_resetNextSynchronizableInstanceSelect_method_token)) == -1)
                    return -1;
            }
        } else {
            implemented_method_info* implemented_methods = tweak->SynchronizableInstanceFields_methods;

            if(tweak->nb_ref_fields > 0) {
                if((tweak->hasCurrentSynchronizableInstance_method = add_method(acf, class, synclib->SynchronizableInstanceFields.hasCurrentSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->hasCurrentSynchronizableInstance_method->is_overriding = 1;
                tweak->hasCurrentSynchronizableInstance_method->internal_overrided_method = implemented_methods[synclib->SynchronizableInstanceFields.hasCurrentSynchronizableInstance_method_token].implementation;

                if((tweak->selectNextSynchronizableInstance_method = add_method(acf, class, synclib->SynchronizableInstanceFields.selectNextSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->selectNextSynchronizableInstance_method->is_overriding = 1;
                tweak->selectNextSynchronizableInstance_method->internal_overrided_method = implemented_methods[synclib->SynchronizableInstanceFields.selectNextSynchronizableInstance_method_token].implementation;

                if((tweak->getCurrentSynchronizableInstance_method = add_method(acf, class, synclib->SynchronizableInstanceFields.getCurrentSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->getCurrentSynchronizableInstance_method->is_overriding = 1;
                tweak->getCurrentSynchronizableInstance_method->internal_overrided_method = implemented_methods[synclib->SynchronizableInstanceFields.getCurrentSynchronizableInstance_method_token].implementation;

                if((tweak->setCurrentSynchronizableInstance_method = add_method(acf, class, synclib->SynchronizableInstanceFields.setCurrentSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->setCurrentSynchronizableInstance_method->is_overriding = 1;
                tweak->setCurrentSynchronizableInstance_method->internal_overrided_method = implemented_methods[synclib->SynchronizableInstanceFields.setCurrentSynchronizableInstance_method_token].implementation;

                if((tweak->resetNextSynchronizableInstanceSelect_method = add_method(acf, class, synclib->SynchronizableInstanceFields.resetNextSynchronizableInstanceSelect_sig, METHOD_PUBLIC)) == NULL)
                    return -1;

                tweak->resetNextSynchronizableInstanceSelect_method->is_overriding = 1;
                tweak->resetNextSynchronizableInstanceSelect_method->internal_overrided_method = implemented_methods[synclib->SynchronizableInstanceFields.resetNextSynchronizableInstanceSelect_method_token].implementation;

                if(add_supercalls(acf, class, synclib) == -1)
                    return -1;
            } else {
                tweak->inherited_hasCurrentSynchronizableInstance_method = implemented_methods[synclib->SynchronizableInstanceFields.hasCurrentSynchronizableInstance_method_token].implementation;

                tweak->inherited_selectNextSynchronizableInstance_method = implemented_methods[synclib->SynchronizableInstanceFields.selectNextSynchronizableInstance_method_token].implementation;

                tweak->inherited_getCurrentSynchronizableInstance_method = implemented_methods[synclib->SynchronizableInstanceFields.getCurrentSynchronizableInstance_method_token].implementation;

                tweak->inherited_setCurrentSynchronizableInstance_method = implemented_methods[synclib->SynchronizableInstanceFields.setCurrentSynchronizableInstance_method_token].implementation;

                tweak->inherited_resetNextSynchronizableInstanceSelect_method = implemented_methods[synclib->SynchronizableInstanceFields.resetNextSynchronizableInstanceSelect_method_token].implementation;
            }
        }
    } else {
        if((tweak->hasCurrentSynchronizableInstance_method = add_method(acf, class, synclib->SynchronizableInstanceFields.hasCurrentSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->selectNextSynchronizableInstance_method = add_method(acf, class, synclib->SynchronizableInstanceFields.selectNextSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->getCurrentSynchronizableInstance_method = add_method(acf, class, synclib->SynchronizableInstanceFields.getCurrentSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->setCurrentSynchronizableInstance_method = add_method(acf, class, synclib->SynchronizableInstanceFields.setCurrentSynchronizableInstance_sig, METHOD_PUBLIC)) == NULL)
            return -1;

        if((tweak->resetNextSynchronizableInstanceSelect_method = add_method(acf, class, synclib->SynchronizableInstanceFields.resetNextSynchronizableInstanceSelect_sig, METHOD_PUBLIC)) == NULL)
            return -1;
    }

    return 0;

}


/**
 * Generate bytecodes for the given method to return the state of the instances
 * iterator.
 *
 * \verbatim
// Generated code

return currentField < (short)nb_ref_fields;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the SynchronizableInstanceFields interface is
 * added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_hasCurrentSynchronizableInstance_bytecodes(method_info* method, class_info* class) {

    tweak_info* tweak = (tweak_info*)class->tweak;
    int16_t nb_ref_fields = tweak->nb_ref_fields + (tweak->hasCurrentSynchronizableInstance_supercall_ref ? 1 : 0);

    bytecode_info* getfield_s_this = NULL;
    bytecode_info* push_value = NULL;
    bytecode_info* if_scmpge = NULL;
    bytecode_info* sconst_1 = NULL;
    bytecode_info* sreturn_1 = NULL;
    bytecode_info* sconst_0 = NULL;
    bytecode_info* sreturn_2 = NULL;

    if(method == NULL)
        return 0;

    getfield_s_this = create_getfield_T_this_bytecode(tweak->currentField_field);
    if(getfield_s_this == NULL)
        return -1;

    push_value = create_push_short_value_bytecode(nb_ref_fields);
    if(push_value == NULL)
        return -1;

    sconst_1 = create_push_short_value_bytecode(1);
    if(sconst_1 == NULL)
        return -1;

    sreturn_1 = create_sreturn_bytecode();
    if(sreturn_1 == NULL)
        return -1;

    sconst_0 = create_push_short_value_bytecode(0);
    if(sconst_0 == NULL)
        return -1;

    if_scmpge = create_if_scmpge_bytecode(sconst_0);
    if(if_scmpge == NULL)
        return -1;

    sreturn_2 = create_sreturn_bytecode();
    if(sreturn_2 == NULL)
        return -1;

    method->bytecodes_count = 7;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info) * method->bytecodes_count);
    if(method->bytecodes == NULL) {
        perror("add_hasCurrentSynchronizableInstance_bytecodes");
        return -1;
    }

    method->bytecodes[0] = getfield_s_this;
    method->bytecodes[1] = push_value;
    method->bytecodes[2] = if_scmpge;
    method->bytecodes[3] = sconst_1;
    method->bytecodes[4] = sreturn_1;
    method->bytecodes[5] = sconst_0;
    method->bytecodes[6] = sreturn_2;

    method->nargs = 1;
    method->max_stack = 2;

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to move the instances iterator
 * on the next instance.
 * 
 * If the given class superclass implements the SynchronizableInstanceFields, a
 * call to the super.selectNextSynchronizableInstance method is added.
 *
 * \verbatim
// Generated code

if(currentField == (short)0) {
    super.selectNextSynchronizableInstance();
    if(!super.hasCurrentSynchronizableInstance())
        ++currentField;
    return;
}
if(currentfield < (short)nb_ref_fields)
    ++currentField;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the SynchronizableInstanceFields interface is
 * added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_selectNextSynchronizableInstance_bytecodes(method_info* method, class_info* class) {

    tweak_info* tweak = (tweak_info*)class->tweak;
    int16_t nb_ref_fields = tweak->nb_ref_fields + (tweak->hasCurrentSynchronizableInstance_supercall_ref ? 1 : 0);

    bytecode_info* getfield_s_this_1 = NULL;
    bytecode_info* ifne_1 = NULL;
    bytecode_info* aload_0_1 = NULL;
    bytecode_info* invokesp_1 = NULL;
    bytecode_info* aload_0_2 = NULL;
    bytecode_info* invokesp_2 = NULL;
    bytecode_info* ifne_2 = NULL;
    bytecode_info* getfield_s_this_2 = NULL;
    bytecode_info* sconst_1_1 = NULL;
    bytecode_info* sadd_1 = NULL;
    bytecode_info* putfield_s_this_1 = NULL;
    bytecode_info* return_1 = NULL;
    bytecode_info* getfield_s_this_3 = NULL;
    bytecode_info* push_value = NULL;
    bytecode_info* if_scmpge = NULL;
    bytecode_info* getfield_s_this_4 = NULL;
    bytecode_info* sconst_1_2 = NULL;
    bytecode_info* sadd_2 = NULL;
    bytecode_info* putfield_s_this_2 = NULL;
    bytecode_info* return_2 = NULL;

    if(method == NULL)
        return 0;

    if(tweak->hasCurrentSynchronizableInstance_supercall_ref) {
        getfield_s_this_1 = create_getfield_T_this_bytecode(tweak->currentField_field);
        if(getfield_s_this_1 == NULL)
            return -1;

        aload_0_1 = create_aload_0_bytecode();
        if(aload_0_1 == NULL)
            return -1;

        invokesp_1 = create_invokespecial_bytecode(tweak->selectNextSynchronizableInstance_supercall_ref);
        if(invokesp_1 == NULL)
            return -1;

        aload_0_2 = create_aload_0_bytecode();
        if(aload_0_2 == NULL)
            return -1;

        invokesp_2 = create_invokespecial_bytecode(tweak->hasCurrentSynchronizableInstance_supercall_ref);
        if(invokesp_2 == NULL)
            return -1;

        getfield_s_this_2 = create_getfield_T_this_bytecode(tweak->currentField_field);
        if(getfield_s_this_2 == NULL)
            return -1;

        sconst_1_1 = create_push_short_value_bytecode(1);
        if(sconst_1_1 == NULL)
            return -1;

        sadd_1 = create_sadd_bytecode();
        if(sadd_1 == NULL)
            return -1;

        putfield_s_this_1 = create_putfield_T_this_bytecode(tweak->currentField_field);
        if(putfield_s_this_1 == NULL)
            return -1;

        return_1 = create_return_bytecode();
        if(return_1 == NULL)
            return -1;
    }

    getfield_s_this_3 = create_getfield_T_this_bytecode(tweak->currentField_field);
    if(getfield_s_this_3 == NULL)
        return -1;

    push_value = create_push_short_value_bytecode(nb_ref_fields);
    if(push_value == NULL)
        return -1;

    getfield_s_this_4 = create_getfield_T_this_bytecode(tweak->currentField_field);
    if(getfield_s_this_4 == NULL)
        return -1;

    sconst_1_2 = create_push_short_value_bytecode(1);
    if(sconst_1_2 == NULL)
        return -1;

    sadd_2 = create_sadd_bytecode();
    if(sadd_2 == NULL)
        return -1;

    putfield_s_this_2 = create_putfield_T_this_bytecode(tweak->currentField_field);
    if(putfield_s_this_2 == NULL)
        return -1;

    return_2 = create_return_bytecode();
    if(return_2 == NULL)
        return -1;

    if(tweak->hasCurrentSynchronizableInstance_supercall_ref) {
        method->bytecodes_count = 20;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
        if(method->bytecodes == NULL) {
            perror("add_hasCurrentSynchronizableInstance_bytecodes");
            return -1;
        }

        ifne_1 = create_ifne_bytecode(getfield_s_this_3);
        if(ifne_1 == NULL)
            return -1;

        ifne_2 = create_ifne_bytecode(return_1);
        if(ifne_2 == NULL)
            return -1;

        if_scmpge = create_if_scmpge_bytecode(return_2);
        if(if_scmpge == NULL)
            return -1;

        method->bytecodes[0] = getfield_s_this_1;
        method->bytecodes[1] = ifne_1;
        method->bytecodes[2] = aload_0_1;
        method->bytecodes[3] = invokesp_1;
        method->bytecodes[4] = aload_0_2;
        method->bytecodes[5] = invokesp_2;
        method->bytecodes[6] = ifne_2;
        method->bytecodes[7] = getfield_s_this_2;
        method->bytecodes[8] = sconst_1_1;
        method->bytecodes[9] = sadd_1;
        method->bytecodes[10] = putfield_s_this_1;
        method->bytecodes[11] = return_1;
        method->bytecodes[12] = getfield_s_this_3;
        method->bytecodes[13] = push_value;
        method->bytecodes[14] = if_scmpge;
        method->bytecodes[15] = getfield_s_this_4;
        method->bytecodes[16] = sconst_1_2;
        method->bytecodes[17] = sadd_2;
        method->bytecodes[18] = putfield_s_this_2;
        method->bytecodes[19] = return_2;
    } else {
        method->bytecodes_count = 8;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
        if(method->bytecodes == NULL) {
            perror("add_hasCurrentSynchronizableInstance_bytecodes");
            return -1;
        }

        if_scmpge = create_if_scmpge_bytecode(return_2);
        if(if_scmpge == NULL)
            return -1;

        method->bytecodes[0] = getfield_s_this_3;
        method->bytecodes[1] = push_value;
        method->bytecodes[2] = if_scmpge;
        method->bytecodes[3] = getfield_s_this_4;
        method->bytecodes[4] = sconst_1_2;
        method->bytecodes[5] = sadd_2;
        method->bytecodes[6] = putfield_s_this_2;
        method->bytecodes[7] = return_2;
    }

    method->nargs = 1;
    method->max_stack = 2;

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to reset the instances iterator.
 *
 * If the given class super class implements the SynchronizableInstanceFields
 * interface, a call to super.resetNextSynchronizableInstanceSelect is added.
 *
 * \verbatim
// Generated code

super.resetNextSynchronizableInstanceSelect_supercall_ref;
currentField = (short)0;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the SynchronizableInstanceFields interface is
 * added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_resetNextSynchronizableInstanceSelect_bytecodes(method_info* method, class_info* class) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    bytecode_info* aload_0 = NULL;
    bytecode_info* invokesp = NULL;
    bytecode_info* sconst_0 = NULL;
    bytecode_info* putfield_s_this = NULL;
    bytecode_info* vreturn = NULL;

    if(method == NULL)
        return 0;

    if(tweak->resetNextSynchronizableInstanceSelect_supercall_ref) {
        aload_0 = create_aload_0_bytecode();
        if(aload_0 == NULL)
            return -1;

        invokesp = create_invokespecial_bytecode(tweak->resetNextSynchronizableInstanceSelect_supercall_ref);
        if(invokesp == NULL)
            return -1;
    }

    sconst_0 = create_push_short_value_bytecode(0);
    if(sconst_0 == NULL)
        return -1;

    putfield_s_this = create_putfield_T_this_bytecode(tweak->currentField_field);
    if(putfield_s_this == NULL)
        return -1;

    vreturn = create_return_bytecode();
    if(vreturn == NULL)
        return -1;

    if(tweak->resetNextSynchronizableInstanceSelect_supercall_ref) {
        method->bytecodes_count = 5;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
        if(method->bytecodes == NULL) {
            perror("add_resetNextSynchronizableInstanceSelect_bytecodes");
            return -1;
        }

        method->bytecodes[0] = aload_0;
        method->bytecodes[1] = invokesp;
        method->bytecodes[2] = sconst_0;
        method->bytecodes[3] = putfield_s_this;
        method->bytecodes[4] = vreturn;
    } else {
        method->bytecodes_count = 3;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
        if(method->bytecodes == NULL) {
            perror("add_resetNextSynchronizableInstanceSelect_bytecodes");
            return -1;
        }

        method->bytecodes[0] = sconst_0;
        method->bytecodes[1] = putfield_s_this;
        method->bytecodes[2] = vreturn;
    }

    method->nargs = 1;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to return the currently
 * selected instance.
 *
 * If the given class super class implements the SynchronizableInstanceFields
 * interface, a call to the super.getCurrentSynchronizableInstance method is
 * added.
 *
 * \verbatim
// Generated code

switch(currentField) {
    case (short)0:
        return super.getCurrentSynchronizableInstance();
    case (short)1:
        return someField;
}
return null;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the SynchronizableInstanceFields interface is
 * added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getCurrentSynchronizableInstance_bytecodes(method_info* method, class_info* class) {

    tweak_info* tweak = (tweak_info*)class->tweak;
    u2 nb_ref_fields = tweak->nb_ref_fields + (tweak->getCurrentSynchronizableInstance_supercall_ref ? 1 : 0);
    u2 u2Index = 0;

    bytecode_info** tmp = NULL;
    bytecode_info* getfield_s_this = NULL;

    bytecode_info* stableswitch = NULL;
    u2 stableswitch_index = 0;
    bytecode_info** branches = NULL;
    u2 crt_case = 0;

    bytecode_info* aconst_null = NULL;
    bytecode_info* areturn_2 = NULL;

    if(method == NULL)
        return 0;

    method->bytecodes_count = 0;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * 2);
    if(method->bytecodes == NULL) {
        perror("add_getCurrentSynchronizableInstance_bytecodes");
        return -1;
    }

    getfield_s_this = create_getfield_T_this_bytecode(tweak->currentField_field);
    if(getfield_s_this == NULL)
        return -1;

    method->bytecodes[method->bytecodes_count++] = getfield_s_this;

    branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_ref_fields);
    if(branches == NULL) {
        perror("add_getCurrentSynchronizableInstance_bytecodes");
        return -1;
    }

    stableswitch_index = method->bytecodes_count++;

    if(tweak->getCurrentSynchronizableInstance_supercall_ref) {
        bytecode_info* aload_0 = NULL;
        bytecode_info* invokesp = NULL;
        bytecode_info* areturn_1 = NULL;

        aload_0 = create_aload_0_bytecode();
        if(aload_0 == NULL)
            return -1;

        invokesp = create_invokespecial_bytecode(tweak->getCurrentSynchronizableInstance_supercall_ref);
        if(invokesp == NULL)
            return -1;

        areturn_1 = create_areturn_bytecode();
        if(areturn_1 == NULL)
            return -1;

        tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 3));
        if(tmp == NULL) {
            perror("add_getCurrentSynchronizableInstance_bytecodes");
            return -1;
        }
        method->bytecodes = tmp;
        method->bytecodes[method->bytecodes_count++] = aload_0;
        method->bytecodes[method->bytecodes_count++] = invokesp;
        method->bytecodes[method->bytecodes_count++] = areturn_1;
        branches[crt_case++] = aload_0;
    }

    for(; u2Index < class->fields_count; ++u2Index)
        if(!(class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) && ((class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_REF|TYPE_DESCRIPTOR_ARRAY)) == TYPE_DESCRIPTOR_REF)) {
            bytecode_info* getfield_a_this = NULL;
            bytecode_info* areturn_3 = NULL;

            getfield_a_this = create_getfield_T_this_bytecode(class->fields[u2Index]->this_field);
            if(getfield_a_this == NULL)
                return -1;

            areturn_3 = create_areturn_bytecode();
            if(areturn_3 == NULL)
                return -1;

            tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 2));
            if(tmp == NULL) {
                perror("add_getCurrentSynchronizableInstance_bytecodes");
                return -1;
            }
            method->bytecodes = tmp;
            method->bytecodes[method->bytecodes_count++] = getfield_a_this;
            method->bytecodes[method->bytecodes_count++] = areturn_3;
            branches[crt_case++] = getfield_a_this;
        }

    aconst_null = create_aconst_null_bytecode();
    if(aconst_null == NULL)
        return -1;

    stableswitch = create_stableswitch_bytecode(0, nb_ref_fields - 1, branches, aconst_null);
    if(stableswitch == NULL)
        return -1;

    areturn_2 = create_areturn_bytecode();
    if(areturn_2 == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 2));
    if(tmp == NULL) {
        perror("add_getCurrentSynchronizableInstance_bytecodes");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[stableswitch_index] = stableswitch;
    method->bytecodes[method->bytecodes_count++] = aconst_null;
    method->bytecodes[method->bytecodes_count++] = areturn_2;

    method->nargs = 1;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Generate bytecodes for the given method to set the currently selected
 * method with the Synchronizable parameter.
 *
 * If the given class super class implements the SynchronizableInstanceFields
 * interface, a call to the super.setSynchronizableInstance method is added.
 *
 * \verbatim
// Generated code

switch(currentField) {
    case (short)0:
        super.setCurrentSynchronizableInstance(someField);
        return;
    case (short)1:
        this.someField = (SomeField)someField;
        return;
}
return;
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param class The class to which the SynchronizableInstanceFields interface is
 * added.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_setCurrentSynchronizableInstance_bytecodes(method_info* method, class_info* class) {

    tweak_info* tweak = (tweak_info*)class->tweak;
    u2 nb_ref_fields = tweak->nb_ref_fields + (tweak->setCurrentSynchronizableInstance_supercall_ref ? 1 : 0);
    u2 u2Index = 0;

    bytecode_info** tmp = NULL;
    bytecode_info* getfield_s_this = NULL;
    bytecode_info* stableswitch = NULL;
    u2 stableswitch_index = 0;
    bytecode_info** branches = NULL;
    u2 crt_case = 0;
    bytecode_info* vreturn_2 = NULL;

    if(method == NULL)
        return 0;

    method->bytecodes_count = 0;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * 2);
    if(method->bytecodes == NULL) {
        perror("add_setCurrentSynchronizableInstance_bytecodes");
        return -1;
    }

    getfield_s_this = create_getfield_T_this_bytecode(tweak->currentField_field);
    if(getfield_s_this == NULL)
        return -1;

    method->bytecodes[method->bytecodes_count++] = getfield_s_this;

    branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_ref_fields);
    if(branches == NULL) {
        perror("add_setCurrentSynchronizableInstance_bytecodes");
        return -1;
    }

    stableswitch_index = method->bytecodes_count++;

    if(tweak->setCurrentSynchronizableInstance_supercall_ref) {
        bytecode_info* aload_0 = NULL;
        bytecode_info* aload_1 = NULL;
        bytecode_info* invokesp = NULL;
        bytecode_info* vreturn_1 = NULL;

        aload_0 = create_aload_0_bytecode();
        if(aload_0 == NULL)
            return -1;

        aload_1 = create_aload_1_bytecode();
        if(aload_1 == NULL)
            return -1;

        invokesp = create_invokespecial_bytecode(tweak->setCurrentSynchronizableInstance_supercall_ref);
        if(invokesp == NULL)
            return -1;

        vreturn_1 = create_return_bytecode();
        if(vreturn_1 == NULL)
            return -1;

        tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 4));
        if(tmp == NULL) {
            perror("add_setCurrentSynchronizableInstance_bytecodes");
            return -1;
        }
        method->bytecodes = tmp;
        method->bytecodes[method->bytecodes_count++] = aload_0;
        method->bytecodes[method->bytecodes_count++] = aload_1;
        method->bytecodes[method->bytecodes_count++] = invokesp;
        method->bytecodes[method->bytecodes_count++] = vreturn_1;
        branches[crt_case++] = aload_0;
    }

    for(; u2Index < class->fields_count; ++u2Index)
        if(!(class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) && ((class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_REF|TYPE_DESCRIPTOR_ARRAY)) == TYPE_DESCRIPTOR_REF)) {
            bytecode_info* aload_1 = NULL;
            bytecode_info* checkcast = NULL;
            bytecode_info* putfield_a_this = NULL;
            bytecode_info* vreturn_3 = NULL;

            aload_1 = create_aload_1_bytecode();
            if(aload_1 == NULL)
                return -1;

            checkcast = create_checkcast_bytecode(class->fields[u2Index]->type->types->ref);
            if(checkcast == NULL)
                return -1;

            putfield_a_this = create_putfield_T_this_bytecode(class->fields[u2Index]->this_field);
            if(putfield_a_this == NULL)
                return -1;

            if((crt_case + 1) < nb_ref_fields ) {
                vreturn_3 = create_return_bytecode();
                if(vreturn_3 == NULL)
                    return -1;
            }

            tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + (((crt_case + 1) < nb_ref_fields) ? 4 : 3)));
            if(tmp == NULL) {
                perror("add_setCurrentSynchronizableInstance_bytecodes");
                return -1;
            }
            method->bytecodes = tmp;
            method->bytecodes[method->bytecodes_count++] = aload_1;
            method->bytecodes[method->bytecodes_count++] = checkcast;
            method->bytecodes[method->bytecodes_count++] = putfield_a_this;
            if((crt_case + 1) < nb_ref_fields)
                method->bytecodes[method->bytecodes_count++] = vreturn_3;
            branches[crt_case++] = aload_1;
        }


    vreturn_2 = create_return_bytecode();
    if(vreturn_2 == NULL)
        return -1;

    stableswitch = create_stableswitch_bytecode(0, nb_ref_fields - 1, branches, vreturn_2);
    if(stableswitch == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 1));
    if(tmp == NULL) {
        perror("add_setCurrentSynchronizableInstance_bytecodes");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[stableswitch_index] = stableswitch;
    method->bytecodes[method->bytecodes_count++] = vreturn_2;

    method->nargs = 2;
    method->max_stack = tweak->setCurrentSynchronizableInstance_supercall_ref ? 2 : 1;

    return 0;

}
