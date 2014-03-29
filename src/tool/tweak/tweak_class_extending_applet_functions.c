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
 * \file tweak_class_extending_applet_functions.c
 * \brief Tweak a class extending the SyncApplet class and handle the static
 * fields.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tweak_info.h"
#include "javacard_framework_info.h"
#include "synclib_info.h"
#include "generic_tweak_functions.h"
#include "add_field_functions.h"
#include "create_bytecode_functions.h"

static method_info* get_getShareableInterfaceObject_method(class_info* class, u1 getShareableInterfaceObject_method_token);
static int tweak_getShareableInterfaceObject_method(method_info* method, constant_pool_entry_info* syncaid_ref, constant_pool_entry_info* equals_ref);
static int is_implementing_external_interface(class_info* class, constant_pool_entry_info* interface_ref);
static method_info* get_implemented_method(class_info* class, u1 imported_package_token, u1 interface_token, u1 interface_method_token);
static int tweak_uninstall_method_impl(method_info* method, constant_pool_entry_info* syncuninstall_ref, u1 overrided_uninstall_method_token);
static int add_supercall_to_process_at_begin(method_info* method, constant_pool_entry_info* ref);
static int add_supercall_to_select_at_begin(method_info* method, constant_pool_entry_info* ref);
static method_info* get_method_by_token_from_class(class_info* class, u1 method_token);
static int add_inherited_protected_methods(analyzed_cap_file* acf, class_info* class, synclib_info* synclib, char is_using_int);

static int generate_list_of_classes(analyzed_cap_file* acf, synclib_info* synclib, constant_pool_entry_info*** list, int* nb_entries);
static int add_getClassId_bytecodes(analyzed_cap_file* acf, constant_pool_entry_info** list, int nb_entries);
static int add_getNewInstance_bytecodes(analyzed_cap_file* acf, constant_pool_entry_info** list, int nb_entries);

static int16_t get_nb_static_boolean_arrays(analyzed_cap_file* acf);
static int add_getNbStaticBooleanArrays_bytecodes(analyzed_cap_file* acf);
static int add_getAnyStaticBooleanArray_bytecodes(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib);
static int16_t get_nb_static_byte_arrays(analyzed_cap_file* acf);
static int add_getNbStaticByteArrays_bytecodes(analyzed_cap_file* acf);
static int add_getAnyStaticByteArray_bytecodes(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib);
static int16_t get_nb_static_short_arrays(analyzed_cap_file* acf);
static int add_getNbStaticShortArrays_bytecodes(analyzed_cap_file* acf);
static int add_getAnyStaticShortArray_bytecodes(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib);
static int16_t get_nb_static_int_arrays(analyzed_cap_file* acf);
static int add_getNbStaticIntArrays_bytecodes(analyzed_cap_file* acf);
static int add_getAnyStaticIntArray_bytecodes(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib);

static int16_t get_nb_classes_with_static(analyzed_cap_file* acf);
static int add_resetStaticPathAll_bytecodes(analyzed_cap_file* acf, synclib_info* synclib);
static int add_numberStaticPathAll_bytecodes(analyzed_cap_file* acf, synclib_info* synclib);
static int add_serializeStaticAll_bytecodes(analyzed_cap_file* acf, synclib_info* synclib);
static int add_mergeStaticAll_bytecodes(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib);



/**
 * \brief Tweak the given class while taking into account that it has SyncApplet
 * in its class hierarchy.
 *  
 * Tweak javacard.framework.Applet.getShareableInterfaceObject method if
 * present.
 *
 * Tweak javacard.framework.AppletEvent.uninstall implementing method if
 * present.
 *
 * Tweak javacard.framework.Applet.select method if present.
 *
 * Tweak javacard.framework.Applet.process method if present.
 *
 * Add methods inherited from synclib.SyncApplet if the given class direct
 * superclass is synclib.SyncApplet.
 *
 * \param acf The analyzed CAP file.
 * \param class The class extending the SyncApplet.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 * \param is_using_int Is the int type used?
 *
 * \return Return -1 if an error occurred, 0 else.
 */
int tweak_class_extending_applet(analyzed_cap_file* acf, class_info* class, javacard_framework_info* javacard_framework, synclib_info* synclib, char is_using_int) {

    method_info* getShareableInterfaceObject = NULL;
    method_info* process_method = NULL;
    method_info* select_method = NULL;
    tweak_info* tweak = (tweak_info*)class->tweak;

    class->flags |= CLASS_SHAREABLE;   /* since SyncApplet implements the shareable interface */

   if((getShareableInterfaceObject = get_getShareableInterfaceObject_method(class, javacard_framework->Applet.getShareableInterfaceObject_method_token)) != NULL) {
        if(tweak_getShareableInterfaceObject_method(getShareableInterfaceObject, synclib->SyncApplet.syncAID_ref, javacard_framework->AID.equals_ref) == -1)
            return -1;
    }

    if(is_implementing_external_interface(class, javacard_framework->AppletEvent.interface_ref)) {
        method_info* uninstall_method = get_implemented_method(class, javacard_framework->package_token, javacard_framework->AppletEvent.interface_token, 0);
        if(uninstall_method == NULL)
            return -1;

        if(!(uninstall_method->flags & METHOD_ABSTRACT)) {
            if(tweak_uninstall_method_impl(uninstall_method, synclib->SyncApplet.syncUninstall_ref, synclib->SyncApplet.uninstall_method_token) == -1)
                return -1;
        }
    }

    if((process_method = get_method_by_token_from_class(class, javacard_framework->Applet.process_method_token)) != NULL) {
        if(!(process_method->flags & METHOD_ABSTRACT)) {
            constant_pool_entry_info* ref = NULL;
            if((ref = add_supercall_to_constant_pool(acf, class, javacard_framework->Applet.process_method_token, NULL, process_method->signature)) == NULL)
                return -1;
            if(add_supercall_to_process_at_begin(process_method, ref) == -1)
                return -1;
        }
    }

    if((select_method = get_method_by_token_from_class(class, javacard_framework->Applet.select_method_token)) != NULL) {
        constant_pool_entry_info* ref = NULL;
        if((ref = add_supercall_to_constant_pool(acf, class, javacard_framework->Applet.select_method_token, NULL, select_method->signature)) == NULL)
            return -1;
        if(add_supercall_to_select_at_begin(select_method, ref) == -1)
            return -1;
    }

    if(tweak->is_SyncApplet_immediate_superclass) {
        if(add_inherited_protected_methods(acf, class, synclib, is_using_int) == -1)
            return -1;
    }

    return 0;

}


/**
 * \brief Generate the bytecodes for handling static field but also
 * synclib.SyncApplet.getClassId and synclib.SyncApplet.getNewInstance methods.
 *
 * Bytecodes are generated for the following methods:
 *  - synclib.SyncApplet.getClassId
 *  - synclib.SyncApplet.getNewInstance
 *  - synclib.SyncApplet.getNbStaticBooleanArrays
 *  - synclib.SyncApplet.getAnyStaticBooleanArray
 *  - synclib.SyncApplet.getNbStaticByteArrays
 *  - synclib.SyncApplet.getAnyStaticByteArray
 *  - synclib.SyncApplet.getNbStaticShortArrays
 *  - synclib.SyncApplet.getAnyStaticShortArray
 *  - synclib.SyncApplet.getNbStaticIntArrays (if is_using_int is 1)
 *  - synclib.SyncApplet.getAnyStaticIntArray (if is_using_int is 1)
 *  - synclib.SyncApplet.resetStaticPathAll
 *  - synclib.SyncApplet.numberStaticPathAll
 *  - synclib.SyncApplet.serializeStaticAll
 *  - synclib.SyncApplet.mergeStaticAll
 *
 * \param acf The analyzed CAP file.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 * \param is_using_int Is the int type used?
 *
 * \return Return -1 if an error occurred, 0 else.
 */
int generate_bytecodes_handling_static(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib, char is_using_int) {

    int nb_entries = 0;
    constant_pool_entry_info** list = NULL;
    if(generate_list_of_classes(acf, synclib, &list, &nb_entries) == -1)
        return -1;

    if(add_getClassId_bytecodes(acf, list, nb_entries) == -1)
        return -1;

    if(add_getNewInstance_bytecodes(acf, list, nb_entries) == -1)
        return -1;

    free(list);

    if(add_getNbStaticBooleanArrays_bytecodes(acf) == -1)
        return -1;

    if(add_getAnyStaticBooleanArray_bytecodes(acf, javacard_framework, synclib) == -1)
        return -1;

    if(add_getNbStaticByteArrays_bytecodes(acf) == -1)
        return -1;

    if(add_getAnyStaticByteArray_bytecodes(acf, javacard_framework, synclib) == -1)
        return -1;

    if(add_getNbStaticShortArrays_bytecodes(acf) == -1)
        return -1;

    if(add_getAnyStaticShortArray_bytecodes(acf, javacard_framework, synclib) == -1)
        return -1;

    if(is_using_int) {
        if(add_getNbStaticIntArrays_bytecodes(acf) == -1)
            return -1;

        if(add_getAnyStaticIntArray_bytecodes(acf, javacard_framework, synclib) == -1)
            return -1;
    }

    if(add_resetStaticPathAll_bytecodes(acf, synclib) == -1)
        return -1;

    if(add_numberStaticPathAll_bytecodes(acf, synclib) == -1)
        return -1;

    if(add_serializeStaticAll_bytecodes(acf, synclib) == -1)
        return -1;

    if(add_mergeStaticAll_bytecodes(acf, javacard_framework, synclib) == -1)
        return -1;

    return 0;

}


/**
 * \brief Return the getShareableInterfaceObject method from the given class.
 * 
 * \param class The class extending the SyncApplet class.
 * \param getShareableInterfaceObject_method_token The token of the
 * getShareableInterfaceObject method.
 *
 * \return Return the method or NULL if not found.
 */
static method_info* get_getShareableInterfaceObject_method(class_info* class, u1 getShareableInterfaceObject_method_token) {

    u2 u2Index = 0;

    for(; u2Index < class->methods_count; ++u2Index)
        if(!(class->methods[u2Index]->flags & (METHOD_STATIC|METHOD_INIT)) && (class->methods[u2Index]->token == getShareableInterfaceObject_method_token))
            return class->methods[u2Index];

    return NULL;

}


/**
 * \brief Tweak getShareableInterfaceObject method.
 *
 * We add a test to verify if the clientAID is equal to sync.SyncManager applet AID.
 * If it is the case, we return this.
 *
 * Bytecodes are inserted at the beginning of the method.
 *
 * \param method The method to tweak.
 * \param syncaid_ref An analyzed constant pool entry to the syncAID field. 
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int tweak_getShareableInterfaceObject_method(method_info* method, constant_pool_entry_info* syncaid_ref, constant_pool_entry_info* equals_ref) {

    bytecode_info** tmp = NULL;
    u2 u2Index = 0;

    bytecode_info* aload_1 = NULL;
    bytecode_info* getstatic = NULL;
    bytecode_info* invokevirtual = NULL;
    bytecode_info* ifne = NULL;
    bytecode_info* aload_0 = NULL;
    bytecode_info* areturn = NULL;

    aload_1 = create_aload_0_bytecode();
    if(aload_1 == NULL)
        return -1;

    getstatic = create_getstatic_T_bytecode(syncaid_ref);
    if(getstatic == NULL)
        return -1;

    invokevirtual = create_invokevirtual_bytecode(equals_ref);
    if(invokevirtual == NULL)
        return -1;

    ifne = create_ifne_bytecode(method->bytecodes[0]);
    if(ifne == NULL)
        return -1;

    aload_0 = create_aload_0_bytecode();
    if(aload_0 == NULL)
        return -1;

    areturn = create_areturn_bytecode();
    if(areturn == NULL)
        return -1;
    
    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 6));
    if(tmp == NULL) {
        perror("tweak_getShareableInterfaceObject_method");
        return -1;
    }
    method->bytecodes = tmp;

    method->bytecodes_count += 6;

    for(u2Index = method->bytecodes_count - 1; u2Index > 5; --u2Index)
        method->bytecodes[u2Index] = method->bytecodes[u2Index - 6];

    method->bytecodes[0] = aload_1;
    method->bytecodes[1] = getstatic;
    method->bytecodes[2] = invokevirtual;
    method->bytecodes[3] = ifne;
    method->bytecodes[4] = aload_0;
    method->bytecodes[5] = areturn;

    return 0;

}

/**
 * \brief Check if a class is implementing an external interface.
 *
 * \param class The class to check.
 * \param interface_ref An analyzed constant pool entry to the external
 * interface.
 *
 * \return Return 1 if the class implements the interface, 0 else.
 */
static int is_implementing_external_interface(class_info* class, constant_pool_entry_info* interface_ref) {

    u1 u1Index = 0;

    while(class != NULL) {
        for(; u1Index < class->interfaces_count; ++u1Index)
            if((class->interfaces[u1Index].ref->flags & CONSTANT_POOL_IS_EXTERNAL) && (class->interfaces[u1Index].ref->external_package->my_index == interface_ref->external_package->my_index) && (class->interfaces[u1Index].ref->external_class_token == interface_ref->external_class_token))
                return 1;

        if(class->superclass && !(class->superclass->flags & CONSTANT_POOL_IS_EXTERNAL))
            class = class->superclass->internal_class;
        else
            break;
    }

    return 0;

}


/**
 * \brief Return the implementing method of a class given the package, interface and interface method tokens.
 *
 * \param class The class extending the SyncApplet class.
 * \param imported_package_token The imported package token.
 * \param interface_token The interface token.
 * \param interface_method_token The interface method token.
 *
 * \return Return the implementing method or NULL if not found.
 */
static method_info* get_implemented_method(class_info* class, u1 imported_package_token, u1 interface_token, u1 interface_method_token) {

    u1 u1Index = 0;

    for(; u1Index < class->interfaces_count; ++u1Index)
        if((class->interfaces[u1Index].ref->flags & CONSTANT_POOL_IS_EXTERNAL) && (class->interfaces[u1Index].ref->external_package->my_index == imported_package_token) && (class->interfaces[u1Index].ref->external_class_token == interface_token)) {
            if(interface_method_token < class->interfaces[u1Index].count)
                return class->interfaces[u1Index].index[interface_method_token].implementation;
            else
                return NULL;
        }

    fprintf(stderr, "Could not find the method implementation\n");
    return NULL;

}


/**
 * \brief Tweak the uninstall method by adding a call to this.SyncUninstall().
 *
 * \param method The uninstall method of the class extending the SyncApplet
 * class.
 * \param syncuninstall_ref An analyzed constant pool entry to the syncUninstall
 * method.
 * \param overrided_uninstall_method_token The token of the overriden uninstall
 * method.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int tweak_uninstall_method_impl(method_info* method, constant_pool_entry_info* syncuninstall_ref, u1 overrided_uninstall_method_token) {

    bytecode_info** tmp = NULL;
    bytecode_info* aload = NULL;
    bytecode_info* invokevirtual = NULL;

    aload = create_aload_0_bytecode();
    if(aload == NULL)
        return -1;

    invokevirtual = create_invokevirtual_bytecode(syncuninstall_ref);
    if(invokevirtual == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 2));
    if(tmp == NULL) {
        perror("tweak_uninstall_method_impl");
        return -1;
    }
    method->bytecodes = tmp;

    method->bytecodes[method->bytecodes_count + 1] = method->bytecodes[method->bytecodes_count - 1];
    method->bytecodes[method->bytecodes_count - 1] = aload;
    method->bytecodes[method->bytecodes_count] = invokevirtual;

    method->token = overrided_uninstall_method_token;
    method->is_overriding = 1;

    method->bytecodes_count += 2;
    return 0;

}


/**
 * \brief Add a call to super.process at the beginning of the given process
 * method.
 *
 * \param method The process method of the class extending the SyncApplet class.
 * \param ref An analyzed constant pool entry to the supercall to the
 * super.process method.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_supercall_to_process_at_begin(method_info* method, constant_pool_entry_info* ref) {

    u2 u2Index = 0;
    bytecode_info** tmp = NULL;

    bytecode_info* aload_0 = NULL;
    bytecode_info* aload_1 = NULL;
    bytecode_info* invoke = NULL;

    aload_0 = create_aload_0_bytecode();
    if(aload_0 == NULL)
        return -1;

    aload_1 = create_aload_1_bytecode();
    if(aload_1 == NULL)
        return -1;

    invoke = create_invokespecial_bytecode(ref);
    if(invoke == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 3));
    if(tmp == NULL) {
        perror("add_supercall_to_process_at_begin");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes_count += 3;

    for(u2Index = method->bytecodes_count - 1; u2Index > 2; --u2Index)
        method->bytecodes[u2Index] = method->bytecodes[u2Index - 3];

    method->bytecodes[0] = aload_0;
    method->bytecodes[1] = aload_1;
    method->bytecodes[2] = invoke;

    return 0;

}


/**
 * \brief Add a call to super.select at the beginning of the give select method.
 *
 * \param method The select method of the class extending the SyncApplet class.
 * \param ref An analyzed constant pool entry to the supercall to the
 * super.select method.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_supercall_to_select_at_begin(method_info* method, constant_pool_entry_info* ref) {

    u2 u2Index = 0;
    bytecode_info** tmp = NULL;

    bytecode_info* aload_0 = NULL;
    bytecode_info* invoke = NULL;
    bytecode_info* pop = NULL;

    aload_0 = create_aload_0_bytecode();
    if(aload_0 == NULL)
        return -1;

    invoke = create_invokespecial_bytecode(ref);
    if(invoke == NULL)
        return -1;

    pop = create_pop_bytecode();
    if(pop == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 3));
    if(tmp == NULL) {
        perror("add_supercall_to_select_at_begin");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes_count += 3;

    for(u2Index = method->bytecodes_count - 1; u2Index > 2; --u2Index)
        method->bytecodes[u2Index] = method->bytecodes[u2Index - 3];

    method->bytecodes[0] = aload_0;
    method->bytecodes[1] = invoke;
    method->bytecodes[2] = pop;

    return 0;

}


/**
 * \brief Get a method from a given class by its token.
 *
 * \param class An analyzed class.
 * \param method_token A method token.
 *
 * \return Return the method or NULL if not found.
 */
