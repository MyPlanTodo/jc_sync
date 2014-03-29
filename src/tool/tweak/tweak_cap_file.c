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
 * \file tweak_cap_file.c
 * \brief Entry point for adding the sync synchronization framework in a CAP
 * file. A few functions are also defined and implemented for changing constant
 * pool entries or computing statistic about the classes defined in a CAP file. 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <exp_file.h>
#include <cap_file.h>
#include <analyzed_cap_file.h>
#include <cap_file_reader.h>
#include <cap_file_analyze.h>
#include <cap_file_generate.h>
#include <cap_file_writer.h>

#include "tweak_info.h"
#include "javacard_framework_info.h"
#include "synclib_info.h"
#include "init_javacard_framework_info.h"
#include "init_synclib_info.h"
#include "add_field_functions.h"
#include "tweak_class_extending_applet_functions.h"
#include "generic_tweak_functions.h"
#include "synchronizable_itf_functions.h"
#include "synchronizable_array_fields_itf_functions.h"
#include "synchronizable_instance_fields_itf_functions.h"
#include "create_bytecode_functions.h"


/**
 * \brief Change everywhere javacard.framework.Applet constant pool entries to an synclib.SyncApplet one.
 *
 * javacard.framework -> synclib
 * javacard.framework.Applet -> synclib.Applet
 * javacard.framework.Applet.register() -> synclib.framework.SyncApplet.syncRegister()
 * javacard.framework.Applet.register(byte[], short, byte) -> synclib.framework.SyncApplet.syncRegister(byte[], short, byte)
 * 
 * \param acf The analyzed CAP file.
 * \param javacard_framework Information on the javacard.framework package.
 * \param synclib Information on the synclib package.
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int change_applet_constant_pool_entries(analyzed_cap_file* acf, javacard_framework_info* javacard_framework, synclib_info* synclib) {

    u2 u2Index = 0;

    for(; u2Index < acf->constant_pool_count; ++u2Index)
        if(acf->constant_pool[u2Index]->flags & CONSTANT_POOL_IS_EXTERNAL) {
            if((acf->constant_pool[u2Index]->flags & CONSTANT_POOL_CLASSREF) && (acf->constant_pool[u2Index]->external_package->my_index == javacard_framework->package_token) && (acf->constant_pool[u2Index]->external_class_token == javacard_framework->Applet.class_token)) {
                acf->constant_pool[u2Index]->external_package = synclib->imported_package;
                acf->constant_pool[u2Index]->external_class_token = synclib->SyncApplet.class_token;
            } else if((acf->constant_pool[u2Index]->flags & CONSTANT_POOL_VIRTUALMETHODREF) && (acf->constant_pool[u2Index]->external_package->my_index == javacard_framework->package_token) && (acf->constant_pool[u2Index]->external_class_token == javacard_framework->Applet.class_token)) {
                acf->constant_pool[u2Index]->external_package = synclib->imported_package;
                acf->constant_pool[u2Index]->external_class_token = synclib->SyncApplet.class_token;
                if(acf->constant_pool[u2Index]->method_token == javacard_framework->Applet.register_1_method_token)
                    acf->constant_pool[u2Index]->method_token = synclib->SyncApplet.syncRegister_1_method_token;
                else if(acf->constant_pool[u2Index]->method_token == javacard_framework->Applet.register_2_method_token)
                    acf->constant_pool[u2Index]->method_token = synclib->SyncApplet.syncRegister_2_method_token;
            } else if((acf->constant_pool[u2Index]->flags & CONSTANT_POOL_SUPERMETHODREF) && (acf->constant_pool[u2Index]->external_package->my_index == javacard_framework->package_token) && (acf->constant_pool[u2Index]->external_class_token == javacard_framework->Applet.class_token)) {
                acf->constant_pool[u2Index]->external_package = synclib->imported_package;
                acf->constant_pool[u2Index]->external_class_token = synclib->SyncApplet.class_token;
            } else if((acf->constant_pool[u2Index]->flags & CONSTANT_POOL_STATICMETHODREF) && (acf->constant_pool[u2Index]->external_package->my_index == javacard_framework->package_token) && (acf->constant_pool[u2Index]->external_class_token == javacard_framework->Applet.class_token) && (acf->constant_pool[u2Index]->method_token == javacard_framework->Applet.Applet_method_token)) {
                acf->constant_pool[u2Index]->external_package = synclib->imported_package;
                acf->constant_pool[u2Index]->external_class_token = synclib->SyncApplet.class_token;
                acf->constant_pool[u2Index]->method_token = synclib->SyncApplet.SyncApplet_method_token;
            }
        }

    return 0;

}


/**
 * \brief Get the index in the constant pool of the given parse export file of
 * the synclib.SyncApplet class.
 *
 * \param ef The parsed export file of the synclib package.
 *
 * \return Return the index of constant pool entry in the given export file.
 */
