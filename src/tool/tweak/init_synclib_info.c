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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <analyzed_cap_file.h>
#include <exp_file.h>

#include "tweak_info.h"
#include "generic_tweak_functions.h"
#include "synclib_info.h"

static const u1 synclib_package_aid[] = {0xDE,0xAD,0xBE,0xEF,0x01,0xDE,0xAD,0xBE,0xEF,0x00,0x00};
static const u1 synclib_package_aid_length = 11;

static export_file* get_synclib_export_file(export_file** export_files, int nb_export_files);
static constant_pool_entry_info* add_field_from_imported_package_to_constant_pool(analyzed_cap_file* acf, imported_package_info* imported_package, u1 class_token, const char* name, export_file** export_files, int nb_export_files);
static constant_pool_entry_info* add_external_method_to_constant_pool(analyzed_cap_file* acf, imported_package_info* imported_package, u1 class_token, const char* name, const char* signature, export_file** export_files, int nb_export_files);
static int init_SyncApplet_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, SyncApplet_info* SyncApplet, char is_using_int);
static int init_Input_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, Input_info* Input, char is_using_int);
static int init_Output_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, Output_info* Output, char is_using_int);
static int init_Synchronizable_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, Synchronizable_info* Synchronizable);
static int init_SynchronizableArrayFields_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, SynchronizableArrayFields_info* SynchronizableArrayFields, char is_using_int);
static int init_SynchronizableInstanceFields_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, SynchronizableInstanceFields_info* SynchronizableInstanceFields);
static int init_ResetIdSource_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, ResetIdSource_info* ResetIdSource);
static type_descriptor_info* create_getId_sig(analyzed_cap_file* acf);
static type_descriptor_info* create_setId_sig(analyzed_cap_file* acf);
static type_descriptor_info* create_isDuplicated_sig(analyzed_cap_file* acf);
static type_descriptor_info* create_setPreviousSynchronizableInstance_sig(analyzed_cap_file* acf, synclib_info* synclib);
static type_descriptor_info* create_resetPath_sig(analyzed_cap_file* acf, synclib_info* synclib);
static type_descriptor_info* create_numberPath_sig(analyzed_cap_file* acf, synclib_info* synclib);
static type_descriptor_info* create_serialize_sig(analyzed_cap_file* acf, synclib_info* synclib);
static type_descriptor_info* create_merge_sig(analyzed_cap_file* acf, synclib_info* synclib);
static type_descriptor_info* create_resetStaticPathAll_sig(analyzed_cap_file* acf, synclib_info* synclib);
static type_descriptor_info* create_getClassId_sig(analyzed_cap_file* acf, synclib_info* synclib);
static type_descriptor_info* create_getNewInstance_sig(analyzed_cap_file* acf, synclib_info* synclib);
static type_descriptor_info* create_getAnyStaticBooleanArray_sig(analyzed_cap_file* acf);
static type_descriptor_info* create_getAnyStaticByteArray_sig(analyzed_cap_file* acf);
static type_descriptor_info* create_getAnyStaticShortArray_sig(analyzed_cap_file* acf);
static type_descriptor_info* create_getAnyStaticIntArray_sig(analyzed_cap_file* acf);
static type_descriptor_info* create_getNext_sig(analyzed_cap_file* acf, synclib_info* synclib);
static type_descriptor_info* create_setNext_sig(analyzed_cap_file* acf, synclib_info* synclib);
static type_descriptor_info* create_resetNextSynchronizableInstanceSelect_sig(analyzed_cap_file* acf);


int init_synclib_info(analyzed_cap_file* acf, export_file** export_files, int nb_export_files, synclib_info* synclib, char is_using_int) {

    export_file* synclib_ef = NULL;

    if((synclib_ef = get_synclib_export_file(export_files, nb_export_files)) == NULL) {
        fprintf(stderr, "Could not find synclib export file\n");
        return -1;
    }

    if((synclib->imported_package = add_imported_package_to_analyzed_cap_file(acf, synclib_ef)) == NULL)
        return -1;

    if(init_SyncApplet_info(acf, synclib->imported_package, export_files, nb_export_files, &(synclib->SyncApplet), is_using_int) == -1)
        return -1;

    if(init_Input_info(acf, synclib->imported_package, export_files, nb_export_files, &(synclib->Input), is_using_int) == -1)
        return -1;

    if(init_Output_info(acf, synclib->imported_package, export_files, nb_export_files, &(synclib->Output), is_using_int) == -1)
        return -1;

    if(init_Synchronizable_info(acf, synclib->imported_package, export_files, nb_export_files, &(synclib->Synchronizable)) == -1)
        return -1;

    if(init_SynchronizableArrayFields_info(acf, synclib->imported_package, export_files, nb_export_files, &(synclib->SynchronizableArrayFields), is_using_int) == -1)
        return -1;

    if(init_SynchronizableInstanceFields_info(acf, synclib->imported_package, export_files, nb_export_files, &(synclib->SynchronizableInstanceFields)) == -1)
        return -1;

    if(init_ResetIdSource_info(acf, synclib->imported_package, export_files, nb_export_files, &(synclib->ResetIdSource)) == -1)
        return -1;

    // Synchronizable method type descriptors

    if((synclib->Synchronizable.getId_sig = create_getId_sig(acf)) == NULL)
        return -1;

    if((synclib->Synchronizable.setId_sig = create_setId_sig(acf)) == NULL)
        return -1;

    if((synclib->Synchronizable.isDuplicated_sig = create_isDuplicated_sig(acf)) == NULL)
        return -1;

    synclib->Synchronizable.getPreviousSynchronizableInstance_sig = synclib->Input.newlyCreatedInstance_ref->type;

    if((synclib->Synchronizable.setPreviousSynchronizableInstance_sig = create_setPreviousSynchronizableInstance_sig(acf, synclib)) == NULL)
        return -1;

    if((synclib->Synchronizable.resetPath_sig = create_resetPath_sig(acf, synclib)) == NULL)
        return -1;

    if((synclib->Synchronizable.numberPath_sig = create_numberPath_sig(acf, synclib)) == NULL)
        return -1;

    if((synclib->Synchronizable.serialize_sig = create_serialize_sig(acf, synclib)) == NULL)
        return -1;

    if((synclib->Synchronizable.merge_sig = create_merge_sig(acf, synclib)) == NULL)
        return -1;

    synclib->Synchronizable.init_method_sig = synclib->Synchronizable.merge_sig;

    // SyncApplet protected abstract method type descriptors

    if((synclib->SyncApplet.resetStaticPathAll_sig = create_resetStaticPathAll_sig(acf, synclib)) == NULL)
        return -1;

    synclib->SyncApplet.numberStaticPathAll_sig = synclib->Synchronizable.serialize_sig;
    synclib->SyncApplet.serializeStaticAll_sig = synclib->Synchronizable.serialize_sig;
    synclib->SyncApplet.mergeStaticAll_sig = synclib->Synchronizable.merge_sig;

    if((synclib->SyncApplet.getClassId_sig = create_getClassId_sig(acf, synclib)) == NULL)
        return -1;

    if((synclib->SyncApplet.getNewInstance_sig = create_getNewInstance_sig(acf, synclib)) == NULL)
        return -1;

    synclib->SyncApplet.getNbStaticBooleanArrays_sig = synclib->Synchronizable.getId_sig;

    if((synclib->SyncApplet.getAnyStaticBooleanArray_sig = create_getAnyStaticBooleanArray_sig(acf)) == NULL)
        return -1;

    synclib->SyncApplet.getNbStaticByteArrays_sig = synclib->Synchronizable.getId_sig;

    if((synclib->SyncApplet.getAnyStaticByteArray_sig = create_getAnyStaticByteArray_sig(acf)) == NULL)
        return -1;

    synclib->SyncApplet.getNbStaticShortArrays_sig = synclib->Synchronizable.getId_sig;

    if((synclib->SyncApplet.getAnyStaticShortArray_sig = create_getAnyStaticShortArray_sig(acf)) == NULL)
        return -1;

    if(is_using_int) {
        synclib->SyncApplet.getNbStaticIntArrays_sig = synclib->Synchronizable.getId_sig;
        if((synclib->SyncApplet.getAnyStaticIntArray_sig = create_getAnyStaticIntArray_sig(acf)) == NULL)
            return -1;
    }


    // SynchronizableArrayFields method type descriptors

    if((synclib->SynchronizableArrayFields.getNext_sig = create_getNext_sig(acf, synclib)) == NULL)
        return -1;

    if((synclib->SynchronizableArrayFields.setNext_sig = create_setNext_sig(acf, synclib)) == NULL)
        return -1;

    synclib->SynchronizableArrayFields.getNbBooleanArrays_sig = synclib->SyncApplet.getNbStaticBooleanArrays_sig;
    synclib->SynchronizableArrayFields.getBooleanArray_sig = synclib->SyncApplet.getAnyStaticBooleanArray_sig;

    synclib->SynchronizableArrayFields.getNbByteArrays_sig = synclib->SyncApplet.getNbStaticByteArrays_sig;
    synclib->SynchronizableArrayFields.getByteArray_sig = synclib->SyncApplet.getAnyStaticByteArray_sig;

    synclib->SynchronizableArrayFields.getNbShortArrays_sig = synclib->SyncApplet.getNbStaticShortArrays_sig;
    synclib->SynchronizableArrayFields.getShortArray_sig = synclib->SyncApplet.getAnyStaticShortArray_sig;

    if(is_using_int) {
        synclib->SynchronizableArrayFields.getNbIntArrays_sig = synclib->SyncApplet.getNbStaticIntArrays_sig;
        synclib->SynchronizableArrayFields.getIntArray_sig = synclib->SyncApplet.getAnyStaticIntArray_sig;
    }


    // SynchronizableInstanceFields method type descriptors
    synclib->SynchronizableInstanceFields.getCurrentSynchronizableInstance_sig = synclib->Synchronizable.getPreviousSynchronizableInstance_sig;
    synclib->SynchronizableInstanceFields.setCurrentSynchronizableInstance_sig = synclib->Synchronizable.setPreviousSynchronizableInstance_sig;
    synclib->SynchronizableInstanceFields.hasCurrentSynchronizableInstance_sig = synclib->Synchronizable.isDuplicated_sig;

    if((synclib->SynchronizableInstanceFields.resetNextSynchronizableInstanceSelect_sig = create_resetNextSynchronizableInstanceSelect_sig(acf)) == NULL)
        return -1;

    synclib->SynchronizableInstanceFields.selectNextSynchronizableInstance_sig = synclib->SynchronizableInstanceFields.resetNextSynchronizableInstanceSelect_sig;

    return 0;

}


