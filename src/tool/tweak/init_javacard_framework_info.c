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

#include "javacard_framework_info.h"
#include "generic_tweak_functions.h"

static int get_package_token_from_analyzed_cap_file(analyzed_cap_file* acf, const u1* aid, u1 aid_length, u1* token);
static imported_package_info* get_imported_package_from_package_token(analyzed_cap_file* acf, u1 package_token);
static constant_pool_entry_info* get_external_static_void_method_with_one_short_arg_ref(analyzed_cap_file* acf, imported_package_info* imported_package, u1 class_token, u1 method_token);
static constant_pool_entry_info* add_equals_ref(analyzed_cap_file* acf, u1 aid_class_token, constant_pool_entry_info* aid_ref, u1 equals_method_token);
static constant_pool_entry_info* add_getReason_ref(analyzed_cap_file* acf, u1 UserException_class_token, constant_pool_entry_info* UserException_ref, u1 getReason_method_token);

const u1 javacard_framework_package_aid[] = {0xA0,0x00,0x00,0x00,0x62,0x01,0x01};
const u1 javacard_framework_package_aid_length = 7;

int init_javacard_framework_info(analyzed_cap_file* acf, export_file** export_files, int nb_export_files, javacard_framework_info* javacard_framework) {

    javacard_framework->Applet.class_token = 3;
    javacard_framework->Applet.Applet_method_token = 0;
    javacard_framework->Applet.register_1_method_token = 1;   /* No arg */
    javacard_framework->Applet.register_2_method_token = 2;   /* With args */
    javacard_framework->Applet.process_method_token = 7;
    javacard_framework->Applet.select_method_token = 6;
    javacard_framework->Applet.getShareableInterfaceObject_method_token = 5;

    javacard_framework->AID.class_token = 6;
    javacard_framework->AID.equals_method_token = 0;

    javacard_framework->Shareable.interface_token = 2;

    javacard_framework->ISOException.class_token = 7;
    javacard_framework->ISOException.throwIt_method_token = 1;

    javacard_framework->ISO7816.sw_unknown_value = 0x6F00;

    javacard_framework->AppletEvent.interface_token = 18;
    javacard_framework->AppletEvent.uninstall_method_token = 0;

    javacard_framework->UserException.class_token = 15;
    javacard_framework->UserException.getReason_method_token = 1;


    if(get_package_token_from_analyzed_cap_file(acf, javacard_framework_package_aid, javacard_framework_package_aid_length, &(javacard_framework->package_token)) == -1)
        return -1;

    if((javacard_framework->imported_package = get_imported_package_from_package_token(acf, javacard_framework->package_token)) == NULL)
        return -1;

    if((javacard_framework->ISOException.throwIt_ref = get_external_static_void_method_with_one_short_arg_ref(acf, javacard_framework->imported_package, javacard_framework->ISOException.class_token, javacard_framework->ISOException.throwIt_method_token)) == NULL)
        return -1;

    if((javacard_framework->AppletEvent.interface_ref = get_external_classref_from_constant_pool(acf, javacard_framework->imported_package , "javacard.framework.AppletEvent", export_files, nb_export_files)) == NULL)
        return -1;

    if((javacard_framework->AID.class_ref = get_external_classref_from_constant_pool(acf, javacard_framework->imported_package, "javacard.framework.AID", export_files, nb_export_files)) == NULL)
        return -1;

    if((javacard_framework->AID.equals_ref = add_equals_ref(acf, javacard_framework->AID.class_token, javacard_framework->AID.class_ref, javacard_framework->AID.equals_method_token)) == NULL)
        return -1;

    if((javacard_framework->UserException.class_ref = get_external_classref_from_constant_pool(acf, javacard_framework->imported_package, "javacard.framework.UserException", export_files, nb_export_files)) == NULL)
        return -1;

    if((javacard_framework->UserException.getReason_ref = add_getReason_ref(acf, javacard_framework->UserException.class_token, javacard_framework->UserException.class_ref, javacard_framework->UserException.getReason_method_token)) == NULL)
        return -1;

    return 0;

}


static int get_package_token_from_analyzed_cap_file(analyzed_cap_file* acf, const u1* aid, u1 aid_length, u1* token) {

    u1 u1Index1 = 0;

    for(; u1Index1 < acf->imported_packages_count; ++u1Index1)
        if(acf->imported_packages[u1Index1]->aid_length == aid_length) {
            u1 u1Index2 = 0;

            for(; u1Index2 < aid_length; ++u1Index2)
                if(aid[u1Index2] != acf->imported_packages[u1Index1]->aid[u1Index2])
                    break;

            if(u1Index2 == aid_length) {
                *token = acf->imported_packages[u1Index1]->my_index;
                return 0;
            }
        }

    fprintf(stderr, "Could not find the package token from its aid\n");
    return -1;

}