static u2 get_SyncApplet_ref_in_export_file(export_file* ef) {

    u2 u2Index = 0;
    char* name = "synclib/SyncApplet";
    u2 name_length = 16;


    for(; u2Index < ef->constant_pool_count; ++u2Index)
        if((ef->constant_pool[u2Index].tag == EF_CONSTANT_CLASSREF) && (ef->constant_pool[ef->constant_pool[u2Index].CONSTANT_Classref.name_index].CONSTANT_Utf8.length == name_length) && (strncmp((char*)ef->constant_pool[ef->constant_pool[u2Index].CONSTANT_Classref.name_index].CONSTANT_Utf8.bytes, name, name_length) == 0))
            break;

    return u2Index;

}


/**
 * \brief Check if a class given its token has the synclib.SyncApplet class in its
 * class hierarchy.
 * 
 * \param ef The parsed export file defining the class to check.
 * \param class_token The token of the class.
 * 
 * \return Return 1 if the class has synclib.SyncApplet in its class hierarchy, 0 else.
 */
static int has_SyncApplet_as_superclass_in_export_file(export_file* ef, u2 class_token) {

    u2 u2Index1 = 0;
    u2 SyncApplet_cp_index = get_SyncApplet_ref_in_export_file(ef);

    if(SyncApplet_cp_index == ef->constant_pool_count)
        return 0;

    for(; u2Index1 < ef->export_class_count; ++u2Index1)
        if(ef->classes[u2Index1].token == class_token) {
            u2 u2Index2 = 0;

            for(; u2Index2 < ef->classes[u2Index1].export_supers_count; ++u2Index2)
                if(ef->classes[u2Index1].supers[u2Index2] == SyncApplet_cp_index)
                    return 1;
            return 0;
        }

    return 0;

}


/**
 * \brief Check if an analyzed class directly extends the synclib.SyncApplet
 * class.
 *
 * \param class The analyzed class to check.
 * \param synclib_imported_package The synclib analyzed imported package.
 * \param SyncApplet_class_token The class token of the synclib.SyncApplet class.
 *
 * \return Return 1 if the class has synclib.SyncApplet as direct superclass, 0 else.
 */
static int is_SyncApplet_immediate_superclass(class_info* class, imported_package_info* synclib_imported_package, u1 SyncApplet_class_token) {

    return class->superclass && (class->superclass->flags & CONSTANT_POOL_IS_EXTERNAL) && (class->superclass->external_package->my_index == synclib_imported_package->my_index) && (class->superclass->external_class_token == SyncApplet_class_token);

}


/**
 * \brief Check if an analyzed class has the synclib.SyncApple in its class
 * hierarchy.
 *
 * \param class The analyzed class to check.
 * \param synclib_imported_package The synclib analyzed imported package.
 * \param SyncApplet_class_token The class token of the synclib.SyncApplet class.
 * 
 * \return Return 1 if the class has synclib.SyncApplet in its class hierarchy, 0 else.
 */
static int has_SyncApplet_in_class_hierarchy(class_info* class, imported_package_info* synclib_imported_package, u1 SyncApplet_class_token) {

    if(is_SyncApplet_immediate_superclass(class, synclib_imported_package, SyncApplet_class_token))
        return 1;

    while(class->superclass != NULL) {
        if(class->superclass->flags & CONSTANT_POOL_IS_EXTERNAL)
                return is_SyncApplet_immediate_superclass(class, synclib_imported_package, SyncApplet_class_token) || has_SyncApplet_as_superclass_in_export_file(class->superclass->external_package->ef, class->superclass->external_class_token);

        class = class->superclass->internal_class;

    }

    return 0;

}