/**
 * Return the synclib parsed export file from the given array of parsed export file.
 */
static export_file* get_synclib_export_file(export_file** export_files, int nb_export_files) {

    int i = 0;

    for(; i < nb_export_files; ++i)
        if(export_files[i]->constant_pool[export_files[i]->this_package].CONSTANT_Package.aid_length == synclib_package_aid_length) {
            u1 u1Index = 0;

            for(; u1Index < synclib_package_aid_length; ++u1Index)
                if(export_files[i]->constant_pool[export_files[i]->this_package].CONSTANT_Package.aid[u1Index] != synclib_package_aid[u1Index])
                    break;

            if(u1Index == synclib_package_aid_length)
                break;
        }

    if(i != nb_export_files)
        return export_files[i];

    return NULL;

}


static type_descriptor_info* make_field_type_descriptor_from_imported_package(analyzed_cap_file* acf, imported_package_info* imported_package, u2 descriptor_index, export_file** export_files, int nb_export_files) {

    u2 u2Index = 0;
    char fully_qualified_class_name[0XFFFFu];

    type_descriptor_info* type = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(type == NULL) {
        perror("make_field_type_descriptor_from_export_file");
        return NULL;
    }

    type->count = 0;
    type->offset = 0;
    type->types_count = 1;
    type->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info));
    if(type->types == NULL) {
        perror("make_field_type_descriptor_from_export_file");
        return NULL;
    }

    type->types->type = 0;
    type->types->ref = NULL;

    switch(imported_package->ef->constant_pool[descriptor_index].CONSTANT_Utf8.bytes[0]) {
        case 'B':
            type->types->type = TYPE_DESCRIPTOR_BYTE;
            break;

        case 'I':
            type->types->type = TYPE_DESCRIPTOR_INT;
            break;

        case 'L':
            type->types->type = TYPE_DESCRIPTOR_REF;
            memcpy(fully_qualified_class_name, imported_package->ef->constant_pool[descriptor_index].CONSTANT_Utf8.bytes + 1, imported_package->ef->constant_pool[descriptor_index].CONSTANT_Utf8.length - 2);
            fully_qualified_class_name[imported_package->ef->constant_pool[descriptor_index].CONSTANT_Utf8.length - 2] = '\0';
            type->types->ref = get_external_classref_from_constant_pool(acf, NULL, fully_qualified_class_name, export_files, nb_export_files);
            if(type->types->ref == NULL)
                return NULL;
            break;

        case 'S':
            type->types->type = TYPE_DESCRIPTOR_SHORT;
            break;

        case 'Z':
            type->types->type = TYPE_DESCRIPTOR_BOOLEAN;
            break;

        case '[':
            type->types->type = TYPE_DESCRIPTOR_ARRAY;
            switch(imported_package->ef->constant_pool[descriptor_index].CONSTANT_Utf8.bytes[1]) {
                case 'B':
                    type->types->type |= TYPE_DESCRIPTOR_BYTE;
                    break;

                case 'I':
                    type->types->type |= TYPE_DESCRIPTOR_INT;
                    break;

                case 'L':
                    type->types->type |= TYPE_DESCRIPTOR_REF;
                    memcpy(fully_qualified_class_name, imported_package->ef->constant_pool[descriptor_index].CONSTANT_Utf8.bytes + 2, imported_package->ef->constant_pool[descriptor_index].CONSTANT_Utf8.length - 3);
                    fully_qualified_class_name[imported_package->ef->constant_pool[descriptor_index].CONSTANT_Utf8.length - 3] = '\0';
                    type->types->ref = get_external_classref_from_constant_pool(acf, NULL, fully_qualified_class_name, export_files, nb_export_files);
                    if(type->types->ref == NULL)
                        return NULL;
                    break;

                case 'S':
                    type->types->type |= TYPE_DESCRIPTOR_SHORT;
                    break;

                case 'Z':
                    type->types->type |= TYPE_DESCRIPTOR_BOOLEAN;
            }
    }

    u2Index = is_type_descriptor_not_unique(acf, type);

    if(u2Index == acf->signature_pool_count) {
        type_descriptor_info** tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp == NULL) {
            perror("make_field_type_descriptor_from_imported_package");
            return NULL;
        }
        acf->signature_pool = tmp;

        acf->signature_pool[acf->signature_pool_count] = type;
        ++acf->signature_pool_count;
    } else {
        free(type->types);
        free(type);
        type = acf->signature_pool[u2Index];
    }

    return type;

}