static method_info* get_method_by_token_from_class(class_info* class, u1 method_token) {

    u2 u2Index = 0;

    for(; u2Index < class->methods_count; ++u2Index)
        if(class->methods[u2Index]->token == method_token)
            return class->methods[u2Index];

    return NULL;

}


/**
 * \brief Add inherited methods from synclib.SyncApplet to the given class as
 * protected overriding methods.
 *
 * Bytecodes are added later.
 * 
 * \param acf The analyzed CAP file.
 * \param class The class extending the SyncApplet class.
 * \param synclib Information on the synclib package.
 * \param is_using_int Is the int type used?
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_inherited_protected_methods(analyzed_cap_file* acf, class_info* class, synclib_info* synclib, char is_using_int) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    if((tweak->getClassId_method = add_method(acf, class, synclib->SyncApplet.getClassId_sig, METHOD_PROTECTED)) == NULL)
        return -1;
    tweak->getClassId_method->is_overriding = 1;
    tweak->getClassId_method->token = synclib->SyncApplet.getClassId_method_token;

    if((tweak->getNewInstance_method = add_method(acf, class, synclib->SyncApplet.getNewInstance_sig, METHOD_PROTECTED)) == NULL)
        return -1;
    tweak->getNewInstance_method->is_overriding = 1;
    tweak->getNewInstance_method->token = synclib->SyncApplet.getNewInstance_method_token;

    if(get_nb_static_boolean_arrays(acf) > 0) {
        if((tweak->getNbStaticBooleanArrays_method = add_method(acf, class, synclib->SyncApplet.getNbStaticBooleanArrays_sig, METHOD_PROTECTED)) == NULL)
            return -1;
        tweak->getNbStaticBooleanArrays_method->is_overriding = 1;
        tweak->getNbStaticBooleanArrays_method->token = synclib->SyncApplet.getNbStaticBooleanArrays_method_token;

        if((tweak->getAnyStaticBooleanArray_method = add_method(acf, class, synclib->SyncApplet.getAnyStaticBooleanArray_sig, METHOD_PROTECTED)) == NULL)
            return -1;
        tweak->getAnyStaticBooleanArray_method->is_overriding = 1;
        tweak->getAnyStaticBooleanArray_method->token = synclib->SyncApplet.getAnyStaticBooleanArray_method_token;
    }

    if(get_nb_static_byte_arrays(acf) > 0) {
        if((tweak->getNbStaticByteArrays_method = add_method(acf, class, synclib->SyncApplet.getNbStaticByteArrays_sig, METHOD_PROTECTED)) == NULL)
            return -1;
        tweak->getNbStaticByteArrays_method->is_overriding = 1;
        tweak->getNbStaticByteArrays_method->token = synclib->SyncApplet.getNbStaticByteArrays_method_token;

        if((tweak->getAnyStaticByteArray_method = add_method(acf, class, synclib->SyncApplet.getAnyStaticByteArray_sig, METHOD_PROTECTED)) == NULL)
            return -1;
        tweak->getAnyStaticByteArray_method->is_overriding = 1;
        tweak->getAnyStaticByteArray_method->token = synclib->SyncApplet.getAnyStaticByteArray_method_token;
    }

    if(get_nb_static_short_arrays(acf) > 0) {
        if((tweak->getNbStaticShortArrays_method = add_method(acf, class, synclib->SyncApplet.getNbStaticShortArrays_sig, METHOD_PROTECTED)) == NULL)
            return -1;
        tweak->getNbStaticShortArrays_method->is_overriding = 1;
        tweak->getNbStaticShortArrays_method->token = synclib->SyncApplet.getNbStaticShortArrays_method_token;

        if((tweak->getAnyStaticShortArray_method = add_method(acf, class, synclib->SyncApplet.getAnyStaticShortArray_sig, METHOD_PROTECTED)) == NULL)
            return -1;
        tweak->getAnyStaticShortArray_method->is_overriding = 1;
        tweak->getAnyStaticShortArray_method->token = synclib->SyncApplet.getAnyStaticShortArray_method_token;
    }

    if(is_using_int) {
        if(get_nb_static_int_arrays(acf) > 0) {
            if((tweak->getNbStaticIntArrays_method = add_method(acf, class, synclib->SyncApplet.getNbStaticIntArrays_sig, METHOD_PROTECTED)) == NULL)
                return -1;
            tweak->getNbStaticIntArrays_method->is_overriding = 1;
            tweak->getNbStaticIntArrays_method->token = synclib->SyncApplet.getNbStaticIntArrays_method_token;

            if((tweak->getAnyStaticIntArray_method = add_method(acf, class, synclib->SyncApplet.getAnyStaticIntArray_sig, METHOD_PROTECTED)) == NULL)
                return -1;
            tweak->getAnyStaticIntArray_method->is_overriding = 1;
            tweak->getAnyStaticIntArray_method->token = synclib->SyncApplet.getAnyStaticIntArray_method_token;
        }
    }


    if(get_nb_classes_with_static(acf) > 0) {
        if((tweak->resetStaticPathAll_method = add_method(acf, class, synclib->SyncApplet.resetStaticPathAll_sig, METHOD_PROTECTED)) == NULL)
            return -1;
        tweak->resetStaticPathAll_method->is_overriding = 1;
        tweak->resetStaticPathAll_method->token = synclib->SyncApplet.resetStaticPathAll_method_token;

        if((tweak->numberStaticPathAll_method = add_method(acf, class, synclib->SyncApplet.numberStaticPathAll_sig, METHOD_PROTECTED)) == NULL)
            return -1;
        tweak->numberStaticPathAll_method->is_overriding = 1;
        tweak->numberStaticPathAll_method->token = synclib->SyncApplet.numberStaticPathAll_method_token;

        if((tweak->serializeStaticAll_method = add_method(acf, class, synclib->SyncApplet.serializeStaticAll_sig, METHOD_PROTECTED)) == NULL)
            return -1;
        tweak->serializeStaticAll_method->is_overriding = 1;
        tweak->serializeStaticAll_method->token = synclib->SyncApplet.serializeStaticAll_method_token;

        if((tweak->mergeStaticAll_method = add_method(acf, class, synclib->SyncApplet.mergeStaticAll_sig, METHOD_PROTECTED)) == NULL)
            return -1;
        tweak->mergeStaticAll_method->is_overriding = 1;
        tweak->mergeStaticAll_method->token = synclib->SyncApplet.mergeStaticAll_method_token;
    }

    return 0;

}


/**
 * \brief Generate an array of classref constant pool entries used by new or
 * anewarray bytecodes.
 * 
 * \param acf The analyzed CAP file.
 * \param synclib Information on the synclib package.
 * \param list The generated list.
 * \param nb_entries The number of entries in the list.
 *
 * \return -1 if an error occurred, 0 else.
 */
static int generate_list_of_classes(analyzed_cap_file* acf, synclib_info* synclib, constant_pool_entry_info*** list, int* nb_entries) {

    constant_pool_entry_info** tmp = NULL;
    u2 u2Index1 = 0;

    *list = NULL;
    *nb_entries = 0;

    for(; u2Index1 < acf->classes_count; ++u2Index1) {
        u2 u2Index2 = 0;

        for(; u2Index2 < ((tweak_info*)acf->classes[u2Index1]->tweak)->nb_methods_before_tweak; ++u2Index2) {
            u2 u2Index3 = 0;

            for(; u2Index3 < acf->classes[u2Index1]->methods[u2Index2]->bytecodes_count; ++u2Index3)
                if((acf->classes[u2Index1]->methods[u2Index2]->bytecodes[u2Index3]->opcode == 143) || (acf->classes[u2Index1]->methods[u2Index2]->bytecodes[u2Index3]->opcode == 145)) {
                    constant_pool_entry_info* ref = acf->classes[u2Index1]->methods[u2Index2]->bytecodes[u2Index3]->ref;

                    if(is_internal_class_implementing_interface(ref, synclib->Synchronizable.interface_ref, NULL) || is_external_class_implementing_interface(ref, "synclib/Synchronizable")) {
                        int i = 0;

                        for(; i < *nb_entries; ++i)
                            if((*list)[i] == ref)
                                break;

                        if(i == *nb_entries) {
                            tmp = (constant_pool_entry_info**)realloc(*list, sizeof(constant_pool_entry_info*) * (*nb_entries + 1));
                            if(tmp == NULL) {
                                perror("generate_list_of_classes");
                                return -1;
                            }
                            *list = tmp;

                            (*list)[(*nb_entries)++] = acf->classes[u2Index1]->methods[u2Index2]->bytecodes[u2Index3]->ref;
                        }
                    }
                }
        }
    }

    return 0;

}


/**
 * \brief Generate the bytecodes for the synclib.SyncApplet.getClassId method
 * using the array of constant pool entries for each class with it being
 * previously added.
 *
 * Bytecodes are generated for each valid
 * ((tweak_info*)acf->classes[]->tweak)->getClassId_method field.
 * 
 * \verbatim
// Generated code

if(instance instanceof SomeClass)
    return 0;
if(instance instanceof SomeOtherClass)
    return 1;
return -1;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param list The list generated by a call to \link generate_list_of_classes()
 * \endlink.
 * \param nb_entries The number of entries in the list.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getClassId_bytecodes(analyzed_cap_file* acf, constant_pool_entry_info** list, int nb_entries) {

    u2 u2Index = 0;

    for(; u2Index < acf->classes_count; ++u2Index)
        if(((tweak_info*)acf->classes[u2Index]->tweak)->getClassId_method) {
            method_info* method = ((tweak_info*)acf->classes[u2Index]->tweak)->getClassId_method;
            int16_t crt_id = 0;
            int i = 0;
            bytecode_info** tmp = NULL;
            bytecode_info* sconst_m1 = NULL;
            bytecode_info* sreturn_2 = NULL;

            for(; i < nb_entries; ++i) {
                bytecode_info* aload_1 = NULL;
                bytecode_info* instanceof = NULL;
                bytecode_info* push_value = NULL;
                bytecode_info* sreturn_1 = NULL;

                aload_1 = create_aload_1_bytecode();
                if(aload_1 == NULL)
                    return -1;

                if(method->bytecodes_count > 4) {
                    method->bytecodes[method->bytecodes_count - 3] = create_ifeq_bytecode(aload_1);
                        if(method->bytecodes[method->bytecodes_count - 3] == NULL)
                            return -1;
                }

                instanceof = create_instanceof_bytecode(list[i]);
                if(instanceof == NULL)
                    return -1;

                push_value = create_push_short_value_bytecode(crt_id++);
                if(push_value == NULL)
                    return -1;

                sreturn_1 = create_sreturn_bytecode();
                if(sreturn_1 == NULL)
                    return -1;

                tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 5));
                if(tmp == NULL) {
                    perror("add_getClassId_bytecodes");
                    return -1;
                }
                method->bytecodes = tmp;

                method->bytecodes[method->bytecodes_count++] = aload_1;
                method->bytecodes[method->bytecodes_count++] = instanceof;
                method->bytecodes[method->bytecodes_count++] = NULL; // ifeq
                method->bytecodes[method->bytecodes_count++] = push_value;
                method->bytecodes[method->bytecodes_count++] = sreturn_1;
            }

            sconst_m1 = create_push_short_value_bytecode(-1);
            if(sconst_m1 == NULL)
                return -1;

            if(method->bytecodes_count > 4) {
                method->bytecodes[method->bytecodes_count - 3] = create_ifeq_bytecode(sconst_m1);
                if(method->bytecodes[method->bytecodes_count - 3] == NULL)
                    return -1;
            }

            sreturn_2 = create_sreturn_bytecode();
            if(sreturn_2 == NULL)
                return -1;

            tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 2));
            if(tmp == NULL) {
                perror("add_getClassId_bytecodes");
                return -1;
            }
            method->bytecodes = tmp;

            method->bytecodes[method->bytecodes_count++] = sconst_m1;
            method->bytecodes[method->bytecodes_count++] = sreturn_2;

            method->nargs = 2;
            method->max_stack = 1;
        }

    return 0;

}

/**
 * \brief Generate the bytecodes for the synclib.SyncApplet.getNewInstance method
 * using the array of constant pool entries for each class with it being
 * previously added.
 * 
 * Bytecodes are generated for each valid
 * ((tweak_info*)acf->classes[]->tweak)->getNewInstance_method field.
 *
 * \verbatim
// Generated code
switch(classId) {
    case 0:
        return new SomeClass(in);
    case 1:
        return new SomeOtherClass(in);
}
return null;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param list The list generated by a call to \link generate_list_of_classes()
 * \endlink.
 * \param nb_entries The number of entries in the list.
 *
 */
static int add_getNewInstance_bytecodes(analyzed_cap_file* acf, constant_pool_entry_info** list, int nb_entries) {

    u2 u2Index = 0;

    for(; u2Index < acf->classes_count; ++u2Index)
        if(((tweak_info*)acf->classes[u2Index]->tweak)->getNewInstance_method) {
            int i = 0;
            method_info* method = ((tweak_info*)acf->classes[u2Index]->tweak)->getNewInstance_method;
            bytecode_info* sload_2 = NULL;
            bytecode_info* stableswitch = NULL;
            bytecode_info** branches = NULL;
            bytecode_info* aconst_null = NULL;
            bytecode_info* areturn_2 = NULL;

            sload_2 = create_sload_2_bytecode();
            if(sload_2 == NULL)
                return -1;

            branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_entries);
            if(branches == NULL) {
                perror("add_getNewInstance_bytecodes");
                return -1;
            }

            method->bytecodes_count = 2;
            method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * (2 + (nb_entries * 5) + 2));
            if(method->bytecodes == NULL) {
                perror("add_getNewInstance_bytecodes");
                return -1;
            }

            method->bytecodes[0] = sload_2;
            method->bytecodes[1] = NULL;

            for(; i < nb_entries; ++i) {
                bytecode_info* new = NULL;
                bytecode_info* dup = NULL;
                bytecode_info* aload_1 = NULL;
                bytecode_info* invokesp = NULL;
                bytecode_info* areturn_1 = NULL;

                new = create_new_bytecode(list[i]);
                if(new == NULL)
                    return -1;

                dup = create_dup_bytecode();
                if(dup == NULL)
                    return -1;

                aload_1 = create_aload_1_bytecode();
                if(aload_1 == NULL)
                    return -1;

                invokesp = create_invokespecial_bytecode(((tweak_info*)list[i]->internal_class->tweak)->init_method->this_method);
                if(invokesp == NULL)
                    return -1;

                areturn_1 = create_areturn_bytecode();
                if(areturn_1 == NULL)
                    return -1;

                method->bytecodes[method->bytecodes_count++] = new;
                method->bytecodes[method->bytecodes_count++] = dup;
                method->bytecodes[method->bytecodes_count++] = aload_1;
                method->bytecodes[method->bytecodes_count++] = invokesp;
                method->bytecodes[method->bytecodes_count++] = areturn_1;

                branches[i] = aload_1;
            }

            aconst_null = create_aconst_null_bytecode();
            if(aconst_null == NULL)
                return -1;

            stableswitch = create_stableswitch_bytecode(0, nb_entries - 1, branches, aconst_null);
            if(stableswitch == NULL)
                return -1;

            areturn_2 = create_areturn_bytecode();
            if(areturn_2 == NULL)
                return -1;

            method->bytecodes[1] = stableswitch;
            method->bytecodes[method->bytecodes_count++] = aconst_null;
            method->bytecodes[method->bytecodes_count++] = areturn_2;


            method->nargs = 3;
            method->max_stack = 3;
        }

    return 0;

}


/**
 * \brief Get the total number of static boolean arrays from every classes in
 * this package.
 * 
 * \param acf The analyzed CAP file.
 *
 * \return Return the number of static boolean arrays.
 */
static int16_t get_nb_static_boolean_arrays(analyzed_cap_file* acf) {

    u2 u2Index = 0;
    int16_t count = 0;

    for(; u2Index < acf->classes_count; ++u2Index)
        count += ((tweak_info*)acf->classes[u2Index]->tweak)->nb_static_boolean_array_fields;

    return count;

}