/**
 * \brief Get the number of boolean arrays in a class hierarchy.
 *
 * Can be called only after a call to compute_statistics_on_fields. The boolean
 * arrays in the given class are included.
 *
 * \param class The analyzed class to start from.
 *
 * \return Return the number of boolean arrays in the class hierarchy.
 */
static u2 get_nb_boolean_array_fields(class_info* class) {

    u2 count = 0;
    class_info* currentClass = class;

    while(currentClass != NULL) {
        count += ((tweak_info*)currentClass->tweak)->nb_boolean_array_fields;
        if(currentClass->superclass && !(currentClass->superclass->flags & CONSTANT_POOL_IS_EXTERNAL))
            currentClass = currentClass->superclass->internal_class;
        else
            break;
    }

    return count;

}


/**
 * \brief Get the number of byte arrays in a class hierarchy.
 *
 * Can be called only after a call to compute_statistics_on_fields. The byte
 * arrays in the given class are included.
 *
 * \param class The analyzed class to start from.
 *
 * \return Return the number of byte arrays in the class hierarchy.
 */
static u2 get_nb_byte_array_fields(class_info* class) {

    u2 count = 0;
    class_info* currentClass = class;

    while(currentClass != NULL) {
        count += ((tweak_info*)currentClass->tweak)->nb_byte_array_fields;
        if(currentClass->superclass && !(currentClass->superclass->flags & CONSTANT_POOL_IS_EXTERNAL))
            currentClass = currentClass->superclass->internal_class;
        else
            break;
    }

    return count;

}


/**
 * \brief Get the number of short arrays in a class hierarchy.
 *
 * Can be called only after a call to compute_statistics_on_fields. The short
 * arrays in the given class are included.
 *
 * \param class The analyzed class to start from.
 *
 * \return Return the number of short arrays in the class hierarchy.
 */
static u2 get_nb_short_array_fields(class_info* class) {

    u2 count = 0;
    class_info* currentClass = class;

    while(currentClass != NULL) {
        count += ((tweak_info*)currentClass->tweak)->nb_short_array_fields;
        if(currentClass->superclass && !(currentClass->superclass->flags & CONSTANT_POOL_IS_EXTERNAL))
            currentClass = currentClass->superclass->internal_class;
        else
            break;
    }

    return count;

}


/**
 * \brief Get the number of int arrays in a class hierarchy.
 *
 * Can be called only after a call to compute_statistics_on_fields. The int
 * arrays in the given class are included.
 *
 * \param class The analyzed class to start from.
 *
 * \return Return the number of int arrays in the class hierarchy.
 */
static u2 get_nb_int_array_fields(class_info* class) {

    u2 count = 0;
    class_info* currentClass = class;

    while(currentClass != NULL) {
        count += ((tweak_info*)currentClass->tweak)->nb_int_array_fields;
        if(currentClass->superclass && !(currentClass->superclass->flags & CONSTANT_POOL_IS_EXTERNAL))
            currentClass = currentClass->superclass->internal_class;
        else
            break;
    }

    return count;

}


/**
 * \brief Compute several statistics about a class fields.
 *
 * \param class The analyzed class on which the statistics are computed.
 */
static void compute_statistics_on_fields(class_info* class) {

    u2 u2Index = 0;

    tweak_info* tweak = (tweak_info*)class->tweak;

    tweak->nb_methods_before_tweak = class->methods_count;

    for(; u2Index < class->fields_count; ++u2Index) {
        if(!(class->fields[u2Index]->flags & (FIELD_FINAL|FIELD_SHOULD_NOT_SYNCHRONIZE))) {
            if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_ARRAY) {
                if(class->fields[u2Index]->flags & FIELD_STATIC)
                    ++tweak->nb_static_array_fields;
                else
                    ++tweak->nb_array_fields;

                if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BOOLEAN) {
                    if(class->fields[u2Index]->flags & FIELD_STATIC)
                        ++tweak->nb_static_boolean_array_fields;
                    else
                        ++tweak->nb_boolean_array_fields;
                } else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_BYTE) {
                    if(class->fields[u2Index]->flags & FIELD_STATIC)
                        ++tweak->nb_static_byte_array_fields;
                    else
                        ++tweak->nb_byte_array_fields;
                } else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_SHORT) {
                    if(class->fields[u2Index]->flags & FIELD_STATIC)
                        ++tweak->nb_static_short_array_fields;
                    else
                        ++tweak->nb_short_array_fields;
                } else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_INT) {
                    if(class->fields[u2Index]->flags & FIELD_STATIC)
                        ++tweak->nb_static_int_array_fields;
                    else
                        ++tweak->nb_int_array_fields;
                }
            } else if(class->fields[u2Index]->type->types->type & TYPE_DESCRIPTOR_REF) {
                if(class->fields[u2Index]->flags & FIELD_STATIC)
                    ++tweak->nb_static_ref_fields;
                else
                    ++tweak->nb_ref_fields;
            } else {
                if(class->fields[u2Index]->flags & FIELD_STATIC)
                    ++tweak->nb_static_fields;
                else
                    ++tweak->nb_fields;
            }
        }
    }

}