static constant_pool_entry_info* add_field_from_imported_package_to_constant_pool(analyzed_cap_file* acf, imported_package_info* imported_package, u1 class_token, const char* name, export_file** export_files, int nb_export_files) {

    u1 u1Index = 0;
    u2 name_length = strlen(name);

    for(; u1Index < imported_package->ef->export_class_count; ++u1Index)
        if(imported_package->ef->classes[u1Index].token == class_token) {
            u2 u2Index = 0;

            for(; u2Index < imported_package->ef->classes[u1Index].export_fields_count; ++u2Index)
                if((imported_package->ef->constant_pool[imported_package->ef->classes[u1Index].fields[u2Index].name_index].CONSTANT_Utf8.length == name_length) && (strncmp((char*)imported_package->ef->constant_pool[imported_package->ef->classes[u1Index].fields[u2Index].name_index].CONSTANT_Utf8.bytes, name, name_length) == 0)) {
                    constant_pool_entry_info** tmp = NULL;
                    constant_pool_entry_info* entry = (constant_pool_entry_info*)malloc(sizeof(constant_pool_entry_info));
                    if(entry == NULL) {
                        perror("add_field_from_export_file_to_constant_pool");
                        return NULL;
                    }

                    if(imported_package->ef->classes[u1Index].fields[u2Index].access_flags & EF_ACC_STATIC)
                        entry->flags = CONSTANT_POOL_STATICFIELDREF|CONSTANT_POOL_IS_EXTERNAL;
                    else
                        entry->flags = CONSTANT_POOL_INSTANCEFIELDREF|CONSTANT_POOL_IS_EXTERNAL;

                    entry->count = 0;
                    entry->type = make_field_type_descriptor_from_imported_package(acf, imported_package, imported_package->ef->classes[u1Index].fields[u2Index].descriptor_index, export_files, nb_export_files);
                    if(entry->type == NULL)
                        return NULL;
                    entry->external_package = imported_package;
                    entry->external_class_token = class_token;
                    entry->external_field_token = imported_package->ef->classes[u1Index].fields[u2Index].token;
                    entry->my_index = acf->constant_pool_count;

                    tmp = (constant_pool_entry_info**)realloc(acf->constant_pool, sizeof(constant_pool_entry_info*) * (acf->constant_pool_count + 1));
                    if(tmp == NULL) {
                        perror("add_field_from_imported_package_to_constant_pool");
                        return NULL;
                    }
                    acf->constant_pool = tmp;
                    acf->constant_pool[acf->constant_pool_count] = entry;
                    ++acf->constant_pool_count;

                    return entry;
                }

            return NULL;
        }

    return NULL;

}


static type_descriptor_info* make_signature_descriptor_from_imported_package(analyzed_cap_file* acf, imported_package_info* imported_package, u2 descriptor_index, export_file** export_files, int nb_export_files) {

    u2 crt_position = 1;
    char fully_qualified_class_name[0XFFFFu];
    u2 semicolon_position = 0;

    ef_CONSTANT_Utf8_info* signature = &(imported_package->ef->constant_pool[descriptor_index].CONSTANT_Utf8);
    type_descriptor_info* new = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(new == NULL) {
        perror("make_signature_descriptor_from_imported_package");
        return NULL;
    }

    new->count = 0;
    new->offset = 0;
    new->types_count = 0;
    new->types = NULL;

    if(signature->bytes[0] != '(')
        return NULL;

    while(crt_position < signature->length) {
        one_type_descriptor_info* tmp = NULL;

        if(signature->bytes[crt_position] == ')')
            ++crt_position;

        tmp = (one_type_descriptor_info*)realloc(new->types, sizeof(one_type_descriptor_info) * (new->types_count + 1));
        if(tmp == NULL) {
            perror("make_signature_descriptor_from_imported_package");
            return NULL;
        }
        new->types = tmp;

        memset(new->types + new->types_count, 0, sizeof(one_type_descriptor_info));

        switch(signature->bytes[crt_position]) {
            case 'B':
                new->types[new->types_count].type = TYPE_DESCRIPTOR_BYTE;
                break;

            case 'I':
                new->types[new->types_count].type = TYPE_DESCRIPTOR_INT;
                break;

            case 'S':
                new->types[new->types_count].type = TYPE_DESCRIPTOR_SHORT;
                break;

            case 'V':
                new->types[new->types_count].type = TYPE_DESCRIPTOR_VOID;
                break;

            case 'Z':
                new->types[new->types_count].type = TYPE_DESCRIPTOR_BOOLEAN;
                break;

            case 'L':
                new->types[new->types_count].type = TYPE_DESCRIPTOR_REF;
                semicolon_position = ((u1*)memchr(signature->bytes + crt_position + 1, ';', signature->length - crt_position - 1)) - (signature->bytes + crt_position + 1);
                memcpy(fully_qualified_class_name, signature->bytes + crt_position + 1, semicolon_position);
                fully_qualified_class_name[semicolon_position] = '\0';
                new->types[new->types_count].ref = get_external_classref_from_constant_pool(acf, NULL, fully_qualified_class_name, export_files, nb_export_files);
                if(new->types[new->types_count].ref == NULL)
                    return NULL;
                crt_position += semicolon_position + 1;
                break;

            case '[':
                ++crt_position;
                new->types[new->types_count].type = TYPE_DESCRIPTOR_ARRAY;
                switch(signature->bytes[crt_position]) {
                    case 'B':
                        new->types[new->types_count].type |= TYPE_DESCRIPTOR_BYTE;
                        break;

                    case 'I':
                        new->types[new->types_count].type |= TYPE_DESCRIPTOR_INT;
                        break;

                    case 'S':
                        new->types[new->types_count].type |= TYPE_DESCRIPTOR_SHORT;
                        break;

                    case 'Z':
                        new->types[new->types_count].type |= TYPE_DESCRIPTOR_BOOLEAN;
                        break;

                    case 'L':
                        new->types[new->types_count].type |= TYPE_DESCRIPTOR_REF;
                        semicolon_position = ((u1*)memchr(signature->bytes + crt_position + 1, ';', signature->length - crt_position - 1)) - (signature->bytes + crt_position + 1);
                        memcpy(fully_qualified_class_name, signature->bytes + crt_position + 1, semicolon_position);
                        fully_qualified_class_name[semicolon_position] = '\0';
                        new->types[new->types_count].ref = get_external_classref_from_constant_pool(acf, NULL, fully_qualified_class_name, export_files, nb_export_files);
                        if(new->types[new->types_count].ref == NULL)
                            return NULL;
                        crt_position += semicolon_position + 1;
                        break;

                }
        }
        ++new->types_count;
        ++crt_position;
    }

    return new;

}