static imported_package_info* get_imported_package_from_package_token(analyzed_cap_file* acf, u1 package_token) {

    u1 u1Index = 0;

    for(; u1Index < acf->imported_packages_count; ++u1Index)
        if(acf->imported_packages[u1Index]->my_index == package_token)
            return acf->imported_packages[u1Index];

    return NULL;

}


static constant_pool_entry_info* get_external_static_void_method_with_one_short_arg_ref(analyzed_cap_file* acf, imported_package_info* imported_package, u1 class_token, u1 method_token) {

    u2 u2Index = 0;
    u2 type_index = acf->signature_pool_count;
    constant_pool_entry_info** tmp = NULL;
    constant_pool_entry_info* new_entry = NULL;
    type_descriptor_info* signature = NULL;

    for(; u2Index < acf->constant_pool_count; ++u2Index)
        if((acf->constant_pool[u2Index]->flags & CONSTANT_POOL_STATICMETHODREF) && (acf->constant_pool[u2Index]->flags & CONSTANT_POOL_IS_EXTERNAL) && (acf->constant_pool[u2Index]->external_package->my_index == imported_package->my_index) && (acf->constant_pool[u2Index]->external_class_token == class_token) && (acf->constant_pool[u2Index]->method_token == method_token))
            return acf->constant_pool[u2Index];

    tmp = (constant_pool_entry_info**)realloc(acf->constant_pool, sizeof(constant_pool_entry_info*) * (acf->constant_pool_count + 1));
    if(tmp == NULL) {
        perror("get_external_static_void_method_with_one_short_arg_ref");
        return NULL;
    }
    acf->constant_pool = tmp;

    new_entry = (constant_pool_entry_info*)malloc(sizeof(constant_pool_entry_info));
    if(new_entry == NULL) {
        perror("get_external_static_void_method_with_one_short_arg_ref");
        return NULL;
    }
    acf->constant_pool[acf->constant_pool_count] = new_entry;
    ++acf->constant_pool_count;

    new_entry->flags = CONSTANT_POOL_STATICMETHODREF|CONSTANT_POOL_IS_EXTERNAL;
    new_entry->my_index = 0;
    new_entry->count = 0;

    signature = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(signature == NULL) {
        perror("get_external_static_void_method_with_one_short_arg_ref");
        return NULL;
    }

    signature->count = 0;
    signature->offset = 0;
    signature->types_count = 2;

    signature->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * 2);
    if(signature->types == NULL) {
        perror("get_external_static_void_method_with_one_short_arg_ref");
        return NULL;
    }

    memset(signature->types, 0, sizeof(one_type_descriptor_info) * 2);

    signature->types[0].type = TYPE_DESCRIPTOR_SHORT;
    signature->types[1].type = TYPE_DESCRIPTOR_VOID;

    if((type_index = is_type_descriptor_not_unique(acf, signature)) == acf->signature_pool_count) {
        type_descriptor_info** tmp2 = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp2 == NULL) {
            perror("get_external_static_void_method_with_one_short_arg_ref");
            return NULL;
        }
        acf->signature_pool = tmp2;

        acf->signature_pool[acf->signature_pool_count] = signature;
        ++acf->signature_pool_count;
    } else {
        free(signature->types);
        free(signature);
        signature = acf->signature_pool[type_index];
    }

    new_entry->type = signature;
    new_entry->external_package = imported_package;
    new_entry->external_class_token = class_token;
    new_entry->method_token = method_token;

    return new_entry;
}