/**
 * \brief Compute several statistics on a class superclass if any.
 *
 * \param class The analyzed class on which the statistics are computed.
 * \param synclib Information on the synclib package.
 */
static void compute_statistics_on_superclass(class_info* class, synclib_info* synclib) {

    tweak_info* tweak = (tweak_info*)class->tweak;

    if(class->superclass) {
        tweak->is_SyncApplet_immediate_superclass = is_SyncApplet_immediate_superclass(class, synclib->imported_package, synclib->SyncApplet.class_token);
        tweak->has_SyncApplet_in_class_hierarchy = has_SyncApplet_in_class_hierarchy(class, synclib->imported_package, synclib->SyncApplet.class_token);

        tweak->is_superclass_implementing_Synchronizable_interface = (class->superclass->flags & CONSTANT_POOL_IS_EXTERNAL) ? is_external_class_implementing_interface(class->superclass, "synclib/Synchronizable") : is_internal_class_implementing_interface(class->superclass, synclib->Synchronizable.interface_ref, &(tweak->Synchronizable_methods));

        tweak->is_superclass_implementing_SynchronizableArrayFields_interface = (class->superclass->flags & CONSTANT_POOL_IS_EXTERNAL) ? is_external_class_implementing_interface(class->superclass, "synclib/SynchronizableArrayFields") : is_internal_class_implementing_interface(class->superclass, synclib->SynchronizableArrayFields.interface_ref, &(tweak->SynchronizableArrayFields_methods));

        tweak->is_superclass_implementing_SynchronizableInstanceFields_interface = (class->superclass->flags & CONSTANT_POOL_IS_EXTERNAL) ? is_external_class_implementing_interface(class->superclass, "synclib/SynchronizableInstanceFields") : is_internal_class_implementing_interface(class->superclass, synclib->SynchronizableInstanceFields.interface_ref, &(tweak->SynchronizableInstanceFields_methods));

        if(tweak->is_superclass_implementing_SynchronizableArrayFields_interface && !(class->superclass->flags & CONSTANT_POOL_IS_EXTERNAL)) {
            tweak->nb_boolean_array_superclass_fields = get_nb_boolean_array_fields(class->superclass->internal_class);
            tweak->nb_byte_array_superclass_fields = get_nb_byte_array_fields(class->superclass->internal_class);
            tweak->nb_short_array_superclass_fields = get_nb_short_array_fields(class->superclass->internal_class);
            tweak->nb_int_array_superclass_fields = get_nb_int_array_fields(class->superclass->internal_class);
        }
    }

}


/**
 * \brief Add the sync synchronization framework to an analyzed CAP file.
 *
 * \param acf The analyzed CAP file
 * \param export_files An array of parsed export files
 * \param nb_export_files The number of parsed export files in the array
 * \param is_using_int Is the int type used?
 *
 * \return Return -1 if an error occurred, 0 else.
 */