/**
 * \brief Generate the bytecodes for the getNbStaticBooleanArrays method for
 * each class with it being added previously.
 *
 * Bytecodes are generated for each valide
 * ((tweak_info*)acf->classes[]->tweak)->getNbStaticBooleanArrays_method field.
 *
 * \verbatim
// Generated code

return nbStaticBooleanArrays;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getNbStaticBooleanArrays_bytecodes(analyzed_cap_file* acf) {  

    int16_t nb_static_boolean_arrays = get_nb_static_boolean_arrays(acf);
    u2 u2Index = 0;

    for(; u2Index < acf->classes_count; ++u2Index)
        if(((tweak_info*)acf->classes[u2Index]->tweak)->getNbStaticBooleanArrays_method) {
            method_info* method = ((tweak_info*)acf->classes[u2Index]->tweak)->getNbStaticBooleanArrays_method;
            bytecode_info* push_value = NULL;
            bytecode_info* sreturn = NULL;

            push_value = create_push_short_value_bytecode(nb_static_boolean_arrays);
            if(push_value == NULL)
                return -1;

            sreturn = create_sreturn_bytecode();
            if(sreturn == NULL)
                return -1;

            method->bytecodes_count = 2;
            method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
            if(method->bytecodes == NULL) {
                perror("add_getNbStaticBooleanArrays_bytecodes");
                return -1;
            }
            method->bytecodes[0] = push_value;
            method->bytecodes[1] = sreturn;

            method->nargs = 1;
            method->max_stack = 1;
        }

    return 0;

}


/**
 * \brief Add the getStaticBooleanArray method and generate the necessary bytecodes.
 *
 * Static boolean array fields are numbered starting from the given starting_fieldId parameter.
 *
 * The added method (if any) is put in the tweak->getStaticBooleanArray_method field.
 *
 * \verbatim
// Generated code

switch(fieldId) {
    case 0:
        return someStaticField;
    case 1:
        return someOtherStaticField;
}
ISOException.throwIt(ISO7816.SW_UNKNOWN);
return null;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param class The class to which the method is added if necessary.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 * \param starting_fieldId The starting number for numbering the boolean array
 * fields.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getStaticBooleanArray_method(analyzed_cap_file* acf, class_info* class, javacard_framework_info* javacard_framework, synclib_info* synclib, u2 starting_fieldId) {

    u2 u2Index = 0;
    method_info* method = NULL;
    bytecode_info* sload_0 = NULL;

    bytecode_info* stableswitch = NULL;
    u2 crt_case = 0;
    u2 nb_cases = 0;
    bytecode_info** branches = NULL;

    bytecode_info* sspush = NULL;
    bytecode_info* invokestatic = NULL;
    bytecode_info* aconst_null = NULL;
    bytecode_info* areturn_2 = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if((method = tweak->getStaticBooleanArray_method = add_method(acf, class, synclib->SyncApplet.getAnyStaticBooleanArray_sig, METHOD_STATIC|METHOD_PACKAGE)) == NULL)
        return -1;

    sload_0 = create_sload_0_bytecode();
    if(sload_0 == NULL)
        return -1;

    nb_cases = (u2)tweak->nb_static_boolean_array_fields;
   branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_cases);
    if(branches == NULL) {
        perror("add_getStaticBooleanArray_method");
        return -1;
    }

    method->bytecodes_count = 2;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * (2 + (nb_cases * 2) + 4));
    if(method->bytecodes == NULL) {
        perror("add_getStaticBooleanArray_method");
        return -1;
    }

    method->bytecodes[0] = sload_0;
    method->bytecodes[1] = NULL;

    for(; u2Index < class->fields_count; ++u2Index)
        if(!(class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) && (class->fields[u2Index]->type->types->type == (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_BOOLEAN))) {
            bytecode_info* getstatic_a = NULL;
            bytecode_info* areturn_1 = NULL;

            getstatic_a = create_getstatic_T_bytecode(class->fields[u2Index]->this_field);
            if(getstatic_a == NULL)
                return -1;

            areturn_1 = create_areturn_bytecode();
            if(areturn_1 == NULL)
                return -1;

            branches[crt_case++] = getstatic_a;

            method->bytecodes[method->bytecodes_count++] = getstatic_a;
            method->bytecodes[method->bytecodes_count++] = areturn_1;
        }

    sspush = create_push_short_value_bytecode(javacard_framework->ISO7816.sw_unknown_value);
    if(sspush == NULL)
        return -1;

    stableswitch = create_stableswitch_bytecode(starting_fieldId, starting_fieldId + nb_cases - 1, branches, sspush);
    if(stableswitch == NULL)
        return -1; 

    invokestatic = create_invokestatic_bytecode(javacard_framework->ISOException.throwIt_ref);
    if(invokestatic == NULL)
        return -1;

    aconst_null = create_aconst_null_bytecode();
    if(aconst_null == NULL)
        return -1;

    areturn_2 = create_areturn_bytecode();
    if(areturn_2 == NULL)
        return -1;

    method->bytecodes[1] = stableswitch;
    method->bytecodes[method->bytecodes_count++] = sspush;
    method->bytecodes[method->bytecodes_count++] = invokestatic;
    method->bytecodes[method->bytecodes_count++] = aconst_null;
    method->bytecodes[method->bytecodes_count++] = areturn_2;

    method->nargs = 1;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Add the supporting getStaticBooleanArray methods and generate bytecodes to
 * appropriately call them to the getAnyStaticBooleanArray method for each class
 * with it being previously added.
 *
 * Bytecodes are generated for each valid
 * ((tweak_info*)acf->classes[]->tweak)->getAnyStaticBooleanArray_method field. 
 *
 * \verbatim
// Generated code

if(fieldId < (short)2)
    return SomeClass.getStaticBooleanArray(fieldId);
if(fieldId < (short)3)
    return SomeOtherClass.getStaticBooleanArray(fieldId);
ISOException.throwIt(ISO7816.SW_UNKNOWN);
return null;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getAnyStaticBooleanArray_bytecodes(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib) {

    u2 u2Index1 = 0;
    int16_t crt_fieldId = 0;
    int16_t nb_classes = 0;

    for(; u2Index1 < acf->classes_count; ++u2Index1)
        if(((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_boolean_array_fields > 0) {
            if(add_getStaticBooleanArray_method(acf, acf->classes[u2Index1], javacard_framework, synclib, crt_fieldId) == -1)
                return -1;
            crt_fieldId += ((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_boolean_array_fields;
            ++nb_classes;
        }

    for(u2Index1 = 0; u2Index1 < acf->classes_count; ++u2Index1)
        if(((tweak_info*)acf->classes[u2Index1]->tweak)->getAnyStaticBooleanArray_method) {
            u2 u2Index2 = 0;
            method_info* method = ((tweak_info*)acf->classes[u2Index1]->tweak)->getAnyStaticBooleanArray_method;

            bytecode_info* sspush = NULL;
            bytecode_info* invokestatic_2 = NULL;
            bytecode_info* aconst_null = NULL;
            bytecode_info* areturn_2 = NULL;

            crt_fieldId = 0;

            method->bytecodes_count = 0;
            method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * ((nb_classes * 6) + 4));
            if(method->bytecodes == NULL) {
                perror("add_getAnyStaticBooleanArray_bytecodes");
                return -1;
            }

            for(; u2Index2 < acf->classes_count; ++u2Index2)
                if(((tweak_info*)acf->classes[u2Index2]->tweak)->getStaticBooleanArray_method) {
                    bytecode_info* sload_1_1 = NULL;
                    bytecode_info* push_value = NULL;
                    bytecode_info* sload_1_2 = NULL;
                    bytecode_info* invokestatic_1 = NULL;
                    bytecode_info* areturn_1 = NULL;

                    crt_fieldId += ((tweak_info*)acf->classes[u2Index2]->tweak)->nb_static_boolean_array_fields;

                    sload_1_1 = create_sload_1_bytecode();
                    if(sload_1_1 == NULL)
                        return -1;

                    if(method->bytecodes_count > 5) {
                        method->bytecodes[method->bytecodes_count - 4] = create_if_scmpge_bytecode(sload_1_1);
                        if(method->bytecodes[method->bytecodes_count - 4] == NULL)
                            return -1;
                    }

                    push_value = create_push_short_value_bytecode(crt_fieldId);
                    if(push_value == NULL)
                        return -1;

                    sload_1_2 = create_sload_1_bytecode();
                    if(sload_1_2 == NULL)
                        return -1;

                    invokestatic_1 = create_invokestatic_bytecode(((tweak_info*)acf->classes[u2Index2]->tweak)->getStaticBooleanArray_method->this_method);
                    if(invokestatic_1 == NULL)
                        return -1;

                    areturn_1 = create_areturn_bytecode();
                    if(areturn_1 == NULL)
                        return -1;

                    method->bytecodes[method->bytecodes_count++] = sload_1_1;
                    method->bytecodes[method->bytecodes_count++] = push_value;
                    ++method->bytecodes_count;
                    method->bytecodes[method->bytecodes_count++] = sload_1_2;
                    method->bytecodes[method->bytecodes_count++] = invokestatic_1;
                    method->bytecodes[method->bytecodes_count++] = areturn_1;
                }

            sspush = create_push_short_value_bytecode(javacard_framework->ISO7816.sw_unknown_value);
            if(sspush == NULL)
                return -1;

            if(method->bytecodes_count > 5) {
                method->bytecodes[method->bytecodes_count - 4] = create_if_scmpge_bytecode(sspush);
                if(method->bytecodes[method->bytecodes_count - 4] == NULL)
                    return -1;
            }

            invokestatic_2 = create_invokestatic_bytecode(javacard_framework->ISOException.throwIt_ref);
            if(invokestatic_2 == NULL)
                return -1;

            aconst_null = create_aconst_null_bytecode();
            if(aconst_null == NULL)
                return -1;

            areturn_2 = create_areturn_bytecode();
            if(areturn_2 == NULL)
                return -1;

            method->bytecodes[method->bytecodes_count++] = sspush;
            method->bytecodes[method->bytecodes_count++] = invokestatic_2;
            method->bytecodes[method->bytecodes_count++] = aconst_null;
            method->bytecodes[method->bytecodes_count++] = areturn_2;

            method->nargs = 2;
            method->max_stack = 2;
        }

    return 0; 

}


/**
 * \brief Get the total number of static byte arrays from every classes in this
 * package.
 * 
 * \param acf The analyzed CAP file.
 *
 * \return Return the number of static byte arrays.
 */
static int16_t get_nb_static_byte_arrays(analyzed_cap_file* acf) {

    u2 u2Index = 0;
    int16_t count = 0;

    for(; u2Index < acf->classes_count; ++u2Index)
        count += ((tweak_info*)acf->classes[u2Index]->tweak)->nb_static_byte_array_fields;

    return count;

}


/**
 * \brief Generate the bytecodes for the getNbStaticByteArrays method
 * for each class with it being added previously.
 *
 * Bytecodes are generated for each valid
 * ((tweak_info*)acf->classes[]->tweak)->getNbStaticByteArrays_method field. 
 *
 * \verbatim
// Generated code

return nbStaticByteArrays;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getNbStaticByteArrays_bytecodes(analyzed_cap_file* acf) {  

    int16_t nb_static_byte_arrays = get_nb_static_byte_arrays(acf);
    u2 u2Index = 0;

    for(; u2Index < acf->classes_count; ++u2Index)
        if(((tweak_info*)acf->classes[u2Index]->tweak)->getNbStaticByteArrays_method) {
            method_info* method = ((tweak_info*)acf->classes[u2Index]->tweak)->getNbStaticByteArrays_method;
            bytecode_info* push_value = NULL;
            bytecode_info* sreturn = NULL;

            push_value = create_push_short_value_bytecode(nb_static_byte_arrays);
            if(push_value == NULL)
                return -1;

            sreturn = create_sreturn_bytecode();
            if(sreturn == NULL)
                return -1;

            method->bytecodes_count = 2;
            method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
            if(method->bytecodes == NULL) {
                perror("add_getNbStaticByteArrays_bytecodes");
                return -1;
            }
            method->bytecodes[0] = push_value;
            method->bytecodes[1] = sreturn;

            method->nargs = 1;
            method->max_stack = 1;
        }

    return 0;

}


/**
 * \brief Add the getStaticByteArray method and generate the necessary
 * bytecodes.
 *
 * Static byte array fields are numbered starting from the given
 * starting_fieldId parameter.
 *
 * Added method (if any) is put in tweak->getStaticByteArray_method field.
 *
 * \verbatim
// Generated code

switch(fieldId) {
    case 0:
        return someStaticField;
    case 1:
        return someOtherStaticField;
}
ISOException.throwIt(ISO7816.SW_UNKNOWN);
return null;
\endverbatim
 * 
 * \param acf The analyzed CAP file.
 * \param class The class to which the method is added if necessary.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 * \param starting_fieldId The starting number for numbering the byte array
 * fields.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getStaticByteArray_method(analyzed_cap_file* acf, class_info* class, javacard_framework_info* javacard_framework, synclib_info* synclib, u2 starting_fieldId) {

    u2 u2Index = 0;
    method_info* method = NULL;
    bytecode_info* sload_0 = NULL;

    bytecode_info* stableswitch = NULL;
    u2 crt_case = 0;
    u2 nb_cases = 0;
    bytecode_info** branches = NULL;

    bytecode_info* sspush = NULL;
    bytecode_info* invokestatic = NULL;
    bytecode_info* aconst_null = NULL;
    bytecode_info* areturn_2 = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if((method = tweak->getStaticByteArray_method = add_method(acf, class, synclib->SyncApplet.getAnyStaticByteArray_sig, METHOD_STATIC|METHOD_PACKAGE)) == NULL)
        return -1;

    sload_0 = create_sload_0_bytecode();
    if(sload_0 == NULL)
        return -1;

    nb_cases = (u2)tweak->nb_static_byte_array_fields;
    branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_cases);
    if(branches == NULL) {
        perror("add_getStaticByteArray_method");
        return -1;
    }

    method->bytecodes_count = 2;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * (2 + (nb_cases * 2) + 4));
    if(method->bytecodes == NULL) {
        perror("add_getStaticByteArray_method");
        return -1;
    }

    method->bytecodes[0] = sload_0;
    method->bytecodes[1] = NULL;

    for(; u2Index < class->fields_count; ++u2Index)
        if(((class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) == FIELD_STATIC) && ((class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_BYTE)) == (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_BYTE))) {
            bytecode_info* getstatic_a = NULL;
            bytecode_info* areturn_1 = NULL;

            getstatic_a = create_getstatic_T_bytecode(class->fields[u2Index]->this_field);
            if(getstatic_a == NULL)
                return -1;

            areturn_1 = create_areturn_bytecode();
            if(areturn_1 == NULL)
                return -1;

            branches[crt_case++] = getstatic_a;

            method->bytecodes[method->bytecodes_count++] = getstatic_a;
            method->bytecodes[method->bytecodes_count++] = areturn_1;
        }

    sspush = create_push_short_value_bytecode(javacard_framework->ISO7816.sw_unknown_value);
    if(sspush == NULL)
        return -1;

    stableswitch = create_stableswitch_bytecode(starting_fieldId, starting_fieldId + nb_cases - 1, branches, sspush);
    if(stableswitch == NULL)
        return -1; 

    invokestatic = create_invokestatic_bytecode(javacard_framework->ISOException.throwIt_ref);
    if(invokestatic == NULL)
        return -1;

    aconst_null = create_aconst_null_bytecode();
    if(aconst_null == NULL)
        return -1;

    areturn_2 = create_areturn_bytecode();
    if(areturn_2 == NULL)
        return -1;

    method->bytecodes[1] = stableswitch;
    method->bytecodes[method->bytecodes_count++] = sspush;
    method->bytecodes[method->bytecodes_count++] = invokestatic;
    method->bytecodes[method->bytecodes_count++] = aconst_null;
    method->bytecodes[method->bytecodes_count++] = areturn_2;

    method->nargs = 1;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Add the supporting getStaticByteArray methods and generate bytecodes
 * to appropriately call them to the getAnyStaticByteArray method for each class
 * with it being previously added.
 *
 * Bytecodes are generated for each valid
 * ((tweak_info*)acf->classes[]->tweak)->getAnyStaticByteArray_method field.
 *
 * \verbatim
// Generated code

if(fieldId < (short)2)
    return SomeClass.getStaticByteArray(fieldId);
if(fieldId < (short)3)
    return SomeOtherClass.getStaticByteArray(fieldId);
ISOException.throwIt(ISO7816.SW_UNKNOWN);
return null;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getAnyStaticByteArray_bytecodes(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib) {

    u2 u2Index1 = 0;
    int16_t crt_fieldId = 0;
    int16_t nb_classes = 0;

    for(; u2Index1 < acf->classes_count; ++u2Index1)
        if(((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_byte_array_fields > 0) {
            if(add_getStaticByteArray_method(acf, acf->classes[u2Index1], javacard_framework, synclib, crt_fieldId) == -1)
                return -1;
            crt_fieldId += ((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_byte_array_fields;
            ++nb_classes;
        }

    for(u2Index1 = 0; u2Index1 < acf->classes_count; ++u2Index1)
        if(((tweak_info*)acf->classes[u2Index1]->tweak)->getAnyStaticByteArray_method) {
            u2 u2Index2 = 0;
            method_info* method = ((tweak_info*)acf->classes[u2Index1]->tweak)->getAnyStaticByteArray_method;

            bytecode_info* sspush = NULL;
            bytecode_info* invokestatic_2 = NULL;
            bytecode_info* aconst_null = NULL;
            bytecode_info* areturn_2 = NULL;

            crt_fieldId = 0;

            method->bytecodes_count = 0;
            method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * ((nb_classes * 6) + 4));
            if(method->bytecodes == NULL) {
                perror("add_getAnyStaticByteArray_bytecodes");
                return -1;
            }

            for(; u2Index2 < acf->classes_count; ++u2Index2)
                if(((tweak_info*)acf->classes[u2Index2]->tweak)->getStaticByteArray_method) {
                    bytecode_info* sload_1_1 = NULL;
                    bytecode_info* push_value = NULL;
                    bytecode_info* sload_1_2 = NULL;
                    bytecode_info* invokestatic_1 = NULL;
                    bytecode_info* areturn_1 = NULL;

                    crt_fieldId += ((tweak_info*)acf->classes[u2Index2]->tweak)->nb_static_byte_array_fields;

                    sload_1_1 = create_sload_1_bytecode();
                    if(sload_1_1 == NULL)
                        return -1;

                    if(method->bytecodes_count > 5) {
                        method->bytecodes[method->bytecodes_count - 4] = create_if_scmpge_bytecode(sload_1_1);
                        if(method->bytecodes[method->bytecodes_count - 4] == NULL)
                            return -1;
                    }

                    push_value = create_push_short_value_bytecode(crt_fieldId);
                    if(push_value == NULL)
                        return -1;

                    sload_1_2 = create_sload_1_bytecode();
                    if(sload_1_2 == NULL)
                        return -1;

                    invokestatic_1 = create_invokestatic_bytecode(((tweak_info*)acf->classes[u2Index2]->tweak)->getStaticByteArray_method->this_method);
                    if(invokestatic_1 == NULL)
                        return -1;

                    areturn_1 = create_areturn_bytecode();
                    if(areturn_1 == NULL)
                        return -1;

                    method->bytecodes[method->bytecodes_count++] = sload_1_1;
                    method->bytecodes[method->bytecodes_count++] = push_value;
                    ++method->bytecodes_count;
                    method->bytecodes[method->bytecodes_count++] = sload_1_2;
                    method->bytecodes[method->bytecodes_count++] = invokestatic_1;
                    method->bytecodes[method->bytecodes_count++] = areturn_1;
                }

            sspush = create_push_short_value_bytecode(javacard_framework->ISO7816.sw_unknown_value);
            if(sspush == NULL)
                return -1;

            if(method->bytecodes_count > 5) {
                method->bytecodes[method->bytecodes_count - 4] = create_if_scmpge_bytecode(sspush);
                if(method->bytecodes[method->bytecodes_count - 4] == NULL)
                    return -1;
            }

            invokestatic_2 = create_invokestatic_bytecode(javacard_framework->ISOException.throwIt_ref);
            if(invokestatic_2 == NULL)
                return -1;

            aconst_null = create_aconst_null_bytecode();
            if(aconst_null == NULL)
                return -1;

            areturn_2 = create_areturn_bytecode();
            if(areturn_2 == NULL)
                return -1;

            method->bytecodes[method->bytecodes_count++] = sspush;
            method->bytecodes[method->bytecodes_count++] = invokestatic_2;
            method->bytecodes[method->bytecodes_count++] = aconst_null;
            method->bytecodes[method->bytecodes_count++] = areturn_2;

            method->nargs = 2;
            method->max_stack = 2;
        }

    return 0; 

}


/**
 * \brief Get the total number of static short arrays from every classes in this
 * package.
 *
 * \param acf The analyzed CAP file.
 *
 * \return Return the number of static short arrays.
 */