static constant_pool_entry_info* add_equals_ref(analyzed_cap_file* acf, u1 aid_class_token, constant_pool_entry_info* aid_ref, u1 equals_method_token) {

    u2 type_index = acf->signature_pool_count;
    constant_pool_entry_info** tmp = NULL;
    type_descriptor_info* type = NULL;
    constant_pool_entry_info* entry = NULL;

    u2 u2Index = 0;

    for(; u2Index < acf->constant_pool_count; ++u2Index)
        if((acf->constant_pool[u2Index]->flags & CONSTANT_POOL_VIRTUALMETHODREF) && (acf->constant_pool[u2Index]->flags & CONSTANT_POOL_IS_EXTERNAL) && (acf->constant_pool[u2Index]->external_package->my_index == aid_ref->external_package->my_index) && (acf->constant_pool[u2Index]->external_class_token == aid_class_token) && (acf->constant_pool[u2Index]->method_token == equals_method_token))
            return acf->constant_pool[u2Index];

    entry = (constant_pool_entry_info*)malloc(sizeof(constant_pool_entry_info));
    if(entry == NULL) {
        perror("add_equals_ref");
        return NULL;
    }

    memset(entry, 0, sizeof(constant_pool_entry_info));
    entry->flags = CONSTANT_POOL_VIRTUALMETHODREF|CONSTANT_POOL_IS_EXTERNAL;
    entry->my_index = acf->constant_pool_count;
    entry->external_package = aid_ref->external_package;
    entry->external_class_token = aid_class_token;
    entry->method_token = equals_method_token;

    type = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(type == NULL) {
        perror("add_equals_ref");
        return NULL;
    }

    memset(type, 0, sizeof(type_descriptor_info));
    type->types_count = 2;
    type->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * type->types_count);
    if(type->types == NULL) {
        perror("add_equals_ref");
        return NULL;
    }

    memset(type->types, 0, sizeof(one_type_descriptor_info) * type->types_count);
    type->types[0].type = TYPE_DESCRIPTOR_REF;
    type->types[0].ref = aid_ref;
    type->types[1].type = TYPE_DESCRIPTOR_BOOLEAN;

    if((type_index = is_type_descriptor_not_unique(acf, type)) == acf->signature_pool_count) {
        type_descriptor_info** tmp2 = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp2 == NULL) {
            perror("add_equals_ref");
            return NULL;
        }
        acf->signature_pool = tmp2;

        acf->signature_pool[acf->signature_pool_count] = type;
        ++acf->signature_pool_count;
    } else {
        free(type->types);
        free(type);
        type = acf->signature_pool[type_index];
    }

    entry->type = type;

    tmp = (constant_pool_entry_info**)realloc(acf->constant_pool, sizeof(constant_pool_entry_info*) * (acf->constant_pool_count + 1));
    if(tmp == NULL) {
        perror("add_equals_ref");
        return NULL;
    }
    acf->constant_pool = tmp;

    acf->constant_pool[acf->constant_pool_count] = entry;
    ++acf->constant_pool_count;

    return entry;

}


static constant_pool_entry_info* add_getReason_ref(analyzed_cap_file* acf, u1 UserException_class_token, constant_pool_entry_info* UserException_ref, u1 getReason_method_token) {

    u2 type_index = acf->signature_pool_count;
    constant_pool_entry_info** tmp = NULL;
    type_descriptor_info* type = NULL;
    constant_pool_entry_info* entry = NULL;

    u2 u2Index = 0;

    for(; u2Index < acf->constant_pool_count; ++u2Index)
        if((acf->constant_pool[u2Index]->flags & CONSTANT_POOL_VIRTUALMETHODREF) && (acf->constant_pool[u2Index]->flags & CONSTANT_POOL_IS_EXTERNAL) && (acf->constant_pool[u2Index]->external_package->my_index == UserException_ref->external_package->my_index) && (acf->constant_pool[u2Index]->external_class_token == UserException_class_token) && (acf->constant_pool[u2Index]->method_token == getReason_method_token))
            return acf->constant_pool[u2Index];

    entry = (constant_pool_entry_info*)malloc(sizeof(constant_pool_entry_info));
    if(entry == NULL) {
        perror("add_getReason_ref");
        return NULL;
    }

    memset(entry, 0, sizeof(constant_pool_entry_info));
    entry->flags = CONSTANT_POOL_VIRTUALMETHODREF|CONSTANT_POOL_IS_EXTERNAL;
    entry->my_index = acf->constant_pool_count;
    entry->external_package = UserException_ref->external_package;
    entry->external_class_token = UserException_class_token;
    entry->method_token = getReason_method_token;

    type = (type_descriptor_info*)malloc(sizeof(type_descriptor_info));
    if(type == NULL) {
        perror("add_getReason_ref");
        return NULL;
    }

    memset(type, 0, sizeof(type_descriptor_info));
    type->types_count = 1;
    type->types = (one_type_descriptor_info*)malloc(sizeof(one_type_descriptor_info) * type->types_count);
    if(type->types == NULL) {
        perror("add_getReason_ref");
        return NULL;
    }

    memset(type->types, 0, sizeof(one_type_descriptor_info) * type->types_count);
    type->types[0].type = TYPE_DESCRIPTOR_SHORT;

    if((type_index = is_type_descriptor_not_unique(acf, type)) == acf->signature_pool_count) {
        type_descriptor_info** tmp2 = (type_descriptor_info**)realloc(acf->signature_pool, sizeof(type_descriptor_info*) * (acf->signature_pool_count + 1));
        if(tmp2 == NULL) {
            perror("add_getReason_ref");
            return NULL;
        }
        acf->signature_pool = tmp2;

        acf->signature_pool[acf->signature_pool_count] = type;
        ++acf->signature_pool_count;
    } else {
        free(type->types);
        free(type);
        type = acf->signature_pool[type_index];
    }

    entry->type = type;

    tmp = (constant_pool_entry_info**)realloc(acf->constant_pool, sizeof(constant_pool_entry_info*) * (acf->constant_pool_count + 1));
    if(tmp == NULL) {
        perror("add_getReason_ref");
        return NULL;
    }
    acf->constant_pool = tmp;

    acf->constant_pool[acf->constant_pool_count] = entry;
    ++acf->constant_pool_count;

    return entry;

}