static int add_sync_to_cap_file(analyzed_cap_file* acf, export_file** export_files, int nb_export_files, char is_using_int) {

    u2 u2Index1 = 0;

    javacard_framework_info javacard_framework;
    synclib_info synclib;

    /* We first fetch some information about the javacard.framework package.
       Some entries are also added to the constant pool. */
    if(init_javacard_framework_info(acf, export_files, nb_export_files, &javacard_framework) == -1)
        return -1;

    /* We fetch information about the synclib package.
       It also adds entries to the constant pool for fields, methods, classes and interfaces. */
    if(init_synclib_info(acf, export_files, nb_export_files, &synclib, is_using_int) == -1)
        return -1;

    /* Tweak some constant pool entries to put in place synclib.SyncApplet class and methods. */
    if(change_applet_constant_pool_entries(acf, &javacard_framework, &synclib) == -1)
        return -1;

    for(; u2Index1 < acf->classes_count; ++u2Index1) {
        acf->classes[u2Index1]->tweak = malloc(sizeof(tweak_info));
        if (acf->classes[u2Index1]->tweak == NULL) {
            perror("add_sync_to_cap_file");
            return -1;
        }
        memset(acf->classes[u2Index1]->tweak, 0, sizeof(tweak_info));

        compute_statistics_on_fields(acf->classes[u2Index1]);
    }

    /* For each classes within the analyzed CAP file, we tweak. */
    for(u2Index1 = 0; u2Index1 < acf->classes_count; ++u2Index1) {
        compute_statistics_on_superclass(acf->classes[u2Index1], &synclib);

        /* If the tweaked class has SyncApplet in its class hierarchy,
           then there some more methods to add. */
        if(((tweak_info*)acf->classes[u2Index1]->tweak)->has_SyncApplet_in_class_hierarchy)
            if(tweak_class_extending_applet(acf, acf->classes[u2Index1], &javacard_framework, &synclib, is_using_int) == -1)
                return -1;

        /* We add the Synchronizable interface to the tweaked class. */
        if(add_Synchronizable_interface(acf, acf->classes[u2Index1], &javacard_framework, &synclib) == -1)
            return -1;

        /* If there is at least one array or if the interface is implemented in the class hierarchy then we add it. */
        if((((tweak_info*)acf->classes[u2Index1]->tweak)->nb_array_fields > 0) || ((tweak_info*)acf->classes[u2Index1]->tweak)->is_superclass_implementing_SynchronizableArrayFields_interface)
            if(add_SynchronizableArrayFields_interface(acf, acf->classes[u2Index1], &synclib, &javacard_framework, is_using_int) == -1)
                return -1;

        /* If there is at least one reference field (apart from array field) or if the interface is implemented in the class hierarchy then we add it. */
        if((((tweak_info*)acf->classes[u2Index1]->tweak)->nb_ref_fields > 0) || ((tweak_info*)acf->classes[u2Index1]->tweak)->is_superclass_implementing_SynchronizableInstanceFields_interface)
            if(add_SynchronizableInstanceFields_interface(acf, acf->classes[u2Index1], &synclib) == -1)
                return -1;
    }

    /* We generate the bytecodes for static fields and add needed methods. */
    if(generate_bytecodes_handling_static(acf, &javacard_framework, &synclib, is_using_int) == -1)
        return -1;

    return 0;

}


/**
 * \brief The main for adding the sync synchronization framework to a CAP file.
 * 
 * \param argc The number of parameters in argv.
 * \param argv The program parameters.
 *
 * \return Return EXIT_FAILURE if an error occurred, EXIT_SUCCESS else.
 */
int main(int argc, char* argv[]) {

    cap_file* cf = NULL;
    cap_file* new_cf = NULL;
    analyzed_cap_file* acf = NULL;
    export_file** export_files = NULL;
    int nb_export_files = 0;

    int i = 0;
    char** directories = NULL;
    int nb_directories = 0;

    if(argc < 4) {
        fprintf(stderr, "Usage: %s exp_files_directory [exp_files_directory] filename_in filename_out\n", argv[0]);
        return EXIT_FAILURE;
    }

    if((cf = read_cap_file(argv[argc - 2])) == NULL)
        return EXIT_FAILURE;

    nb_directories = argc - 3;
    directories = (char**)malloc(sizeof(char*) * nb_directories);
    if(directories == NULL) {
        perror("main");
        return EXIT_FAILURE;
    }

    for(; i < nb_directories; ++i)
        directories[i] = argv[1 + i];

    export_files = get_export_files_from_directories(directories, nb_directories, &nb_export_files);

    if((acf = analyze_cap_file(cf, export_files, nb_export_files)) == NULL)
        return EXIT_FAILURE;

    if(add_sync_to_cap_file(acf, export_files, nb_export_files, 0) == -1)
        return EXIT_FAILURE;

    if((new_cf = generate_cap_file(acf)) == NULL)
        return EXIT_FAILURE;

    if(write_cap_file(new_cf, argv[argc - 1]) == -1)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;

}