static constant_pool_entry_info* add_external_method_to_constant_pool(analyzed_cap_file* acf, imported_package_info* imported_package, u1 class_token, const char* name, const char* signature, export_file** export_files, int nb_export_files) {

    char is_static = 0;

    u2 name_length = strlen(name);

    type_descriptor_info* new_signature = NULL;
    constant_pool_entry_info** tmp1 = NULL;
    constant_pool_entry_info* new = NULL;

    u2 is_unique = acf->signature_pool_count;
    u1 method_token = 0;

    u1 u1Index = 0;
    u2 u2Index = 0;


    for(; u1Index < imported_package->ef->export_class_count; ++u1Index)
        if(imported_package->ef->classes[u1Index].token == class_token) {
            for(u2Index = 0; u2Index < imported_package->ef->classes[u1Index].export_methods_count; ++u2Index)
                if((imported_package->ef->constant_pool[imported_package->ef->classes[u1Index].methods[u2Index].name_index].CONSTANT_Utf8.length == name_length) && (strncmp((char*)imported_package->ef->constant_pool[imported_package->ef->classes[u1Index].methods[u2Index].name_index].CONSTANT_Utf8.bytes, name, name_length) == 0)) {
                    ef_CONSTANT_Utf8_info* export_file_descriptor = &(imported_package->ef->constant_pool[imported_package->ef->classes[u1Index].methods[u2Index].descriptor_index].CONSTANT_Utf8);
                    if((export_file_descriptor->length == strlen(signature)) && (strncmp((char*)export_file_descriptor->bytes, signature, export_file_descriptor->length) == 0))
                        break;
                }

            if(u2Index != imported_package->ef->classes[u1Index].export_methods_count) {
                method_token = imported_package->ef->classes[u1Index].methods[u2Index].token;
                break;
            } else {
                return NULL;
            }
        }


    new = (constant_pool_entry_info*)malloc(sizeof(constant_pool_entry_info));
    if(new == NULL) {
        perror("add_external_method_to_constant_pool");
        return NULL;
    }

    is_static =imported_package->ef->classes[u1Index].methods[u2Index].access_flags & EF_ACC_STATIC;
    if(is_static)
        new->flags = CONSTANT_POOL_STATICMETHODREF|CONSTANT_POOL_IS_EXTERNAL;
    else
        new->flags = CONSTANT_POOL_VIRTUALMETHODREF|CONSTANT_POOL_IS_EXTERNAL;

    new->my_index = 0;
    new->count = 0;

    new_signature = make_signature_descriptor_from_imported_package(acf, imported_package, imported_package->ef->classes[u1Index].methods[u2Index].descriptor_index, export_files, nb_export_files);
    if(new_signature == NULL)
        return NULL;
    is_unique = is_type_descriptor_not_unique(acf, new_signature);

    if(is_unique == acf->signature_pool_count) {
        type_descriptor_info** tmp2 = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp2 == NULL) {
            perror("add_external_method_to_constant_pool");
            return NULL;
        }
        acf->signature_pool = tmp2;

        acf->signature_pool[acf->signature_pool_count] = new_signature;
        ++acf->signature_pool_count;
        new->type = new_signature;
    } else {
        free(new_signature->types);
        free(new_signature);
        new->type = acf->signature_pool[is_unique];
    }

    new->external_package = imported_package;
    new->external_class_token = class_token;
    new->method_token = method_token;

    tmp1 = (constant_pool_entry_info**)realloc(acf->constant_pool, sizeof(constant_pool_entry_info*) * (acf->constant_pool_count + 1));
    if(tmp1 == NULL) {
        perror("add_external_method_to_constant_pool");
        return NULL;
    }
    acf->constant_pool = tmp1;
    acf->constant_pool[acf->constant_pool_count] = new;
    ++acf->constant_pool_count;

    return new;

}