static int16_t get_nb_static_short_arrays(analyzed_cap_file* acf) {

    u2 u2Index = 0;
    int16_t count = 0;

    for(; u2Index < acf->classes_count; ++u2Index)
        count += ((tweak_info*)acf->classes[u2Index]->tweak)->nb_static_short_array_fields;

    return count;

}


/**
 * \brief Generate the bytecodes for the getNbStaticShortArrays method for each
 * class with it being added previously.
 *
 * Bytecodes are generated for each valid
 * acf->classse[]->tweak.getNbStaticShortArrays_methods field.
 *
 * \verbatim
// Generated code

return nbStaticShortArrays;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getNbStaticShortArrays_bytecodes(analyzed_cap_file* acf) {  

    int16_t nb_static_short_arrays = get_nb_static_short_arrays(acf);
    u2 u2Index = 0;

    for(; u2Index < acf->classes_count; ++u2Index)
        if(((tweak_info*)acf->classes[u2Index]->tweak)->getNbStaticShortArrays_method) {
            method_info* method = ((tweak_info*)acf->classes[u2Index]->tweak)->getNbStaticShortArrays_method;
            bytecode_info* push_value = NULL;
            bytecode_info* sreturn = NULL;

            push_value = create_push_short_value_bytecode(nb_static_short_arrays);
            if(push_value == NULL)
                return -1;

            sreturn = create_sreturn_bytecode();
            if(sreturn == NULL)
                return -1;

            method->bytecodes_count = 2;
            method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
            if(method->bytecodes == NULL) {
                perror("add_getNbStaticShortArrays_bytecodes");
                return -1;
            }
            method->bytecodes[0] = push_value;
            method->bytecodes[1] = sreturn;

            method->nargs = 1;
            method->max_stack = 1;
        }

    return 0;

}


/**
 * \bref Add the getStaticShortArray method and generate the necessary
 * bytecodes.
 *
 * Static short array fields are numbered starting from the given
 * starting_fieldId parameter.
 *
 * Added method (if any) is put in tweak->getStaticShortArray_method
 * field.
 *
 * \verbatim
// Generated code

switch(fieldId) {
    case 0:
        return someStaticField;
    case 1:
        return someOtherStaticField;
}
ISOException.throwIt(ISO7816.SW_UNKNOWN);
return null;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param class The class to which the method is added if necessary.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 * \param starting_fieldId The starting number for numbering the short array
 * fields.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getStaticShortArray_method(analyzed_cap_file* acf, class_info* class, javacard_framework_info* javacard_framework, synclib_info* synclib, u2 starting_fieldId) {

    u2 u2Index = 0;
    method_info* method = NULL;
    bytecode_info* sload_0 = NULL;

    bytecode_info* stableswitch = NULL;
    u2 crt_case = 0;
    u2 nb_cases = 0;
    bytecode_info** branches = NULL;

    bytecode_info* sspush = NULL;
    bytecode_info* invokestatic = NULL;
    bytecode_info* aconst_null = NULL;
    bytecode_info* areturn_2 = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if((method = tweak->getStaticShortArray_method = add_method(acf, class, synclib->SyncApplet.getAnyStaticShortArray_sig, METHOD_STATIC|METHOD_PACKAGE)) == NULL)
        return -1;

    sload_0 = create_sload_0_bytecode();
    if(sload_0 == NULL)
        return -1;

    nb_cases = (u2)tweak->nb_static_short_array_fields;
   branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_cases);
    if(branches == NULL) {
        perror("add_getStaticShortArray_method");
        return -1;
    }

    method->bytecodes_count = 2;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * (2 + (nb_cases * 2) + 4));
    if(method->bytecodes == NULL) {
        perror("add_getStaticShortArray_method");
        return -1;
    }

    method->bytecodes[0] = sload_0;
    method->bytecodes[1] = NULL;

    for(; u2Index < class->fields_count; ++u2Index)
        if(((class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) == FIELD_STATIC) && ((class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_SHORT)) == (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_SHORT))) {
            bytecode_info* getstatic_a = NULL;
            bytecode_info* areturn_1 = NULL;

            getstatic_a = create_getstatic_T_bytecode(class->fields[u2Index]->this_field);
            if(getstatic_a == NULL)
                return -1;

            areturn_1 = create_areturn_bytecode();
            if(areturn_1 == NULL)
                return -1;

            branches[crt_case++] = getstatic_a;

            method->bytecodes[method->bytecodes_count++] = getstatic_a;
            method->bytecodes[method->bytecodes_count++] = areturn_1;
        }

    sspush = create_push_short_value_bytecode(javacard_framework->ISO7816.sw_unknown_value);
    if(sspush == NULL)
        return -1;

    stableswitch = create_stableswitch_bytecode(starting_fieldId, starting_fieldId + nb_cases - 1, branches, sspush);
    if(stableswitch == NULL)
        return -1; 

    invokestatic = create_invokestatic_bytecode(javacard_framework->ISOException.throwIt_ref);
    if(invokestatic == NULL)
        return -1;

    aconst_null = create_aconst_null_bytecode();
    if(aconst_null == NULL)
        return -1;

    areturn_2 = create_areturn_bytecode();
    if(areturn_2 == NULL)
        return -1;

    method->bytecodes[1] = stableswitch;
    method->bytecodes[method->bytecodes_count++] = sspush;
    method->bytecodes[method->bytecodes_count++] = invokestatic;
    method->bytecodes[method->bytecodes_count++] = aconst_null;
    method->bytecodes[method->bytecodes_count++] = areturn_2;

    method->nargs = 1;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Add the supporting getStaticShortArray methods and generate bytecodes to
 * appropriately call them to the getAnyStaticShortArray method for each class
 * with it being previously added.
 *
 * Bytecodes are generated for each valid
 * ((tweak_info*)acf->classes[]->tweak)->getAnyStaticShortArray_method field.
 *
 * \verbatim
// Generated code

if(fieldId < (short)2)
    return SomeClass.getStaticShortArray(fieldId);
if(fieldId < (short)3)
    return SomeOtherClass.getStaticShortArray(fieldId);
ISOException.throwIt(ISO7816.SW_UNKNOWN);
return null;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getAnyStaticShortArray_bytecodes(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib) {

    u2 u2Index1 = 0;
    int16_t crt_fieldId = 0;
    int16_t nb_classes = 0;

    for(; u2Index1 < acf->classes_count; ++u2Index1)
        if(((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_short_array_fields > 0) {
            if(add_getStaticShortArray_method(acf, acf->classes[u2Index1], javacard_framework, synclib, crt_fieldId) == -1)
                return -1;
            crt_fieldId += ((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_short_array_fields;
            ++nb_classes;
        }

    for(u2Index1 = 0; u2Index1 < acf->classes_count; ++u2Index1)
        if(((tweak_info*)acf->classes[u2Index1]->tweak)->getAnyStaticShortArray_method) {
            u2 u2Index2 = 0;
            method_info* method = ((tweak_info*)acf->classes[u2Index1]->tweak)->getAnyStaticShortArray_method;

            bytecode_info* sspush = NULL;
            bytecode_info* invokestatic_2 = NULL;
            bytecode_info* aconst_null = NULL;
            bytecode_info* areturn_2 = NULL;

            crt_fieldId = 0;

            method->bytecodes_count = 0;
            method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * ((nb_classes * 6) + 4));
            if(method->bytecodes == NULL) {
                perror("add_getAnyStaticShortArray_bytecodes");
                return -1;
            }

            for(; u2Index2 < acf->classes_count; ++u2Index2)
                if(((tweak_info*)acf->classes[u2Index2]->tweak)->getStaticShortArray_method) {
                    bytecode_info* sload_1_1 = NULL;
                    bytecode_info* push_value = NULL;
                    bytecode_info* sload_1_2 = NULL;
                    bytecode_info* invokestatic_1 = NULL;
                    bytecode_info* areturn_1 = NULL;

                    crt_fieldId += ((tweak_info*)acf->classes[u2Index2]->tweak)->nb_static_short_array_fields;

                    sload_1_1 = create_sload_1_bytecode();
                    if(sload_1_1 == NULL)
                        return -1;

                    if(method->bytecodes_count > 5) {
                        method->bytecodes[method->bytecodes_count - 4] = create_if_scmpge_bytecode(sload_1_1);
                        if(method->bytecodes[method->bytecodes_count - 4] == NULL)
                            return -1;
                    }

                    push_value = create_push_short_value_bytecode(crt_fieldId);
                    if(push_value == NULL)
                        return -1;

                    sload_1_2 = create_sload_1_bytecode();
                    if(sload_1_2 == NULL)
                        return -1;

                    invokestatic_1 = create_invokestatic_bytecode(((tweak_info*)acf->classes[u2Index2]->tweak)->getStaticShortArray_method->this_method);
                    if(invokestatic_1 == NULL)
                        return -1;

                    areturn_1 = create_areturn_bytecode();
                    if(areturn_1 == NULL)
                        return -1;

                    method->bytecodes[method->bytecodes_count++] = sload_1_1;
                    method->bytecodes[method->bytecodes_count++] = push_value;
                    ++method->bytecodes_count;
                    method->bytecodes[method->bytecodes_count++] = sload_1_2;
                    method->bytecodes[method->bytecodes_count++] = invokestatic_1;
                    method->bytecodes[method->bytecodes_count++] = areturn_1;
                }

            sspush = create_push_short_value_bytecode(javacard_framework->ISO7816.sw_unknown_value);
            if(sspush == NULL)
                return -1;

            if(method->bytecodes_count > 5) {
                method->bytecodes[method->bytecodes_count - 4] = create_if_scmpge_bytecode(sspush);
                if(method->bytecodes[method->bytecodes_count - 4] == NULL)
                    return -1;
            }

            invokestatic_2 = create_invokestatic_bytecode(javacard_framework->ISOException.throwIt_ref);
            if(invokestatic_2 == NULL)
                return -1;

            aconst_null = create_aconst_null_bytecode();
            if(aconst_null == NULL)
                return -1;

            areturn_2 = create_areturn_bytecode();
            if(areturn_2 == NULL)
                return -1;

            method->bytecodes[method->bytecodes_count++] = sspush;
            method->bytecodes[method->bytecodes_count++] = invokestatic_2;
            method->bytecodes[method->bytecodes_count++] = aconst_null;
            method->bytecodes[method->bytecodes_count++] = areturn_2;

            method->nargs = 2;
            method->max_stack = 2;
        }

    return 0; 

}


/**
 * \brief Get the total number of static int arrays from every classes in this
 * package.
 * 
 * \param acf The analyzed CAP file.
 *
 * \return Return the number of static int arrays.
 */
static int16_t get_nb_static_int_arrays(analyzed_cap_file* acf) {

    u2 u2Index = 0;
    int16_t count = 0;

    for(; u2Index < acf->classes_count; ++u2Index)
        count += ((tweak_info*)acf->classes[u2Index]->tweak)->nb_static_int_array_fields;

    return count;

}