static int init_SyncApplet_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, SyncApplet_info* SyncApplet, char is_using_int) {

    // Get the token of the SyncApplet class in the synclib package
    if(get_class_token_from_export_file(synclib->ef, "synclib.SyncApplet", &(SyncApplet->class_token)) == -1)
        return -1;

    // Get the constant pool entry for the syncAID field from the Input class.
    if((SyncApplet->syncAID_ref = add_field_from_imported_package_to_constant_pool(acf, synclib, SyncApplet->class_token, "syncAID", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the token of the SyncApplet constructor method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "<init>", "()V", &(SyncApplet->SyncApplet_method_token)) == -1)
        return -1;

    // Get the token of the syncRegister() method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "syncRegister", "()V", &(SyncApplet->syncRegister_1_method_token)) == -1)
        return -1;

    // Get the token of the syncRegister(byte[]], short, byte) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "syncRegister", "([BSB)V", &(SyncApplet->syncRegister_2_method_token)) == -1)
        return -1;

    // Get the token of the uninstall() method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "uninstall", "()V", &(SyncApplet->uninstall_method_token)) == -1)
        return -1;

    // Get the token of the getId() method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "getId", "()S", &(SyncApplet->getId_method_token)) == -1)
        return -1;

    // Get the token of the setId(short) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "setId", "(S)V", &(SyncApplet->setId_method_token)) == -1)
        return -1;

    // Get the token of the isDuplicated() method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "isDuplicated", "()Z", &(SyncApplet->isDuplicated_method_token)) == -1)
        return -1;

    // Get the token of the getPreviousSynchronizableInstance() method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "getPreviousSynchronizableInstance", "()Lsynclib/Synchronizable;", &(SyncApplet->getPreviousSynchronizableInstance_method_token)) == -1)
        return -1;

    // Get the token of the setPreviousSynchronizableInstance(Synchronizable) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "setPreviousSynchronizableInstance", "(Lsynclib/Synchronizable;)V", &(SyncApplet->setPreviousSynchronizableInstance_method_token)) == -1)
        return -1;

    // Get the token of the resetPath(ResetIdSource) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "resetPath", "(Lsynclib/ResetIdSource;)Z", &(SyncApplet->resetPath_method_token)) == -1)
        return -1;

    // Get the token of the numberPath(Output) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "numberPath", "(Lsynclib/Output;)Z", &(SyncApplet->numberPath_method_token)) == -1)
        return -1;

    // Get the token of the serialize(Output) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "serialize", "(Lsynclib/Output;)V", &(SyncApplet->serialize_method_token)) == -1)
        return -1;

    // Get the token of the merge(Input) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "merge", "(Lsynclib/Input;)V", &(SyncApplet->merge_method_token)) == -1)
        return -1;

    // Get the token of the resetStaticPathAll(ResetIdSource) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "resetStaticPathAll", "(Lsynclib/ResetIdSource;)V", &(SyncApplet->resetStaticPathAll_method_token)) == -1)
        return -1;

    // Get the token of the numberStaticPathAll(Output) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "numberStaticPathAll", "(Lsynclib/Output;)V", &(SyncApplet->numberStaticPathAll_method_token)) == -1)
        return -1;

    // Get the token of the serializeStaticAll(Output) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "serializeStaticAll", "(Lsynclib/Output;)V", &(SyncApplet->serializeStaticAll_method_token)) == -1)
        return -1;

    // Get the token of the mergeStaticAll(Input) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "mergeStaticAll", "(Lsynclib/Input;)V", &(SyncApplet->mergeStaticAll_method_token)) == -1)
        return -1;

    // Get the token of the getClassId(Synchronizable) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "getClassId", "(Lsynclib/Synchronizable;)S", &(SyncApplet->getClassId_method_token)) == -1)
        return -1;

    // Get the token of the getNewInstance(Input, short) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "getNewInstance", "(Lsynclib/Input;S)Lsynclib/Synchronizable;", &(SyncApplet->getNewInstance_method_token)) == -1)
        return -1;

    // Get the token of the getNbStaticBooleanArrays() method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "getNbStaticBooleanArrays", "()S", &(SyncApplet->getNbStaticBooleanArrays_method_token)) == -1)
        return -1;

    // Get the token of the getAnyStaticBooleanArray(short) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "getAnyStaticBooleanArray", "(S)[Z", &(SyncApplet->getAnyStaticBooleanArray_method_token)) == -1)
        return -1;

    // Get the token of the getNbStaticByteArrays() method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "getNbStaticByteArrays", "()S", &(SyncApplet->getNbStaticByteArrays_method_token)) == -1)
        return -1;

    // Get the token of the getAnyStaticByteArray(short) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "getAnyStaticByteArray", "(S)[B", &(SyncApplet->getAnyStaticByteArray_method_token)) == -1)
        return -1;

    // Get the token of the getNbStaticShortArrays() method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "getNbStaticShortArrays", "()S", &(SyncApplet->getNbStaticShortArrays_method_token)) == -1)
        return -1;

    // Get the token of the getAnyStaticShortArray(short) method from the SyncApplet class.
    if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "getAnyStaticShortArray", "(S)[S", &(SyncApplet->getAnyStaticShortArray_method_token)) == -1)
        return -1;

    if(is_using_int) {
        // Get the token of the getNbStaticIntArrays() method from the SyncApplet class.
        if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "getNbStaticIntArrays", "()S", &(SyncApplet->getNbStaticIntArrays_method_token)) == -1)
            return -1;

        // Get the token of the getAnyStaticIntArray(short) method from the SyncApplet class.
        if(get_method_token_from_export_file(synclib->ef, SyncApplet->class_token, "getAnyStaticIntArray", "(S)[I", &(SyncApplet->getAnyStaticIntArray_method_token)) == -1)
            return -1;
    }


    // Get the constant pool entry for the resetPathAll(ResetIdSource, Synchronizable) method from the SyncApplet class.
    if((SyncApplet->resetPathAll_ref = add_external_method_to_constant_pool(acf, synclib, SyncApplet->class_token, "resetPathAll", "(Lsynclib/ResetIdSource;Lsynclib/Synchronizable;)V", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the numberPathAll(Output, Synchronizable) method from the SyncApplet class.
    if((SyncApplet->numberPathAll_ref = add_external_method_to_constant_pool(acf, synclib, SyncApplet->class_token, "numberPathAll", "(Lsynclib/Output;Lsynclib/Synchronizable;)V", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the serializeAll(Output, Synchronizable) method from the SyncApplet class.
    if((SyncApplet->serializeAll_ref = add_external_method_to_constant_pool(acf, synclib, SyncApplet->class_token, "serializeAll", "(Lsynclib/Output;Lsynclib/Synchronizable;)V", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the mergeAll(Input, Synchronizable) method from the SyncApplet class.
    if((SyncApplet->mergeAll_ref = add_external_method_to_constant_pool(acf, synclib, SyncApplet->class_token, "mergeAll", "(Lsynclib/Input;Lsynclib/Synchronizable;)Lsynclib/Synchronizable;", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the syncUninstall() method from the SyncApplet class.
    if((SyncApplet->syncUninstall_ref = add_external_method_to_constant_pool(acf, synclib, SyncApplet->class_token, "syncUninstall", "()V", export_files, nb_export_files)) == NULL)
        return -1;

    return 0;

}


static int get_final_static_field_value_from_export_file(export_file* ef, const char* fully_qualified_class_name, const char* name, u4* value) {

    u1 u1Index = 0;
    u2 name_length = strlen(name);

    u1 class_token = 0;

    if(get_class_token_from_export_file(ef, fully_qualified_class_name, &class_token) == -1)
        return -1;

    for(; u1Index < ef->export_class_count; ++u1Index)
        if(ef->classes[u1Index].token == class_token) {
            u2 u2Index = 0;

            for(; u2Index < ef->classes[u1Index].export_fields_count; ++u2Index)
                if((ef->classes[u1Index].fields[u2Index].access_flags & (EF_ACC_STATIC | EF_ACC_FINAL)) && (ef->constant_pool[ef->classes[u1Index].fields[u2Index].name_index].CONSTANT_Utf8.length == name_length) && (strncmp((char*)ef->constant_pool[ef->classes[u1Index].fields[u2Index].name_index].CONSTANT_Utf8.bytes, name, name_length) == 0)) {
                    *value = ef->constant_pool[ef->classes[u1Index].fields[u2Index].attributes[0].constantvalue_index].CONSTANT_Integer.bytes;
                    return 0;
                }
        }

    return -1;

}


static int init_Input_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, Input_info* Input, char is_using_int) {

    // Get the token of the Input class in the synclib package.
    if(get_class_token_from_export_file(synclib->ef, "synclib.Input", &(Input->class_token)) == -1)
        return -1;

    // Get the constant pool entry for the Input class.
    if((Input->class_ref = get_external_classref_from_constant_pool(acf, synclib , "synclib.Input", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the newlyCreatedInstance field from the Input class.
    if((Input->newlyCreatedInstance_ref = add_field_from_imported_package_to_constant_pool(acf, synclib, Input->class_token, "newlyCreatedInstance", export_files, nb_export_files)) == NULL)
        return -1;


    // Get the constant pool entry for the resetResume() method from the Input class.
    if((Input->resetResume_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "resetResume", "()V", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the read(boolean) method from the Input class.
    if((Input->read_boolean_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "read", "(Z)Z", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the read(byte) method from the Input class.
    if((Input->read_byte_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "read", "(B)B", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the read(short) method from the Input class.
    if((Input->read_short_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "read", "(S)S", export_files, nb_export_files)) == NULL)
        return -1;

    if(is_using_int) {
        // Get the constant pool entry for the read(int) method from the Input class.
        if((Input->read_int_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "read", "(I)I", export_files, nb_export_files)) == NULL)
            return -1;
    }

    // Get the constant pool entry for the checkArray(boolean) method from the Input class.
    if((Input->check_boolean_array_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "checkArray", "([Z)[Z", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the read(boolean[]) method from the Input class.
    if((Input->read_boolean_array_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "read", "([Z)V", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the checkArray(byte[]) method from the Input class.
    if((Input->check_byte_array_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "checkArray", "([B)[B", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the read(byte[]) method from the Input class.
    if((Input->read_byte_array_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "read", "([B)V", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the checkArray(short[]) method from the Input class.
    if((Input->check_short_array_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "checkArray", "([S)[S", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the read(short[]) method from the Input class.
    if((Input->read_short_array_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "read", "([S)V", export_files, nb_export_files)) == NULL)
        return -1;

    if(is_using_int) {
        // Get the constant pool entry for the checkArray(int) method from the Input class.
        if((Input->check_int_array_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "read", "([I)[I", export_files, nb_export_files)) == NULL)
            return -1;

        // Get the constant pool entry for the read(int[]) method from the Input class.
        if((Input->read_int_array_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "read", "([I)V", export_files, nb_export_files)) == NULL)
            return -1;
    }

    // Get the constant pool entry for the readN(short) method from the Input class.
    if((Input->readN_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "readN", "(S)Z", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry for the getReadNValue(short) method from the Input class.
    if((Input->getReadNValue_ref = add_external_method_to_constant_pool(acf, synclib, Input->class_token, "getReadNValue", "(S)B", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the value of the SKIP_INSTANCE final static field of the Input class.
    if(get_final_static_field_value_from_export_file(synclib->ef, "synclib.Input", "SKIP_INSTANCE", &(Input->skip_instance_value)) == -1)
        return -1;

    // Get the value of the NEW_INSTANCE final static field of the Input class.
    if(get_final_static_field_value_from_export_file(synclib->ef, "synclib.Input", "NEW_INSTANCE", &(Input->new_instance_value)) == -1)
        return -1;

    return 0;

}


static int init_Output_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, Output_info* Output, char is_using_int) {

    // Get the token of the class Output in the package synclib.
    if(get_class_token_from_export_file(synclib->ef, "synclib.Output", &(Output->class_token)) == -1)
        return -1;

    // Get the constant pool entry of the class Output.
    if((Output->class_ref = get_external_classref_from_constant_pool(acf, synclib , "synclib.Output", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry of the method resetResume() of the class Output.
    if((Output->resetResume_ref = add_external_method_to_constant_pool(acf, synclib, Output->class_token, "resetResume", "()V", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry of the method getNextId() of the class Output.
    if((Output->getNextId_ref = add_external_method_to_constant_pool(acf, synclib, Output->class_token, "getNextId", "()S", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry of the method skipInstance() of the class Output.
    if((Output->skipInstance_ref = add_external_method_to_constant_pool(acf, synclib, Output->class_token, "skipInstance", "()V", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry of the method write(boolean) of the class Output.
    if((Output->write_boolean_ref = add_external_method_to_constant_pool(acf, synclib, Output->class_token, "write", "(Z)V", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry of the method write(byte) of the class Output.
    if((Output->write_byte_ref = add_external_method_to_constant_pool(acf, synclib, Output->class_token, "write", "(B)V", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry of the method write(short) of the class Output.
    if((Output->write_short_ref = add_external_method_to_constant_pool(acf, synclib, Output->class_token, "write", "(S)V", export_files, nb_export_files)) == NULL)
        return -1;

    if(is_using_int)
        // Get the constant pool entry of the method write(int) of the class Output.
        if((Output->write_int_ref = add_external_method_to_constant_pool(acf, synclib, Output->class_token, "write", "(I)V", export_files, nb_export_files)) == NULL)
            return -1;

    // Get the constant pool entry of the method write(boolean[]) of the class Output.
    if((Output->write_boolean_array_ref = add_external_method_to_constant_pool(acf, synclib, Output->class_token, "write", "([ZS)V", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry of the method write(byte[]) of the class Output.
    if((Output->write_byte_array_ref = add_external_method_to_constant_pool(acf, synclib, Output->class_token, "write", "([BS)V", export_files, nb_export_files)) == NULL)
        return -1;

    // Get the constant pool entry of the method write(short[]) of the class Output.
    if((Output->write_short_array_ref = add_external_method_to_constant_pool(acf, synclib, Output->class_token, "write", "([SS)V", export_files, nb_export_files)) == NULL)
        return -1;

    if(is_using_int)
        // Get the constant pool entry of the method write(int[]) of the class Output.
        if((Output->write_int_array_ref = add_external_method_to_constant_pool(acf, synclib, Output->class_token, "write", "([IS)V", export_files, nb_export_files)) == NULL)
            return -1;

    return 0;

}


static int init_Synchronizable_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, Synchronizable_info* Synchronizable) {

    if(get_class_token_from_export_file(synclib->ef, "synclib.Synchronizable", &(Synchronizable->interface_token)) == -1)
        return -1;

    if((Synchronizable->interface_ref = get_external_classref_from_constant_pool(acf, synclib, "synclib.Synchronizable", export_files, nb_export_files)) == NULL)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, Synchronizable->interface_token, "getId", "()S", &(Synchronizable->getId_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, Synchronizable->interface_token, "setId", "(S)V", &(Synchronizable->setId_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, Synchronizable->interface_token, "isDuplicated", "()Z", &(Synchronizable->isDuplicated_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, Synchronizable->interface_token, "getPreviousSynchronizableInstance", "()Lsynclib/Synchronizable;", &(Synchronizable->getPreviousSynchronizableInstance_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, Synchronizable->interface_token, "setPreviousSynchronizableInstance", "(Lsynclib/Synchronizable;)V", &(Synchronizable->setPreviousSynchronizableInstance_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, Synchronizable->interface_token, "resetPath", "(Lsynclib/ResetIdSource;)Z", &(Synchronizable->resetPath_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, Synchronizable->interface_token, "numberPath", "(Lsynclib/Output;)Z", &(Synchronizable->numberPath_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, Synchronizable->interface_token, "serialize", "(Lsynclib/Output;)V", &(Synchronizable->serialize_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, Synchronizable->interface_token, "merge", "(Lsynclib/Input;)V", &(Synchronizable->merge_method_token)) == -1)
        return -1;

    return 0;

}

static int init_SynchronizableArrayFields_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, SynchronizableArrayFields_info* SynchronizableArrayFields, char is_using_int) {

    if(get_class_token_from_export_file(synclib->ef, "synclib.SynchronizableArrayFields", &(SynchronizableArrayFields->interface_token)) == -1)
        return -1;

    if((SynchronizableArrayFields->interface_ref = get_external_classref_from_constant_pool(acf, synclib, "synclib.SynchronizableArrayFields", export_files, nb_export_files)) == NULL)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "getId", "()S", &(SynchronizableArrayFields->getId_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "setId", "(S)V", &(SynchronizableArrayFields->setId_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "isDuplicated", "()Z", &(SynchronizableArrayFields->isDuplicated_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "getPreviousSynchronizableInstance", "()Lsynclib/Synchronizable;", &(SynchronizableArrayFields->getPreviousSynchronizableInstance_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "setPreviousSynchronizableInstance", "(Lsynclib/Synchronizable;)V", &(SynchronizableArrayFields->setPreviousSynchronizableInstance_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "resetPath", "(Lsynclib/ResetIdSource;)Z", &(SynchronizableArrayFields->resetPath_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "numberPath", "(Lsynclib/Output;)Z", &(SynchronizableArrayFields->numberPath_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "serialize", "(Lsynclib/Output;)V", &(SynchronizableArrayFields->serialize_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "merge", "(Lsynclib/Input;)V", &(SynchronizableArrayFields->merge_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "getNext", "()Lsynclib/SynchronizableArrayFields;", &(SynchronizableArrayFields->getNext_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "setNext", "(Lsynclib/SynchronizableArrayFields;)V", &(SynchronizableArrayFields->setNext_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "getNbBooleanArrays", "()S", &(SynchronizableArrayFields->getNbBooleanArrays_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "getBooleanArray", "(S)[Z", &(SynchronizableArrayFields->getBooleanArray_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "getNbByteArrays", "()S", &(SynchronizableArrayFields->getNbByteArrays_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "getByteArray", "(S)[B", &(SynchronizableArrayFields->getByteArray_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "getNbShortArrays", "()S", &(SynchronizableArrayFields->getNbShortArrays_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "getShortArray", "(S)[S", &(SynchronizableArrayFields->getShortArray_method_token)) == -1)
        return -1;

    if(is_using_int) {
        if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "getNbIntArrays", "()S", &(SynchronizableArrayFields->getNbIntArrays_method_token)) == -1)
            return -1;

        if(get_method_token_from_export_file(synclib->ef, SynchronizableArrayFields->interface_token, "getIntArray", "(S)[I", &(SynchronizableArrayFields->getIntArray_method_token)) == -1)
            return -1;
    }

    return 0;

}

static int init_SynchronizableInstanceFields_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, SynchronizableInstanceFields_info* SynchronizableInstanceFields) {

    if(get_class_token_from_export_file(synclib->ef, "synclib.SynchronizableInstanceFields", &(SynchronizableInstanceFields->interface_token)) == -1)
        return -1;

    if((SynchronizableInstanceFields->interface_ref = get_external_classref_from_constant_pool(acf, synclib, "synclib.SynchronizableInstanceFields", export_files, nb_export_files)) == NULL)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "getId", "()S", &(SynchronizableInstanceFields->getId_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "setId", "(S)V", &(SynchronizableInstanceFields->setId_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "isDuplicated", "()Z", &(SynchronizableInstanceFields->isDuplicated_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "getPreviousSynchronizableInstance", "()Lsynclib/Synchronizable;", &(SynchronizableInstanceFields->getPreviousSynchronizableInstance_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "setPreviousSynchronizableInstance", "(Lsynclib/Synchronizable;)V", &(SynchronizableInstanceFields->setPreviousSynchronizableInstance_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "resetPath", "(Lsynclib/ResetIdSource;)Z", &(SynchronizableInstanceFields->resetPath_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "numberPath", "(Lsynclib/Output;)Z", &(SynchronizableInstanceFields->numberPath_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "serialize", "(Lsynclib/Output;)V", &(SynchronizableInstanceFields->serialize_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "merge", "(Lsynclib/Input;)V", &(SynchronizableInstanceFields->merge_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "getCurrentSynchronizableInstance", "()Lsynclib/Synchronizable;", &(SynchronizableInstanceFields->getCurrentSynchronizableInstance_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "setCurrentSynchronizableInstance", "(Lsynclib/Synchronizable;)V", &(SynchronizableInstanceFields->setCurrentSynchronizableInstance_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "hasCurrentSynchronizableInstance", "()Z", &(SynchronizableInstanceFields->hasCurrentSynchronizableInstance_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "selectNextSynchronizableInstance", "()V", &(SynchronizableInstanceFields->selectNextSynchronizableInstance_method_token)) == -1)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, SynchronizableInstanceFields->interface_token, "resetNextSynchronizableInstanceSelect", "()V", &(SynchronizableInstanceFields->resetNextSynchronizableInstanceSelect_method_token)) == -1)
        return -1;

    return 0;

}

static int init_ResetIdSource_info(analyzed_cap_file* acf, imported_package_info* synclib, export_file** export_files, int nb_export_files, ResetIdSource_info* ResetIdSource) {

    if(get_class_token_from_export_file(synclib->ef, "synclib.ResetIdSource", &(ResetIdSource->interface_token)) == -1)
        return -1;

    if((ResetIdSource->interface_ref = get_external_classref_from_constant_pool(acf, synclib, "synclib.ResetIdSource", export_files, nb_export_files)) == NULL)
        return -1;

    if(get_method_token_from_export_file(synclib->ef, ResetIdSource->interface_token, "getResetId", "()S", &(ResetIdSource->getResetId_method_token)) == -1)
        return -1;

    return 0;

}


static type_descriptor_info* create_getId_sig(analyzed_cap_file* acf) {

    u2 is_unique = acf->signature_pool_count;
    type_descriptor_info* sig = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_getId_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 1;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_getId_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_SHORT;

    is_unique = is_type_descriptor_not_unique(acf, sig);
    if(is_unique == acf->signature_pool_count) {
        type_descriptor_info** tmp = NULL;

        tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp == NULL) {
            perror("create_getId_sig");
            return NULL;
        }
        acf->signature_pool = tmp;

        acf->signature_pool[acf->signature_pool_count] = sig;
        ++acf->signature_pool_count;
    } else {
        free(sig->types);
        free(sig);
        sig = acf->signature_pool[is_unique];
    }

    return sig;

}

static type_descriptor_info* create_setId_sig(analyzed_cap_file* acf) {

    u2 is_unique = acf->signature_pool_count;
    type_descriptor_info* sig = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_setId_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_setId_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_SHORT;

    sig->types[1].type = TYPE_DESCRIPTOR_VOID;

    is_unique = is_type_descriptor_not_unique(acf, sig);
    if(is_unique == acf->signature_pool_count) {
        type_descriptor_info** tmp = NULL;

        tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp == NULL) {
            perror("create_setId_sig");
            return NULL;
        }
        acf->signature_pool = tmp;

        acf->signature_pool[acf->signature_pool_count] = sig;
        ++acf->signature_pool_count;
    } else {
        free(sig->types);
        free(sig);
        sig = acf->signature_pool[is_unique];
    }

    return sig;

}

static type_descriptor_info* create_isDuplicated_sig(analyzed_cap_file* acf) {

    u2 is_unique = acf->signature_pool_count;
    type_descriptor_info* sig = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_isDuplicated_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 1;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_idDuplicated_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_BOOLEAN;

    is_unique = is_type_descriptor_not_unique(acf, sig);
    if(is_unique == acf->signature_pool_count) {
        type_descriptor_info** tmp = NULL;

        tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp == NULL) {
            perror("create_isDuplicated_sig");
            return NULL;
        }
        acf->signature_pool = tmp;

        acf->signature_pool[acf->signature_pool_count] = sig;
        ++acf->signature_pool_count;
    } else {
        free(sig->types);
        free(sig);
        sig = acf->signature_pool[is_unique];
    }

    return sig;

}


static type_descriptor_info* create_setPreviousSynchronizableInstance_sig(analyzed_cap_file* acf, synclib_info* synclib) {

    type_descriptor_info* sig = NULL;
    type_descriptor_info** tmp = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_setPreviousSynchronizableInstance_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_setPreviousSynchronizableInstance_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_REF;
    sig->types[0].ref = synclib->Synchronizable.interface_ref;

    sig->types[1].type = TYPE_DESCRIPTOR_VOID;

    tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
    if(tmp == NULL) {
        perror("create_setPreviousSynchronizableInstance_sig");
        return NULL;
    }
    acf->signature_pool = tmp;

    acf->signature_pool[acf->signature_pool_count] = sig;
    ++acf->signature_pool_count;

    return sig;

}

static type_descriptor_info* create_resetPath_sig(analyzed_cap_file* acf, synclib_info* synclib) {

    type_descriptor_info* sig = NULL;
    type_descriptor_info** tmp = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_resetPath_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_resetPath_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_REF;
    sig->types[0].ref = synclib->ResetIdSource.interface_ref;

    sig->types[1].type = TYPE_DESCRIPTOR_BOOLEAN;

    tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
    if(tmp == NULL) {
        perror("create_resetPath_sig");
        return NULL;
    }
    acf->signature_pool = tmp;

    acf->signature_pool[acf->signature_pool_count] = sig;
    ++acf->signature_pool_count;

    return sig;

}


static type_descriptor_info* create_numberPath_sig(analyzed_cap_file* acf, synclib_info* synclib) {

    type_descriptor_info* sig = NULL;
    type_descriptor_info** tmp = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_numberPath_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_numberPath_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_REF;
    sig->types[0].ref = synclib->Output.class_ref;

    sig->types[1].type = TYPE_DESCRIPTOR_BOOLEAN;

    tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
    if(tmp == NULL) {
        perror("create_numberPath_sig");
        return NULL;
    }
    acf->signature_pool = tmp;

    acf->signature_pool[acf->signature_pool_count] = sig;
    ++acf->signature_pool_count;

    return sig;

}


static type_descriptor_info* create_serialize_sig(analyzed_cap_file* acf, synclib_info* synclib) {

    type_descriptor_info* sig = NULL;
    type_descriptor_info** tmp = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_serialize_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_serialize_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_REF;
    sig->types[0].ref = synclib->Output.class_ref;

    sig->types[1].type = TYPE_DESCRIPTOR_VOID;

    tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
    if(tmp == NULL) {
        perror("create_serialize_sig");
        return NULL;
    }
    acf->signature_pool = tmp;

    acf->signature_pool[acf->signature_pool_count] = sig;
    ++acf->signature_pool_count;

    return sig;

}


static type_descriptor_info* create_merge_sig(analyzed_cap_file* acf, synclib_info* synclib) {

    type_descriptor_info* sig = NULL;
    type_descriptor_info** tmp = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_merge_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_merge_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_REF;
    sig->types[0].ref = synclib->Input.class_ref;

    sig->types[1].type = TYPE_DESCRIPTOR_VOID;

    tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
    if(tmp == NULL) {
        perror("create_merge_sig");
        return NULL;
    }
    acf->signature_pool = tmp;

    acf->signature_pool[acf->signature_pool_count] = sig;
    ++acf->signature_pool_count;

    return sig;

}

static type_descriptor_info* create_resetStaticPathAll_sig(analyzed_cap_file* acf, synclib_info* synclib) {

    type_descriptor_info* sig = NULL;
    type_descriptor_info** tmp = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_resetStaticPathAll_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_resetStaticPathAll_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_REF;
    sig->types[0].ref = synclib->ResetIdSource.interface_ref;

    sig->types[1].type = TYPE_DESCRIPTOR_VOID;

    tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
    if(tmp == NULL) {
        perror("create_resetStaticPathAll_sig");
        return NULL;
    }
    acf->signature_pool = tmp;

    acf->signature_pool[acf->signature_pool_count] = sig;
    ++acf->signature_pool_count;

    return sig;

}

static type_descriptor_info* create_getClassId_sig(analyzed_cap_file* acf, synclib_info* synclib) {

    type_descriptor_info* sig = NULL;
    type_descriptor_info** tmp = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_getClassId_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_getClassId_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_REF;
    sig->types[0].ref = synclib->Synchronizable.interface_ref;

    sig->types[1].type = TYPE_DESCRIPTOR_SHORT;

    tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
    if(tmp == NULL) {
        perror("create_getClassId_sig");
        return NULL;
    }
    acf->signature_pool = tmp;

    acf->signature_pool[acf->signature_pool_count] = sig;
    ++acf->signature_pool_count;

    return sig;

}

static type_descriptor_info* create_getNewInstance_sig(analyzed_cap_file* acf, synclib_info* synclib) {

    type_descriptor_info* sig = NULL;
    type_descriptor_info** tmp = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_getNewInstance_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 3;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_getNewInstance_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_REF;
    sig->types[0].ref = synclib->Input.class_ref;

    sig->types[1].type = TYPE_DESCRIPTOR_SHORT;

    sig->types[2].type = TYPE_DESCRIPTOR_REF;
    sig->types[2].ref = synclib->Synchronizable.interface_ref;

    tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
    if(tmp == NULL) {
        perror("create_getNewInstance_sig");
        return NULL;
    }
    acf->signature_pool = tmp;

    acf->signature_pool[acf->signature_pool_count] = sig;
    ++acf->signature_pool_count;

    return sig;

}

static type_descriptor_info* create_getAnyStaticBooleanArray_sig(analyzed_cap_file* acf) {

    u2 is_unique = acf->signature_pool_count;
    type_descriptor_info* sig = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_getAnyStaticBooleanArray_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_getAnyStaticBooleanArray_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_SHORT;

    sig->types[1].type = TYPE_DESCRIPTOR_BOOLEAN|TYPE_DESCRIPTOR_ARRAY;

    is_unique = is_type_descriptor_not_unique(acf, sig);
    if(is_unique == acf->signature_pool_count) {
        type_descriptor_info** tmp = NULL;

        tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp == NULL) {
            perror("create_getAnyStaticBooleanArray_sig");
            return NULL;
        }
        acf->signature_pool = tmp;

        acf->signature_pool[acf->signature_pool_count] = sig;
        ++acf->signature_pool_count;
    } else {
        free(sig->types);
        free(sig);
        sig = acf->signature_pool[is_unique];
    }

    return sig;

}

static type_descriptor_info* create_getAnyStaticByteArray_sig(analyzed_cap_file* acf) {

    u2 is_unique = acf->signature_pool_count;
    type_descriptor_info* sig = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_getAnyStaticByteArray_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_getAnyStaticByteArray_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_SHORT;

    sig->types[1].type = TYPE_DESCRIPTOR_BYTE|TYPE_DESCRIPTOR_ARRAY;

    is_unique = is_type_descriptor_not_unique(acf, sig);
    if(is_unique == acf->signature_pool_count) {
        type_descriptor_info** tmp = NULL;

        tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp == NULL) {
            perror("create_getAnyStaticByteArray_sig");
            return NULL;
        }
        acf->signature_pool = tmp;

        acf->signature_pool[acf->signature_pool_count] = sig;
        ++acf->signature_pool_count;
    } else {
        free(sig->types);
        free(sig);
        sig = acf->signature_pool[is_unique];
    }

    return sig;

}

static type_descriptor_info* create_getAnyStaticShortArray_sig(analyzed_cap_file* acf) {

    u2 is_unique = acf->signature_pool_count;
    type_descriptor_info* sig = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_getNbStaticBooleanArrays_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_getAnyStaticShortArray_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_SHORT;

    sig->types[1].type = TYPE_DESCRIPTOR_SHORT|TYPE_DESCRIPTOR_ARRAY;

    is_unique = is_type_descriptor_not_unique(acf, sig);
    if(is_unique == acf->signature_pool_count) {
        type_descriptor_info** tmp = NULL;

        tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp == NULL) {
            perror("create_getAnyStaticShortArray_sig");
            return NULL;
        }
        acf->signature_pool = tmp;

        acf->signature_pool[acf->signature_pool_count] = sig;
        ++acf->signature_pool_count;
    } else {
        free(sig->types);
        free(sig);
        sig = acf->signature_pool[is_unique];
    }

    return sig;

}

static type_descriptor_info* create_getAnyStaticIntArray_sig(analyzed_cap_file* acf) {

    u2 is_unique = acf->signature_pool_count;
    type_descriptor_info* sig = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_getAnyStaticIntArray_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_getAnyStaticIntArray_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_SHORT;

    sig->types[1].type = TYPE_DESCRIPTOR_INT|TYPE_DESCRIPTOR_ARRAY;

    is_unique = is_type_descriptor_not_unique(acf, sig);
    if(is_unique == acf->signature_pool_count) {
        type_descriptor_info** tmp = NULL;

        tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp == NULL) {
            perror("create_getAnyStaticIntArray_sig");
            return NULL;
        }
        acf->signature_pool = tmp;

        acf->signature_pool[acf->signature_pool_count] = sig;
        ++acf->signature_pool_count;
    } else {
        free(sig->types);
        free(sig);
        sig = acf->signature_pool[is_unique];
    }

    return sig;

}

static type_descriptor_info* create_getNext_sig(analyzed_cap_file* acf, synclib_info* synclib) {


    type_descriptor_info* sig = NULL;
    type_descriptor_info** tmp = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_getNext_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 1;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_getNext_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_REF;
    sig->types[0].ref = synclib->SynchronizableArrayFields.interface_ref;

    tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
    if(tmp == NULL) {
        perror("create_getNext_sig");
        return NULL;
    }
    acf->signature_pool = tmp;

    acf->signature_pool[acf->signature_pool_count] = sig;
    ++acf->signature_pool_count;

    return sig;

}

static type_descriptor_info* create_setNext_sig(analyzed_cap_file* acf, synclib_info* synclib) {
    type_descriptor_info* sig = NULL;
    type_descriptor_info** tmp = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_setNext_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 2;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_setNext_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_REF;
    sig->types[0].ref = synclib->SynchronizableArrayFields.interface_ref;

    sig->types[1].type = TYPE_DESCRIPTOR_VOID;

    tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
    if(tmp == NULL) {
        perror("create_setNext_sig");
        return NULL;
    }
    acf->signature_pool = tmp;

    acf->signature_pool[acf->signature_pool_count] = sig;
    ++acf->signature_pool_count;

    return sig;

}

static type_descriptor_info* create_resetNextSynchronizableInstanceSelect_sig(analyzed_cap_file* acf) {

    u2 is_unique = acf->signature_pool_count;
    type_descriptor_info* sig = NULL;

    sig = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(sig == NULL) {
        perror("create_resetNextSynchronizableInstanceSelect_sig");
        return NULL;
    }

    sig->count = 0;
    sig->types_count = 1;
    sig->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * sig->types_count);
    if(sig->types == NULL) {
        perror("create_resetNextSynchronizableInstanceSelect_sig");
        return NULL;
    }

    memset(sig->types, 0, sizeof(one_type_descriptor_info) * sig->types_count);

    sig->types[0].type = TYPE_DESCRIPTOR_VOID;

    is_unique = is_type_descriptor_not_unique(acf, sig);
    if(is_unique == acf->signature_pool_count) {
        type_descriptor_info** tmp = NULL;

        tmp = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp == NULL) {
            perror("create_resetNextSynchronizableInstanceSelect_sig");
            return NULL;
        }
        acf->signature_pool = tmp;

        acf->signature_pool[acf->signature_pool_count] = sig;
        ++acf->signature_pool_count;
    } else {
        free(sig->types);
        free(sig);
        sig = acf->signature_pool[is_unique];
    }

    return sig;

}