/**
 * \brief Generate the bytecodes for the getNbStaticIntArrays method for each
 * class with it being added previously.
 *
 * Bytecodes are generated for each valid
 * ((tweak_info*)acf->classes[]->tweak)->getNbStaticIntArrays_method field.
 *
 * \verbatim
// Generated code

return nbStaticIntArrays;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getNbStaticIntArrays_bytecodes(analyzed_cap_file* acf) {  

    int16_t nb_static_int_arrays = get_nb_static_int_arrays(acf);
    u2 u2Index = 0;

    for(; u2Index < acf->classes_count; ++u2Index)
        if(((tweak_info*)acf->classes[u2Index]->tweak)->getNbStaticIntArrays_method) {
            method_info* method = ((tweak_info*)acf->classes[u2Index]->tweak)->getNbStaticIntArrays_method;
            bytecode_info* push_value = NULL;
            bytecode_info* sreturn = NULL;

            push_value = create_push_short_value_bytecode(nb_static_int_arrays);
            if(push_value == NULL)
                return -1;

            sreturn = create_sreturn_bytecode();
            if(sreturn == NULL)
                return -1;

            method->bytecodes_count = 2;
            method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
            if(method->bytecodes == NULL) {
                perror("add_getNbStaticIntArrays_bytecodes");
                return -1;
            }
            method->bytecodes[0] = push_value;
            method->bytecodes[1] = sreturn;

            method->nargs = 1;
            method->max_stack = 1;
        }

    return 0;

}


/**
 * \brief Add the getStaticIntArray method and generate the necessary bytecodes.
 *
 * Static int array fields are numbered starting from the given starting_fieldId
 * parameter.
 *
 * Added method (if any) is put in tweak->getStaticIntArray_method field.
 * 
 * \verbatim
// Generated code

switch(fieldId) {
    case 0:
        return someStaticField;
    case 1:
        return someOtherStaticField;
}
ISOException.throwIt(ISO7816.SW_UNKNOWN);
return null;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param class The class to which the method is added if necessary.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 * \param starting_fieldId The starting number for numbering the int array
 * fields.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getStaticIntArray_method(analyzed_cap_file* acf, class_info* class, javacard_framework_info* javacard_framework, synclib_info* synclib, u2 starting_fieldId) {

    u2 u2Index = 0;
    method_info* method = NULL;
    bytecode_info* sload_0 = NULL;

    bytecode_info* stableswitch = NULL;
    u2 crt_case = 0;
    u2 nb_cases = 0;
    bytecode_info** branches = NULL;

    bytecode_info* sspush = NULL;
    bytecode_info* invokestatic = NULL;
    bytecode_info* aconst_null = NULL;
    bytecode_info* areturn_2 = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if((method = tweak->getStaticIntArray_method = add_method(acf, class, synclib->SyncApplet.getAnyStaticIntArray_sig, METHOD_STATIC|METHOD_PACKAGE)) == NULL)
        return -1;

    sload_0 = create_sload_0_bytecode();
    if(sload_0 == NULL)
        return -1;

    nb_cases = (u2)tweak->nb_static_int_array_fields;
   branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_cases);
    if(branches == NULL) {
        perror("add_getStaticIntArray_method");
        return -1;
    }

    method->bytecodes_count = 2;
    method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * (2 + (nb_cases * 2) + 4));
    if(method->bytecodes == NULL) {
        perror("add_getStaticIntArray_method");
        return -1;
    }

    method->bytecodes[0] = sload_0;
    method->bytecodes[1] = NULL;

    for(; u2Index < class->fields_count; ++u2Index)
        if(((class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) == FIELD_STATIC) && ((class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_INT)) == (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_INT))) {
            bytecode_info* getstatic_a = NULL;
            bytecode_info* areturn_1 = NULL;

            getstatic_a = create_getstatic_T_bytecode(class->fields[u2Index]->this_field);
            if(getstatic_a == NULL)
                return -1;

            areturn_1 = create_areturn_bytecode();
            if(areturn_1 == NULL)
                return -1;

            branches[crt_case++] = getstatic_a;

            method->bytecodes[method->bytecodes_count++] = getstatic_a;
            method->bytecodes[method->bytecodes_count++] = areturn_1;
        }

    sspush = create_push_short_value_bytecode(javacard_framework->ISO7816.sw_unknown_value);
    if(sspush == NULL)
        return -1;

    stableswitch = create_stableswitch_bytecode(starting_fieldId, starting_fieldId + nb_cases - 1, branches, sspush);
    if(stableswitch == NULL)
        return -1; 

    invokestatic = create_invokestatic_bytecode(javacard_framework->ISOException.throwIt_ref);
    if(invokestatic == NULL)
        return -1;

    aconst_null = create_aconst_null_bytecode();
    if(aconst_null == NULL)
        return -1;

    areturn_2 = create_areturn_bytecode();
    if(areturn_2 == NULL)
        return -1;

    method->bytecodes[1] = stableswitch;
    method->bytecodes[method->bytecodes_count++] = sspush;
    method->bytecodes[method->bytecodes_count++] = invokestatic;
    method->bytecodes[method->bytecodes_count++] = aconst_null;
    method->bytecodes[method->bytecodes_count++] = areturn_2;

    method->nargs = 1;
    method->max_stack = 1;

    return 0;

}


/**
 * \brief Add the supporting getStaticIntArray methods and generate bytecodes to
 * appropriately call them to the getAnyStaticIntArray method for each class
 * with it being previously added.
 *
 * Bytecodes are generated for each valid
 * ((tweak_info*)acf->classes[]->tweak)->getAnyStaticIntArray_method field.
 *
 * \verbatim
// Generated code

if(fieldId < (short)2)
    return SomeClass.getStaticIntArray(fieldId);
if(fieldId < (short)3)
    return SomeOtherClass.getStaticIntArray(fieldId);
ISOException.throwIt(ISO7816.SW_UNKNOWN);
return null;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_getAnyStaticIntArray_bytecodes(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib) {

    u2 u2Index1 = 0;
    int16_t crt_fieldId = 0;
    int16_t nb_classes = 0;

    for(; u2Index1 < acf->classes_count; ++u2Index1)
        if(((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_int_array_fields > 0) {
            if(add_getStaticIntArray_method(acf, acf->classes[u2Index1], javacard_framework, synclib, crt_fieldId) == -1)
                return -1;
            crt_fieldId += ((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_int_array_fields;
            ++nb_classes;
        }

    for(u2Index1 = 0; u2Index1 < acf->classes_count; ++u2Index1)
        if(((tweak_info*)acf->classes[u2Index1]->tweak)->getAnyStaticIntArray_method) {
            u2 u2Index2 = 0;
            method_info* method = ((tweak_info*)acf->classes[u2Index1]->tweak)->getAnyStaticIntArray_method;

            bytecode_info* sspush = NULL;
            bytecode_info* invokestatic_2 = NULL;
            bytecode_info* aconst_null = NULL;
            bytecode_info* areturn_2 = NULL;

            crt_fieldId = 0;

            method->bytecodes_count = 0;
            method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * ((nb_classes * 6) + 4));
            if(method->bytecodes == NULL) {
                perror("add_getAnyStaticIntArray_bytecodes");
                return -1;
            }

            for(; u2Index2 < acf->classes_count; ++u2Index2)
                if(((tweak_info*)acf->classes[u2Index2]->tweak)->getStaticIntArray_method) {
                    bytecode_info* sload_1_1 = NULL;
                    bytecode_info* push_value = NULL;
                    bytecode_info* sload_1_2 = NULL;
                    bytecode_info* invokestatic_1 = NULL;
                    bytecode_info* areturn_1 = NULL;

                    crt_fieldId += ((tweak_info*)acf->classes[u2Index2]->tweak)->nb_static_int_array_fields;

                    sload_1_1 = create_sload_1_bytecode();
                    if(sload_1_1 == NULL)
                        return -1;

                    if(method->bytecodes_count > 5) {
                        method->bytecodes[method->bytecodes_count - 4] = create_if_scmpge_bytecode(sload_1_1);
                        if(method->bytecodes[method->bytecodes_count - 4] == NULL)
                            return -1;
                    }

                    push_value = create_push_short_value_bytecode(crt_fieldId);
                    if(push_value == NULL)
                        return -1;

                    sload_1_2 = create_sload_1_bytecode();
                    if(sload_1_2 == NULL)
                        return -1;

                    invokestatic_1 = create_invokestatic_bytecode(((tweak_info*)acf->classes[u2Index2]->tweak)->getStaticIntArray_method->this_method);
                    if(invokestatic_1 == NULL)
                        return -1;

                    areturn_1 = create_areturn_bytecode();
                    if(areturn_1 == NULL)
                        return -1;

                    method->bytecodes[method->bytecodes_count++] = sload_1_1;
                    method->bytecodes[method->bytecodes_count++] = push_value;
                    ++method->bytecodes_count;
                    method->bytecodes[method->bytecodes_count++] = sload_1_2;
                    method->bytecodes[method->bytecodes_count++] = invokestatic_1;
                    method->bytecodes[method->bytecodes_count++] = areturn_1;
                }

            sspush = create_push_short_value_bytecode(javacard_framework->ISO7816.sw_unknown_value);
            if(sspush == NULL)
                return -1;

            if(method->bytecodes_count > 5) {
                method->bytecodes[method->bytecodes_count - 4] = create_if_scmpge_bytecode(sspush);
                if(method->bytecodes[method->bytecodes_count - 4] == NULL)
                    return -1;
            }

            invokestatic_2 = create_invokestatic_bytecode(javacard_framework->ISOException.throwIt_ref);
            if(invokestatic_2 == NULL)
                return -1;

            aconst_null = create_aconst_null_bytecode();
            if(aconst_null == NULL)
                return -1;

            areturn_2 = create_areturn_bytecode();
            if(areturn_2 == NULL)
                return -1;

            method->bytecodes[method->bytecodes_count++] = sspush;
            method->bytecodes[method->bytecodes_count++] = invokestatic_2;
            method->bytecodes[method->bytecodes_count++] = aconst_null;
            method->bytecodes[method->bytecodes_count++] = areturn_2;

            method->nargs = 2;
            method->max_stack = 2;
        }

    return 0; 

}


/**
 * \brief Add the static short currentStaticField field to the given class.
 *
 * A static_fieldref constant pool entry is also added to the constant pool.
 *
 * This field is used for keeping track of the serializing/merging process
 * within a class possessing enough static fields.
 *
 * It should be added if there is a static array or more than one static field
 * within one class.
 *
 * The added field is put in tweak->currentStaticField_field field.
 *
 * \param acf The analyzed CAP file.
 * \param class The class to which the field is added.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_currentStaticField_field(analyzed_cap_file* acf, class_info* class) {

    tweak_info* tweak = (tweak_info*)class->tweak;
    
    tweak->currentStaticField_field = add_short_field(acf, class, FIELD_SHOULD_NOT_SYNCHRONIZE|FIELD_STATIC|FIELD_PRIVATE);
    if(tweak->currentStaticField_field == NULL) {
        perror("add_currentStaticField_field");
        return -1;
    }

    return 0;

}


/**
 * \brief Add the static short currentClass field to the given class.
 *
 * A static_fieldref constant pool entry is also added to the constant pool.
 *
 * This field is used for keeping track of the serializing/merging process
 * within a class possessing a serializeStaticAll and mergeStaticAll methods.
 *
 * It should be added if there is more than one class with a static field.
 *
 * The added field is put in tweak->currentClass_field field.
 *
 * \param acf The analyzed CAP file.
 * \param class The class to which the field is added.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_currentClass_field(analyzed_cap_file* acf, class_info* class) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    tweak->currentClass_field = add_short_field(acf, class, FIELD_SHOULD_NOT_SYNCHRONIZE|FIELD_STATIC|FIELD_PRIVATE);
    if(tweak->currentClass_field == NULL) {
        perror("add_currentClass_field");
        return -1;
    }

    return 0;

}


/**
 * \brief Add the resetStaticPath method and generate the appropriate bytecodes for
 * setting a default value to the supporting static fields (currentStaticField
 * and currentClass) and calling synclib.SyncApplet.resetStaticAll on static
 * reference fields.
 *
 * This method is not added if currentStaticField and
 * currentClass are not present and if there is no static reference fields (thus
 * no call to synclib.SyncApplet.resetStaticAll method).
 *
 * Added method (if any) is put in class->resetStaticPath_method field.
 *
 * \verbatim
// Generated code

currentStaticField = (short)0;
currentClass = (short)0;
if(someStaticField instanceof Synchronizable)
    SyncApplet.resetPathAll(source, someStaticField);
if(someOtherStaticField instanceof Synchronizable)
    SyncApplet.resetPathAll(source, someOtherStaticField);
return;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param class The class to which the method is added.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_resetStaticPath_method(analyzed_cap_file* acf, class_info* class, synclib_info* synclib) {

    method_info* method = NULL;
    bytecode_info** tmp = NULL;
    bytecode_info* vreturn = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if((tweak->currentStaticField_field == NULL) && (tweak->currentClass_field == NULL) && (tweak->nb_static_ref_fields == 0))
        return 0;

    if((method = tweak->resetStaticPath_method = add_method(acf, class, synclib->SyncApplet.resetStaticPathAll_sig, METHOD_STATIC|METHOD_PACKAGE)) == NULL)
        return -1;

    if(tweak->currentStaticField_field) {
        bytecode_info* sconst_0 = NULL;
        bytecode_info* putstatic_s = NULL;

        sconst_0 = create_push_short_value_bytecode(0);
        if(sconst_0 == NULL)
            return -1;

        putstatic_s = create_putstatic_T_bytecode(tweak->currentStaticField_field);
        if(putstatic_s == NULL)
            return -1;

        method->bytecodes_count = 2;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
        if(method->bytecodes == NULL) {
            perror("add_resetStaticPath_method");
            return -1;
        }
        method->bytecodes[0] = sconst_0;
        method->bytecodes[1] = putstatic_s;
    }

    if(tweak->currentClass_field) {
        bytecode_info** tmp = NULL;
        bytecode_info* sconst_0 = NULL;
        bytecode_info* putstatic_s = NULL;

        sconst_0 = create_push_short_value_bytecode(0);
        if(sconst_0 == NULL)
            return -1;

        putstatic_s = create_putstatic_T_bytecode(tweak->currentClass_field);
        if(putstatic_s == NULL)
            return -1;

        tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 2));
        if(tmp == NULL) {
            perror("add_resetStaticPath_method");
            return -1;
        }
        method->bytecodes = tmp;
        method->bytecodes[method->bytecodes_count++] = sconst_0;
        method->bytecodes[method->bytecodes_count++] = putstatic_s;
    }

    if(tweak->nb_static_ref_fields > 0) {
        u2 u2Index = 0;

        for(; u2Index < class->fields_count; ++u2Index)
            if(((class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) == FIELD_STATIC) && ((class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_REF)) == TYPE_DESCRIPTOR_REF)) {
                bytecode_info* getstatic_a_1 = NULL;
                bytecode_info* instanceof = NULL;
                bytecode_info* aload_0 = NULL;
                bytecode_info* getstatic_a_2 = NULL;
                bytecode_info* invokestatic = NULL;

                getstatic_a_1 = create_getstatic_T_bytecode(class->fields[u2Index]->this_field);
                if(getstatic_a_1 == NULL)
                    return -1;

                instanceof = create_instanceof_bytecode(synclib->Synchronizable.interface_ref);
                if(instanceof == NULL)
                    return -1;

                if(method->bytecodes_count > 5) {
                    method->bytecodes[method->bytecodes_count - 4] = create_ifeq_bytecode(getstatic_a_1);
                    if(method->bytecodes[method->bytecodes_count - 4] == NULL)
                        return -1;
                }

                aload_0 = create_aload_0_bytecode();
                if(aload_0 == NULL)
                    return -1;

                getstatic_a_2 = create_getstatic_T_bytecode(class->fields[u2Index]->this_field);
                if(getstatic_a_2 == NULL)
                    return -1;

                invokestatic = create_invokestatic_bytecode(synclib->SyncApplet.resetPathAll_ref);
                if(invokestatic == NULL)
                    return -1;

                tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 6));
                if(tmp == NULL) {
                    perror("add_resetStaticPath_method");
                    return -1;
                }
                method->bytecodes = tmp;
                method->bytecodes[method->bytecodes_count++] = getstatic_a_1;
                method->bytecodes[method->bytecodes_count++] = instanceof;
                method->bytecodes[method->bytecodes_count++] = NULL; // ifeq
                method->bytecodes[method->bytecodes_count++] = aload_0;
                method->bytecodes[method->bytecodes_count++] = getstatic_a_2;
                method->bytecodes[method->bytecodes_count++] = invokestatic;
            }

        method->nargs = 1;
        method->max_stack = 2;
    } else {
        method->nargs = 1;
        method->max_stack = 1;
    }

    vreturn = create_return_bytecode();
    if(vreturn == NULL)
        return -1;

    if(method->bytecodes_count > 5) {
        method->bytecodes[method->bytecodes_count - 4] = create_ifeq_bytecode(vreturn);
        if(method->bytecodes[method->bytecodes_count - 4] == NULL)
            return -1;
    }

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 1));
    if(tmp == NULL) {
        perror("add_resetStaticPath_method");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[method->bytecodes_count++] = vreturn;

    return 0;

}


/**
 * \brief Get the number of classes with at least one static field within the
 * analyzed CAP file.
 *
 * \param acf The analyzed CAP file.
 *
 * \return Return the number of classes.
 */
static int16_t get_nb_classes_with_static(analyzed_cap_file* acf) {

    int16_t nb_classes = 0;
    u2 u2Index = 0;

    for(; u2Index < acf->classes_count; ++u2Index)
        if((((tweak_info*)acf->classes[u2Index]->tweak)->nb_static_fields + ((tweak_info*)acf->classes[u2Index]->tweak)->nb_static_ref_fields + ((tweak_info*)acf->classes[u2Index]->tweak)->nb_static_array_fields) > 0)
            ++nb_classes;

    return nb_classes;

}


/**
 * \brief Generate the bytecodes for the resetStaticPathAll method for each class with
 * it being added previously.
 *
 * currentStaticField and currentClass fields are
 * added if necessary. resetStaticPath methods are also added when necessary.
 *
 * A call to each previously added resetStaticPath method is generated.
 *
 * Bytecodes are generated for each valid
 * ((tweak_info*)acf->classes[]->tweak)->resetStaticPathAll_method field.
 * 
 * \verbatim
// Generated code

SomeClass.resetStaticPath(source);
SomeOtherClass.resetStaticPath(source);
return;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_resetStaticPathAll_bytecodes(analyzed_cap_file* acf, synclib_info* synclib) {

    u2 u2Index1 = 0;

    int16_t nb_classes = get_nb_classes_with_static(acf);

    for(; u2Index1 < acf->classes_count; ++u2Index1) {
        if(((((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_fields + ((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_ref_fields) > 1) || (((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_array_fields > 0))
            if(add_currentStaticField_field(acf, acf->classes[u2Index1]) == -1)
                return -1;

        if((nb_classes > 1) && ((tweak_info*)acf->classes[u2Index1]->tweak)->resetStaticPathAll_method)
            if(add_currentClass_field(acf, acf->classes[u2Index1]) == -1)
                return -1;

        if(add_resetStaticPath_method(acf, acf->classes[u2Index1], synclib) == -1)
            return -1;
    }

    for(u2Index1 = 0; u2Index1 < acf->classes_count; ++u2Index1)
        if(((tweak_info*)acf->classes[u2Index1]->tweak)->resetStaticPathAll_method) {
            method_info* method = ((tweak_info*)acf->classes[u2Index1]->tweak)->resetStaticPathAll_method;
            u2 u2Index2 = 0;
            bytecode_info** tmp = NULL;
            bytecode_info* vreturn = NULL;

            for(; u2Index2 < acf->classes_count; ++u2Index2)
                if(((tweak_info*)acf->classes[u2Index2]->tweak)->resetStaticPath_method) {
                    bytecode_info* aload_1 = NULL;
                    bytecode_info* invokestatic = NULL;

                    aload_1 = create_aload_1_bytecode();
                    if(aload_1 == NULL)
                        return -1;

                    invokestatic = create_invokestatic_bytecode(((tweak_info*)acf->classes[u2Index2]->tweak)->resetStaticPath_method->this_method);
                    if(invokestatic == NULL)
                        return -1;

                    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 2));
                    if(tmp == NULL) {
                        perror("add_resetStaticPathAll_bytecodes");
                        return -1;
                    }
                    method->bytecodes = tmp;
                    method->bytecodes[method->bytecodes_count++] = aload_1;
                    method->bytecodes[method->bytecodes_count++] = invokestatic;
                    method->max_stack = 1;
                }

            vreturn = create_return_bytecode();
            if(vreturn == NULL)
                return -1;

            tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 1));
            if(tmp == NULL) {
                perror("add_resetStaticPathAll_bytecodes");
                return -1;
            }
            method->bytecodes = tmp;
            method->bytecodes[method->bytecodes_count++] = vreturn;

            method->nargs = 2;
        }

    return 0;

}


/**
 * \brief Add the numberStaticPath method and generate the appropriate bytecodes
 * for calling synclib.SyncApplet.numberPathAll method on each static reference
 * field (array fields excluded).
 *
 * The method is added only if the given class has at least one reference field.
 *
 * Added method (if any) is put in class->numberStaticPath_method field.
 *
 * \verbatim
// Generated code

if(someStaticField instanceof Synchronizable)
    SyncApplet.numberPathAll(out, someStaticField);
if(someOtherStaticField instanceof Synchronizable)
    SyncApplet.numberPathAll(out, someOtherStaticField);
return;
\endverbatim
 *
 * \param The analyzed CAP file.
 * \param class The class to which the method is added.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_numberStaticPath_method(analyzed_cap_file* acf, class_info* class, synclib_info* synclib) {

    u2 u2Index = 0;
    method_info* method = NULL;
    bytecode_info** tmp = NULL;
    bytecode_info* vreturn = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(tweak->nb_static_ref_fields == 0)
        return 0;

    if((method = tweak->numberStaticPath_method = add_method(acf, class, synclib->SyncApplet.numberStaticPathAll_sig, METHOD_STATIC|METHOD_PACKAGE)) == NULL)
        return -1;

    for(; u2Index < class->fields_count; ++u2Index)
        if(((class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) == FIELD_STATIC) && ((class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_REF|TYPE_DESCRIPTOR_ARRAY)) == TYPE_DESCRIPTOR_REF)) {
            bytecode_info* getstatic_a_1 = NULL;
            bytecode_info* instanceof = NULL;
            bytecode_info* aload_0 = NULL;
            bytecode_info* getstatic_a_2 = NULL;
            bytecode_info* invokestatic = NULL;

            getstatic_a_1 = create_getstatic_T_bytecode(class->fields[u2Index]->this_field);
            if(getstatic_a_1 == NULL)
                return -1;

            if(method->bytecodes_count > 5) {
                method->bytecodes[method->bytecodes_count - 4] = create_ifeq_bytecode(getstatic_a_1);
                if(method->bytecodes[method->bytecodes_count - 4] == NULL)
                    return -1;
            }

            instanceof = create_instanceof_bytecode(synclib->Synchronizable.interface_ref);
            if(instanceof == NULL)
                return -1;

            aload_0 = create_aload_0_bytecode();
            if(aload_0 == NULL)
                return -1;

            getstatic_a_2 = create_getstatic_T_bytecode(class->fields[u2Index]->this_field);
            if(getstatic_a_2 == NULL)
                return -1;

            invokestatic = create_invokestatic_bytecode(synclib->SyncApplet.numberPathAll_ref);
            if(invokestatic == NULL)
                return -1;

            tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 6));
            if(tmp == NULL) {
                perror("add_numberStaticPath_method");
                return -1;
            }
            method->bytecodes = tmp;
            method->bytecodes[method->bytecodes_count++] = getstatic_a_1;
            method->bytecodes[method->bytecodes_count++] = instanceof;
            method->bytecodes[method->bytecodes_count++] = NULL; // ifeq
            method->bytecodes[method->bytecodes_count++] = aload_0;
            method->bytecodes[method->bytecodes_count++] = getstatic_a_2;
            method->bytecodes[method->bytecodes_count++] = invokestatic;
        }

    vreturn = create_return_bytecode();
    if(vreturn == NULL)
        return -1;

    if(method->bytecodes_count > 5) {
        method->bytecodes[method->bytecodes_count - 4] = create_ifeq_bytecode(vreturn);
        if(method->bytecodes[method->bytecodes_count - 4] == NULL)
            return -1;
    }

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 1));
    if(tmp == NULL) {
        perror("add_numberStaticPath_method");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[method->bytecodes_count++] = vreturn;

    method->nargs = 1;
    method->max_stack = 2;

    return 0;

}


/**
 * \brief Generate bytecodes to call each added numberStaticPath method if any.
 *
 * Bytecodes are generated for each valid
 * ((tweak_info*)acf->classes[]->tweak)->numberStaticPathAll_method field.
 *
 * \verbatim
    // Generated code
    //
    // SomeClass.numberStaticPath(out);
    // SomeOtherClass.numberStaticPath(out);
    // return;
\endverbatim
 * 
 * \param acf The analyzed CAP file.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_numberStaticPathAll_bytecodes(analyzed_cap_file* acf, synclib_info* synclib) {

    u2 u2Index1 = 0;

    for(; u2Index1 < acf->classes_count; ++u2Index1)
        if(((tweak_info*)acf->classes[u2Index1]->tweak)->nb_ref_fields > 0) 
            if(add_numberStaticPath_method(acf, acf->classes[u2Index1], synclib) == -1)
                return -1;

    for(u2Index1 = 0; u2Index1 < acf->classes_count; ++u2Index1)
        if(((tweak_info*)acf->classes[u2Index1]->tweak)->numberStaticPathAll_method) {
            u2 u2Index2 = 0;
            method_info* method = ((tweak_info*)acf->classes[u2Index1]->tweak)->numberStaticPathAll_method;
            bytecode_info** tmp = NULL;
            bytecode_info* vreturn;

            for(; u2Index2 < acf->classes_count; ++u2Index2)
                if(((tweak_info*)acf->classes[u2Index2]->tweak)->numberStaticPath_method) {
                    bytecode_info* aload_1 = NULL;
                    bytecode_info* invokestatic = NULL;

                    aload_1 = create_aload_1_bytecode();
                    if(aload_1 == NULL)
                        return -1;

                    invokestatic = create_invokestatic_bytecode(((tweak_info*)acf->classes[u2Index2]->tweak)->numberStaticPath_method->this_method);
                    if(invokestatic == NULL)
                        return -1;

                    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 2));
                    if(tmp == NULL) {
                        perror("add_numberStaticPath_method");
                        return -1;
                    }
                    method->bytecodes = tmp;
                    method->bytecodes[method->bytecodes_count++] = aload_1;
                    method->bytecodes[method->bytecodes_count++] = invokestatic;
                    method->max_stack = 1;
                }

            vreturn = create_return_bytecode();
            if(vreturn == NULL)
                return -1;

            tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 1));
            if(tmp == NULL) {
                perror("add_numberStaticPath_method");
                return -1;
            }
            method->bytecodes = tmp;
            method->bytecodes[method->bytecodes_count++] = vreturn;

            method->nargs = 2;
        }

    return 0;

}


/**
 * \brief Append bytecodes to the given method for serializing the given static
 * primitive field.
 * 
 * \verbatim
    // Generated code
    // 
    // ...
    // out.write(field);
\endverbatim
 *
 * \param method The method to which the bytecodes are appended.
 * \param synclib Information on the synclib package.
 * \param field An analyzed constant pool entry to the static primitive field.
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_serializeOneStaticField_bytecodes(method_info* method, synclib_info* synclib, constant_pool_entry_info* field) {

    bytecode_info** tmp = NULL;
    bytecode_info* aload_0 = NULL;
    bytecode_info* getstatic_T = NULL;
    bytecode_info* invokevirtual = NULL;

    if(field->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_REF))
        return -1;

    aload_0 = create_aload_0_bytecode();
    if(aload_0 == NULL)
        return -1;

    getstatic_T = create_getstatic_T_bytecode(field);
    if(getstatic_T == NULL)
        return -1;

    if(field->type->types->type & TYPE_DESCRIPTOR_BOOLEAN)
        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_boolean_ref);
    else if(field->type->types->type & TYPE_DESCRIPTOR_BYTE)
        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_byte_ref);
    else if(field->type->types->type & TYPE_DESCRIPTOR_SHORT)
        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_short_ref);
    else
        invokevirtual= create_invokevirtual_bytecode(synclib->Output.write_int_ref);

    if(invokevirtual == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 3));
    if(tmp == NULL) {
        perror("add_serializeOneStaticField_bytecodes");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[method->bytecodes_count++] = aload_0;
    method->bytecodes[method->bytecodes_count++] = getstatic_T;
    method->bytecodes[method->bytecodes_count++] = invokevirtual;

    return 0;

}


/**
 * \brief Append bytecodes to the given method for serializing the given static array
 * field.
 * 
 * starting_fieldId_T is used for identifying the array for duplicate and
 * is incrementing depending on the array type.
 *
 * \verbatim
// Generated code

// ...
out.write(field, (short)starting_fieldId);
\endverbatim
 * \param method The method to which the bytecodes are appended.
 * \param synclib Information on the synclib package.
 * \param field An analyzed constant pool entry to the static array field.
 * \param starting_fieldId_boolean The starting number for numbering a static
 * boolean array field.
 * \param starting_fieldId_byte The starting number for numbering a static byte
 * array field.
 * \param starting_fieldId_short The starting number for numbering a static
 * short array field.
 * \param starting_fieldId_int The starting number for numbering a static int
 * array field.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_serializeOneStaticArrayField_bytecodes(method_info* method, synclib_info* synclib, constant_pool_entry_info* field, u2* starting_fieldId_boolean, u2* starting_fieldId_byte, u2* starting_fieldId_short, u2* starting_fieldId_int) {

    bytecode_info** tmp = NULL;
    bytecode_info* aload_0 = NULL;
    bytecode_info* getstatic_a = NULL;
    bytecode_info* push_value = NULL;
    bytecode_info* invokevirtual = NULL;

    if(!(field->type->types->type & TYPE_DESCRIPTOR_ARRAY))
        return -1;

    aload_0 = create_aload_0_bytecode();
    if(aload_0 == NULL)
        return -1;

    getstatic_a = create_getstatic_T_bytecode(field);
    if(getstatic_a == NULL)
        return -1;

    if(field->type->types->type & TYPE_DESCRIPTOR_BOOLEAN) {
        push_value = create_push_short_value_bytecode((*starting_fieldId_boolean)++);
        if(push_value == NULL)
            return -1;

        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_boolean_array_ref);
    } else if(field->type->types->type & TYPE_DESCRIPTOR_BYTE) {
        push_value = create_push_short_value_bytecode((*starting_fieldId_byte)++);
        if(push_value == NULL)
            return -1;

        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_byte_array_ref);
    } else if(field->type->types->type & TYPE_DESCRIPTOR_SHORT) {
        push_value = create_push_short_value_bytecode((*starting_fieldId_short)++);
        if(push_value == NULL)
            return -1;

        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_short_array_ref);
    } else if(field->type->types->type & TYPE_DESCRIPTOR_INT) {
        push_value = create_push_short_value_bytecode((*starting_fieldId_int)++);
        if(push_value == NULL)
            return -1;

        invokevirtual = create_invokevirtual_bytecode(synclib->Output.write_int_array_ref);
    } else
        return -1;

    if(invokevirtual == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 4));
    if(tmp == NULL) {
        perror("add_serializeOneStaticArrayField_bytecodes");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[method->bytecodes_count++] = aload_0;
    method->bytecodes[method->bytecodes_count++] = getstatic_a;
    method->bytecodes[method->bytecodes_count++] = push_value;
    method->bytecodes[method->bytecodes_count++] = invokevirtual;

    return 0;

}


/**
 * \briefAppend bytecodes to the given method for serializing the given static
 * reference field.
 *
 * The given jump bytecode is used for branching after the call to
 * synclib.SyncApplet.serializeAll method.
 * 
 * \verbatim
    // Generated code
    //
    // ...
    // if(someClassField instanceof Synchronizable)
    //     SyncApplet.serializeAll(out, someClassField);
    // else
    //     out.skipInstance();
\endverbatim
 * \param method The method to which the bytecodes are appended.
 * \param synclib Information on the synclib package.
 * \param field An analyzed constant pool entry to the static reference field.
 * \param jump An analyzed bytecode following the appended bytecodes (can be
 * NULL if not yet available).
 * 
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_serializeOneStaticReferenceField_bytecodes(method_info* method, synclib_info* synclib, constant_pool_entry_info* field, bytecode_info* jump) {

    bytecode_info** tmp = NULL;
    bytecode_info* getstatic_a_1 = NULL;
    bytecode_info* instanceof = NULL;
    bytecode_info* ifeq = NULL;
    bytecode_info* aload_0_1 = NULL;
    bytecode_info* getstatic_a_2 = NULL;
    bytecode_info* invokestatic = NULL;
    bytecode_info* aload_0_2 = NULL;
    bytecode_info* invokevirtual = NULL;

    getstatic_a_1 = create_getstatic_T_bytecode(field);
    if(getstatic_a_1 == NULL)
        return -1;

    instanceof = create_instanceof_bytecode(synclib->Synchronizable.interface_ref);
    if(instanceof == NULL)
        return -1;

    aload_0_1 = create_aload_0_bytecode();
    if(aload_0_1 == NULL)
        return -1;

    getstatic_a_2 = create_getstatic_T_bytecode(field);
    if(getstatic_a_2 == NULL)
        return -1;

    invokestatic = create_invokestatic_bytecode(synclib->SyncApplet.serializeAll_ref);
    if(invokestatic == NULL)
        return -1;

    aload_0_2 = create_aload_0_bytecode();
    if(aload_0_2 == NULL)
        return -1;

    ifeq = create_ifeq_bytecode(aload_0_2);
    if(ifeq == NULL)
        return -1;

    invokevirtual = create_invokevirtual_bytecode(synclib->Output.skipInstance_ref);
    if(invokevirtual == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 9));
    if(tmp == NULL) {
        perror("add_serializeOneStaticReferenceField_bytecodes");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[method->bytecodes_count++] = getstatic_a_1;
    method->bytecodes[method->bytecodes_count++] = instanceof;
    method->bytecodes[method->bytecodes_count++] = ifeq;
    method->bytecodes[method->bytecodes_count++] = aload_0_1;
    method->bytecodes[method->bytecodes_count++] = getstatic_a_2;
    method->bytecodes[method->bytecodes_count++] = invokestatic;
    method->bytecodes[method->bytecodes_count++] = jump;
    method->bytecodes[method->bytecodes_count++] = aload_0_2;
    method->bytecodes[method->bytecodes_count++] = invokevirtual;

    return 0;

}


/**
 * \brief Append bytecodes for reseting the resume mechanism of the input or
 * output and incrementing the currentStaticField field.
 *
 * \verbatim
// Generated code

out.resetResume()
// or (aload_n bytecode does not care)
in.resetResume()
++currentStaticField;
\endverbatim
 *
 * \param method The method to which the bytecodes are appended.
 * \param static_field An analyzed constant pool entry to the currentStaticField
 * field.
 * \param resetResume_ref An analyzed constant pool entry to the resetResume
 * method.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_resetResume_bytecodes(method_info* method, constant_pool_entry_info* static_field, constant_pool_entry_info* resetResume_ref) {

    bytecode_info** tmp = NULL;
    bytecode_info* aload_n = NULL;
    bytecode_info* invokevirtual = NULL;
    bytecode_info* getstatic_s = NULL;
    bytecode_info* sconst_1 = NULL;
    bytecode_info* sadd = NULL;
    bytecode_info* putstatic_s = NULL;

    if(method->flags & METHOD_STATIC)
        aload_n = create_aload_0_bytecode();
    else
        aload_n = create_aload_1_bytecode();
    if(aload_n == NULL)
        return -1;

    invokevirtual = create_invokevirtual_bytecode(resetResume_ref);
    if(invokevirtual == NULL)
        return -1;

    getstatic_s = create_getstatic_T_bytecode(static_field);
    if(getstatic_s == NULL)
        return -1;

    sconst_1 = create_push_short_value_bytecode(1);
    if(sconst_1 == NULL)
        return -1;

    sadd = create_sadd_bytecode();
    if(sadd == NULL)
        return -1;

    putstatic_s = create_putstatic_T_bytecode(static_field);
    if(putstatic_s == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 6));
    if(tmp == NULL) {
        perror("add_resetResume_bytecodes");
        return -1;
    }
    method->bytecodes = tmp;

    method->bytecodes[method->bytecodes_count++] = aload_n;
    method->bytecodes[method->bytecodes_count++] = invokevirtual;
    method->bytecodes[method->bytecodes_count++] = getstatic_s;
    method->bytecodes[method->bytecodes_count++] = sconst_1;
    method->bytecodes[method->bytecodes_count++] = sadd;
    method->bytecodes[method->bytecodes_count++] = putstatic_s;

    return 0;

}


/**
 * \brief Add the serializeStatic method to the given class and generate the
 * appropriate bytecodes for serializing the static fields of the given class.
 *
 * Should be called only if there is at least one static field.
 *
 * starting_fieldId_T parameters are used for static array fields serialization.
 *
 * Added method is put in tweak->serializeStatic_method field.
 *
 * \verbatim
// Generated code

out.write(someFiled);
return;
\endverbatim
 * or
 * \verbatim
// Generated code

switch(currentStaticField) {
    case 0:
        if(someField instanceof Synchronizable)
            SyncApplet.serializeAll(out, someField);
        else
            out.skipInstance();
        out.resetResume();
        ++currentStaticField;
    case 1:
        out.write(someOtherField);
        out.resetResume();
        ++currentStaticField;
    case 2:
        out.write(yetAnotherField, (short)starting_fieldId);
}
return;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param class The class to which the method is added.
 * \param synclib Information on the synclib package.
 * \param starting_fieldId_boolean The starting number for numbering a static
 * boolean array field.
 * \param starting_fieldId_byte The starting number for numbering a static byte
 * array field.
 * \param starting_fieldId_short The starting number for numbering a static
 * short array field.
 * \param starting_fieldId_int The starting number for numbering a static int
 * array field.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_serializeStatic_method(analyzed_cap_file* acf, class_info* class, synclib_info* synclib, u2* starting_fieldId_boolean, u2* starting_fieldId_byte, u2* starting_fieldId_short, u2* starting_fieldId_int) {

    u2 u2Index = 0;
    method_info* method = NULL;
    bytecode_info** tmp = NULL;
    bytecode_info* vreturn = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if((method = tweak->serializeStatic_method = add_method(acf, class, synclib->SyncApplet.serializeStaticAll_sig, METHOD_STATIC|METHOD_PACKAGE)) == NULL)
        return -1;

    vreturn = create_return_bytecode();
    if(vreturn == NULL)
        return -1;

    if((tweak->nb_static_fields + tweak->nb_static_array_fields + tweak->nb_static_ref_fields) == 1) {
        if(tweak->nb_static_fields == 1) {
            for(; u2Index < class->fields_count; ++u2Index)
                if(((class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) == FIELD_STATIC) && !(class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_REF)))
                    break;

            if(add_serializeOneStaticField_bytecodes(method, synclib, class->fields[u2Index]->this_field) == -1)
                return -1;

            if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_INT)
                method->max_stack = 3;
            else
                method->max_stack = 2;
        } else if(tweak->nb_static_array_fields == 1) {
            for(; u2Index < class->fields_count; ++u2Index)
                if(((class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) == FIELD_STATIC) && ((class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_REF)) == TYPE_DESCRIPTOR_ARRAY))
                    break;

            if(add_serializeOneStaticArrayField_bytecodes(method, synclib, class->fields[u2Index]->this_field, starting_fieldId_boolean, starting_fieldId_byte, starting_fieldId_short, starting_fieldId_int) == -1)
                return -1;

            method->max_stack = 3;
        } else {
            for(; u2Index < class->fields_count; ++u2Index)
                if(((class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) == FIELD_STATIC) && ((class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_REF)) == TYPE_DESCRIPTOR_REF))
                    break;

            if(add_serializeOneStaticReferenceField_bytecodes(method, synclib, class->fields[u2Index]->this_field, vreturn) == -1)
                return -1;

            method->max_stack = 2;
        }
    } else {
        char was_int_serialized = 0;
        bytecode_info* getstatic_s = NULL;

        bytecode_info* stableswitch = NULL;
        bytecode_info** branches = NULL;
        u2 nb_cases = tweak->nb_static_fields + tweak->nb_static_array_fields + tweak->nb_static_ref_fields;
        u2 crt_case = 0;

        getstatic_s = create_getstatic_T_bytecode(tweak->currentStaticField_field);
        if(getstatic_s == NULL)
            return -1;

        branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_cases);
        if(branches == NULL) {
            perror("add_serializeStatic_method");
            return -1;
        }

        method->bytecodes_count = 2;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
        if(method->bytecodes == NULL) {
            perror("add_serializeStatic_method");
            return -1;
        }

        method->bytecodes[0] = getstatic_s;
        method->bytecodes[1] = NULL; // stableswitch

        for(; u2Index < class->fields_count; ++u2Index)
            if(((class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) == FIELD_STATIC)) {
                bytecode_info* a_goto = NULL;

                if(!(class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_REF))) {
                    if(add_serializeOneStaticField_bytecodes(method, synclib, class->fields[u2Index]->this_field))
                        return -1;

                    if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_INT)
                        was_int_serialized = 1;

                    branches[crt_case++] = method->bytecodes[method->bytecodes_count - 3];
                } else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_ARRAY) {
                    if(add_serializeOneStaticArrayField_bytecodes(method, synclib, class->fields[u2Index]->this_field, starting_fieldId_boolean, starting_fieldId_byte, starting_fieldId_short, starting_fieldId_int) == -1)
                        return -1;

                    branches[crt_case++] = method->bytecodes[method->bytecodes_count - 4];
                } else {
                    a_goto = create_goto_bytecode(NULL);
                    if(a_goto == NULL)
                        return -1;

                    if(add_serializeOneStaticReferenceField_bytecodes(method, synclib, class->fields[u2Index]->this_field, a_goto) == -1)
                        return -1;

                    branches[crt_case++] = method->bytecodes[method->bytecodes_count - 9];
                }
                if(crt_case < nb_cases)
                    if(add_resetResume_bytecodes(method, tweak->currentStaticField_field, synclib->Output.resetResume_ref) == -1)
                        return -1;
                if(a_goto)
                    a_goto->branch = method->bytecodes[method->bytecodes_count - 6];
            }

        stableswitch = create_stableswitch_bytecode(0, nb_cases - 1, branches, vreturn);
        if(stableswitch == NULL)
            return -1;

        method->bytecodes[1] = stableswitch;

        if(was_int_serialized || (tweak->nb_static_array_fields > 0))
            method->max_stack = 3;
        else
            method->max_stack = 2;
    }

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 1));
    if(tmp == NULL) {
        perror("add_serializeStatic_method");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[method->bytecodes_count++] = vreturn;

    method->nargs = 1;

    return 0;

}


/**
 * \brief Generate bytecodes to call each added serializeStatic method if any.
 *
 * Bytecodes are generated for each valid
 * ((tweak_info*)acf->classes[]->tweak)->serializeStaticAll_method field. 
 * 
 * \verbatim
// Generated code

switch(currentClass) {
    case 0:
        SomeClass.serializeStatic(out);
        out.resetResume();
        ++currentClass;
    case 1:
        SomeOtherClass.serializeStatic(out);
}
return;
\endverbatim
 * or
 * \verbatim
// Generated code

SomeClass.serializeStatic(out);
return;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_serializeStaticAll_bytecodes(analyzed_cap_file* acf, synclib_info* synclib) {

    u2 u2Index1 = 0;
    u2 starting_fieldId_boolean = 0;
    u2 starting_fieldId_byte = 0;
    u2 starting_fieldId_short = 0;
    u2 starting_fieldId_int = 0;

    u2 nb_classes = get_nb_classes_with_static(acf);

    for(;u2Index1 < acf->classes_count; ++u2Index1)
        if((((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_fields + ((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_array_fields + ((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_ref_fields) > 0)
            if(add_serializeStatic_method(acf, acf->classes[u2Index1], synclib, &starting_fieldId_boolean, &starting_fieldId_byte, &starting_fieldId_short, &starting_fieldId_int) == -1)
                return -1;

    if(nb_classes > 1) {
        for(u2Index1 = 0; u2Index1 < acf->classes_count; ++u2Index1)
            if(((tweak_info*)acf->classes[u2Index1]->tweak)->serializeStaticAll_method) {
                u2 u2Index2 = 0;
                method_info* method = ((tweak_info*)acf->classes[u2Index1]->tweak)->serializeStaticAll_method;
                bytecode_info** tmp = NULL;

                bytecode_info* getstatic_s = NULL;

                bytecode_info* stableswitch = NULL;
                bytecode_info** branches = NULL;
                u2 crt_case = 0;

                bytecode_info* vreturn = NULL;

                getstatic_s = create_getstatic_T_bytecode(((tweak_info*)acf->classes[u2Index1]->tweak)->currentClass_field);
                if(getstatic_s == NULL)
                    return -1;

                branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_classes);
                if(branches == NULL) {
                    perror("add_serializeStaticAll_bytecodes");
                    return -1;
                }

                method->bytecodes_count = 2;
                method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
                if(method->bytecodes == NULL) {
                    perror("add_serializeStaticAll_bytecodes");
                    return -1;
                }
                method->bytecodes[0] = getstatic_s;

                for(; u2Index2 < acf->classes_count; ++u2Index2)
                    if(((tweak_info*)acf->classes[u2Index2]->tweak)->serializeStatic_method) {
                        bytecode_info* aload_1 = NULL;
                        bytecode_info* invokestatic = NULL;

                        aload_1 = create_aload_1_bytecode();
                        if(aload_1 == NULL)
                            return -1;

                        invokestatic = create_invokestatic_bytecode(((tweak_info*)acf->classes[u2Index2]->tweak)->serializeStatic_method->this_method);
                        if(invokestatic == NULL)
                            return -1;

                        tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 2));
                        if(tmp == NULL) {
                            perror("add_serializeStaticAll_bytecodes");
                            return -1;
                        }
                        method->bytecodes = tmp;
                        method->bytecodes[method->bytecodes_count++] = aload_1;
                        method->bytecodes[method->bytecodes_count++] = invokestatic;

                        branches[crt_case++] = aload_1;

                        if(crt_case < nb_classes)
                            if(add_resetResume_bytecodes(method, ((tweak_info*)acf->classes[u2Index1]->tweak)->currentClass_field, synclib->Output.resetResume_ref) == -1)
                                return -1;
                    }

                vreturn = create_return_bytecode();
                if(vreturn == NULL)
                    return -1;

                stableswitch = create_stableswitch_bytecode(0, nb_classes - 1, branches, vreturn);
                if(stableswitch == NULL)
                    return -1;

                tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 1));
                if(tmp == NULL) {
                    perror("add_serializeStaticAll_bytecodes");
                    return -1;
                }
                method->bytecodes = tmp;
                method->bytecodes[1] = stableswitch;
                method->bytecodes[method->bytecodes_count++] = vreturn;

                method->nargs = 2;
                method->max_stack = 1;
            }
    } else {
        for(u2Index1 = 0; u2Index1 < acf->classes_count; ++u2Index1)
            if(((tweak_info*)acf->classes[u2Index1]->tweak)->serializeStaticAll_method) {
                u2 u2Index2 = 0;
                method_info* method = ((tweak_info*)acf->classes[u2Index1]->tweak)->serializeStaticAll_method;
                bytecode_info* aload_1 = NULL;
                bytecode_info* invokestatic = NULL;
                bytecode_info* vreturn = NULL;

                if(nb_classes == 1) {
                    for(; u2Index2 < acf->classes_count; ++u2Index2)
                        if(((tweak_info*)acf->classes[u2Index2]->tweak)->serializeStatic_method)
                            break;

                    aload_1 = create_aload_1_bytecode();
                    if(aload_1 == NULL)
                        return -1;

                    invokestatic = create_invokestatic_bytecode(((tweak_info*)acf->classes[u2Index2]->tweak)->serializeStatic_method->this_method);
                    if(invokestatic == NULL)
                        return -1;
                }

                vreturn = create_return_bytecode();
                if(vreturn == NULL)
                    return -1;

                method->bytecodes_count = nb_classes == 1 ? 3 : 1;
                method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
                if(method->bytecodes == NULL) {
                    perror("add_serializeStaticAll_bytecodes");
                    return -1;
                }
                if(nb_classes == 1) {
                    method->bytecodes[0] = aload_1;
                    method->bytecodes[1] = invokestatic;
                    method->bytecodes[2] = vreturn;
                } else {
                    method->bytecodes[0] = vreturn;
                }

                method->nargs = 2;
                if(nb_classes == 1)
                    method->max_stack = 1;
            }
    }

    return 0;

}


/**
 * \brief Append to the given method bytecodes for merging the given static primitive
 * field.
 *
 * \verbatim
// Generated code

// ...
someField = in.read(someField);
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param synclib Information on the synclib package.
 * \param field An analyzed constant pool entry to the static primitive field.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_mergeOneStaticField_bytecodes(method_info* method, synclib_info* synclib, constant_pool_entry_info* field) {

    bytecode_info** tmp = NULL;
    bytecode_info* aload_0 = NULL;
    bytecode_info* getstatic_T = NULL;
    bytecode_info* invokevirtual = NULL;
    bytecode_info* putstatic_T = NULL;

    if(field->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_REF))
        return -1;

    aload_0 = create_aload_0_bytecode();
    if(aload_0 == NULL)
        return -1;

    getstatic_T = create_getstatic_T_bytecode(field);
    if(getstatic_T == NULL)
        return -1;

    if(field->type->types->type & TYPE_DESCRIPTOR_BOOLEAN)
        invokevirtual = create_invokevirtual_bytecode(synclib->Input.read_boolean_ref);
    else if(field->type->types->type & TYPE_DESCRIPTOR_BYTE)
        invokevirtual = create_invokevirtual_bytecode(synclib->Input.read_byte_ref);
    else if(field->type->types->type & TYPE_DESCRIPTOR_SHORT)
        invokevirtual = create_invokevirtual_bytecode(synclib->Input.read_short_ref);
    else
        invokevirtual = create_invokevirtual_bytecode(synclib->Input.read_int_ref);

    if(invokevirtual == NULL)
        return -1;

    putstatic_T = create_putstatic_T_bytecode(field);
    if(putstatic_T == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 4));
    if(tmp == NULL) {
        perror("add_mergeOneStaticField_bytecodes");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[method->bytecodes_count++] = aload_0;
    method->bytecodes[method->bytecodes_count++] = getstatic_T;
    method->bytecodes[method->bytecodes_count++] = invokevirtual;
    method->bytecodes[method->bytecodes_count++] = putstatic_T;

    return 0;

}


/**
 * \brief Append to the given method bytecodes for merging the given static
 * array field.
 *
 * \verbatim
// Generated code

// ...
someField = in.checkArray(someField);
in.resetResume();
++currentStaticField;
in.read(someField);
\endverbatim
 *
 * \param method The method to which the bytecodes are added.
 * \param synclib Information on the synclib package.
 * \param field An analyzed constant pool entry to the static array field.
 * \param currentStaticField_field An analyzed constant pool entry to the
 * currentStaticField field.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_mergeOneStaticArrayField_bytecodes(method_info* method, synclib_info* synclib, constant_pool_entry_info* field, constant_pool_entry_info* currentStaticField_field) {

    bytecode_info** tmp = NULL;
    bytecode_info* aload_0_1 = NULL;
    bytecode_info* getstatic_T_1 = NULL;
    bytecode_info* invokevirtual_1 = NULL;
    bytecode_info* putstatic_T = NULL;
    bytecode_info* aload_0_2 = NULL;
    bytecode_info* getstatic_T_2 = NULL;
    bytecode_info* invokevirtual_2 = NULL;

    if(!(field->type->types->type & TYPE_DESCRIPTOR_ARRAY))
        return -1;

    aload_0_1 = create_aload_0_bytecode();
    if(aload_0_1 == NULL)
        return -1;

    getstatic_T_1 = create_getstatic_T_bytecode(field);
    if(getstatic_T_1 == NULL)
        return -1;

    if(field->type->types->type & TYPE_DESCRIPTOR_BOOLEAN)
        invokevirtual_1 = create_invokevirtual_bytecode(synclib->Input.check_boolean_array_ref);
    else if(field->type->types->type & TYPE_DESCRIPTOR_BYTE)
        invokevirtual_1 = create_invokevirtual_bytecode(synclib->Input.check_byte_array_ref);
    else if(field->type->types->type & TYPE_DESCRIPTOR_SHORT)
        invokevirtual_1 = create_invokevirtual_bytecode(synclib->Input.check_short_array_ref);
    else if(field->type->types->type & TYPE_DESCRIPTOR_INT)
        invokevirtual_1 = create_invokevirtual_bytecode(synclib->Input.check_int_array_ref);
    else
        return -1;

    if(invokevirtual_1 == NULL)
        return -1;

    putstatic_T = create_putstatic_T_bytecode(field);
    if(putstatic_T == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 4));
    if(tmp == NULL) {
        perror("add_mergeOneStaticArrayField_bytecodes");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[method->bytecodes_count++] = aload_0_1;
    method->bytecodes[method->bytecodes_count++] = getstatic_T_1;
    method->bytecodes[method->bytecodes_count++] = invokevirtual_1;
    method->bytecodes[method->bytecodes_count++] = putstatic_T;

    if(add_resetResume_bytecodes(method, currentStaticField_field, synclib->Input.resetResume_ref) == -1)
        return -1;

    aload_0_2 = create_aload_0_bytecode();
    if(aload_0_2 == NULL)
        return -1;

    getstatic_T_2 = create_getstatic_T_bytecode(field);
    if(getstatic_T_2 == NULL)
        return -1;

    if(field->type->types->type & TYPE_DESCRIPTOR_BOOLEAN)
        invokevirtual_2 = create_invokevirtual_bytecode(synclib->Input.read_boolean_array_ref);
    else if(field->type->types->type & TYPE_DESCRIPTOR_BYTE)
        invokevirtual_2 = create_invokevirtual_bytecode(synclib->Input.read_byte_array_ref);
    else if(field->type->types->type & TYPE_DESCRIPTOR_SHORT)
        invokevirtual_2 = create_invokevirtual_bytecode(synclib->Input.read_short_array_ref);
    else
        invokevirtual_2 = create_invokevirtual_bytecode(synclib->Input.read_int_array_ref);

    if(invokevirtual_2 == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 3));
    if(tmp == NULL) {
        perror("add_mergeOneStaticArrayField_bytecodes");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[method->bytecodes_count++] = aload_0_2;
    method->bytecodes[method->bytecodes_count++] = getstatic_T_2;
    method->bytecodes[method->bytecodes_count++] = invokevirtual_2;

    return 0;

}


/**
 * \brief Add an exception handler to the CAP file exception handlers array and
 * to the given class exception handler arrays.
 *
 * \param acf The analyzed CAP file.
 * \param method The method to which the added exception handler pertains.
 * \param catch_type An analyzed constant pool entry to the class type of the
 * catch block.
 * \param start An analyzed bytecode starting the try block.
 * \param end An analyzed bytecode ending the try block.
 * \param handler An analyzed bytecode starting the handler.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_exception_handler(analyzed_cap_file* acf, method_info* method, constant_pool_entry_info* catch_type, bytecode_info* start, bytecode_info* end, bytecode_info* handler) {

    exception_handler_info** tmp = NULL;
    exception_handler_info* new = (exception_handler_info*)malloc(sizeof(exception_handler_info));
    if(new == NULL) {
        perror("add_exception_handler");
        return -1;
    }

    new->stop_bit = 1;
    new->my_index = acf->exception_handlers_count;
    new->try_in = method;
    new->start = start;
    new->end = end;
    new->handler = handler;
    new->catch_type = catch_type;

    tmp = (exception_handler_info**)realloc(method->exception_handlers, sizeof(exception_handler_info*) * (method->exception_handlers_count + 1));
    if(tmp == NULL) {
        perror("add_exception_handler");
        return -1;
    }
    method->exception_handlers = tmp;
    method->exception_handlers[method->exception_handlers_count++] = new;

    tmp = (exception_handler_info**)realloc(acf->exception_handlers, sizeof(exception_handler_info*) * (acf->exception_handlers_count + 1));
    if(tmp == NULL) {
        perror("add_exception_handler");
        return -1;
    }
    acf->exception_handlers = tmp;
    acf->exception_handlers[acf->exception_handlers_count++] = new;

    return 0;

}


/**
 * \brief Append to the given method bytecodes for merging the given static
 * reference field.
 *
 * An exception handler is added.
 *
 * The given jump bytecode is used for branching after one of the call to
 * synclib.SyncApplet.mergeAll method.
 *
 * \verbatim
// Generated code

...
try {
    if(someField instance Synchronizable)
        someField = (SomeClass)SyncApplet.mergeAll(in, someField);
    else
        someField = (SomeClass)SyncApplet.mergeAll(in, null);
}
catch(UserException e) {
    if(e.getReason() == Input.NEW_INSTANCE) {
        someField = (SomeClass)in.newlyCreatedInstance;
        SyncApplet.mergeAll(in, someField);
    } else if(e.getReason() != Input.SKIP_INSTANCE)
        throw e;
}
\endverbatim
 * 
 * \param acf The analyzed CAP file.
 * \param method The method to which the bytecodes are appended.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 * \param field An analyzed constant pool entry to the static reference field.
 * \param jump An analyzed bytecode following the appended bytecodes (can be
 * NULL if not yet available).
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_mergeOneStaticReferenceField_bytecodes(analyzed_cap_file* acf, method_info* method, javacard_framework_info* javacard_framework, synclib_info* synclib, constant_pool_entry_info* field, bytecode_info* jump) {

    bytecode_info** tmp = NULL;
    bytecode_info* getstatic_a_1 = NULL;
    bytecode_info* instanceof = NULL;
    bytecode_info* ifeq = NULL;
    bytecode_info* aload_0_1 = NULL;
    bytecode_info* getstatic_a_2 = NULL;
    bytecode_info* invokestatic_1 = NULL;
    bytecode_info* checkcast_1 = NULL;
    bytecode_info* putstatic_a_1 = NULL;
    bytecode_info* goto_1 = NULL;
    bytecode_info* aload_0_2 = NULL;
    bytecode_info* aconst_null = NULL;
    bytecode_info* invokestatic_2 = NULL;
    bytecode_info* checkcast_2 = NULL;
    bytecode_info* putstatic_a_2 = NULL;
    bytecode_info* goto_2 = NULL;
    bytecode_info* astore_1 = NULL;
    bytecode_info* aload_1_1 = NULL;
    bytecode_info* invokevirtual_1 = NULL;
    bytecode_info* push_value_1 = NULL;
    bytecode_info* if_scmpne = NULL;
    bytecode_info* aload_0_3 = NULL;
    bytecode_info* getfield_a = NULL;
    bytecode_info* checkcast_3 = NULL;
    bytecode_info* putstatic_a_3 = NULL;
    bytecode_info* aload_0_4 = NULL;
    bytecode_info* getstatic_a_3 = NULL;
    bytecode_info* invokestatic_3 = NULL;
    bytecode_info* pop = NULL;
    bytecode_info* goto_3 = NULL;
    bytecode_info* aload_1_2 = NULL;
    bytecode_info* invokevirtual_2 = NULL;
    bytecode_info* push_value_2 = NULL;
    bytecode_info* if_scmpeq = NULL;
    bytecode_info* aload_1_3 = NULL;
    bytecode_info* athrow = NULL;

    getstatic_a_1 = create_getstatic_T_bytecode(field);
    if(getstatic_a_1 == NULL)
        return -1;

    instanceof = create_instanceof_bytecode(synclib->Synchronizable.interface_ref);
    if(instanceof == NULL)
        return -1;

    aload_0_1 = create_aload_0_bytecode();
    if(aload_0_1 == NULL)
        return -1;

    getstatic_a_2 = create_getstatic_T_bytecode(field);
    if(getstatic_a_2 == NULL)
        return -1;

    invokestatic_1 = create_invokestatic_bytecode(synclib->SyncApplet.mergeAll_ref);
    if(invokestatic_1 == NULL)
        return -1;

    checkcast_1 = create_checkcast_bytecode(field->type->types->ref);
    if(checkcast_1 == NULL)
        return -1;

    putstatic_a_1 = create_putstatic_T_bytecode(field);
    if(putstatic_a_1 == NULL)
        return -1;

    goto_1 = create_goto_bytecode(jump);
    if(goto_1 == NULL)
        return -1;

    aload_0_2 = create_aload_0_bytecode();
    if(aload_0_2 == NULL)
        return -1;

    ifeq = create_ifeq_bytecode(aload_0_2);
    if(ifeq == NULL)
        return -1;

    aconst_null = create_aconst_null_bytecode();
    if(aconst_null == NULL)
        return -1;

    invokestatic_2 = create_invokestatic_bytecode(synclib->SyncApplet.mergeAll_ref);
    if(invokestatic_2 == NULL)
        return -1;

    checkcast_2 = create_checkcast_bytecode(field->type->types->ref);
    if(checkcast_2 == NULL)
        return -1;

    putstatic_a_2 = create_putstatic_T_bytecode(field);
    if(putstatic_a_2 == NULL)
        return -1;

    goto_2 = create_goto_bytecode(jump);
    if(goto_2 == NULL)
        return -1;

    astore_1 = create_astore_1_bytecode();
    if(astore_1 == NULL)
        return -1;

    aload_1_1 = create_aload_1_bytecode();
    if(aload_1_1 == NULL)
        return -1;

    invokevirtual_1 = create_invokevirtual_bytecode(javacard_framework->UserException.getReason_ref);
    if(invokevirtual_1 == NULL)
        return -1;

    push_value_1 = create_push_short_value_bytecode(synclib->Input.new_instance_value);
    if(push_value_1 == NULL)
        return -1;

    aload_0_3 = create_aload_0_bytecode();
    if(aload_0_3 == NULL)
        return -1;

    getfield_a = create_getfield_T_bytecode(synclib->Input.newlyCreatedInstance_ref);
    if(getfield_a == NULL)
        return -1;

    checkcast_3 = create_checkcast_bytecode(field->type->types->ref);
    if(checkcast_3 == NULL)
        return -1;

    putstatic_a_3 = create_putstatic_T_bytecode(field);
    if(putstatic_a_3 == NULL)
        return -1;

    aload_0_4 = create_aload_0_bytecode();
    if(aload_0_4 == NULL)
        return -1;

    getstatic_a_3 = create_getstatic_T_bytecode(field);
    if(getstatic_a_3 == NULL)
        return -1;

    invokestatic_3 = create_invokestatic_bytecode(synclib->SyncApplet.mergeAll_ref);
    if(invokestatic_3 == NULL)
        return -1;

    pop = create_pop_bytecode();
    if(pop == NULL)
        return -1;

    goto_3 = create_goto_bytecode(jump);
    if(goto_3 == NULL)
        return -1;

    aload_1_2 = create_aload_1_bytecode();
    if(aload_1_2 == NULL)
        return -1;

    if_scmpne = create_if_scmpne_bytecode(aload_1_2);
    if(if_scmpne == NULL)
        return -1;

    invokevirtual_2 = create_invokevirtual_bytecode(javacard_framework->UserException.getReason_ref);
    if(invokevirtual_2 == NULL)
        return -1;

    push_value_2 = create_push_short_value_bytecode(synclib->Input.skip_instance_value);
    if(push_value_2 == NULL)
        return -1;

    if_scmpeq = create_if_scmpeq_bytecode(jump);
    if(if_scmpeq == NULL)
        return -1;

    aload_1_3 = create_aload_1_bytecode();
    if(aload_1_3 == NULL)
        return -1;

    athrow = create_athrow_bytecode();
    if(athrow == NULL)
        return -1;

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 35));
    if(tmp == NULL) {
        perror("add_mergeOneStaticReferenceField_bytecodes");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[method->bytecodes_count++] = getstatic_a_1;
    method->bytecodes[method->bytecodes_count++] = instanceof;
    method->bytecodes[method->bytecodes_count++] = ifeq;
    method->bytecodes[method->bytecodes_count++] = aload_0_1;
    method->bytecodes[method->bytecodes_count++] = getstatic_a_2;
    method->bytecodes[method->bytecodes_count++] = invokestatic_1;
    method->bytecodes[method->bytecodes_count++] = checkcast_1;
    method->bytecodes[method->bytecodes_count++] = putstatic_a_1;
    method->bytecodes[method->bytecodes_count++] = goto_1;
    method->bytecodes[method->bytecodes_count++] = aload_0_2;
    method->bytecodes[method->bytecodes_count++] = aconst_null;
    method->bytecodes[method->bytecodes_count++] = invokestatic_2;
    method->bytecodes[method->bytecodes_count++] = checkcast_2;
    method->bytecodes[method->bytecodes_count++] = putstatic_a_2;
    method->bytecodes[method->bytecodes_count++] = goto_2;
    method->bytecodes[method->bytecodes_count++] = astore_1;
    method->bytecodes[method->bytecodes_count++] = aload_1_1;
    method->bytecodes[method->bytecodes_count++] = invokevirtual_1;
    method->bytecodes[method->bytecodes_count++] = push_value_1;
    method->bytecodes[method->bytecodes_count++] = if_scmpne;
    method->bytecodes[method->bytecodes_count++] = aload_0_3;
    method->bytecodes[method->bytecodes_count++] = getfield_a;
    method->bytecodes[method->bytecodes_count++] = checkcast_3;
    method->bytecodes[method->bytecodes_count++] = putstatic_a_3;
    method->bytecodes[method->bytecodes_count++] = aload_0_4;
    method->bytecodes[method->bytecodes_count++] = getstatic_a_3;
    method->bytecodes[method->bytecodes_count++] = invokestatic_3;
    method->bytecodes[method->bytecodes_count++] = pop;
    method->bytecodes[method->bytecodes_count++] = goto_3;
    method->bytecodes[method->bytecodes_count++] = aload_1_2;
    method->bytecodes[method->bytecodes_count++] = invokevirtual_2;
    method->bytecodes[method->bytecodes_count++] = push_value_2;
    method->bytecodes[method->bytecodes_count++] = if_scmpeq;
    method->bytecodes[method->bytecodes_count++] = aload_1_3;
    method->bytecodes[method->bytecodes_count++] = athrow;

    if(add_exception_handler(acf, method, javacard_framework->UserException.class_ref, getstatic_a_1, goto_2, astore_1) == -1)
        return -1;

    return 0;

}


/**
 * \brief Add the mergeStatic method to the given class and generate the appropriate
 * bytecodes for merging each static fields of the given class.
 *
 * Should be called only if there is at least one static field.
 *
 * Added method is put in tweak->mergeStatic_method field.
 *
 * \verbatim
// Generated code

someField = in.read(someField);
return;
\endverbatim
 * or
 * \verbatim
// Generated code

switch(currentStaticField) {
    case 0:
        try {
            if(someField instanceof Synchronizable)
                someField = (SomeField)SyncApplet.mergeAll(in, (Synchronizable)someField);
            else
                someField = (someField)SyncApplet.mergeAll(in, null);
        }
        catch(UserException e) {
            if(e.getReason() == Input.NEW_INSTANCE) {
                someField = (SomeField)in.newlyCreatedInstance;
                SyncApplet.mergeAll(in, someField);
            } else if(e.getReason() != Input.SKIP_INSTANCE)
                throw e;
        }
        in.resetResume();
        ++currentStaticField;
    case 1:
        someOtherField = in.read(someOtherField);
        in.resetResume();
        ++currentStaticField;
    case 2:
        yetAnotherField = in.checkArray(yetAnotherField);
        in.resetResume();
        ++currentStaticField;
    case 3:
        in.read(yetAnotherField);
}
return;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param class The class to which the method is added.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_mergeStatic_method(analyzed_cap_file* acf, class_info* class, javacard_framework_info* javacard_framework, synclib_info* synclib) {

    u2 u2Index = 0;
    method_info* method = NULL;
    bytecode_info** tmp = NULL;
    bytecode_info* vreturn = NULL;

    tweak_info* tweak = (tweak_info*)class->tweak;

    if((method = tweak->mergeStatic_method = add_method(acf, class, synclib->SyncApplet.mergeStaticAll_sig, METHOD_STATIC|METHOD_PACKAGE)) == NULL)
        return -1;

    vreturn = create_return_bytecode();
    if(vreturn == NULL)
        return -1;

    if(((tweak->nb_static_fields + tweak->nb_static_array_fields + tweak->nb_static_ref_fields) == 1) && (tweak->nb_static_array_fields == 0)) {
        if(tweak->nb_static_fields == 1) {
            for(; u2Index < class->fields_count; ++u2Index)
                if(((class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) == FIELD_STATIC) && !(class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_REF)))
                    break;

            if(add_mergeOneStaticField_bytecodes(method, synclib, class->fields[u2Index]->this_field) == -1)
                return -1;

            if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_INT)
                method->max_stack = 3;
            else
                method->max_stack = 2;
        } else {
            for(; u2Index < class->fields_count; ++u2Index)
                if(((class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) == FIELD_STATIC) && ((class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_REF)) == TYPE_DESCRIPTOR_REF))
                    break;

            if(add_mergeOneStaticReferenceField_bytecodes(acf, method, javacard_framework, synclib, class->fields[u2Index]->this_field, vreturn) == -1)
                return -1;

            method->max_stack = 2;
            method->max_locals = 1;
        }
    } else {
        char was_int_merged = 0;
        bytecode_info* getstatic_s = NULL;

        bytecode_info* stableswitch = NULL;
        bytecode_info** branches = NULL;
        u2 nb_cases = tweak->nb_static_fields + (2 * tweak->nb_static_array_fields) + tweak->nb_static_ref_fields;
        u2 crt_case = 0;

        getstatic_s = create_getstatic_T_bytecode(tweak->currentStaticField_field);
        if(getstatic_s == NULL)
            return -1;

        branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_cases);
        if(branches == NULL) {
            perror("add_mergeStatic_method");
            return -1;
        }

        stableswitch = create_stableswitch_bytecode(0, nb_cases - 1, branches, vreturn);
        if(stableswitch == NULL)
            return -1;

        method->bytecodes_count = 2;
        method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
        if(method->bytecodes == NULL) {
            perror("add_mergeStatic_method");
            return -1;
        }
        method->bytecodes[0] = getstatic_s;
        method->bytecodes[1] = stableswitch;

        for(; u2Index < class->fields_count; ++u2Index)
            if((class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_STATIC|FIELD_SHOULD_NOT_SYNCHRONIZE)) == FIELD_STATIC) {
                u2 crt_bytecodes_count = method->bytecodes_count;

                if(!(class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_ARRAY|TYPE_DESCRIPTOR_REF))) {
                    if(add_mergeOneStaticField_bytecodes(method, synclib, class->fields[u2Index]->this_field) == -1)
                        return -1;

                    if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_INT)
                        was_int_merged = 1;
                } else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_ARRAY) {
                    if(add_mergeOneStaticArrayField_bytecodes(method, synclib, class->fields[u2Index]->this_field, tweak->currentStaticField_field) == -1)
                        return -1;
                } else {
                    if(add_mergeOneStaticReferenceField_bytecodes(acf, method, javacard_framework, synclib, class->fields[u2Index]->this_field, (crt_case + 1) < nb_cases ? NULL : vreturn) == -1)
                        return -1;
                }

                branches[crt_case++] = method->bytecodes[crt_bytecodes_count];
                if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_ARRAY)
                    branches[crt_case++] = method->bytecodes[crt_bytecodes_count + 10];

                if(crt_case < nb_cases) {
                    if(add_resetResume_bytecodes(method, tweak->currentStaticField_field, synclib->Input.resetResume_ref) == -1)
                        return -1;

                    if((class->fields[u2Index]->type->types->type & (TYPE_DESCRIPTOR_REF|TYPE_DESCRIPTOR_ARRAY)) == TYPE_DESCRIPTOR_REF) {
                        method->bytecodes[crt_bytecodes_count + 8]->branch = method->bytecodes[method->bytecodes_count - 6];
                        method->bytecodes[crt_bytecodes_count + 14]->branch = method->bytecodes[method->bytecodes_count - 6];
                        method->bytecodes[crt_bytecodes_count + 28]->branch = method->bytecodes[method->bytecodes_count - 6];
                        method->bytecodes[crt_bytecodes_count + 32]->branch = method->bytecodes[method->bytecodes_count - 6];
                    }
                }
            }

        if(was_int_merged)
            method->max_stack = 3;
        else
            method->max_stack = 2;
        if(tweak->nb_static_ref_fields > 0)
            method->max_locals = 1;
    }

    tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 1));
    if(tmp == NULL) {
        perror("add_mergeStatic_method");
        return -1;
    }
    method->bytecodes = tmp;
    method->bytecodes[method->bytecodes_count++] = vreturn;

    method->nargs = 1;
    return 0;

}


/**
 * \brief Generate bytecodes for calling each added mergeStatic method if any.
 *
 * Bytecodes are generated for each valid
 * ((tweak_info*)acf->classes[]->tweak)->mergeStaticAll_method field.
 *
 * \verbatim
// Generated code

switch(currentClass) {
    case 0:
        SomeClass.mergeStatic(out);
        out.resetResume();
        ++currentClass;
    case 1:
        SomeOtherClass.mergeStatic(out);
}
currentClass = (short)0;
\endverbatim
 * or
 * \verbatim
// Generated code

SomeClass.mergeStatic(out);
return;
\endverbatim
 *
 * \param acf The analyzed CAP file.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_mergeStaticAll_bytecodes(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib) {

    u2 u2Index1 = 0;

    u2 nb_classes = get_nb_classes_with_static(acf);

    for(;u2Index1 < acf->classes_count; ++u2Index1)
        if((((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_fields + ((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_array_fields + ((tweak_info*)acf->classes[u2Index1]->tweak)->nb_static_ref_fields) > 0)
            if(add_mergeStatic_method(acf, acf->classes[u2Index1], javacard_framework, synclib) == -1)
                return -1;

    if(nb_classes > 1) {
        for(u2Index1 = 0; u2Index1 < acf->classes_count; ++u2Index1)
            if(((tweak_info*)acf->classes[u2Index1]->tweak)->mergeStaticAll_method) {
                u2 u2Index2 = 0;
                method_info* method = ((tweak_info*)acf->classes[u2Index1]->tweak)->mergeStaticAll_method;
                bytecode_info** tmp = NULL;

                bytecode_info* getstatic_s = NULL;

                bytecode_info* stableswitch = NULL;
                bytecode_info** branches = NULL;
                u2 crt_case = 0;

                bytecode_info* sconst_0 = NULL;
                bytecode_info* putstatic_s = NULL;
                bytecode_info* vreturn = NULL;

                getstatic_s = create_getstatic_T_bytecode(((tweak_info*)acf->classes[u2Index1]->tweak)->currentClass_field);
                if(getstatic_s == NULL)
                    return -1;

                branches = (bytecode_info**)malloc(sizeof(bytecode_info*) * nb_classes);
                if(branches == NULL) {
                    perror("add_mergeStaticAll_bytecodes");
                    return -1;
                }

                method->bytecodes_count = 2;
                method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
                if(method->bytecodes == NULL) {
                    perror("add_mergeStaticAll_bytecodes");
                    return -1;
                }
                method->bytecodes[0] = getstatic_s;

                for(; u2Index2 < acf->classes_count; ++u2Index2)
                    if(((tweak_info*)acf->classes[u2Index2]->tweak)->mergeStatic_method) {
                        bytecode_info* aload_1 = NULL;
                        bytecode_info* invokestatic = NULL;

                        aload_1 = create_aload_1_bytecode();
                        if(aload_1 == NULL)
                            return -1;

                        invokestatic = create_invokestatic_bytecode(((tweak_info*)acf->classes[u2Index2]->tweak)->mergeStatic_method->this_method);
                        if(invokestatic == NULL)
                            return -1;

                        tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 2));
                        if(tmp == NULL) {
                            perror("add_mergeStaticAll_bytecodes");
                            return -1;
                        }
                        method->bytecodes = tmp;
                        method->bytecodes[method->bytecodes_count++] = aload_1;
                        method->bytecodes[method->bytecodes_count++] = invokestatic;

                        branches[crt_case++] = aload_1;

                        if(crt_case < nb_classes)
                            if(add_resetResume_bytecodes(method, ((tweak_info*)acf->classes[u2Index1]->tweak)->currentClass_field, synclib->Input.resetResume_ref) == -1)
                                return -1;
                    }

                sconst_0 = create_push_short_value_bytecode(0);
                if(sconst_0 == NULL)
                    return -1;

                stableswitch = create_stableswitch_bytecode(0, nb_classes - 1, branches, sconst_0);
                if(stableswitch == NULL)
                    return -1;

                putstatic_s = create_putstatic_T_bytecode(((tweak_info*)acf->classes[u2Index1]->tweak)->currentClass_field);
                if(putstatic_s == NULL)
                    return -1;

                vreturn = create_return_bytecode();
                if(vreturn == NULL)
                    return -1;

                tmp = (bytecode_info**)realloc(method->bytecodes, sizeof(bytecode_info*) * (method->bytecodes_count + 3));
                if(tmp == NULL) {
                    perror("add_mergeStaticAll_bytecodes");
                    return -1;
                }
                method->bytecodes = tmp;
                method->bytecodes[1] = stableswitch;
                method->bytecodes[method->bytecodes_count++] = sconst_0;
                method->bytecodes[method->bytecodes_count++] = putstatic_s;
                method->bytecodes[method->bytecodes_count++] = vreturn;

                method->nargs = 2;
                method->max_stack = 1;
            }
    } else {
        for(u2Index1 = 0; u2Index1 < acf->classes_count; ++u2Index1)
            if(((tweak_info*)acf->classes[u2Index1]->tweak)->mergeStaticAll_method) {
                u2 u2Index2 = 0;
                method_info* method = ((tweak_info*)acf->classes[u2Index1]->tweak)->mergeStaticAll_method;
                bytecode_info* aload_1 = NULL;
                bytecode_info* invokestatic = NULL;
                bytecode_info* vreturn = NULL;

                if(nb_classes == 1) {
                    for(; u2Index2 < acf->classes_count; ++u2Index2)
                        if(((tweak_info*)acf->classes[u2Index2]->tweak)->mergeStatic_method)
                            break;

                    aload_1 = create_aload_1_bytecode();
                    if(aload_1 == NULL)
                        return -1;

                    invokestatic = create_invokestatic_bytecode(((tweak_info*)acf->classes[u2Index2]->tweak)->mergeStatic_method->this_method);
                    if(invokestatic == NULL)
                        return -1;
                }

                vreturn = create_return_bytecode();
                if(vreturn == NULL)
                    return -1;

                method->bytecodes_count = nb_classes == 1 ? 3 : 1;
                method->bytecodes = (bytecode_info**)malloc(sizeof(bytecode_info*) * method->bytecodes_count);
                if(method->bytecodes == NULL) {
                    perror("add_mergeStaticAll_bytecodes");
                    return -1;
                }
                if(nb_classes == 1) {
                    method->bytecodes[0] = aload_1;
                    method->bytecodes[1] = invokestatic;
                    method->bytecodes[2] = vreturn;
                } else {
                    method->bytecodes[0] = vreturn;
                }

                method->nargs = 2;
                if(nb_classes == 1)
                    method->max_stack = 1;
            }
    }

    return 0;

}
